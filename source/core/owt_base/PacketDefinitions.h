/*
 * Add proxy in rtcConn wrapper layer (or create a wrapper for libwebrtc.a)
 */

namespace owt_base {

class FeedbackSink {
 public:
    virtual ~FeedbackSink() {}
    virtual int deliverFeedback(char* data, int len) = 0;
};

class FeedbackSource {
 protected:
    FeedbackSink* fb_sink_;
 public:
    FeedbackSource(): fb_sink_{nullptr} {}
    virtual ~FeedbackSource() {}
    void setFeedbackSink(FeedbackSink* sink) {
        fb_sink_ = sink;
    }
};

/*
 * A PacketSink
 */
class PacketSink {
 protected:
    // Is it able to provide Feedback
    FeedbackSource* sink_fb_source_;
 public:
    PacketSink() : sink_fb_source_{nullptr} {}
    virtual ~PacketSink() {}
    FeedbackSource* getFeedbackSource() { return sink_fb_source_; }
    virtual void setAudioSinkSSRC(uint32_t ssrc) {}
    virtual void setVideoSinkSSRC(uint32_t ssrc) {}
    virtual int deliverAudioData(char* data, int len) = 0;
    virtual int deliverVideoData(char* data, int len) = 0;
    virtual void close() = 0;
};

/**
 * A MediaSource is any class that produces audio or video data.
 */
class PacketSource {
 protected:
    PacketSink* sink_;
    // can it accept feedback
    FeedbackSink* source_fb_sink_;
 public:
    PacketSource() : sink_{nullptr}, source_fb_sink_{nullptr} {}
    virtual ~PacketSource() {}
    virtual void setPacketSink(PacketSink* packet_sink) = 0;
    virtual FeedbackSink* getFeedbackSink() = 0;
    virtual void close() = 0;
};

} // namespace owt_base
