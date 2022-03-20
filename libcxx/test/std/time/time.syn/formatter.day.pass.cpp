//===----------------------------------------------------------------------===//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17
// UNSUPPORTED: libcpp-has-no-localization
// UNSUPPORTED: libcpp-has-no-incomplete-format

// REQUIRES: locale.fr_FR.UTF-8
// REQUIRES: locale.ja_JP.UTF-8

// <chrono>
//
// template<class charT> struct formatter<chrono::day, charT>;

#include <chrono>
#include <format>

#include <cassert>
#include <concepts>
#include <locale>
#include <iostream>
#include <type_traits>

#include "test_macros.h"

#include "make_string.h"
#include "platform_support.h" // locale name macros
#include "test_macros.h"

// XXX Add a validate check for compile-time errors

#define SV(S) MAKE_STRING_VIEW(CharT, S)

// XXX This will be needed after P2216 and P2418
#ifndef TEST_HAS_NO_WIDE_CHARACTERS
template <class>
using context = std::format_context;
#else
template <class CharT>
using context = std::conditional_t<std::same_as<CharT, char>, std::format_context, std::wformat_context>;
#endif

// XXX use string literal... can we use statically_widen? Not since internal type

template <class CharT, class... Args>
static void check(std::basic_string_view<CharT> fmt, std::basic_string_view<CharT> expected, const Args&... args) {
  std::basic_string<CharT> out = std::format(fmt, args...);
  if constexpr (std::same_as<CharT, char>)
    if (out != expected)
      std::cerr << "\nFormat string   " << fmt << "\nExpected output " << expected << "\nActual output   " << out
                << '\n';
  assert(out == expected);
}

template <class CharT, class... Args>
static void check(const std::locale& loc, std::basic_string_view<CharT> fmt, std::basic_string_view<CharT> expected,
                  const Args&... args) {
  std::basic_string<CharT> out = std::format(loc, fmt, args...);
  if constexpr (std::same_as<CharT, char>)
    if (out != expected)
      std::cerr << "\nFormat string   " << fmt << "\nExpected output " << expected << "\nActual output   " << out
                << '\n';
  assert(out == expected);
}

template <class CharT, class... Args>
static void check_exception([[maybe_unused]] std::basic_string_view<CharT> fmt,
                            [[maybe_unused]] std::basic_string_view<char> what, [[maybe_unused]] const Args&... args) {
#ifndef TEST_HAS_NO_EXCEPTIONS
  try {
    std::format(fmt, args...);
#  ifndef TEST_HAS_NO_LOCALIZATION
    if constexpr (std::same_as<CharT, char>)
      std::cerr << "\nFormat string   " << fmt << "\nDidn't throw an exception.\n";
#  endif
    assert(false);
  } catch (const std::format_error& e) {
    if constexpr (std::same_as<CharT, char>)
#  if defined(_LIBCPP_VERSION) && !defined(TEST_HAS_NO_LOCALIZATION)
      if (e.what() != what)
        std::cerr << "\nFormat string   " << fmt << "\nExpected exception " << what << "\nActual exception   "
                  << e.what() << '\n';
#  endif
    LIBCPP_ASSERT(e.what() == what);
    return;
  }
  assert(false);
#endif
};

template <class CharT>
static void test_no_chrono_specs() {
  using namespace std::literals::chrono_literals;

  // Valid day
  check(SV("{}"), SV("01"), 1d);
  check(SV("{:*^4}"), SV("*01*"), 1d);
  check(SV("{:*>3%d}"), SV("*01"), 1d);

  // Invalid day
  check(SV("{}"), SV("00 is not a valid day"), 0d);
  check(SV("{:*^23}"), SV("*00 is not a valid day*"), 0d);
}

template <class CharT>
static void test_valid_values() {
  using namespace std::literals::chrono_literals;

  std::basic_string_view<CharT> fmt = SV("{:%%d='%d'%t%%Od='%Od'%t%%e='%e'%t%%Oe='%Oe'%n}");
  std::basic_string_view<CharT> lfmt = SV("{:L%%d='%d'%t%%Od='%Od'%t%%e='%e'%t%%Oe='%Oe'%n}");

  const std::locale loc(LOCALE_ja_JP_UTF_8);
  std::locale::global(std::locale(LOCALE_fr_FR_UTF_8));

  // Non localized output using C-locale
  check(fmt, SV("%d='00'\t%Od='00'\t%e=' 0'\t%Oe=' 0'\n"), 0d);
  check(fmt, SV("%d='01'\t%Od='01'\t%e=' 1'\t%Oe=' 1'\n"), 1d);
  check(fmt, SV("%d='31'\t%Od='31'\t%e='31'\t%Oe='31'\n"), 31d);
  check(fmt, SV("%d='255'\t%Od='255'\t%e='255'\t%Oe='255'\n"), 255d);

  // Use the global locale (fr_FR)
  check(lfmt, SV("%d='00'\t%Od='00'\t%e=' 0'\t%Oe=' 0'\n"), 0d);
  check(lfmt, SV("%d='01'\t%Od='01'\t%e=' 1'\t%Oe=' 1'\n"), 1d);
  check(lfmt, SV("%d='31'\t%Od='31'\t%e='31'\t%Oe='31'\n"), 31d);
  check(lfmt, SV("%d='255'\t%Od='255'\t%e='255'\t%Oe='255'\n"), 255d);

  // Use supplied locale (ja_JP). This locale has a different alternate.
  check(loc, lfmt, SV("%d='00'\t%Od='〇'\t%e=' 0'\t%Oe='〇'\n"), 0d);
  check(loc, lfmt, SV("%d='01'\t%Od='一'\t%e=' 1'\t%Oe='一'\n"), 1d);
  check(loc, lfmt, SV("%d='31'\t%Od='三十一'\t%e='31'\t%Oe='三十一'\n"), 31d);
  check(loc, lfmt, SV("%d='255'\t%Od='255'\t%e='255'\t%Oe='255'\n"), 255d);

  std::locale::global(std::locale::classic());
}

template <class CharT>
static void test_invalid_types() {
  const std::chrono::day arg{0};

  // *** No modifier
  check_exception(SV("{:%a}"), "The supplied date time doesn't contain a weekday", arg);
  check_exception(SV("{:%A}"), "The supplied date time doesn't contain a weekday", arg);
  check_exception(SV("{:%b}"), "The supplied date time doesn't contain a month", arg);
  check_exception(SV("{:%B}"), "The supplied date time doesn't contain a month", arg);
  check_exception(SV("{:%c}"), "The supplied date time doesn't contain a date and time", arg);
  check_exception(SV("{:%C}"), "The supplied date time doesn't contain a year", arg);
  // d - valid
  check_exception(SV("{:%D}"), "The supplied date time doesn't contain a date", arg);
  // e - valid
  // E - the modifier is checked separately
  check_exception(SV("{:%f}"), "The date time type specifier is invalid", arg);
  check_exception(SV("{:%F}"), "The supplied date time doesn't contain a date", arg);
  check_exception(SV("{:%g}"), "The supplied date time doesn't contain a date", arg);
  check_exception(SV("{:%G}"), "The supplied date time doesn't contain a date", arg);
  check_exception(SV("{:%h}"), "The supplied date time doesn't contain a month", arg);
  check_exception(SV("{:%H}"), "The supplied date time doesn't contain a hour", arg);
  check_exception(SV("{:%i}"), "The date time type specifier is invalid", arg);
  check_exception(SV("{:%I}"), "The supplied date time doesn't contain a hour", arg);
  check_exception(SV("{:%j}"), "The supplied date time doesn't contain a date or duration", arg);
  check_exception(SV("{:%J}"), "The date time type specifier is invalid", arg);
  check_exception(SV("{:%k}"), "The date time type specifier is invalid", arg);
  check_exception(SV("{:%K}"), "The date time type specifier is invalid", arg);
  check_exception(SV("{:%l}"), "The date time type specifier is invalid", arg);
  check_exception(SV("{:%L}"), "The date time type specifier is invalid", arg);
  check_exception(SV("{:%m}"), "The supplied date time doesn't contain a month", arg);
  check_exception(SV("{:%M}"), "The supplied date time doesn't contain a minute", arg);
  // n - valid
  check_exception(SV("{:%N}"), "The date time type specifier is invalid", arg);
  check_exception(SV("{:%o}"), "The date time type specifier is invalid", arg);
  // O - the modifier is checked separately
  check_exception(SV("{:%p}"), "The supplied date time doesn't contain a hour", arg);
  check_exception(SV("{:%P}"), "The date time type specifier is invalid", arg);
  check_exception(SV("{:%q}"), "The supplied date time doesn't contain a duration", arg);
  check_exception(SV("{:%Q}"), "The supplied date time doesn't contain a duration", arg);
  check_exception(SV("{:%r}"), "The supplied date time doesn't contain a time", arg);
  check_exception(SV("{:%R}"), "The supplied date time doesn't contain a time", arg);
  check_exception(SV("{:%s}"), "The date time type specifier is invalid", arg);
  check_exception(SV("{:%S}"), "The supplied date time doesn't contain a second", arg);
  // t - valid
  check_exception(SV("{:%T}"), "The supplied date time doesn't contain a time", arg);
  check_exception(SV("{:%u}"), "The supplied date time doesn't contain a weekday", arg);
  check_exception(SV("{:%U}"), "The supplied date time doesn't contain a date", arg);
  check_exception(SV("{:%v}"), "The date time type specifier is invalid", arg);
  check_exception(SV("{:%V}"), "The supplied date time doesn't contain a date", arg);
  check_exception(SV("{:%w}"), "The supplied date time doesn't contain a weekday", arg);
  check_exception(SV("{:%W}"), "The supplied date time doesn't contain a date", arg);
  check_exception(SV("{:%x}"), "The supplied date time doesn't contain a date", arg);
  check_exception(SV("{:%X}"), "The supplied date time doesn't contain a time", arg);
  check_exception(SV("{:%y}"), "The supplied date time doesn't contain a year", arg);
  check_exception(SV("{:%Y}"), "The supplied date time doesn't contain a year", arg);
  check_exception(SV("{:%z}"), "The supplied date time doesn't contain a time zone", arg);
  check_exception(SV("{:%Z}"), "The supplied date time doesn't contain a time zone", arg);

  // *** E modifier
  check_exception(SV("{:%Ea}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%EA}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%Eb}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%EB}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%Ec}"), "The supplied date time doesn't contain a date and time", arg);
  check_exception(SV("{:%EC}"), "The supplied date time doesn't contain a year", arg);
  check_exception(SV("{:%Ed}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%ED}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%Ee}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%EE}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%Ef}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%EF}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%Eg}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%EG}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%Eh}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%EH}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%Ei}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%EI}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%Ej}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%EJ}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%Ek}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%EK}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%El}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%EL}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%Em}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%EM}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%En}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%EN}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%Eo}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%EO}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%Ep}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%EP}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%Eq}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%EQ}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%Er}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%ER}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%Es}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%ES}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%Et}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%ET}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%Eu}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%EU}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%Ev}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%EV}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%Ew}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%EW}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%Ex}"), "The supplied date time doesn't contain a date", arg);
  check_exception(SV("{:%EX}"), "The supplied date time doesn't contain a time", arg);
  check_exception(SV("{:%Ey}"), "The supplied date time doesn't contain a year", arg);
  check_exception(SV("{:%EY}"), "The supplied date time doesn't contain a year", arg);
  check_exception(SV("{:%Ez}"), "The supplied date time doesn't contain a time zone", arg);
  check_exception(SV("{:%EZ}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%E%}"), "The date time type specifier for modifier E is invalid", arg);

  // *** O modifier
  check_exception(SV("{:%Oa}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OA}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%Ob}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OB}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%Oc}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OC}"), "The date time type specifier for modifier O is invalid", arg);
  // Od - valid
  check_exception(SV("{:%OD}"), "The date time type specifier for modifier O is invalid", arg);
  // Oe - valid
  check_exception(SV("{:%OE}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%Of}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OF}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%Og}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OG}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%Oh}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OH}"), "The supplied date time doesn't contain a hour", arg);
  check_exception(SV("{:%Oi}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OI}"), "The supplied date time doesn't contain a hour", arg);
  check_exception(SV("{:%Oj}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OJ}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%Ok}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OK}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%Ol}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OL}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%Om}"), "The supplied date time doesn't contain a month", arg);
  check_exception(SV("{:%OM}"), "The supplied date time doesn't contain a minute", arg);
  check_exception(SV("{:%On}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%ON}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%Oo}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OO}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%Op}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OP}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%Oq}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OQ}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%Or}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OR}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%Os}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OS}"), "The supplied date time doesn't contain a second", arg);
  check_exception(SV("{:%Ot}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OT}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%Ou}"), "The supplied date time doesn't contain a weekday", arg);
  check_exception(SV("{:%OU}"), "The supplied date time doesn't contain a date", arg);
  check_exception(SV("{:%Ov}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OV}"), "The supplied date time doesn't contain a date", arg);
  check_exception(SV("{:%Ow}"), "The supplied date time doesn't contain a weekday", arg);
  check_exception(SV("{:%OW}"), "The supplied date time doesn't contain a date", arg);
  check_exception(SV("{:%Ox}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OX}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%Oy}"), "The supplied date time doesn't contain a year", arg);
  check_exception(SV("{:%OY}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%Oz}"), "The supplied date time doesn't contain a time zone", arg);
  check_exception(SV("{:%OZ}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%O%}"), "The date time type specifier for modifier O is invalid", arg);
}

template <class CharT>
static void test() {
  using namespace std::literals::chrono_literals;

  // XXX test with :%% no output.... intended??

  test_no_chrono_specs<CharT>();
  test_valid_values<CharT>();
  test_invalid_types<CharT>();

  check_exception(SV("{:A"), "The chrono-specs should start with a '%' or '}'", 0d);
  check_exception(SV("{:%%{"), "The chrono-specs contains a '{'", 0d);
  check_exception(SV("{:%"), "End of input while parsing the modifier chrono conversion-spec", 0d);
  check_exception(SV("{:%E"), "End of input while parsing the modifier E", 0d);
  check_exception(SV("{:%O"), "End of input while parsing the modifier O", 0d);

  // Precision not allowed
  check_exception(SV("{:.3}"), "The chrono-specs should start with a '%' or '}'",
                  0d); // XXX Message not entirely accurate } means not specs
}

int main(int, char**) {
  test<char>();

#ifndef TEST_HAS_NO_WIDE_CHARACTERS
  //  format_tests_char_to_wchar_t(test);
  test<wchar_t>();
#endif

  return 0;
}
