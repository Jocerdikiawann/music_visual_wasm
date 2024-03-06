#include "jni_fn.h"
JNI_DATA jd = {};

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
  TraceLog(LOG_ERROR, "INITIALIZE JNI");

  jd.jvm = vm;
  JNIEnv *env = NULL;
  if (jd.jvm) {
    (*jd.jvm)->AttachCurrentThread(jd.jvm, &env, NULL);
  }

  return JNI_VERSION_1_6;
}
