// RegistWin - A Windows Registry Extension for Node.js
// Copyright (c) 2014 Lu Yuan <luyuan.china@gmail.com>
// Portions Copyright(c) 2014 Google Inc, and 2014 NetEase Youdao Inc. and other
// heX contributors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef REGISTWIN_LIB_REGISTWIN_H_
#define REGISTWIN_LIB_REGISTWIN_H_

#include <uv.h>
#include <v8.h>
#include "base/win/registry.h"

namespace registwin {

struct Watcher {
  uv_work_t* request;
  v8::Persistent<v8::Function> callback;
  base::win::RegKey* reg_key;
  bool can_call;
};

#define SYMBOL(symbol) v8::String::NewSymbol(symbol)

#define RESULT(value) return scope.Close(value)

#define SET_PROPERTY(obj, name, value, ...)                                    \
obj->Set(SYMBOL(name), value, ##__VA_ARGS__)

#define SET_METHOD(obj, name, callback, ...)                                   \
obj->Set(SYMBOL(name), v8::FunctionTemplate::New(callback)->GetFunction(),     \
    ##__VA_ARGS__)

#define RODD static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete)

#define INT(integer) v8::Int32::New(integer)

#define UNWRAP(key) base::win::RegKey* key = NULL;                             \
if (!args.This().IsEmpty() &&                                                  \
    args.This()->InternalFieldCount() > 0) {                                   \
  key = static_cast<base::win::RegKey*>(                                       \
      NanGetInternalFieldPointer(args.This(),0));                              \
}                                                                              \
if (!key) {                                                                    \
  v8::ThrowException(v8::Exception::TypeError(SYMBOL("Illegal invocation."))); \
  NanReturnUndefined();                                                        \
}

#define WRAP(key, object) NanSetInternalFieldPointer(object, 0, key);

}  // namespace registwin

#endif  // REGISTWIN_LIB_REGISTWIN_H_
