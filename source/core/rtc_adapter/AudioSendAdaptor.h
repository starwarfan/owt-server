// Copyright (C) <2019> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0

#ifndef AudioFramePacketizer_h
#define AudioFramePacketizer_h

#include "MediaFramePipeline.h"
#include "WebRTCTransport.h"
#include "SsrcGenerator.h"

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <modules/rtp_rtcp/include/rtp_rtcp.h>
#include <modules/rtp_rtcp/source/rtp_sender_audio.h>
#include <api/rtc_event_log/rtc_event_log.h>

namespace rtc_adaptor {

/**
 * This is the class to send out the audio frame with a given format.
 */
class AudioSendAdaptorImpl : public AudioSendAdaptor,
                             public webrtc::Transport {
public:
    AudioSendAdaptorImpl(CallOwner* owner,  const RtcAdaptor::Config& config);
    ~AudioSendAdaptorImpl();

    // Implement AudioSendAdaptor
    int onFrame(char* data, int len) override;
    int onRtpFeedback(char* data, int len) override;

    // Implement webrtc::Transport
    bool SendRtp(const uint8_t* packet,
                 size_t length,
                 const webrtc::PacketOptions& options) override;
    bool SendRtcp(const uint8_t* packet, size_t length) override;

    uint32_t getSsrc() { return m_ssrc; }

private:
    bool init();
    bool setSendCodec(FrameFormat format);
    void close();
    void updateSeqNo(uint8_t* rtp);

    boost::shared_mutex m_rtpRtcpMutex;
    std::unique_ptr<webrtc::RtpRtcp> m_rtpRtcp;

    boost::shared_ptr<WebRTCTaskRunner> m_taskRunner;
    FrameFormat m_frameFormat;
    boost::shared_mutex m_transport_mutex;

    uint16_t m_lastOriginSeqNo;
    uint16_t m_seqNo;
    uint32_t m_ssrc;
    SsrcGenerator* const m_ssrc_generator;

    webrtc::Clock *m_clock;
    std::unique_ptr<webrtc::RtcEventLog> m_eventLog;
    std::unique_ptr<webrtc::RTPSenderAudio> m_senderAudio;

    RtcAdaptor::Config m_config;
    // Listeners
    AdaptorDataLisenter* m_rtpListener;
    AdaptorStatsListener* m_statsListener;
};

}
#endif /* AudioFramePacketizer_h */
