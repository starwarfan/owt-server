// Copyright (C) <2020> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0

#ifndef RTC_ADAPTOR_VIDEO_RECEIVE_ADAPTOR_
#define RTC_ADAPTOR_VIDEO_RECEIVE_ADAPTOR_

#include <RtcAdaptor.h>
#include <AdaptorInternalDefinitions.h>

#include <rtc_base/task_queue.h>
#include <call/call.h>
#include <api/task_queue/default_task_queue_factory.h>
#include <api/video_codecs/video_codec.h>
#include <api/video_codecs/video_decoder.h>
#include <api/video_codecs/video_decoder_factory.h>

namespace rtc_adaptor {

class VideoReceiveAdaptorImpl : public VideoReceiveAdaptor,
                                public rtc::VideoSinkInterface<webrtc::VideoFrame>,
                                public webrtc::VideoDecoderFactory,
                                public webrtc::Transport {
public:
    VideoReceiveAdaptorImpl(CallOwner* owner,  const RtcAdaptor::Config& config);

    // Implement VideoReceiveAdaptor
    int onRtpData(char* data, int len) override;
    void requestKeyFrame() override;

    // Implements rtc::VideoSinkInterface<VideoFrame>.
    void OnFrame(const webrtc::VideoFrame& video_frame) override;

    // Implements the webrtc::VideoDecoderFactory interface.
    std::vector<webrtc::SdpVideoFormat> GetSupportedFormats() const override;
    std::unique_ptr<webrtc::VideoDecoder> CreateVideoDecoder(
      const webrtc::SdpVideoFormat& format) override;

    // Implements webrtc::Transport
    bool SendRtp(const uint8_t* packet,
                 size_t length,
                 const webrtc::PacketOptions& options) override;
    bool SendRtcp(const uint8_t* packet, size_t length) override;

private:
    class AdaptorDecoder : public webrtc::VideoDecoder {
      public:
        AdaptorDecoder(VideoReceiveAdaptorImpl* parent): m_parent(parent) {}

        int32_t InitDecode(const webrtc::VideoCodec* config,
                           int32_t number_of_cores) override;

        int32_t Decode(const webrtc::EncodedImage& input,
                       bool missing_frames,
                       int64_t render_time_ms) override;

        int32_t RegisterDecodeCompleteCallback(
            webrtc::DecodedImageCallback* callback) override { return 0; }

        int32_t Release() override { return 0; }
      private:
        VideoReceiveAdaptorImpl* m_parent;
        webrtc::VideoCodecType m_codec;
        std::unique_ptr<uint8_t[]> m_frameBuffer;
        uint32_t m_bufferSize;
    };

    void CreateReceiveVideo();

    std::unique_ptr<webrtc::Call> call() {
        return m_owner ? m_owner->call() : nullptr;
    }
    std::unique_ptr<rtc::TaskQueue> taskQueue() {
        return m_owner ? m_owner->taskQueue() : nullptr;
    }

    bool m_enableDump;
    RtcAdaptor::Config m_config;
    // Video Statistics collected in decoder thread
    FrameFormat m_format;
    uint16_t m_width;
    uint16_t m_height;
    // Listeners
    AdaptorFrameListener* m_frameListener;
    AdaptorDataLisenter* m_rtcpListener;
    AdaptorStatsListener* m_statsListener;

    std::atomic<bool> m_isWaitingKeyFrame;
    CallOwner* m_owner;

    webrtc::VideoReceiveStream* m_videoRecvStream = nullptr;
};

} // namespace rtc_adaptor
