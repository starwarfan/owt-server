// Copyright (C) <2020> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0

#ifndef RTC_ADAPTOR_RTC_ADAPTOR_H_
#define RTC_ADAPTOR_RTC_ADAPTOR_H_

#include <MediaFramePipeline.h>

namespace rtc_adaptor {

class AdaptorDataListener {
public:
    virtual void onAdaptorData(char* data, int len) = 0;
};

class AdaptorFrameListener {
public:
    virtual void onAdaptorFrame(const Frame& frame) = 0;
};

class AdaptorFeedbackListener {
    virtual void onFeedback(const FeedbackMsg& msg) = 0;
};

struct AdaptorStats {
    int width;
    int height;
    owt_base::FrameFormat format;
    int estimatedBandwidth;
};

class AdaptorStatsListener {
public:
    virtual void onAdaptorStats(const AdaptorStats& stat) = 0;
};

class VideoReceiveAdaptor {
public:
    virtual int onRtpData(char* data, int len) = 0;
    virtual void requestKeyFrame() = 0;
};

class VideoSendAdaptor {
public:
    virtual void onFrame(const owt_base::Frame&) = 0;
    virtual int onRtcpData(char* data, int len) = 0;
};

class AudioReceiveAdaptor {
public:
    virtual int onRtpData(char* data, int len) = 0;
};

class AudioSendAdaptor {
public:
    virtual int onFrame(char* data, int len) = 0;
    virtual int onRtcpData(char* data, int len) = 0;
};

class RtcAdaptor {
public:
    struct Config {
        // SSRC of target stream
        uint32_t ssrc = 0;
        // Transport-cc externsion ID
        int transport_cc = 0;
        int red_payload = 0;
        int ulpfec_payload = 0;
        AdaptorDataListener* rtp_listener = nullptr;
        AdaptorStatsListener* stats_listener = nullptr;
        AdaptorFrameListener* frame_listener = nullptr;
        AdaptorFeedbackListener* feedback_listener = nullptr;
    };
    virtual VideoReceiveAdaptor* createVideoReceiver(const Config&) = 0;
    virtual void destoryVideoReceiver(VideoReceiveAdaptor*) = 0;
    virtual VideoSendAdaptor* createVideoSender(const Config&) = 0;
    virtual void destoryVideoSender(VideoSendAdaptor*) = 0;

    virtual AudioReceiveAdaptor* createAudioReceiver(const Config&) = 0;
    virtual void destoryAudioReceiver(AudioReceiveAdaptor*) = 0;
    virtual AudioSendAdaptor* createAudioSender(const Config&) = 0;
    virtual void destoryAudioSender(AudioSendAdaptor*) = 0;
};

class RtcAdaptorFactory {
public:
    static RtcAdaptor* CreateRtcAdaptor();
    // Use delete instead of this function
    static void DestroyRtcAdaptor(RtcAdaptor*);
};

} // namespace rtc_adaptor

#endif
