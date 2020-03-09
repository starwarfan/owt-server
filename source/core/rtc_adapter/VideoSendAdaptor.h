// Copyright (C) <2019> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0

#ifndef RTC_ADAPTOR_VIDEO_RECEIVE_ADAPTOR_
#define RTC_ADAPTOR_VIDEO_RECEIVE_ADAPTOR_

#include <RtcAdaptor.h>
#include <AdaptorInternalDefinitions.h>

#include "WebRTCTaskRunner.h"
#include "MediaFramePipeline.h"
#include "SsrcGenerator.h"

#include <modules/rtp_rtcp/include/rtp_rtcp.h>
#include <modules/rtp_rtcp/include/rtp_rtcp_defines.h>
#include <modules/rtp_rtcp/source/rtp_sender_video.h>
#include <api/transport/field_trial_based_config.h>

#include <rtc_base/random.h>
#include <rtc_base/time_utils.h>
#include <rtc_base/rate_limiter.h>

namespace rtc_adaptor {

class VideoSendAdaptorImpl : public VideoSendAdaptor,
                             public webrtc::Transport,
                             public webrtc::RtcpIntraFrameObserver {
public:
    VideoSendAdaptorImpl(CallOwner* owner,  const RtcAdaptor::Config& config);
    ~VideoSendAdaptorImpl();

    // Implement VideoSendAdaptor
    void onFrame(const owt_base::Frame&) override;
    int onRtcpData(char* data, int len) override;

    // Implement webrtc::Transport
    bool SendRtp(const uint8_t* packet,
                 size_t length,
                 const webrtc::PacketOptions& options) override;
    bool SendRtcp(const uint8_t* packet, size_t length) override;

    // Implements webrtc::RtcpIntraFrameObserver.
    void OnReceivedIntraFrameRequest(uint32_t ssrc);
    void OnReceivedSLI(uint32_t ssrc, uint8_t picture_id) { }
    void OnReceivedRPSI(uint32_t ssrc, uint64_t picture_id) { }
    void OnLocalSsrcChanged(uint32_t old_ssrc, uint32_t new_ssrc) { }

private:
    bool init();
    void reset();

    bool m_enableDump;
    RtcAdaptor::Config m_config;

    bool m_keyFrameArrived;
    std::unique_ptr<webrtc::RateLimiter> m_retransmissionRateLimiter;
    // boost::scoped_ptr<webrtc::BitrateController> m_bitrateController;
    boost::scoped_ptr<webrtc::RtcpBandwidthObserver> m_bandwidthObserver;
    std::unique_ptr<webrtc::RtpRtcp> m_rtpRtcp;
    boost::shared_mutex m_rtpRtcpMutex;

    boost::shared_ptr<webrtc::Transport> m_videoTransport;
    boost::shared_ptr<WebRTCTaskRunner> m_taskRunner;
    FrameFormat m_frameFormat;
    uint16_t m_frameWidth;
    uint16_t m_frameHeight;
    webrtc::Random m_random;
    uint32_t m_ssrc;
    SsrcGenerator* const m_ssrcGenerator;

    boost::shared_mutex m_transportMutex;

    webrtc::Clock *m_clock;
    int64_t m_timeStampOffset;

    std::unique_ptr<webrtc::RtcEventLog> m_eventLog;
    std::unique_ptr<webrtc::RTPSenderVideo> m_senderVideo;
    std::unique_ptr<webrtc::PlayoutDelayOracle> m_playoutDelayOracle;
    std::unique_ptr<webrtc::FieldTrialBasedConfig> m_fieldTrialConfig;

    // Listeners
    AdaptorFeedbackListener* m_feedbackListener;
    AdaptorDataLisenter* m_rtpListener;
    AdaptorStatsListener* m_statsListener;
};

}
#endif /* EncodedVideoFrameSender_h */
