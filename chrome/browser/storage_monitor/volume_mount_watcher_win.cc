// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/storage_monitor/volume_mount_watcher_win.h"

#include <windows.h>

#include <dbt.h>
#include <fileapi.h>
#include <winioctl.h>

#include "base/bind_helpers.h"
#include "base/metrics/histogram.h"
#include "base/stl_util.h"
#include "base/string_util.h"
#include "base/stringprintf.h"
#include "base/strings/string_number_conversions.h"
#include "base/task_runner_util.h"
#include "base/time.h"
#include "base/utf_string_conversions.h"
#include "base/win/scoped_handle.h"
#include "chrome/browser/storage_monitor/media_storage_util.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/user_metrics.h"

using content::BrowserThread;

namespace {

const DWORD kMaxPathBufLen = MAX_PATH + 1;

enum DeviceType {
  FLOPPY,
  REMOVABLE,
  FIXED,
};

// Histogram values for recording frequencies of eject attempts and
// outcomes.
enum EjectWinLockOutcomes {
  LOCK_ATTEMPT,
  LOCK_TIMEOUT,
  LOCK_TIMEOUT2,
  NUM_LOCK_OUTCOMES,
};

// We are trying to figure out whether the drive is a fixed volume,
// a removable storage, or a floppy. A "floppy" here means "a volume we
// want to basically ignore because it won't fit media and will spin
// if we touch it to get volume metadata." GetDriveType returns DRIVE_REMOVABLE
// on either floppy or removable volumes. The DRIVE_CDROM type is handled
// as a floppy, as are DRIVE_UNKNOWN and DRIVE_NO_ROOT_DIR, as there are
// reports that some floppy drives don't report as DRIVE_REMOVABLE.
DeviceType GetDeviceType(const string16& mount_point) {
  UINT drive_type = GetDriveType(mount_point.c_str());
  VLOG(1) << "Getting drive type for " << mount_point << " is " << drive_type;
  if (drive_type == DRIVE_FIXED || drive_type == DRIVE_REMOTE ||
      drive_type == DRIVE_RAMDISK) {
    return FIXED;
  }
  if (drive_type != DRIVE_REMOVABLE)
    return FLOPPY;

  string16 device = mount_point;
  if (EndsWith(mount_point, L"\\", false))
    device = mount_point.substr(0, device.length() - 1);
  string16 device_path;
  if (!QueryDosDevice(device.c_str(), WriteInto(&device_path, kMaxPathBufLen),
                      kMaxPathBufLen))
    return REMOVABLE;
  VLOG(1) << "Got device path " << device_path;
  return device_path.find(L"Floppy") == string16::npos ? REMOVABLE : FLOPPY;
}

// Returns 0 if the devicetype is not volume.
uint32 GetVolumeBitMaskFromBroadcastHeader(LPARAM data) {
  DEV_BROADCAST_VOLUME* dev_broadcast_volume =
      reinterpret_cast<DEV_BROADCAST_VOLUME*>(data);
  if (dev_broadcast_volume->dbcv_devicetype == DBT_DEVTYP_VOLUME)
    return dev_broadcast_volume->dbcv_unitmask;
  return 0;
}

// Returns true if |data| represents a logical volume structure.
bool IsLogicalVolumeStructure(LPARAM data) {
  DEV_BROADCAST_HDR* broadcast_hdr =
      reinterpret_cast<DEV_BROADCAST_HDR*>(data);
  return broadcast_hdr != NULL &&
         broadcast_hdr->dbch_devicetype == DBT_DEVTYP_VOLUME;
}

// Gets the total volume of the |mount_point| in bytes.
uint64 GetVolumeSize(const string16& mount_point) {
  ULARGE_INTEGER total;
  if (!GetDiskFreeSpaceExW(mount_point.c_str(), NULL, &total, NULL))
    return 0;
  return total.QuadPart;
}

// Gets mass storage device information given a |device_path|. On success,
// returns true and fills in |info|.
// The following msdn blog entry is helpful for understanding disk volumes
// and how they are treated in Windows:
// http://blogs.msdn.com/b/adioltean/archive/2005/04/16/408947.aspx.
bool GetDeviceDetails(const base::FilePath& device_path,
                      chrome::StorageInfo* info) {
  string16 mount_point;
  if (!GetVolumePathName(device_path.value().c_str(),
                         WriteInto(&mount_point, kMaxPathBufLen),
                         kMaxPathBufLen)) {
    return false;
  }

  // Note: experimentally this code does not spin a floppy drive. It
  // returns a GUID associated with the device, not the volume.
  string16 guid;
  if (info) {
    if (!GetVolumeNameForVolumeMountPoint(mount_point.c_str(),
                                          WriteInto(&guid, kMaxPathBufLen),
                                          kMaxPathBufLen)) {
      return false;
    }
    // In case it has two GUID's (see above mentioned blog), do it again.
    if (!GetVolumeNameForVolumeMountPoint(guid.c_str(),
                                          WriteInto(&guid, kMaxPathBufLen),
                                          kMaxPathBufLen)) {
      return false;
    }
    VLOG(1) << "guid=" << guid;
  }

  // If we're adding a floppy drive, return without querying any more
  // drive metadata -- it will cause the floppy drive to seek.
  // Note: treats FLOPPY as FIXED_MASS_STORAGE. This is intentional.
  DeviceType device_type = GetDeviceType(mount_point);
  if (device_type == FLOPPY) {
    VLOG(1) << "Returning floppy";
    if (info) {
      info->device_id = chrome::MediaStorageUtil::MakeDeviceId(
          chrome::MediaStorageUtil::FIXED_MASS_STORAGE, UTF16ToUTF8(guid));
    }
    return true;
  }

  if (!info)
    return true;

  chrome::MediaStorageUtil::Type type =
      chrome::MediaStorageUtil::FIXED_MASS_STORAGE;
  if (device_type == REMOVABLE) {
    type = chrome::MediaStorageUtil::REMOVABLE_MASS_STORAGE_NO_DCIM;
    if (chrome::MediaStorageUtil::HasDcim(base::FilePath(mount_point)))
       type = chrome::MediaStorageUtil::REMOVABLE_MASS_STORAGE_WITH_DCIM;
  }

  // NOTE: experimentally, this function returns false if there is no volume
  // name set.
  string16 volume_label;
  GetVolumeInformationW(device_path.value().c_str(),
                        WriteInto(&volume_label, kMaxPathBufLen),
                        kMaxPathBufLen, NULL, NULL, NULL, NULL, 0);

  info->location = mount_point;
  info->total_size_in_bytes = GetVolumeSize(mount_point);
  info->device_id = chrome::MediaStorageUtil::MakeDeviceId(
      type, UTF16ToUTF8(guid));

  // TODO(gbillock): if volume_label.empty(), get the vendor/model information
  // for the volume.
  info->vendor_name = string16();
  info->model_name = string16();
  info->storage_label = volume_label;

  return true;
}

// Returns a vector of all the removable mass storage devices that are
// connected.
std::vector<base::FilePath> GetAttachedDevices() {
  std::vector<base::FilePath> result;
  string16 volume_name;
  HANDLE find_handle = FindFirstVolume(WriteInto(&volume_name, kMaxPathBufLen),
                                       kMaxPathBufLen);
  if (find_handle == INVALID_HANDLE_VALUE)
    return result;

  while (true) {
    string16 volume_path;
    DWORD return_count;
    if (GetVolumePathNamesForVolumeName(volume_name.c_str(),
                                        WriteInto(&volume_path, kMaxPathBufLen),
                                        kMaxPathBufLen, &return_count)) {
      result.push_back(base::FilePath(volume_path));
    }
    if (!FindNextVolume(find_handle, WriteInto(&volume_name, kMaxPathBufLen),
                        kMaxPathBufLen)) {
      if (GetLastError() != ERROR_NO_MORE_FILES)
        DPLOG(ERROR);
      break;
    }
  }

  FindVolumeClose(find_handle);
  return result;
}

// Eject a removable volume at the specified |device| path. This works by
// 1) locking the volume,
// 2) unmounting the volume,
// 3) ejecting the volume.
// If the lock fails, it will re-schedule itself.
// See http://support.microsoft.com/kb/165721
void EjectDeviceInThreadPool(
    const base::FilePath& device,
    base::Callback<void(chrome::StorageMonitor::EjectStatus)> callback,
    scoped_refptr<base::SequencedTaskRunner> task_runner,
    int iteration) {
  base::FilePath::StringType volume_name;
  base::FilePath::CharType drive_letter = device.value()[0];
  // Don't try to eject if the path isn't a simple one -- we're not
  // sure how to do that yet. Need to figure out how to eject volumes mounted
  // at not-just-drive-letter paths.
  if (drive_letter < L'A' || drive_letter > L'Z' ||
      device != device.DirName()) {
    content::BrowserThread::PostTask(
        content::BrowserThread::UI, FROM_HERE,
        base::Bind(callback, chrome::StorageMonitor::EJECT_FAILURE));
    return;
  }
  base::SStringPrintf(&volume_name, L"\\\\.\\%lc:", drive_letter);

  base::win::ScopedHandle volume_handle(CreateFile(
      volume_name.c_str(),
      GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
      NULL, OPEN_EXISTING, 0, NULL));

  if (!volume_handle.IsValid()) {
    content::BrowserThread::PostTask(
        content::BrowserThread::UI, FROM_HERE,
        base::Bind(callback, chrome::StorageMonitor::EJECT_FAILURE));
    return;
  }

  DWORD bytes_returned = 0;  // Unused, but necessary for ioctl's.

  // Lock the drive to be ejected (so that other processes can't open
  // files on it). If this fails, it means some other process has files
  // open on the device. Note that the lock is released when the volume
  // handle is closed, and this is done by the ScopedHandle above.
  BOOL locked = DeviceIoControl(volume_handle, FSCTL_LOCK_VOLUME,
                                NULL, 0, NULL, 0, &bytes_returned, NULL);
  UMA_HISTOGRAM_ENUMERATION("StorageMonitor.EjectWinLock",
                            LOCK_ATTEMPT, NUM_LOCK_OUTCOMES);
  if (!locked) {
    UMA_HISTOGRAM_ENUMERATION("StorageMonitor.EjectWinLock",
                              iteration == 0 ? LOCK_TIMEOUT : LOCK_TIMEOUT2,
                              NUM_LOCK_OUTCOMES);
    const int kNumLockRetries = 1;
    const base::TimeDelta kLockRetryInterval =
        base::TimeDelta::FromMilliseconds(500);
    if (iteration < kNumLockRetries) {
      // Try again -- the lock may have been a transient one. This happens on
      // things like AV disk lock for some reason, or another process
      // transient disk lock.
      task_runner->PostDelayedTask(FROM_HERE,
          base::Bind(&EjectDeviceInThreadPool,
                     device, callback, task_runner, iteration + 1),
          kLockRetryInterval);
      return;
    }

    content::BrowserThread::PostTask(
        content::BrowserThread::UI, FROM_HERE,
        base::Bind(callback, chrome::StorageMonitor::EJECT_IN_USE));
    return;
  }

  // Unmount the device from the filesystem -- this will remove it from
  // the file picker, drive enumerations, etc.
  BOOL dismounted = DeviceIoControl(volume_handle, FSCTL_DISMOUNT_VOLUME,
                                    NULL, 0, NULL, 0, &bytes_returned, NULL);

  // Reached if we acquired a lock, but could not dismount. This might
  // occur if another process unmounted without locking. Call this OK,
  // since the volume is now unreachable.
  if (!dismounted) {
    DeviceIoControl(volume_handle, FSCTL_UNLOCK_VOLUME,
                    NULL, 0, NULL, 0, &bytes_returned, NULL);
    content::BrowserThread::PostTask(
        content::BrowserThread::UI, FROM_HERE,
        base::Bind(callback, chrome::StorageMonitor::EJECT_OK));
    return;
  }

  PREVENT_MEDIA_REMOVAL pmr_buffer;
  pmr_buffer.PreventMediaRemoval = FALSE;
  // Mark the device as safe to remove.
  if (!DeviceIoControl(volume_handle, IOCTL_STORAGE_MEDIA_REMOVAL,
                       &pmr_buffer, sizeof(PREVENT_MEDIA_REMOVAL),
                       NULL, 0, &bytes_returned, NULL)) {
    content::BrowserThread::PostTask(
        content::BrowserThread::UI, FROM_HERE,
        base::Bind(callback, chrome::StorageMonitor::EJECT_FAILURE));
    return;
  }

  // Physically eject or soft-eject the device.
  if (!DeviceIoControl(volume_handle, IOCTL_STORAGE_EJECT_MEDIA,
                       NULL, 0, NULL, 0, &bytes_returned, NULL)) {
    content::BrowserThread::PostTask(
        content::BrowserThread::UI, FROM_HERE,
        base::Bind(callback, chrome::StorageMonitor::EJECT_FAILURE));
    return;
  }

  content::BrowserThread::PostTask(
      content::BrowserThread::UI, FROM_HERE,
      base::Bind(callback, chrome::StorageMonitor::EJECT_OK));
}

}  // namespace

namespace chrome {

const int kWorkerPoolNumThreads = 3;
const char* kWorkerPoolNamePrefix = "DeviceInfoPool";

VolumeMountWatcherWin::VolumeMountWatcherWin()
    : device_info_worker_pool_(new base::SequencedWorkerPool(
          kWorkerPoolNumThreads, kWorkerPoolNamePrefix)),
      weak_factory_(this),
      notifications_(NULL) {
  task_runner_ =
      device_info_worker_pool_->GetSequencedTaskRunnerWithShutdownBehavior(
          device_info_worker_pool_->GetSequenceToken(),
          base::SequencedWorkerPool::CONTINUE_ON_SHUTDOWN);
}

// static
base::FilePath VolumeMountWatcherWin::DriveNumberToFilePath(int drive_number) {
  if (drive_number < 0 || drive_number > 25)
    return base::FilePath();
  string16 path(L"_:\\");
  path[0] = L'A' + drive_number;
  return base::FilePath(path);
}

// In order to get all the weak pointers created on the UI thread, and doing
// synchronous Windows calls in the worker pool, this kicks off a chain of
// events which will
// a) Enumerate attached devices
// b) Create weak pointers for which to send completion signals from
// c) Retrieve metadata on the volumes and then
// d) Notify that metadata to listeners.
void VolumeMountWatcherWin::Init() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  // When VolumeMountWatcherWin is created, the message pumps are not running
  // so a posted task from the constructor would never run. Therefore, do all
  // the initializations here.
  base::PostTaskAndReplyWithResult(task_runner_, FROM_HERE,
      GetAttachedDevicesCallback(),
      base::Bind(&VolumeMountWatcherWin::AddDevicesOnUIThread,
                 weak_factory_.GetWeakPtr()));
}

void VolumeMountWatcherWin::AddDevicesOnUIThread(
    std::vector<base::FilePath> removable_devices) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  for (size_t i = 0; i < removable_devices.size(); i++) {
    VLOG(1) << "Adding device " << removable_devices[i].value();
    if (ContainsKey(pending_device_checks_, removable_devices[i]))
      continue;
    pending_device_checks_.insert(removable_devices[i]);
    task_runner_->PostTask(FROM_HERE, base::Bind(
        &VolumeMountWatcherWin::RetrieveInfoForDeviceAndAdd,
        removable_devices[i], GetDeviceDetailsCallback(),
        weak_factory_.GetWeakPtr()));
  }
}

// static
void VolumeMountWatcherWin::RetrieveInfoForDeviceAndAdd(
    const base::FilePath& device_path,
    const GetDeviceDetailsCallbackType& get_device_details_callback,
    base::WeakPtr<chrome::VolumeMountWatcherWin> volume_watcher) {
  StorageInfo info;
  if (!get_device_details_callback.Run(device_path, &info)) {
    BrowserThread::PostTask(BrowserThread::UI, FROM_HERE, base::Bind(
        &chrome::VolumeMountWatcherWin::DeviceCheckComplete,
        volume_watcher, device_path));
    return;
  }

  BrowserThread::PostTask(BrowserThread::UI, FROM_HERE, base::Bind(
      &chrome::VolumeMountWatcherWin::HandleDeviceAttachEventOnUIThread,
      volume_watcher, device_path, info));
}

void VolumeMountWatcherWin::DeviceCheckComplete(
    const base::FilePath& device_path) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  pending_device_checks_.erase(device_path);

  if (pending_device_checks_.size() == 0) {
    if (notifications_)
      notifications_->MarkInitialized();
  }
}

VolumeMountWatcherWin::GetAttachedDevicesCallbackType
    VolumeMountWatcherWin::GetAttachedDevicesCallback() const {
  return base::Bind(&GetAttachedDevices);
}

VolumeMountWatcherWin::GetDeviceDetailsCallbackType
    VolumeMountWatcherWin::GetDeviceDetailsCallback() const {
  return base::Bind(&GetDeviceDetails);
}

bool VolumeMountWatcherWin::GetDeviceInfo(const base::FilePath& device_path,
                                          StorageInfo* info) const {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  base::FilePath path(device_path);
  MountPointDeviceMetadataMap::const_iterator iter =
      device_metadata_.find(path.value());
  while (iter == device_metadata_.end() && path.DirName() != path) {
    path = path.DirName();
    iter = device_metadata_.find(path.value());
  }

  if (iter == device_metadata_.end())
    return false;

  if (info)
    *info = iter->second;

  return true;
}

void VolumeMountWatcherWin::OnWindowMessage(UINT event_type, LPARAM data) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  switch (event_type) {
    case DBT_DEVICEARRIVAL: {
      if (IsLogicalVolumeStructure(data)) {
        DWORD unitmask = GetVolumeBitMaskFromBroadcastHeader(data);
        std::vector<base::FilePath> paths;
        for (int i = 0; unitmask; ++i, unitmask >>= 1) {
          if (!(unitmask & 0x01))
            continue;
          paths.push_back(DriveNumberToFilePath(i));
        }
        AddDevicesOnUIThread(paths);
      }
      break;
    }
    case DBT_DEVICEREMOVECOMPLETE: {
      if (IsLogicalVolumeStructure(data)) {
        DWORD unitmask = GetVolumeBitMaskFromBroadcastHeader(data);
        for (int i = 0; unitmask; ++i, unitmask >>= 1) {
          if (!(unitmask & 0x01))
            continue;
          HandleDeviceDetachEventOnUIThread(DriveNumberToFilePath(i).value());
        }
      }
      break;
    }
  }
}

void VolumeMountWatcherWin::SetNotifications(
    StorageMonitor::Receiver* notifications) {
  notifications_ = notifications;
}

VolumeMountWatcherWin::~VolumeMountWatcherWin() {
  weak_factory_.InvalidateWeakPtrs();
  device_info_worker_pool_->Shutdown();
}

void VolumeMountWatcherWin::HandleDeviceAttachEventOnUIThread(
    const base::FilePath& device_path,
    const StorageInfo& info) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  device_metadata_[device_path.value()] = info;

  DeviceCheckComplete(device_path);

  // Don't call removable storage observers for fixed volumes.
  if (!MediaStorageUtil::IsRemovableDevice(info.device_id))
    return;

  if (notifications_)
    notifications_->ProcessAttach(info);
}

void VolumeMountWatcherWin::HandleDeviceDetachEventOnUIThread(
    const string16& device_location) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  MountPointDeviceMetadataMap::const_iterator device_info =
      device_metadata_.find(device_location);
  // If the device isn't type removable (like a CD), it won't be there.
  if (device_info == device_metadata_.end())
    return;

  if (notifications_)
    notifications_->ProcessDetach(device_info->second.device_id);
  device_metadata_.erase(device_info);
}

void VolumeMountWatcherWin::EjectDevice(
    const std::string& device_id,
    base::Callback<void(StorageMonitor::EjectStatus)> callback) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  base::FilePath device =
      chrome::MediaStorageUtil::FindDevicePathById(device_id);
  if (device.empty()) {
    callback.Run(StorageMonitor::EJECT_FAILURE);
    return;
  }
  if (device_metadata_.erase(device.value()) == 0) {
    callback.Run(StorageMonitor::EJECT_FAILURE);
    return;
  }

  task_runner_->PostTask(FROM_HERE,
      base::Bind(&EjectDeviceInThreadPool, device, callback, task_runner_, 0));
}

}  // namespace chrome
