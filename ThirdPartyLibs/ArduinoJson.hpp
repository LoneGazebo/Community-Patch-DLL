// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2022, Benoit BLANCHON
// MIT License

#pragma once

#if __cplusplus >= 201103L
#  define ARDUINOJSON_HAS_LONG_LONG 1
#  define ARDUINOJSON_HAS_RVALUE_REFERENCES 1
#else
#  define ARDUINOJSON_HAS_LONG_LONG 0
#  define ARDUINOJSON_HAS_RVALUE_REFERENCES 0
#endif
#ifndef ARDUINOJSON_HAS_NULLPTR
#  if __cplusplus >= 201103L
#    define ARDUINOJSON_HAS_NULLPTR 1
#  else
#    define ARDUINOJSON_HAS_NULLPTR 0
#  endif
#endif
#if defined(_MSC_VER) && !ARDUINOJSON_HAS_LONG_LONG
#  define ARDUINOJSON_HAS_INT64 1
#else
#  define ARDUINOJSON_HAS_INT64 0
#endif
#ifndef ARDUINOJSON_ENABLE_STD_STREAM
#  ifdef __has_include
#    if __has_include(<istream>) && \
    __has_include(<ostream>) && \
    !defined(min) && \
    !defined(max)
#      define ARDUINOJSON_ENABLE_STD_STREAM 1
#    else
#      define ARDUINOJSON_ENABLE_STD_STREAM 0
#    endif
#  else
#    ifdef ARDUINO
#      define ARDUINOJSON_ENABLE_STD_STREAM 0
#    else
#      define ARDUINOJSON_ENABLE_STD_STREAM 1
#    endif
#  endif
#endif
#ifndef ARDUINOJSON_ENABLE_STD_STRING
#  ifdef __has_include
#    if __has_include(<string>) && !defined(min) && !defined(max)
#      define ARDUINOJSON_ENABLE_STD_STRING 1
#    else
#      define ARDUINOJSON_ENABLE_STD_STRING 0
#    endif
#  else
#    ifdef ARDUINO
#      define ARDUINOJSON_ENABLE_STD_STRING 0
#    else
#      define ARDUINOJSON_ENABLE_STD_STRING 1
#    endif
#  endif
#endif
#ifndef ARDUINOJSON_ENABLE_STRING_VIEW
#  ifdef __has_include
#    if __has_include(<string_view>) && __cplusplus >= 201703L
#      define ARDUINOJSON_ENABLE_STRING_VIEW 1
#    else
#      define ARDUINOJSON_ENABLE_STRING_VIEW 0
#    endif
#  else
#    define ARDUINOJSON_ENABLE_STRING_VIEW 0
#  endif
#endif
#ifndef ARDUINOJSON_USE_DOUBLE
#  define ARDUINOJSON_USE_DOUBLE 1
#endif
#ifndef ARDUINOJSON_USE_LONG_LONG
#  if ARDUINOJSON_HAS_LONG_LONG && defined(__SIZEOF_POINTER__) && \
          __SIZEOF_POINTER__ >= 4 ||                              \
      defined(_MSC_VER)
#    define ARDUINOJSON_USE_LONG_LONG 1
#  endif
#endif
#ifndef ARDUINOJSON_USE_LONG_LONG
#  define ARDUINOJSON_USE_LONG_LONG 0
#endif
#ifndef ARDUINOJSON_DEFAULT_NESTING_LIMIT
#  define ARDUINOJSON_DEFAULT_NESTING_LIMIT 10
#endif
#ifndef ARDUINOJSON_SLOT_OFFSET_SIZE
#  if defined(__SIZEOF_POINTER__) && __SIZEOF_POINTER__ <= 2
#    define ARDUINOJSON_SLOT_OFFSET_SIZE 1
#  elif defined(__SIZEOF_POINTER__) && __SIZEOF_POINTER__ >= 8 || \
      defined(_WIN64) && _WIN64
#    define ARDUINOJSON_SLOT_OFFSET_SIZE 4
#  else
#    define ARDUINOJSON_SLOT_OFFSET_SIZE 2
#  endif
#endif
#ifdef ARDUINO
#  ifndef ARDUINOJSON_ENABLE_ARDUINO_STRING
#    define ARDUINOJSON_ENABLE_ARDUINO_STRING 1
#  endif
#  ifndef ARDUINOJSON_ENABLE_ARDUINO_STREAM
#    define ARDUINOJSON_ENABLE_ARDUINO_STREAM 1
#  endif
#  ifndef ARDUINOJSON_ENABLE_ARDUINO_PRINT
#    define ARDUINOJSON_ENABLE_ARDUINO_PRINT 1
#  endif
#  ifndef ARDUINOJSON_ENABLE_PROGMEM
#    define ARDUINOJSON_ENABLE_PROGMEM 1
#  endif
#else  // ARDUINO
#  ifndef ARDUINOJSON_ENABLE_ARDUINO_STRING
#    define ARDUINOJSON_ENABLE_ARDUINO_STRING 0
#  endif
#  ifndef ARDUINOJSON_ENABLE_ARDUINO_STREAM
#    define ARDUINOJSON_ENABLE_ARDUINO_STREAM 0
#  endif
#  ifndef ARDUINOJSON_ENABLE_ARDUINO_PRINT
#    define ARDUINOJSON_ENABLE_ARDUINO_PRINT 0
#  endif
#  ifndef ARDUINOJSON_ENABLE_PROGMEM
#    define ARDUINOJSON_ENABLE_PROGMEM 0
#  endif
#endif  // ARDUINO
#ifndef ARDUINOJSON_DECODE_UNICODE
#  define ARDUINOJSON_DECODE_UNICODE 1
#endif
#ifndef ARDUINOJSON_ENABLE_COMMENTS
#  define ARDUINOJSON_ENABLE_COMMENTS 0
#endif
#ifndef ARDUINOJSON_ENABLE_NAN
#  define ARDUINOJSON_ENABLE_NAN 0
#endif
#ifndef ARDUINOJSON_ENABLE_INFINITY
#  define ARDUINOJSON_ENABLE_INFINITY 0
#endif
#ifndef ARDUINOJSON_POSITIVE_EXPONENTIATION_THRESHOLD
#  define ARDUINOJSON_POSITIVE_EXPONENTIATION_THRESHOLD 1e7
#endif
#ifndef ARDUINOJSON_NEGATIVE_EXPONENTIATION_THRESHOLD
#  define ARDUINOJSON_NEGATIVE_EXPONENTIATION_THRESHOLD 1e-5
#endif
#ifndef ARDUINOJSON_LITTLE_ENDIAN
#  if defined(_MSC_VER) ||                           \
      (defined(__BYTE_ORDER__) &&                    \
       __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) || \
      defined(__LITTLE_ENDIAN__) || defined(__i386) || defined(__x86_64)
#    define ARDUINOJSON_LITTLE_ENDIAN 1
#  else
#    define ARDUINOJSON_LITTLE_ENDIAN 0
#  endif
#endif
#ifndef ARDUINOJSON_ENABLE_ALIGNMENT
#  if defined(__AVR)
#    define ARDUINOJSON_ENABLE_ALIGNMENT 0
#  else
#    define ARDUINOJSON_ENABLE_ALIGNMENT 1
#  endif
#endif
#ifndef ARDUINOJSON_TAB
#  define ARDUINOJSON_TAB "  "
#endif
#ifndef ARDUINOJSON_ENABLE_STRING_DEDUPLICATION
#  define ARDUINOJSON_ENABLE_STRING_DEDUPLICATION 1
#endif
#ifndef ARDUINOJSON_STRING_BUFFER_SIZE
#  define ARDUINOJSON_STRING_BUFFER_SIZE 32
#endif
#ifndef ARDUINOJSON_DEBUG
#  ifdef __PLATFORMIO_BUILD_DEBUG__
#    define ARDUINOJSON_DEBUG 1
#  else
#    define ARDUINOJSON_DEBUG 0
#  endif
#endif
#if ARDUINOJSON_HAS_NULLPTR && defined(nullptr)
#  error nullptr is defined as a macro. Remove the faulty #define or #undef nullptr
#endif

// Include Arduino.h before stdlib.h to avoid conflict with atexit()
// https://github.com/bblanchon/ArduinoJson/pull/1693#issuecomment-1001060240
#if ARDUINOJSON_ENABLE_ARDUINO_STRING || ARDUINOJSON_ENABLE_ARDUINO_STREAM || \
    ARDUINOJSON_ENABLE_ARDUINO_PRINT || ARDUINOJSON_ENABLE_PROGMEM
#include <Arduino.h>
#endif

#if !ARDUINOJSON_DEBUG
#  ifdef __clang__
#    pragma clang system_header
#  elif defined __GNUC__
#    pragma GCC system_header
#  endif
#endif

#define ARDUINOJSON_EXPAND6(a, b, c, d, e, f) a, b, c, d, e, f
#define ARDUINOJSON_EXPAND9(a, b, c, d, e, f, g, h, i) a, b, c, d, e, f, g, h, i
#define ARDUINOJSON_EXPAND18(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, \
                             q, r)                                           \
  a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r
#define ARDUINOJSON_CONCAT_(A, B) A##B
#define ARDUINOJSON_CONCAT2(A, B) ARDUINOJSON_CONCAT_(A, B)
#define ARDUINOJSON_CONCAT4(A, B, C, D) \
  ARDUINOJSON_CONCAT2(ARDUINOJSON_CONCAT2(A, B), ARDUINOJSON_CONCAT2(C, D))
#define ARDUINOJSON_HEX_DIGIT_0000() 0
#define ARDUINOJSON_HEX_DIGIT_0001() 1
#define ARDUINOJSON_HEX_DIGIT_0010() 2
#define ARDUINOJSON_HEX_DIGIT_0011() 3
#define ARDUINOJSON_HEX_DIGIT_0100() 4
#define ARDUINOJSON_HEX_DIGIT_0101() 5
#define ARDUINOJSON_HEX_DIGIT_0110() 6
#define ARDUINOJSON_HEX_DIGIT_0111() 7
#define ARDUINOJSON_HEX_DIGIT_1000() 8
#define ARDUINOJSON_HEX_DIGIT_1001() 9
#define ARDUINOJSON_HEX_DIGIT_1010() A
#define ARDUINOJSON_HEX_DIGIT_1011() B
#define ARDUINOJSON_HEX_DIGIT_1100() C
#define ARDUINOJSON_HEX_DIGIT_1101() D
#define ARDUINOJSON_HEX_DIGIT_1110() E
#define ARDUINOJSON_HEX_DIGIT_1111() F
#define ARDUINOJSON_HEX_DIGIT_(A, B, C, D) ARDUINOJSON_HEX_DIGIT_##A##B##C##D()
#define ARDUINOJSON_HEX_DIGIT(A, B, C, D) ARDUINOJSON_HEX_DIGIT_(A, B, C, D)
#define ARDUINOJSON_VERSION "6.20.1"
#define ARDUINOJSON_VERSION_MAJOR 6
#define ARDUINOJSON_VERSION_MINOR 20
#define ARDUINOJSON_VERSION_REVISION 1
#ifndef ARDUINOJSON_NAMESPACE
#  define ARDUINOJSON_NAMESPACE                                               \
    ARDUINOJSON_CONCAT4(                                                      \
        ARDUINOJSON_CONCAT4(ArduinoJson, ARDUINOJSON_VERSION_MAJOR,           \
                            ARDUINOJSON_VERSION_MINOR,                        \
                            ARDUINOJSON_VERSION_REVISION),                    \
        _,                                                                    \
        ARDUINOJSON_HEX_DIGIT(                                                \
            ARDUINOJSON_ENABLE_PROGMEM, ARDUINOJSON_USE_LONG_LONG,            \
            ARDUINOJSON_USE_DOUBLE, ARDUINOJSON_ENABLE_STRING_DEDUPLICATION), \
        ARDUINOJSON_HEX_DIGIT(                                                \
            ARDUINOJSON_ENABLE_NAN, ARDUINOJSON_ENABLE_INFINITY,              \
            ARDUINOJSON_ENABLE_COMMENTS, ARDUINOJSON_DECODE_UNICODE))
#endif
namespace ARDUINOJSON_NAMESPACE {
template <typename T, typename Enable = void>
struct Converter;
template <typename T1, typename T2>
class InvalidConversion;  // Error here? See https://arduinojson.org/v6/invalid-conversion/
template <typename T>
struct ConverterNeedsWriteableRef;
}  // namespace ARDUINOJSON_NAMESPACE
#include <stddef.h>
#if defined(_MSC_VER) && _MSC_VER < 1600
    // MSVC 2008 and older don't have stdint.h
    #include "msinttypes/stdint.h"
#else
    // Modern compilers (including clang/LLVM) have these headers
    #include <stdint.h>
#endif
namespace ARDUINOJSON_NAMESPACE {
#if ARDUINOJSON_ENABLE_ALIGNMENT
inline bool isAligned(size_t value) {
  const size_t mask = sizeof(void*) - 1;
  size_t addr = value;
  return (addr & mask) == 0;
}
inline size_t addPadding(size_t bytes) {
  const size_t mask = sizeof(void*) - 1;
  return (bytes + mask) & ~mask;
}
template <size_t bytes>
struct AddPadding {
  static const size_t mask = sizeof(void*) - 1;
  static const size_t value = (bytes + mask) & ~mask;
};
#else
inline bool isAligned(size_t) {
  return true;
}
inline size_t addPadding(size_t bytes) {
  return bytes;
}
template <size_t bytes>
struct AddPadding {
  static const size_t value = bytes;
};
#endif
template <typename T>
inline bool isAligned(T* ptr) {
  return isAligned(reinterpret_cast<size_t>(ptr));
}
template <typename T>
inline T* addPadding(T* p) {
  size_t address = addPadding(reinterpret_cast<size_t>(p));
  return reinterpret_cast<T*>(address);
}
}  // namespace ARDUINOJSON_NAMESPACE
#if ARDUINOJSON_DEBUG
#include <assert.h>
#  define ARDUINOJSON_ASSERT(X) assert(X)
#else
#  define ARDUINOJSON_ASSERT(X) ((void)0)
#endif
namespace ARDUINOJSON_NAMESPACE {
template <size_t X, size_t Y, bool MaxIsX = (X > Y)>
struct Max {};
template <size_t X, size_t Y>
struct Max<X, Y, true> {
  static const size_t value = X;
};
template <size_t X, size_t Y>
struct Max<X, Y, false> {
  static const size_t value = Y;
};
template <bool Condition, class TrueType, class FalseType>
struct conditional {
  typedef TrueType type;
};
template <class TrueType, class FalseType>
struct conditional<false, TrueType, FalseType> {
  typedef FalseType type;
};
template <bool Condition, typename T = void>
struct enable_if {};
template <typename T>
struct enable_if<true, T> {
  typedef T type;
};
template <typename T, T v>
struct integral_constant {
  static const T value = v;
};
typedef integral_constant<bool, true> true_type;
typedef integral_constant<bool, false> false_type;
template <typename T>
struct is_array : false_type {};
template <typename T>
struct is_array<T[]> : true_type {};
template <typename T, size_t N>
struct is_array<T[N]> : true_type {};
template <typename TBase, typename TDerived>
class is_base_of {
 protected:  // <- to avoid GCC's "all member functions in class are private"
  static int probe(const TBase*);
  static char probe(...);
 public:
  static const bool value =
      sizeof(probe(reinterpret_cast<TDerived*>(0))) == sizeof(int);
};
template <typename T>
T declval();
template <typename T>
struct is_class {
 protected:  // <- to avoid GCC's "all member functions in class are private"
  template <typename U>
  static int probe(void (U::*)(void));
  template <typename>
  static char probe(...);
 public:
  static const bool value = sizeof(probe<T>(0)) == sizeof(int);
};
template <typename T>
struct is_const : false_type {};
template <typename T>
struct is_const<const T> : true_type {};
}  // namespace ARDUINOJSON_NAMESPACE
#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4244)
#endif
#ifdef __ICCARM__
#pragma diag_suppress=Pa093
#endif
namespace ARDUINOJSON_NAMESPACE {
template <typename From, typename To>
struct is_convertible {
 protected:  // <- to avoid GCC's "all member functions in class are private"
  static int probe(To);
  static char probe(...);
  static From& _from;
 public:
  static const bool value = sizeof(probe(_from)) == sizeof(int);
};
}  // namespace ARDUINOJSON_NAMESPACE
#ifdef _MSC_VER
#  pragma warning(pop)
#endif
#ifdef __ICCARM__
#pragma diag_default=Pa093
#endif
namespace ARDUINOJSON_NAMESPACE {
template <typename T, typename U>
struct is_same : false_type {};
template <typename T>
struct is_same<T, T> : true_type {};
template <typename T>
struct remove_cv {
  typedef T type;
};
template <typename T>
struct remove_cv<const T> {
  typedef T type;
};
template <typename T>
struct remove_cv<volatile T> {
  typedef T type;
};
template <typename T>
struct remove_cv<const volatile T> {
  typedef T type;
};
template <class T>
struct is_floating_point
    : integral_constant<
          bool,  //
          is_same<float, typename remove_cv<T>::type>::value ||
              is_same<double, typename remove_cv<T>::type>::value> {};
template <typename T>
struct is_integral : integral_constant<bool,
    is_same<typename remove_cv<T>::type, signed char>::value ||
    is_same<typename remove_cv<T>::type, unsigned char>::value ||
    is_same<typename remove_cv<T>::type, signed short>::value ||
    is_same<typename remove_cv<T>::type, unsigned short>::value ||
    is_same<typename remove_cv<T>::type, signed int>::value ||
    is_same<typename remove_cv<T>::type, unsigned int>::value ||
    is_same<typename remove_cv<T>::type, signed long>::value ||
    is_same<typename remove_cv<T>::type, unsigned long>::value ||
#if ARDUINOJSON_HAS_LONG_LONG
    is_same<typename remove_cv<T>::type, signed long long>::value ||
    is_same<typename remove_cv<T>::type, unsigned long long>::value ||
#endif
#if ARDUINOJSON_HAS_INT64
    is_same<typename remove_cv<T>::type, signed __int64>::value ||
    is_same<typename remove_cv<T>::type, unsigned __int64>::value ||
#endif
    is_same<typename remove_cv<T>::type, char>::value ||
    is_same<typename remove_cv<T>::type, bool>::value> {};
template <typename T>
struct is_enum {
  static const bool value = is_convertible<T, int>::value &&
                            !is_class<T>::value && !is_integral<T>::value &&
                            !is_floating_point<T>::value;
};
template <typename T>
struct is_pointer : false_type {};
template <typename T>
struct is_pointer<T*> : true_type {};
template <typename T>
struct is_signed : integral_constant<bool, 
    is_same<typename remove_cv<T>::type, char>::value ||
    is_same<typename remove_cv<T>::type, signed char>::value ||
    is_same<typename remove_cv<T>::type, signed short>::value ||
    is_same<typename remove_cv<T>::type, signed int>::value ||
    is_same<typename remove_cv<T>::type, signed long>::value ||
#if ARDUINOJSON_HAS_LONG_LONG
    is_same<typename remove_cv<T>::type, signed long long>::value ||
#endif
#if ARDUINOJSON_HAS_INT64
    is_same<typename remove_cv<T>::type, signed __int64>::value ||
#endif
    is_same<typename remove_cv<T>::type, float>::value ||
    is_same<typename remove_cv<T>::type, double>::value> {};
template <typename T>
struct is_unsigned : integral_constant<bool,
    is_same<typename remove_cv<T>::type, unsigned char>::value ||
    is_same<typename remove_cv<T>::type, unsigned short>::value ||
    is_same<typename remove_cv<T>::type, unsigned int>::value ||
    is_same<typename remove_cv<T>::type, unsigned long>::value ||
#if ARDUINOJSON_HAS_INT64
    is_same<typename remove_cv<T>::type, unsigned __int64>::value ||
#endif
#if ARDUINOJSON_HAS_LONG_LONG
    is_same<typename remove_cv<T>::type, unsigned long long>::value ||
#endif
    is_same<typename remove_cv<T>::type, bool>::value> {};
template <typename T>
struct type_identity {
  typedef T type;
};
template <typename T>
struct make_unsigned;
template <>
struct make_unsigned<char> : type_identity<unsigned char> {};
template <>
struct make_unsigned<signed char> : type_identity<unsigned char> {};
template <>
struct make_unsigned<unsigned char> : type_identity<unsigned char> {};
template <>
struct make_unsigned<signed short> : type_identity<unsigned short> {};
template <>
struct make_unsigned<unsigned short> : type_identity<unsigned short> {};
template <>
struct make_unsigned<signed int> : type_identity<unsigned int> {};
template <>
struct make_unsigned<unsigned int> : type_identity<unsigned int> {};
template <>
struct make_unsigned<signed long> : type_identity<unsigned long> {};
template <>
struct make_unsigned<unsigned long> : type_identity<unsigned long> {};
#if ARDUINOJSON_HAS_LONG_LONG
template <>
struct make_unsigned<signed long long> : type_identity<unsigned long long> {};
template <>
struct make_unsigned<unsigned long long> : type_identity<unsigned long long> {};
#endif
#if ARDUINOJSON_HAS_INT64
template <>
struct make_unsigned<signed __int64> : type_identity<unsigned __int64> {};
template <>
struct make_unsigned<unsigned __int64> : type_identity<unsigned __int64> {};
#endif
template <class = void>
struct make_void {
  typedef void type;
};
template <typename T>
struct remove_const {
  typedef T type;
};
template <typename T>
struct remove_const<const T> {
  typedef T type;
};
template <typename T>
struct remove_reference {
  typedef T type;
};
template <typename T>
struct remove_reference<T&> {
  typedef T type;
};
}  // namespace ARDUINOJSON_NAMESPACE
#include <string.h>
namespace ARDUINOJSON_NAMESPACE {
namespace StringStoragePolicy {
struct Link {};
struct Copy {};
struct LinkOrCopy {
  bool link;
};
}  // namespace StringStoragePolicy
template <typename TString, typename Enable = void>
struct StringAdapter;
template <typename TString, typename Enable = void>
struct SizedStringAdapter;
template <typename TString>
typename StringAdapter<TString>::AdaptedString adaptString(const TString& s) {
  return StringAdapter<TString>::adapt(s);
}
template <typename TChar>
typename StringAdapter<TChar*>::AdaptedString adaptString(TChar* p) {
  return StringAdapter<TChar*>::adapt(p);
}
template <typename TChar>
typename SizedStringAdapter<TChar*>::AdaptedString adaptString(TChar* p,
                                                               size_t n) {
  return SizedStringAdapter<TChar*>::adapt(p, n);
}
template <typename T>
struct IsChar
    : integral_constant<bool, is_integral<T>::value && sizeof(T) == 1> {};
class ZeroTerminatedRamString {
 public:
  static const size_t typeSortKey = 3;
  ZeroTerminatedRamString(const char* str) : _str(str) {}
  bool isNull() const {
    return !_str;
  }
  size_t size() const {
    return _str ? ::strlen(_str) : 0;
  }
  char operator[](size_t i) const {
    ARDUINOJSON_ASSERT(_str != 0);
    ARDUINOJSON_ASSERT(i <= size());
    return _str[i];
  }
  const char* data() const {
    return _str;
  }
  friend int stringCompare(ZeroTerminatedRamString a,
                           ZeroTerminatedRamString b) {
    ARDUINOJSON_ASSERT(!a.isNull());
    ARDUINOJSON_ASSERT(!b.isNull());
    return ::strcmp(a._str, b._str);
  }
  friend bool stringEquals(ZeroTerminatedRamString a,
                           ZeroTerminatedRamString b) {
    return stringCompare(a, b) == 0;
  }
  StringStoragePolicy::Copy storagePolicy() const {
    return StringStoragePolicy::Copy();
  }
 protected:
  const char* _str;
};
template <typename TChar>
struct StringAdapter<TChar*, typename enable_if<IsChar<TChar>::value>::type> {
  typedef ZeroTerminatedRamString AdaptedString;
  static AdaptedString adapt(const TChar* p) {
    return AdaptedString(reinterpret_cast<const char*>(p));
  }
};
template <typename TChar, size_t N>
struct StringAdapter<TChar[N], typename enable_if<IsChar<TChar>::value>::type> {
  typedef ZeroTerminatedRamString AdaptedString;
  static AdaptedString adapt(const TChar* p) {
    return AdaptedString(reinterpret_cast<const char*>(p));
  }
};
class StaticStringAdapter : public ZeroTerminatedRamString {
 public:
  StaticStringAdapter(const char* str) : ZeroTerminatedRamString(str) {}
  StringStoragePolicy::Link storagePolicy() const {
    return StringStoragePolicy::Link();
  }
};
template <>
struct StringAdapter<const char*, void> {
  typedef StaticStringAdapter AdaptedString;
  static AdaptedString adapt(const char* p) {
    return AdaptedString(p);
  }
};
class SizedRamString {
 public:
  static const size_t typeSortKey = 2;
  SizedRamString(const char* str, size_t sz) : _str(str), _size(sz) {}
  bool isNull() const {
    return !_str;
  }
  size_t size() const {
    return _size;
  }
  char operator[](size_t i) const {
    ARDUINOJSON_ASSERT(_str != 0);
    ARDUINOJSON_ASSERT(i <= size());
    return _str[i];
  }
  const char* data() const {
    return _str;
  }
  StringStoragePolicy::Copy storagePolicy() const {
    return StringStoragePolicy::Copy();
  }
 protected:
  const char* _str;
  size_t _size;
};
template <typename TChar>
struct SizedStringAdapter<TChar*,
                          typename enable_if<IsChar<TChar>::value>::type> {
  typedef SizedRamString AdaptedString;
  static AdaptedString adapt(const TChar* p, size_t n) {
    return AdaptedString(reinterpret_cast<const char*>(p), n);
  }
};
template <typename T>
class SafeBoolIdom {
 protected:
  typedef void (T::*bool_type)() const;
  void safeBoolHelper() const {}
  static bool_type safe_true() {
    return &SafeBoolIdom::safeBoolHelper;
  }
  static bool_type safe_false() {
    return 0;
  }
};
}  // namespace ARDUINOJSON_NAMESPACE
#if ARDUINOJSON_ENABLE_STD_STREAM
#include <ostream>
#endif
namespace ARDUINOJSON_NAMESPACE {
class JsonString : public SafeBoolIdom<JsonString> {
 public:
  enum Ownership { Copied, Linked };
  JsonString() : _data(0), _size(0), _ownership(Linked) {}
  JsonString(const char* data, Ownership ownership = Linked)
      : _data(data), _size(data ? ::strlen(data) : 0), _ownership(ownership) {}
  JsonString(const char* data, size_t sz, Ownership ownership = Linked)
      : _data(data), _size(sz), _ownership(ownership) {}
  const char* c_str() const {
    return _data;
  }
  bool isNull() const {
    return !_data;
  }
  bool isLinked() const {
    return _ownership == Linked;
  }
  size_t size() const {
    return _size;
  }
  operator bool_type() const {
    return _data ? safe_true() : safe_false();
  }
  friend bool operator==(JsonString lhs, JsonString rhs) {
    if (lhs._size != rhs._size)
      return false;
    if (lhs._data == rhs._data)
      return true;
    if (!lhs._data)
      return false;
    if (!rhs._data)
      return false;
    return memcmp(lhs._data, rhs._data, lhs._size) == 0;
  }
  friend bool operator!=(JsonString lhs, JsonString rhs) {
    return !(lhs == rhs);
  }
#if ARDUINOJSON_ENABLE_STD_STREAM
  friend std::ostream& operator<<(std::ostream& lhs, const JsonString& rhs) {
    lhs.write(rhs.c_str(), static_cast<std::streamsize>(rhs.size()));
    return lhs;
  }
#endif
 private:
  const char* _data;
  size_t _size;
  Ownership _ownership;
};
class JsonStringAdapter : public SizedRamString {
 public:
  JsonStringAdapter(const JsonString& s)
      : SizedRamString(s.c_str(), s.size()), _linked(s.isLinked()) {}
  StringStoragePolicy::LinkOrCopy storagePolicy() const {
    StringStoragePolicy::LinkOrCopy policy = {_linked};
    return policy;
  }
 private:
  bool _linked;
};
template <>
struct StringAdapter<JsonString> {
  typedef JsonStringAdapter AdaptedString;
  static AdaptedString adapt(const JsonString& s) {
    return AdaptedString(s);
  }
};
}  // namespace ARDUINOJSON_NAMESPACE
#if ARDUINOJSON_ENABLE_STD_STRING
#include <string>
namespace ARDUINOJSON_NAMESPACE {
template <typename TCharTraits, typename TAllocator>
struct StringAdapter<std::basic_string<char, TCharTraits, TAllocator>, void> {
  typedef SizedRamString AdaptedString;
  static AdaptedString adapt(
      const std::basic_string<char, TCharTraits, TAllocator>& s) {
    return AdaptedString(s.c_str(), s.size());
  }
};
}  // namespace ARDUINOJSON_NAMESPACE
#endif
#if ARDUINOJSON_ENABLE_STRING_VIEW
#include <string_view>
namespace ARDUINOJSON_NAMESPACE {
template <>
struct StringAdapter<std::string_view, void> {
  typedef SizedRamString AdaptedString;
  static AdaptedString adapt(const std::string_view& s) {
    return AdaptedString(s.data(), s.size());
  }
};
}  // namespace ARDUINOJSON_NAMESPACE
#endif
#if ARDUINOJSON_ENABLE_ARDUINO_STRING
namespace ARDUINOJSON_NAMESPACE {
template <typename T>
struct StringAdapter<
    T, typename enable_if<is_same<T, ::String>::value ||
                          is_same<T, ::StringSumHelper>::value>::type> {
  typedef SizedRamString AdaptedString;
  static AdaptedString adapt(const ::String& s) {
    return AdaptedString(s.c_str(), s.length());
  }
};
}  // namespace ARDUINOJSON_NAMESPACE
#endif
#if ARDUINOJSON_ENABLE_PROGMEM
namespace ARDUINOJSON_NAMESPACE {
struct pgm_p {
  pgm_p(const void* p) : address(reinterpret_cast<const char*>(p)) {}
  const char* address;
};
}  // namespace ARDUINOJSON_NAMESPACE
#ifndef strlen_P
inline size_t strlen_P(ARDUINOJSON_NAMESPACE::pgm_p s) {
  const char* p = s.address;
  ARDUINOJSON_ASSERT(p != NULL);
  while (pgm_read_byte(p))
    p++;
  return size_t(p - s.address);
}
#endif
#ifndef strncmp_P
inline int strncmp_P(const char* a, ARDUINOJSON_NAMESPACE::pgm_p b, size_t n) {
  const char* s1 = a;
  const char* s2 = b.address;
  ARDUINOJSON_ASSERT(s1 != NULL);
  ARDUINOJSON_ASSERT(s2 != NULL);
  while (n-- > 0) {
    char c1 = *s1++;
    char c2 = static_cast<char>(pgm_read_byte(s2++));
    if (c1 < c2)
      return -1;
    if (c1 > c2)
      return 1;
    if (c1 == 0 /* and c2 as well */)
      return 0;
  }
  return 0;
}
#endif
#ifndef strcmp_P
inline int strcmp_P(const char* a, ARDUINOJSON_NAMESPACE::pgm_p b) {
  const char* s1 = a;
  const char* s2 = b.address;
  ARDUINOJSON_ASSERT(s1 != NULL);
  ARDUINOJSON_ASSERT(s2 != NULL);
  for (;;) {
    char c1 = *s1++;
    char c2 = static_cast<char>(pgm_read_byte(s2++));
    if (c1 < c2)
      return -1;
    if (c1 > c2)
      return 1;
    if (c1 == 0 /* and c2 as well */)
      return 0;
  }
}
#endif
#ifndef memcmp_P
inline int memcmp_P(const void* a, ARDUINOJSON_NAMESPACE::pgm_p b, size_t n) {
  const uint8_t* p1 = reinterpret_cast<const uint8_t*>(a);
  const char* p2 = b.address;
  ARDUINOJSON_ASSERT(p1 != NULL);
  ARDUINOJSON_ASSERT(p2 != NULL);
  while (n-- > 0) {
    uint8_t v1 = *p1++;
    uint8_t v2 = pgm_read_byte(p2++);
    if (v1 != v2)
      return v1 - v2;
  }
  return 0;
}
#endif
#ifndef memcpy_P
inline void* memcpy_P(void* dst, ARDUINOJSON_NAMESPACE::pgm_p src, size_t n) {
  uint8_t* d = reinterpret_cast<uint8_t*>(dst);
  const char* s = src.address;
  ARDUINOJSON_ASSERT(d != NULL);
  ARDUINOJSON_ASSERT(s != NULL);
  while (n-- > 0) {
    *d++ = pgm_read_byte(s++);
  }
  return dst;
}
#endif
#ifndef pgm_read_dword
inline uint32_t pgm_read_dword(ARDUINOJSON_NAMESPACE::pgm_p p) {
  uint32_t result;
  memcpy_P(&result, p.address, 4);
  return result;
}
#endif
#ifndef pgm_read_ptr
inline void* pgm_read_ptr(ARDUINOJSON_NAMESPACE::pgm_p p) {
  void* result;
  memcpy_P(&result, p.address, sizeof(result));
  return result;
}
#endif
namespace ARDUINOJSON_NAMESPACE {
class FlashString {
 public:
  static const size_t typeSortKey = 1;
  FlashString(const __FlashStringHelper* str, size_t sz)
      : _str(reinterpret_cast<const char*>(str)), _size(sz) {}
  bool isNull() const {
    return !_str;
  }
  char operator[](size_t i) const {
    ARDUINOJSON_ASSERT(_str != 0);
    ARDUINOJSON_ASSERT(i <= _size);
    return static_cast<char>(pgm_read_byte(_str + i));
  }
  size_t size() const {
    return _size;
  }
  friend bool stringEquals(FlashString a, SizedRamString b) {
    ARDUINOJSON_ASSERT(a.typeSortKey < b.typeSortKey);
    ARDUINOJSON_ASSERT(!a.isNull());
    ARDUINOJSON_ASSERT(!b.isNull());
    if (a.size() != b.size())
      return false;
    return ::memcmp_P(b.data(), a._str, a._size) == 0;
  }
  friend int stringCompare(FlashString a, SizedRamString b) {
    ARDUINOJSON_ASSERT(a.typeSortKey < b.typeSortKey);
    ARDUINOJSON_ASSERT(!a.isNull());
    ARDUINOJSON_ASSERT(!b.isNull());
    size_t minsize = a.size() < b.size() ? a.size() : b.size();
    int res = ::memcmp_P(b.data(), a._str, minsize);
    if (res)
      return -res;
    if (a.size() < b.size())
      return -1;
    if (a.size() > b.size())
      return 1;
    return 0;
  }
  friend void stringGetChars(FlashString s, char* p, size_t n) {
    ARDUINOJSON_ASSERT(s.size() <= n);
    ::memcpy_P(p, s._str, n);
  }
  StringStoragePolicy::Copy storagePolicy() const {
    return StringStoragePolicy::Copy();
  }
 private:
  const char* _str;
  size_t _size;
};
template <>
struct StringAdapter<const __FlashStringHelper*, void> {
  typedef FlashString AdaptedString;
  static AdaptedString adapt(const __FlashStringHelper* s) {
    return AdaptedString(s, s ? strlen_P(reinterpret_cast<const char*>(s)) : 0);
  }
};
template <>
struct SizedStringAdapter<const __FlashStringHelper*, void> {
  typedef FlashString AdaptedString;
  static AdaptedString adapt(const __FlashStringHelper* s, size_t n) {
    return AdaptedString(s, n);
  }
};
}  // namespace ARDUINOJSON_NAMESPACE
#endif
namespace ARDUINOJSON_NAMESPACE {
template <typename TAdaptedString1, typename TAdaptedString2>
typename enable_if<TAdaptedString1::typeSortKey <= TAdaptedString2::typeSortKey,
                   int>::type
stringCompare(TAdaptedString1 s1, TAdaptedString2 s2) {
  ARDUINOJSON_ASSERT(!s1.isNull());
  ARDUINOJSON_ASSERT(!s2.isNull());
  size_t size1 = s1.size();
  size_t size2 = s2.size();
  size_t n = size1 < size2 ? size1 : size2;
  for (size_t i = 0; i < n; i++) {
    if (s1[i] != s2[i])
      return s1[i] - s2[i];
  }
  if (size1 < size2)
    return -1;
  if (size1 > size2)
    return 1;
  return 0;
}
template <typename TAdaptedString1, typename TAdaptedString2>
typename enable_if<
    (TAdaptedString1::typeSortKey > TAdaptedString2::typeSortKey), int>::type
stringCompare(TAdaptedString1 s1, TAdaptedString2 s2) {
  return -stringCompare(s2, s1);
}
template <typename TAdaptedString1, typename TAdaptedString2>
typename enable_if<TAdaptedString1::typeSortKey <= TAdaptedString2::typeSortKey,
                   bool>::type
stringEquals(TAdaptedString1 s1, TAdaptedString2 s2) {
  ARDUINOJSON_ASSERT(!s1.isNull());
  ARDUINOJSON_ASSERT(!s2.isNull());
  size_t size1 = s1.size();
  size_t size2 = s2.size();
  if (size1 != size2)
    return false;
  for (size_t i = 0; i < size1; i++) {
    if (s1[i] != s2[i])
      return false;
  }
  return true;
}
template <typename TAdaptedString1, typename TAdaptedString2>
typename enable_if<
    (TAdaptedString1::typeSortKey > TAdaptedString2::typeSortKey), bool>::type
stringEquals(TAdaptedString1 s1, TAdaptedString2 s2) {
  return stringEquals(s2, s1);
}
template <typename TAdaptedString>
static void stringGetChars(TAdaptedString s, char* p, size_t n) {
  ARDUINOJSON_ASSERT(s.size() <= n);
  for (size_t i = 0; i < n; i++) {
    p[i] = s[i];
  }
}
template <int Bits>
struct int_t;
template <>
struct int_t<8> {
  typedef int8_t type;
};
template <>
struct int_t<16> {
  typedef int16_t type;
};
template <>
struct int_t<32> {
  typedef int32_t type;
};
}  // namespace ARDUINOJSON_NAMESPACE
#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4310)
#endif
namespace ARDUINOJSON_NAMESPACE {
template <typename T, typename Enable = void>
struct numeric_limits;
template <typename T>
struct numeric_limits<T, typename enable_if<is_unsigned<T>::value>::type> {
  static T lowest() {
    return 0;
  }
  static T highest() {
    return T(-1);
  }
};
template <typename T>
struct numeric_limits<
    T, typename enable_if<is_integral<T>::value && is_signed<T>::value>::type> {
  static T lowest() {
    return T(T(1) << (sizeof(T) * 8 - 1));
  }
  static T highest() {
    return T(~lowest());
  }
};
}  // namespace ARDUINOJSON_NAMESPACE
#ifdef _MSC_VER
#  pragma warning(pop)
#endif
namespace ARDUINOJSON_NAMESPACE {
class MemoryPool;
class VariantData;
class VariantSlot;
class CollectionData {
  VariantSlot* _head;
  VariantSlot* _tail;
 public:
  VariantData* addElement(MemoryPool* pool);
  VariantData* getElement(size_t index) const;
  VariantData* getOrAddElement(size_t index, MemoryPool* pool);
  void removeElement(size_t index);
  template <typename TAdaptedString>
  VariantData* addMember(TAdaptedString key, MemoryPool* pool);
  template <typename TAdaptedString>
  VariantData* getMember(TAdaptedString key) const;
  template <typename TAdaptedString>
  VariantData* getOrAddMember(TAdaptedString key, MemoryPool* pool);
  template <typename TAdaptedString>
  void removeMember(TAdaptedString key) {
    removeSlot(getSlot(key));
  }
  template <typename TAdaptedString>
  bool containsKey(const TAdaptedString& key) const;
  void clear();
  size_t memoryUsage() const;
  size_t size() const;
  VariantSlot* addSlot(MemoryPool*);
  void removeSlot(VariantSlot* slot);
  bool copyFrom(const CollectionData& src, MemoryPool* pool);
  VariantSlot* head() const {
    return _head;
  }
  void movePointers(ptrdiff_t stringDistance, ptrdiff_t variantDistance);
 private:
  VariantSlot* getSlot(size_t index) const;
  template <typename TAdaptedString>
  VariantSlot* getSlot(TAdaptedString key) const;
  VariantSlot* getPreviousSlot(VariantSlot*) const;
};
inline const VariantData* collectionToVariant(
    const CollectionData* collection) {
  const void* data = collection;  // prevent warning cast-align
  return reinterpret_cast<const VariantData*>(data);
}
inline VariantData* collectionToVariant(CollectionData* collection) {
  void* data = collection;  // prevent warning cast-align
  return reinterpret_cast<VariantData*>(data);
}
#if ARDUINOJSON_USE_DOUBLE
typedef double JsonFloat;
#else
typedef float JsonFloat;
#endif
#if ARDUINOJSON_USE_LONG_LONG
typedef int64_t JsonInteger;
typedef uint64_t JsonUInt;
#else
typedef long JsonInteger;
typedef unsigned long JsonUInt;
#endif
}  // namespace ARDUINOJSON_NAMESPACE
#if ARDUINOJSON_HAS_LONG_LONG && !ARDUINOJSON_USE_LONG_LONG
#  define ARDUINOJSON_ASSERT_INTEGER_TYPE_IS_SUPPORTED(T)                  \
    static_assert(sizeof(T) <= sizeof(ARDUINOJSON_NAMESPACE::JsonInteger), \
                  "To use 64-bit integers with ArduinoJson, you must set " \
                  "ARDUINOJSON_USE_LONG_LONG to 1. See "                   \
                  "https://arduinojson.org/v6/api/config/use_long_long/");
#else
#  define ARDUINOJSON_ASSERT_INTEGER_TYPE_IS_SUPPORTED(T)
#endif
namespace ARDUINOJSON_NAMESPACE {
enum {
  VALUE_MASK = 0x7F,
  OWNED_VALUE_BIT = 0x01,
  VALUE_IS_NULL = 0,
  VALUE_IS_LINKED_RAW = 0x02,
  VALUE_IS_OWNED_RAW = 0x03,
  VALUE_IS_LINKED_STRING = 0x04,
  VALUE_IS_OWNED_STRING = 0x05,
  VALUE_IS_BOOLEAN = 0x06,
  NUMBER_BIT = 0x08,
  VALUE_IS_UNSIGNED_INTEGER = 0x08,
  VALUE_IS_SIGNED_INTEGER = 0x0A,
  VALUE_IS_FLOAT = 0x0C,
  COLLECTION_MASK = 0x60,
  VALUE_IS_OBJECT = 0x20,
  VALUE_IS_ARRAY = 0x40,
  OWNED_KEY_BIT = 0x80
};
struct RawData {
  const char* data;
  size_t size;
};
union VariantContent {
  JsonFloat asFloat;
  bool asBoolean;
  JsonUInt asUnsignedInteger;
  JsonInteger asSignedInteger;
  CollectionData asCollection;
  struct {
    const char* data;
    size_t size;
  } asString;
};
typedef int_t<ARDUINOJSON_SLOT_OFFSET_SIZE * 8>::type VariantSlotDiff;
class VariantSlot {
  VariantContent _content;
  uint8_t _flags;
  VariantSlotDiff _next;
  const char* _key;
 public:
  VariantData* data() {
    return reinterpret_cast<VariantData*>(&_content);
  }
  const VariantData* data() const {
    return reinterpret_cast<const VariantData*>(&_content);
  }
  VariantSlot* next() {
    return _next ? this + _next : 0;
  }
  const VariantSlot* next() const {
    return const_cast<VariantSlot*>(this)->next();
  }
  VariantSlot* next(size_t distance) {
    VariantSlot* slot = this;
    while (distance--) {
      if (!slot->_next)
        return 0;
      slot += slot->_next;
    }
    return slot;
  }
  const VariantSlot* next(size_t distance) const {
    return const_cast<VariantSlot*>(this)->next(distance);
  }
  void setNext(VariantSlot* slot) {
    ARDUINOJSON_ASSERT(!slot || slot - this >=
                                    numeric_limits<VariantSlotDiff>::lowest());
    ARDUINOJSON_ASSERT(!slot || slot - this <=
                                    numeric_limits<VariantSlotDiff>::highest());
    _next = VariantSlotDiff(slot ? slot - this : 0);
  }
  void setNextNotNull(VariantSlot* slot) {
    ARDUINOJSON_ASSERT(slot != 0);
    ARDUINOJSON_ASSERT(slot - this >=
                       numeric_limits<VariantSlotDiff>::lowest());
    ARDUINOJSON_ASSERT(slot - this <=
                       numeric_limits<VariantSlotDiff>::highest());
    _next = VariantSlotDiff(slot - this);
  }
  void setKey(JsonString k) {
    ARDUINOJSON_ASSERT(k);
    if (k.isLinked())
      _flags &= VALUE_MASK;
    else
      _flags |= OWNED_KEY_BIT;
    _key = k.c_str();
  }
  const char* key() const {
    return _key;
  }
  bool ownsKey() const {
    return (_flags & OWNED_KEY_BIT) != 0;
  }
  void clear() {
    _next = 0;
    _flags = 0;
    _key = 0;
  }
  void movePointers(ptrdiff_t stringDistance, ptrdiff_t variantDistance) {
    if (_flags & OWNED_KEY_BIT)
      _key += stringDistance;
    if (_flags & OWNED_VALUE_BIT)
      _content.asString.data += stringDistance;
    if (_flags & COLLECTION_MASK)
      _content.asCollection.movePointers(stringDistance, variantDistance);
  }
};
}  // namespace ARDUINOJSON_NAMESPACE
#define JSON_STRING_SIZE(SIZE) (SIZE + 1)
#define JSON_ARRAY_SIZE(NUMBER_OF_ELEMENTS) \
  ((NUMBER_OF_ELEMENTS) * sizeof(ARDUINOJSON_NAMESPACE::VariantSlot))
#define JSON_OBJECT_SIZE(NUMBER_OF_ELEMENTS) \
  ((NUMBER_OF_ELEMENTS) * sizeof(ARDUINOJSON_NAMESPACE::VariantSlot))
namespace ARDUINOJSON_NAMESPACE {
class MemoryPool {
 public:
  MemoryPool(char* buf, size_t capa)
      : _begin(buf),
        _left(buf),
        _right(buf ? buf + capa : 0),
        _end(buf ? buf + capa : 0),
        _overflowed(false) {
    ARDUINOJSON_ASSERT(isAligned(_begin));
    ARDUINOJSON_ASSERT(isAligned(_right));
    ARDUINOJSON_ASSERT(isAligned(_end));
  }
  void* buffer() {
    return _begin;  // NOLINT(clang-analyzer-unix.Malloc)
  }
  size_t capacity() const {
    return size_t(_end - _begin);
  }
  size_t size() const {
    return size_t(_left - _begin + _end - _right);
  }
  bool overflowed() const {
    return _overflowed;
  }
  VariantSlot* allocVariant() {
    return allocRight<VariantSlot>();
  }
  template <typename TAdaptedString>
  const char* saveString(TAdaptedString str) {
    if (str.isNull())
      return 0;
#if ARDUINOJSON_ENABLE_STRING_DEDUPLICATION
    const char* existingCopy = findString(str);
    if (existingCopy)
      return existingCopy;
#endif
    size_t n = str.size();
    char* newCopy = allocString(n + 1);
    if (newCopy) {
      stringGetChars(str, newCopy, n);
      newCopy[n] = 0;  // force null-terminator
    }
    return newCopy;
  }
  void getFreeZone(char** zoneStart, size_t* zoneSize) const {
    *zoneStart = _left;
    *zoneSize = size_t(_right - _left);
  }
  const char* saveStringFromFreeZone(size_t len) {
#if ARDUINOJSON_ENABLE_STRING_DEDUPLICATION
    const char* dup = findString(adaptString(_left, len));
    if (dup)
      return dup;
#endif
    const char* str = _left;
    _left += len;
    *_left++ = 0;
    checkInvariants();
    return str;
  }
  void markAsOverflowed() {
    _overflowed = true;
  }
  void clear() {
    _left = _begin;
    _right = _end;
    _overflowed = false;
  }
  bool canAlloc(size_t bytes) const {
    return _left + bytes <= _right;
  }
  bool owns(void* p) const {
    return _begin <= p && p < _end;
  }
  void* operator new(size_t, void* p) {
    return p;
  }
  ptrdiff_t squash() {
    char* new_right = addPadding(_left);
    if (new_right >= _right)
      return 0;
    size_t right_size = static_cast<size_t>(_end - _right);
    memmove(new_right, _right, right_size);
    ptrdiff_t bytes_reclaimed = _right - new_right;
    _right = new_right;
    _end = new_right + right_size;
    return bytes_reclaimed;
  }
  void movePointers(ptrdiff_t offset) {
    _begin += offset;
    _left += offset;
    _right += offset;
    _end += offset;
  }
 private:
  void checkInvariants() {
    ARDUINOJSON_ASSERT(_begin <= _left);
    ARDUINOJSON_ASSERT(_left <= _right);
    ARDUINOJSON_ASSERT(_right <= _end);
    ARDUINOJSON_ASSERT(isAligned(_right));
  }
#if ARDUINOJSON_ENABLE_STRING_DEDUPLICATION
  template <typename TAdaptedString>
  const char* findString(const TAdaptedString& str) const {
    size_t n = str.size();
    for (char* next = _begin; next + n < _left; ++next) {
      if (next[n] == '\0' && stringEquals(str, adaptString(next, n)))
        return next;
      while (*next)
        ++next;
    }
    return 0;
  }
#endif
  char* allocString(size_t n) {
    if (!canAlloc(n)) {
      _overflowed = true;
      return 0;
    }
    char* s = _left;
    _left += n;
    checkInvariants();
    return s;
  }
  template <typename T>
  T* allocRight() {
    return reinterpret_cast<T*>(allocRight(sizeof(T)));
  }
  void* allocRight(size_t bytes) {
    if (!canAlloc(bytes)) {
      _overflowed = true;
      return 0;
    }
    _right -= bytes;
    return _right;
  }
  char *_begin, *_left, *_right, *_end;
  bool _overflowed;
};
template <typename TAdaptedString, typename TCallback>
bool storeString(MemoryPool* pool, TAdaptedString str,
                 StringStoragePolicy::Copy, TCallback callback) {
  const char* copy = pool->saveString(str);
  JsonString storedString(copy, str.size(), JsonString::Copied);
  callback(storedString);
  return copy != 0;
}
template <typename TAdaptedString, typename TCallback>
bool storeString(MemoryPool*, TAdaptedString str, StringStoragePolicy::Link,
                 TCallback callback) {
  JsonString storedString(str.data(), str.size(), JsonString::Linked);
  callback(storedString);
  return !str.isNull();
}
template <typename TAdaptedString, typename TCallback>
bool storeString(MemoryPool* pool, TAdaptedString str,
                 StringStoragePolicy::LinkOrCopy policy, TCallback callback) {
  if (policy.link)
    return storeString(pool, str, StringStoragePolicy::Link(), callback);
  else
    return storeString(pool, str, StringStoragePolicy::Copy(), callback);
}
template <typename TAdaptedString, typename TCallback>
bool storeString(MemoryPool* pool, TAdaptedString str, TCallback callback) {
  return storeString(pool, str, str.storagePolicy(), callback);
}
template <typename T, typename Enable = void>
struct IsString : false_type {};
template <typename T>
struct IsString<
    T, typename make_void<typename StringAdapter<T>::AdaptedString>::type>
    : true_type {};
}  // namespace ARDUINOJSON_NAMESPACE
#ifdef _MSC_VER  // Visual Studio
#  define FORCE_INLINE  // __forceinline causes C4714 when returning std::string
#  define NO_INLINE __declspec(noinline)
#elif defined(__GNUC__)  // GCC or Clang
#  define FORCE_INLINE __attribute__((always_inline))
#  define NO_INLINE __attribute__((noinline))
#else  // Other compilers
#  define FORCE_INLINE
#  define NO_INLINE
#endif
#if __cplusplus >= 201103L
#  define NOEXCEPT noexcept
#else
#  define NOEXCEPT throw()
#endif
#if defined(__has_attribute)
#  if __has_attribute(no_sanitize)
#    define ARDUINOJSON_NO_SANITIZE(check) __attribute__((no_sanitize(check)))
#  else
#    define ARDUINOJSON_NO_SANITIZE(check)
#  endif
#else
#  define ARDUINOJSON_NO_SANITIZE(check)
#endif
namespace ARDUINOJSON_NAMESPACE {
class JsonArray;
class JsonObject;
class JsonVariant;
template <typename T>
struct VariantTo {};
template <>
struct VariantTo<JsonArray> {
  typedef JsonArray type;
};
template <>
struct VariantTo<JsonObject> {
  typedef JsonObject type;
};
template <>
struct VariantTo<JsonVariant> {
  typedef JsonVariant type;
};
class VariantAttorney {
  template <typename TClient>
  struct ResultOfGetData {
   protected:  // <- to avoid GCC's "all member functions in class are private"
    static int probe(const VariantData*);
    static char probe(VariantData*);
    static TClient& client;
   public:
    typedef typename conditional<sizeof(probe(client.getData())) == sizeof(int),
                                 const VariantData*, VariantData*>::type type;
  };
 public:
  template <typename TClient>
  FORCE_INLINE static MemoryPool* getPool(TClient& client) {
    return client.getPool();
  }
  template <typename TClient>
  FORCE_INLINE static typename ResultOfGetData<TClient>::type getData(
      TClient& client) {
    return client.getData();
  }
  template <typename TClient>
  FORCE_INLINE static VariantData* getOrCreateData(TClient& client) {
    return client.getOrCreateData();
  }
};
template <typename T>
class SerializedValue {
 public:
  explicit SerializedValue(T str) : _str(str) {}
  operator T() const {
    return _str;
  }
  const char* data() const {
    return _str.c_str();
  }
  size_t size() const {
    return _str.length();
  }
 private:
  T _str;
};
template <typename TChar>
class SerializedValue<TChar*> {
 public:
  explicit SerializedValue(TChar* p, size_t n) : _data(p), _size(n) {}
  operator TChar*() const {
    return _data;
  }
  TChar* data() const {
    return _data;
  }
  size_t size() const {
    return _size;
  }
 private:
  TChar* _data;
  size_t _size;
};
template <typename T>
inline SerializedValue<T> serialized(T str) {
  return SerializedValue<T>(str);
}
template <typename TChar>
inline SerializedValue<TChar*> serialized(TChar* p) {
  return SerializedValue<TChar*>(p, adaptString(p).size());
}
template <typename TChar>
inline SerializedValue<TChar*> serialized(TChar* p, size_t n) {
  return SerializedValue<TChar*>(p, n);
}
}  // namespace ARDUINOJSON_NAMESPACE
#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wconversion"
#elif defined(__GNUC__)
#  if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#    pragma GCC diagnostic push
#  endif
#  pragma GCC diagnostic ignored "-Wconversion"
#endif
#include <stdlib.h>
namespace ARDUINOJSON_NAMESPACE {
#ifndef isnan
template <typename T>
bool isnan(T x) {
  return x != x;
}
#endif
#ifndef isinf
template <typename T>
bool isinf(T x) {
  return x != 0.0 && x * 2 == x;
}
#endif
template <typename T, typename F>
struct alias_cast_t {
  union {
    F raw;
    T data;
  };
};
template <typename T, typename F>
T alias_cast(F raw_data) {
  alias_cast_t<T, F> ac;
  ac.raw = raw_data;
  return ac.data;
}
}  // namespace ARDUINOJSON_NAMESPACE
#if ARDUINOJSON_ENABLE_PROGMEM
#endif
namespace ARDUINOJSON_NAMESPACE {
#if ARDUINOJSON_ENABLE_PROGMEM
#  ifndef ARDUINOJSON_DEFINE_PROGMEM_ARRAY
#    define ARDUINOJSON_DEFINE_PROGMEM_ARRAY(type, name, value) \
      static type const name[] PROGMEM = value;
#  endif
template <typename T>
inline const T* pgm_read(const T* const* p) {
  return reinterpret_cast<const T*>(pgm_read_ptr(p));
}
inline uint32_t pgm_read(const uint32_t* p) {
  return pgm_read_dword(p);
}
#else
#  ifndef ARDUINOJSON_DEFINE_PROGMEM_ARRAY
#    define ARDUINOJSON_DEFINE_PROGMEM_ARRAY(type, name, value) \
      static type const name[] = value;
#  endif
template <typename T>
inline T pgm_read(const T* p) {
  return *p;
}
#endif
template <typename T, size_t = sizeof(T)>
struct FloatTraits {};
template <typename T>
struct FloatTraits<T, 8 /*64bits*/> {
  typedef uint64_t mantissa_type;
  static const short mantissa_bits = 52;
  static const mantissa_type mantissa_max =
      (mantissa_type(1) << mantissa_bits) - 1;
  typedef int16_t exponent_type;
  static const exponent_type exponent_max = 308;
  template <typename TExponent>
  static T make_float(T m, TExponent e) {
    if (e > 0) {
      for (uint8_t index = 0; e != 0; index++) {
        if (e & 1)
          m *= positiveBinaryPowerOfTen(index);
        e >>= 1;
      }
    } else {
      e = TExponent(-e);
      for (uint8_t index = 0; e != 0; index++) {
        if (e & 1)
          m *= negativeBinaryPowerOfTen(index);
        e >>= 1;
      }
    }
    return m;
  }
  static T positiveBinaryPowerOfTen(int index) {
    ARDUINOJSON_DEFINE_PROGMEM_ARRAY(  //
        uint32_t, factors,
        ARDUINOJSON_EXPAND18({
            0x40240000, 0x00000000,  // 1e1
            0x40590000, 0x00000000,  // 1e2
            0x40C38800, 0x00000000,  // 1e4
            0x4197D784, 0x00000000,  // 1e8
            0x4341C379, 0x37E08000,  // 1e16
            0x4693B8B5, 0xB5056E17,  // 1e32
            0x4D384F03, 0xE93FF9F5,  // 1e64
            0x5A827748, 0xF9301D32,  // 1e128
            0x75154FDD, 0x7F73BF3C   // 1e256
        }));
    return forge(pgm_read(factors + 2 * index),
                 pgm_read(factors + 2 * index + 1));
  }
  static T negativeBinaryPowerOfTen(int index) {
    ARDUINOJSON_DEFINE_PROGMEM_ARRAY(  //
        uint32_t, factors,
        ARDUINOJSON_EXPAND18({
            0x3FB99999, 0x9999999A,  // 1e-1
            0x3F847AE1, 0x47AE147B,  // 1e-2
            0x3F1A36E2, 0xEB1C432D,  // 1e-4
            0x3E45798E, 0xE2308C3A,  // 1e-8
            0x3C9CD2B2, 0x97D889BC,  // 1e-16
            0x3949F623, 0xD5A8A733,  // 1e-32
            0x32A50FFD, 0x44F4A73D,  // 1e-64
            0x255BBA08, 0xCF8C979D,  // 1e-128
            0x0AC80628, 0x64AC6F43   // 1e-256
        }));
    return forge(pgm_read(factors + 2 * index),
                 pgm_read(factors + 2 * index + 1));
  }
  static T negativeBinaryPowerOfTenPlusOne(int index) {
    ARDUINOJSON_DEFINE_PROGMEM_ARRAY(  //
        uint32_t, factors,
        ARDUINOJSON_EXPAND18({
            0x3FF00000, 0x00000000,  // 1e0
            0x3FB99999, 0x9999999A,  // 1e-1
            0x3F50624D, 0xD2F1A9FC,  // 1e-3
            0x3E7AD7F2, 0x9ABCAF48,  // 1e-7
            0x3CD203AF, 0x9EE75616,  // 1e-15
            0x398039D6, 0x65896880,  // 1e-31
            0x32DA53FC, 0x9631D10D,  // 1e-63
            0x25915445, 0x81B7DEC2,  // 1e-127
            0x0AFE07B2, 0x7DD78B14   // 1e-255
        }));
    return forge(pgm_read(factors + 2 * index),
                 pgm_read(factors + 2 * index + 1));
  }
  static T nan() {
    return forge(0x7ff80000, 0x00000000);
  }
  static T inf() {
    return forge(0x7ff00000, 0x00000000);
  }
  static T highest() {
    return forge(0x7FEFFFFF, 0xFFFFFFFF);
  }
  template <typename TOut>  // int64_t
  static T highest_for(
      typename enable_if<is_integral<TOut>::value && is_signed<TOut>::value &&
                             sizeof(TOut) == 8,
                         signed>::type* = 0) {
    return forge(0x43DFFFFF, 0xFFFFFFFF);  //  9.2233720368547748e+18
  }
  template <typename TOut>  // uint64_t
  static T highest_for(
      typename enable_if<is_integral<TOut>::value && is_unsigned<TOut>::value &&
                             sizeof(TOut) == 8,
                         unsigned>::type* = 0) {
    return forge(0x43EFFFFF, 0xFFFFFFFF);  //  1.8446744073709549568e+19
  }
  static T lowest() {
    return forge(0xFFEFFFFF, 0xFFFFFFFF);
  }
  static T forge(uint32_t msb, uint32_t lsb) {
    return alias_cast<T>((uint64_t(msb) << 32) | lsb);
  }
};
template <typename T>
struct FloatTraits<T, 4 /*32bits*/> {
  typedef uint32_t mantissa_type;
  static const short mantissa_bits = 23;
  static const mantissa_type mantissa_max =
      (mantissa_type(1) << mantissa_bits) - 1;
  typedef int8_t exponent_type;
  static const exponent_type exponent_max = 38;
  template <typename TExponent>
  static T make_float(T m, TExponent e) {
    if (e > 0) {
      for (uint8_t index = 0; e != 0; index++) {
        if (e & 1)
          m *= positiveBinaryPowerOfTen(index);
        e >>= 1;
      }
    } else {
      e = -e;
      for (uint8_t index = 0; e != 0; index++) {
        if (e & 1)
          m *= negativeBinaryPowerOfTen(index);
        e >>= 1;
      }
    }
    return m;
  }
  static T positiveBinaryPowerOfTen(int index) {
    ARDUINOJSON_DEFINE_PROGMEM_ARRAY(uint32_t, factors,
                                     ARDUINOJSON_EXPAND6({
                                         0x41200000,  // 1e1f
                                         0x42c80000,  // 1e2f
                                         0x461c4000,  // 1e4f
                                         0x4cbebc20,  // 1e8f
                                         0x5a0e1bca,  // 1e16f
                                         0x749dc5ae   // 1e32f
                                     }));
    return forge(pgm_read(factors + index));
  }
  static T negativeBinaryPowerOfTen(int index) {
    ARDUINOJSON_DEFINE_PROGMEM_ARRAY(uint32_t, factors,
                                     ARDUINOJSON_EXPAND6({
                                         0x3dcccccd,  // 1e-1f
                                         0x3c23d70a,  // 1e-2f
                                         0x38d1b717,  // 1e-4f
                                         0x322bcc77,  // 1e-8f
                                         0x24e69595,  // 1e-16f
                                         0x0a4fb11f   // 1e-32f
                                     }));
    return forge(pgm_read(factors + index));
  }
  static T negativeBinaryPowerOfTenPlusOne(int index) {
    ARDUINOJSON_DEFINE_PROGMEM_ARRAY(uint32_t, factors,
                                     ARDUINOJSON_EXPAND6({
                                         0x3f800000,  // 1e0f
                                         0x3dcccccd,  // 1e-1f
                                         0x3a83126f,  // 1e-3f
                                         0x33d6bf95,  // 1e-7f
                                         0x26901d7d,  // 1e-15f
                                         0x0c01ceb3   // 1e-31f
                                     }));
    return forge(pgm_read(factors + index));
  }
  static T forge(uint32_t bits) {
    return alias_cast<T>(bits);
  }
  static T nan() {
    return forge(0x7fc00000);
  }
  static T inf() {
    return forge(0x7f800000);
  }
  static T highest() {
    return forge(0x7f7fffff);
  }
  template <typename TOut>  // int32_t
  static T highest_for(
      typename enable_if<is_integral<TOut>::value && is_signed<TOut>::value &&
                             sizeof(TOut) == 4,
                         signed>::type* = 0) {
    return forge(0x4EFFFFFF);  // 2.14748352E9
  }
  template <typename TOut>  // uint32_t
  static T highest_for(
      typename enable_if<is_integral<TOut>::value && is_unsigned<TOut>::value &&
                             sizeof(TOut) == 4,
                         unsigned>::type* = 0) {
    return forge(0x4F7FFFFF);  // 4.29496704E9
  }
  template <typename TOut>  // int64_t
  static T highest_for(
      typename enable_if<is_integral<TOut>::value && is_signed<TOut>::value &&
                             sizeof(TOut) == 8,
                         signed>::type* = 0) {
    return forge(0x5EFFFFFF);  // 9.22337148709896192E18
  }
  template <typename TOut>  // uint64_t
  static T highest_for(
      typename enable_if<is_integral<TOut>::value && is_unsigned<TOut>::value &&
                             sizeof(TOut) == 8,
                         unsigned>::type* = 0) {
    return forge(0x5F7FFFFF);  // 1.844674297419792384E19
  }
  static T lowest() {
    return forge(0xFf7fffff);
  }
};
template <typename TOut, typename TIn>
typename enable_if<is_integral<TIn>::value && is_unsigned<TIn>::value &&
                       is_integral<TOut>::value && sizeof(TOut) <= sizeof(TIn),
                   bool>::type
canConvertNumber(TIn value) {
  return value <= TIn(numeric_limits<TOut>::highest());
}
template <typename TOut, typename TIn>
typename enable_if<is_integral<TIn>::value && is_unsigned<TIn>::value &&
                       is_integral<TOut>::value && sizeof(TIn) < sizeof(TOut),
                   bool>::type
canConvertNumber(TIn) {
  return true;
}
template <typename TOut, typename TIn>
typename enable_if<is_integral<TIn>::value && is_floating_point<TOut>::value,
                   bool>::type
canConvertNumber(TIn) {
  return true;
}
template <typename TOut, typename TIn>
typename enable_if<is_integral<TIn>::value && is_signed<TIn>::value &&
                       is_integral<TOut>::value && is_signed<TOut>::value &&
                       sizeof(TOut) < sizeof(TIn),
                   bool>::type
canConvertNumber(TIn value) {
  return value >= TIn(numeric_limits<TOut>::lowest()) &&
         value <= TIn(numeric_limits<TOut>::highest());
}
template <typename TOut, typename TIn>
typename enable_if<is_integral<TIn>::value && is_signed<TIn>::value &&
                       is_integral<TOut>::value && is_signed<TOut>::value &&
                       sizeof(TIn) <= sizeof(TOut),
                   bool>::type
canConvertNumber(TIn) {
  return true;
}
template <typename TOut, typename TIn>
typename enable_if<is_integral<TIn>::value && is_signed<TIn>::value &&
                       is_integral<TOut>::value && is_unsigned<TOut>::value &&
                       sizeof(TOut) >= sizeof(TIn),
                   bool>::type
canConvertNumber(TIn value) {
  if (value < 0)
    return false;
  return TOut(value) <= numeric_limits<TOut>::highest();
}
template <typename TOut, typename TIn>
typename enable_if<is_integral<TIn>::value && is_signed<TIn>::value &&
                       is_integral<TOut>::value && is_unsigned<TOut>::value &&
                       sizeof(TOut) < sizeof(TIn),
                   bool>::type
canConvertNumber(TIn value) {
  if (value < 0)
    return false;
  return value <= TIn(numeric_limits<TOut>::highest());
}
template <typename TOut, typename TIn>
typename enable_if<is_floating_point<TIn>::value && is_integral<TOut>::value &&
                       sizeof(TOut) < sizeof(TIn),
                   bool>::type
canConvertNumber(TIn value) {
  return value >= numeric_limits<TOut>::lowest() &&
         value <= numeric_limits<TOut>::highest();
}
template <typename TOut, typename TIn>
typename enable_if<is_floating_point<TIn>::value && is_integral<TOut>::value &&
                       sizeof(TOut) >= sizeof(TIn),
                   bool>::type
canConvertNumber(TIn value) {
  return value >= numeric_limits<TOut>::lowest() &&
         value <= FloatTraits<TIn>::template highest_for<TOut>();
}
template <typename TOut, typename TIn>
TOut convertNumber(TIn value) {
  return canConvertNumber<TOut>(value) ? TOut(value) : 0;
}
}  // namespace ARDUINOJSON_NAMESPACE
#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#    pragma GCC diagnostic pop
#  endif
#endif
#if defined(__GNUC__)
#  if __GNUC__ >= 7
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#    pragma GCC diagnostic ignored "-Wuninitialized"
#  endif
#endif
namespace ARDUINOJSON_NAMESPACE {
class VariantData {
  VariantContent _content;  // must be first to allow cast from array to variant
  uint8_t _flags;
 public:
  void init() {
    _flags = VALUE_IS_NULL;
  }
  void operator=(const VariantData& src) {
    _content = src._content;
    _flags = uint8_t((_flags & OWNED_KEY_BIT) | (src._flags & ~OWNED_KEY_BIT));
  }
  template <typename TVisitor>
  typename TVisitor::result_type accept(TVisitor& visitor) const {
    switch (type()) {
      case VALUE_IS_FLOAT:
        return visitor.visitFloat(_content.asFloat);
      case VALUE_IS_ARRAY:
        return visitor.visitArray(_content.asCollection);
      case VALUE_IS_OBJECT:
        return visitor.visitObject(_content.asCollection);
      case VALUE_IS_LINKED_STRING:
      case VALUE_IS_OWNED_STRING:
        return visitor.visitString(_content.asString.data,
                                   _content.asString.size);
      case VALUE_IS_OWNED_RAW:
      case VALUE_IS_LINKED_RAW:
        return visitor.visitRawJson(_content.asString.data,
                                    _content.asString.size);
      case VALUE_IS_SIGNED_INTEGER:
        return visitor.visitSignedInteger(_content.asSignedInteger);
      case VALUE_IS_UNSIGNED_INTEGER:
        return visitor.visitUnsignedInteger(_content.asUnsignedInteger);
      case VALUE_IS_BOOLEAN:
        return visitor.visitBoolean(_content.asBoolean != 0);
      default:
        return visitor.visitNull();
    }
  }
  template <typename T>
  T asIntegral() const;
  template <typename T>
  T asFloat() const;
  JsonString asString() const;
  bool asBoolean() const;
  CollectionData* asArray() {
    return isArray() ? &_content.asCollection : 0;
  }
  const CollectionData* asArray() const {
    return const_cast<VariantData*>(this)->asArray();
  }
  const CollectionData* asCollection() const {
    return isCollection() ? &_content.asCollection : 0;
  }
  CollectionData* asObject() {
    return isObject() ? &_content.asCollection : 0;
  }
  const CollectionData* asObject() const {
    return const_cast<VariantData*>(this)->asObject();
  }
  bool copyFrom(const VariantData& src, MemoryPool* pool);
  bool isArray() const {
    return (_flags & VALUE_IS_ARRAY) != 0;
  }
  bool isBoolean() const {
    return type() == VALUE_IS_BOOLEAN;
  }
  bool isCollection() const {
    return (_flags & COLLECTION_MASK) != 0;
  }
  template <typename T>
  bool isInteger() const {
    switch (type()) {
      case VALUE_IS_UNSIGNED_INTEGER:
        return canConvertNumber<T>(_content.asUnsignedInteger);
      case VALUE_IS_SIGNED_INTEGER:
        return canConvertNumber<T>(_content.asSignedInteger);
      default:
        return false;
    }
  }
  bool isFloat() const {
    return (_flags & NUMBER_BIT) != 0;
  }
  bool isString() const {
    return type() == VALUE_IS_LINKED_STRING || type() == VALUE_IS_OWNED_STRING;
  }
  bool isObject() const {
    return (_flags & VALUE_IS_OBJECT) != 0;
  }
  bool isNull() const {
    return type() == VALUE_IS_NULL;
  }
  bool isEnclosed() const {
    return !isFloat();
  }
  void remove(size_t index) {
    if (isArray())
      _content.asCollection.removeElement(index);
  }
  template <typename TAdaptedString>
  void remove(TAdaptedString key) {
    if (isObject())
      _content.asCollection.removeMember(key);
  }
  void setBoolean(bool value) {
    setType(VALUE_IS_BOOLEAN);
    _content.asBoolean = value;
  }
  void setFloat(JsonFloat value) {
    setType(VALUE_IS_FLOAT);
    _content.asFloat = value;
  }
  void setLinkedRaw(SerializedValue<const char*> value) {
    if (value.data()) {
      setType(VALUE_IS_LINKED_RAW);
      _content.asString.data = value.data();
      _content.asString.size = value.size();
    } else {
      setType(VALUE_IS_NULL);
    }
  }
  template <typename T>
  bool storeOwnedRaw(SerializedValue<T> value, MemoryPool* pool) {
    const char* dup = pool->saveString(adaptString(value.data(), value.size()));
    if (dup) {
      setType(VALUE_IS_OWNED_RAW);
      _content.asString.data = dup;
      _content.asString.size = value.size();
      return true;
    } else {
      setType(VALUE_IS_NULL);
      return false;
    }
  }
  template <typename T>
  typename enable_if<is_unsigned<T>::value>::type setInteger(T value) {
    setType(VALUE_IS_UNSIGNED_INTEGER);
    _content.asUnsignedInteger = static_cast<JsonUInt>(value);
  }
  template <typename T>
  typename enable_if<is_signed<T>::value>::type setInteger(T value) {
    setType(VALUE_IS_SIGNED_INTEGER);
    _content.asSignedInteger = value;
  }
  void setNull() {
    setType(VALUE_IS_NULL);
  }
  void setString(JsonString s) {
    ARDUINOJSON_ASSERT(s);
    if (s.isLinked())
      setType(VALUE_IS_LINKED_STRING);
    else
      setType(VALUE_IS_OWNED_STRING);
    _content.asString.data = s.c_str();
    _content.asString.size = s.size();
  }
  CollectionData& toArray() {
    setType(VALUE_IS_ARRAY);
    _content.asCollection.clear();
    return _content.asCollection;
  }
  CollectionData& toObject() {
    setType(VALUE_IS_OBJECT);
    _content.asCollection.clear();
    return _content.asCollection;
  }
  size_t memoryUsage() const {
    switch (type()) {
      case VALUE_IS_OWNED_STRING:
      case VALUE_IS_OWNED_RAW:
        return _content.asString.size + 1;
      case VALUE_IS_OBJECT:
      case VALUE_IS_ARRAY:
        return _content.asCollection.memoryUsage();
      default:
        return 0;
    }
  }
  size_t size() const {
    return isCollection() ? _content.asCollection.size() : 0;
  }
  VariantData* addElement(MemoryPool* pool) {
    if (isNull())
      toArray();
    if (!isArray())
      return 0;
    return _content.asCollection.addElement(pool);
  }
  VariantData* getElement(size_t index) const {
    const CollectionData* col = asArray();
    return col ? col->getElement(index) : 0;
  }
  VariantData* getOrAddElement(size_t index, MemoryPool* pool) {
    if (isNull())
      toArray();
    if (!isArray())
      return 0;
    return _content.asCollection.getOrAddElement(index, pool);
  }
  template <typename TAdaptedString>
  VariantData* getMember(TAdaptedString key) const {
    const CollectionData* col = asObject();
    return col ? col->getMember(key) : 0;
  }
  template <typename TAdaptedString>
  VariantData* getOrAddMember(TAdaptedString key, MemoryPool* pool) {
    if (isNull())
      toObject();
    if (!isObject())
      return 0;
    return _content.asCollection.getOrAddMember(key, pool);
  }
  void movePointers(ptrdiff_t stringDistance, ptrdiff_t variantDistance) {
    if (_flags & OWNED_VALUE_BIT)
      _content.asString.data += stringDistance;
    if (_flags & COLLECTION_MASK)
      _content.asCollection.movePointers(stringDistance, variantDistance);
  }
  uint8_t type() const {
    return _flags & VALUE_MASK;
  }
  template <typename TAdaptedString>
  inline bool setString(TAdaptedString value, MemoryPool* pool) {
    if (value.isNull()) {
      setNull();
      return true;
    }
    return storeString(pool, value, VariantStringSetter(this));
  }
 private:
  void setType(uint8_t t) {
    _flags &= OWNED_KEY_BIT;
    _flags |= t;
  }
  struct VariantStringSetter {
    VariantStringSetter(VariantData* instance) : _instance(instance) {}
    template <typename TStoredString>
    void operator()(TStoredString s) {
      if (s)
        _instance->setString(s);
      else
        _instance->setNull();
    }
    VariantData* _instance;
  };
};
}  // namespace ARDUINOJSON_NAMESPACE
#if defined(__GNUC__)
#  if __GNUC__ >= 8
#    pragma GCC diagnostic pop
#  endif
#endif
namespace ARDUINOJSON_NAMESPACE {
template <typename TResult>
struct Visitor {
  typedef TResult result_type;
  TResult visitArray(const CollectionData&) {
    return TResult();
  }
  TResult visitBoolean(bool) {
    return TResult();
  }
  TResult visitFloat(JsonFloat) {
    return TResult();
  }
  TResult visitSignedInteger(JsonInteger) {
    return TResult();
  }
  TResult visitNull() {
    return TResult();
  }
  TResult visitObject(const CollectionData&) {
    return TResult();
  }
  TResult visitUnsignedInteger(JsonUInt) {
    return TResult();
  }
  TResult visitRawJson(const char*, size_t) {
    return TResult();
  }
  TResult visitString(const char*, size_t) {
    return TResult();
  }
};
template <typename TVisitor>
inline typename TVisitor::result_type variantAccept(const VariantData* var,
                                                    TVisitor& visitor) {
  if (var != 0)
    return var->accept(visitor);
  else
    return visitor.visitNull();
}
inline bool variantCopyFrom(VariantData* dst, const VariantData* src,
                            MemoryPool* pool) {
  if (!dst)
    return false;
  if (!src) {
    dst->setNull();
    return true;
  }
  return dst->copyFrom(*src, pool);
}
inline void variantSetNull(VariantData* var) {
  if (!var)
    return;
  var->setNull();
}
template <typename TAdaptedString>
inline bool variantSetString(VariantData* var, TAdaptedString value,
                             MemoryPool* pool) {
  return var != 0 ? var->setString(value, pool) : 0;
}
inline size_t variantSize(const VariantData* var) {
  return var != 0 ? var->size() : 0;
}
inline CollectionData* variantToArray(VariantData* var) {
  if (!var)
    return 0;
  return &var->toArray();
}
inline CollectionData* variantToObject(VariantData* var) {
  if (!var)
    return 0;
  return &var->toObject();
}
inline VariantData* variantGetElement(const VariantData* var, size_t index) {
  return var != 0 ? var->getElement(index) : 0;
}
inline NO_INLINE VariantData* variantAddElement(VariantData* var,
                                                MemoryPool* pool) {
  return var != 0 ? var->addElement(pool) : 0;
}
inline NO_INLINE VariantData* variantGetOrAddElement(VariantData* var,
                                                     size_t index,
                                                     MemoryPool* pool) {
  return var != 0 ? var->getOrAddElement(index, pool) : 0;
}
template <typename TAdaptedString>
VariantData* variantGetMember(const VariantData* var, TAdaptedString key) {
  if (!var)
    return 0;
  return var->getMember(key);
}
template <typename TAdaptedString>
VariantData* variantGetOrAddMember(VariantData* var, TAdaptedString key,
                                   MemoryPool* pool) {
  if (!var)
    return 0;
  return var->getOrAddMember(key, pool);
}
inline bool variantIsNull(const VariantData* var) {
  return var == 0 || var->isNull();
}
inline size_t variantNesting(const VariantData* var) {
  if (!var)
    return 0;
  const CollectionData* collection = var->asCollection();
  if (!collection)
    return 0;
  size_t maxChildNesting = 0;
  for (const VariantSlot* s = collection->head(); s; s = s->next()) {
    size_t childNesting = variantNesting(s->data());
    if (childNesting > maxChildNesting)
      maxChildNesting = childNesting;
  }
  return maxChildNesting + 1;
}
enum CompareResult {
  COMPARE_RESULT_DIFFER = 0,
  COMPARE_RESULT_EQUAL = 1,
  COMPARE_RESULT_GREATER = 2,
  COMPARE_RESULT_LESS = 4,
  COMPARE_RESULT_GREATER_OR_EQUAL = 3,
  COMPARE_RESULT_LESS_OR_EQUAL = 5
};
template <typename T>
CompareResult arithmeticCompare(const T& lhs, const T& rhs) {
  if (lhs < rhs)
    return COMPARE_RESULT_LESS;
  else if (lhs > rhs)
    return COMPARE_RESULT_GREATER;
  else
    return COMPARE_RESULT_EQUAL;
}
template <typename T1, typename T2>
CompareResult arithmeticCompare(
    const T1& lhs, const T2& rhs,
    typename enable_if<is_integral<T1>::value && is_integral<T2>::value &&
                           sizeof(T1) < sizeof(T2),
                       int  // Using int instead of void to avoid C2572 on
                       >::type* = 0) {
  return arithmeticCompare<T2>(static_cast<T2>(lhs), rhs);
}
template <typename T1, typename T2>
CompareResult arithmeticCompare(
    const T1& lhs, const T2& rhs,
    typename enable_if<is_integral<T1>::value && is_integral<T2>::value &&
                       sizeof(T2) < sizeof(T1)>::type* = 0) {
  return arithmeticCompare<T1>(lhs, static_cast<T1>(rhs));
}
template <typename T1, typename T2>
CompareResult arithmeticCompare(
    const T1& lhs, const T2& rhs,
    typename enable_if<is_integral<T1>::value && is_integral<T2>::value &&
                       is_signed<T1>::value == is_signed<T2>::value &&
                       sizeof(T2) == sizeof(T1)>::type* = 0) {
  return arithmeticCompare<T1>(lhs, static_cast<T1>(rhs));
}
template <typename T1, typename T2>
CompareResult arithmeticCompare(
    const T1& lhs, const T2& rhs,
    typename enable_if<is_integral<T1>::value && is_integral<T2>::value &&
                       is_unsigned<T1>::value && is_signed<T2>::value &&
                       sizeof(T2) == sizeof(T1)>::type* = 0) {
  if (rhs < 0)
    return COMPARE_RESULT_GREATER;
  return arithmeticCompare<T1>(lhs, static_cast<T1>(rhs));
}
template <typename T1, typename T2>
CompareResult arithmeticCompare(
    const T1& lhs, const T2& rhs,
    typename enable_if<is_integral<T1>::value && is_integral<T2>::value &&
                       is_signed<T1>::value && is_unsigned<T2>::value &&
                       sizeof(T2) == sizeof(T1)>::type* = 0) {
  if (lhs < 0)
    return COMPARE_RESULT_LESS;
  return arithmeticCompare<T2>(static_cast<T2>(lhs), rhs);
}
template <typename T1, typename T2>
CompareResult arithmeticCompare(
    const T1& lhs, const T2& rhs,
    typename enable_if<is_floating_point<T1>::value ||
                       is_floating_point<T2>::value>::type* = 0) {
  return arithmeticCompare<double>(static_cast<double>(lhs),
                                   static_cast<double>(rhs));
}
template <typename T2>
CompareResult arithmeticCompareNegateLeft(
    JsonUInt, const T2&,
    typename enable_if<is_unsigned<T2>::value>::type* = 0) {
  return COMPARE_RESULT_LESS;
}
template <typename T2>
CompareResult arithmeticCompareNegateLeft(
    JsonUInt lhs, const T2& rhs,
    typename enable_if<is_signed<T2>::value>::type* = 0) {
  if (rhs > 0)
    return COMPARE_RESULT_LESS;
  return arithmeticCompare(-rhs, static_cast<T2>(lhs));
}
template <typename T1>
CompareResult arithmeticCompareNegateRight(
    const T1&, JsonUInt,
    typename enable_if<is_unsigned<T1>::value>::type* = 0) {
  return COMPARE_RESULT_GREATER;
}
template <typename T1>
CompareResult arithmeticCompareNegateRight(
    const T1& lhs, JsonUInt rhs,
    typename enable_if<is_signed<T1>::value>::type* = 0) {
  if (lhs > 0)
    return COMPARE_RESULT_GREATER;
  return arithmeticCompare(static_cast<T1>(rhs), -lhs);
}
struct VariantTag {};
template <typename T>
struct IsVariant : is_base_of<VariantTag, T> {};
class JsonVariantConst;
template <typename T>
CompareResult compare(JsonVariantConst lhs,
                      const T& rhs);  // VariantCompare.cpp
struct VariantOperatorTag {};
template <typename TVariant>
struct VariantOperators : VariantOperatorTag {
  template <typename T>
  friend
      typename enable_if<!IsVariant<T>::value && !is_array<T>::value, T>::type
      operator|(const TVariant& variant, const T& defaultValue) {
    if (variant.template is<T>())
      return variant.template as<T>();
    else
      return defaultValue;
  }
  friend const char* operator|(const TVariant& variant,
                               const char* defaultValue) {
    if (variant.template is<const char*>())
      return variant.template as<const char*>();
    else
      return defaultValue;
  }
  template <typename T>
  friend typename enable_if<IsVariant<T>::value, JsonVariantConst>::type
  operator|(const TVariant& variant, T defaultValue) {
    if (variant)
      return variant;
    else
      return defaultValue;
  }
  template <typename T>
  friend bool operator==(T* lhs, TVariant rhs) {
    return compare(rhs, lhs) == COMPARE_RESULT_EQUAL;
  }
  template <typename T>
  friend bool operator==(const T& lhs, TVariant rhs) {
    return compare(rhs, lhs) == COMPARE_RESULT_EQUAL;
  }
  template <typename T>
  friend bool operator==(TVariant lhs, T* rhs) {
    return compare(lhs, rhs) == COMPARE_RESULT_EQUAL;
  }
  template <typename T>
  friend
      typename enable_if<!is_base_of<VariantOperatorTag, T>::value, bool>::type
      operator==(TVariant lhs, const T& rhs) {
    return compare(lhs, rhs) == COMPARE_RESULT_EQUAL;
  }
  template <typename T>
  friend bool operator!=(T* lhs, TVariant rhs) {
    return compare(rhs, lhs) != COMPARE_RESULT_EQUAL;
  }
  template <typename T>
  friend bool operator!=(const T& lhs, TVariant rhs) {
    return compare(rhs, lhs) != COMPARE_RESULT_EQUAL;
  }
  template <typename T>
  friend bool operator!=(TVariant lhs, T* rhs) {
    return compare(lhs, rhs) != COMPARE_RESULT_EQUAL;
  }
  template <typename T>
  friend
      typename enable_if<!is_base_of<VariantOperatorTag, T>::value, bool>::type
      operator!=(TVariant lhs, const T& rhs) {
    return compare(lhs, rhs) != COMPARE_RESULT_EQUAL;
  }
  template <typename T>
  friend bool operator<(T* lhs, TVariant rhs) {
    return compare(rhs, lhs) == COMPARE_RESULT_GREATER;
  }
  template <typename T>
  friend bool operator<(const T& lhs, TVariant rhs) {
    return compare(rhs, lhs) == COMPARE_RESULT_GREATER;
  }
  template <typename T>
  friend bool operator<(TVariant lhs, T* rhs) {
    return compare(lhs, rhs) == COMPARE_RESULT_LESS;
  }
  template <typename T>
  friend
      typename enable_if<!is_base_of<VariantOperatorTag, T>::value, bool>::type
      operator<(TVariant lhs, const T& rhs) {
    return compare(lhs, rhs) == COMPARE_RESULT_LESS;
  }
  template <typename T>
  friend bool operator<=(T* lhs, TVariant rhs) {
    return (compare(rhs, lhs) & COMPARE_RESULT_GREATER_OR_EQUAL) != 0;
  }
  template <typename T>
  friend bool operator<=(const T& lhs, TVariant rhs) {
    return (compare(rhs, lhs) & COMPARE_RESULT_GREATER_OR_EQUAL) != 0;
  }
  template <typename T>
  friend bool operator<=(TVariant lhs, T* rhs) {
    return (compare(lhs, rhs) & COMPARE_RESULT_LESS_OR_EQUAL) != 0;
  }
  template <typename T>
  friend
      typename enable_if<!is_base_of<VariantOperatorTag, T>::value, bool>::type
      operator<=(TVariant lhs, const T& rhs) {
    return (compare(lhs, rhs) & COMPARE_RESULT_LESS_OR_EQUAL) != 0;
  }
  template <typename T>
  friend bool operator>(T* lhs, TVariant rhs) {
    return compare(rhs, lhs) == COMPARE_RESULT_LESS;
  }
  template <typename T>
  friend bool operator>(const T& lhs, TVariant rhs) {
    return compare(rhs, lhs) == COMPARE_RESULT_LESS;
  }
  template <typename T>
  friend bool operator>(TVariant lhs, T* rhs) {
    return compare(lhs, rhs) == COMPARE_RESULT_GREATER;
  }
  template <typename T>
  friend
      typename enable_if<!is_base_of<VariantOperatorTag, T>::value, bool>::type
      operator>(TVariant lhs, const T& rhs) {
    return compare(lhs, rhs) == COMPARE_RESULT_GREATER;
  }
  template <typename T>
  friend bool operator>=(T* lhs, TVariant rhs) {
    return (compare(rhs, lhs) & COMPARE_RESULT_LESS_OR_EQUAL) != 0;
  }
  template <typename T>
  friend bool operator>=(const T& lhs, TVariant rhs) {
    return (compare(rhs, lhs) & COMPARE_RESULT_LESS_OR_EQUAL) != 0;
  }
  template <typename T>
  friend bool operator>=(TVariant lhs, T* rhs) {
    return (compare(lhs, rhs) & COMPARE_RESULT_GREATER_OR_EQUAL) != 0;
  }
  template <typename T>
  friend
      typename enable_if<!is_base_of<VariantOperatorTag, T>::value, bool>::type
      operator>=(TVariant lhs, const T& rhs) {
    return (compare(lhs, rhs) & COMPARE_RESULT_GREATER_OR_EQUAL) != 0;
  }
};
class JsonArray;
class JsonObject;
class JsonVariantConst : public VariantTag,
                         public VariantOperators<JsonVariantConst> {
  friend class VariantAttorney;
 public:
  JsonVariantConst() : _data(0) {}
  explicit JsonVariantConst(const VariantData* data) : _data(data) {}
  FORCE_INLINE bool isNull() const {
    return variantIsNull(_data);
  }
  FORCE_INLINE bool isUnbound() const {
    return !_data;
  }
  FORCE_INLINE size_t memoryUsage() const {
    return _data ? _data->memoryUsage() : 0;
  }
  FORCE_INLINE size_t nesting() const {
    return variantNesting(_data);
  }
  size_t size() const {
    return variantSize(_data);
  }
  template <typename T>
  FORCE_INLINE
      typename enable_if<!is_same<T, char*>::value && !is_same<T, char>::value,
                         T>::type
      as() const {
    return Converter<T>::fromJson(*this);
  }
  template <typename T>
  FORCE_INLINE
      typename enable_if<!is_same<T, char*>::value && !is_same<T, char>::value,
                         bool>::type
      is() const {
    return Converter<T>::checkJson(*this);
  }
  template <typename T>
  FORCE_INLINE operator T() const {
    return as<T>();
  }
  FORCE_INLINE JsonVariantConst operator[](size_t index) const {
    return JsonVariantConst(variantGetElement(_data, index));
  }
  template <typename TString>
  FORCE_INLINE
      typename enable_if<IsString<TString>::value, JsonVariantConst>::type
      operator[](const TString& key) const {
    return JsonVariantConst(variantGetMember(_data, adaptString(key)));
  }
  template <typename TChar>
  FORCE_INLINE
      typename enable_if<IsString<TChar*>::value, JsonVariantConst>::type
      operator[](TChar* key) const {
    return JsonVariantConst(variantGetMember(_data, adaptString(key)));
  }
  template <typename TString>
  FORCE_INLINE typename enable_if<IsString<TString>::value, bool>::type
  containsKey(const TString& key) const {
    return variantGetMember(getData(), adaptString(key)) != 0;
  }
  template <typename TChar>
  FORCE_INLINE typename enable_if<IsString<TChar*>::value, bool>::type
  containsKey(TChar* key) const {
    return variantGetMember(getData(), adaptString(key)) != 0;
  }
 protected:
  const VariantData* getData() const {
    return _data;
  }
 private:
  const VariantData* _data;
};
class JsonVariant;
template <typename>
class ElementProxy;
template <typename, typename>
class MemberProxy;
template <typename TDerived>
class VariantRefBase : public VariantTag {
  friend class VariantAttorney;
 public:
  FORCE_INLINE void clear() const {
    variantSetNull(getData());
  }
  FORCE_INLINE bool isNull() const {
    return variantIsNull(getData());
  }
  FORCE_INLINE bool isUnbound() const {
    return !getData();
  }
  template <typename T>
  FORCE_INLINE
      typename enable_if<!ConverterNeedsWriteableRef<T>::value, T>::type
      as() const {
    return Converter<T>::fromJson(getVariantConst());
  }
  template <typename T>
  FORCE_INLINE typename enable_if<ConverterNeedsWriteableRef<T>::value, T>::type
  as() const {
    return Converter<T>::fromJson(getVariant());
  }
  template <typename T>
  FORCE_INLINE operator T() const {
    return as<T>();
  }
  template <typename T>
  typename enable_if<is_same<T, JsonArray>::value, JsonArray>::type to() const;
  template <typename T>
  typename enable_if<is_same<T, JsonObject>::value, JsonObject>::type to()
      const;
  template <typename T>
  typename enable_if<is_same<T, JsonVariant>::value, JsonVariant>::type to()
      const;
  template <typename T>
  FORCE_INLINE
      typename enable_if<ConverterNeedsWriteableRef<T>::value, bool>::type
      is() const {
    return Converter<T>::checkJson(getVariant());
  }
  template <typename T>
  FORCE_INLINE typename enable_if<!ConverterNeedsWriteableRef<T>::value &&
                                      !is_same<T, char*>::value &&
                                      !is_same<T, char>::value,
                                  bool>::type
  is() const {
    return Converter<T>::checkJson(getVariantConst());
  }
  FORCE_INLINE void shallowCopy(JsonVariantConst target) {
    VariantData* data = getOrCreateData();
    if (!data)
      return;
    const VariantData* targetData = VariantAttorney::getData(target);
    if (targetData)
      *data = *targetData;
    else
      data->setNull();
  }
  template <typename T>
  FORCE_INLINE bool set(const T& value) const {
    Converter<T>::toJson(value, getOrCreateVariant());
    MemoryPool* pool = getPool();
    return pool && !pool->overflowed();
  }
  template <typename T>
  FORCE_INLINE bool set(T* value) const {
    Converter<T*>::toJson(value, getOrCreateVariant());
    MemoryPool* pool = getPool();
    return pool && !pool->overflowed();
  }
  FORCE_INLINE size_t size() const {
    return variantSize(getData());
  }
  FORCE_INLINE size_t memoryUsage() const {
    VariantData* data = getData();
    return data ? data->memoryUsage() : 0;
  }
  FORCE_INLINE size_t nesting() const {
    return variantNesting(getData());
  }
  FORCE_INLINE JsonVariant add() const;
  template <typename T>
  FORCE_INLINE bool add(const T& value) const {
    return add().set(value);
  }
  template <typename T>
  FORCE_INLINE bool add(T* value) const {
    return add().set(value);
  }
  FORCE_INLINE void remove(size_t index) const {
    VariantData* data = getData();
    if (data)
      data->remove(index);
  }
  template <typename TChar>
  FORCE_INLINE typename enable_if<IsString<TChar*>::value>::type remove(
      TChar* key) const {
    VariantData* data = getData();
    if (data)
      data->remove(adaptString(key));
  }
  template <typename TString>
  FORCE_INLINE typename enable_if<IsString<TString>::value>::type remove(
      const TString& key) const {
    VariantData* data = getData();
    if (data)
      data->remove(adaptString(key));
  }
  FORCE_INLINE JsonArray createNestedArray() const;
  FORCE_INLINE JsonObject createNestedObject() const;
  FORCE_INLINE ElementProxy<TDerived> operator[](size_t index) const;
  template <typename TString>
  FORCE_INLINE typename enable_if<IsString<TString>::value, bool>::type
  containsKey(const TString& key) const;
  template <typename TChar>
  FORCE_INLINE typename enable_if<IsString<TChar*>::value, bool>::type
  containsKey(TChar* key) const;
  template <typename TString>
  FORCE_INLINE typename enable_if<IsString<TString>::value,
                                  MemberProxy<TDerived, TString> >::type
  operator[](const TString& key) const;
  template <typename TChar>
  FORCE_INLINE typename enable_if<IsString<TChar*>::value,
                                  MemberProxy<TDerived, TChar*> >::type
  operator[](TChar* key) const;
  template <typename TString>
  FORCE_INLINE JsonArray createNestedArray(const TString& key) const;
  template <typename TChar>
  FORCE_INLINE JsonArray createNestedArray(TChar* key) const;
  template <typename TString>
  JsonObject createNestedObject(const TString& key) const;
  template <typename TChar>
  JsonObject createNestedObject(TChar* key) const;
 private:
  TDerived& derived() {
    return static_cast<TDerived&>(*this);
  }
  const TDerived& derived() const {
    return static_cast<const TDerived&>(*this);
  }
  FORCE_INLINE MemoryPool* getPool() const {
    return VariantAttorney::getPool(derived());
  }
  FORCE_INLINE VariantData* getData() const {
    return VariantAttorney::getData(derived());
  }
  FORCE_INLINE VariantData* getOrCreateData() const {
    return VariantAttorney::getOrCreateData(derived());
  }
 private:
  FORCE_INLINE JsonVariant getVariant() const;
  FORCE_INLINE JsonVariantConst getVariantConst() const {
    return JsonVariantConst(getData());
  }
  FORCE_INLINE JsonVariant getOrCreateVariant() const;
};
template <typename TUpstream>
class ElementProxy : public VariantRefBase<ElementProxy<TUpstream> >,
                     public VariantOperators<ElementProxy<TUpstream> > {
  friend class VariantAttorney;
 public:
  ElementProxy(TUpstream upstream, size_t index)
      : _upstream(upstream), _index(index) {}
  ElementProxy(const ElementProxy& src)
      : _upstream(src._upstream), _index(src._index) {}
  FORCE_INLINE ElementProxy& operator=(const ElementProxy& src) {
    this->set(src);
    return *this;
  }
  template <typename T>
  FORCE_INLINE ElementProxy& operator=(const T& src) {
    this->set(src);
    return *this;
  }
  template <typename T>
  FORCE_INLINE ElementProxy& operator=(T* src) {
    this->set(src);
    return *this;
  }
 private:
  FORCE_INLINE MemoryPool* getPool() const {
    return VariantAttorney::getPool(_upstream);
  }
  FORCE_INLINE VariantData* getData() const {
    return variantGetElement(VariantAttorney::getData(_upstream), _index);
  }
  FORCE_INLINE VariantData* getOrCreateData() const {
    return variantGetOrAddElement(VariantAttorney::getOrCreateData(_upstream),
                                  _index, VariantAttorney::getPool(_upstream));
  }
  TUpstream _upstream;
  size_t _index;
};
class JsonVariant : public VariantRefBase<JsonVariant>,
                    public VariantOperators<JsonVariant> {
  friend class VariantAttorney;
 public:
  JsonVariant() : _data(0), _pool(0) {}
  JsonVariant(MemoryPool* pool, VariantData* data) : _data(data), _pool(pool) {}
 private:
  FORCE_INLINE MemoryPool* getPool() const {
    return _pool;
  }
  FORCE_INLINE VariantData* getData() const {
    return _data;
  }
  FORCE_INLINE VariantData* getOrCreateData() const {
    return _data;
  }
  VariantData* _data;
  MemoryPool* _pool;
};
template <>
struct Converter<JsonVariant> : private VariantAttorney {
  static void toJson(JsonVariant src, JsonVariant dst) {
    variantCopyFrom(getData(dst), getData(src), getPool(dst));
  }
  static JsonVariant fromJson(JsonVariant src) {
    return src;
  }
  static InvalidConversion<JsonVariantConst, JsonVariant> fromJson(
      JsonVariantConst);
  static bool checkJson(JsonVariant src) {
    VariantData* data = getData(src);
    return !!data;
  }
  static bool checkJson(JsonVariantConst) {
    return false;
  }
};
template <>
struct Converter<JsonVariantConst> : private VariantAttorney {
  static void toJson(JsonVariantConst src, JsonVariant dst) {
    variantCopyFrom(getData(dst), getData(src), getPool(dst));
  }
  static JsonVariantConst fromJson(JsonVariantConst src) {
    return JsonVariantConst(getData(src));
  }
  static bool checkJson(JsonVariantConst src) {
    const VariantData* data = getData(src);
    return !!data;
  }
};
struct SlotKeySetter {
  SlotKeySetter(VariantSlot* instance) : _instance(instance) {}
  template <typename TStoredString>
  void operator()(TStoredString s) {
    if (!s)
      return;
    ARDUINOJSON_ASSERT(_instance != 0);
    _instance->setKey(s);
  }
  VariantSlot* _instance;
};
template <typename TAdaptedString>
inline bool slotSetKey(VariantSlot* var, TAdaptedString key, MemoryPool* pool) {
  if (!var)
    return false;
  return storeString(pool, key, SlotKeySetter(var));
}
inline size_t slotSize(const VariantSlot* var) {
  size_t n = 0;
  while (var) {
    n++;
    var = var->next();
  }
  return n;
}
inline VariantData* slotData(VariantSlot* slot) {
  return reinterpret_cast<VariantData*>(slot);
}
class VariantPtr {
 public:
  VariantPtr(MemoryPool* pool, VariantData* data) : _variant(pool, data) {}
  JsonVariant* operator->() {
    return &_variant;
  }
  JsonVariant& operator*() {
    return _variant;
  }
 private:
  JsonVariant _variant;
};
class JsonArrayIterator {
  friend class JsonArray;
 public:
  JsonArrayIterator() : _slot(0) {}
  explicit JsonArrayIterator(MemoryPool* pool, VariantSlot* slot)
      : _pool(pool), _slot(slot) {}
  JsonVariant operator*() const {
    return JsonVariant(_pool, _slot->data());
  }
  VariantPtr operator->() {
    return VariantPtr(_pool, _slot->data());
  }
  bool operator==(const JsonArrayIterator& other) const {
    return _slot == other._slot;
  }
  bool operator!=(const JsonArrayIterator& other) const {
    return _slot != other._slot;
  }
  JsonArrayIterator& operator++() {
    _slot = _slot->next();
    return *this;
  }
  JsonArrayIterator& operator+=(size_t distance) {
    _slot = _slot->next(distance);
    return *this;
  }
 private:
  MemoryPool* _pool;
  VariantSlot* _slot;
};
class VariantConstPtr {
 public:
  VariantConstPtr(const VariantData* data) : _variant(data) {}
  JsonVariantConst* operator->() {
    return &_variant;
  }
  JsonVariantConst& operator*() {
    return _variant;
  }
 private:
  JsonVariantConst _variant;
};
class JsonArrayConstIterator {
  friend class JsonArray;
 public:
  JsonArrayConstIterator() : _slot(0) {}
  explicit JsonArrayConstIterator(const VariantSlot* slot) : _slot(slot) {}
  JsonVariantConst operator*() const {
    return JsonVariantConst(_slot->data());
  }
  VariantConstPtr operator->() {
    return VariantConstPtr(_slot->data());
  }
  bool operator==(const JsonArrayConstIterator& other) const {
    return _slot == other._slot;
  }
  bool operator!=(const JsonArrayConstIterator& other) const {
    return _slot != other._slot;
  }
  JsonArrayConstIterator& operator++() {
    _slot = _slot->next();
    return *this;
  }
  JsonArrayConstIterator& operator+=(size_t distance) {
    _slot = _slot->next(distance);
    return *this;
  }
 private:
  const VariantSlot* _slot;
};
class JsonObject;
class JsonArrayConst : public VariantOperators<JsonArrayConst> {
  friend class JsonArray;
  friend class VariantAttorney;
 public:
  typedef JsonArrayConstIterator iterator;
  FORCE_INLINE iterator begin() const {
    if (!_data)
      return iterator();
    return iterator(_data->head());
  }
  FORCE_INLINE iterator end() const {
    return iterator();
  }
  FORCE_INLINE JsonArrayConst() : _data(0) {}
  FORCE_INLINE JsonArrayConst(const CollectionData* data) : _data(data) {}
  FORCE_INLINE bool operator==(JsonArrayConst rhs) const {
    if (_data == rhs._data)
      return true;
    if (!_data || !rhs._data)
      return false;
    iterator it1 = begin();
    iterator it2 = rhs.begin();
    for (;;) {
      bool end1 = it1 == end();
      bool end2 = it2 == rhs.end();
      if (end1 && end2)
        return true;
      if (end1 || end2)
        return false;
      if (*it1 != *it2)
        return false;
      ++it1;
      ++it2;
    }
  }
  FORCE_INLINE JsonVariantConst operator[](size_t index) const {
    return JsonVariantConst(_data ? _data->getElement(index) : 0);
  }
  operator JsonVariantConst() const {
    return JsonVariantConst(collectionToVariant(_data));
  }
  FORCE_INLINE bool isNull() const {
    return _data == 0;
  }
  FORCE_INLINE operator bool() const {
    return _data != 0;
  }
  FORCE_INLINE size_t memoryUsage() const {
    return _data ? _data->memoryUsage() : 0;
  }
  FORCE_INLINE size_t nesting() const {
    return variantNesting(collectionToVariant(_data));
  }
  FORCE_INLINE size_t size() const {
    return _data ? _data->size() : 0;
  }
 private:
  const VariantData* getData() const {
    return collectionToVariant(_data);
  }
  const CollectionData* _data;
};
template <>
struct Converter<JsonArrayConst> : private VariantAttorney {
  static void toJson(JsonVariantConst src, JsonVariant dst) {
    variantCopyFrom(getData(dst), getData(src), getPool(dst));
  }
  static JsonArrayConst fromJson(JsonVariantConst src) {
    const VariantData* data = getData(src);
    return data ? data->asArray() : 0;
  }
  static bool checkJson(JsonVariantConst src) {
    const VariantData* data = getData(src);
    return data && data->isArray();
  }
};
class JsonObject;
class JsonArray : public VariantOperators<JsonArray> {
  friend class VariantAttorney;
 public:
  typedef JsonArrayIterator iterator;
  FORCE_INLINE JsonArray() : _data(0), _pool(0) {}
  FORCE_INLINE JsonArray(MemoryPool* pool, CollectionData* data)
      : _data(data), _pool(pool) {}
  operator JsonVariant() {
    void* data = _data;  // prevent warning cast-align
    return JsonVariant(_pool, reinterpret_cast<VariantData*>(data));
  }
  operator JsonArrayConst() const {
    return JsonArrayConst(_data);
  }
  JsonVariant add() const {
    if (!_data)
      return JsonVariant();
    return JsonVariant(_pool, _data->addElement(_pool));
  }
  template <typename T>
  FORCE_INLINE bool add(const T& value) const {
    return add().set(value);
  }
  template <typename T>
  FORCE_INLINE bool add(T* value) const {
    return add().set(value);
  }
  FORCE_INLINE iterator begin() const {
    if (!_data)
      return iterator();
    return iterator(_pool, _data->head());
  }
  FORCE_INLINE iterator end() const {
    return iterator();
  }
  FORCE_INLINE bool set(JsonArrayConst src) const {
    if (!_data || !src._data)
      return false;
    return _data->copyFrom(*src._data, _pool);
  }
  FORCE_INLINE bool operator==(JsonArray rhs) const {
    return JsonArrayConst(_data) == JsonArrayConst(rhs._data);
  }
  FORCE_INLINE void remove(iterator it) const {
    if (!_data)
      return;
    _data->removeSlot(it._slot);
  }
  FORCE_INLINE void remove(size_t index) const {
    if (!_data)
      return;
    _data->removeElement(index);
  }
  void clear() const {
    if (!_data)
      return;
    _data->clear();
  }
  FORCE_INLINE ElementProxy<JsonArray> operator[](size_t index) const {
    return ElementProxy<JsonArray>(*this, index);
  }
  FORCE_INLINE JsonObject createNestedObject() const;
  FORCE_INLINE JsonArray createNestedArray() const {
    return add().to<JsonArray>();
  }
  operator JsonVariantConst() const {
    return JsonVariantConst(collectionToVariant(_data));
  }
  FORCE_INLINE bool isNull() const {
    return _data == 0;
  }
  FORCE_INLINE operator bool() const {
    return _data != 0;
  }
  FORCE_INLINE size_t memoryUsage() const {
    return _data ? _data->memoryUsage() : 0;
  }
  FORCE_INLINE size_t nesting() const {
    return variantNesting(collectionToVariant(_data));
  }
  FORCE_INLINE size_t size() const {
    return _data ? _data->size() : 0;
  }
 private:
  MemoryPool* getPool() const {
    return _pool;
  }
  VariantData* getData() const {
    return collectionToVariant(_data);
  }
  VariantData* getOrCreateData() const {
    return collectionToVariant(_data);
  }
  CollectionData* _data;
  MemoryPool* _pool;
};
template <>
struct Converter<JsonArray> : private VariantAttorney {
  static void toJson(JsonVariantConst src, JsonVariant dst) {
    variantCopyFrom(getData(dst), getData(src), getPool(dst));
  }
  static JsonArray fromJson(JsonVariant src) {
    VariantData* data = getData(src);
    MemoryPool* pool = getPool(src);
    return JsonArray(pool, data != 0 ? data->asArray() : 0);
  }
  static InvalidConversion<JsonVariantConst, JsonArray> fromJson(
      JsonVariantConst);
  static bool checkJson(JsonVariantConst) {
    return false;
  }
  static bool checkJson(JsonVariant src) {
    VariantData* data = getData(src);
    return data && data->isArray();
  }
};
class JsonPair {
 public:
  JsonPair(MemoryPool* pool, VariantSlot* slot) {
    if (slot) {
      _key = JsonString(slot->key(), slot->ownsKey() ? JsonString::Copied
                                                     : JsonString::Linked);
      _value = JsonVariant(pool, slot->data());
    }
  }
  JsonString key() const {
    return _key;
  }
  JsonVariant value() const {
    return _value;
  }
 private:
  JsonString _key;
  JsonVariant _value;
};
class JsonPairConst {
 public:
  JsonPairConst(const VariantSlot* slot) {
    if (slot) {
      _key = JsonString(slot->key(), slot->ownsKey() ? JsonString::Copied
                                                     : JsonString::Linked);
      _value = JsonVariantConst(slot->data());
    }
  }
  JsonString key() const {
    return _key;
  }
  JsonVariantConst value() const {
    return _value;
  }
 private:
  JsonString _key;
  JsonVariantConst _value;
};
class JsonPairPtr {
 public:
  JsonPairPtr(MemoryPool* pool, VariantSlot* slot) : _pair(pool, slot) {}
  const JsonPair* operator->() const {
    return &_pair;
  }
  const JsonPair& operator*() const {
    return _pair;
  }
 private:
  JsonPair _pair;
};
class JsonObjectIterator {
  friend class JsonObject;
 public:
  JsonObjectIterator() : _slot(0) {}
  explicit JsonObjectIterator(MemoryPool* pool, VariantSlot* slot)
      : _pool(pool), _slot(slot) {}
  JsonPair operator*() const {
    return JsonPair(_pool, _slot);
  }
  JsonPairPtr operator->() {
    return JsonPairPtr(_pool, _slot);
  }
  bool operator==(const JsonObjectIterator& other) const {
    return _slot == other._slot;
  }
  bool operator!=(const JsonObjectIterator& other) const {
    return _slot != other._slot;
  }
  JsonObjectIterator& operator++() {
    _slot = _slot->next();
    return *this;
  }
  JsonObjectIterator& operator+=(size_t distance) {
    _slot = _slot->next(distance);
    return *this;
  }
 private:
  MemoryPool* _pool;
  VariantSlot* _slot;
};
class JsonPairConstPtr {
 public:
  JsonPairConstPtr(const VariantSlot* slot) : _pair(slot) {}
  const JsonPairConst* operator->() const {
    return &_pair;
  }
  const JsonPairConst& operator*() const {
    return _pair;
  }
 private:
  JsonPairConst _pair;
};
class JsonObjectConstIterator {
  friend class JsonObject;
 public:
  JsonObjectConstIterator() : _slot(0) {}
  explicit JsonObjectConstIterator(const VariantSlot* slot) : _slot(slot) {}
  JsonPairConst operator*() const {
    return JsonPairConst(_slot);
  }
  JsonPairConstPtr operator->() {
    return JsonPairConstPtr(_slot);
  }
  bool operator==(const JsonObjectConstIterator& other) const {
    return _slot == other._slot;
  }
  bool operator!=(const JsonObjectConstIterator& other) const {
    return _slot != other._slot;
  }
  JsonObjectConstIterator& operator++() {
    _slot = _slot->next();
    return *this;
  }
  JsonObjectConstIterator& operator+=(size_t distance) {
    _slot = _slot->next(distance);
    return *this;
  }
 private:
  const VariantSlot* _slot;
};
class JsonObjectConst : public VariantOperators<JsonObjectConst> {
  friend class JsonObject;
  friend class VariantAttorney;
 public:
  typedef JsonObjectConstIterator iterator;
  JsonObjectConst() : _data(0) {}
  JsonObjectConst(const CollectionData* data) : _data(data) {}
  operator JsonVariantConst() const {
    return JsonVariantConst(collectionToVariant(_data));
  }
  FORCE_INLINE bool isNull() const {
    return _data == 0;
  }
  FORCE_INLINE operator bool() const {
    return _data != 0;
  }
  FORCE_INLINE size_t memoryUsage() const {
    return _data ? _data->memoryUsage() : 0;
  }
  FORCE_INLINE size_t nesting() const {
    return variantNesting(collectionToVariant(_data));
  }
  FORCE_INLINE size_t size() const {
    return _data ? _data->size() : 0;
  }
  FORCE_INLINE iterator begin() const {
    if (!_data)
      return iterator();
    return iterator(_data->head());
  }
  FORCE_INLINE iterator end() const {
    return iterator();
  }
  template <typename TString>
  FORCE_INLINE bool containsKey(const TString& key) const {
    return getMember(adaptString(key)) != 0;
  }
  template <typename TChar>
  FORCE_INLINE bool containsKey(TChar* key) const {
    return getMember(adaptString(key)) != 0;
  }
  template <typename TString>
  FORCE_INLINE
      typename enable_if<IsString<TString>::value, JsonVariantConst>::type
      operator[](const TString& key) const {
    return JsonVariantConst(getMember(adaptString(key)));
  }
  template <typename TChar>
  FORCE_INLINE
      typename enable_if<IsString<TChar*>::value, JsonVariantConst>::type
      operator[](TChar* key) const {
    return JsonVariantConst(getMember(adaptString(key)));
  }
  FORCE_INLINE bool operator==(JsonObjectConst rhs) const {
    if (_data == rhs._data)
      return true;
    if (!_data || !rhs._data)
      return false;
    size_t count = 0;
    for (iterator it = begin(); it != end(); ++it) {
      if (it->value() != rhs[it->key()])
        return false;
      count++;
    }
    return count == rhs.size();
  }
 private:
  const VariantData* getData() const {
    return collectionToVariant(_data);
  }
  template <typename TAdaptedString>
  const VariantData* getMember(TAdaptedString key) const {
    if (!_data)
      return 0;
    return _data->getMember(key);
  }
  const CollectionData* _data;
};
template <>
struct Converter<JsonObjectConst> : private VariantAttorney {
  static void toJson(JsonVariantConst src, JsonVariant dst) {
    variantCopyFrom(getData(dst), getData(src), getPool(dst));
  }
  static JsonObjectConst fromJson(JsonVariantConst src) {
    const VariantData* data = getData(src);
    return data != 0 ? data->asObject() : 0;
  }
  static bool checkJson(JsonVariantConst src) {
    const VariantData* data = getData(src);
    return data && data->isObject();
  }
};
template <typename TUpstream, typename TStringRef>
class MemberProxy
    : public VariantRefBase<MemberProxy<TUpstream, TStringRef> >,
      public VariantOperators<MemberProxy<TUpstream, TStringRef> > {
  friend class VariantAttorney;
 public:
  FORCE_INLINE MemberProxy(TUpstream upstream, TStringRef key)
      : _upstream(upstream), _key(key) {}
  MemberProxy(const MemberProxy& src)
      : _upstream(src._upstream), _key(src._key) {}
  FORCE_INLINE MemberProxy& operator=(const MemberProxy& src) {
    this->set(src);
    return *this;
  }
  template <typename T>
  FORCE_INLINE MemberProxy& operator=(const T& src) {
    this->set(src);
    return *this;
  }
  template <typename T>
  FORCE_INLINE MemberProxy& operator=(T* src) {
    this->set(src);
    return *this;
  }
 private:
  FORCE_INLINE MemoryPool* getPool() const {
    return VariantAttorney::getPool(_upstream);
  }
  FORCE_INLINE VariantData* getData() const {
    return variantGetMember(VariantAttorney::getData(_upstream),
                            adaptString(_key));
  }
  FORCE_INLINE VariantData* getOrCreateData() const {
    return variantGetOrAddMember(VariantAttorney::getOrCreateData(_upstream),
                                 adaptString(_key),
                                 VariantAttorney::getPool(_upstream));
  }
 private:
  TUpstream _upstream;
  TStringRef _key;
};
class JsonArray;
class JsonObject : public VariantOperators<JsonObject> {
  friend class VariantAttorney;
 public:
  typedef JsonObjectIterator iterator;
  FORCE_INLINE JsonObject() : _data(0), _pool(0) {}
  FORCE_INLINE JsonObject(MemoryPool* buf, CollectionData* data)
      : _data(data), _pool(buf) {}
  operator JsonVariant() const {
    void* data = _data;  // prevent warning cast-align
    return JsonVariant(_pool, reinterpret_cast<VariantData*>(data));
  }
  operator JsonObjectConst() const {
    return JsonObjectConst(_data);
  }
  operator JsonVariantConst() const {
    return JsonVariantConst(collectionToVariant(_data));
  }
  FORCE_INLINE bool isNull() const {
    return _data == 0;
  }
  FORCE_INLINE operator bool() const {
    return _data != 0;
  }
  FORCE_INLINE size_t memoryUsage() const {
    return _data ? _data->memoryUsage() : 0;
  }
  FORCE_INLINE size_t nesting() const {
    return variantNesting(collectionToVariant(_data));
  }
  FORCE_INLINE size_t size() const {
    return _data ? _data->size() : 0;
  }
  FORCE_INLINE iterator begin() const {
    if (!_data)
      return iterator();
    return iterator(_pool, _data->head());
  }
  FORCE_INLINE iterator end() const {
    return iterator();
  }
  void clear() const {
    if (!_data)
      return;
    _data->clear();
  }
  FORCE_INLINE bool set(JsonObjectConst src) {
    if (!_data || !src._data)
      return false;
    return _data->copyFrom(*src._data, _pool);
  }
  FORCE_INLINE bool operator==(JsonObject rhs) const {
    return JsonObjectConst(_data) == JsonObjectConst(rhs._data);
  }
  template <typename TString>
  FORCE_INLINE typename enable_if<IsString<TString>::value,
                                  MemberProxy<JsonObject, TString> >::type
  operator[](const TString& key) const {
    return MemberProxy<JsonObject, TString>(*this, key);
  }
  template <typename TChar>
  FORCE_INLINE typename enable_if<IsString<TChar*>::value,
                                  MemberProxy<JsonObject, TChar*> >::type
  operator[](TChar* key) const {
    return MemberProxy<JsonObject, TChar*>(*this, key);
  }
  FORCE_INLINE void remove(iterator it) const {
    if (!_data)
      return;
    _data->removeSlot(it._slot);
  }
  template <typename TString>
  FORCE_INLINE void remove(const TString& key) const {
    removeMember(adaptString(key));
  }
  template <typename TChar>
  FORCE_INLINE void remove(TChar* key) const {
    removeMember(adaptString(key));
  }
  template <typename TString>
  FORCE_INLINE typename enable_if<IsString<TString>::value, bool>::type
  containsKey(const TString& key) const {
    return getMember(adaptString(key)) != 0;
  }
  template <typename TChar>
  FORCE_INLINE typename enable_if<IsString<TChar*>::value, bool>::type
  containsKey(TChar* key) const {
    return getMember(adaptString(key)) != 0;
  }
  template <typename TString>
  FORCE_INLINE JsonArray createNestedArray(const TString& key) const;
  template <typename TChar>
  FORCE_INLINE JsonArray createNestedArray(TChar* key) const;
  template <typename TString>
  JsonObject createNestedObject(const TString& key) const {
    return operator[](key).template to<JsonObject>();
  }
  template <typename TChar>
  JsonObject createNestedObject(TChar* key) const {
    return operator[](key).template to<JsonObject>();
  }
 private:
  MemoryPool* getPool() const {
    return _pool;
  }
  VariantData* getData() const {
    return collectionToVariant(_data);
  }
  VariantData* getOrCreateData() const {
    return collectionToVariant(_data);
  }
  template <typename TAdaptedString>
  inline VariantData* getMember(TAdaptedString key) const {
    if (!_data)
      return 0;
    return _data->getMember(key);
  }
  template <typename TAdaptedString>
  void removeMember(TAdaptedString key) const {
    if (!_data)
      return;
    _data->removeMember(key);
  }
  CollectionData* _data;
  MemoryPool* _pool;
};
template <>
struct Converter<JsonObject> : private VariantAttorney {
  static void toJson(JsonVariantConst src, JsonVariant dst) {
    variantCopyFrom(getData(dst), getData(src), getPool(dst));
  }
  static JsonObject fromJson(JsonVariant src) {
    VariantData* data = getData(src);
    MemoryPool* pool = getPool(src);
    return JsonObject(pool, data != 0 ? data->asObject() : 0);
  }
  static InvalidConversion<JsonVariantConst, JsonObject> fromJson(
      JsonVariantConst);
  static bool checkJson(JsonVariantConst) {
    return false;
  }
  static bool checkJson(JsonVariant src) {
    VariantData* data = getData(src);
    return data && data->isObject();
  }
};
}  // namespace ARDUINOJSON_NAMESPACE

namespace ARDUINOJSON_NAMESPACE {
class JsonDocument : public VariantOperators<const JsonDocument&> {
  friend class VariantAttorney;
 public:
  template <typename T>
  T as() {
    return getVariant().template as<T>();
  }
  template <typename T>
  T as() const {
    return getVariant().template as<T>();
  }
  void clear() {
    _pool.clear();
    _data.init();
  }
  template <typename T>
  bool is() {
    return getVariant().template is<T>();
  }
  template <typename T>
  bool is() const {
    return getVariant().template is<T>();
  }
  bool isNull() const {
    return getVariant().isNull();
  }
  size_t memoryUsage() const {
    return _pool.size();
  }
  bool overflowed() const {
    return _pool.overflowed();
  }
  size_t nesting() const {
    return variantNesting(&_data);
  }
  size_t capacity() const {
    return _pool.capacity();
  }
  size_t size() const {
    return _data.size();
  }
  bool set(const JsonDocument& src) {
    return to<JsonVariant>().set(src.as<JsonVariantConst>());
  }
  template <typename T>
  typename enable_if<!is_base_of<JsonDocument, T>::value, bool>::type set(
      const T& src) {
    return to<JsonVariant>().set(src);
  }
  template <typename T>
  typename VariantTo<T>::type to() {
    clear();
    return getVariant().template to<T>();
  }
  JsonArray createNestedArray() {
    return add().to<JsonArray>();
  }
  template <typename TChar>
  JsonArray createNestedArray(TChar* key) {
    return operator[](key).template to<JsonArray>();
  }
  template <typename TString>
  JsonArray createNestedArray(const TString& key) {
    return operator[](key).template to<JsonArray>();
  }
  JsonObject createNestedObject() {
    return add().to<JsonObject>();
  }
  template <typename TChar>
  JsonObject createNestedObject(TChar* key) {
    return operator[](key).template to<JsonObject>();
  }
  template <typename TString>
  JsonObject createNestedObject(const TString& key) {
    return operator[](key).template to<JsonObject>();
  }
  template <typename TChar>
  bool containsKey(TChar* key) const {
    return _data.getMember(adaptString(key)) != 0;
  }
  template <typename TString>
  bool containsKey(const TString& key) const {
    return _data.getMember(adaptString(key)) != 0;
  }
  template <typename TString>
  FORCE_INLINE typename enable_if<IsString<TString>::value,
                                  MemberProxy<JsonDocument&, TString> >::type
  operator[](const TString& key) {
    return MemberProxy<JsonDocument&, TString>(*this, key);
  }
  template <typename TChar>
  FORCE_INLINE typename enable_if<IsString<TChar*>::value,
                                  MemberProxy<JsonDocument&, TChar*> >::type
  operator[](TChar* key) {
    return MemberProxy<JsonDocument&, TChar*>(*this, key);
  }
  template <typename TString>
  FORCE_INLINE
      typename enable_if<IsString<TString>::value, JsonVariantConst>::type
      operator[](const TString& key) const {
    return JsonVariantConst(_data.getMember(adaptString(key)));
  }
  template <typename TChar>
  FORCE_INLINE
      typename enable_if<IsString<TChar*>::value, JsonVariantConst>::type
      operator[](TChar* key) const {
    return JsonVariantConst(_data.getMember(adaptString(key)));
  }
  FORCE_INLINE ElementProxy<JsonDocument&> operator[](size_t index) {
    return ElementProxy<JsonDocument&>(*this, index);
  }
  FORCE_INLINE JsonVariantConst operator[](size_t index) const {
    return JsonVariantConst(_data.getElement(index));
  }
  FORCE_INLINE JsonVariant add() {
    return JsonVariant(&_pool, _data.addElement(&_pool));
  }
  template <typename TValue>
  FORCE_INLINE bool add(const TValue& value) {
    return add().set(value);
  }
  template <typename TChar>
  FORCE_INLINE bool add(TChar* value) {
    return add().set(value);
  }
  FORCE_INLINE void remove(size_t index) {
    _data.remove(index);
  }
  template <typename TChar>
  FORCE_INLINE typename enable_if<IsString<TChar*>::value>::type remove(
      TChar* key) {
    _data.remove(adaptString(key));
  }
  template <typename TString>
  FORCE_INLINE typename enable_if<IsString<TString>::value>::type remove(
      const TString& key) {
    _data.remove(adaptString(key));
  }
  FORCE_INLINE operator JsonVariant() {
    return getVariant();
  }
  FORCE_INLINE operator JsonVariantConst() const {
    return getVariant();
  }
 protected:
  JsonDocument() : _pool(0, 0) {
    _data.init();
  }
  JsonDocument(MemoryPool pool) : _pool(pool) {
    _data.init();
  }
  JsonDocument(char* buf, size_t capa) : _pool(buf, capa) {
    _data.init();
  }
  ~JsonDocument() {}
  void replacePool(MemoryPool pool) {
    _pool = pool;
  }
  JsonVariant getVariant() {
    return JsonVariant(&_pool, &_data);
  }
  JsonVariantConst getVariant() const {
    return JsonVariantConst(&_data);
  }
  MemoryPool _pool;
  VariantData _data;
 private:
  JsonDocument(const JsonDocument&);
  JsonDocument& operator=(const JsonDocument&);
 protected:
  MemoryPool* getPool() {
    return &_pool;
  }
  VariantData* getData() {
    return &_data;
  }
  const VariantData* getData() const {
    return &_data;
  }
  VariantData* getOrCreateData() {
    return &_data;
  }
};
inline void convertToJson(const JsonDocument& src, JsonVariant dst) {
  dst.set(src.as<JsonVariantConst>());
}
template <typename TAllocator>
class AllocatorOwner {
 public:
  AllocatorOwner() {}
  AllocatorOwner(TAllocator a) : _allocator(a) {}
  void* allocate(size_t size) {
    return _allocator.allocate(size);
  }
  void deallocate(void* ptr) {
    if (ptr)
      _allocator.deallocate(ptr);
  }
  void* reallocate(void* ptr, size_t new_size) {
    return _allocator.reallocate(ptr, new_size);
  }
  TAllocator& allocator() {
    return _allocator;
  }
 private:
  TAllocator _allocator;
};
template <typename TAllocator>
class BasicJsonDocument : AllocatorOwner<TAllocator>, public JsonDocument {
 public:
  explicit BasicJsonDocument(size_t capa, TAllocator alloc = TAllocator())
      : AllocatorOwner<TAllocator>(alloc), JsonDocument(allocPool(capa)) {}
  BasicJsonDocument(const BasicJsonDocument& src)
      : AllocatorOwner<TAllocator>(src), JsonDocument() {
    copyAssignFrom(src);
  }
#if ARDUINOJSON_HAS_RVALUE_REFERENCES
  BasicJsonDocument(BasicJsonDocument&& src) : AllocatorOwner<TAllocator>(src) {
    moveAssignFrom(src);
  }
#endif
  BasicJsonDocument(const JsonDocument& src) {
    copyAssignFrom(src);
  }
  template <typename T>
  BasicJsonDocument(
      const T& src,
      typename enable_if<
          is_same<T, JsonVariant>::value ||
          is_same<T, JsonVariantConst>::value || is_same<T, JsonArray>::value ||
          is_same<T, JsonArrayConst>::value || is_same<T, JsonObject>::value ||
          is_same<T, JsonObjectConst>::value>::type* = 0)
      : JsonDocument(allocPool(src.memoryUsage())) {
    set(src);
  }
  BasicJsonDocument(JsonVariant src)
      : JsonDocument(allocPool(src.memoryUsage())) {
    set(src);
  }
  ~BasicJsonDocument() {
    freePool();
  }
  BasicJsonDocument& operator=(const BasicJsonDocument& src) {
    copyAssignFrom(src);
    return *this;
  }
#if ARDUINOJSON_HAS_RVALUE_REFERENCES
  BasicJsonDocument& operator=(BasicJsonDocument&& src) {
    moveAssignFrom(src);
    return *this;
  }
#endif
  template <typename T>
  BasicJsonDocument& operator=(const T& src) {
    size_t requiredSize = src.memoryUsage();
    if (requiredSize > capacity())
      reallocPool(requiredSize);
    set(src);
    return *this;
  }
  void shrinkToFit() {
    ptrdiff_t bytes_reclaimed = _pool.squash();
    if (bytes_reclaimed == 0)
      return;
    void* old_ptr = _pool.buffer();
    void* new_ptr = this->reallocate(old_ptr, _pool.capacity());
    ptrdiff_t ptr_offset =
        static_cast<char*>(new_ptr) - static_cast<char*>(old_ptr);
    _pool.movePointers(ptr_offset);
    _data.movePointers(ptr_offset, ptr_offset - bytes_reclaimed);
  }
  bool garbageCollect() {
    BasicJsonDocument tmp(*this);
    if (!tmp.capacity())
      return false;
    tmp.set(*this);
    moveAssignFrom(tmp);
    return true;
  }
  using AllocatorOwner<TAllocator>::allocator;
 private:
  MemoryPool allocPool(size_t requiredSize) {
    size_t capa = addPadding(requiredSize);
    return MemoryPool(reinterpret_cast<char*>(this->allocate(capa)), capa);
  }
  void reallocPool(size_t requiredSize) {
    size_t capa = addPadding(requiredSize);
    if (capa == _pool.capacity())
      return;
    freePool();
    replacePool(allocPool(addPadding(requiredSize)));
  }
  void freePool() {
    this->deallocate(getPool()->buffer());
  }
  void copyAssignFrom(const JsonDocument& src) {
    reallocPool(src.capacity());
    set(src);
  }
  void moveAssignFrom(BasicJsonDocument& src) {
    freePool();
    _data = src._data;
    _pool = src._pool;
    src._data.setNull();
    src._pool = MemoryPool(0, 0);
  }
};
struct DefaultAllocator {
  void* allocate(size_t size) {
    return malloc(size);
  }
  void deallocate(void* ptr) {
    free(ptr);
  }
  void* reallocate(void* ptr, size_t new_size) {
    return realloc(ptr, new_size);
  }
};
typedef BasicJsonDocument<DefaultAllocator> DynamicJsonDocument;
template <size_t desiredCapacity>
class StaticJsonDocument : public JsonDocument {
  static const size_t _capacity =
      AddPadding<Max<1, desiredCapacity>::value>::value;
 public:
  StaticJsonDocument() : JsonDocument(_buffer, _capacity) {}
  StaticJsonDocument(const StaticJsonDocument& src)
      : JsonDocument(_buffer, _capacity) {
    set(src);
  }
  template <typename T>
  StaticJsonDocument(
      const T& src,
      typename enable_if<is_convertible<T, JsonVariantConst>::value>::type* = 0)
      : JsonDocument(_buffer, _capacity) {
    set(src);
  }
  StaticJsonDocument(JsonVariant src) : JsonDocument(_buffer, _capacity) {
    set(src);
  }
  StaticJsonDocument& operator=(const StaticJsonDocument& src) {
    set(src);
    return *this;
  }
  template <typename T>
  StaticJsonDocument& operator=(const T& src) {
    set(src);
    return *this;
  }
  void garbageCollect() {
    StaticJsonDocument tmp(*this);
    set(tmp);
  }
 private:
  char _buffer[_capacity];
};
}  // namespace ARDUINOJSON_NAMESPACE

namespace ARDUINOJSON_NAMESPACE {
inline JsonObject JsonArray::createNestedObject() const {
  return add().to<JsonObject>();
}
template <typename TDerived>
inline JsonArray VariantRefBase<TDerived>::createNestedArray() const {
  return add().template to<JsonArray>();
}
template <typename TDerived>
inline JsonObject VariantRefBase<TDerived>::createNestedObject() const {
  return add().template to<JsonObject>();
}
template <typename TDerived>
inline ElementProxy<TDerived> VariantRefBase<TDerived>::operator[](
    size_t index) const {
  return ElementProxy<TDerived>(derived(), index);
}
template <typename T>
inline typename enable_if<!is_array<T>::value, bool>::type copyArray(
    const T& src, JsonVariant dst) {
  return dst.set(src);
}
template <typename T, size_t N, typename TDestination>
inline typename enable_if<!is_base_of<JsonDocument, TDestination>::value,
                          bool>::type
copyArray(T (&src)[N], const TDestination& dst) {
  return copyArray(src, N, dst);
}
template <typename T, typename TDestination>
inline typename enable_if<!is_base_of<JsonDocument, TDestination>::value,
                          bool>::type
copyArray(const T* src, size_t len, const TDestination& dst) {
  bool ok = true;
  for (size_t i = 0; i < len; i++) {
    ok &= copyArray(src[i], dst.add());
  }
  return ok;
}
template <typename TDestination>
inline bool copyArray(const char* src, size_t, const TDestination& dst) {
  return dst.set(src);
}
template <typename T>
inline bool copyArray(const T& src, JsonDocument& dst) {
  return copyArray(src, dst.to<JsonArray>());
}
template <typename T>
inline bool copyArray(const T* src, size_t len, JsonDocument& dst) {
  return copyArray(src, len, dst.to<JsonArray>());
}
template <typename T>
inline typename enable_if<!is_array<T>::value, size_t>::type copyArray(
    JsonVariantConst src, T& dst) {
  dst = src.as<T>();
  return 1;
}
template <typename T, size_t N>
inline size_t copyArray(JsonArrayConst src, T (&dst)[N]) {
  return copyArray(src, dst, N);
}
template <typename T>
inline size_t copyArray(JsonArrayConst src, T* dst, size_t len) {
  size_t i = 0;
  for (JsonArrayConst::iterator it = src.begin(); it != src.end() && i < len;
       ++it)
    copyArray(*it, dst[i++]);
  return i;
}
template <size_t N>
inline size_t copyArray(JsonVariantConst src, char (&dst)[N]) {
  JsonString s = src;
  size_t len = N - 1;
  if (len > s.size())
    len = s.size();
  memcpy(dst, s.c_str(), len);
  dst[len] = 0;
  return 1;
}
template <typename TSource, typename T>
inline typename enable_if<is_array<T>::value &&
                              is_base_of<JsonDocument, TSource>::value,
                          size_t>::type
copyArray(const TSource& src, T& dst) {
  return copyArray(src.template as<JsonArrayConst>(), dst);
}
inline VariantSlot* CollectionData::addSlot(MemoryPool* pool) {
  VariantSlot* slot = pool->allocVariant();
  if (!slot)
    return 0;
  if (_tail) {
    ARDUINOJSON_ASSERT(pool->owns(_tail));  // Can't alter a linked array/object
    _tail->setNextNotNull(slot);
    _tail = slot;
  } else {
    _head = slot;
    _tail = slot;
  }
  slot->clear();
  return slot;
}
inline VariantData* CollectionData::addElement(MemoryPool* pool) {
  return slotData(addSlot(pool));
}
template <typename TAdaptedString>
inline VariantData* CollectionData::addMember(TAdaptedString key,
                                              MemoryPool* pool) {
  VariantSlot* slot = addSlot(pool);
  if (!slotSetKey(slot, key, pool)) {
    removeSlot(slot);
    return 0;
  }
  return slot->data();
}
inline void CollectionData::clear() {
  _head = 0;
  _tail = 0;
}
template <typename TAdaptedString>
inline bool CollectionData::containsKey(const TAdaptedString& key) const {
  return getSlot(key) != 0;
}
inline bool CollectionData::copyFrom(const CollectionData& src,
                                     MemoryPool* pool) {
  clear();
  for (VariantSlot* s = src._head; s; s = s->next()) {
    VariantData* var;
    if (s->key() != 0) {
      JsonString key(s->key(),
                     s->ownsKey() ? JsonString::Copied : JsonString::Linked);
      var = addMember(adaptString(key), pool);
    } else {
      var = addElement(pool);
    }
    if (!var)
      return false;
    if (!var->copyFrom(*s->data(), pool))
      return false;
  }
  return true;
}
template <typename TAdaptedString>
inline VariantSlot* CollectionData::getSlot(TAdaptedString key) const {
  if (key.isNull())
    return 0;
  VariantSlot* slot = _head;
  while (slot) {
    if (stringEquals(key, adaptString(slot->key())))
      break;
    slot = slot->next();
  }
  return slot;
}
inline VariantSlot* CollectionData::getSlot(size_t index) const {
  if (!_head)
    return 0;
  return _head->next(index);
}
inline VariantSlot* CollectionData::getPreviousSlot(VariantSlot* target) const {
  VariantSlot* current = _head;
  while (current) {
    VariantSlot* next = current->next();
    if (next == target)
      return current;
    current = next;
  }
  return 0;
}
template <typename TAdaptedString>
inline VariantData* CollectionData::getMember(TAdaptedString key) const {
  VariantSlot* slot = getSlot(key);
  return slot ? slot->data() : 0;
}
template <typename TAdaptedString>
inline VariantData* CollectionData::getOrAddMember(TAdaptedString key,
                                                   MemoryPool* pool) {
  if (key.isNull())
    return 0;
  VariantSlot* slot = getSlot(key);
  if (slot)
    return slot->data();
  return addMember(key, pool);
}
inline VariantData* CollectionData::getElement(size_t index) const {
  VariantSlot* slot = getSlot(index);
  return slot ? slot->data() : 0;
}
inline VariantData* CollectionData::getOrAddElement(size_t index,
                                                    MemoryPool* pool) {
  VariantSlot* slot = _head;
  while (slot && index > 0) {
    slot = slot->next();
    index--;
  }
  if (!slot)
    index++;
  while (index > 0) {
    slot = addSlot(pool);
    index--;
  }
  return slotData(slot);
}
inline void CollectionData::removeSlot(VariantSlot* slot) {
  if (!slot)
    return;
  VariantSlot* prev = getPreviousSlot(slot);
  VariantSlot* next = slot->next();
  if (prev)
    prev->setNext(next);
  else
    _head = next;
  if (!next)
    _tail = prev;
}
inline void CollectionData::removeElement(size_t index) {
  removeSlot(getSlot(index));
}
inline size_t CollectionData::memoryUsage() const {
  size_t total = 0;
  for (VariantSlot* s = _head; s; s = s->next()) {
    total += sizeof(VariantSlot) + s->data()->memoryUsage();
    if (s->ownsKey())
      total += strlen(s->key()) + 1;
  }
  return total;
}
inline size_t CollectionData::size() const {
  return slotSize(_head);
}
template <typename T>
inline void movePointer(T*& p, ptrdiff_t offset) {
  if (!p)
    return;
  p = reinterpret_cast<T*>(
      reinterpret_cast<void*>(reinterpret_cast<char*>(p) + offset));
  ARDUINOJSON_ASSERT(isAligned(p));
}
inline void CollectionData::movePointers(ptrdiff_t stringDistance,
                                         ptrdiff_t variantDistance) {
  movePointer(_head, variantDistance);
  movePointer(_tail, variantDistance);
  for (VariantSlot* slot = _head; slot; slot = slot->next())
    slot->movePointers(stringDistance, variantDistance);
}
template <typename TString>
inline JsonArray JsonObject::createNestedArray(const TString& key) const {
  return operator[](key).template to<JsonArray>();
}
template <typename TChar>
inline JsonArray JsonObject::createNestedArray(TChar* key) const {
  return operator[](key).template to<JsonArray>();
}
template <typename TDerived>
template <typename TString>
inline JsonArray VariantRefBase<TDerived>::createNestedArray(
    const TString& key) const {
  return operator[](key).template to<JsonArray>();
}
template <typename TDerived>
template <typename TChar>
inline JsonArray VariantRefBase<TDerived>::createNestedArray(TChar* key) const {
  return operator[](key).template to<JsonArray>();
}
template <typename TDerived>
template <typename TString>
inline JsonObject VariantRefBase<TDerived>::createNestedObject(
    const TString& key) const {
  return operator[](key).template to<JsonObject>();
}
template <typename TDerived>
template <typename TChar>
inline JsonObject VariantRefBase<TDerived>::createNestedObject(
    TChar* key) const {
  return operator[](key).template to<JsonObject>();
}
template <typename TDerived>
template <typename TString>
inline typename enable_if<IsString<TString>::value, bool>::type
VariantRefBase<TDerived>::containsKey(const TString& key) const {
  return variantGetMember(VariantAttorney::getData(derived()),
                          adaptString(key)) != 0;
}
template <typename TDerived>
template <typename TChar>
inline typename enable_if<IsString<TChar*>::value, bool>::type
VariantRefBase<TDerived>::containsKey(TChar* key) const {
  return variantGetMember(VariantAttorney::getData(derived()),
                          adaptString(key)) != 0;
}
template <typename TDerived>
template <typename TString>
inline typename enable_if<IsString<TString*>::value,
                          MemberProxy<TDerived, TString*> >::type
VariantRefBase<TDerived>::operator[](TString* key) const {
  return MemberProxy<TDerived, TString*>(derived(), key);
}
template <typename TDerived>
template <typename TString>
inline typename enable_if<IsString<TString>::value,
                          MemberProxy<TDerived, TString> >::type
VariantRefBase<TDerived>::operator[](const TString& key) const {
  return MemberProxy<TDerived, TString>(derived(), key);
}
class EscapeSequence {
 public:
  static char escapeChar(char c) {
    const char* p = escapeTable(true);
    while (p[0] && p[1] != c) {
      p += 2;
    }
    return p[0];
  }
  static char unescapeChar(char c) {
    const char* p = escapeTable(false);
    for (;;) {
      if (p[0] == '\0')
        return 0;
      if (p[0] == c)
        return p[1];
      p += 2;
    }
  }
 private:
  static const char* escapeTable(bool excludeSolidus) {
    return &"//\"\"\\\\b\bf\fn\nr\rt\t"[excludeSolidus ? 2 : 0];
  }
};
template <typename TFloat>
struct FloatParts {
  uint32_t integral;
  uint32_t decimal;
  int16_t exponent;
  int8_t decimalPlaces;
  FloatParts(TFloat value) {
    uint32_t maxDecimalPart = sizeof(TFloat) >= 8 ? 1000000000 : 1000000;
    decimalPlaces = sizeof(TFloat) >= 8 ? 9 : 6;
    exponent = normalize(value);
    integral = uint32_t(value);
    for (uint32_t tmp = integral; tmp >= 10; tmp /= 10) {
      maxDecimalPart /= 10;
      decimalPlaces--;
    }
    TFloat remainder = (value - TFloat(integral)) * TFloat(maxDecimalPart);
    decimal = uint32_t(remainder);
    remainder = remainder - TFloat(decimal);
    decimal += uint32_t(remainder * 2);
    if (decimal >= maxDecimalPart) {
      decimal = 0;
      integral++;
      if (exponent && integral >= 10) {
        exponent++;
        integral = 1;
      }
    }
    while (decimal % 10 == 0 && decimalPlaces > 0) {
      decimal /= 10;
      decimalPlaces--;
    }
  }
  static int16_t normalize(TFloat& value) {
    typedef FloatTraits<TFloat> traits;
    int16_t powersOf10 = 0;
    int8_t index = sizeof(TFloat) == 8 ? 8 : 5;
    int bit = 1 << index;
    if (value >= ARDUINOJSON_POSITIVE_EXPONENTIATION_THRESHOLD) {
      for (; index >= 0; index--) {
        if (value >= traits::positiveBinaryPowerOfTen(index)) {
          value *= traits::negativeBinaryPowerOfTen(index);
          powersOf10 = int16_t(powersOf10 + bit);
        }
        bit >>= 1;
      }
    }
    if (value > 0 && value <= ARDUINOJSON_NEGATIVE_EXPONENTIATION_THRESHOLD) {
      for (; index >= 0; index--) {
        if (value < traits::negativeBinaryPowerOfTenPlusOne(index)) {
          value *= traits::positiveBinaryPowerOfTen(index);
          powersOf10 = int16_t(powersOf10 - bit);
        }
        bit >>= 1;
      }
    }
    return powersOf10;
  }
};
template <typename TWriter>
class CountingDecorator {
 public:
  explicit CountingDecorator(TWriter& writer) : _writer(writer), _count(0) {}
  void write(uint8_t c) {
    _count += _writer.write(c);
  }
  void write(const uint8_t* s, size_t n) {
    _count += _writer.write(s, n);
  }
  size_t count() const {
    return _count;
  }
 private:
  TWriter _writer;
  size_t _count;
};
template <typename TWriter>
class TextFormatter {
 public:
  explicit TextFormatter(TWriter writer) : _writer(writer) {}
  size_t bytesWritten() const {
    return _writer.count();
  }
  void writeBoolean(bool value) {
    if (value)
      writeRaw("true");
    else
      writeRaw("false");
  }
  void writeString(const char* value) {
    ARDUINOJSON_ASSERT(value != NULL);
    writeRaw('\"');
    while (*value)
      writeChar(*value++);
    writeRaw('\"');
  }
  void writeString(const char* value, size_t n) {
    ARDUINOJSON_ASSERT(value != NULL);
    writeRaw('\"');
    while (n--)
      writeChar(*value++);
    writeRaw('\"');
  }
  void writeChar(char c) {
    char specialChar = EscapeSequence::escapeChar(c);
    if (specialChar) {
      writeRaw('\\');
      writeRaw(specialChar);
    } else if (c) {
      writeRaw(c);
    } else {
      writeRaw("\\u0000");
    }
  }
  template <typename T>
  void writeFloat(T value) {
    if (isnan(value))
      return writeRaw(ARDUINOJSON_ENABLE_NAN ? "NaN" : "null");
#if ARDUINOJSON_ENABLE_INFINITY
    if (value < 0.0) {
      writeRaw('-');
      value = -value;
    }
    if (isinf(value))
      return writeRaw("Infinity");
#else
    if (isinf(value))
      return writeRaw("null");
    if (value < 0.0) {
      writeRaw('-');
      value = -value;
    }
#endif
    FloatParts<T> parts(value);
    writeInteger(parts.integral);
    if (parts.decimalPlaces)
      writeDecimals(parts.decimal, parts.decimalPlaces);
    if (parts.exponent) {
      writeRaw('e');
      writeInteger(parts.exponent);
    }
  }
  template <typename T>
  typename enable_if<is_signed<T>::value>::type writeInteger(T value) {
    typedef typename make_unsigned<T>::type unsigned_type;
    unsigned_type unsigned_value;
    if (value < 0) {
      writeRaw('-');
      unsigned_value = unsigned_type(unsigned_type(~value) + 1);
    } else {
      unsigned_value = unsigned_type(value);
    }
    writeInteger(unsigned_value);
  }
  template <typename T>
  typename enable_if<is_unsigned<T>::value>::type writeInteger(T value) {
    char buffer[22];
    char* end = buffer + sizeof(buffer);
    char* begin = end;
    do {
      *--begin = char(value % 10 + '0');
      value = T(value / 10);
    } while (value);
    writeRaw(begin, end);
  }
  void writeDecimals(uint32_t value, int8_t width) {
    char buffer[16];
    char* end = buffer + sizeof(buffer);
    char* begin = end;
    while (width--) {
      *--begin = char(value % 10 + '0');
      value /= 10;
    }
    *--begin = '.';
    writeRaw(begin, end);
  }
  void writeRaw(const char* s) {
    _writer.write(reinterpret_cast<const uint8_t*>(s), strlen(s));
  }
  void writeRaw(const char* s, size_t n) {
    _writer.write(reinterpret_cast<const uint8_t*>(s), n);
  }
  void writeRaw(const char* begin, const char* end) {
    _writer.write(reinterpret_cast<const uint8_t*>(begin),
                  static_cast<size_t>(end - begin));
  }
  template <size_t N>
  void writeRaw(const char (&s)[N]) {
    _writer.write(reinterpret_cast<const uint8_t*>(s), N - 1);
  }
  void writeRaw(char c) {
    _writer.write(static_cast<uint8_t>(c));
  }
 protected:
  CountingDecorator<TWriter> _writer;
 private:
  TextFormatter& operator=(const TextFormatter&);  // cannot be assigned
};
class DummyWriter {
 public:
  size_t write(uint8_t) {
    return 1;
  }
  size_t write(const uint8_t*, size_t n) {
    return n;
  }
};
template <template <typename> class TSerializer>
size_t measure(JsonVariantConst source) {
  DummyWriter dp;
  TSerializer<DummyWriter> serializer(dp);
  return variantAccept(VariantAttorney::getData(source), serializer);
}
template <typename TDestination, typename Enable = void>
class Writer {
 public:
  explicit Writer(TDestination& dest) : _dest(&dest) {}
  size_t write(uint8_t c) {
    return _dest->write(c);
  }
  size_t write(const uint8_t* s, size_t n) {
    return _dest->write(s, n);
  }
 private:
  TDestination* _dest;
};
class StaticStringWriter {
 public:
  StaticStringWriter(char* buf, size_t size) : end(buf + size), p(buf) {}
  size_t write(uint8_t c) {
    if (p >= end)
      return 0;
    *p++ = static_cast<char>(c);
    return 1;
  }
  size_t write(const uint8_t* s, size_t n) {
    char* begin = p;
    while (p < end && n > 0) {
      *p++ = static_cast<char>(*s++);
      n--;
    }
    return size_t(p - begin);
  }
 private:
  char* end;
  char* p;
};
}  // namespace ARDUINOJSON_NAMESPACE
#if ARDUINOJSON_ENABLE_STD_STRING
namespace ARDUINOJSON_NAMESPACE {
template <class T>
struct is_std_string : false_type {};
template <class TCharTraits, class TAllocator>
struct is_std_string<std::basic_string<char, TCharTraits, TAllocator> >
    : true_type {};
template <typename TDestination>
class Writer<TDestination,
             typename enable_if<is_std_string<TDestination>::value>::type> {
 public:
  Writer(TDestination& str) : _str(&str) {}
  size_t write(uint8_t c) {
    _str->operator+=(static_cast<char>(c));
    return 1;
  }
  size_t write(const uint8_t* s, size_t n) {
    _str->append(reinterpret_cast<const char*>(s), n);
    return n;
  }
 private:
  TDestination* _str;
};
}  // namespace ARDUINOJSON_NAMESPACE
#endif
#if ARDUINOJSON_ENABLE_ARDUINO_STRING
namespace ARDUINOJSON_NAMESPACE {
template <>
class Writer< ::String, void> {
  static const size_t bufferCapacity = ARDUINOJSON_STRING_BUFFER_SIZE;
 public:
  explicit Writer(::String& str) : _destination(&str) {
    _size = 0;
  }
  ~Writer() {
    flush();
  }
  size_t write(uint8_t c) {
    if (_size + 1 >= bufferCapacity)
      if (flush() != 0)
        return 0;
    _buffer[_size++] = static_cast<char>(c);
    return 1;
  }
  size_t write(const uint8_t* s, size_t n) {
    for (size_t i = 0; i < n; i++) {
      write(s[i]);
    }
    return n;
  }
  size_t flush() {
    ARDUINOJSON_ASSERT(_size < bufferCapacity);
    _buffer[_size] = 0;
    if (_destination->concat(_buffer))
      _size = 0;
    return _size;
  }
 private:
  ::String* _destination;
  char _buffer[bufferCapacity];
  size_t _size;
};
}  // namespace ARDUINOJSON_NAMESPACE
#endif
#if ARDUINOJSON_ENABLE_STD_STREAM
namespace ARDUINOJSON_NAMESPACE {
template <typename TDestination>
class Writer<
    TDestination,
    typename enable_if<is_base_of<std::ostream, TDestination>::value>::type> {
 public:
  explicit Writer(std::ostream& os) : _os(&os) {}
  size_t write(uint8_t c) {
    _os->put(static_cast<char>(c));
    return 1;
  }
  size_t write(const uint8_t* s, size_t n) {
    _os->write(reinterpret_cast<const char*>(s),
               static_cast<std::streamsize>(n));
    return n;
  }
 private:
  std::ostream* _os;
};
}  // namespace ARDUINOJSON_NAMESPACE
#endif
#if ARDUINOJSON_ENABLE_ARDUINO_PRINT
namespace ARDUINOJSON_NAMESPACE {
template <typename TDestination>
class Writer<
    TDestination,
    typename enable_if<is_base_of< ::Print, TDestination>::value>::type> {
 public:
  explicit Writer(::Print& print) : _print(&print) {}
  size_t write(uint8_t c) {
    return _print->write(c);
  }
  size_t write(const uint8_t* s, size_t n) {
    return _print->write(s, n);
  }
 private:
  ::Print* _print;
};
}  // namespace ARDUINOJSON_NAMESPACE
#endif
namespace ARDUINOJSON_NAMESPACE {
template <template <typename> class TSerializer, typename TWriter>
size_t doSerialize(JsonVariantConst source, TWriter writer) {
  TSerializer<TWriter> serializer(writer);
  return variantAccept(VariantAttorney::getData(source), serializer);
}
template <template <typename> class TSerializer, typename TDestination>
size_t serialize(JsonVariantConst source, TDestination& destination) {
  Writer<TDestination> writer(destination);
  return doSerialize<TSerializer>(source, writer);
}
template <template <typename> class TSerializer>
typename enable_if<!TSerializer<StaticStringWriter>::producesText, size_t>::type
serialize(JsonVariantConst source, void* buffer, size_t bufferSize) {
  StaticStringWriter writer(reinterpret_cast<char*>(buffer), bufferSize);
  return doSerialize<TSerializer>(source, writer);
}
template <template <typename> class TSerializer>
typename enable_if<TSerializer<StaticStringWriter>::producesText, size_t>::type
serialize(JsonVariantConst source, void* buffer, size_t bufferSize) {
  StaticStringWriter writer(reinterpret_cast<char*>(buffer), bufferSize);
  size_t n = doSerialize<TSerializer>(source, writer);
  if (n < bufferSize)
    reinterpret_cast<char*>(buffer)[n] = 0;
  return n;
}
template <template <typename> class TSerializer, typename TChar, size_t N>
typename enable_if<IsChar<TChar>::value, size_t>::type serialize(
    JsonVariantConst source, TChar (&buffer)[N]) {
  return serialize<TSerializer>(source, buffer, N);
}
template <typename TWriter>
class JsonSerializer : public Visitor<size_t> {
 public:
  static const bool producesText = true;
  JsonSerializer(TWriter writer) : _formatter(writer) {}
  FORCE_INLINE size_t visitArray(const CollectionData& array) {
    write('[');
    const VariantSlot* slot = array.head();
    while (slot != 0) {
      slot->data()->accept(*this);
      slot = slot->next();
      if (slot == 0)
        break;
      write(',');
    }
    write(']');
    return bytesWritten();
  }
  size_t visitObject(const CollectionData& object) {
    write('{');
    const VariantSlot* slot = object.head();
    while (slot != 0) {
      _formatter.writeString(slot->key());
      write(':');
      slot->data()->accept(*this);
      slot = slot->next();
      if (slot == 0)
        break;
      write(',');
    }
    write('}');
    return bytesWritten();
  }
  size_t visitFloat(JsonFloat value) {
    _formatter.writeFloat(value);
    return bytesWritten();
  }
  size_t visitString(const char* value) {
    _formatter.writeString(value);
    return bytesWritten();
  }
  size_t visitString(const char* value, size_t n) {
    _formatter.writeString(value, n);
    return bytesWritten();
  }
  size_t visitRawJson(const char* data, size_t n) {
    _formatter.writeRaw(data, n);
    return bytesWritten();
  }
  size_t visitSignedInteger(JsonInteger value) {
    _formatter.writeInteger(value);
    return bytesWritten();
  }
  size_t visitUnsignedInteger(JsonUInt value) {
    _formatter.writeInteger(value);
    return bytesWritten();
  }
  size_t visitBoolean(bool value) {
    _formatter.writeBoolean(value);
    return bytesWritten();
  }
  size_t visitNull() {
    _formatter.writeRaw("null");
    return bytesWritten();
  }
 protected:
  size_t bytesWritten() const {
    return _formatter.bytesWritten();
  }
  void write(char c) {
    _formatter.writeRaw(c);
  }
  void write(const char* s) {
    _formatter.writeRaw(s);
  }
 private:
  TextFormatter<TWriter> _formatter;
};
template <typename TDestination>
size_t serializeJson(JsonVariantConst source, TDestination& destination) {
  return serialize<JsonSerializer>(source, destination);
}
inline size_t serializeJson(JsonVariantConst source, void* buffer,
                            size_t bufferSize) {
  return serialize<JsonSerializer>(source, buffer, bufferSize);
}
inline size_t measureJson(JsonVariantConst source) {
  return measure<JsonSerializer>(source);
}
#if ARDUINOJSON_ENABLE_STD_STREAM
template <typename T>
inline typename enable_if<is_convertible<T, JsonVariantConst>::value,
                          std::ostream&>::type
operator<<(std::ostream& os, const T& source) {
  serializeJson(source, os);
  return os;
}
#endif
template <typename T, typename Enable>
struct Converter {
  static void toJson(const T& src, JsonVariant dst) {
    convertToJson(src, dst); // Error here? See https://arduinojson.org/v6/unsupported-set/
  }
  static T fromJson(JsonVariantConst src) {
    T result; // Error here? See https://arduinojson.org/v6/non-default-constructible/
    convertFromJson(src, result);  // Error here? See https://arduinojson.org/v6/unsupported-as/
    return result;
  }
  static bool checkJson(JsonVariantConst src) {
    T dummy = T();
    return canConvertFromJson(src, dummy);  // Error here? See https://arduinojson.org/v6/unsupported-is/
  }
};
template <typename T>
struct Converter<
    T, typename enable_if<is_integral<T>::value && !is_same<bool, T>::value &&
                          !is_same<char, T>::value>::type>
    : private VariantAttorney {
  static void toJson(T src, JsonVariant dst) {
    VariantData* data = getData(dst);
    ARDUINOJSON_ASSERT_INTEGER_TYPE_IS_SUPPORTED(T);
    if (data)
      data->setInteger(src);
  }
  static T fromJson(JsonVariantConst src) {
    ARDUINOJSON_ASSERT_INTEGER_TYPE_IS_SUPPORTED(T);
    const VariantData* data = getData(src);
    return data ? data->asIntegral<T>() : T();
  }
  static bool checkJson(JsonVariantConst src) {
    const VariantData* data = getData(src);
    return data && data->isInteger<T>();
  }
};
template <typename T>
struct Converter<T, typename enable_if<is_enum<T>::value>::type>
    : private VariantAttorney {
  static void toJson(T src, JsonVariant dst) {
    dst.set(static_cast<JsonInteger>(src));
  }
  static T fromJson(JsonVariantConst src) {
    const VariantData* data = getData(src);
    return data ? static_cast<T>(data->asIntegral<int>()) : T();
  }
  static bool checkJson(JsonVariantConst src) {
    const VariantData* data = getData(src);
    return data && data->isInteger<int>();
  }
};
template <>
struct Converter<bool> : private VariantAttorney {
  static void toJson(bool src, JsonVariant dst) {
    VariantData* data = getData(dst);
    if (data)
      data->setBoolean(src);
  }
  static bool fromJson(JsonVariantConst src) {
    const VariantData* data = getData(src);
    return data ? data->asBoolean() : false;
  }
  static bool checkJson(JsonVariantConst src) {
    const VariantData* data = getData(src);
    return data && data->isBoolean();
  }
};
template <typename T>
struct Converter<T, typename enable_if<is_floating_point<T>::value>::type>
    : private VariantAttorney {
  static void toJson(T src, JsonVariant dst) {
    VariantData* data = getData(dst);
    if (data)
      data->setFloat(static_cast<JsonFloat>(src));
  }
  static T fromJson(JsonVariantConst src) {
    const VariantData* data = getData(src);
    return data ? data->asFloat<T>() : 0;
  }
  static bool checkJson(JsonVariantConst src) {
    const VariantData* data = getData(src);
    return data && data->isFloat();
  }
};
template <>
struct Converter<const char*> : private VariantAttorney {
  static void toJson(const char* src, JsonVariant dst) {
    variantSetString(getData(dst), adaptString(src), getPool(dst));
  }
  static const char* fromJson(JsonVariantConst src) {
    const VariantData* data = getData(src);
    return data ? data->asString().c_str() : 0;
  }
  static bool checkJson(JsonVariantConst src) {
    const VariantData* data = getData(src);
    return data && data->isString();
  }
};
template <>
struct Converter<JsonString> : private VariantAttorney {
  static void toJson(JsonString src, JsonVariant dst) {
    variantSetString(getData(dst), adaptString(src), getPool(dst));
  }
  static JsonString fromJson(JsonVariantConst src) {
    const VariantData* data = getData(src);
    return data ? data->asString() : 0;
  }
  static bool checkJson(JsonVariantConst src) {
    const VariantData* data = getData(src);
    return data && data->isString();
  }
};
template <typename T>
inline typename enable_if<IsString<T>::value, bool>::type convertToJson(
    const T& src, JsonVariant dst) {
  VariantData* data = VariantAttorney::getData(dst);
  MemoryPool* pool = VariantAttorney::getPool(dst);
  return variantSetString(data, adaptString(src), pool);
}
template <>
struct Converter<SerializedValue<const char*> > {
  static void toJson(SerializedValue<const char*> src, JsonVariant dst) {
    VariantData* data = VariantAttorney::getData(dst);
    if (data)
      data->setLinkedRaw(src);
  }
};
template <typename T>
struct Converter<SerializedValue<T>,
                 typename enable_if<!is_same<const char*, T>::value>::type>
    : private VariantAttorney {
  static void toJson(SerializedValue<T> src, JsonVariant dst) {
    VariantData* data = getData(dst);
    MemoryPool* pool = getPool(dst);
    if (data)
      data->storeOwnedRaw(src, pool);
  }
};
#if ARDUINOJSON_HAS_NULLPTR
template <>
struct Converter<decltype(nullptr)> : private VariantAttorney {
  static void toJson(decltype(nullptr), JsonVariant dst) {
    variantSetNull(getData(dst));
  }
  static decltype(nullptr) fromJson(JsonVariantConst) {
    return nullptr;
  }
  static bool checkJson(JsonVariantConst src) {
    const VariantData* data = getData(src);
    return data == 0 || data->isNull();
  }
};
#endif
#if ARDUINOJSON_ENABLE_ARDUINO_STREAM
class MemoryPoolPrint : public Print {
 public:
  MemoryPoolPrint(MemoryPool* pool) : _pool(pool), _size(0) {
    pool->getFreeZone(&_string, &_capacity);
  }
  JsonString str() {
    ARDUINOJSON_ASSERT(_size < _capacity);
    return JsonString(_pool->saveStringFromFreeZone(_size), _size,
                      JsonString::Copied);
  }
  size_t write(uint8_t c) {
    if (_size >= _capacity)
      return 0;
    _string[_size++] = char(c);
    return 1;
  }
  size_t write(const uint8_t* buffer, size_t size) {
    if (_size + size >= _capacity) {
      _size = _capacity;  // mark as overflowed
      return 0;
    }
    memcpy(&_string[_size], buffer, size);
    _size += size;
    return size;
  }
  bool overflowed() const {
    return _size >= _capacity;
  }
 private:
  MemoryPool* _pool;
  size_t _size;
  char* _string;
  size_t _capacity;
};
inline void convertToJson(const ::Printable& src, JsonVariant dst) {
  MemoryPool* pool = VariantAttorney::getPool(dst);
  VariantData* data = VariantAttorney::getData(dst);
  if (!pool || !data)
    return;
  MemoryPoolPrint print(pool);
  src.printTo(print);
  if (print.overflowed()) {
    pool->markAsOverflowed();
    data->setNull();
    return;
  }
  data->setString(print.str());
}
#endif
#if ARDUINOJSON_ENABLE_ARDUINO_STRING
inline void convertFromJson(JsonVariantConst src, ::String& dst) {
  JsonString str = src.as<JsonString>();
  if (str)
    dst = str.c_str();
  else
    serializeJson(src, dst);
}
inline bool canConvertFromJson(JsonVariantConst src, const ::String&) {
  return src.is<JsonString>();
}
#endif
#if ARDUINOJSON_ENABLE_STD_STRING
inline void convertFromJson(JsonVariantConst src, std::string& dst) {
  JsonString str = src.as<JsonString>();
  if (str)
    dst.assign(str.c_str(), str.size());
  else
    serializeJson(src, dst);
}
inline bool canConvertFromJson(JsonVariantConst src, const std::string&) {
  return src.is<JsonString>();
}
#endif
#if ARDUINOJSON_ENABLE_STRING_VIEW
inline void convertFromJson(JsonVariantConst src, std::string_view& dst) {
  JsonString str = src.as<JsonString>();
  if (str)  // the standard doesn't allow passing null to the constructor
    dst = std::string_view(str.c_str(), str.size());
}
inline bool canConvertFromJson(JsonVariantConst src, const std::string_view&) {
  return src.is<JsonString>();
}
#endif
template <typename T>
struct ConverterNeedsWriteableRef {
 protected:  // <- to avoid GCC's "all member functions in class are private"
  static int probe(T (*f)(JsonVariant));
  static char probe(T (*f)(JsonVariantConst));
 public:
  static const bool value =
      sizeof(probe(Converter<T>::fromJson)) == sizeof(int);
};
class CollectionData;
struct ComparerBase : Visitor<CompareResult> {};
template <typename T, typename Enable = void>
struct Comparer;
template <typename T>
struct Comparer<T, typename enable_if<IsString<T>::value>::type>
    : ComparerBase {
  T rhs;  // TODO: store adapted string?
  explicit Comparer(T value) : rhs(value) {}
  CompareResult visitString(const char* lhs, size_t n) {
    int i = stringCompare(adaptString(rhs), adaptString(lhs, n));
    if (i < 0)
      return COMPARE_RESULT_GREATER;
    else if (i > 0)
      return COMPARE_RESULT_LESS;
    else
      return COMPARE_RESULT_EQUAL;
  }
  CompareResult visitNull() {
    if (adaptString(rhs).isNull())
      return COMPARE_RESULT_EQUAL;
    else
      return COMPARE_RESULT_DIFFER;
  }
};
template <typename T>
struct Comparer<T, typename enable_if<is_integral<T>::value ||
                                      is_floating_point<T>::value>::type>
    : ComparerBase {
  T rhs;
  explicit Comparer(T value) : rhs(value) {}
  CompareResult visitFloat(JsonFloat lhs) {
    return arithmeticCompare(lhs, rhs);
  }
  CompareResult visitSignedInteger(JsonInteger lhs) {
    return arithmeticCompare(lhs, rhs);
  }
  CompareResult visitUnsignedInteger(JsonUInt lhs) {
    return arithmeticCompare(lhs, rhs);
  }
  CompareResult visitBoolean(bool lhs) {
    return visitUnsignedInteger(static_cast<JsonUInt>(lhs));
  }
};
struct NullComparer : ComparerBase {
  CompareResult visitNull() {
    return COMPARE_RESULT_EQUAL;
  }
};
#if ARDUINOJSON_HAS_NULLPTR
template <>
struct Comparer<decltype(nullptr), void> : NullComparer {
  explicit Comparer(decltype(nullptr)) : NullComparer() {}
};
#endif
struct ArrayComparer : ComparerBase {
  const CollectionData* _rhs;
  explicit ArrayComparer(const CollectionData& rhs) : _rhs(&rhs) {}
  CompareResult visitArray(const CollectionData& lhs) {
    if (JsonArrayConst(&lhs) == JsonArrayConst(_rhs))
      return COMPARE_RESULT_EQUAL;
    else
      return COMPARE_RESULT_DIFFER;
  }
};
struct ObjectComparer : ComparerBase {
  const CollectionData* _rhs;
  explicit ObjectComparer(const CollectionData& rhs) : _rhs(&rhs) {}
  CompareResult visitObject(const CollectionData& lhs) {
    if (JsonObjectConst(&lhs) == JsonObjectConst(_rhs))
      return COMPARE_RESULT_EQUAL;
    else
      return COMPARE_RESULT_DIFFER;
  }
};
struct RawComparer : ComparerBase {
  const char* _rhsData;
  size_t _rhsSize;
  explicit RawComparer(const char* rhsData, size_t rhsSize)
      : _rhsData(rhsData), _rhsSize(rhsSize) {}
  CompareResult visitRawJson(const char* lhsData, size_t lhsSize) {
    size_t size = _rhsSize < lhsSize ? _rhsSize : lhsSize;
    int n = memcmp(lhsData, _rhsData, size);
    if (n < 0)
      return COMPARE_RESULT_LESS;
    else if (n > 0)
      return COMPARE_RESULT_GREATER;
    else
      return COMPARE_RESULT_EQUAL;
  }
};
struct VariantComparer : ComparerBase {
  const VariantData* rhs;
  explicit VariantComparer(const VariantData* value) : rhs(value) {}
  CompareResult visitArray(const CollectionData& lhs) {
    ArrayComparer comparer(lhs);
    return accept(comparer);
  }
  CompareResult visitObject(const CollectionData& lhs) {
    ObjectComparer comparer(lhs);
    return accept(comparer);
  }
  CompareResult visitFloat(JsonFloat lhs) {
    Comparer<JsonFloat> comparer(lhs);
    return accept(comparer);
  }
  CompareResult visitString(const char* lhs, size_t) {
    Comparer<const char*> comparer(lhs);
    return accept(comparer);
  }
  CompareResult visitRawJson(const char* lhsData, size_t lhsSize) {
    RawComparer comparer(lhsData, lhsSize);
    return accept(comparer);
  }
  CompareResult visitSignedInteger(JsonInteger lhs) {
    Comparer<JsonInteger> comparer(lhs);
    return accept(comparer);
  }
  CompareResult visitUnsignedInteger(JsonUInt lhs) {
    Comparer<JsonUInt> comparer(lhs);
    return accept(comparer);
  }
  CompareResult visitBoolean(bool lhs) {
    Comparer<bool> comparer(lhs);
    return accept(comparer);
  }
  CompareResult visitNull() {
    NullComparer comparer;
    return accept(comparer);
  }
 private:
  template <typename TComparer>
  CompareResult accept(TComparer& comparer) {
    CompareResult reversedResult = variantAccept(rhs, comparer);
    switch (reversedResult) {
      case COMPARE_RESULT_GREATER:
        return COMPARE_RESULT_LESS;
      case COMPARE_RESULT_LESS:
        return COMPARE_RESULT_GREATER;
      default:
        return reversedResult;
    }
  }
};
template <typename T>
struct Comparer<
    T, typename enable_if<is_convertible<T, JsonVariantConst>::value>::type>
    : VariantComparer {
  explicit Comparer(const T& value)
      : VariantComparer(VariantAttorney::getData(value)) {}
};
template <typename T>
CompareResult compare(JsonVariantConst lhs, const T& rhs) {
  Comparer<T> comparer(rhs);
  return variantAccept(VariantAttorney::getData(lhs), comparer);
}
#ifndef isdigit
inline bool isdigit(char c) {
  return '0' <= c && c <= '9';
}
#endif
inline bool issign(char c) {
  return '-' == c || c == '+';
}
template <typename A, typename B>
struct choose_largest : conditional<(sizeof(A) > sizeof(B)), A, B> {};
inline bool parseNumber(const char* s, VariantData& result) {
  typedef FloatTraits<JsonFloat> traits;
  typedef choose_largest<traits::mantissa_type, JsonUInt>::type mantissa_t;
  typedef traits::exponent_type exponent_t;
  ARDUINOJSON_ASSERT(s != 0);
  bool is_negative = false;
  switch (*s) {
    case '-':
      is_negative = true;
      s++;
      break;
    case '+':
      s++;
      break;
  }
#if ARDUINOJSON_ENABLE_NAN
  if (*s == 'n' || *s == 'N') {
    result.setFloat(traits::nan());
    return true;
  }
#endif
#if ARDUINOJSON_ENABLE_INFINITY
  if (*s == 'i' || *s == 'I') {
    result.setFloat(is_negative ? -traits::inf() : traits::inf());
    return true;
  }
#endif
  if (!isdigit(*s) && *s != '.')
    return false;
  mantissa_t mantissa = 0;
  exponent_t exponent_offset = 0;
  const mantissa_t maxUint = JsonUInt(-1);
  while (isdigit(*s)) {
    uint8_t digit = uint8_t(*s - '0');
    if (mantissa > maxUint / 10)
      break;
    mantissa *= 10;
    if (mantissa > maxUint - digit)
      break;
    mantissa += digit;
    s++;
  }
  if (*s == '\0') {
    if (is_negative) {
      const mantissa_t sintMantissaMax = mantissa_t(1)
                                         << (sizeof(JsonInteger) * 8 - 1);
      if (mantissa <= sintMantissaMax) {
        result.setInteger(JsonInteger(~mantissa + 1));
        return true;
      }
    } else {
      result.setInteger(JsonUInt(mantissa));
      return true;
    }
  }
  while (mantissa > traits::mantissa_max) {
    mantissa /= 10;
    exponent_offset++;
  }
  while (isdigit(*s)) {
    exponent_offset++;
    s++;
  }
  if (*s == '.') {
    s++;
    while (isdigit(*s)) {
      if (mantissa < traits::mantissa_max / 10) {
        mantissa = mantissa * 10 + uint8_t(*s - '0');
        exponent_offset--;
      }
      s++;
    }
  }
  int exponent = 0;
  if (*s == 'e' || *s == 'E') {
    s++;
    bool negative_exponent = false;
    if (*s == '-') {
      negative_exponent = true;
      s++;
    } else if (*s == '+') {
      s++;
    }
    while (isdigit(*s)) {
      exponent = exponent * 10 + (*s - '0');
      if (exponent + exponent_offset > traits::exponent_max) {
        if (negative_exponent)
          result.setFloat(is_negative ? -0.0f : 0.0f);
        else
          result.setFloat(is_negative ? -traits::inf() : traits::inf());
        return true;
      }
      s++;
    }
    if (negative_exponent)
      exponent = -exponent;
  }
  exponent += exponent_offset;
  if (*s != '\0')
    return false;
  JsonFloat final_result =
      traits::make_float(static_cast<JsonFloat>(mantissa), exponent);
  result.setFloat(is_negative ? -final_result : final_result);
  return true;
}
template <typename T>
inline T parseNumber(const char* s) {
  VariantData value;
  value.init();  // VariantData is a POD, so it has no constructor
  parseNumber(s, value);
  return Converter<T>::fromJson(JsonVariantConst(&value));
}
template <typename T>
inline T VariantData::asIntegral() const {
  switch (type()) {
    case VALUE_IS_BOOLEAN:
      return _content.asBoolean;
    case VALUE_IS_UNSIGNED_INTEGER:
      return convertNumber<T>(_content.asUnsignedInteger);
    case VALUE_IS_SIGNED_INTEGER:
      return convertNumber<T>(_content.asSignedInteger);
    case VALUE_IS_LINKED_STRING:
    case VALUE_IS_OWNED_STRING:
      return parseNumber<T>(_content.asString.data);
    case VALUE_IS_FLOAT:
      return convertNumber<T>(_content.asFloat);
    default:
      return 0;
  }
}
inline bool VariantData::asBoolean() const {
  switch (type()) {
    case VALUE_IS_BOOLEAN:
      return _content.asBoolean;
    case VALUE_IS_SIGNED_INTEGER:
    case VALUE_IS_UNSIGNED_INTEGER:
      return _content.asUnsignedInteger != 0;
    case VALUE_IS_FLOAT:
      return _content.asFloat != 0;
    case VALUE_IS_NULL:
      return false;
    default:
      return true;
  }
}
template <typename T>
inline T VariantData::asFloat() const {
  switch (type()) {
    case VALUE_IS_BOOLEAN:
      return static_cast<T>(_content.asBoolean);
    case VALUE_IS_UNSIGNED_INTEGER:
      return static_cast<T>(_content.asUnsignedInteger);
    case VALUE_IS_SIGNED_INTEGER:
      return static_cast<T>(_content.asSignedInteger);
    case VALUE_IS_LINKED_STRING:
    case VALUE_IS_OWNED_STRING:
      return parseNumber<T>(_content.asString.data);
    case VALUE_IS_FLOAT:
      return static_cast<T>(_content.asFloat);
    default:
      return 0;
  }
}
inline JsonString VariantData::asString() const {
  switch (type()) {
    case VALUE_IS_LINKED_STRING:
      return JsonString(_content.asString.data, _content.asString.size,
                        JsonString::Linked);
    case VALUE_IS_OWNED_STRING:
      return JsonString(_content.asString.data, _content.asString.size,
                        JsonString::Copied);
    default:
      return JsonString();
  }
}
inline bool VariantData::copyFrom(const VariantData& src, MemoryPool* pool) {
  switch (src.type()) {
    case VALUE_IS_ARRAY:
      return toArray().copyFrom(src._content.asCollection, pool);
    case VALUE_IS_OBJECT:
      return toObject().copyFrom(src._content.asCollection, pool);
    case VALUE_IS_OWNED_STRING: {
      JsonString value = src.asString();
      return setString(adaptString(value), pool);
    }
    case VALUE_IS_OWNED_RAW:
      return storeOwnedRaw(
          serialized(src._content.asString.data, src._content.asString.size),
          pool);
    default:
      setType(src.type());
      _content = src._content;
      return true;
  }
}
template <typename TDerived>
inline JsonVariant VariantRefBase<TDerived>::add() const {
  return JsonVariant(getPool(),
                     variantAddElement(getOrCreateData(), getPool()));
}
template <typename TDerived>
inline JsonVariant VariantRefBase<TDerived>::getVariant() const {
  return JsonVariant(getPool(), getData());
}
template <typename TDerived>
inline JsonVariant VariantRefBase<TDerived>::getOrCreateVariant() const {
  return JsonVariant(getPool(), getOrCreateData());
}
template <typename TDerived>
template <typename T>
inline typename enable_if<is_same<T, JsonArray>::value, JsonArray>::type
VariantRefBase<TDerived>::to() const {
  return JsonArray(getPool(), variantToArray(getOrCreateData()));
}
template <typename TDerived>
template <typename T>
typename enable_if<is_same<T, JsonObject>::value, JsonObject>::type
VariantRefBase<TDerived>::to() const {
  return JsonObject(getPool(), variantToObject(getOrCreateData()));
}
template <typename TDerived>
template <typename T>
typename enable_if<is_same<T, JsonVariant>::value, JsonVariant>::type
VariantRefBase<TDerived>::to() const {
  variantSetNull(getOrCreateData());
  return *this;
}
template <typename TDerived>
inline void convertToJson(const VariantRefBase<TDerived>& src,
                          JsonVariant dst) {
  dst.set(src.template as<JsonVariantConst>());
}
}  // namespace ARDUINOJSON_NAMESPACE

#if ARDUINOJSON_ENABLE_STD_STREAM
#endif
namespace ARDUINOJSON_NAMESPACE {
class DeserializationError : public SafeBoolIdom<DeserializationError> {
 public:
  enum Code {
    Ok,
    EmptyInput,
    IncompleteInput,
    InvalidInput,
    NoMemory,
    TooDeep
  };
  DeserializationError() {}
  DeserializationError(Code c) : _code(c) {}
  friend bool operator==(const DeserializationError& lhs,
                         const DeserializationError& rhs) {
    return lhs._code == rhs._code;
  }
  friend bool operator!=(const DeserializationError& lhs,
                         const DeserializationError& rhs) {
    return lhs._code != rhs._code;
  }
  friend bool operator==(const DeserializationError& lhs, Code rhs) {
    return lhs._code == rhs;
  }
  friend bool operator==(Code lhs, const DeserializationError& rhs) {
    return lhs == rhs._code;
  }
  friend bool operator!=(const DeserializationError& lhs, Code rhs) {
    return lhs._code != rhs;
  }
  friend bool operator!=(Code lhs, const DeserializationError& rhs) {
    return lhs != rhs._code;
  }
  operator bool_type() const {
    return _code != Ok ? safe_true() : safe_false();
  }
  Code code() const {
    return _code;
  }
  const char* c_str() const {
    static const char* messages[] = {
        "Ok",           "EmptyInput", "IncompleteInput",
        "InvalidInput", "NoMemory",   "TooDeep"};
    ARDUINOJSON_ASSERT(static_cast<size_t>(_code) <
                       sizeof(messages) / sizeof(messages[0]));
    return messages[_code];
  }
#if ARDUINOJSON_ENABLE_PROGMEM
  const __FlashStringHelper* f_str() const {
    ARDUINOJSON_DEFINE_PROGMEM_ARRAY(char, s0, "Ok");
    ARDUINOJSON_DEFINE_PROGMEM_ARRAY(char, s1, "EmptyInput");
    ARDUINOJSON_DEFINE_PROGMEM_ARRAY(char, s2, "IncompleteInput");
    ARDUINOJSON_DEFINE_PROGMEM_ARRAY(char, s3, "InvalidInput");
    ARDUINOJSON_DEFINE_PROGMEM_ARRAY(char, s4, "NoMemory");
    ARDUINOJSON_DEFINE_PROGMEM_ARRAY(char, s5, "TooDeep");
    ARDUINOJSON_DEFINE_PROGMEM_ARRAY(
        const char*, messages, ARDUINOJSON_EXPAND6({s0, s1, s2, s3, s4, s5}));
    return reinterpret_cast<const __FlashStringHelper*>(
        pgm_read(messages + _code));
  }
#endif
 private:
  Code _code;
};
#if ARDUINOJSON_ENABLE_STD_STREAM
inline std::ostream& operator<<(std::ostream& s,
                                const DeserializationError& e) {
  s << e.c_str();
  return s;
}
inline std::ostream& operator<<(std::ostream& s, DeserializationError::Code c) {
  s << DeserializationError(c).c_str();
  return s;
}
#endif
class Filter {
 public:
  explicit Filter(JsonVariantConst v) : _variant(v) {}
  bool allow() const {
    return _variant;
  }
  bool allowArray() const {
    return _variant == true || _variant.is<JsonArrayConst>();
  }
  bool allowObject() const {
    return _variant == true || _variant.is<JsonObjectConst>();
  }
  bool allowValue() const {
    return _variant == true;
  }
  template <typename TKey>
  Filter operator[](const TKey& key) const {
    if (_variant == true)  // "true" means "allow recursively"
      return *this;
    JsonVariantConst member = _variant[key];
    return Filter(member.isNull() ? _variant["*"] : member);
  }
 private:
  JsonVariantConst _variant;
};
struct AllowAllFilter {
  bool allow() const {
    return true;
  }
  bool allowArray() const {
    return true;
  }
  bool allowObject() const {
    return true;
  }
  bool allowValue() const {
    return true;
  }
  template <typename TKey>
  AllowAllFilter operator[](const TKey&) const {
    return AllowAllFilter();
  }
};
class NestingLimit {
 public:
  NestingLimit() : _value(ARDUINOJSON_DEFAULT_NESTING_LIMIT) {}
  explicit NestingLimit(uint8_t n) : _value(n) {}
  NestingLimit decrement() const {
    ARDUINOJSON_ASSERT(_value > 0);
    return NestingLimit(static_cast<uint8_t>(_value - 1));
  }
  bool reached() const {
    return _value == 0;
  }
 private:
  uint8_t _value;
};
template <typename TSource, typename Enable = void>
struct Reader {
 public:
  Reader(TSource& source) : _source(&source) {}
  int read() {
    return _source->read();  // Error here? You passed an unsupported input type
  }
  size_t readBytes(char* buffer, size_t length) {
    return _source->readBytes(buffer, length);
  }
 private:
  TSource* _source;
};
template <typename TSource, typename Enable = void>
struct BoundedReader {
};
template <typename TIterator>
class IteratorReader {
  TIterator _ptr, _end;
 public:
  explicit IteratorReader(TIterator begin, TIterator end)
      : _ptr(begin), _end(end) {}
  int read() {
    if (_ptr < _end)
      return static_cast<unsigned char>(*_ptr++);
    else
      return -1;
  }
  size_t readBytes(char* buffer, size_t length) {
    size_t i = 0;
    while (i < length && _ptr < _end)
      buffer[i++] = *_ptr++;
    return i;
  }
};
template <typename T>
struct void_ {
  typedef void type;
};
template <typename TSource>
struct Reader<TSource, typename void_<typename TSource::const_iterator>::type>
    : IteratorReader<typename TSource::const_iterator> {
  explicit Reader(const TSource& source)
      : IteratorReader<typename TSource::const_iterator>(source.begin(),
                                                         source.end()) {}
};
template <typename T>
struct IsCharOrVoid {
  static const bool value =
      is_same<T, void>::value || is_same<T, char>::value ||
      is_same<T, unsigned char>::value || is_same<T, signed char>::value;
};
template <typename T>
struct IsCharOrVoid<const T> : IsCharOrVoid<T> {};
template <typename TSource>
struct Reader<TSource*,
              typename enable_if<IsCharOrVoid<TSource>::value>::type> {
  const char* _ptr;
 public:
  explicit Reader(const void* ptr)
      : _ptr(ptr ? reinterpret_cast<const char*>(ptr) : "") {}
  int read() {
    return static_cast<unsigned char>(*_ptr++);
  }
  size_t readBytes(char* buffer, size_t length) {
    for (size_t i = 0; i < length; i++)
      buffer[i] = *_ptr++;
    return length;
  }
};
template <typename TSource>
struct BoundedReader<TSource*,
                     typename enable_if<IsCharOrVoid<TSource>::value>::type>
    : public IteratorReader<const char*> {
 public:
  explicit BoundedReader(const void* ptr, size_t len)
      : IteratorReader<const char*>(reinterpret_cast<const char*>(ptr),
                                    reinterpret_cast<const char*>(ptr) + len) {}
};
template <typename TVariant>
struct Reader<TVariant, typename enable_if<IsVariant<TVariant>::value>::type>
    : Reader<char*, void> {
  explicit Reader(const TVariant& x)
      : Reader<char*, void>(x.template as<const char*>()) {}
};
}  // namespace ARDUINOJSON_NAMESPACE
#if ARDUINOJSON_ENABLE_ARDUINO_STREAM
namespace ARDUINOJSON_NAMESPACE {
template <typename TSource>
struct Reader<TSource,
              typename enable_if<is_base_of<Stream, TSource>::value>::type> {
 public:
  explicit Reader(Stream& stream) : _stream(&stream) {}
  int read() {
    char c;
    return _stream->readBytes(&c, 1) ? static_cast<unsigned char>(c) : -1;
  }
  size_t readBytes(char* buffer, size_t length) {
    return _stream->readBytes(buffer, length);
  }
 private:
  Stream* _stream;
};
}  // namespace ARDUINOJSON_NAMESPACE
#endif
#if ARDUINOJSON_ENABLE_ARDUINO_STRING
namespace ARDUINOJSON_NAMESPACE {
template <typename TSource>
struct Reader<TSource,
              typename enable_if<is_base_of< ::String, TSource>::value>::type>
    : BoundedReader<const char*> {
  explicit Reader(const ::String& s)
      : BoundedReader<const char*>(s.c_str(), s.length()) {}
};
}  // namespace ARDUINOJSON_NAMESPACE
#endif
#if ARDUINOJSON_ENABLE_PROGMEM
namespace ARDUINOJSON_NAMESPACE {
template <>
struct Reader<const __FlashStringHelper*, void> {
  const char* _ptr;
 public:
  explicit Reader(const __FlashStringHelper* ptr)
      : _ptr(reinterpret_cast<const char*>(ptr)) {}
  int read() {
    return pgm_read_byte(_ptr++);
  }
  size_t readBytes(char* buffer, size_t length) {
    memcpy_P(buffer, _ptr, length);
    _ptr += length;
    return length;
  }
};
template <>
struct BoundedReader<const __FlashStringHelper*, void> {
  const char* _ptr;
  const char* _end;
 public:
  explicit BoundedReader(const __FlashStringHelper* ptr, size_t size)
      : _ptr(reinterpret_cast<const char*>(ptr)), _end(_ptr + size) {}
  int read() {
    if (_ptr < _end)
      return pgm_read_byte(_ptr++);
    else
      return -1;
  }
  size_t readBytes(char* buffer, size_t length) {
    size_t available = static_cast<size_t>(_end - _ptr);
    if (available < length)
      length = available;
    memcpy_P(buffer, _ptr, length);
    _ptr += length;
    return length;
  }
};
}  // namespace ARDUINOJSON_NAMESPACE
#endif
#if ARDUINOJSON_ENABLE_STD_STREAM
#include <istream>
namespace ARDUINOJSON_NAMESPACE {
template <typename TSource>
struct Reader<TSource, typename enable_if<
                           is_base_of<std::istream, TSource>::value>::type> {
 public:
  explicit Reader(std::istream& stream) : _stream(&stream) {}
  int read() {
    return _stream->get();
  }
  size_t readBytes(char* buffer, size_t length) {
    _stream->read(buffer, static_cast<std::streamsize>(length));
    return static_cast<size_t>(_stream->gcount());
  }
 private:
  std::istream* _stream;
};
}  // namespace ARDUINOJSON_NAMESPACE
#endif
namespace ARDUINOJSON_NAMESPACE {
class StringCopier {
 public:
  StringCopier(MemoryPool* pool) : _pool(pool) {}
  void startString() {
    _pool->getFreeZone(&_ptr, &_capacity);
    _size = 0;
    if (_capacity == 0)
      _pool->markAsOverflowed();
  }
  JsonString save() {
    ARDUINOJSON_ASSERT(_ptr);
    ARDUINOJSON_ASSERT(_size < _capacity);  // needs room for the terminator
    return JsonString(_pool->saveStringFromFreeZone(_size), _size,
                      JsonString::Copied);
  }
  void append(const char* s) {
    while (*s)
      append(*s++);
  }
  void append(const char* s, size_t n) {
    while (n-- > 0)
      append(*s++);
  }
  void append(char c) {
    if (_size + 1 < _capacity)
      _ptr[_size++] = c;
    else
      _pool->markAsOverflowed();
  }
  bool isValid() const {
    return !_pool->overflowed();
  }
  size_t size() const {
    return _size;
  }
  JsonString str() const {
    ARDUINOJSON_ASSERT(_ptr);
    ARDUINOJSON_ASSERT(_size < _capacity);
    _ptr[_size] = 0;
    return JsonString(_ptr, _size, JsonString::Copied);
  }
 private:
  MemoryPool* _pool;
  char* _ptr;
  size_t _size, _capacity;
};
class StringMover {
 public:
  StringMover(char* ptr) : _writePtr(ptr) {}
  void startString() {
    _startPtr = _writePtr;
  }
  FORCE_INLINE JsonString save() {
    JsonString s = str();
    _writePtr++;
    return s;
  }
  void append(char c) {
    *_writePtr++ = c;
  }
  bool isValid() const {
    return true;
  }
  JsonString str() const {
    _writePtr[0] = 0;  // terminator
    return JsonString(_startPtr, size(), JsonString::Linked);
  }
  size_t size() const {
    return size_t(_writePtr - _startPtr);
  }
 private:
  char* _writePtr;
  char* _startPtr;
};
template <typename TInput>
StringCopier makeStringStorage(TInput&, MemoryPool* pool) {
  ARDUINOJSON_ASSERT(pool != 0);
  return StringCopier(pool);
}
template <typename TChar>
StringMover makeStringStorage(
    TChar* input, MemoryPool*,
    typename enable_if<!is_const<TChar>::value>::type* = 0) {
  return StringMover(reinterpret_cast<char*>(input));
}
template <template <typename, typename> class TDeserializer, typename TReader,
          typename TWriter>
TDeserializer<TReader, TWriter> makeDeserializer(MemoryPool* pool,
                                                 TReader reader,
                                                 TWriter writer) {
  ARDUINOJSON_ASSERT(pool != 0);
  return TDeserializer<TReader, TWriter>(pool, reader, writer);
}
template <template <typename, typename> class TDeserializer, typename TString,
          typename TFilter>
typename enable_if<!is_array<TString>::value, DeserializationError>::type
deserialize(JsonDocument& doc, const TString& input, NestingLimit nestingLimit,
            TFilter filter) {
  Reader<TString> reader(input);
  VariantData* data = VariantAttorney::getData(doc);
  MemoryPool* pool = VariantAttorney::getPool(doc);
  doc.clear();
  return makeDeserializer<TDeserializer>(pool, reader,
                                         makeStringStorage(input, pool))
      .parse(*data, filter, nestingLimit);
}
template <template <typename, typename> class TDeserializer, typename TChar,
          typename TFilter>
DeserializationError deserialize(JsonDocument& doc, TChar* input,
                                 size_t inputSize, NestingLimit nestingLimit,
                                 TFilter filter) {
  BoundedReader<TChar*> reader(input, inputSize);
  VariantData* data = VariantAttorney::getData(doc);
  MemoryPool* pool = VariantAttorney::getPool(doc);
  doc.clear();
  return makeDeserializer<TDeserializer>(pool, reader,
                                         makeStringStorage(input, pool))
      .parse(*data, filter, nestingLimit);
}
template <template <typename, typename> class TDeserializer, typename TStream,
          typename TFilter>
DeserializationError deserialize(JsonDocument& doc, TStream& input,
                                 NestingLimit nestingLimit, TFilter filter) {
  Reader<TStream> reader(input);
  VariantData* data = VariantAttorney::getData(doc);
  MemoryPool* pool = VariantAttorney::getPool(doc);
  doc.clear();
  return makeDeserializer<TDeserializer>(pool, reader,
                                         makeStringStorage(input, pool))
      .parse(*data, filter, nestingLimit);
}
template <typename TReader>
class Latch {
 public:
  Latch(TReader reader) : _reader(reader), _loaded(false) {
#if ARDUINOJSON_DEBUG
    _ended = false;
#endif
  }
  void clear() {
    _loaded = false;
  }
  int last() const {
    return _current;
  }
  FORCE_INLINE char current() {
    if (!_loaded) {
      load();
    }
    return _current;
  }
 private:
  void load() {
    ARDUINOJSON_ASSERT(!_ended);
    int c = _reader.read();
#if ARDUINOJSON_DEBUG
    if (c <= 0)
      _ended = true;
#endif
    _current = static_cast<char>(c > 0 ? c : 0);
    _loaded = true;
  }
  TReader _reader;
  char _current;  // NOLINT(clang-analyzer-optin.cplusplus.UninitializedObject)
  bool _loaded;
#if ARDUINOJSON_DEBUG
  bool _ended;
#endif
};
}  // namespace ARDUINOJSON_NAMESPACE
#if defined(__GNUC__)
#  if __GNUC__ >= 7
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#  endif
#endif
namespace ARDUINOJSON_NAMESPACE {
namespace Utf16 {
inline bool isHighSurrogate(uint16_t codeunit) {
  return codeunit >= 0xD800 && codeunit < 0xDC00;
}
inline bool isLowSurrogate(uint16_t codeunit) {
  return codeunit >= 0xDC00 && codeunit < 0xE000;
}
class Codepoint {
 public:
  Codepoint() : _highSurrogate(0), _codepoint(0) {}
  bool append(uint16_t codeunit) {
    if (isHighSurrogate(codeunit)) {
      _highSurrogate = codeunit & 0x3FF;
      return false;
    }
    if (isLowSurrogate(codeunit)) {
      _codepoint =
          uint32_t(0x10000 + ((_highSurrogate << 10) | (codeunit & 0x3FF)));
      return true;
    }
    _codepoint = codeunit;
    return true;
  }
  uint32_t value() const {
    return _codepoint;
  }
 private:
  uint16_t _highSurrogate;
  uint32_t _codepoint;
};
}  // namespace Utf16
}  // namespace ARDUINOJSON_NAMESPACE
#if defined(__GNUC__)
#  if __GNUC__ >= 8
#    pragma GCC diagnostic pop
#  endif
#endif
namespace ARDUINOJSON_NAMESPACE {
namespace Utf8 {
template <typename TStringBuilder>
inline void encodeCodepoint(uint32_t codepoint32, TStringBuilder& str) {
  if (codepoint32 < 0x80) {
    str.append(char(codepoint32));
  } else {
    char buf[5];
    char* p = buf;
    *(p++) = 0;
    *(p++) = char((codepoint32 | 0x80) & 0xBF);
    uint16_t codepoint16 = uint16_t(codepoint32 >> 6);
    if (codepoint16 < 0x20) {  // 0x800
      *(p++) = char(codepoint16 | 0xC0);
    } else {
      *(p++) = char((codepoint16 | 0x80) & 0xBF);
      codepoint16 = uint16_t(codepoint16 >> 6);
      if (codepoint16 < 0x10) {  // 0x10000
        *(p++) = char(codepoint16 | 0xE0);
      } else {
        *(p++) = char((codepoint16 | 0x80) & 0xBF);
        codepoint16 = uint16_t(codepoint16 >> 6);
        *(p++) = char(codepoint16 | 0xF0);
      }
    }
    while (*(--p)) {
      str.append(*p);
    }
  }
}
}  // namespace Utf8
template <typename TReader, typename TStringStorage>
class JsonDeserializer {
 public:
  JsonDeserializer(MemoryPool* pool, TReader reader,
                   TStringStorage stringStorage)
      : _stringStorage(stringStorage),
        _foundSomething(false),
        _latch(reader),
        _pool(pool) {}
  template <typename TFilter>
  DeserializationError parse(VariantData& variant, TFilter filter,
                             NestingLimit nestingLimit) {
    DeserializationError::Code err;
    err = parseVariant(variant, filter, nestingLimit);
    if (!err && _latch.last() != 0 && !variant.isEnclosed()) {
      return DeserializationError::InvalidInput;
    }
    return err;
  }
 private:
  char current() {
    return _latch.current();
  }
  void move() {
    _latch.clear();
  }
  bool eat(char charToSkip) {
    if (current() != charToSkip)
      return false;
    move();
    return true;
  }
  template <typename TFilter>
  DeserializationError::Code parseVariant(VariantData& variant, TFilter filter,
                                          NestingLimit nestingLimit) {
    DeserializationError::Code err;
    err = skipSpacesAndComments();
    if (err)
      return err;
    switch (current()) {
      case '[':
        if (filter.allowArray())
          return parseArray(variant.toArray(), filter, nestingLimit);
        else
          return skipArray(nestingLimit);
      case '{':
        if (filter.allowObject())
          return parseObject(variant.toObject(), filter, nestingLimit);
        else
          return skipObject(nestingLimit);
      case '\"':
      case '\'':
        if (filter.allowValue())
          return parseStringValue(variant);
        else
          return skipQuotedString();
      case 't':
        if (filter.allowValue())
          variant.setBoolean(true);
        return skipKeyword("true");
      case 'f':
        if (filter.allowValue())
          variant.setBoolean(false);
        return skipKeyword("false");
      case 'n':
        return skipKeyword("null");
      default:
        if (filter.allowValue())
          return parseNumericValue(variant);
        else
          return skipNumericValue();
    }
  }
  DeserializationError::Code skipVariant(NestingLimit nestingLimit) {
    DeserializationError::Code err;
    err = skipSpacesAndComments();
    if (err)
      return err;
    switch (current()) {
      case '[':
        return skipArray(nestingLimit);
      case '{':
        return skipObject(nestingLimit);
      case '\"':
      case '\'':
        return skipQuotedString();
      case 't':
        return skipKeyword("true");
      case 'f':
        return skipKeyword("false");
      case 'n':
        return skipKeyword("null");
      default:
        return skipNumericValue();
    }
  }
  template <typename TFilter>
  DeserializationError::Code parseArray(CollectionData& array, TFilter filter,
                                        NestingLimit nestingLimit) {
    DeserializationError::Code err;
    if (nestingLimit.reached())
      return DeserializationError::TooDeep;
    ARDUINOJSON_ASSERT(current() == '[');
    move();
    err = skipSpacesAndComments();
    if (err)
      return err;
    if (eat(']'))
      return DeserializationError::Ok;
    TFilter memberFilter = filter[0UL];
    for (;;) {
      if (memberFilter.allow()) {
        VariantData* value = array.addElement(_pool);
        if (!value)
          return DeserializationError::NoMemory;
        err = parseVariant(*value, memberFilter, nestingLimit.decrement());
        if (err)
          return err;
      } else {
        err = skipVariant(nestingLimit.decrement());
        if (err)
          return err;
      }
      err = skipSpacesAndComments();
      if (err)
        return err;
      if (eat(']'))
        return DeserializationError::Ok;
      if (!eat(','))
        return DeserializationError::InvalidInput;
    }
  }
  DeserializationError::Code skipArray(NestingLimit nestingLimit) {
    DeserializationError::Code err;
    if (nestingLimit.reached())
      return DeserializationError::TooDeep;
    ARDUINOJSON_ASSERT(current() == '[');
    move();
    for (;;) {
      err = skipVariant(nestingLimit.decrement());
      if (err)
        return err;
      err = skipSpacesAndComments();
      if (err)
        return err;
      if (eat(']'))
        return DeserializationError::Ok;
      if (!eat(','))
        return DeserializationError::InvalidInput;
    }
  }
  template <typename TFilter>
  DeserializationError::Code parseObject(CollectionData& object, TFilter filter,
                                         NestingLimit nestingLimit) {
    DeserializationError::Code err;
    if (nestingLimit.reached())
      return DeserializationError::TooDeep;
    ARDUINOJSON_ASSERT(current() == '{');
    move();
    err = skipSpacesAndComments();
    if (err)
      return err;
    if (eat('}'))
      return DeserializationError::Ok;
    for (;;) {
      err = parseKey();
      if (err)
        return err;
      err = skipSpacesAndComments();
      if (err)
        return err;
      if (!eat(':'))
        return DeserializationError::InvalidInput;
      JsonString key = _stringStorage.str();
      TFilter memberFilter = filter[key.c_str()];
      if (memberFilter.allow()) {
        VariantData* variant = object.getMember(adaptString(key.c_str()));
        if (!variant) {
          key = _stringStorage.save();
          VariantSlot* slot = object.addSlot(_pool);
          if (!slot)
            return DeserializationError::NoMemory;
          slot->setKey(key);
          variant = slot->data();
        }
        err = parseVariant(*variant, memberFilter, nestingLimit.decrement());
        if (err)
          return err;
      } else {
        err = skipVariant(nestingLimit.decrement());
        if (err)
          return err;
      }
      err = skipSpacesAndComments();
      if (err)
        return err;
      if (eat('}'))
        return DeserializationError::Ok;
      if (!eat(','))
        return DeserializationError::InvalidInput;
      err = skipSpacesAndComments();
      if (err)
        return err;
    }
  }
  DeserializationError::Code skipObject(NestingLimit nestingLimit) {
    DeserializationError::Code err;
    if (nestingLimit.reached())
      return DeserializationError::TooDeep;
    ARDUINOJSON_ASSERT(current() == '{');
    move();
    err = skipSpacesAndComments();
    if (err)
      return err;
    if (eat('}'))
      return DeserializationError::Ok;
    for (;;) {
      err = skipKey();
      if (err)
        return err;
      err = skipSpacesAndComments();
      if (err)
        return err;
      if (!eat(':'))
        return DeserializationError::InvalidInput;
      err = skipVariant(nestingLimit.decrement());
      if (err)
        return err;
      err = skipSpacesAndComments();
      if (err)
        return err;
      if (eat('}'))
        return DeserializationError::Ok;
      if (!eat(','))
        return DeserializationError::InvalidInput;
      err = skipSpacesAndComments();
      if (err)
        return err;
    }
  }
  DeserializationError::Code parseKey() {
    _stringStorage.startString();
    if (isQuote(current())) {
      return parseQuotedString();
    } else {
      return parseNonQuotedString();
    }
  }
  DeserializationError::Code parseStringValue(VariantData& variant) {
    DeserializationError::Code err;
    _stringStorage.startString();
    err = parseQuotedString();
    if (err)
      return err;
    variant.setString(_stringStorage.save());
    return DeserializationError::Ok;
  }
  DeserializationError::Code parseQuotedString() {
#if ARDUINOJSON_DECODE_UNICODE
    Utf16::Codepoint codepoint;
    DeserializationError::Code err;
#endif
    const char stopChar = current();
    move();
    for (;;) {
      char c = current();
      move();
      if (c == stopChar)
        break;
      if (c == '\0')
        return DeserializationError::IncompleteInput;
      if (c == '\\') {
        c = current();
        if (c == '\0')
          return DeserializationError::IncompleteInput;
        if (c == 'u') {
#if ARDUINOJSON_DECODE_UNICODE
          move();
          uint16_t codeunit;
          err = parseHex4(codeunit);
          if (err)
            return err;
          if (codepoint.append(codeunit))
            Utf8::encodeCodepoint(codepoint.value(), _stringStorage);
#else
          _stringStorage.append('\\');
#endif
          continue;
        }
        c = EscapeSequence::unescapeChar(c);
        if (c == '\0')
          return DeserializationError::InvalidInput;
        move();
      }
      _stringStorage.append(c);
    }
    if (!_stringStorage.isValid())
      return DeserializationError::NoMemory;
    return DeserializationError::Ok;
  }
  DeserializationError::Code parseNonQuotedString() {
    char c = current();
    ARDUINOJSON_ASSERT(c);
    if (canBeInNonQuotedString(c)) {  // no quotes
      do {
        move();
        _stringStorage.append(c);
        c = current();
      } while (canBeInNonQuotedString(c));
    } else {
      return DeserializationError::InvalidInput;
    }
    if (!_stringStorage.isValid())
      return DeserializationError::NoMemory;
    return DeserializationError::Ok;
  }
  DeserializationError::Code skipKey() {
    if (isQuote(current())) {
      return skipQuotedString();
    } else {
      return skipNonQuotedString();
    }
  }
  DeserializationError::Code skipQuotedString() {
    const char stopChar = current();
    move();
    for (;;) {
      char c = current();
      move();
      if (c == stopChar)
        break;
      if (c == '\0')
        return DeserializationError::IncompleteInput;
      if (c == '\\') {
        if (current() != '\0')
          move();
      }
    }
    return DeserializationError::Ok;
  }
  DeserializationError::Code skipNonQuotedString() {
    char c = current();
    while (canBeInNonQuotedString(c)) {
      move();
      c = current();
    }
    return DeserializationError::Ok;
  }
  DeserializationError::Code parseNumericValue(VariantData& result) {
    uint8_t n = 0;
    char c = current();
    while (canBeInNumber(c) && n < 63) {
      move();
      _buffer[n++] = c;
      c = current();
    }
    _buffer[n] = 0;
    if (!parseNumber(_buffer, result))
      return DeserializationError::InvalidInput;
    return DeserializationError::Ok;
  }
  DeserializationError::Code skipNumericValue() {
    char c = current();
    while (canBeInNumber(c)) {
      move();
      c = current();
    }
    return DeserializationError::Ok;
  }
  DeserializationError::Code parseHex4(uint16_t& result) {
    result = 0;
    for (uint8_t i = 0; i < 4; ++i) {
      char digit = current();
      if (!digit)
        return DeserializationError::IncompleteInput;
      uint8_t value = decodeHex(digit);
      if (value > 0x0F)
        return DeserializationError::InvalidInput;
      result = uint16_t((result << 4) | value);
      move();
    }
    return DeserializationError::Ok;
  }
  static inline bool isBetween(char c, char min, char max) {
    return min <= c && c <= max;
  }
  static inline bool canBeInNumber(char c) {
    return isBetween(c, '0', '9') || c == '+' || c == '-' || c == '.' ||
#if ARDUINOJSON_ENABLE_NAN || ARDUINOJSON_ENABLE_INFINITY
           isBetween(c, 'A', 'Z') || isBetween(c, 'a', 'z');
#else
           c == 'e' || c == 'E';
#endif
  }
  static inline bool canBeInNonQuotedString(char c) {
    return isBetween(c, '0', '9') || isBetween(c, '_', 'z') ||
           isBetween(c, 'A', 'Z');
  }
  static inline bool isQuote(char c) {
    return c == '\'' || c == '\"';
  }
  static inline uint8_t decodeHex(char c) {
    if (c < 'A')
      return uint8_t(c - '0');
    c = char(c & ~0x20);  // uppercase
    return uint8_t(c - 'A' + 10);
  }
  DeserializationError::Code skipSpacesAndComments() {
    for (;;) {
      switch (current()) {
        case '\0':
          return _foundSomething ? DeserializationError::IncompleteInput
                                 : DeserializationError::EmptyInput;
        case ' ':
        case '\t':
        case '\r':
        case '\n':
          move();
          continue;
#if ARDUINOJSON_ENABLE_COMMENTS
        case '/':
          move();  // skip '/'
          switch (current()) {
            case '*': {
              move();  // skip '*'
              bool wasStar = false;
              for (;;) {
                char c = current();
                if (c == '\0')
                  return DeserializationError::IncompleteInput;
                if (c == '/' && wasStar) {
                  move();
                  break;
                }
                wasStar = c == '*';
                move();
              }
              break;
            }
            case '/':
              for (;;) {
                move();
                char c = current();
                if (c == '\0')
                  return DeserializationError::IncompleteInput;
                if (c == '\n')
                  break;
              }
              break;
            default:
              return DeserializationError::InvalidInput;
          }
          break;
#endif
        default:
          _foundSomething = true;
          return DeserializationError::Ok;
      }
    }
  }
  DeserializationError::Code skipKeyword(const char* s) {
    while (*s) {
      char c = current();
      if (c == '\0')
        return DeserializationError::IncompleteInput;
      if (*s != c)
        return DeserializationError::InvalidInput;
      ++s;
      move();
    }
    return DeserializationError::Ok;
  }
  TStringStorage _stringStorage;
  bool _foundSomething;
  Latch<TReader> _latch;
  MemoryPool* _pool;
  char _buffer[64];  // using a member instead of a local variable because it
};
template <typename TString>
DeserializationError deserializeJson(
    JsonDocument& doc, const TString& input,
    NestingLimit nestingLimit = NestingLimit()) {
  return deserialize<JsonDeserializer>(doc, input, nestingLimit,
                                       AllowAllFilter());
}
template <typename TString>
DeserializationError deserializeJson(
    JsonDocument& doc, const TString& input, Filter filter,
    NestingLimit nestingLimit = NestingLimit()) {
  return deserialize<JsonDeserializer>(doc, input, nestingLimit, filter);
}
template <typename TString>
DeserializationError deserializeJson(JsonDocument& doc, const TString& input,
                                     NestingLimit nestingLimit, Filter filter) {
  return deserialize<JsonDeserializer>(doc, input, nestingLimit, filter);
}
template <typename TStream>
DeserializationError deserializeJson(
    JsonDocument& doc, TStream& input,
    NestingLimit nestingLimit = NestingLimit()) {
  return deserialize<JsonDeserializer>(doc, input, nestingLimit,
                                       AllowAllFilter());
}
template <typename TStream>
DeserializationError deserializeJson(
    JsonDocument& doc, TStream& input, Filter filter,
    NestingLimit nestingLimit = NestingLimit()) {
  return deserialize<JsonDeserializer>(doc, input, nestingLimit, filter);
}
template <typename TStream>
DeserializationError deserializeJson(JsonDocument& doc, TStream& input,
                                     NestingLimit nestingLimit, Filter filter) {
  return deserialize<JsonDeserializer>(doc, input, nestingLimit, filter);
}
template <typename TChar>
DeserializationError deserializeJson(
    JsonDocument& doc, TChar* input,
    NestingLimit nestingLimit = NestingLimit()) {
  return deserialize<JsonDeserializer>(doc, input, nestingLimit,
                                       AllowAllFilter());
}
template <typename TChar>
DeserializationError deserializeJson(
    JsonDocument& doc, TChar* input, Filter filter,
    NestingLimit nestingLimit = NestingLimit()) {
  return deserialize<JsonDeserializer>(doc, input, nestingLimit, filter);
}
template <typename TChar>
DeserializationError deserializeJson(JsonDocument& doc, TChar* input,
                                     NestingLimit nestingLimit, Filter filter) {
  return deserialize<JsonDeserializer>(doc, input, nestingLimit, filter);
}
template <typename TChar>
DeserializationError deserializeJson(
    JsonDocument& doc, TChar* input, size_t inputSize,
    NestingLimit nestingLimit = NestingLimit()) {
  return deserialize<JsonDeserializer>(doc, input, inputSize, nestingLimit,
                                       AllowAllFilter());
}
template <typename TChar>
DeserializationError deserializeJson(
    JsonDocument& doc, TChar* input, size_t inputSize, Filter filter,
    NestingLimit nestingLimit = NestingLimit()) {
  return deserialize<JsonDeserializer>(doc, input, inputSize, nestingLimit,
                                       filter);
}
template <typename TChar>
DeserializationError deserializeJson(JsonDocument& doc, TChar* input,
                                     size_t inputSize,
                                     NestingLimit nestingLimit, Filter filter) {
  return deserialize<JsonDeserializer>(doc, input, inputSize, nestingLimit,
                                       filter);
}
template <typename TWriter>
class PrettyJsonSerializer : public JsonSerializer<TWriter> {
  typedef JsonSerializer<TWriter> base;
 public:
  PrettyJsonSerializer(TWriter writer) : base(writer), _nesting(0) {}
  size_t visitArray(const CollectionData& array) {
    const VariantSlot* slot = array.head();
    if (slot) {
      base::write("[\r\n");
      _nesting++;
      while (slot != 0) {
        indent();
        slot->data()->accept(*this);
        slot = slot->next();
        base::write(slot ? ",\r\n" : "\r\n");
      }
      _nesting--;
      indent();
      base::write("]");
    } else {
      base::write("[]");
    }
    return this->bytesWritten();
  }
  size_t visitObject(const CollectionData& object) {
    const VariantSlot* slot = object.head();
    if (slot) {
      base::write("{\r\n");
      _nesting++;
      while (slot != 0) {
        indent();
        base::visitString(slot->key());
        base::write(": ");
        slot->data()->accept(*this);
        slot = slot->next();
        base::write(slot ? ",\r\n" : "\r\n");
      }
      _nesting--;
      indent();
      base::write("}");
    } else {
      base::write("{}");
    }
    return this->bytesWritten();
  }
 private:
  void indent() {
    for (uint8_t i = 0; i < _nesting; i++)
      base::write(ARDUINOJSON_TAB);
  }
  uint8_t _nesting;
};
template <typename TDestination>
size_t serializeJsonPretty(JsonVariantConst source, TDestination& destination) {
  return serialize<PrettyJsonSerializer>(source, destination);
}
inline size_t serializeJsonPretty(JsonVariantConst source, void* buffer,
                                  size_t bufferSize) {
  return serialize<PrettyJsonSerializer>(source, buffer, bufferSize);
}
inline size_t measureJsonPretty(JsonVariantConst source) {
  return measure<PrettyJsonSerializer>(source);
}
#if ARDUINOJSON_LITTLE_ENDIAN
inline void swapBytes(uint8_t& a, uint8_t& b) {
  uint8_t t(a);
  a = b;
  b = t;
}
inline void fixEndianess(uint8_t* p, integral_constant<size_t, 8>) {
  swapBytes(p[0], p[7]);
  swapBytes(p[1], p[6]);
  swapBytes(p[2], p[5]);
  swapBytes(p[3], p[4]);
}
inline void fixEndianess(uint8_t* p, integral_constant<size_t, 4>) {
  swapBytes(p[0], p[3]);
  swapBytes(p[1], p[2]);
}
inline void fixEndianess(uint8_t* p, integral_constant<size_t, 2>) {
  swapBytes(p[0], p[1]);
}
inline void fixEndianess(uint8_t*, integral_constant<size_t, 1>) {}
template <typename T>
inline void fixEndianess(T& value) {
  fixEndianess(reinterpret_cast<uint8_t*>(&value),
               integral_constant<size_t, sizeof(T)>());
}
#else
template <typename T>
inline void fixEndianess(T&) {}
#endif
inline void doubleToFloat(const uint8_t d[8], uint8_t f[4]) {
  f[0] = uint8_t((d[0] & 0xC0) | (d[0] << 3 & 0x3f) | (d[1] >> 5));
  f[1] = uint8_t((d[1] << 3) | (d[2] >> 5));
  f[2] = uint8_t((d[2] << 3) | (d[3] >> 5));
  f[3] = uint8_t((d[3] << 3) | (d[4] >> 5));
}
template <typename TReader, typename TStringStorage>
class MsgPackDeserializer {
 public:
  MsgPackDeserializer(MemoryPool* pool, TReader reader,
                      TStringStorage stringStorage)
      : _pool(pool),
        _reader(reader),
        _stringStorage(stringStorage),
        _foundSomething(false) {}
  template <typename TFilter>
  DeserializationError parse(VariantData& variant, TFilter filter,
                             NestingLimit nestingLimit) {
    DeserializationError::Code err;
    err = parseVariant(&variant, filter, nestingLimit);
    return _foundSomething ? err : DeserializationError::EmptyInput;
  }
 private:
  template <typename TFilter>
  DeserializationError::Code parseVariant(VariantData* variant, TFilter filter,
                                          NestingLimit nestingLimit) {
    DeserializationError::Code err;
    uint8_t code = 0;  // TODO: why do we need to initialize this variable?
    err = readByte(code);
    if (err)
      return err;
    _foundSomething = true;
    bool allowValue = filter.allowValue();
    if (allowValue) {
      ARDUINOJSON_ASSERT(variant != 0);
    }
    switch (code) {
      case 0xc0:
        return DeserializationError::Ok;
      case 0xc1:
        return DeserializationError::InvalidInput;
      case 0xc2:
        if (allowValue)
          variant->setBoolean(false);
        return DeserializationError::Ok;
      case 0xc3:
        if (allowValue)
          variant->setBoolean(true);
        return DeserializationError::Ok;
      case 0xc4:  // bin 8 (not supported)
        return skipString<uint8_t>();
      case 0xc5:  // bin 16 (not supported)
        return skipString<uint16_t>();
      case 0xc6:  // bin 32 (not supported)
        return skipString<uint32_t>();
      case 0xc7:  // ext 8 (not supported)
        return skipExt<uint8_t>();
      case 0xc8:  // ext 16 (not supported)
        return skipExt<uint16_t>();
      case 0xc9:  // ext 32 (not supported)
        return skipExt<uint32_t>();
      case 0xca:
        if (allowValue)
          return readFloat<float>(variant);
        else
          return skipBytes(4);
      case 0xcb:
        if (allowValue)
          return readDouble<double>(variant);
        else
          return skipBytes(8);
      case 0xcc:
        if (allowValue)
          return readInteger<uint8_t>(variant);
        else
          return skipBytes(1);
      case 0xcd:
        if (allowValue)
          return readInteger<uint16_t>(variant);
        else
          return skipBytes(2);
      case 0xce:
        if (allowValue)
          return readInteger<uint32_t>(variant);
        else
          return skipBytes(4);
      case 0xcf:
#if ARDUINOJSON_USE_LONG_LONG
        if (allowValue)
          return readInteger<uint64_t>(variant);
        else
          return skipBytes(8);
#else
        return skipBytes(8);  // not supported
#endif
      case 0xd0:
        if (allowValue)
          return readInteger<int8_t>(variant);
        else
          return skipBytes(1);
      case 0xd1:
        if (allowValue)
          return readInteger<int16_t>(variant);
        else
          return skipBytes(2);
      case 0xd2:
        if (allowValue)
          return readInteger<int32_t>(variant);
        else
          return skipBytes(4);
      case 0xd3:
#if ARDUINOJSON_USE_LONG_LONG
        if (allowValue)
          return readInteger<int64_t>(variant);
        else
          return skipBytes(8);  // not supported
#else
        return skipBytes(8);
#endif
      case 0xd4:  // fixext 1 (not supported)
        return skipBytes(2);
      case 0xd5:  // fixext 2 (not supported)
        return skipBytes(3);
      case 0xd6:  // fixext 4 (not supported)
        return skipBytes(5);
      case 0xd7:  // fixext 8 (not supported)
        return skipBytes(9);
      case 0xd8:  // fixext 16 (not supported)
        return skipBytes(17);
      case 0xd9:
        if (allowValue)
          return readString<uint8_t>(variant);
        else
          return skipString<uint8_t>();
      case 0xda:
        if (allowValue)
          return readString<uint16_t>(variant);
        else
          return skipString<uint16_t>();
      case 0xdb:
        if (allowValue)
          return readString<uint32_t>(variant);
        else
          return skipString<uint32_t>();
      case 0xdc:
        return readArray<uint16_t>(variant, filter, nestingLimit);
      case 0xdd:
        return readArray<uint32_t>(variant, filter, nestingLimit);
      case 0xde:
        return readObject<uint16_t>(variant, filter, nestingLimit);
      case 0xdf:
        return readObject<uint32_t>(variant, filter, nestingLimit);
    }
    switch (code & 0xf0) {
      case 0x80:
        return readObject(variant, code & 0x0F, filter, nestingLimit);
      case 0x90:
        return readArray(variant, code & 0x0F, filter, nestingLimit);
    }
    if ((code & 0xe0) == 0xa0) {
      if (allowValue)
        return readString(variant, code & 0x1f);
      else
        return skipBytes(code & 0x1f);
    }
    if (allowValue)
      variant->setInteger(static_cast<int8_t>(code));
    return DeserializationError::Ok;
  }
  DeserializationError::Code readByte(uint8_t& value) {
    int c = _reader.read();
    if (c < 0)
      return DeserializationError::IncompleteInput;
    value = static_cast<uint8_t>(c);
    return DeserializationError::Ok;
  }
  DeserializationError::Code readBytes(uint8_t* p, size_t n) {
    if (_reader.readBytes(reinterpret_cast<char*>(p), n) == n)
      return DeserializationError::Ok;
    return DeserializationError::IncompleteInput;
  }
  template <typename T>
  DeserializationError::Code readBytes(T& value) {
    return readBytes(reinterpret_cast<uint8_t*>(&value), sizeof(value));
  }
  DeserializationError::Code skipBytes(size_t n) {
    for (; n; --n) {
      if (_reader.read() < 0)
        return DeserializationError::IncompleteInput;
    }
    return DeserializationError::Ok;
  }
  template <typename T>
  DeserializationError::Code readInteger(T& value) {
    DeserializationError::Code err;
    err = readBytes(value);
    if (err)
      return err;
    fixEndianess(value);
    return DeserializationError::Ok;
  }
  template <typename T>
  DeserializationError::Code readInteger(VariantData* variant) {
    DeserializationError::Code err;
    T value;
    err = readInteger(value);
    if (err)
      return err;
    variant->setInteger(value);
    return DeserializationError::Ok;
  }
  template <typename T>
  typename enable_if<sizeof(T) == 4, DeserializationError::Code>::type
  readFloat(VariantData* variant) {
    DeserializationError::Code err;
    T value;
    err = readBytes(value);
    if (err)
      return err;
    fixEndianess(value);
    variant->setFloat(value);
    return DeserializationError::Ok;
  }
  template <typename T>
  typename enable_if<sizeof(T) == 8, DeserializationError::Code>::type
  readDouble(VariantData* variant) {
    DeserializationError::Code err;
    T value;
    err = readBytes(value);
    if (err)
      return err;
    fixEndianess(value);
    variant->setFloat(value);
    return DeserializationError::Ok;
  }
  template <typename T>
  typename enable_if<sizeof(T) == 4, DeserializationError::Code>::type
  readDouble(VariantData* variant) {
    DeserializationError::Code err;
    uint8_t i[8];  // input is 8 bytes
    T value;       // output is 4 bytes
    uint8_t* o = reinterpret_cast<uint8_t*>(&value);
    err = readBytes(i, 8);
    if (err)
      return err;
    doubleToFloat(i, o);
    fixEndianess(value);
    variant->setFloat(value);
    return DeserializationError::Ok;
  }
  template <typename T>
  DeserializationError::Code readString(VariantData* variant) {
    DeserializationError::Code err;
    T size;
    err = readInteger(size);
    if (err)
      return err;
    return readString(variant, size);
  }
  template <typename T>
  DeserializationError::Code readString() {
    DeserializationError::Code err;
    T size;
    err = readInteger(size);
    if (err)
      return err;
    return readString(size);
  }
  template <typename T>
  DeserializationError::Code skipString() {
    DeserializationError::Code err;
    T size;
    err = readInteger(size);
    if (err)
      return err;
    return skipBytes(size);
  }
  DeserializationError::Code readString(VariantData* variant, size_t n) {
    DeserializationError::Code err;
    err = readString(n);
    if (err)
      return err;
    variant->setString(_stringStorage.save());
    return DeserializationError::Ok;
  }
  DeserializationError::Code readString(size_t n) {
    DeserializationError::Code err;
    _stringStorage.startString();
    for (; n; --n) {
      uint8_t c;
      err = readBytes(c);
      if (err)
        return err;
      _stringStorage.append(static_cast<char>(c));
    }
    if (!_stringStorage.isValid())
      return DeserializationError::NoMemory;
    return DeserializationError::Ok;
  }
  template <typename TSize, typename TFilter>
  DeserializationError::Code readArray(VariantData* variant, TFilter filter,
                                       NestingLimit nestingLimit) {
    DeserializationError::Code err;
    TSize size;
    err = readInteger(size);
    if (err)
      return err;
    return readArray(variant, size, filter, nestingLimit);
  }
  template <typename TFilter>
  DeserializationError::Code readArray(VariantData* variant, size_t n,
                                       TFilter filter,
                                       NestingLimit nestingLimit) {
    DeserializationError::Code err;
    if (nestingLimit.reached())
      return DeserializationError::TooDeep;
    bool allowArray = filter.allowArray();
    CollectionData* array = allowArray ? &variant->toArray() : 0;
    TFilter memberFilter = filter[0U];
    for (; n; --n) {
      VariantData* value;
      if (memberFilter.allow()) {
        value = array->addElement(_pool);
        if (!value)
          return DeserializationError::NoMemory;
      } else {
        value = 0;
      }
      err = parseVariant(value, memberFilter, nestingLimit.decrement());
      if (err)
        return err;
    }
    return DeserializationError::Ok;
  }
  template <typename TSize, typename TFilter>
  DeserializationError::Code readObject(VariantData* variant, TFilter filter,
                                        NestingLimit nestingLimit) {
    DeserializationError::Code err;
    TSize size;
    err = readInteger(size);
    if (err)
      return err;
    return readObject(variant, size, filter, nestingLimit);
  }
  template <typename TFilter>
  DeserializationError::Code readObject(VariantData* variant, size_t n,
                                        TFilter filter,
                                        NestingLimit nestingLimit) {
    DeserializationError::Code err;
    if (nestingLimit.reached())
      return DeserializationError::TooDeep;
    CollectionData* object = filter.allowObject() ? &variant->toObject() : 0;
    for (; n; --n) {
      err = readKey();
      if (err)
        return err;
      JsonString key = _stringStorage.str();
      TFilter memberFilter = filter[key.c_str()];
      VariantData* member;
      if (memberFilter.allow()) {
        ARDUINOJSON_ASSERT(object);
        key = _stringStorage.save();
        VariantSlot* slot = object->addSlot(_pool);
        if (!slot)
          return DeserializationError::NoMemory;
        slot->setKey(key);
        member = slot->data();
      } else {
        member = 0;
      }
      err = parseVariant(member, memberFilter, nestingLimit.decrement());
      if (err)
        return err;
    }
    return DeserializationError::Ok;
  }
  DeserializationError::Code readKey() {
    DeserializationError::Code err;
    uint8_t code;
    err = readByte(code);
    if (err)
      return err;
    if ((code & 0xe0) == 0xa0)
      return readString(code & 0x1f);
    switch (code) {
      case 0xd9:
        return readString<uint8_t>();
      case 0xda:
        return readString<uint16_t>();
      case 0xdb:
        return readString<uint32_t>();
      default:
        return DeserializationError::InvalidInput;
    }
  }
  template <typename T>
  DeserializationError::Code skipExt() {
    DeserializationError::Code err;
    T size;
    err = readInteger(size);
    if (err)
      return err;
    return skipBytes(size + 1U);
  }
  MemoryPool* _pool;
  TReader _reader;
  TStringStorage _stringStorage;
  bool _foundSomething;
};
template <typename TString>
DeserializationError deserializeMsgPack(
    JsonDocument& doc, const TString& input,
    NestingLimit nestingLimit = NestingLimit()) {
  return deserialize<MsgPackDeserializer>(doc, input, nestingLimit,
                                          AllowAllFilter());
}
template <typename TString>
DeserializationError deserializeMsgPack(
    JsonDocument& doc, const TString& input, Filter filter,
    NestingLimit nestingLimit = NestingLimit()) {
  return deserialize<MsgPackDeserializer>(doc, input, nestingLimit, filter);
}
template <typename TString>
DeserializationError deserializeMsgPack(JsonDocument& doc, const TString& input,
                                        NestingLimit nestingLimit,
                                        Filter filter) {
  return deserialize<MsgPackDeserializer>(doc, input, nestingLimit, filter);
}
template <typename TStream>
DeserializationError deserializeMsgPack(
    JsonDocument& doc, TStream& input,
    NestingLimit nestingLimit = NestingLimit()) {
  return deserialize<MsgPackDeserializer>(doc, input, nestingLimit,
                                          AllowAllFilter());
}
template <typename TStream>
DeserializationError deserializeMsgPack(
    JsonDocument& doc, TStream& input, Filter filter,
    NestingLimit nestingLimit = NestingLimit()) {
  return deserialize<MsgPackDeserializer>(doc, input, nestingLimit, filter);
}
template <typename TStream>
DeserializationError deserializeMsgPack(JsonDocument& doc, TStream& input,
                                        NestingLimit nestingLimit,
                                        Filter filter) {
  return deserialize<MsgPackDeserializer>(doc, input, nestingLimit, filter);
}
template <typename TChar>
DeserializationError deserializeMsgPack(
    JsonDocument& doc, TChar* input,
    NestingLimit nestingLimit = NestingLimit()) {
  return deserialize<MsgPackDeserializer>(doc, input, nestingLimit,
                                          AllowAllFilter());
}
template <typename TChar>
DeserializationError deserializeMsgPack(
    JsonDocument& doc, TChar* input, Filter filter,
    NestingLimit nestingLimit = NestingLimit()) {
  return deserialize<MsgPackDeserializer>(doc, input, nestingLimit, filter);
}
template <typename TChar>
DeserializationError deserializeMsgPack(JsonDocument& doc, TChar* input,
                                        NestingLimit nestingLimit,
                                        Filter filter) {
  return deserialize<MsgPackDeserializer>(doc, input, nestingLimit, filter);
}
template <typename TChar>
DeserializationError deserializeMsgPack(
    JsonDocument& doc, TChar* input, size_t inputSize,
    NestingLimit nestingLimit = NestingLimit()) {
  return deserialize<MsgPackDeserializer>(doc, input, inputSize, nestingLimit,
                                          AllowAllFilter());
}
template <typename TChar>
DeserializationError deserializeMsgPack(
    JsonDocument& doc, TChar* input, size_t inputSize, Filter filter,
    NestingLimit nestingLimit = NestingLimit()) {
  return deserialize<MsgPackDeserializer>(doc, input, inputSize, nestingLimit,
                                          filter);
}
template <typename TChar>
DeserializationError deserializeMsgPack(JsonDocument& doc, TChar* input,
                                        size_t inputSize,
                                        NestingLimit nestingLimit,
                                        Filter filter) {
  return deserialize<MsgPackDeserializer>(doc, input, inputSize, nestingLimit,
                                          filter);
}
template <typename TWriter>
class MsgPackSerializer : public Visitor<size_t> {
 public:
  static const bool producesText = false;
  MsgPackSerializer(TWriter writer) : _writer(writer) {}
  template <typename T>
  typename enable_if<sizeof(T) == 4, size_t>::type visitFloat(T value32) {
    if (canConvertNumber<JsonInteger>(value32)) {
      JsonInteger truncatedValue = JsonInteger(value32);
      if (value32 == T(truncatedValue))
        return visitSignedInteger(truncatedValue);
    }
    writeByte(0xCA);
    writeInteger(value32);
    return bytesWritten();
  }
  template <typename T>
  ARDUINOJSON_NO_SANITIZE("float-cast-overflow")
  typename enable_if<sizeof(T) == 8, size_t>::type visitFloat(T value64) {
    float value32 = float(value64);
    if (value32 == value64)
      return visitFloat(value32);
    writeByte(0xCB);
    writeInteger(value64);
    return bytesWritten();
  }
  size_t visitArray(const CollectionData& array) {
    size_t n = array.size();
    if (n < 0x10) {
      writeByte(uint8_t(0x90 + array.size()));
    } else if (n < 0x10000) {
      writeByte(0xDC);
      writeInteger(uint16_t(n));
    } else {
      writeByte(0xDD);
      writeInteger(uint32_t(n));
    }
    for (const VariantSlot* slot = array.head(); slot; slot = slot->next()) {
      slot->data()->accept(*this);
    }
    return bytesWritten();
  }
  size_t visitObject(const CollectionData& object) {
    size_t n = object.size();
    if (n < 0x10) {
      writeByte(uint8_t(0x80 + n));
    } else if (n < 0x10000) {
      writeByte(0xDE);
      writeInteger(uint16_t(n));
    } else {
      writeByte(0xDF);
      writeInteger(uint32_t(n));
    }
    for (const VariantSlot* slot = object.head(); slot; slot = slot->next()) {
      visitString(slot->key());
      slot->data()->accept(*this);
    }
    return bytesWritten();
  }
  size_t visitString(const char* value) {
    return visitString(value, strlen(value));
  }
  size_t visitString(const char* value, size_t n) {
    ARDUINOJSON_ASSERT(value != NULL);
    if (n < 0x20) {
      writeByte(uint8_t(0xA0 + n));
    } else if (n < 0x100) {
      writeByte(0xD9);
      writeInteger(uint8_t(n));
    } else if (n < 0x10000) {
      writeByte(0xDA);
      writeInteger(uint16_t(n));
    } else {
      writeByte(0xDB);
      writeInteger(uint32_t(n));
    }
    writeBytes(reinterpret_cast<const uint8_t*>(value), n);
    return bytesWritten();
  }
  size_t visitRawJson(const char* data, size_t size) {
    writeBytes(reinterpret_cast<const uint8_t*>(data), size);
    return bytesWritten();
  }
  size_t visitSignedInteger(JsonInteger value) {
    if (value > 0) {
      visitUnsignedInteger(static_cast<JsonUInt>(value));
    } else if (value >= -0x20) {
      writeInteger(int8_t(value));
    } else if (value >= -0x80) {
      writeByte(0xD0);
      writeInteger(int8_t(value));
    } else if (value >= -0x8000) {
      writeByte(0xD1);
      writeInteger(int16_t(value));
    }
#if ARDUINOJSON_USE_LONG_LONG
    else if (value >= -0x80000000LL)
#else
    else
#endif
    {
      writeByte(0xD2);
      writeInteger(int32_t(value));
    }
#if ARDUINOJSON_USE_LONG_LONG
    else {
      writeByte(0xD3);
      writeInteger(int64_t(value));
    }
#endif
    return bytesWritten();
  }
  size_t visitUnsignedInteger(JsonUInt value) {
    if (value <= 0x7F) {
      writeInteger(uint8_t(value));
    } else if (value <= 0xFF) {
      writeByte(0xCC);
      writeInteger(uint8_t(value));
    } else if (value <= 0xFFFF) {
      writeByte(0xCD);
      writeInteger(uint16_t(value));
    }
#if ARDUINOJSON_USE_LONG_LONG
    else if (value <= 0xFFFFFFFF)
#else
    else
#endif
    {
      writeByte(0xCE);
      writeInteger(uint32_t(value));
    }
#if ARDUINOJSON_USE_LONG_LONG
    else {
      writeByte(0xCF);
      writeInteger(uint64_t(value));
    }
#endif
    return bytesWritten();
  }
  size_t visitBoolean(bool value) {
    writeByte(value ? 0xC3 : 0xC2);
    return bytesWritten();
  }
  size_t visitNull() {
    writeByte(0xC0);
    return bytesWritten();
  }
 private:
  size_t bytesWritten() const {
    return _writer.count();
  }
  void writeByte(uint8_t c) {
    _writer.write(c);
  }
  void writeBytes(const uint8_t* p, size_t n) {
    _writer.write(p, n);
  }
  template <typename T>
  void writeInteger(T value) {
    fixEndianess(value);
    writeBytes(reinterpret_cast<uint8_t*>(&value), sizeof(value));
  }
  CountingDecorator<TWriter> _writer;
};
template <typename TDestination>
inline size_t serializeMsgPack(JsonVariantConst source, TDestination& output) {
  return serialize<MsgPackSerializer>(source, output);
}
inline size_t serializeMsgPack(JsonVariantConst source, void* output,
                               size_t size) {
  return serialize<MsgPackSerializer>(source, output, size);
}
inline size_t measureMsgPack(JsonVariantConst source) {
  return measure<MsgPackSerializer>(source);
}
}  // namespace ARDUINOJSON_NAMESPACE

#ifdef __GNUC__
#define ARDUINOJSON_PRAGMA(x) _Pragma(#x)
#define ARDUINOJSON_COMPILE_ERROR(msg) ARDUINOJSON_PRAGMA(GCC error msg)
#define ARDUINOJSON_STRINGIFY(S) #S
#define ARDUINOJSON_DEPRECATION_ERROR(X, Y) \
  ARDUINOJSON_COMPILE_ERROR(ARDUINOJSON_STRINGIFY(X is a Y from ArduinoJson 5. Please see https:/\/arduinojson.org/upgrade to learn how to upgrade your program to ArduinoJson version 6))
#define StaticJsonBuffer ARDUINOJSON_DEPRECATION_ERROR(StaticJsonBuffer, class)
#define DynamicJsonBuffer ARDUINOJSON_DEPRECATION_ERROR(DynamicJsonBuffer, class)
#define JsonBuffer ARDUINOJSON_DEPRECATION_ERROR(JsonBuffer, class)
#define RawJson ARDUINOJSON_DEPRECATION_ERROR(RawJson, function)
#endif

namespace ArduinoJson {
using ARDUINOJSON_NAMESPACE::BasicJsonDocument;
using ARDUINOJSON_NAMESPACE::copyArray;
using ARDUINOJSON_NAMESPACE::DeserializationError;
using ARDUINOJSON_NAMESPACE::deserializeJson;
using ARDUINOJSON_NAMESPACE::deserializeMsgPack;
using ARDUINOJSON_NAMESPACE::DynamicJsonDocument;
using ARDUINOJSON_NAMESPACE::JsonArray;
using ARDUINOJSON_NAMESPACE::JsonArrayConst;
using ARDUINOJSON_NAMESPACE::JsonDocument;
using ARDUINOJSON_NAMESPACE::JsonFloat;
using ARDUINOJSON_NAMESPACE::JsonInteger;
using ARDUINOJSON_NAMESPACE::JsonObject;
using ARDUINOJSON_NAMESPACE::JsonObjectConst;
using ARDUINOJSON_NAMESPACE::JsonPair;
using ARDUINOJSON_NAMESPACE::JsonPairConst;
using ARDUINOJSON_NAMESPACE::JsonString;
using ARDUINOJSON_NAMESPACE::JsonUInt;
using ARDUINOJSON_NAMESPACE::JsonVariant;
using ARDUINOJSON_NAMESPACE::JsonVariantConst;
using ARDUINOJSON_NAMESPACE::measureJson;
using ARDUINOJSON_NAMESPACE::serialized;
using ARDUINOJSON_NAMESPACE::serializeJson;
using ARDUINOJSON_NAMESPACE::serializeJsonPretty;
using ARDUINOJSON_NAMESPACE::serializeMsgPack;
using ARDUINOJSON_NAMESPACE::StaticJsonDocument;

namespace DeserializationOption {
using ARDUINOJSON_NAMESPACE::Filter;
using ARDUINOJSON_NAMESPACE::NestingLimit;
}  // namespace DeserializationOption
}  // namespace ArduinoJson
