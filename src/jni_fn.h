#ifndef JNI_FN
#define JNI_FN

#include "./../include/asset_manager.h"
#include "./../include/asset_manager_jni.h"
#include "./../include/jni.h"

#include "ext.h"

typedef struct {
  JavaVM *jvm;
  JNIEnv *env;
  jobject activity;
} JNI_DATA;

void open_dir();

#endif // !JNI_FN
