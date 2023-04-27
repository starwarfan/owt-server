// Copyright (C) <2019> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0

#ifndef INTERNAL_QUIC_H_
#define INTERNAL_QUIC_H_

#include "QuicTransport.h"
#include "MediaFramePipelineWrapper.h"
#include <nan.h>

/*
 * Wrapper class of QuicIn
 *
 * Receives media from one
 */
class InternalQuicIn : public FrameSource {
 public:
  static NAN_MODULE_INIT(Init);
  QuicIn* me;

 private:
  InternalQuicIn();
  ~InternalQuicIn();

  static Nan::Persistent<v8::Function> constructor;

  static NAN_METHOD(New);
  static NAN_METHOD(close);

  static NAN_METHOD(getListeningPort);
  static NAN_METHOD(addDestination);
  static NAN_METHOD(removeDestination);
};

/*
 * Wrapper class of QuicOut
 *
 * Send media from one
 */
class InternalQuicOut : public FrameDestination {
 public:
  static NAN_MODULE_INIT(Init);
  QuicOut* me;

 private:
  InternalQuicOut();
  ~InternalQuicOut();

  static Nan::Persistent<v8::Function> constructor;

  static NAN_METHOD(New);
  static NAN_METHOD(close);
};

#endif  // INTERNAL_QUIC_H_
