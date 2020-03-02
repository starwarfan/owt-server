#ifndef OWT_BASE_MEDIA_WRAPPER_H_
#define OWT_BASE_MEDIA_WRAPPER_H_

#include <nan.h>
#include <PacketDefinitions.h>

/*
 * Wrapper class of owt_base::PacketSink
 */
class MediaSink : public Nan::ObjectWrap {
 public:
    owt_base::PacketSink* msink;
};

/*
 * Wrapper class of owt_base::PacketSource
 */
class MediaSource : public Nan::ObjectWrap {
 public:
    owt_base::PacketSource* msource;
};

/*
 * Wrapper class of both owt_base::PacketSink and owt_base::PacketSource
 */
class MediaFilter : public Nan::ObjectWrap {
 public:
    owt_base::PacketSink* msink;
    owt_base::PacketSource* msource;
};

#endif  // OWT_BASE_MEDIA_WRAPPER_H_
