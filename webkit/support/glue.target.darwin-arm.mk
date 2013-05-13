# This file is generated by gyp; do not edit.

include $(CLEAR_VARS)

LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_MODULE := webkit_support_glue_gyp
LOCAL_MODULE_SUFFIX := .a
LOCAL_MODULE_TAGS := optional
gyp_intermediate_dir := $(call local-intermediates-dir)
gyp_shared_intermediate_dir := $(call intermediates-dir-for,GYP,shared)

# Make sure our deps are built first.
GYP_TARGET_DEPENDENCIES := \
	$(call intermediates-dir-for,GYP,ppapi_ppapi_c_gyp)/ppapi_c.stamp \
	$(call intermediates-dir-for,STATIC_LIBRARIES,skia_skia_gyp)/skia_skia_gyp.a \
	$(call intermediates-dir-for,GYP,third_party_icu_icui18n_gyp)/icui18n.stamp \
	$(call intermediates-dir-for,GYP,third_party_icu_icuuc_gyp)/icuuc.stamp \
	$(call intermediates-dir-for,GYP,third_party_npapi_npapi_gyp)/npapi.stamp \
	$(call intermediates-dir-for,STATIC_LIBRARIES,ui_gl_gl_gyp)/ui_gl_gl_gyp.a \
	$(call intermediates-dir-for,STATIC_LIBRARIES,ui_ui_gyp)/ui_ui_gyp.a \
	$(call intermediates-dir-for,GYP,ui_ui_resources_gyp)/ui_resources.stamp \
	$(call intermediates-dir-for,GYP,v8_tools_gyp_v8_gyp)/v8.stamp \
	$(call intermediates-dir-for,GYP,webkit_support_webkit_resources_gyp)/webkit_resources.stamp \
	$(call intermediates-dir-for,GYP,webkit_support_webkit_strings_gyp)/webkit_strings.stamp \
	$(call intermediates-dir-for,GYP,webkit_support_overscroller_jni_headers_gyp)/overscroller_jni_headers.stamp

GYP_GENERATED_OUTPUTS :=

# Make sure our deps and generated files are built first.
LOCAL_ADDITIONAL_DEPENDENCIES := $(GYP_TARGET_DEPENDENCIES) $(GYP_GENERATED_OUTPUTS)

LOCAL_CPP_EXTENSION := .cc
LOCAL_GENERATED_SOURCES :=

GYP_COPIED_SOURCE_ORIGIN_DIRS :=

LOCAL_SRC_FILES := \
	webkit/plugins/npapi/plugin_host.cc \
	webkit/plugins/npapi/plugin_instance.cc \
	webkit/plugins/npapi/plugin_lib.cc \
	webkit/plugins/npapi/plugin_lib_posix.cc \
	webkit/plugins/npapi/plugin_list.cc \
	webkit/plugins/npapi/plugin_list_posix.cc \
	webkit/plugins/npapi/plugin_stream.cc \
	webkit/plugins/npapi/plugin_stream_posix.cc \
	webkit/plugins/npapi/plugin_stream_url.cc \
	webkit/plugins/npapi/plugin_string_stream.cc \
	webkit/plugins/npapi/plugin_utils.cc \
	webkit/plugins/npapi/webplugin.cc \
	webkit/plugins/npapi/webplugin_delegate_impl.cc \
	webkit/plugins/npapi/webplugin_delegate_impl_android.cc \
	webkit/plugins/npapi/webplugin_impl.cc \
	webkit/plugins/plugin_constants.cc \
	webkit/plugins/plugin_switches.cc \
	webkit/plugins/ppapi/audio_helper.cc \
	webkit/plugins/ppapi/content_decryptor_delegate.cc \
	webkit/plugins/ppapi/event_conversion.cc \
	webkit/plugins/ppapi/file_callbacks.cc \
	webkit/plugins/ppapi/host_array_buffer_var.cc \
	webkit/plugins/ppapi/host_globals.cc \
	webkit/plugins/ppapi/host_var_tracker.cc \
	webkit/plugins/ppapi/message_channel.cc \
	webkit/plugins/ppapi/npapi_glue.cc \
	webkit/plugins/ppapi/npobject_var.cc \
	webkit/plugins/ppapi/plugin_module.cc \
	webkit/plugins/ppapi/plugin_object.cc \
	webkit/plugins/ppapi/ppapi_interface_factory.cc \
	webkit/plugins/ppapi/ppapi_plugin_instance.cc \
	webkit/plugins/ppapi/ppapi_webplugin_impl.cc \
	webkit/plugins/ppapi/ppb_audio_impl.cc \
	webkit/plugins/ppapi/ppb_broker_impl.cc \
	webkit/plugins/ppapi/ppb_buffer_impl.cc \
	webkit/plugins/ppapi/ppb_file_ref_impl.cc \
	webkit/plugins/ppapi/ppb_flash_message_loop_impl.cc \
	webkit/plugins/ppapi/ppb_gpu_blacklist_private_impl.cc \
	webkit/plugins/ppapi/ppb_graphics_3d_impl.cc \
	webkit/plugins/ppapi/ppb_image_data_impl.cc \
	webkit/plugins/ppapi/ppb_network_monitor_private_impl.cc \
	webkit/plugins/ppapi/ppb_proxy_impl.cc \
	webkit/plugins/ppapi/ppb_scrollbar_impl.cc \
	webkit/plugins/ppapi/ppb_tcp_server_socket_private_impl.cc \
	webkit/plugins/ppapi/ppb_tcp_socket_private_impl.cc \
	webkit/plugins/ppapi/ppb_uma_private_impl.cc \
	webkit/plugins/ppapi/ppb_url_loader_impl.cc \
	webkit/plugins/ppapi/ppb_var_deprecated_impl.cc \
	webkit/plugins/ppapi/ppb_video_decoder_impl.cc \
	webkit/plugins/ppapi/ppb_widget_impl.cc \
	webkit/plugins/ppapi/ppb_x509_certificate_private_impl.cc \
	webkit/plugins/ppapi/quota_file_io.cc \
	webkit/plugins/ppapi/resource_creation_impl.cc \
	webkit/plugins/ppapi/resource_helper.cc \
	webkit/plugins/ppapi/string.cc \
	webkit/plugins/ppapi/url_response_info_util.cc \
	webkit/plugins/ppapi/url_request_info_util.cc \
	webkit/plugins/ppapi/usb_key_code_conversion.cc \
	webkit/plugins/sad_plugin.cc \
	webkit/plugins/webplugininfo.cc \
	webkit/plugins/webview_plugin.cc \
	webkit/glue/alt_error_page_resource_fetcher.cc \
	webkit/glue/cpp_bound_class.cc \
	webkit/glue/cpp_variant.cc \
	webkit/glue/dom_operations.cc \
	webkit/glue/fling_curve_configuration.cc \
	webkit/glue/fling_animator_impl_android.cc \
	webkit/glue/ftp_directory_listing_response_delegate.cc \
	webkit/glue/glue_serialize.cc \
	webkit/glue/image_decoder.cc \
	webkit/glue/image_resource_fetcher.cc \
	webkit/glue/multi_resolution_image_resource_fetcher.cc \
	webkit/glue/multipart_response_delegate.cc \
	webkit/glue/npruntime_util.cc \
	webkit/glue/resource_fetcher.cc \
	webkit/glue/resource_loader_bridge.cc \
	webkit/glue/resource_request_body.cc \
	webkit/glue/resource_type.cc \
	webkit/glue/scoped_clipboard_writer_glue.cc \
	webkit/glue/simple_webmimeregistry_impl.cc \
	webkit/glue/touch_fling_gesture_curve.cc \
	webkit/glue/web_discardable_memory_impl.cc \
	webkit/glue/webclipboard_impl.cc \
	webkit/glue/webcookie.cc \
	webkit/glue/webcursor.cc \
	webkit/glue/webcursor_android.cc \
	webkit/glue/webdropdata.cc \
	webkit/glue/webfileutilities_impl.cc \
	webkit/glue/webkit_glue.cc \
	webkit/glue/webkitplatformsupport_impl.cc \
	webkit/glue/webmenuitem.cc \
	webkit/glue/webpreferences.cc \
	webkit/glue/websocketstreamhandle_impl.cc \
	webkit/glue/webthemeengine_impl_android.cc \
	webkit/glue/webthread_impl.cc \
	webkit/glue/weburlloader_impl.cc \
	webkit/glue/weburlrequest_extradata_impl.cc \
	webkit/glue/weburlresponse_extradata_impl.cc \
	webkit/glue/web_io_operators.cc \
	webkit/glue/worker_task_runner.cc


# Flags passed to both C and C++ files.
MY_CFLAGS := \
	-Wno-narrowing \
	-fstack-protector \
	--param=ssp-buffer-size=4 \
	-Werror \
	-fno-exceptions \
	-fno-strict-aliasing \
	-Wall \
	-Wno-unused-parameter \
	-Wno-missing-field-initializers \
	-fvisibility=hidden \
	-pipe \
	-fPIC \
	-fno-tree-sra \
	-fuse-ld=gold \
	-Wno-psabi \
	-ffunction-sections \
	-funwind-tables \
	-g \
	-fstack-protector \
	-fno-short-enums \
	-finline-limit=64 \
	-Wa,--noexecstack \
	-U_FORTIFY_SOURCE \
	-Wno-extra \
	-Wno-ignored-qualifiers \
	-Wno-type-limits \
	-Wno-address \
	-Wno-format-security \
	-Wno-return-type \
	-Wno-sequence-point \
	-Os \
	-g \
	-fomit-frame-pointer \
	-fdata-sections \
	-ffunction-sections

MY_CFLAGS_C :=

MY_DEFS := \
	'-D_FILE_OFFSET_BITS=64' \
	'-DUSE_LINUX_BREAKPAD' \
	'-DNO_TCMALLOC' \
	'-DDISABLE_NACL' \
	'-DCHROMIUM_BUILD' \
	'-DUSE_LIBJPEG_TURBO=1' \
	'-DUSE_PROPRIETARY_CODECS' \
	'-DENABLE_GPU=1' \
	'-DUSE_OPENSSL=1' \
	'-DENABLE_EGLIMAGE=1' \
	'-DENABLE_LANGUAGE_DETECTION=1' \
	'-DWEBKIT_EXTENSIONS_IMPLEMENTATION' \
	'-DWEBKIT_GLUE_IMPLEMENTATION' \
	'-DWEBKIT_PLUGINS_IMPLEMENTATION' \
	'-DMEDIA_DISABLE_LIBVPX' \
	'-DPOSIX_AVOID_MMAP' \
	'-DSK_BUILD_NO_IMAGE_ENCODE' \
	'-DSK_DEFERRED_CANVAS_USES_GPIPE=1' \
	'-DGR_GL_CUSTOM_SETUP_HEADER="GrGLConfig_chrome.h"' \
	'-DGR_AGGRESSIVE_SHADER_OPTS=1' \
	'-DSK_ENABLE_INST_COUNT=0' \
	'-DSK_USE_POSIX_THREADS' \
	'-DSK_BUILD_FOR_ANDROID_NDK' \
	'-DU_USING_ICU_NAMESPACE=0' \
	'-D__STDC_CONSTANT_MACROS' \
	'-D__STDC_FORMAT_MACROS' \
	'-DANDROID' \
	'-D__GNU_SOURCE=1' \
	'-DUSE_STLPORT=1' \
	'-D_STLP_USE_PTR_SPECIALIZATIONS=1' \
	'-DCHROME_BUILD_ID=""' \
	'-DDYNAMIC_ANNOTATIONS_ENABLED=1' \
	'-DWTF_USE_DYNAMIC_ANNOTATIONS=1' \
	'-D_DEBUG'

LOCAL_CFLAGS := $(MY_CFLAGS_C) $(MY_CFLAGS) $(MY_DEFS)

# Include paths placed before CFLAGS/CPPFLAGS
LOCAL_C_INCLUDES := \
	$(gyp_intermediate_dir) \
	$(gyp_shared_intermediate_dir)/webkit \
	$(gyp_shared_intermediate_dir)/ui \
	$(gyp_shared_intermediate_dir)/shim_headers/ashmem/target \
	$(gyp_shared_intermediate_dir)/shim_headers/icui18n/target \
	$(gyp_shared_intermediate_dir)/shim_headers/icuuc/target \
	$(LOCAL_PATH)/third_party/khronos \
	$(LOCAL_PATH)/gpu \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/skia/config \
	$(LOCAL_PATH)/third_party/skia/src/core \
	$(LOCAL_PATH)/third_party/skia/include/config \
	$(LOCAL_PATH)/third_party/skia/include/core \
	$(LOCAL_PATH)/third_party/skia/include/effects \
	$(LOCAL_PATH)/third_party/skia/include/pdf \
	$(LOCAL_PATH)/third_party/skia/include/gpu \
	$(LOCAL_PATH)/third_party/skia/include/gpu/gl \
	$(LOCAL_PATH)/third_party/skia/include/pathops \
	$(LOCAL_PATH)/third_party/skia/include/pipe \
	$(LOCAL_PATH)/third_party/skia/include/ports \
	$(LOCAL_PATH)/third_party/skia/include/utils \
	$(LOCAL_PATH)/skia/ext \
	$(LOCAL_PATH)/third_party/WebKit/Source/Platform/chromium \
	$(LOCAL_PATH)/third_party/WebKit/Source/Platform/chromium \
	$(GYP_ABS_ANDROID_TOP_DIR)/external/icu4c/common \
	$(GYP_ABS_ANDROID_TOP_DIR)/external/icu4c/i18n \
	$(LOCAL_PATH)/third_party/npapi \
	$(LOCAL_PATH)/third_party/npapi/bindings \
	$(gyp_shared_intermediate_dir)/ui/gl \
	$(LOCAL_PATH)/third_party/mesa/MesaLib/include \
	$(gyp_shared_intermediate_dir)/ui/ui_resources \
	$(LOCAL_PATH)/v8/include \
	$(GYP_ABS_ANDROID_TOP_DIR)/frameworks/wilhelm/include \
	$(GYP_ABS_ANDROID_TOP_DIR)/bionic \
	$(GYP_ABS_ANDROID_TOP_DIR)/external/stlport/stlport

LOCAL_C_INCLUDES := $(GYP_COPIED_SOURCE_ORIGIN_DIRS) $(LOCAL_C_INCLUDES)

# Flags passed to only C++ (and not C) files.
LOCAL_CPPFLAGS := \
	-Wno-narrowing \
	-fno-rtti \
	-fno-threadsafe-statics \
	-fvisibility-inlines-hidden \
	-Wsign-compare \
	-Wno-abi \
	-Wno-error=c++0x-compat \
	-Wno-non-virtual-dtor \
	-Wno-sign-promo \
	-Wno-non-virtual-dtor

### Rules for final target.

LOCAL_LDFLAGS := \
	-Wl,-z,now \
	-Wl,-z,relro \
	-Wl,-z,noexecstack \
	-fPIC \
	-Wl,-z,relro \
	-Wl,-z,now \
	-fuse-ld=gold \
	-nostdlib \
	-Wl,--no-undefined \
	-Wl,--exclude-libs=ALL \
	-Wl,--icf=safe \
	-Wl,--gc-sections \
	-Wl,-O1 \
	-Wl,--as-needed


LOCAL_STATIC_LIBRARIES := \
	skia_skia_gyp \
	ui_gl_gl_gyp \
	ui_ui_gyp

# Enable grouping to fix circular references
LOCAL_GROUP_STATIC_LIBRARIES := true

LOCAL_SHARED_LIBRARIES := \
	libstlport \
	libdl

# Add target alias to "gyp_all_modules" target.
.PHONY: gyp_all_modules
gyp_all_modules: webkit_support_glue_gyp

# Alias gyp target name.
.PHONY: glue
glue: webkit_support_glue_gyp

include $(BUILD_STATIC_LIBRARY)
