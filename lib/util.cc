// RegistWin - A Windows Registry Extension for Node.js
// Copyright (c) 2014 Lu Yuan <luyuan.china@gmail.com>
// Portions Copyright(c) 2014 Google Inc, and 2013 NetEase Youdao Inc. and other
// heX contributors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "util.h"

namespace registwin {

std::string HKEY2HexString(const HKEY& key) {
  uint32_t i = reinterpret_cast<uint32_t>(key);
  char str[20];
  sprintf(str, "0x%lx", i);
  return str;
}

#pragma warning(disable: 4101)
HKEY HexString2HKEY(const std::string& str) {
  try {
    uint32_t i = std::stoul(str, 0, 16);
    return reinterpret_cast<HKEY>(i);
  } catch(const std::invalid_argument& e) {
    return NULL;
  }
}
#pragma warning(default: 4101)

void StringArrayToMultiSZBytes(const std::vector<string16>& strings,
                               std::vector<char>* buffer) {
  DCHECK(buffer);
  buffer->clear();

  if (strings.empty()) {
    // Leave buffer empty if we have no strings.
    return;
  }

  size_t total_wchars = 0;
  {
    std::vector<string16>::const_iterator iter(strings.begin());
    for (; iter != strings.end(); ++iter) {
      total_wchars += iter->length();
      total_wchars++;  // Space for the null char.
    }
    total_wchars++;  // Space for the extra terminating null char.
  }

  size_t total_length = total_wchars * sizeof(wchar_t);
  buffer->resize(total_length);
  wchar_t* write_pointer = reinterpret_cast<wchar_t*>(&((*buffer)[0]));
  // Keep an end pointer around for sanity checking.
  wchar_t* end_pointer = write_pointer + total_wchars;

  std::vector<string16>::const_iterator copy_iter(strings.begin());
  for (; copy_iter != strings.end() && write_pointer < end_pointer;
       copy_iter++) {
    // First copy the source string.
    size_t string_length = copy_iter->length() + 1;
    memcpy(write_pointer, copy_iter->c_str(),
           string_length * sizeof(wchar_t));
    write_pointer += string_length;

    // We should never run off the end while in this loop.
    DCHECK(write_pointer < end_pointer);
  }
  *write_pointer = L'\0';  // Explicitly set the final null char.
  DCHECK(++write_pointer == end_pointer);
}


// From libuv, src/win/util.c
int uv_utf16_to_utf8(const WCHAR* utf16Buffer, size_t utf16Size,
    char* utf8Buffer, size_t utf8Size) {
  return WideCharToMultiByte(CP_UTF8,
                             0,
                             utf16Buffer,
                             utf16Size,
                             utf8Buffer,
                             utf8Size,
                             NULL,
                             NULL);
}

int uv_utf8_to_utf16(const char* utf8Buffer, WCHAR* utf16Buffer,
    size_t utf16Size) {
  return MultiByteToWideChar(CP_UTF8,
                             0,
                             utf8Buffer,
                             -1,
                             utf16Buffer,
                             utf16Size);
}

string16 V8StringToString16(v8::Handle<v8::String> v8_string) {
  v8::HandleScope scope;

  size_t len = v8_string->Length();
  char16* buf = new char16[len + 1];
  v8_string->Write(reinterpret_cast<uint16_t*>(buf), 0, len + 1);
  string16 string(buf);
  delete[] buf;
  return string;
}

v8::Handle<v8::String> String16ToV8String(const string16& string) {
  WCHAR* buf = NULL, *ptr;
  size_t buf_char_len = 4096;
  size_t len = wcslen(string.c_str());
  WCHAR* str2;
  int size;
  char* res;

  str2 = (WCHAR*)malloc(sizeof(WCHAR) * (len + 4));

  buf = (WCHAR*)malloc(buf_char_len * sizeof(WCHAR));
  ptr = buf;

  while ((ptr - buf) + len + 1 > buf_char_len) {
    buf_char_len *= 2;
    str2 = buf;
    buf = (WCHAR*)realloc(buf, buf_char_len * sizeof(WCHAR));
    ptr = buf + (ptr - str2);
  }

  wcscpy(ptr, string.c_str());
  ptr += len + 1;

  if (buf) {
    size = uv_utf16_to_utf8(buf, buf_char_len, NULL, 0);
    if (!size) {
      return v8::String::New("");
    }

    res = (char*)malloc(size + 1);
    if (!res) {
      return v8::String::New("");
    }

    size = uv_utf16_to_utf8(buf, buf_char_len, (char*)res, size);
    if (!size) {
      free(buf);
      free(res);
      res = NULL;
      return v8::String::New("");
    }
    free(buf);

    ((char*)res)[size] = '\0';
  } else {
    return v8::String::New("");
  }

  return v8::String::New(res);
}

string16 GetKeyPathFromHKEY(HKEY key) {
  string16 key_path;
  if (!key) {
    return key_path;
  }
  if (key != NULL) {
    HMODULE library = LoadLibrary(L"ntdll.dll");
    if (library != NULL) {
      NtQueryKeyType NtQueryKey = reinterpret_cast<NtQueryKeyType>(
          GetProcAddress(library, "NtQueryKey"));

      if (NtQueryKey != NULL) {
        DWORD size = 0;
        DWORD result = 0;
        result = NtQueryKey(key, 3, NULL, 0, &size);
        if (result == STATUS_BUFFER_TOO_SMALL) {
          size += 2;
          WCHAR* buffer = new (std::nothrow)WCHAR[size / sizeof(WCHAR)];
          if (buffer != NULL) {
            result = NtQueryKey(key, 3, buffer, size, &size);
            if (result == STATUS_SUCCESS) {
              buffer[size / sizeof(WCHAR)] = L'\0';
              key_path = string16(buffer + 2);
            }
            delete[] buffer;
          }
        }
      }
      FreeLibrary(library);
    }
  }
  return key_path;
}

}  // namespace registwin
