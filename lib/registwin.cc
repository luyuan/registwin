// RegistWin - A Windows Registry Extension for Node.js
// Copyright (c) 2014 Lu Yuan <luyuan.china@gmail.com>
// Portions Copyright(c) 2014 Google Inc, and 2014 NetEase Youdao Inc. and other
// heX contributors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION
#endif // BUILDING_NODE_EXTENSION

#include "registwin.h"
#include "util.h"
#include <nan.h>

namespace registwin {

NAN_METHOD(New) {
  NanScope();

  if (args.Length() == 1 && args[0]->IsInt32()) {
    HKEY key = reinterpret_cast<HKEY>(args[0]->Int32Value());
    base::win::RegKey* reg_key = new base::win::RegKey(key);
    WRAP(reg_key, args.This());
    NanReturnUndefined();
  }
  if (args.Length() == 1 && args[0]->IsString()) {
    char hex_str[20];
    args[0]->ToString()->WriteAscii(hex_str);
    HKEY key = HexString2HKEY(hex_str);
    base::win::RegKey* reg_key = new base::win::RegKey(key);
    WRAP(reg_key, args.This());
    NanReturnUndefined();
  }
  if (args.Length() >= 2 && args[0]->IsInt32() && args[1]->IsString()) {
    HKEY rootkey = reinterpret_cast<HKEY>(args[0]->Int32Value());
    string16 subkey = V8StringToString16(args[1]->ToString());
    REGSAM access = KEY_READ;
    if (args[2]->IsInt32())
      access = static_cast<REGSAM>(args[2]->Int32Value());
    base::win::RegKey* reg_key =
        new base::win::RegKey(rootkey, rootkey ? subkey.c_str() : NULL, access);
    WRAP(reg_key, args.This());
    NanReturnUndefined();
  }
  if (args.Length() >= 2 && args[0]->IsString() && args[1]->IsString()) {
    char hex_str[20];
    args[0]->ToString()->WriteAscii(hex_str);
    HKEY rootkey = HexString2HKEY(hex_str);
    string16 subkey = V8StringToString16(args[1]->ToString());
    REGSAM access = KEY_READ;
    if (args[2]->IsInt32())
      access = static_cast<REGSAM>(args[2]->Int32Value());
    base::win::RegKey* reg_key =
        new base::win::RegKey(rootkey, rootkey ? subkey.c_str() : NULL, access);
    WRAP(reg_key, args.This());
    NanReturnUndefined();
  }
  
  base::win::RegKey* reg_key = new base::win::RegKey;
  WRAP(reg_key, args.This());
  NanReturnUndefined();
}

NAN_METHOD(Close) {
  NanScope();

  UNWRAP(reg_key);

  reg_key->Close();
  NanReturnUndefined();
}

NAN_METHOD(Create) {
  NanScope();

  UNWRAP(reg_key);

  if (args.Length() < 1 || (!args[0]->IsInt32() && !args[0]->IsString())) {
    v8::ThrowException(v8::Exception::TypeError(SYMBOL(
        "Parameters are not correct. The first parameter must be a handle.")));
    NanReturnUndefined();
  }

  HKEY root_key;
  if (args[0]->IsInt32()) {
    root_key = reinterpret_cast<HKEY>(args[0]->Int32Value());
  }
  if (args[0]->IsString()) {
    char hex_str[20];
    args[0]->ToString()->WriteAscii(hex_str);
    root_key = HexString2HKEY(hex_str);
  }
  string16 sub_key;
  if (args[1]->IsString())
    sub_key = V8StringToString16(args[1]->ToString());
  REGSAM access = KEY_ALL_ACCESS;
  if (!args[1]->IsUndefined() && args[2]->IsInt32())
    access = static_cast<REGSAM>(args[2]->Int32Value());
  if (args[1]->IsInt32())
    access = static_cast<REGSAM>(args[1]->Int32Value());

  LONG r = reg_key->Create(root_key, sub_key.empty() ? NULL : sub_key.c_str(),
      access);
  NanReturnValue(NanNew<v8::Boolean>(r == ERROR_SUCCESS));
}

NAN_METHOD(CreateKey) {
  NanScope();

  UNWRAP(reg_key);

  if (args.Length() < 1 || !args[0]->IsString()) {
    v8::ThrowException(v8::Exception::TypeError(SYMBOL(
        "Parameters are not correct. The first parameter must be a key name."
            )));
    NanReturnUndefined();
  }

  string16 subkey = V8StringToString16(args[0]->ToString());
  REGSAM access = KEY_ALL_ACCESS;
  if (args[1]->IsInt32())
    access = static_cast<REGSAM>(args[1]->Int32Value());

  LONG r = reg_key->CreateKey(subkey.c_str(), access);
  NanReturnValue(NanNew<v8::Boolean>(r == ERROR_SUCCESS));
}

NAN_METHOD(Open) {
  NanScope();

  UNWRAP(reg_key);

  if (args.Length() < 1 || (!args[0]->IsInt32() && !args[0]->IsString())) {
    v8::ThrowException(v8::Exception::TypeError(SYMBOL(
        "Parameters are not correct. The first parameter must be a handle.")));
    NanReturnUndefined();
  }

  HKEY root_key;
  if (args[0]->IsInt32()) {
    root_key = reinterpret_cast<HKEY>(args[0]->Int32Value());
  }
  if (args[0]->IsString()) {
    char hex_str[20];
    args[0]->ToString()->WriteAscii(hex_str);
    root_key = HexString2HKEY(hex_str);
  }
  string16 sub_key;
  if (args[1]->IsString())
    sub_key = V8StringToString16(args[1]->ToString());
  REGSAM access = KEY_READ;
  if (!args[1]->IsUndefined() && args[2]->IsInt32())
    access = static_cast<REGSAM>(args[2]->Int32Value());
  if (args[1]->IsInt32())
    access = static_cast<REGSAM>(args[1]->Int32Value());

  LONG r = reg_key->Open(root_key, sub_key.empty() ? NULL : sub_key.c_str(),
      access);
  NanReturnValue(NanNew<v8::Boolean>(r == ERROR_SUCCESS));
}

NAN_METHOD(OpenKey) {
  NanScope();

  UNWRAP(reg_key);

  if (args.Length() < 1 || !args[0]->IsString()) {
    v8::ThrowException(v8::Exception::TypeError(SYMBOL(
        "Parameters are not correct. The first parameter must be a key name."
            )));
    NanReturnUndefined();
  }

  string16 subkey = V8StringToString16(args[0]->ToString());
  REGSAM access = KEY_READ;
  if (args[1]->IsInt32())
    access = static_cast<REGSAM>(args[1]->Int32Value());

  LONG r = reg_key->OpenKey(subkey.c_str(), access);
  NanReturnValue(NanNew<v8::Boolean>(r == ERROR_SUCCESS));
}

NAN_METHOD(Set) {
  NanScope();

  UNWRAP(reg_key);

  if (args.Length() < 1 || (!args[0]->IsInt32() && !args[0]->IsString())) {
    v8::ThrowException(v8::Exception::TypeError(SYMBOL(
        "Parameters are not correct. The first parameter must be a handle.")));
    NanReturnUndefined();
  }

  HKEY key;
  if (args[0]->IsInt32()) {
    key = reinterpret_cast<HKEY>(args[0]->Int32Value());
  }
  if (args[0]->IsString()) {
    char hex_str[20];
    args[0]->ToString()->WriteAscii(hex_str);
    key = HexString2HKEY(hex_str);
  }

  reg_key->Set(key);
  NanReturnUndefined();
}

NAN_METHOD(Take) {
  NanScope();

  UNWRAP(reg_key);

  NanReturnValue(NanNew<v8::String>(HKEY2HexString(reg_key->Take()).c_str()));
}

NAN_METHOD(HasValue) {
  NanScope();

  UNWRAP(reg_key);

  if (args.Length() < 1 || !args[0]->IsString()) {
    v8::ThrowException(v8::Exception::TypeError(SYMBOL(
        "Parameters are not correct. The first parameter must be a value name."
            )));
    NanReturnUndefined();
  }

  NanReturnValue(v8::Boolean::New(reg_key->HasValue(
      V8StringToString16(args[0]->ToString()).c_str())));
}

NAN_METHOD(GetValueCount) {
  NanScope();

  UNWRAP(reg_key);

  NanReturnValue(NanNew<v8::Integer>(reg_key->GetValueCount()));
}

NAN_METHOD(GetValueNameAt) {
  NanScope();

  UNWRAP(reg_key);

  if (args.Length() < 1 || !args[0]->IsInt32()) {
    v8::ThrowException(v8::Exception::TypeError(SYMBOL(
        "Parameters are not correct. The first parameter must be a number.")));
    NanReturnUndefined();
  }

  int index = args[0]->Int32Value();
  string16 name;
  LONG r = reg_key->GetValueNameAt(index, &name);

  if (r == ERROR_SUCCESS)
    NanReturnValue(String16ToV8String(name));
  else
    NanReturnNull();
}

NAN_METHOD(Valid) {
  NanScope();

  UNWRAP(reg_key);

  NanReturnValue(NanNew<v8::Boolean>(reg_key->Valid()));
}

NAN_METHOD(DeleteKey) {
  NanScope();

  UNWRAP(reg_key);

  if (args.Length() < 1 || !args[0]->IsString()) {
    v8::ThrowException(v8::Exception::TypeError(SYMBOL(
        "Parameters are not correct. The first parameter must be a key name."
            )));
    NanReturnUndefined();
  }

  string16 name = V8StringToString16(args[0]->ToString());

  NanReturnValue(NanNew<v8::Boolean>(
      reg_key->DeleteKey(name.c_str()) == ERROR_SUCCESS));
}

NAN_METHOD(DeleteValue) {
  NanScope();

  UNWRAP(reg_key);

  if (args.Length() < 1 || !args[0]->IsString()) {
    v8::ThrowException(v8::Exception::TypeError(SYMBOL(
        "Parameters are not correct. The first parameter must be a value name."
            )));
    NanReturnUndefined();
  }

  string16 name = V8StringToString16(args[0]->ToString());

  NanReturnValue(NanNew<v8::Boolean>(
      reg_key->DeleteValue(name.c_str()) == ERROR_SUCCESS));
}

NAN_METHOD(GetValueType) {
  NanScope();

  UNWRAP(reg_key);

  if (args.Length() < 1 || !args[0]->IsString()) {
    v8::ThrowException(v8::Exception::TypeError(SYMBOL(
        "Parameters are not correct. The first parameter must be a value name."
            )));
    NanReturnUndefined();
  }

  string16 name = V8StringToString16(args[0]->ToString());

  DWORD type;
  LONG r = reg_key->GetValueType(name.c_str(), &type);

  if (r == ERROR_SUCCESS)
    NanReturnValue(NanNew<v8::Integer>(type));
  else
    NanReturnNull();
}

NAN_METHOD(ReadValueDW) {
  NanScope();

  UNWRAP(reg_key);

  if (args.Length() < 1 || !args[0]->IsString()) {
    v8::ThrowException(v8::Exception::TypeError(SYMBOL(
        "Parameters are not correct. The first parameter must be a value name."
            )));
    NanReturnUndefined();
  }

  string16 name = V8StringToString16(args[0]->ToString());
  DWORD dw;

  LONG r = reg_key->ReadValueDW(name.c_str(), &dw);

  if (r == ERROR_SUCCESS)
    NanReturnValue(NanNew<v8::Integer>(dw));
  else
    NanReturnNull();
}

NAN_METHOD(ReadInt64) {
  NanScope();

  UNWRAP(reg_key);

  if (args.Length() < 1 || !args[0]->IsString()) {
    v8::ThrowException(v8::Exception::TypeError(SYMBOL(
        "Parameters are not correct. The first parameter must be a value name."
            )));
    NanReturnUndefined();
  }

  string16 name = V8StringToString16(args[0]->ToString());
  int64 i64;

  LONG r = reg_key->ReadInt64(name.c_str(), &i64);
  char hex_str[100];
  sprintf(hex_str, "%I64d", i64);

  if (r == ERROR_SUCCESS)
    NanReturnValue(v8::String::New(hex_str));
  else
    NanReturnNull();
}

NAN_METHOD(ReadValue) {
  NanScope();

  UNWRAP(reg_key);

  if (args.Length() < 1 || !args[0]->IsString()) {
    v8::ThrowException(v8::Exception::TypeError(SYMBOL(
        "Parameters are not correct. The first parameter must be a value name."
            )));
    NanReturnUndefined();
  }

  string16 name = V8StringToString16(args[0]->ToString());
  string16 value;
  
  LONG r = reg_key->ReadValue(name.c_str(), &value);

  if (r == ERROR_SUCCESS)
    NanReturnValue(String16ToV8String(value));
  else
    NanReturnNull();
}

NAN_METHOD(ReadValues) {
  NanScope();

  UNWRAP(reg_key);

  if (args.Length() < 1 || !args[0]->IsString()) {
    v8::ThrowException(v8::Exception::TypeError(SYMBOL(
        "Parameters are not correct. The first parameter must be a value name."
            )));
    NanReturnUndefined();
  }

  string16 name = V8StringToString16(args[0]->ToString());
  std::vector<string16> values;

  LONG r = reg_key->ReadValues(name.c_str(), &values);

  if (r == ERROR_SUCCESS) {
    v8::Local<v8::Array> arr = v8::Array::New(values.size());
    for (size_t i = 0; i < values.size(); ++i) {
      arr->Set(i, String16ToV8String(values.at(i)));
    }
    NanReturnValue(arr);
  } else
    NanReturnNull();
}

NAN_METHOD(WriteValue) {
  NanScope();

  UNWRAP(reg_key);

  if (args.Length() < 2 || !args[0]->IsString() || (!args[1]->IsInt32() &&
      !args[1]->IsString() || !args[1]->IsArray())) {
    v8::ThrowException(v8::Exception::TypeError(SYMBOL(
        "Parameters are not correct. The first parameter must be a value name. "
        "The second parameter can be a string, integer or array.")));
    NanReturnUndefined();
  }

  string16 name = V8StringToString16(args[0]->ToString());

  LONG r;
  if (args[1]->IsInt32()) {
    r = reg_key->WriteValue(name.c_str(), args[1]->Int32Value());
  } else if (args[1]->IsString()) {
    r = reg_key->WriteValue(name.c_str(),
        V8StringToString16(args[1]->ToString()).c_str());
  } else if (args[1]->IsArray()) {
    std::vector<string16> vec;
    v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(args[1]->ToObject());
    for (size_t i = 0; i < arr->Length(); ++i) {
      vec.push_back(V8StringToString16(arr->Get(i)->ToString()));
    }
    std::vector<char> buffer;
    StringArrayToMultiSZBytes(vec, &buffer);
    r = reg_key->WriteValue(name.c_str(), &buffer[0], buffer.size(),
        REG_MULTI_SZ);
  }

  NanReturnValue(NanNew<v8::Boolean>(r == ERROR_SUCCESS));
}

static void WatcherAsync(uv_work_t* request) {
  Watcher* watcher = static_cast<Watcher*>(request->data);
  base::win::RegKey* reg_key = watcher->reg_key;
  if (reg_key->HasChanged()) {
    watcher->can_call = true;
  }
}

static void WatcherAsyncAfter(uv_work_t* request, int status) {
  assert(status == 0);

  Watcher* watcher = static_cast<Watcher*>(request->data);

  if (watcher->can_call) {
    v8::Local<v8::Value> argv[] = {
        v8::String::New(HKEY2HexString(watcher->reg_key->Handle()).c_str())
    };
    watcher->callback->Call(NanGetCurrentContext()->Global(), 1, argv);
  }

  if (watcher->reg_key && !watcher->reg_key->watch_event()) {
    if (!watcher->callback.IsEmpty()) {
      watcher->callback.Dispose();
      delete watcher;
      return;
    }
  }

  if (request && watcher)
    uv_queue_work(uv_default_loop(), request, WatcherAsync,
        (uv_after_work_cb)WatcherAsyncAfter);
  watcher->can_call = false;
}

NAN_METHOD(StartWatching) {
  NanScope();

  UNWRAP(reg_key);

  if (args[0]->IsFunction()) {
    v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[0]);

    Watcher* watcher = new Watcher;
    watcher->request->data = watcher;
    watcher->callback = v8::Persistent<v8::Function>::New(callback);
    watcher->reg_key = reg_key;
    watcher->can_call = false;

    uv_queue_work(uv_default_loop(), watcher->request, WatcherAsync,
        WatcherAsyncAfter);
  }

  NanReturnValue(
      NanNew<v8::Boolean>(reg_key->StartWatching() == ERROR_SUCCESS));
}

NAN_METHOD(HasChanged) {
  NanScope();

  UNWRAP(reg_key);

  NanReturnValue(NanNew<v8::Boolean>(reg_key->HasChanged()));
}

NAN_METHOD(StopWatching) {
  NanScope();

  UNWRAP(reg_key);

  NanReturnValue(NanNew<v8::Boolean>(reg_key->StopWatching() == ERROR_SUCCESS));
}

NAN_METHOD(IsWatching) {
  NanScope();

  UNWRAP(reg_key);

  NanReturnValue(NanNew<v8::Boolean>(reg_key->IsWatching()));
}

NAN_METHOD(Handle) {
  NanScope();

  UNWRAP(reg_key);

  NanReturnValue(NanNew<v8::String>(HKEY2HexString(reg_key->Handle()).c_str()));
}

NAN_METHOD(Handle2) {
  NanScope();

  UNWRAP(reg_key);

  NanReturnValue(NanNew<v8::Integer>(
      reinterpret_cast<uint32_t>(reg_key->Handle())));
}

NAN_METHOD(GetRealKeyPath) {
  NanScope();

  UNWRAP(reg_key);

  NanReturnValue(String16ToV8String(GetKeyPathFromHKEY(reg_key->Handle())));
}

NAN_METHOD(IterateKeys) {
  NanScope();

  UNWRAP(reg_key);

  HKEY root_key = reg_key->Handle();
  v8::Local<v8::Function> func;
  if (args[0]->IsFunction()) {
    func = v8::Local<v8::Function>::Cast(args[0]->ToObject());
  }

  std::vector<string16> keys;
  size_t i = 0;
  for (base::win::RegistryKeyIterator iter(root_key, NULL);
      iter.Valid(); ++iter, ++i) {
    keys.push_back(iter.Name());
    DWORD count = iter.SubkeyCount();
    if (!func.IsEmpty()) {
      v8::Handle<v8::Value> argv[] = {
          NanNew<v8::Integer>(i),
          String16ToV8String(iter.Name()),
          NanNew<v8::Integer>(count)
      };
      func->Call(args.This(), 3, argv);
    }
  }
  if (!func.IsEmpty()) {
    NanReturnUndefined();
  }

  v8::Local<v8::Array> arr = v8::Array::New();
  for (size_t j = 0; j < keys.size(); ++j) {
    arr->Set(j, String16ToV8String(keys.at(j)));
  }

  NanReturnValue(arr);
}

NAN_METHOD(IterateValues) {
  NanScope();

  UNWRAP(reg_key);

  HKEY root_key = reg_key->Handle();
  v8::Local<v8::Function> func;
  if (args[0]->IsFunction()) {
    func = v8::Local<v8::Function>::Cast(args[0]->ToObject());
  }

  std::vector<string16> keys;
  size_t i = 0;
  for (base::win::RegistryValueIterator iter(root_key, NULL);
      iter.Valid(); ++iter, ++i) {
    keys.push_back(iter.Name());
    DWORD count = iter.ValueCount();
    if (!func.IsEmpty()) {
      v8::Handle<v8::Value> argv[] = {
          NanNew<v8::Integer>(i),
          String16ToV8String(iter.Name()),
          NanNew<v8::Integer>(count)
      };
      func->Call(args.This(), 3, argv);
    }
  }
  if (!func.IsEmpty()) {
    NanReturnUndefined();
  }

  v8::Local<v8::Array> arr = v8::Array::New();
  for (size_t j = 0; j < keys.size(); ++j) {
    arr->Set(j, String16ToV8String(keys.at(j)));
  }

  NanReturnValue(arr);
}

}  // namespace registwin

extern "C" void init(v8::Handle<v8::Object> target) {
  v8::Local<v8::FunctionTemplate> constructor =
      v8::FunctionTemplate::New(registwin::New);
  constructor->InstanceTemplate()->SetInternalFieldCount(1);
  constructor->SetClassName(SYMBOL("Registwin"));
  v8::Local<v8::ObjectTemplate> tmpl = constructor->PrototypeTemplate();

  SET_PROPERTY(constructor, "KEY_QUERY_VALUE", INT(KEY_QUERY_VALUE), RODD);
  SET_PROPERTY(constructor, "KEY_SET_VALUE", INT(KEY_SET_VALUE), RODD);
  SET_PROPERTY(constructor, "KEY_CREATE_SUB_KEY", INT(KEY_CREATE_SUB_KEY),
      RODD);
  SET_PROPERTY(constructor, "KEY_ENUMERATE_SUB_KEYS",
      INT(KEY_ENUMERATE_SUB_KEYS), RODD);
  SET_PROPERTY(constructor, "KEY_NOTIFY", INT(KEY_NOTIFY), RODD);
  SET_PROPERTY(constructor, "KEY_CREATE_LINK", INT(KEY_CREATE_LINK), RODD);
  SET_PROPERTY(constructor, "KEY_WOW64_32KEY", INT(KEY_WOW64_32KEY), RODD);
  SET_PROPERTY(constructor, "KEY_WOW64_64KEY", INT(KEY_WOW64_64KEY), RODD);
  SET_PROPERTY(constructor, "KEY_WOW64_RES", INT(KEY_WOW64_RES), RODD);
  SET_PROPERTY(constructor, "KEY_READ", INT(KEY_READ), RODD);
  SET_PROPERTY(constructor, "KEY_WRITE", INT(KEY_WRITE), RODD);
  SET_PROPERTY(constructor, "KEY_EXECUTE", INT(KEY_EXECUTE), RODD);
  SET_PROPERTY(constructor, "KEY_ALL_ACCESS", INT(KEY_ALL_ACCESS), RODD);

  SET_PROPERTY(constructor, "REG_NONE", INT(REG_NONE), RODD);
  SET_PROPERTY(constructor, "REG_SZ", INT(REG_SZ), RODD);
  SET_PROPERTY(constructor, "REG_EXPAND_SZ", INT(REG_EXPAND_SZ), RODD);
  SET_PROPERTY(constructor, "REG_BINARY", INT(REG_BINARY), RODD);
  SET_PROPERTY(constructor, "REG_DWORD", INT(REG_DWORD), RODD);
  SET_PROPERTY(constructor, "REG_DWORD_LITTLE_ENDIAN",
      INT(REG_DWORD_LITTLE_ENDIAN), RODD);
  SET_PROPERTY(constructor, "REG_DWORD_BIG_ENDIAN",
      INT(REG_DWORD_BIG_ENDIAN), RODD);
  SET_PROPERTY(constructor, "REG_LINK", INT(REG_LINK), RODD);
  SET_PROPERTY(constructor, "REG_MULTI_SZ", INT(REG_MULTI_SZ), RODD);
  SET_PROPERTY(constructor, "REG_RESOURCE_LIST", INT(REG_RESOURCE_LIST), RODD);
  SET_PROPERTY(constructor, "REG_FULL_RESOURCE_DESCRIPTOR",
      INT(REG_FULL_RESOURCE_DESCRIPTOR), RODD);
  SET_PROPERTY(constructor, "REG_RESOURCE_REQUIREMENTS_LIST",
      INT(REG_RESOURCE_REQUIREMENTS_LIST), RODD);
  SET_PROPERTY(constructor, "REG_QWORD", INT(REG_QWORD), RODD);
  SET_PROPERTY(constructor, "REG_QWORD_LITTLE_ENDIAN",
      INT(REG_QWORD_LITTLE_ENDIAN), RODD);

  SET_PROPERTY(constructor, "HKEY_CLASSES_ROOT", v8::String::New(
      registwin::HKEY2HexString(HKEY_CLASSES_ROOT).c_str()), RODD);
  SET_PROPERTY(constructor, "HKEY_CURRENT_USER", v8::String::New(
      registwin::HKEY2HexString(HKEY_CURRENT_USER).c_str()), RODD);
  SET_PROPERTY(constructor, "HKEY_LOCAL_MACHINE", v8::String::New(
      registwin::HKEY2HexString(HKEY_LOCAL_MACHINE).c_str()), RODD);
  SET_PROPERTY(constructor, "HKEY_USERS", v8::String::New(
      registwin::HKEY2HexString(HKEY_USERS).c_str()), RODD);
  SET_PROPERTY(constructor, "HKEY_PERFORMANCE_DATA", v8::String::New(
      registwin::HKEY2HexString(HKEY_PERFORMANCE_DATA).c_str()), RODD);
  SET_PROPERTY(constructor, "HKEY_PERFORMANCE_TEXT", v8::String::New(
      registwin::HKEY2HexString(HKEY_PERFORMANCE_TEXT).c_str()), RODD);
  SET_PROPERTY(constructor, "HKEY_PERFORMANCE_NLSTEXT", v8::String::New(
      registwin::HKEY2HexString(HKEY_PERFORMANCE_NLSTEXT).c_str()), RODD);
  SET_PROPERTY(constructor, "HKEY_CURRENT_CONFIG", v8::String::New(
      registwin::HKEY2HexString(HKEY_CURRENT_CONFIG).c_str()), RODD);
  SET_PROPERTY(constructor, "HKEY_DYN_DATA", v8::String::New(
      registwin::HKEY2HexString(HKEY_DYN_DATA).c_str()), RODD);
  SET_PROPERTY(constructor, "HKEY_CURRENT_USER_LOCAL_SETTINGS",
      v8::String::New(registwin::HKEY2HexString(
          HKEY_CURRENT_USER_LOCAL_SETTINGS).c_str()), RODD);

  SET_METHOD(tmpl, "close", registwin::Close, RODD);
  SET_METHOD(tmpl, "handle", registwin::Handle, RODD);
  SET_METHOD(tmpl, "createKey", registwin::CreateKey, RODD);
  SET_METHOD(tmpl, "open", registwin::Open, RODD);
  SET_METHOD(tmpl, "openKey", registwin::OpenKey, RODD);
  SET_METHOD(tmpl, "set", registwin::Set, RODD);
  SET_METHOD(tmpl, "take", registwin::Take, RODD);
  SET_METHOD(tmpl, "hasValue", registwin::HasValue, RODD);
  SET_METHOD(tmpl, "getValueCount", registwin::GetValueCount, RODD);
  SET_METHOD(tmpl, "getValueNameAt", registwin::GetValueNameAt, RODD);
  SET_METHOD(tmpl, "valid", registwin::Valid, RODD);
  SET_METHOD(tmpl, "deleteKey", registwin::DeleteKey, RODD);
  SET_METHOD(tmpl, "deleteValue", registwin::DeleteValue, RODD);
  SET_METHOD(tmpl, "getValueType", registwin::GetValueType, RODD);
  SET_METHOD(tmpl, "readValueDW", registwin::ReadValueDW, RODD);
  SET_METHOD(tmpl, "readInt64", registwin::ReadInt64, RODD);
  SET_METHOD(tmpl, "readValue", registwin::ReadValue, RODD);
  SET_METHOD(tmpl, "readValues", registwin::ReadValues, RODD);
  SET_METHOD(tmpl, "writeValue", registwin::WriteValue, RODD);
  SET_METHOD(tmpl, "startWatching", registwin::StartWatching, RODD);
  SET_METHOD(tmpl, "hasChanged", registwin::HasChanged, RODD);
  SET_METHOD(tmpl, "stopWatching", registwin::StopWatching, RODD);
  SET_METHOD(tmpl, "isWatching", registwin::IsWatching, RODD);
  SET_METHOD(tmpl, "handle", registwin::Handle, RODD);
  SET_METHOD(tmpl, "handle2", registwin::Handle2, RODD);
  SET_METHOD(tmpl, "getRealKeyPath", registwin::GetRealKeyPath, RODD);
  SET_METHOD(tmpl, "iterateKeys", registwin::IterateKeys, RODD);
  SET_METHOD(tmpl, "iterateValues", registwin::IterateValues, RODD);

  target->Set(SYMBOL("Registwin"), constructor->GetFunction(), RODD);
}


#ifdef NODE_MODULE
NODE_MODULE(registwin, init)
#endif
