#include <iostream>

#include "rtcfactory.h"

#include "talk/app/webrtc/audiotrack.h"
#include "talk/app/webrtc/localaudiosource.h"
#include "talk/app/webrtc/localvideosource.h"
#include "talk/app/webrtc/mediastreamproxy.h"
#include "talk/app/webrtc/mediastreamtrackproxy.h"
#include "talk/app/webrtc/peerconnection.h"
#include "talk/app/webrtc/peerconnectionproxy.h"
#include "talk/app/webrtc/portallocatorfactory.h"
#include "talk/app/webrtc/videosourceproxy.h"
#include "talk/app/webrtc/videotrack.h"
#include "talk/media/devices/dummydevicemanager.h"
#include "talk/media/webrtc/webrtcmediaengine.h"

#include "simplemedia.h"

using talk_base::scoped_refptr;

enum {
    MSG_PREPARE_FACTORY = 100,  
};

struct PrepareParameter {
    bool result;
};

namespace webrtc {

RtcFactory::RtcFactory()
    : PeerConnectionFactory() {
  
}

RtcFactory::~RtcFactory() {

}

bool RtcFactory::Prepare() {
    PrepareParameter p;
    p.result = false;
    talk_base::TypedMessageData<PrepareParameter> d(p);
    signaling_thread_->Send(this, MSG_PREPARE_FACTORY, &d);
    return d.data().result;
}

void RtcFactory::OnMessage(talk_base::Message* msg) { 
    PeerConnectionFactory::OnMessage(msg);

    switch( msg->message_id ) { 
        case MSG_PREPARE_FACTORY: {
            talk_base::TypedMessageData<PrepareParameter>* pdata = static_cast<talk_base::TypedMessageData<PrepareParameter>* > (msg->pdata);
            pdata->data().result = Prepare_s();
            break;
        }
    }
}

bool RtcFactory::Prepare_s() {
    talk_base::InitRandom(talk_base::Time());
    if (owns_ptrs_) {
        allocator_factory_ = PortAllocatorFactory::Create(worker_thread_);
        if (!allocator_factory_)
            return false;
    }
    
    cricket::SimpleMediaEngine* media_engine;
    media_engine = new cricket::SimpleMediaEngine(worker_thread_);

    cricket::DummyDeviceManager* device_manager( 
        new cricket::DummyDeviceManager());
    channel_manager_.reset(new cricket::ChannelManager(
                media_engine, device_manager, worker_thread() ));

    if (!channel_manager_->Init()) {
        return false;
    }

    return true;
}

scoped_refptr<RtcFactory> CreateRtcFactory() {
    scoped_refptr<RtcFactory> rtc_factory(
            new talk_base::RefCountedObject<RtcFactory>());

    if (!rtc_factory->Prepare()) {
        return NULL;
    }

    return rtc_factory;
}

}
