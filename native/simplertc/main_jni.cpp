#include <jni.h>

#include <iostream>
#include <stdlib.h>

#include "talk/base/basicdefs.h"
#include "talk/base/common.h"
#include "talk/base/helpers.h"
#include "talk/base/logging.h"

#include "simplertc.h"

#undef JNIEXPORT
#define JNIEXPORT __attribute__((visibility("default")))
#define JOW(rettype, name) extern "C" rettype JNIEXPORT JNICALL \
          Java_teaonly_simplertc_MainActivity_##name

JOW(int, startRtcTask)(JNIEnv*, jclass) {
    //talk_base::LogMessage::LogToDebug(talk_base::LS_VERBOSE);
    talk_base::LogMessage::LogToDebug(talk_base::LS_ERROR);
    talk_base::LogMessage::LogTimestamps();
    talk_base::LogMessage::LogThreads();

    SimpleRTC *camera = new SimpleRTC( "droid" );
    camera->Login("192.168.0.101", 19790 );
}

