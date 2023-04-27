// Copyright (C) <2019> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0

#include "InternalQuic.h"
#include <node.h>

using namespace v8;

void InitAll(Local<Object> exports) {
  InternalQuicIn::Init(exports);
  InternalQuicOut::Init(exports);
}

NODE_MODULE(addon, InitAll)
