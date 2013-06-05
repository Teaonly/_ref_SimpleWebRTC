#ifndef _CAMERAMEDIAENGINE_H_
#define _CAMERAMEDIAENGINE_H_

#include <string>
#include <vector>

#include "talk/base/scoped_ptr.h"
#include "talk/media/base/codec.h"
#include "talk/media/base/mediachannel.h"
#include "talk/media/base/mediaengine.h"


namespace talk_base {
    class StreamInterface;
    class Thread;
}

namespace cricket {

class SimpleMediaEngine : public MediaEngineInterface {
public:
    SimpleMediaEngine(talk_base::Thread* worker_thread);
    virtual ~SimpleMediaEngine();

    // Initialization
    // Starts the engine.
    virtual bool Init(talk_base::Thread* worker_thread);
    // Shuts down the engine.
    virtual void Terminate();
    // Returns what the engine is capable of, as a set of Capabilities, above.
    virtual int GetCapabilities();

    // MediaChannel creation
    // Creates a voice media channel. Returns NULL on failure.
    virtual VoiceMediaChannel *CreateChannel();
    // Creates a video media channel, paired with the specified voice channel.
    // Returns NULL on failure.
    virtual VideoMediaChannel *CreateVideoChannel(
            VoiceMediaChannel* voice_media_channel);

    // Creates a soundclip object for playing sounds on. Returns NULL on failure.
    virtual SoundclipMedia *CreateSoundclip();

    // Configuration
    // Sets global audio options. "options" are from AudioOptions, above.
    virtual bool SetAudioOptions(int options);
    // Sets global video options. "options" are from VideoOptions, above.
    virtual bool SetVideoOptions(int options);
    // Sets the value used by the echo canceller to offset delay values obtained
    // from the OS.
    virtual bool SetAudioDelayOffset(int offset);
    // Sets the default (maximum) codec/resolution and encoder option to capture
    // and encode video.
    virtual bool SetDefaultVideoEncoderConfig(const VideoEncoderConfig& config)
       ;

    // Device selection
    // TODO(tschmelcher): Add method for selecting the soundclip device.
    virtual bool SetSoundDevices(const Device* in_device,
            const Device* out_device);
    // Sets the externally provided video capturer. The ssrc is the ssrc of the
    // (video) stream for which the video capturer should be set.
    virtual bool SetVideoCapturer(VideoCapturer* capturer);
    virtual VideoCapturer* GetVideoCapturer() const;

    // Device configuration
    // Gets the current speaker volume, as a value between 0 and 255.
    virtual bool GetOutputVolume(int* level);
    // Sets the current speaker volume, as a value between 0 and 255.
    virtual bool SetOutputVolume(int level);  


    // Local monitoring
    // Gets the current microphone level, as a value between 0 and 10.
    virtual int GetInputLevel();
    // Starts or stops the local microphone. Useful if local mic info is needed
    // prior to a call being connected; the mic will be started automatically
    // when a VoiceMediaChannel starts sending.
    virtual bool SetLocalMonitor(bool enable);
    // Installs a callback for raw frames from the local camera.
    virtual bool SetLocalRenderer(VideoRenderer* renderer);
    // Starts/stops local camera.
    virtual bool SetVideoCapture(bool capture);

    virtual const std::vector<RtpHeaderExtension>&
      audio_rtp_header_extensions();
    virtual const std::vector<RtpHeaderExtension>&
      video_rtp_header_extensions();
    virtual const std::vector<AudioCodec>& audio_codecs();
    virtual const std::vector<VideoCodec>& video_codecs();

    // Logging control
    virtual void SetVoiceLogging(int min_sev, const char* filter);
    virtual void SetVideoLogging(int min_sev, const char* filter);

    // media processors for effects
    virtual bool RegisterVideoProcessor(VideoProcessor* video_processor);
    virtual bool UnregisterVideoProcessor(VideoProcessor* video_processor);
    virtual bool RegisterVoiceProcessor(uint32 ssrc,
            VoiceProcessor* video_processor,
            MediaProcessorDirection direction);
    virtual bool UnregisterVoiceProcessor(uint32 ssrc,
            VoiceProcessor* video_processor,
            MediaProcessorDirection direction);

    virtual VideoFormat GetStartCaptureFormat() const;
    virtual sigslot::repeater2<VideoCapturer*, CaptureState>& SignalVideoCaptureStateChange() {
        return signal_state_change_;
    }

private:
    talk_base::Thread* worker_thread_;

    std::vector<VideoCodec> video_codecs_;
    std::vector<AudioCodec> audio_codecs_;    
    std::vector<RtpHeaderExtension> audio_rtp_header_extensions_;
    std::vector<RtpHeaderExtension> video_rtp_header_extensions_;
    sigslot::repeater2<VideoCapturer*, CaptureState> signal_state_change_;


}; 

}

#endif
