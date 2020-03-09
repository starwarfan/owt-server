// Copyright (C) <2019> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0

#ifndef VideoFramePacketizer_h
#define VideoFramePacketizer_h

#include "WebRTCTaskRunner.h"
#include "WebRTCTransport.h"
#include "MediaFramePipeline.h"
#include "SsrcGenerator.h"

#include <MediaDefinitions.h>
#include <MediaDefinitionExtra.h>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <logger.h>

#include <RtcAdaptor.h>

namespace owt_base {
/**
 * This is the class to accept the encoded frame with the given format,
 * packetize the frame and send them out via the given WebRTCTransport.
 * It also gives the feedback to the encoder based on the feedback from the remote.
 */
class VideoFramePacketizer : public FrameDestination,
                             public erizo::MediaSource,
                             public erizo::FeedbackSink,
                             public erizoExtra::RTPDataReceiver,
                             public rtc_adaptor::AdaptorFeedbackListener,
                             public rtc_adaptor::AdaptorStatsListener,
                             public rtc_adaptor::AdaptorDataListener {
    DECLARE_LOGGER();

public:
    VideoFramePacketizer(bool enableRed,
                         bool enableUlpfec,
                         bool enableTransportcc = true,
                         bool selfRequestKeyframe = false,
                         uint32_t transportccExt = 0);
    ~VideoFramePacketizer();

    void bindTransport(erizo::MediaSink* sink);
    void unbindTransport();
    void enable(bool enabled);
    uint32_t getSsrc() { return m_ssrc; }

    // Implements FrameDestination.
    void onFrame(const Frame&);
    void onVideoSourceChanged() override;

    // Implements erizo::MediaSource.
    int sendFirPacket();

    // Implements RTPDataReceiver.
    void receiveRtpData(char*, int len, erizoExtra::DataType, uint32_t channelId);

    // Implements the AdaptorFeedbackListener interfaces.
    void onFeedback(const FeedbackMsg& msg) = 0;
    // Implements the AdaptorStatsListener interfaces.
    void onAdaptorStats(const AdaptorStats& stats) override;
    // Implements the AdaptorDataListener interfaces.
    void onAdaptorData(char* data, int len) override;

private:
    bool init(bool enableRed, bool enableUlpfec, bool enableTransportcc, uint32_t transportccExt);
    void close();

    // Implement erizo::FeedbackSink
    int deliverFeedback_(std::shared_ptr<erizo::DataPacket> data_packet);
    // Implement erizo::MediaSource
    int sendPLI();

    bool m_enabled;
    bool m_enableDump;
    bool m_keyFrameArrived;
    bool m_selfRequestKeyframe;

    FrameFormat m_frameFormat;
    uint16_t m_frameWidth;
    uint16_t m_frameHeight;

    boost::shared_mutex m_transportMutex;

    uint16_t m_sendFrameCount;
    int64_t m_timeStampOffset;

    std::shared_ptr<rtc_adaptor::RtcAdaptor> m_rtcAdaptor;
    rtc_adaptor::VideoSendAdaptor* m_videoSend;
};

}
#endif /* EncodedVideoFrameSender_h */
