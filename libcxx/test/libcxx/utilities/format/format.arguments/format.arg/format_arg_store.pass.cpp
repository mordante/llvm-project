//===----------------------------------------------------------------------===//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17
// UNSUPPORTED: libcpp-no-concepts
// UNSUPPORTED: libcpp-has-no-incomplete-format

// <format>

// Test the internals of basic_format_args specifically whether the interals
// work. The internals aren't specified but have some requirements.
//
// [format.args]/1 An instance of basic_format_args provides access to
// formatting arguments. Implementations should optimize the representation of
// basic_format_args for a small number of formatting arguments.
// [Note 1: For example, by storing indices of type alternatives separately
// from values and packing the former.  --  end note]

// XXX REname to basic_format_args_internal_representation.pass.cpp

// XXX tests for handle

#include <format>
#include <cassert>
#include <concepts>
#include <type_traits>
#include <string>
#include <string_view>

#include "constexpr_char_traits.h"
#include "test_macros.h"
#include "make_string.h"
#include "min_allocator.h"

#define CSTR(S) MAKE_CSTRING(CharT, S)
#define STR(S) MAKE_STRING(CharT, S)
#define SV(S) MAKE_STRING_VIEW(CharT, S)

// A user-defined type used to test the handle formatter.
enum class status : uint16_t { foo = 0xAAAA, bar = 0x5555, foobar = 0xAA55 };

// The formatter for a user-defined type used to test the handle formatter.
// For this test the formatter only needs to exist not do anything useful.
template <class CharT>
struct std::formatter<status, CharT> {
  constexpr auto parse(auto& parse_ctx) { return parse_ctx.begin(); }
  auto format(status, auto& ctx) const { return ctx.out(); }
};

template <class Context, class T>
void test_equal(const std::basic_format_arg<Context>& arg, const T& value) {
  assert(arg);
  std::visit_format_arg(
      [&value](auto a) {
        if constexpr (std::is_same_v<std::remove_cvref_t<T>, decltype(a)>)
          assert(value == a);
        else
          assert(false);
      },
      arg);
}

// Tests the formatting arguments.
template <class CharT>
void test_store() {
  using context = std::basic_format_context<std::back_insert_iterator<std::basic_string<CharT>>, CharT>;

  // Short names used to cast rvalues in std::make_format_args.
  using sc = signed char;
  using uc = unsigned char;
  using us = unsigned short;
  using ll = long long;
  using ul = unsigned long;
  using ull = unsigned long long;
  using ld = long double;
  using cn = const nullptr_t;
  using v = void*;
  using cv = const void*;

  // Depending on the sizeof(long) its stored as an int or a long long.
  // Some helpers to make it generic in other parts of the code.
  constexpr size_t sizeof_long = (sizeof(long) == sizeof(int)) ? sizeof(int) : sizeof(long long);
  constexpr size_t sizeof_ulong = sizeof_long;
  constexpr std::__format::__arg_t type_long =
      (sizeof(long) == sizeof(int)) ? std::__format::__arg_t::__int : std::__format::__arg_t::__long_long;
  constexpr std::__format::__arg_t type_ulong =
      (sizeof(long) == sizeof(int)) ? std::__format::__arg_t::__unsigned : std::__format::__arg_t::__unsigned_long_long;

  CharT* cstring = const_cast<CharT*>(CSTR("hello C"));
  const CharT* const_cstring = CSTR("hello const C");
  std::basic_string<CharT> s = STR("Hello string");
  std::basic_string_view<CharT> sv = SV("Hello string_view");

  // Create formatting args with all supported types.
  auto store = std::make_format_args<context>(true, CharT('A'), sc(-1), short(-42), int(-99), long(-200), ll(-457),
                                              uc(1), us(42), unsigned(99), ul(200), ull(457), float(-1), double(42),
                                              ld(-99), cstring, const_cstring, s, sv, nullptr, cn(nullptr), v(cstring),
                                              cv(const_cstring), status::foo
#ifndef TEST_HAS_NO_INT128
                                              ,
                                              __int128_t(-9999), __uint128_t(9999)
#endif
  );

  // First test whether size of the internal arrays is the expected size.
#ifndef TEST_HAS_NO_INT128
  static_assert((std::same_as<decltype(store.__meta_data), const std::array<std::__format::__arg_meta_data, 26>>));
#else
  static_assert((std::same_as<decltype(store.__meta_data), const std::array<std::__format::__arg_meta_data, 24>>));
#endif
  static_assert(sizeof(store.__data) == sizeof(bool) + sizeof(CharT) + 6 * sizeof(int) + 2 * sizeof_long +
                                            2 * sizeof(long long) + sizeof(float) + sizeof(double) +
                                            sizeof(long double) + 2 * sizeof(CharT*) +
                                            2 * sizeof(std::basic_string_view<CharT>) + 6 * sizeof(void*)
#ifndef TEST_HAS_NO_INT128
                                            + 2 * sizeof(__int128_t)
#endif
  );

  // Test the contents of the meta data.
  // Since the meta data only depends on the types of the argument it's calculated compile-time.
  static_assert(store.__meta_data[0].__offset == 0);
  static_assert(store.__meta_data[0].__type == std::__format::__arg_t::__boolean);
  {
    constexpr size_t offset = store.__meta_data[0].__offset + sizeof(bool);
    static_assert(store.__meta_data[1].__offset == offset);
    static_assert(store.__meta_data[1].__type == std::__format::__arg_t::__char_type);
  }
  {
    constexpr size_t offset = store.__meta_data[1].__offset + sizeof(CharT);
    static_assert(store.__meta_data[2].__offset == offset);
    static_assert(store.__meta_data[2].__type == std::__format::__arg_t::__int);
    static_assert(store.__meta_data[3].__offset == offset + sizeof(int));
    static_assert(store.__meta_data[3].__type == std::__format::__arg_t::__int);
    static_assert(store.__meta_data[4].__offset == offset + 2 * sizeof(int));
    static_assert(store.__meta_data[4].__type == std::__format::__arg_t::__int);
    static_assert(store.__meta_data[5].__offset == offset + 3 * sizeof(int));
    static_assert(store.__meta_data[5].__type == type_long);
    static_assert(store.__meta_data[6].__offset == offset + 3 * sizeof(int) + sizeof_long);
    static_assert(store.__meta_data[6].__type == std::__format::__arg_t::__long_long);
  }
  {
    constexpr size_t offset = store.__meta_data[6].__offset + sizeof(long long);
    static_assert(store.__meta_data[7].__offset == offset);
    static_assert(store.__meta_data[7].__type == std::__format::__arg_t::__unsigned);
    static_assert(store.__meta_data[8].__offset == offset + sizeof(unsigned));
    static_assert(store.__meta_data[8].__type == std::__format::__arg_t::__unsigned);
    static_assert(store.__meta_data[9].__offset == offset + 2 * sizeof(unsigned));
    static_assert(store.__meta_data[9].__type == std::__format::__arg_t::__unsigned);
    static_assert(store.__meta_data[10].__offset == offset + 3 * sizeof(unsigned));
    static_assert(store.__meta_data[10].__type == type_ulong);
    static_assert(store.__meta_data[11].__offset == offset + 3 * sizeof(unsigned) + sizeof_ulong);
    static_assert(store.__meta_data[11].__type == std::__format::__arg_t::__unsigned_long_long);
  }

  {
    constexpr size_t offset = store.__meta_data[11].__offset + sizeof(unsigned long long);
    static_assert(store.__meta_data[12].__offset == offset);
    static_assert(store.__meta_data[12].__type == std::__format::__arg_t::__float);
    static_assert(store.__meta_data[13].__offset == offset + sizeof(float));
    static_assert(store.__meta_data[13].__type == std::__format::__arg_t::__double);
    static_assert(store.__meta_data[14].__offset == offset + sizeof(float) + sizeof(double));
    static_assert(store.__meta_data[14].__type == std::__format::__arg_t::__long_double);
  }
  {
    constexpr size_t offset = store.__meta_data[14].__offset + sizeof(long double);
    static_assert(store.__meta_data[15].__offset == offset);
    static_assert(store.__meta_data[15].__type == std::__format::__arg_t::__const_char_type_ptr);
    static_assert(store.__meta_data[16].__offset == offset + sizeof(CharT*));
    static_assert(store.__meta_data[16].__type == std::__format::__arg_t::__const_char_type_ptr);
  }
  {
    constexpr size_t offset = store.__meta_data[16].__offset + sizeof(CharT*);
    static_assert(store.__meta_data[17].__offset == offset);
    static_assert(store.__meta_data[17].__type == std::__format::__arg_t::__string_view);
    static_assert(store.__meta_data[18].__offset == offset + sizeof(std::basic_string_view<CharT>));
    static_assert(store.__meta_data[18].__type == std::__format::__arg_t::__string_view);
  }
  {
    constexpr size_t offset = store.__meta_data[18].__offset + sizeof(std::basic_string_view<CharT>);
    static_assert(store.__meta_data[19].__offset == offset);
    static_assert(store.__meta_data[19].__type == std::__format::__arg_t::__ptr);
    static_assert(store.__meta_data[20].__offset == offset + sizeof(void*));
    static_assert(store.__meta_data[20].__type == std::__format::__arg_t::__ptr);
    static_assert(store.__meta_data[21].__offset == offset + 2 * sizeof(void*));
    static_assert(store.__meta_data[21].__type == std::__format::__arg_t::__ptr);
    static_assert(store.__meta_data[22].__offset == offset + 3 * sizeof(void*));
    static_assert(store.__meta_data[22].__type == std::__format::__arg_t::__ptr);
  }
  {
    constexpr size_t offset = store.__meta_data[22].__offset + sizeof(void*);
    static_assert(store.__meta_data[23].__offset == offset);
    static_assert(store.__meta_data[23].__type == std::__format::__arg_t::__handle);
  }
#ifndef TEST_HAS_NO_INT128
  {
    constexpr size_t offset = store.__meta_data[23].__offset + 2 * sizeof(void*);
    static_assert(store.__meta_data[24].__offset == offset);
    static_assert(store.__meta_data[24].__type == std::__format::__arg_t::__i128);
    static_assert(store.__meta_data[25].__offset == offset + sizeof(__int128_t));
    static_assert(store.__meta_data[25].__type == std::__format::__arg_t::__u128);
  }
#endif

  // Test the contents of the data retrieving them from the basic_format_args.
  auto args = std::basic_format_args<context>(store);
  test_equal(args.get(0), true);
  test_equal(args.get(1), CharT('A'));
  test_equal(args.get(2), int(-1));
  test_equal(args.get(3), int(-42));
  test_equal(args.get(4), int(-99));
  if (sizeof(long) == sizeof(int))
    test_equal(args.get(5), int(-200));
  else
    test_equal(args.get(5), ll(-200));
  test_equal(args.get(6), ll(-457));
  test_equal(args.get(7), unsigned(1));
  test_equal(args.get(8), unsigned(42));
  test_equal(args.get(9), unsigned(99));
  if (sizeof(long) == sizeof(int))
    test_equal(args.get(10), unsigned(200));
  else
    test_equal(args.get(10), ull(200));
  test_equal(args.get(11), ull(457));
  test_equal(args.get(12), float(-1));
  test_equal(args.get(13), double(42));
  test_equal(args.get(14), ld(-99));
  test_equal(args.get(15), (const CharT*)(cstring));
  test_equal(args.get(16), const_cstring);
  test_equal(args.get(17), std::basic_string_view<CharT>{s});
  test_equal(args.get(18), sv);
  test_equal(args.get(19), cv(nullptr));
  test_equal(args.get(20), cv(nullptr));
  test_equal(args.get(21), cv(cstring));
  test_equal(args.get(22), cv(const_cstring));
  // 23 handle This type can't be inspected, tests for format functions validate this.
#ifndef TEST_HAS_NO_INT128
  test_equal(args.get(24), __int128_t(-9999));
  test_equal(args.get(25), __uint128_t(9999));
#endif
}

template <class CharT>
void test_store_empty() {
  auto store = std::make_format_args();
  static_assert((std::same_as<decltype(store.__meta_data), const std::array<std::__format::__arg_meta_data, 0>>));
  static_assert(sizeof(store.__data) == 1);
}

// Like test_store<CharT>(), but this tests the widening of a char to wchar_t
void test_store_char_to_wchar() {
#ifndef TEST_HAS_NO_WIDE_CHARACTERS
  auto store = std::make_format_args<std::wformat_context>('a');

  static_assert((std::same_as<decltype(store.__meta_data), const std::array<std::__format::__arg_meta_data, 1>>));
  static_assert(store.__meta_data[0].__offset == 0);
  static_assert(store.__meta_data[0].__type == std::__format::__arg_t::__char_type);

  static_assert(sizeof(store.__data) == sizeof(wchar_t));

  auto args = std::basic_format_args<std::wformat_context>(store);
  test_equal(args.get(0), L'a');
#endif
}

template <class CharT>
void test_store_char_string_types() {
  using context = std::basic_format_context<std::back_insert_iterator<std::basic_string<CharT>>, CharT>;

#define VALIDATE                                                                                                       \
  static_assert((std::same_as<decltype(store.__meta_data), const std::array<std::__format::__arg_meta_data, 1>>));     \
  static_assert(sizeof(store.__data) == sizeof(std::basic_string_view<CharT>));                                        \
                                                                                                                       \
  auto args = std::basic_format_args<context>(store);                                                                  \
  test_equal(args.get(0), std::basic_string_view<CharT>{cs});

  const CharT* cs = CSTR("hello world");
  {
    std::basic_string_view<CharT, constexpr_char_traits<CharT>> sv{cs};
    auto store = std::make_format_args<context>(sv);
    VALIDATE
  }
  {
    std::basic_string<CharT, constexpr_char_traits<CharT>, std::allocator<CharT>> s{cs};
    auto store = std::make_format_args<context>(s);
    VALIDATE
  }
  {
    std::basic_string<CharT, std::char_traits<CharT>, min_allocator<CharT>> s{cs};
    auto store = std::make_format_args<context>(s);
    VALIDATE
  }
  {
    std::basic_string<CharT, constexpr_char_traits<CharT>, min_allocator<CharT>> s{cs};
    auto store = std::make_format_args<context>(s);
    VALIDATE
  }

#undef VALIDATE
}

template <class CharT>
void test() {
  test_store<CharT>();
  test_store_empty<CharT>();
  test_store_char_to_wchar();
  test_store_char_string_types<CharT>();
}

// Tests whether the storage type uses the proper type deduction and conversion rules.
namespace test_storage_type {
using namespace std::__format;

template <class Context, class Tp>
concept invalid = !requires {
  __make_storage_type<Context, Tp>();
};

template <class Context>
consteval bool type_independant() {
  static_assert(std::same_as<decltype(__make_storage_type<Context, bool>()), __storage_type<bool>>);
  static_assert(std::same_as<decltype(__make_storage_type<Context, signed char>()), __storage_type<int>>);
  static_assert(std::same_as<decltype(__make_storage_type<Context, short>()), __storage_type<int>>);
  static_assert(std::same_as<decltype(__make_storage_type<Context, int>()), __storage_type<int>>);
  if constexpr (sizeof(long) == sizeof(int))
    static_assert(std::same_as<decltype(__make_storage_type<Context, long>()), __storage_type<int>>);
  else
    static_assert(std::same_as<decltype(__make_storage_type<Context, long>()), __storage_type<long long>>);
  static_assert(std::same_as<decltype(__make_storage_type<Context, long long>()), __storage_type<long long>>);
#ifndef TEST_HAS_NO_INT128
  static_assert(std::same_as<decltype(__make_storage_type<Context, __int128_t>()), __storage_type<__int128_t>>);
#endif

  static_assert(std::same_as<decltype(__make_storage_type<Context, unsigned char>()), __storage_type<unsigned>>);
  static_assert(std::same_as<decltype(__make_storage_type<Context, unsigned short>()), __storage_type<unsigned>>);
  static_assert(std::same_as<decltype(__make_storage_type<Context, unsigned>()), __storage_type<unsigned>>);
  if constexpr (sizeof(long) == sizeof(int))
    static_assert(std::same_as<decltype(__make_storage_type<Context, unsigned long>()), __storage_type<unsigned>>);
  else
    static_assert(
        std::same_as<decltype(__make_storage_type<Context, unsigned long>()), __storage_type<unsigned long long>>);
  static_assert(
      std::same_as<decltype(__make_storage_type<Context, unsigned long long>()), __storage_type<unsigned long long>>);
#ifndef TEST_HAS_NO_INT128
  static_assert(std::same_as<decltype(__make_storage_type<Context, __uint128_t>()), __storage_type<__uint128_t>>);
#endif

  static_assert(std::same_as<decltype(__make_storage_type<Context, float>()), __storage_type<float>>);
  static_assert(std::same_as<decltype(__make_storage_type<Context, double>()), __storage_type<double>>);
  static_assert(std::same_as<decltype(__make_storage_type<Context, long double>()), __storage_type<long double>>);

  static_assert(std::same_as<decltype(__make_storage_type<Context, nullptr_t>()), __storage_type<const void*>>);
  static_assert(std::same_as<decltype(__make_storage_type<Context, void*>()), __storage_type<const void*>>);
  static_assert(std::same_as<decltype(__make_storage_type<Context, const void*>()), __storage_type<const void*>>);

  static_assert(std::same_as<decltype(__make_storage_type<Context, status>()),
                             __storage_type<typename std::basic_format_arg<Context>::handle>>);

  return true;
}

// char types
static_assert(type_independant<std::format_context>());
static_assert(std::same_as<decltype(__make_storage_type<std::format_context, char>()), __storage_type<char>>);
static_assert(std::same_as<decltype(__make_storage_type<std::format_context, char*>()), __storage_type<const char*>>);
static_assert(
    std::same_as<decltype(__make_storage_type<std::format_context, const char*>()), __storage_type<const char*>>);
static_assert(std::same_as<decltype(__make_storage_type<std::format_context, char[42]>()),
                           __storage_type<std::string_view>>); // LWG-XXX
//static_assert(std::same_as<decltype(__make_storage_type<std::format_context, const char[42]>()),
//                           __storage_type<std::string_view>>);
static_assert(std::same_as<decltype(__make_storage_type<std::format_context, std::string_view>()),
                           __storage_type<std::string_view>>);
static_assert(
    std::same_as<decltype(__make_storage_type<std::format_context, std::string>()), __storage_type<std::string_view>>);

#ifndef TEST_HAS_NO_WIDE_CHARACTERS
// wchar_t types
static_assert(type_independant<std::wformat_context>());
static_assert(std::same_as<decltype(__make_storage_type<std::wformat_context, wchar_t>()), __storage_type<wchar_t>>);
static_assert(
    std::same_as<decltype(__make_storage_type<std::wformat_context, wchar_t*>()), __storage_type<const wchar_t*>>);
static_assert(std::same_as<decltype(__make_storage_type<std::wformat_context, const wchar_t*>()),
                           __storage_type<const wchar_t*>>);
static_assert(std::same_as<decltype(__make_storage_type<std::wformat_context, wchar_t[42]>()),
                           __storage_type<std::wstring_view>>); // LWG-XXX
//static_assert(std::same_as<decltype(__make_storage_type<std::wformat_context, const wchar_t[42]>()),
//                           __storage_type<std::wstring_view>>);
static_assert(std::same_as<decltype(__make_storage_type<std::wformat_context, std::wstring_view>()),
                           __storage_type<std::wstring_view>>);
static_assert(std::same_as<decltype(__make_storage_type<std::wformat_context, std::wstring>()),
                           __storage_type<std::wstring_view>>);

// wchar_t to char narrowing
static_assert(invalid<std::format_context, wchar_t>);
static_assert(invalid<std::format_context, wchar_t*>);
static_assert(invalid<std::format_context, const wchar_t*>);
static_assert(invalid<std::format_context, wchar_t[42]>);
static_assert(invalid<std::format_context, const wchar_t[42]>);
static_assert(invalid<std::format_context, std::wstring_view>);
static_assert(invalid<std::format_context, std::wstring>);

// char to wchar_t expansion
static_assert(std::same_as<decltype(__make_storage_type<std::wformat_context, char>()), __storage_type<wchar_t>>);
static_assert(invalid<std::wformat_context, char*>);
static_assert(invalid<std::wformat_context, const char*>);
static_assert(invalid<std::wformat_context, char[42]>);
static_assert(invalid<std::wformat_context, const char[42]>);
static_assert(invalid<std::wformat_context, std::string_view>);
static_assert(invalid<std::wformat_context, std::string>);

#endif

} // namespace test_storage_type

int main(int, char**) {
  test<char>();
#ifndef TEST_HAS_NO_WIDE_CHARACTERS
  test<wchar_t>();
#endif

  return 0;
}
