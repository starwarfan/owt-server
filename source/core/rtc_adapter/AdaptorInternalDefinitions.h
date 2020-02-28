// Copyright (C) <2020> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0

#ifndef RTC_ADAPTOR_ADAPTOR_INTERNAL_DEFINITIONS_H_
#define RTC_ADAPTOR_ADAPTOR_INTERNAL_DEFINITIONS_H_

namespace rtc_adaptor {

class CallOwner {
public:
    virtual std::unique_ptr<webrtc::Call> call() = 0;
    virtual std::unique_ptr<webrtc::TaskQueueFactory> taskQueueFactory() = 0;
    virtual std::unique_ptr<rtc::TaskQueue> taskQueue() = 0;
    virtual std::unique_ptr<webrtc::RtcEventLog> eventLog() = 0;
};

} // namespace rtc_adaptor

#endif