// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_WIN_REGISTRY_H_
#define BASE_WIN_REGISTRY_H_

#include <windows.h>
#include <string>
#include <vector>

#define BASE_EXPORT

#define DCHECK(condition) ((void)0)
#define DCHECK_LT(a, b) ((void)0)
#define DCHECK_GT(a, b) ((void)0)

template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];
#define arraysize(array) (sizeof(ArraySizeHelper(array)))

typedef long long int64;

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

template<typename T>
inline T* vector_as_array(std::vector<T>* v) {
  return v->empty() ? NULL : &*v->begin();
}

  template<typename T>
inline const T* vector_as_array(const std::vector<T>* v) {
  return v->empty() ? NULL : &*v->begin();
}

template <class string_type>
inline typename string_type::value_type* WriteInto(string_type* str,
                                                   size_t length_with_null) {
  DCHECK_GT(length_with_null, 1u);
  str->reserve(length_with_null);
  str->resize(length_with_null - 1);
  return &((*str)[0]);
}


namespace base {
namespace win {

// Utility class to read, write and manipulate the Windows Registry.
// Registry vocabulary primer: a "key" is like a folder, in which there
// are "values", which are <name, data> pairs, with an associated data type.
//
// Note:
// ReadValue family of functions guarantee that the return arguments
// are not touched in case of failure.
class BASE_EXPORT RegKey {
 public:
  RegKey();
  explicit RegKey(HKEY key);
  RegKey(HKEY rootkey, const wchar_t* subkey, REGSAM access);
  ~RegKey();

  LONG Create(HKEY rootkey, const wchar_t* subkey, REGSAM access);

  LONG CreateWithDisposition(HKEY rootkey, const wchar_t* subkey,
                             DWORD* disposition, REGSAM access);

  // Creates a subkey or open it if it already exists.
  LONG CreateKey(const wchar_t* name, REGSAM access);

  // Opens an existing reg key.
  LONG Open(HKEY rootkey, const wchar_t* subkey, REGSAM access);

  // Opens an existing reg key, given the relative key name.
  LONG OpenKey(const wchar_t* relative_key_name, REGSAM access);

  // Closes this reg key.
  void Close();

  // Replaces the handle of the registry key and takes ownership of the handle.
  void Set(HKEY key);

  // Transfers ownership away from this object.
  HKEY Take();

  // Returns false if this key does not have the specified value, of if an error
  // occurrs while attempting to access it.
  bool HasValue(const wchar_t* value_name) const;

  // Returns the number of values for this key, of 0 if the number cannot be
  // determined.
  DWORD GetValueCount() const;

  // Determine the nth value's name.
  LONG GetValueNameAt(int index, std::wstring* name) const;

  // True while the key is valid.
  bool Valid() const { return key_ != NULL; }

  // Kill a key and everything that live below it; please be careful when using
  // it.
  LONG DeleteKey(const wchar_t* name);

  // Deletes a single value within the key.
  LONG DeleteValue(const wchar_t* name);

  // Determine the type of the value.
  LONG GetValueType(const wchar_t* name, DWORD* type) const;

  // Getters:

  // Returns an int32 value. If |name| is NULL or empty, returns the default
  // value, if any.
  LONG ReadValueDW(const wchar_t* name, DWORD* out_value) const;

  // Returns an int64 value. If |name| is NULL or empty, returns the default
  // value, if any.
  LONG ReadInt64(const wchar_t* name, int64* out_value) const;

  // Returns a string value. If |name| is NULL or empty, returns the default
  // value, if any.
  LONG ReadValue(const wchar_t* name, std::wstring* out_value) const;

  // Reads a REG_MULTI_SZ registry field into a vector of strings. Clears
  // |values| initially and adds further strings to the list. Returns
  // ERROR_CANTREAD if type is not REG_MULTI_SZ.
  LONG ReadValues(const wchar_t* name, std::vector<std::wstring>* values);

  // Returns raw data. If |name| is NULL or empty, returns the default
  // value, if any.
  LONG ReadValue(const wchar_t* name,
                 void* data,
                 DWORD* dsize,
                 DWORD* dtype) const;

  // Setters:

  // Sets an int32 value.
  LONG WriteValue(const wchar_t* name, DWORD in_value);

  // Sets a string value.
  LONG WriteValue(const wchar_t* name, const wchar_t* in_value);

  // Sets raw data, including type.
  LONG WriteValue(const wchar_t* name,
                  const void* data,
                  DWORD dsize,
                  DWORD dtype);

  // Starts watching the key to see if any of its values have changed.
  // The key must have been opened with the KEY_NOTIFY access privilege.
  LONG StartWatching();

  // If StartWatching hasn't been called, always returns false.
  // Otherwise, returns true if anything under the key has changed.
  // This can't be const because the |watch_event_| may be refreshed.
  bool HasChanged();

  // Will automatically be called by destructor if not manually called
  // beforehand.  Returns true if it was watching, false otherwise.
  LONG StopWatching();

  inline bool IsWatching() const { return watch_event_ != 0; }
  HANDLE watch_event() const { return watch_event_; }
  HKEY Handle() const { return key_; }

 private:
  HKEY key_;  // The registry key being iterated.
  HANDLE watch_event_;

  DISALLOW_COPY_AND_ASSIGN(RegKey);
};

// Iterates the entries found in a particular folder on the registry.
class BASE_EXPORT RegistryValueIterator {
 public:
  RegistryValueIterator(HKEY root_key, const wchar_t* folder_key);

  ~RegistryValueIterator();

  DWORD ValueCount() const;

  // True while the iterator is valid.
  bool Valid() const;

  // Advances to the next registry entry.
  void operator++();

  const wchar_t* Name() const { return name_.c_str(); }
  const wchar_t* Value() const { return vector_as_array(&value_); }
  // ValueSize() is in bytes.
  DWORD ValueSize() const { return value_size_; }
  DWORD Type() const { return type_; }

  int Index() const { return index_; }

 private:
  // Read in the current values.
  bool Read();

  // The registry key being iterated.
  HKEY key_;

  // Current index of the iteration.
  int index_;

  // Current values.
  std::wstring name_;
  std::vector<wchar_t> value_;
  DWORD value_size_;
  DWORD type_;

  DISALLOW_COPY_AND_ASSIGN(RegistryValueIterator);
};

class BASE_EXPORT RegistryKeyIterator {
 public:
  RegistryKeyIterator(HKEY root_key, const wchar_t* folder_key);

  ~RegistryKeyIterator();

  DWORD SubkeyCount() const;

  // True while the iterator is valid.
  bool Valid() const;

  // Advances to the next entry in the folder.
  void operator++();

  const wchar_t* Name() const { return name_; }

  int Index() const { return index_; }

 private:
  // Read in the current values.
  bool Read();

  // The registry key being iterated.
  HKEY key_;

  // Current index of the iteration.
  int index_;

  wchar_t name_[MAX_PATH];

  DISALLOW_COPY_AND_ASSIGN(RegistryKeyIterator);
};

}  // namespace win
}  // namespace base

#endif  // BASE_WIN_REGISTRY_H_
