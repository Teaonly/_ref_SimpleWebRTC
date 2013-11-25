#ifndef _SIMPLEVOICEENGINE_H_
#define _SIMPLEVOICEENGINE_H_

#include <string>
#include <vector>

#include "talk/base/sigslotrepeater.h"
#include "talk/base/stream.h"
#include "talk/media/base/rtputils.h"
#include "talk/media/webrtc/webrtccommon.h"
#include "talk/media/webrtc/webrtcvoe.h"
#include "talk/session/media/channel.h"
#include "talk/media/base/mediachannel.h"
#include "webrtc/modules/rtp_rtcp/interface/rtp_rtcp_defines.h"
#include "webrtc/modules/audio_coding/codecs/isac/main/interface/isac.h"

namespace cricket {
class VoiceMediaChannel;
class SoundclipMedia;

class SimpleVoiceEngine : public sigslot::has_slots<>, 
                          public talk_base::MessageHandler, 
                          public webrtc::Transport, 
                          public webrtc::RtcpFeedback,
                          public webrtc::RtpData {
public:    
    SimpleVoiceEngine();
    ~SimpleVoiceEngine();
    
    sigslot::signal3<SimpleVoiceEngine*, const void *, int> SignalSendPacket;
    sigslot::signal3<SimpleVoiceEngine*, const void *, int> SignalSendRTCPPacket;

    void StartSend();
    bool InsertRtpPackage(unsigned char *data, unsigned int len);
    bool InsertRtcpPackage(unsigned char *data, unsigned int len);    

protected:
    virtual void OnMessage(talk_base::Message *msg);

    //
    // interfaces from webrtc::Transport and webrtc::RtpData
    // 
    virtual int SendPacket(int channel, const void *data, int len);
    virtual int SendRTCPPacket(int channel, const void *data, int len);
    virtual int32_t OnReceivedPayloadData( const uint8_t* payloadData,
                                           const uint16_t payloadSize,
                                           const webrtc::WebRtcRTPHeader* rtpHeader);

    //
    // interface from webrtc::RtcpFeedback 
    //
    virtual void OnRTCPPacketTimeout(const int32_t id) {
    }
    virtual void OnLipSyncUpdate(const int32_t id, 
            const int32_t audioVideoOffset) {
    };  
    virtual void OnXRVoIPMetricReceived(
            const int32_t id, 
            const webrtc::RTCPVoIPMetric* metric) {
    };  
    virtual void OnApplicationDataReceived(const int32_t id, 
            const uint8_t subType,
            const uint32_t name,
            const uint16_t length,
            const uint8_t* data) {
    };  
    virtual void OnSendReportReceived(const int32_t id, 
            const uint32_t senderSSRC,
            uint32_t ntp_secs,
            uint32_t ntp_frac,
            uint32_t timestamp) {
    };  
    virtual void OnReceiveReportReceived(const int32_t id, 
            const uint32_t senderSSRC) {
    };  
    virtual void OnReceivedSLI(uint32_t ssrc,
            uint8_t pictureId) {
    };  
    virtual void OnReceivedRPSI(uint32_t ssrc,
            uint64_t pictureId) {
    };  
    virtual void OnLocalSsrcChanged(uint32_t old_ssrc, uint32_t new_ssrc) {
    }; 

private:    
    webrtc::RtpRtcp* rtp_rtcp_module_;
    
    ISACStruct* decoder_;    
    ISACStruct* encoder_; 
    talk_base::Thread* encoding_thread_;
    bool isSend_;
};

class SimpleVoiceMediaChannel: public VoiceMediaChannel {
public:
    SimpleVoiceMediaChannel();
    virtual ~SimpleVoiceMediaChannel();

    // Sets the media options to use.
    virtual bool SetOptions(const AudioOptions& options)  {return true;}
    virtual bool GetOptions(AudioOptions* options) const  {return true;}
    virtual bool SetRecvRtpHeaderExtensions(const std::vector<cricket::RtpHeaderExtension>&){return true;}
    virtual bool SetSendRtpHeaderExtensions(const std::vector<cricket::RtpHeaderExtension>&){return true;}

    // Sets the codecs/payload types to be used for incoming media.
    virtual bool SetRecvCodecs(const std::vector<AudioCodec>& codecs);
    // Sets the codecs/payload types to be used for outgoing media.
    virtual bool SetSendCodecs(const std::vector<AudioCodec>& codecs);

    // Starts or stops playout of received audio.
    virtual bool SetPlayout(bool playout);
    // Starts or stops sending (and potentially capture) of local audio.
    virtual bool SetSend(cricket::SendFlags flag);
    virtual bool SetRenderer(uint32, cricket::AudioRenderer*){return true;}
    virtual void OnReadyToSend(bool flag);
    
    // stream create and removed
    virtual bool AddSendStream(const cricket::StreamParams&);
    virtual bool RemoveSendStream(uint32);
    virtual bool AddRecvStream(const cricket::StreamParams&);
    virtual bool RemoveRecvStream(uint32);
    virtual bool MuteStream(uint32, bool);
    virtual bool SetSendBandwidth(bool, int){return true;}

    // data input 
    virtual void OnPacketReceived(talk_base::Buffer*);
    virtual void OnRtcpReceived(talk_base::Buffer*);
    
    /******************* Advanced feature don't needed in app ************/
    // Gets current energy levels for all incoming streams.
    virtual bool GetActiveStreams(AudioInfo::StreamList* actives)  {return true;}
    // Get the current energy level of the stream sent to the speaker.
    virtual int GetOutputLevel()  {return 1;}
    // Get the time in milliseconds since last recorded keystroke, or negative.
    virtual int GetTimeSinceLastTyping()  {return 0;}
    // Temporarily exposed field for tuning typing detect options.
    virtual void SetTypingDetectionParameters(int time_window,
        int cost_per_typing, int reporting_threshold, int penalty_decay,
        int type_event_delay)  {}
    // Set left and right scale for speaker output volume of the specified ssrc.
    virtual bool SetOutputScaling(uint32 ssrc, double left, double right)  {return true;}
    // Get left and right scale for speaker output volume of the specified ssrc.
    virtual bool GetOutputScaling(uint32 ssrc, double* left, double* right)  {return true;}
    // Specifies a ringback tone to be played during call setup.
    virtual bool SetRingbackTone(const char *buf, int len)  {return true;}
    // Plays or stops the aforementioned ringback tone
    virtual bool PlayRingbackTone(uint32 ssrc, bool play, bool loop)  {return true;}
    // Returns if the telephone-event has been negotiated.
    virtual bool CanInsertDtmf() { return false; }
    // Send and/or play a DTMF |event| according to the |flags|.
    virtual bool InsertDtmf(uint32 ssrc, int event, int duration, int flags)  {return true;}
    // Gets quality stats for the channel.
    virtual bool GetStats(VoiceMediaInfo* info)  {return true;}
    // Gets last reported error for this media channel.
    virtual void GetLastMediaError(uint32* ssrc, VoiceMediaChannel::Error* error);

protected:
  void OnSendPacket(SimpleVoiceEngine *, const void *data, int len) ;
  void OnSendRTCPPacket(SimpleVoiceEngine *, const void *data, int len) ;  

private:
    SimpleVoiceEngine* engine_;
    unsigned int target_ssrc_;         
};

	
}  // namespace webrtc

#endif
