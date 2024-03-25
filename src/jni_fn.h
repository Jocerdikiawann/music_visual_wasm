#ifndef JNI_FN
#define JNI_FN

#include "./../include/asset_manager.h"
#include "./../include/asset_manager_jni.h"
#include "./../include/jni.h"
#ifdef PLATFORM_ANDROID
#include <android_native_app_glue.h>
#endif
#include "ext.h"

typedef struct {
  JavaVM *jvm;
  JNIEnv *env;
  jobject activity, application, application_context;
  struct android_app *app;
} JNI_DATA;

void open_dir();

#endif // !JNI_FN
