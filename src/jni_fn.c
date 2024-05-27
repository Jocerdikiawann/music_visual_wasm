#include "jni_fn.h"

static const char *ACTION_GET_CONTENT = "android.intent.action.GET_CONTENT";
static const int REQUEST_CODE_PICK_FILE = 1;
JNI_DATA jd = {};

// JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
//   TraceLog(LOG_ERROR, "INITIALIZE JNI");

//   JNIEnv *env;
//   jd.jvm = vm;

//   if (jd.jvm == NULL) {
//     TraceLog(LOG_ERROR, "Invalid Java VM");
//   }

//   // NOTE: set Java environment
//   int status;
//   status = (*jd.jvm)->GetEnv(jd.jvm, (void **)&jd.env, JNI_VERSION_1_6);
//   if (status < 0) {
//     TraceLog(LOG_ERROR, "Failed to get JNI environment, try to attach
//     thread"); status = (*jd.jvm)->AttachCurrentThread(jd.jvm, &jd.env, NULL);
//     if (status < 0) {
//       TraceLog(LOG_ERROR, "Failed to attach current thread");
//     }
//   }

//   if (jd.env == 0) {
//     TraceLog(LOG_ERROR, "Couldn't get JNI env");
//   }

//   return JNI_VERSION_1_6;
// }

// void android_main(struct android_app *state) { jd.app = state; }

void open_dir() {
  jclass intent_class = (*jd.env)->FindClass(jd.env, "android/content/Intent");
  jmethodID init_method =
      (*jd.env)->GetMethodID(jd.env, intent_class, "<init>", "()V");
  jobject intent = (*jd.env)->NewObject(jd.env, intent_class, init_method);

  jmethodID action_method = (*jd.env)->GetMethodID(
      jd.env, intent_class, "setAction", "(Ljava/lang/String;)V");

  jstring action_content = (*jd.env)->NewStringUTF(jd.env, ACTION_GET_CONTENT);
  (*jd.env)->CallObjectMethod(jd.env, intent, action_method, action_content);

  jmethodID type_method = (*jd.env)->GetMethodID(
      jd.env, intent_class, "setType", "(Ljava/lang/String;)V");

  jstring type_content = (*jd.env)->NewStringUTF(jd.env, "*/*");

  (*jd.env)->CallObjectMethod(jd.env, intent, type_method, type_content);

  jclass activity_class =
      (*jd.env)->FindClass(jd.env, "android/app/NativeActivity");

  jmethodID activity_method =
      (*jd.env)->GetMethodID(jd.env, activity_class, "startActivityForResult",
                             "(Landroid/content/Intent;I)V");

  (*jd.env)->CallVoidMethod(jd.env, jd.activity, activity_method, intent,
                            REQUEST_CODE_PICK_FILE);
}
