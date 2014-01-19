#include <iostream>
#include <stdlib.h>

#include "talk/base/basicdefs.h"
#include "talk/base/common.h"
#include "talk/base/helpers.h"
#include "talk/base/logging.h"

#include "simplertc.h"

int main(int argc, char *argv[]) {
    //talk_base::LogMessage::LogToDebug(talk_base::LS_VERBOSE);
    //talk_base::LogMessage::LogToDebug(talk_base::LS_WARNING);
    talk_base::LogMessage::LogToDebug(talk_base::LS_ERROR);
    talk_base::LogMessage::LogTimestamps();
    talk_base::LogMessage::LogThreads();

    if ( argc < 4) {
        std::cout << "usage: mixer server port name" << std::endl;
        return -1;
    }
    
    SimpleRTC *camera = new SimpleRTC( argv[3] , true);
    camera->Login(argv[1], atoi(argv[2]) );
    camera->Run();
    
    return 0;
}

