#ifndef _RTCFACTORY_H_
#define _RTCFACTORY_H_

#include <string>

#include "talk/app/webrtc/peerconnectionfactory.h"
#include "talk/app/webrtc/mediastreaminterface.h"
#include "talk/app/webrtc/peerconnectioninterface.h"
#include "talk/base/scoped_ptr.h"
#include "talk/base/thread.h"
#include "talk/session/media/channelmanager.h"

namespace webrtc {

class RtcFactory : public PeerConnectionFactory {
public:
    bool Prepare();
    
protected:
    RtcFactory();
    virtual ~RtcFactory();
    
    virtual void OnMessage(talk_base::Message* msg);

private:
    virtual bool Prepare_s();
        
};

// Create a new instance of PeerConnectionFactoryInterface. 
talk_base::scoped_refptr<RtcFactory> CreateRtcFactory();

}  // namespace webrtc

#endif
