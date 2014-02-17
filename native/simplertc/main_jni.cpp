#include <string>
#include <jni.h>

#include "talk/base/ssladapter.h"
#include "talk/base/basicdefs.h"
#include "talk/base/common.h"
#include "talk/base/helpers.h"
#include "talk/base/logging.h"

#include "simplertc.h"

#undef JNIEXPORT
#define JNIEXPORT __attribute__((visibility("default")))
#define JOW(rettype, name) extern "C" rettype JNIEXPORT JNICALL \
      Java_com_beginvision_simplertc_NativeAgent_##name

//
//  Global variables
//


//
//  Internal helper functions
//
static std::string convert_jstring(JNIEnv* env, const jstring &js) {
    static char outbuf[1024];
    int len = env->GetStringLength(js);
    env->GetStringUTFRegion(js, 0, len, outbuf);
    std::string str = outbuf;
    return str;
}

//
//  Global functions called from Java side 
//
JOW(int, jmain)(JNIEnv* env, jclass, jstring js_server, jint port, jstring js_myname) {
    std::string server = convert_jstring(env, js_server);
    std::string myname = convert_jstring(env, js_myname);

    talk_base::InitializeSSL(); 
    //talk_base::LogMessage::LogToDebug(talk_base::LS_VERBOSE);
    //talk_base::LogMessage::LogToDebug(talk_base::LS_WARNING);
    talk_base::LogMessage::LogToDebug(talk_base::LS_ERROR);
    talk_base::LogMessage::LogTimestamps();
    talk_base::LogMessage::LogThreads();
 
    SimpleRTC *camera = new SimpleRTC( myname , true);
    camera->Login(server, port);
 
    return 0;
}


//
//  Top level library load/unload 
//
extern "C" jint JNIEXPORT JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved) {
    JNIEnv* jni;
    if (jvm->GetEnv(reinterpret_cast<void**>(&jni), JNI_VERSION_1_6) != JNI_OK)
        return -1;
    return JNI_VERSION_1_6;
}

extern "C" jint JNIEXPORT JNICALL JNI_OnUnLoad(JavaVM *jvm, void *reserved) {
    return 0;
}

