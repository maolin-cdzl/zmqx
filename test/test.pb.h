// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: test.proto

#ifndef PROTOBUF_test_2eproto__INCLUDED
#define PROTOBUF_test_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace test {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_test_2eproto();
void protobuf_AssignDesc_test_2eproto();
void protobuf_ShutdownFile_test_2eproto();

class Empty;
class Hello;
class Shutdown;

// ===================================================================

class Empty : public ::google::protobuf::Message {
 public:
  Empty();
  virtual ~Empty();

  Empty(const Empty& from);

  inline Empty& operator=(const Empty& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Empty& default_instance();

  void Swap(Empty* other);

  // implements Message ----------------------------------------------

  Empty* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Empty& from);
  void MergeFrom(const Empty& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // @@protoc_insertion_point(class_scope:test.Empty)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_test_2eproto();
  friend void protobuf_AssignDesc_test_2eproto();
  friend void protobuf_ShutdownFile_test_2eproto();

  void InitAsDefaultInstance();
  static Empty* default_instance_;
};
// -------------------------------------------------------------------

class Hello : public ::google::protobuf::Message {
 public:
  Hello();
  virtual ~Hello();

  Hello(const Hello& from);

  inline Hello& operator=(const Hello& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Hello& default_instance();

  void Swap(Hello* other);

  // implements Message ----------------------------------------------

  Hello* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Hello& from);
  void MergeFrom(const Hello& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required string str = 1;
  inline bool has_str() const;
  inline void clear_str();
  static const int kStrFieldNumber = 1;
  inline const ::std::string& str() const;
  inline void set_str(const ::std::string& value);
  inline void set_str(const char* value);
  inline void set_str(const char* value, size_t size);
  inline ::std::string* mutable_str();
  inline ::std::string* release_str();
  inline void set_allocated_str(::std::string* str);

  // @@protoc_insertion_point(class_scope:test.Hello)
 private:
  inline void set_has_str();
  inline void clear_has_str();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* str_;
  friend void  protobuf_AddDesc_test_2eproto();
  friend void protobuf_AssignDesc_test_2eproto();
  friend void protobuf_ShutdownFile_test_2eproto();

  void InitAsDefaultInstance();
  static Hello* default_instance_;
};
// -------------------------------------------------------------------

class Shutdown : public ::google::protobuf::Message {
 public:
  Shutdown();
  virtual ~Shutdown();

  Shutdown(const Shutdown& from);

  inline Shutdown& operator=(const Shutdown& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Shutdown& default_instance();

  void Swap(Shutdown* other);

  // implements Message ----------------------------------------------

  Shutdown* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Shutdown& from);
  void MergeFrom(const Shutdown& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // @@protoc_insertion_point(class_scope:test.Shutdown)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_test_2eproto();
  friend void protobuf_AssignDesc_test_2eproto();
  friend void protobuf_ShutdownFile_test_2eproto();

  void InitAsDefaultInstance();
  static Shutdown* default_instance_;
};
// ===================================================================


// ===================================================================

// Empty

// -------------------------------------------------------------------

// Hello

// required string str = 1;
inline bool Hello::has_str() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Hello::set_has_str() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Hello::clear_has_str() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Hello::clear_str() {
  if (str_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    str_->clear();
  }
  clear_has_str();
}
inline const ::std::string& Hello::str() const {
  // @@protoc_insertion_point(field_get:test.Hello.str)
  return *str_;
}
inline void Hello::set_str(const ::std::string& value) {
  set_has_str();
  if (str_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    str_ = new ::std::string;
  }
  str_->assign(value);
  // @@protoc_insertion_point(field_set:test.Hello.str)
}
inline void Hello::set_str(const char* value) {
  set_has_str();
  if (str_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    str_ = new ::std::string;
  }
  str_->assign(value);
  // @@protoc_insertion_point(field_set_char:test.Hello.str)
}
inline void Hello::set_str(const char* value, size_t size) {
  set_has_str();
  if (str_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    str_ = new ::std::string;
  }
  str_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:test.Hello.str)
}
inline ::std::string* Hello::mutable_str() {
  set_has_str();
  if (str_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    str_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:test.Hello.str)
  return str_;
}
inline ::std::string* Hello::release_str() {
  clear_has_str();
  if (str_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = str_;
    str_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void Hello::set_allocated_str(::std::string* str) {
  if (str_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete str_;
  }
  if (str) {
    set_has_str();
    str_ = str;
  } else {
    clear_has_str();
    str_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:test.Hello.str)
}

// -------------------------------------------------------------------

// Shutdown


// @@protoc_insertion_point(namespace_scope)

}  // namespace test

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_test_2eproto__INCLUDED
