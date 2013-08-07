/* Copyright 2013 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file. */

#ifndef LIBRARIES_NACL_SYS_UTSNAME_H_
#define LIBRARIES_NACL_SYS_UTSNAME_H_

#define _UTSNAME_LENGTH 65

struct utsname {
  char sysname[_UTSNAME_LENGTH];
  char nodename[_UTSNAME_LENGTH];
  char release[_UTSNAME_LENGTH];
  char version[_UTSNAME_LENGTH];
  char machine[_UTSNAME_LENGTH];
};

int uname(struct utsname *buf);

#endif
