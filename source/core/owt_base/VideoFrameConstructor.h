// Copyright (C) <2019> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0

#ifndef VideoFrameConstructor_h
#define VideoFrameConstructor_h

#include "MediaFramePipeline.h"
#include "WebRTCTransport.h"

#include <boost/scoped_ptr.hpp>
#include <MediaDefinitions.h>
#include <MediaDefinitionExtra.h>

#include <JobTimer.h>

#include <RtcAdaptor.h>

namespace owt_base {

class VideoInfoListener {
public:
    virtual ~VideoInfoListener(){};
    virtual void onVideoInfo(const std::string& videoInfoJSON) = 0;
};

/**
 * A class to process the incoming streams by leveraging video coding module from
 * webrtc engine, which will framize the frames.
 */
class VideoFrameConstructor : public erizo::MediaSink,
                              public erizo::FeedbackSource,
                              public FrameSource,
                              public JobTimerListener,
                              public rtc_adaptor::AdaptorFrameListener,
                              public rtc_adaptor::AdaptorStatsListener,
                              public rtc_adaptor::AdaptorDataListener {

public:
    struct Config {
        uint32_t transport_cc = 0;
    };

    VideoFrameConstructor(VideoInfoListener*, uint32_t transportccExtId = 0);
    VideoFrameConstructor(VideoFrameConstructor*, VideoInfoListener*, uint32_t transportccExtId = 0);
    virtual ~VideoFrameConstructor();

    void bindTransport(erizo::MediaSource* source, erizo::FeedbackSink* fbSink);
    void unbindTransport();
    void enable(bool enabled);

    // Implements the JobTimerListener.
    void onTimeout();

    // Implements the FrameSource interfaces.
    void onFeedback(const FeedbackMsg& msg) override;

    // Implements the AdaptorFrameListener interfaces.
    void onAdaptorFrame(const Frame& frame) override;
    // Implements the AdaptorStatsListener interfaces.
    void onAdaptorStats(const AdaptorStats& stats) override;
    // Implements the AdaptorDataListener interfaces.
    void onAdaptorData(char* data, int len) override;

    int32_t RequestKeyFrame();

    bool setBitrate(uint32_t kbps);

private:
    Config m_config;

    // Implement erizo::MediaSink
    int deliverAudioData_(std::shared_ptr<erizo::DataPacket> audio_packet) override;
    int deliverVideoData_(std::shared_ptr<erizo::DataPacket> video_packet) override;
    int deliverEvent_(erizo::MediaEventPtr event) override;
    void close();

    bool m_enabled;
    bool m_enableDump;
    uint32_t m_ssrc;

    erizo::MediaSource* m_transport;
    boost::shared_mutex m_transportMutex;
    boost::scoped_ptr<JobTimer> m_feedbackTimer;
    uint32_t m_pendingKeyFrameRequests;

    VideoInfoListener* m_videoInfoListener;
    std::unique_ptr<WebRTCTransport<erizoExtra::VIDEO>> m_videoTransport;

    // Temporary buffer for dump
    char buf[1500];

    std::shared_ptr<rtc_adaptor::RtcAdaptor> m_rtcAdaptor;
    rtc_adaptor::VideoReceiveAdaptor* m_videoReceive;
};

} // namespace owt_base

#endif /* VideoFrameConstructor_h */
