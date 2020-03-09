// Copyright (C) <2019> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0

#include "AudioFramePacketizer.h"
#include "AudioUtilitiesNew.h"
#include "WebRTCTaskRunner.h"

#include <rtc_base/logging.h>

using namespace webrtc;

namespace rtc_adaptor {

AudioSendAdaptorImpl::AudioSendAdaptorImpl(CallOwner* owner,  const RtcAdaptor::Config& config)
    : m_enabled(true)
    , m_frameFormat(FRAME_FORMAT_UNKNOWN)
    , m_lastOriginSeqNo(0)
    , m_seqNo(0)
    , m_ssrc(0)
    , m_ssrc_generator(SsrcGenerator::GetSsrcGenerator())
    , m_rtpListener(config.rtp_listener)
    , m_statsListener(config.stats_listener)
{
    audio_sink_ = nullptr;
    m_ssrc = m_ssrc_generator->CreateSsrc();
    m_ssrc_generator->RegisterSsrc(m_ssrc);
    m_taskRunner.reset(new owt_base::WebRTCTaskRunner("AudioFramePacketizer"));
    m_taskRunner->Start();
    init();
}

AudioSendAdaptorImpl::~AudioSendAdaptorImpl() {
    close();
    m_taskRunner->Stop();
    m_ssrc_generator->ReturnSsrc(m_ssrc);
    boost::unique_lock<boost::shared_mutex> lock(m_rtpRtcpMutex);
}

int AudioSendAdaptorImpl::onRtpFeedback(char* data, int len) {
    boost::shared_lock<boost::shared_mutex> lock(m_rtpRtcpMutex);
    m_rtpRtcp->IncomingRtcpPacket(reinterpret_cast<uint8_t*>(data), length);
    return length;
}

void AudioFramePacketizer::onFrame(const Frame& frame) {
    if (frame.length <= 0)
        return;

    if (frame.format != m_frameFormat) {
        m_frameFormat = frame.format;
        setSendCodec(m_frameFormat);
    }

    if (frame.additionalInfo.audio.isRtpPacket) {
        // FIXME: Temporarily use Frame to carry rtp-packets
        // due to the premature AudioFrameConstructor implementation.
        updateSeqNo(frame.payload);
        if (m_rtpListener) {
            m_rtpListener->onAdaptorData(reinterpret_cast<char*>(frame.payload), frame.length);
        }
        return;
    }

    int payloadType = getAudioPltype(frame.format);
    if (payloadType == INVALID_PT)
        return;

    boost::shared_lock<boost::shared_mutex> lock(m_rtpRtcpMutex);
    if (!m_rtpRtcp->OnSendingRtpFrame(frame.timeStamp, -1, payloadType, false)) {
        RTC_DLOG(LS_WARNING) << "OnSendingRtpFrame return false";
        return;
    }
    const uint32_t rtp_timestamp = frame.timeStamp + m_rtpRtcp->StartTimestamp();
    // TODO: The frame type information is lost. We treat every frame a kAudioFrameSpeech frame for now.
    if (!m_senderAudio->SendAudio(webrtc::AudioFrameType::kAudioFrameSpeech, payloadType, rtp_timestamp,
                                  frame.payload, frame.length)) {
        RTC_DLOG(LS_ERROR) << "ChannelSend::SendData() failed to send data to RTP/RTCP module";
    }
}

bool AudioFramePacketizer::init() {
    m_clock = Clock::GetRealTimeClock();
    m_eventLog = std::make_unique<webrtc::RtcEventLogNull>();

    RtpRtcp::Configuration configuration;
    configuration.clock = m_clock;
    configuration.audio = true;  // Audio.
    configuration.receiver_only = false;
    configuration.outgoing_transport = this;
    configuration.event_log = m_eventLog.get();
    configuration.local_media_ssrc = m_ssrc;//rtp_config.ssrcs[i];

    m_rtpRtcp = RtpRtcp::Create(configuration);
    m_rtpRtcp->SetSendingStatus(true);
    m_rtpRtcp->SetSendingMediaStatus(true);
    m_rtpRtcp->SetRTCPStatus(RtcpMode::kReducedSize);
    // Set NACK.
    m_rtpRtcp->SetStorePacketsStatus(true, 600);

    m_senderAudio = std::make_unique<RTPSenderAudio>(
        configuration.clock, m_rtpRtcp->RtpSender());
    m_taskRunner->RegisterModule(m_rtpRtcp.get());

    return true;
}

bool AudioFramePacketizer::setSendCodec(FrameFormat format) {
    CodecInst codec;

    if (!getAudioCodecInst(m_frameFormat, codec))
        return false;

    boost::shared_lock<boost::shared_mutex> lock(m_rtpRtcpMutex);
    m_rtpRtcp->RegisterSendPayloadFrequency(codec.pltype, codec.plfreq);
    m_senderAudio->RegisterAudioPayload("audio", codec.pltype,
                                        codec.plfreq, codec.channels, 0);
    return true;
}

void AudioFramePacketizer::close()
{
    m_taskRunner->DeRegisterModule(m_rtpRtcp.get());
}

bool VideoSendAdaptorImpl::SendRtp(const uint8_t* packet,
             size_t length,
             const webrtc::PacketOptions& options) {
    if (m_rtpListener) {
        m_rtpListener->onAdaptorData(reinterpret_cast<char*>(data), len);
        return true;
    }
    return false;
}

bool VideoSendAdaptorImpl::SendRtcp(const uint8_t* packet, size_t length) {
    const RTCPHeader* chead = reinterpret_cast<const RTCPHeader*>(data);
    uint8_t packetType = chead->getPacketType();
    if (packetType == RTCP_Sender_PT) {
        if (m_rtpListener) {
            m_rtpListener->onAdaptorData(reinterpret_cast<char*>(data), len);
            return true;
        }
    }
    return false;
}


void AudioFramePacketizer::updateSeqNo(uint8_t* rtp) {
    uint16_t originSeqNo = *(reinterpret_cast<uint16_t*>(&rtp[2]));
    if ((m_lastOriginSeqNo == m_seqNo) && (m_seqNo == 0)) {
    } else {
        uint16_t step = ((originSeqNo < m_lastOriginSeqNo) ? (UINT16_MAX - m_lastOriginSeqNo + originSeqNo + 1) : (originSeqNo - m_lastOriginSeqNo));
        if ((step == 1) || (step > 10)) {
          m_seqNo += 1;
        } else {
          m_seqNo += step;
        }
    }
    m_lastOriginSeqNo = originSeqNo;
    *(reinterpret_cast<uint16_t*>(&rtp[2])) = htons(m_seqNo);
}


}
