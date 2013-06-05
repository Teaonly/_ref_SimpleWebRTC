#ifndef _CAMERAVOICEENGINE_H_
#define _CAMERAVOICEENGINE_H_

#include <string>
#include <vector>

#include "talk/base/sigslotrepeater.h"
#include "talk/base/stream.h"
#include "talk/media/base/rtputils.h"
#include "talk/media/webrtc/webrtccommon.h"
#include "talk/media/webrtc/webrtcvoe.h"
#include "talk/session/media/channel.h"
#include "talk/media/base/mediachannel.h"

namespace cricket {
class VoiceMediaChannel;
class SoundclipMedia;

class SimpleVoiceMediaEngine {
    // TODO 
};

class SimpleVoiceMediaChannel: public VoiceMediaChannel {
public:
  SimpleVoiceMediaChannel() {}
  virtual ~SimpleVoiceMediaChannel() {}
  
  // Sets the codecs/payload types to be used for incoming media.
  virtual bool SetRecvCodecs(const std::vector<AudioCodec>& codecs) {
      //AudioCodec default_code(9, "G722", 16000, 0, 1, 0);
      //codecs.push_back(default_code);
    return true;
  }

  // Sets the codecs/payload types to be used for outgoing media.
  virtual bool SetSendCodecs(const std::vector<AudioCodec>& codecs)  {
  //AudioCodec default_code(9, "G722", 16000, 0, 1, 0);
  //codecs.push_back(default_code);
  return true;
  }
  // Starts or stops playout of received audio.
  virtual bool SetPlayout(bool playout)  {return true;}
  // Starts or stops sending (and potentially capture) of local audio.
  virtual bool SetSend(cricket::SendFlags flag) {return true;}
  virtual void OnReadyToSend(bool flag)  {}
  // Gets current energy levels for all incoming streams.
  virtual bool GetActiveStreams(AudioInfo::StreamList* actives)  {return true;}
  // Get the current energy level of the stream sent to the speaker.
  virtual int GetOutputLevel()  {return 1;}
  // Get the time in milliseconds since last recorded keystroke, or negative.
  virtual int GetTimeSinceLastTyping()  {return 1;}
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
  // The DTMF out-of-band signal will be used on sending.
  // The |ssrc| should be either 0 or a valid send stream ssrc.
  // The valid value for the |event| are -2 to 15.
  // kDtmfReset(-2) is used to reset the DTMF.
  // kDtmfDelay(-1) is used to insert a delay to the end of the DTMF queue.
  // 0 to 15 which corresponding to DTMF event 0-9, *, #, A-D.
  virtual bool InsertDtmf(uint32 ssrc, int event, int duration, int flags)  {return true;}
  // Gets quality stats for the channel.
  virtual bool GetStats(VoiceMediaInfo* info)  {return true;}
  // Gets last reported error for this media channel.
  virtual void GetLastMediaError(uint32* ssrc,
                                 VoiceMediaChannel::Error* error) {
    ASSERT(error != NULL);
    *error = ERROR_NONE;
  }
  // Sets the media options to use.
  virtual bool SetOptions(const AudioOptions& options)  {return true;}
  virtual bool GetOptions(AudioOptions* options) const  {return true;}
  
  virtual void OnPacketReceived(talk_base::Buffer*) {}
  virtual void OnRtcpReceived(talk_base::Buffer*){}
  virtual bool AddSendStream(const cricket::StreamParams&){return true;}
  virtual bool RemoveSendStream(uint32){return true;}
  virtual bool AddRecvStream(const cricket::StreamParams&){return true;}
  virtual bool RemoveRecvStream(uint32){return true;}
  virtual bool MuteStream(uint32, bool){return true;}
  virtual bool SetRecvRtpHeaderExtensions(const std::vector<cricket::RtpHeaderExtension>&){return true;}
  virtual bool SetSendRtpHeaderExtensions(const std::vector<cricket::RtpHeaderExtension>&){return true;}
  virtual bool SetSendBandwidth(bool, int){return true;}
};

	
}  // namespace webrtc

#endif
