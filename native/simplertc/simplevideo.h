#ifndef _SIMPLEVIDEO_H_
#define _SIMPLEVIDEO_H_

#include <vector>

#include "talk/base/scoped_ptr.h"
#include "talk/media/base/codec.h"
#include "talk/media/base/videocommon.h"
#include "talk/session/media/channel.h"
#include "talk/base/sigslotrepeater.h"
#include "webrtc/modules/rtp_rtcp/interface/rtp_rtcp_defines.h"

namespace webrtc {
class RtpRtcp;
class WebRtcRTPHeader;
};    

namespace cricket {
// from libjingle
class VoiceMediaChannel;
// internal help class
class SimpleVideoRtcpFeedback;

class SimpleVideoEngine : public sigslot::has_slots<>, public talk_base::MessageHandler, public webrtc::Transport {
public:
    SimpleVideoEngine();
    ~SimpleVideoEngine();    
    
    void StartSend(bool send);    
    bool InsertRtcpPackage(unsigned char *data, unsigned int len);

    sigslot::signal3<SimpleVideoEngine*, const void *, int> SignalSendPacket;
    sigslot::signal3<SimpleVideoEngine*, const void *, int> SignalSendRTCPPacket;

protected:
    void doEncoding();
    virtual void OnMessage(talk_base::Message *msg);

    //
    // interfaces from webrtc::Transport
    // 
    virtual int SendPacket(int channel, const void *data, int len) ;
    virtual int SendRTCPPacket(int channel, const void *data, int len) ;

private:
    webrtc::RtpRtcp* rtp_rtcp_module_;
    SimpleVideoRtcpFeedback* rtcp_feedback_;

    talk_base::Thread* encoding_thread_;
    bool isSend_;
};

class SimpleVideoMediaChannel: public cricket::VideoMediaChannel,
                              public talk_base::MessageHandler {
public:
    SimpleVideoMediaChannel(
            SimpleVideoEngine* engine, 
            cricket::VoiceMediaChannel* voice_channel, 
            talk_base::Thread* thread);
    ~SimpleVideoMediaChannel();
    bool Init();
    
    // interfaces from common MediaChannel
    virtual void OnPacketReceived(talk_base::Buffer* packet);
    virtual void OnRtcpReceived(talk_base::Buffer* packet);

    virtual bool AddSendStream(const StreamParams& sp);
    virtual bool RemoveSendStream(uint32 ssrc);
    virtual bool AddRecvStream(const StreamParams& sp);
    virtual bool RemoveRecvStream(uint32 ssrc);

    virtual bool MuteStream(uint32 ssrc, bool on);
    virtual bool SetRecvRtpHeaderExtensions(
            const std::vector<RtpHeaderExtension>& extensions);
    virtual bool SetSendRtpHeaderExtensions(
            const std::vector<RtpHeaderExtension>& extensions);
    virtual bool SetSendBandwidth(bool autobw, int bps);

    // interface from VideoMediaChannel
    virtual bool SetRecvCodecs(const std::vector<VideoCodec>& codecs);
    virtual bool SetSendCodecs(const std::vector<VideoCodec>& codecs);
    virtual bool GetSendCodec(VideoCodec* send_codec);
    virtual bool SetSendStreamFormat(uint32 ssrc, const VideoFormat& format);
    virtual bool SetRender(bool render);
    virtual bool SetSend(bool send);
    virtual bool SetRenderer(uint32 ssrc, VideoRenderer* renderer);
    virtual bool SetCapturer(uint32 ssrc, VideoCapturer* capturer);
    virtual bool GetStats(VideoMediaInfo* info);
    virtual void OnReadyToSend(bool send);

    virtual bool SendIntraFrame();
    virtual bool RequestIntraFrame();
    virtual bool SetOptions(const VideoOptions& options);
    virtual bool GetOptions(VideoOptions* options) const;
    virtual void UpdateAspectRatio(int ratio_w, int ratio_h);

protected:
    void OnSendPacket(SimpleVideoEngine *, const void *data, int len) ;
    void OnSendRTCPPacket(SimpleVideoEngine *, const void *data, int len) ;

private:
    virtual void OnMessage(talk_base::Message *msg);
    
private:
    SimpleVideoEngine* engine_;
    cricket::VoiceMediaChannel* voice_channel_;
    talk_base::Thread* worker_thread_;
    unsigned int target_ssrc_;

    //
    //  codec infomation
    //
    VideoOptions options_;
};


}

#endif /* TALK_APP_WEBRTC_VIDEOMEDIAENGINE_H_ */
