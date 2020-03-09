// Copyright (C) <2019> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0

#ifndef AudioFramePacketizer_h
#define AudioFramePacketizer_h

#include "MediaFramePipeline.h"
#include "WebRTCTransport.h"
#include "SsrcGenerator.h"

#include <logger.h>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <MediaDefinitions.h>
#include <MediaDefinitionExtra.h>

#include <RtcAdaptor.h>

namespace owt_base {

/**
 * This is the class to send out the audio frame with a given format.
 */
class AudioFramePacketizer : public FrameDestination,
                             public erizo::MediaSource,
                             public erizo::FeedbackSink,
                             public erizoExtra::RTPDataReceiver,
                             public rtc_adaptor::AdaptorStatsListener,
                             public rtc_adaptor::AdaptorDataListener {
    DECLARE_LOGGER();

public:
    AudioFramePacketizer();
    ~AudioFramePacketizer();

    void bindTransport(erizo::MediaSink* sink);
    void unbindTransport();
    void enable(bool enabled) {m_enabled = enabled;}
    uint32_t getSsrc() { return m_ssrc; }

    // Implements FrameDestination.
    void onFrame(const Frame&);

    // Implements RTPDataReceiver.
    void receiveRtpData(char*, int len, erizoExtra::DataType, uint32_t channelId);

    // Implements the AdaptorStatsListener interfaces.
    void onAdaptorStats(const AdaptorStats& stats) override;
    // Implements the AdaptorDataListener interfaces.
    void onAdaptorData(char* data, int len) override;

private:
    bool init();
    void close();

    // Implement erizo::FeedbackSink
    int deliverFeedback_(std::shared_ptr<erizo::DataPacket> data_packet);
    // Implement erizo::MediaSource
    int sendPLI();

    bool m_enabled;

    FrameFormat m_frameFormat;
    boost::shared_mutex m_transport_mutex;

    uint16_t m_lastOriginSeqNo;
    uint16_t m_seqNo;
    uint32_t m_ssrc;

    std::shared_ptr<rtc_adaptor::RtcAdaptor> m_rtcAdaptor;
    rtc_adaptor::AudioSendAdaptor* m_audioSend;
};

}
#endif /* AudioFramePacketizer_h */
