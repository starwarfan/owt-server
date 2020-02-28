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

struct AdaptorStats {
    int width;
    int height;
    owt_base::FrameFormat format;
    int estimatedBandwidth;
};

class AdaptorStatsListener {
public:
    virtual void onAdaptorStat(const AdaptorStats& stat) = 0;
};

class VideoReceiveAdaptor {
public:
    virtual int onRtpData(char* data, int len) = 0;
    virtual void requestKeyFrame() = 0;
    virtual void setFrameListener(AdaptorFrameListener*) = 0;
    virtual void setFeedbackListener(AdaptorDataLisenter*) = 0;
    virtual void setAdaptorStatsListener(AdaptorStatsListener*) = 0;
};

class VideoSendAdaptor {
public:
    virtual void onFrame(const owt_base::Frame&) = 0;
    virtual int onRtpFeedback(char* data, int len) = 0;
    virtual void setKeyFrameRequestListener() = 0;
    virtual void setRtpDataListener(AdaptorDataLisenter*) = 0;
    virtual void setAdaptorStatsListener(AdaptorStatsListener*) = 0;
};

class AudioReceiveAdaptor {
public:
    virtual int onRtpData(char* data, int len) = 0;
    virtual void setFrameListener(AdaptorFrameListener*) = 0;
};

class AudioSendAdaptor {
public:
    virtual int onFrame(char* data, int len) = 0;
    virtual int onRtpFeedback(char* data, int len) = 0;
    virtual void setRtpDataListener(AdaptorFrameListener*) = 0;
};

class RtcAdaptor {
public:
    virtual VideoReceiveAdaptor* createVideoReceiver() = 0;
    virtual void destoryVideoReceiver(VideoReceiveAdaptor*) = 0;
    virtual VideoSendAdaptor* createVideoSender() = 0;
    virtual void destoryVideoSender(VideoSendAdaptor*) = 0;

    virtual AudioReceiveAdaptor* createAudioReceiver() = 0;
    virtual void destoryAudioReceiver(AudioReceiveAdaptor*) = 0;
    virtual AudioSendAdaptor* createAudioSender() = 0;
    virtual void destoryAudioSender(AudioSendAdaptor*) = 0;
};

class RtcAdaptorFactory {
public:
    static RtcAdaptor* CreateRtcAdaptor();
};

} // namespace rtc_adaptor

#endif
