// Copyright (C) <2020> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0

#include <RtcAdaptor.h>
#include <AdaptorInternalDefinitions.h>
#include <VideoReceiveAdaptor.h>
#include <VideoSendAdaptor.h>
#include <AudioReceiveAdaptor.h>
#include <AudioSendAdaptor.h>

#include <memory>

namespace rtc_adaptor {

class RtcAdaptorImpl : public RtcAdaptor,
                       public CallOwner {
public:
    RtcAdaptorImpl();
    virtual ~RtcAdaptorImpl();

    // Implement RtcAdaptor
    VideoReceiveAdaptor* createVideoReceiver() override;
    void destoryVideoReceiver(VideoReceiveAdaptor*) override;
    VideoSendAdaptor* createVideoSender() override;
    void destoryVideoSender(VideoSendAdaptor*) override;
    AudioReceiveAdaptor* createAudioReceiver() override;
    void destoryAudioReceiver(AudioReceiveAdaptor*) override;
    AudioSendAdaptor* createAudioSender() override;
    void destoryAudioSender(AudioSendAdaptor*) override;

    // Implement CallOwner
    std::unique_ptr<webrtc::Call> call() override { return m_call; }
    std::unique_ptr<webrtc::TaskQueueFactory> taskQueueFactory() override {
        return m_taskQueueFactory;
    }
    std::unique_ptr<rtc::TaskQueue> taskQueue() override { return m_taskQueue; }
    std::unique_ptr<webrtc::RtcEventLog> eventLog() override { return m_eventLog; }

private:
    void initCall();

    std::unique_ptr<webrtc::TaskQueueFactory> m_taskQueueFactory;
    std::unique_ptr<rtc::TaskQueue> m_taskQueue;
    std::unique_ptr<webrtc::RtcEventLog> m_eventLog;
    std::unique_ptr<webrtc::Call> m_call;
};

RtcAdaptorImpl::RtcAdaptorImpl()
    : m_taskQueueFactory(webrtc::CreateDefaultTaskQueueFactory())
    , m_taskQueue(std::make_shared<rtc::TaskQueue>(m_taskQueueFactory->CreateTaskQueue(
        "CallTaskQueue",
        webrtc::TaskQueueFactory::Priority::NORMAL)))
    , m_eventLog(std::make_shared<webrtc::RtcEventLogNull>()) {

}

RtcAdaptorImpl::~RtcAdaptorImpl() {

}

void RtcAdaptorImpl::initCall() {
    m_taskQueue->PostTask([this]() {
        // Initialize call
        if (!m_call) {
            webrtc::Call::Config call_config(m_eventLog.get());
            call_config.task_queue_factory = m_taskQueueFactory.get();
            m_call.reset(webrtc::Call::Create(call_config));
        }
    });
}

VideoReceiveAdaptor* RtcAdaptorImpl::createVideoReceiver(const Config& config) {
    initCall();
    return new VideoReceiveAdaptorImpl(this, config);
}

void RtcAdaptorImpl::destoryVideoReceiver(VideoReceiveAdaptor* video_recv_adaptor) {
    delete video_recv_adaptor;
}

VideoSendAdaptor* RtcAdaptorImpl::createVideoSender(const Config& config)  {
    return new VideoSendAdaptorImpl(this, config);
}
void RtcAdaptorImpl::destoryVideoSender(VideoSendAdaptor* video_send_adaptor) {
    delete video_send_adaptor;
}

AudioReceiveAdaptor* RtcAdaptorImpl::createAudioReceiver(const Config& config) {
    return nullptr;
}
void RtcAdaptorImpl::destoryAudioReceiver(AudioReceiveAdaptor* audio_recv_adaptor) {}

AudioSendAdaptor* RtcAdaptorImpl::createAudioSender(const Config& config) {
    return new AudioSendAdaptorImpl(this, config);
}

void RtcAdaptorImpl::destoryAudioSender(AudioSendAdaptor* audio_send_adaptor) {
    delete audio_send_adaptor;
}

} // namespace rtc_adaptor
