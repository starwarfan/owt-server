// Copyright (C) <2019> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0

#include <string>
#include "InternalQuic.h"

using namespace v8;

// Class InternalQuicIn
Nan::Persistent<Function> InternalQuicIn::constructor;

InternalQuicIn::InternalQuicIn() {};
InternalQuicIn::~InternalQuicIn() {};

NAN_MODULE_INIT(InternalQuicIn::Init) {
  // Constructor template
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("InternalQuicIn").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  Nan::SetPrototypeMethod(tpl, "close", close);
  Nan::SetPrototypeMethod(tpl, "getListeningPort", getListeningPort);
  Nan::SetPrototypeMethod(tpl, "addDestination", addDestination);
  Nan::SetPrototypeMethod(tpl, "removeDestination", removeDestination);

  constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(target, Nan::New("in").ToLocalChecked(),
           Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(InternalQuicIn::New) {
  Nan::Utf8String param0(Nan::To<v8::String>(info[0]).ToLocalChecked());
  std::string cert_file = std::string(*param0);
  Nan::Utf8String param1(Nan::To<v8::String>(info[0]).ToLocalChecked());
  std::string key_file = std::string(*param1);

  InternalQuicIn* obj = new InternalQuicIn();
  obj->me = new QuicIn(cert_file, key_file);
  obj->src = obj->me;

  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(InternalQuicIn::close) {
  InternalQuicIn* obj = ObjectWrap::Unwrap<InternalQuicIn>(info.Holder());
  delete obj->me;
  obj->me = nullptr;
  obj->src = nullptr;
  obj->src = nullptr;
}

NAN_METHOD(InternalQuicIn::getListeningPort) {
  InternalQuicIn* obj = ObjectWrap::Unwrap<InternalQuicIn>(info.This());
  QuicIn* me = obj->me;

  uint32_t port = me->getListeningPort();

  info.GetReturnValue().Set(Nan::New(port));
}

NAN_METHOD(InternalQuicIn::addDestination) {
  InternalQuicIn* obj = ObjectWrap::Unwrap<InternalQuicIn>(info.Holder());
  QuicIn* me = obj->me;

  Nan::Utf8String param0(Nan::To<v8::String>(info[0]).ToLocalChecked());
  std::string track = std::string(*param0);

  FrameDestination* param = ObjectWrap::Unwrap<FrameDestination>(
      Nan::To<v8::Object>(info[1]).ToLocalChecked());
  owt_base::FrameDestination* dest = param->dest;

  if (track == "audio") {
    me->addAudioDestination(dest);
  } else if (track == "video") {
    me->addVideoDestination(dest);
  }
}

NAN_METHOD(InternalQuicIn::removeDestination) {
  InternalQuicIn* obj = ObjectWrap::Unwrap<InternalQuicIn>(info.Holder());
  QuicIn* me = obj->me;

  Nan::Utf8String param0(Nan::To<v8::String>(info[0]).ToLocalChecked());
  std::string track = std::string(*param0);

  FrameDestination* param = ObjectWrap::Unwrap<FrameDestination>(
      Nan::To<v8::Object>(info[1]).ToLocalChecked());
  owt_base::FrameDestination* dest = param->dest;

  if (track == "audio") {
    me->removeAudioDestination(dest);
  } else if (track == "video") {
    me->removeVideoDestination(dest);
  }
}

// Class InternalQuicOut
Nan::Persistent<Function> InternalQuicOut::constructor;
InternalQuicOut::InternalQuicOut() {};
InternalQuicOut::~InternalQuicOut() {};

NAN_MODULE_INIT(InternalQuicOut::Init) {
  // Constructor template
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("InternalQuicOut").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  Nan::SetPrototypeMethod(tpl, "close", close);

  constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(target, Nan::New("out").ToLocalChecked(),
           Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(InternalQuicOut::New) {
  Nan::Utf8String param0(Nan::To<v8::String>(info[0]).ToLocalChecked());
  std::string ip = std::string(*param0);
  int port = Nan::To<unsigned int>(info[1]).FromJust();

  InternalQuicOut* obj = new InternalQuicOut();
  obj->me = new QuicOut(ip, port);
  obj->dest = obj->me;

  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(InternalQuicOut::close) {
  InternalQuicOut* obj = ObjectWrap::Unwrap<InternalQuicOut>(info.Holder());
  delete obj->me;
  obj->me = nullptr;
  obj->dest = nullptr;
}
