// RegistWin - A Windows Registry Extension for Node.js
// Copyright (c) 2014 Lu Yuan <luyuan.china@gmail.com>
// Portions Copyright(c) 2014 Google Inc, and 2013 NetEase Youdao Inc. and other
// heX contributors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef REGISTWIN_LIB_UTIL_H_
#define REGISTWIN_LIB_UTIL_H_

#include <node.h>
#include <string>
#include <v8.h>
#include "base/win/registry.h"

namespace registwin {

// From Chromium
typedef std::wstring string16;
typedef wchar_t char16;

void StringArrayToMultiSZBytes(const std::vector<string16>& strings,
                               std::vector<char>* buffer);

// From libuv
int uv_utf16_to_utf8(const WCHAR* utf16Buffer, size_t utf16Size,
    char* utf8Buffer, size_t utf8Size);

int uv_utf8_to_utf16(const char* utf8Buffer, WCHAR* utf16Buffer,
    size_t utf16Size);

// From heX
std::string HKEY2HexString(const HKEY& key);

HKEY HexString2HKEY(const std::string& str);

string16 V8StringToString16(v8::Handle<v8::String> v8_string);

v8::Handle<v8::String> String16ToV8String(const string16& string);

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif

#ifndef STATUS_BUFFER_TOO_SMALL
#define STATUS_BUFFER_TOO_SMALL ((NTSTATUS)0xC0000023L)
#endif

typedef DWORD (__stdcall *NtQueryKeyType)(HANDLE  KeyHandle,
                                          int KeyInformationClass,
                                          PVOID  KeyInformation,
                                          ULONG  Length,
                                          PULONG  ResultLength);

string16 GetKeyPathFromHKEY(HKEY key);

};  // namespace registwin

#endif  // REGISTWIN_LIB_UTIL_H_
