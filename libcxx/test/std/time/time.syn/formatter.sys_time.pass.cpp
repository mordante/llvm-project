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

#if 0
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
  check_exception(SV("{:%d}"), "The supplied date time doesn't contain a day", arg);
  check_exception(SV("{:%D}"), "The supplied date time doesn't contain a date", arg);
  check_exception(SV("{:%e}"), "The supplied date time doesn't contain a day", arg);
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
  // n - valid
  check_exception(SV("{:%M}"), "The supplied date time doesn't contain a minute", arg);
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
  check_exception(SV("{:%Od}"), "The supplied date time doesn't contain a day", arg);
  check_exception(SV("{:%OD}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%Oe}"), "The supplied date time doesn't contain a day", arg);
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
#endif

#define SV(S) MAKE_STRING_VIEW(CharT, S)

// XXX This will be needed after P2216 and P2418
#ifndef TEST_HAS_NO_WIDE_CHARACTERS
template <class>
using context = std::format_context;
#else
template <class CharT>
using context = std::conditional_t<std::same_as<CharT, char>, std::format_context, std::wformat_context>;
#endif

// XXX use string literal

// XXX THESE CHECK FUNCTIONS NOT HAVE 3 COPIES, MOVE TO HEADER
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

  // [time.clock.system.nonmembers]/3
  check(SV("{}"), SV("1970-01-01 00:00:00"), std::chrono::sys_seconds(0s));
  check(SV("{}"), SV("2000-01-01 00:00:00"), std::chrono::sys_seconds(946'684'800s));
  check(SV("{}"), SV("2000-01-01 01:02:03"), std::chrono::sys_seconds(946'688'523s));
}

template <class CharT>
static void test_valid_values_year() {
  using namespace std::literals::chrono_literals;

  std::basic_string_view<CharT> fmt =
      SV("{:%%C='%C'%t%%EC='%EC'%t%%y='%y'%t%%Oy='%Oy'%t%%Ey='%Ey'%t%%Y='%Y'%t%%EY='%EY'%n}");
  std::basic_string_view<CharT> lfmt =
      SV("{:L%%C='%C'%t%%EC='%EC'%t%%y='%y'%t%%Oy='%Oy'%t%%Ey='%Ey'%t%%Y='%Y'%t%%EY='%EY'%n}");

  const std::locale loc(LOCALE_ja_JP_UTF_8);
  std::locale::global(std::locale(LOCALE_fr_FR_UTF_8));

  // Non localized output using C-locale
  check(fmt, SV("%C='19'\t%EC='19'\t%y='70'\t%Oy='70'\t%Ey='70'\t%Y='1970'\t%EY='1970'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(fmt, SV("%C='20'\t%EC='20'\t%y='09'\t%Oy='09'\t%Ey='09'\t%Y='2009'\t%EY='2009'\n"),
        std::chrono::sys_seconds(1'234'567'890s)); // 23:31:30 UTC on Friday, 13 February 2009

  // Use the global locale (fr_FR)
  check(lfmt, SV("%C='19'\t%EC='19'\t%y='70'\t%Oy='70'\t%Ey='70'\t%Y='1970'\t%EY='1970'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(lfmt, SV("%C='20'\t%EC='20'\t%y='09'\t%Oy='09'\t%Ey='09'\t%Y='2009'\t%EY='2009'\n"),
        std::chrono::sys_seconds(1'234'567'890s)); // 23:31:30 UTC on Friday, 13 February 2009

  // Use supplied locale (ja_JP). This locale has a different alternate.
  check(loc, lfmt, SV("%C='19'\t%EC='昭和'\t%y='70'\t%Oy='七十'\t%Ey='45'\t%Y='1970'\t%EY='昭和45年'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(loc, lfmt, SV("%C='20'\t%EC='平成'\t%y='09'\t%Oy='九'\t%Ey='21'\t%Y='2009'\t%EY='平成21年'\n"),
        std::chrono::sys_seconds(1'234'567'890s)); // 23:31:30 UTC on Friday, 13 February 2009

  std::locale::global(std::locale::classic());
}

template <class CharT>
static void test_valid_values_month() {
  using namespace std::literals::chrono_literals;

  std::basic_string_view<CharT> fmt = SV("{:%%b='%b'%t%%h='%h'%t%%B='%B'%t%%m='%m'%t%%Om='%Om'%n}");
  std::basic_string_view<CharT> lfmt = SV("{:L%%b='%b'%t%%h='%h'%t%%B='%B'%t%%m='%m'%t%%Om='%Om'%n}");

  const std::locale loc(LOCALE_ja_JP_UTF_8);
  std::locale::global(std::locale(LOCALE_fr_FR_UTF_8));

  // Non localized output using C-locale
  check(fmt, SV("%b='Jan'\t%h='Jan'\t%B='January'\t%m='01'\t%Om='01'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(fmt, SV("%b='May'\t%h='May'\t%B='May'\t%m='05'\t%Om='05'\n"),
        std::chrono::sys_seconds(2'000'000'000s)); // 03:33:20 UTC on Wednesday, 18 May 2033

  // Use the global locale (fr_FR)
  check(lfmt, SV("%b='janv.'\t%h='janv.'\t%B='janvier'\t%m='01'\t%Om='01'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(lfmt, SV("%b='mai'\t%h='mai'\t%B='mai'\t%m='05'\t%Om='05'\n"),
        std::chrono::sys_seconds(2'000'000'000s)); // 03:33:20 UTC on Wednesday, 18 May 2033

  // Use supplied locale (ja_JP). This locale has a different alternate.
  check(loc, lfmt, SV("%b=' 1月'\t%h=' 1月'\t%B='1月'\t%m='01'\t%Om='一'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(loc, lfmt, SV("%b=' 5月'\t%h=' 5月'\t%B='5月'\t%m='05'\t%Om='五'\n"),
        std::chrono::sys_seconds(2'000'000'000s)); // 03:33:20 UTC on Wednesday, 18 May 2033

  std::locale::global(std::locale::classic());
}

template <class CharT>
static void test_valid_values_day() {
  using namespace std::literals::chrono_literals;

  std::basic_string_view<CharT> fmt = SV("{:%%d='%d'%t%%Od='%Od'%t%%e='%e'%t%%Oe='%Oe'%n}");
  std::basic_string_view<CharT> lfmt = SV("{:L%%d='%d'%t%%Od='%Od'%t%%e='%e'%t%%Oe='%Oe'%n}");

  const std::locale loc(LOCALE_ja_JP_UTF_8);
  std::locale::global(std::locale(LOCALE_fr_FR_UTF_8));

  // Non localized output using C-locale
  check(fmt, SV("%d='01'\t%Od='01'\t%e=' 1'\t%Oe=' 1'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(fmt, SV("%d='13'\t%Od='13'\t%e='13'\t%Oe='13'\n"),
        std::chrono::sys_seconds(1'234'567'890s)); // 23:31:30 UTC on Friday, 13 February 2009

  // Use the global locale (fr_FR)
  check(lfmt, SV("%d='01'\t%Od='01'\t%e=' 1'\t%Oe=' 1'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(lfmt, SV("%d='13'\t%Od='13'\t%e='13'\t%Oe='13'\n"),
        std::chrono::sys_seconds(1'234'567'890s)); // 23:31:30 UTC on Friday, 13 February 2009

  // Use supplied locale (ja_JP). This locale has a different alternate.
  check(loc, lfmt, SV("%d='01'\t%Od='一'\t%e=' 1'\t%Oe='一'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(loc, lfmt, SV("%d='13'\t%Od='十三'\t%e='13'\t%Oe='十三'\n"),
        std::chrono::sys_seconds(1'234'567'890s)); // 23:31:30 UTC on Friday, 13 February 2009

  std::locale::global(std::locale::classic());
}

template <class CharT>
static void test_valid_values_weekday() {
  using namespace std::literals::chrono_literals;

  std::basic_string_view<CharT> fmt = SV("{:%%a='%a'%t%%A='%A'%t%%u='%u'%t%%Ou='%Ou'%t%%w='%w'%t%%Ow='%Ow'%n}");
  std::basic_string_view<CharT> lfmt = SV("{:L%%a='%a'%t%%A='%A'%t%%u='%u'%t%%Ou='%Ou'%t%%w='%w'%t%%Ow='%Ow'%n}");

  const std::locale loc(LOCALE_ja_JP_UTF_8);
  std::locale::global(std::locale(LOCALE_fr_FR_UTF_8));

  // Non localized output using C-locale
  check(fmt, SV("%a='Thu'\t%A='Thursday'\t%u='4'\t%Ou='4'\t%w='4'\t%Ow='4'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(fmt, SV("%a='Sun'\t%A='Sunday'\t%u='7'\t%Ou='7'\t%w='0'\t%Ow='0'\n"),
        std::chrono::sys_seconds(4'294'967'295s)); // 06:28:15 UTC on Sunday, 7 February 2106

  // Use the global locale (fr_FR)
  check(lfmt, SV("%a='jeu.'\t%A='jeudi'\t%u='4'\t%Ou='4'\t%w='4'\t%Ow='4'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(lfmt, SV("%a='dim.'\t%A='dimanche'\t%u='7'\t%Ou='7'\t%w='0'\t%Ow='0'\n"),
        std::chrono::sys_seconds(4'294'967'295s)); // 06:28:15 UTC on Sunday, 7 February 2106

  // Use supplied locale (ja_JP). This locale has a different alternate.
  check(loc, lfmt, SV("%a='木'\t%A='木曜日'\t%u='4'\t%Ou='四'\t%w='4'\t%Ow='四'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(loc, lfmt, SV("%a='日'\t%A='日曜日'\t%u='7'\t%Ou='七'\t%w='0'\t%Ow='〇'\n"),
        std::chrono::sys_seconds(4'294'967'295s)); // 06:28:15 UTC on Sunday, 7 February 2106

  std::locale::global(std::locale::classic());
}

template <class CharT>
static void test_valid_values_day_of_year() {
  using namespace std::literals::chrono_literals;

  std::basic_string_view<CharT> fmt = SV("{:%%j='%j'%n}");
  std::basic_string_view<CharT> lfmt = SV("{:L%%j='%j'%n}");

  const std::locale loc(LOCALE_ja_JP_UTF_8);
  std::locale::global(std::locale(LOCALE_fr_FR_UTF_8));

  // Non localized output using C-locale
  check(fmt, SV("%j='001'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(fmt, SV("%j='138'\n"),
        std::chrono::sys_seconds(2'000'000'000s)); // 03:33:20 UTC on Wednesday, 18 May 2033

  // Use the global locale (fr_FR)
  check(lfmt, SV("%j='001'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(lfmt, SV("%j='138'\n"),
        std::chrono::sys_seconds(2'000'000'000s)); // 03:33:20 UTC on Wednesday, 18 May 2033

  // Use supplied locale (ja_JP). This locale has a different alternate.
  check(loc, lfmt, SV("%j='001'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(loc, lfmt, SV("%j='138'\n"),
        std::chrono::sys_seconds(2'000'000'000s)); // 03:33:20 UTC on Wednesday, 18 May 2033

  std::locale::global(std::locale::classic());
}

template <class CharT>
static void test_valid_values_week() {
  using namespace std::literals::chrono_literals;

  std::basic_string_view<CharT> fmt = SV("{:%%U='%U'%t%%OU='%OU'%t%%W='%W'%t%%OW='%OW'%n}");
  std::basic_string_view<CharT> lfmt = SV("{:L%%U='%U'%t%%OU='%OU'%t%%W='%W'%t%%OW='%OW'%n}");

  const std::locale loc(LOCALE_ja_JP_UTF_8);
  std::locale::global(std::locale(LOCALE_fr_FR_UTF_8));

  // Non localized output using C-locale
  check(fmt, SV("%U='00'\t%OU='00'\t%W='00'\t%OW='00'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(fmt, SV("%U='20'\t%OU='20'\t%W='20'\t%OW='20'\n"),
        std::chrono::sys_seconds(2'000'000'000s)); // 03:33:20 UTC on Wednesday, 18 May 2033

  // Use the global locale (fr_FR)
  check(lfmt, SV("%U='00'\t%OU='00'\t%W='00'\t%OW='00'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(lfmt, SV("%U='20'\t%OU='20'\t%W='20'\t%OW='20'\n"),
        std::chrono::sys_seconds(2'000'000'000s)); // 03:33:20 UTC on Wednesday, 18 May 2033

  // Use supplied locale (ja_JP). This locale has a different alternate.
  check(loc, lfmt, SV("%U='00'\t%OU='〇'\t%W='00'\t%OW='〇'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(loc, lfmt, SV("%U='20'\t%OU='二十'\t%W='20'\t%OW='二十'\n"),
        std::chrono::sys_seconds(2'000'000'000s)); // 03:33:20 UTC on Wednesday, 18 May 2033

  std::locale::global(std::locale::classic());
}

template <class CharT>
static void test_valid_values_iso_8601_week() {
  using namespace std::literals::chrono_literals;

  std::basic_string_view<CharT> fmt = SV("{:%%g='%g'%t%%G='%G'%t%%V='%V'%t%%OV='%OV'%n}");
  std::basic_string_view<CharT> lfmt = SV("{:L%%g='%g'%t%%G='%G'%t%%V='%V'%t%%OV='%OV'%n}");

  const std::locale loc(LOCALE_ja_JP_UTF_8);
  std::locale::global(std::locale(LOCALE_fr_FR_UTF_8));

  // Non localized output using C-locale
  check(fmt, SV("%g='70'\t%G='1970'\t%V='01'\t%OV='01'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(fmt, SV("%g='09'\t%G='2009'\t%V='07'\t%OV='07'\n"),
        std::chrono::sys_seconds(1'234'567'890s)); // 23:31:30 UTC on Friday, 13 February 2009

  // Use the global locale (fr_FR)
  check(lfmt, SV("%g='70'\t%G='1970'\t%V='01'\t%OV='01'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(lfmt, SV("%g='09'\t%G='2009'\t%V='07'\t%OV='07'\n"),
        std::chrono::sys_seconds(1'234'567'890s)); // 23:31:30 UTC on Friday, 13 February 2009

  // Use supplied locale (ja_JP). This locale has a different alternate.
  check(loc, lfmt, SV("%g='70'\t%G='1970'\t%V='01'\t%OV='一'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(loc, lfmt, SV("%g='09'\t%G='2009'\t%V='07'\t%OV='七'\n"),
        std::chrono::sys_seconds(1'234'567'890s)); // 23:31:30 UTC on Friday, 13 February 2009

  std::locale::global(std::locale::classic());
}

template <class CharT>
static void test_valid_values_date() {
  using namespace std::literals::chrono_literals;

  std::basic_string_view<CharT> fmt = SV("{:%%D='%D'%t%%F='%F'%t%%x='%x'%t%%Ex='%Ex'%n}");
  std::basic_string_view<CharT> lfmt = SV("{:L%%D='%D'%t%%F='%F'%t%%x='%x'%t%%Ex='%Ex'%n}");

  const std::locale loc(LOCALE_ja_JP_UTF_8);
  std::locale::global(std::locale(LOCALE_fr_FR_UTF_8));

  // Non localized output using C-locale
  check(fmt, SV("%D='01/01/70'\t%F='1970-01-01'\t%x='01/01/70'\t%Ex='01/01/70'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(fmt, SV("%D='02/13/09'\t%F='2009-02-13'\t%x='02/13/09'\t%Ex='02/13/09'\n"),
        std::chrono::sys_seconds(1'234'567'890s)); // 23:31:30 UTC on Friday, 13 February 2009

  // Use the global locale (fr_FR)
  check(lfmt, SV("%D='01/01/70'\t%F='1970-01-01'\t%x='01/01/1970'\t%Ex='01/01/1970'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(lfmt, SV("%D='02/13/09'\t%F='2009-02-13'\t%x='13/02/2009'\t%Ex='13/02/2009'\n"),
        std::chrono::sys_seconds(1'234'567'890s)); // 23:31:30 UTC on Friday, 13 February 2009

  // Use supplied locale (ja_JP). This locale has a different alternate.a
  check(loc, lfmt, SV("%D='01/01/70'\t%F='1970-01-01'\t%x='1970年01月01日'\t%Ex='昭和45年01月01日'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(loc, lfmt, SV("%D='02/13/09'\t%F='2009-02-13'\t%x='2009年02月13日'\t%Ex='平成21年02月13日'\n"),
        std::chrono::sys_seconds(1'234'567'890s)); // 23:31:30 UTC on Friday, 13 February 2009

  std::locale::global(std::locale::classic());
}

template <class CharT>
static void test_valid_values_time() {
  using namespace std::literals::chrono_literals;

  std::basic_string_view<CharT> fmt = SV("{:"
                                         "%%H='%H'%t"
                                         "%%OH='%OH'%t"
                                         "%%I='%I'%t"
                                         "%%OI='%OI'%t"
                                         "%%M='%M'%t"
                                         "%%OM='%OM'%t"
                                         "%%S='%S'%t" // XXX SUB second?
                                         "%%OS='%OS'%t"
                                         "%%p='%p'%t"
                                         "%%R='%R'%t"
                                         "%%T='%T'%t"
                                         "%%r='%r'%t"
                                         "%%X='%X'%t"
                                         "%%EX='%EX'%t"
                                         "%n}");
  std::basic_string_view<CharT> lfmt = SV("{:L"
                                          "%%H='%H'%t"
                                          "%%OH='%OH'%t"
                                          "%%I='%I'%t"
                                          "%%OI='%OI'%t"
                                          "%%M='%M'%t"
                                          "%%OM='%OM'%t"
                                          "%%S='%S'%t" // XXX SUB second?
                                          "%%OS='%OS'%t"
                                          "%%p='%p'%t"
                                          "%%R='%R'%t"
                                          "%%T='%T'%t"
                                          "%%r='%r'%t"
                                          "%%X='%X'%t"
                                          "%%EX='%EX'%t"
                                          "%n}");

  const std::locale loc(LOCALE_ja_JP_UTF_8);
  std::locale::global(std::locale(LOCALE_fr_FR_UTF_8));

  // Non localized output using C-locale
  check(fmt,
        SV("%H='00'\t"
           "%OH='00'\t"
           "%I='12'\t"
           "%OI='12'\t"
           "%M='00'\t"
           "%OM='00'\t"
           "%S='00'\t"
           "%OS='00'\t"
           "%p='AM'\t"
           "%R='00:00'\t"
           "%T='00:00:00'\t"
           "%r='12:00:00 AM'\t"
           "%X='00:00:00'\t"
           "%EX='00:00:00'\t"
           "\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(fmt,
        SV("%H='23'\t"
           "%OH='23'\t"
           "%I='11'\t"
           "%OI='11'\t"
           "%M='31'\t"
           "%OM='31'\t"
           "%S='30'\t"
           "%OS='30'\t"
           "%p='PM'\t"
           "%R='23:31'\t"
           "%T='23:31:30'\t"
           "%r='11:31:30 PM'\t"
           "%X='23:31:30'\t"
           "%EX='23:31:30'\t"
           "\n"),

        std::chrono::sys_seconds(1'234'567'890s)); // 23:31:30 UTC on Friday, 13 February 2009

  // Use the global locale (fr_FR)
  check(lfmt,
        SV("%H='00'\t"
           "%OH='00'\t"
           "%I='12'\t"
           "%OI='12'\t"
           "%M='00'\t"
           "%OM='00'\t"
           "%S='00'\t"
           "%OS='00'\t"
           "%p=''\t" // Locale has no AM or PM.
           "%R='00:00'\t"
           "%T='00:00:00'\t"
           "%r='12:00:00 '\t"
           "%X='00:00:00'\t"
           "%EX='00:00:00'\t"
           "\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(lfmt,
        SV("%H='23'\t"
           "%OH='23'\t"
           "%I='11'\t"
           "%OI='11'\t"
           "%M='31'\t"
           "%OM='31'\t"
           "%S='30'\t"
           "%OS='30'\t"
           "%p=''\t" // Locale has no AM or PM.
           "%R='23:31'\t"
           "%T='23:31:30'\t"
           "%r='11:31:30 '\t"
           "%X='23:31:30'\t"
           "%EX='23:31:30'\t"
           "\n"),
        std::chrono::sys_seconds(1'234'567'890s)); // 23:31:30 UTC on Friday, 13 February 2009

  // Use supplied locale (ja_JP). This locale has a different alternate.a
  check(loc, lfmt,
        SV("%H='00'\t"
           "%OH='〇'\t"
           "%I='12'\t"
           "%OI='十二'\t"
           "%M='00'\t"
           "%OM='〇'\t"
           "%S='00'\t"
           "%OS='〇'\t"
           "%p='午前'\t"
           "%R='00:00'\t"
           "%T='00:00:00'\t"
           "%r='午前12時00分00秒'\t"
           "%X='00時00分00秒'\t"
           "%EX='00時00分00秒'\t"
           "\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(loc, lfmt,
        SV("%H='23'\t"
           "%OH='二十三'\t"
           "%I='11'\t"
           "%OI='十一'\t"
           "%M='31'\t"
           "%OM='三十一'\t"
           "%S='30'\t"
           "%OS='三十'\t"
           "%p='午後'\t"
           "%R='23:31'\t"
           "%T='23:31:30'\t"
           "%r='午後11時31分30秒'\t"
           "%X='23時31分30秒'\t"
           "%EX='23時31分30秒'\t"
           "\n"),
        std::chrono::sys_seconds(1'234'567'890s)); // 23:31:30 UTC on Friday, 13 February 2009

  std::locale::global(std::locale::classic());
}

template <class CharT>
static void test_valid_values_date_time() {
  using namespace std::literals::chrono_literals;

  std::basic_string_view<CharT> fmt = SV("{:%%c='%c'%t%%Ec='%Ec'%n}");
  std::basic_string_view<CharT> lfmt = SV("{:L%%c='%c'%t%%Ec='%Ec'%n}");

  const std::locale loc(LOCALE_ja_JP_UTF_8);
  std::locale::global(std::locale(LOCALE_fr_FR_UTF_8));

  // Non localized output using C-locale
  check(fmt, SV("%c='Thu Jan  1 00:00:00 1970'\t%Ec='Thu Jan  1 00:00:00 1970'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(fmt, SV("%c='Fri Feb 13 23:31:30 2009'\t%Ec='Fri Feb 13 23:31:30 2009'\n"),
        std::chrono::sys_seconds(1'234'567'890s)); // 23:31:30 UTC on Friday, 13 February 2009

  // Use the global locale (fr_FR)

  // Pedantic reading the wording of the Standard doesn't require UTC here...
  // TODO FMT evaluate the time zone part
  check(lfmt, SV("%c='jeu. 01 janv. 1970 00:00:00 UTC'\t%Ec='jeu. 01 janv. 1970 00:00:00 UTC'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(lfmt, SV("%c='ven. 13 févr. 2009 23:31:30 UTC'\t%Ec='ven. 13 févr. 2009 23:31:30 UTC'\n"),
        std::chrono::sys_seconds(1'234'567'890s)); // 23:31:30 UTC on Friday, 13 February 2009

  // Use supplied locale (ja_JP). This locale has a different alternate.a
  check(loc, lfmt, SV("%c='1970年01月01日 00時00分00秒'\t%Ec='昭和45年01月01日 00時00分00秒'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  check(loc, lfmt, SV("%c='2009年02月13日 23時31分30秒'\t%Ec='平成21年02月13日 23時31分30秒'\n"),
        std::chrono::sys_seconds(1'234'567'890s)); // 23:31:30 UTC on Friday, 13 February 2009

  std::locale::global(std::locale::classic());
}

template <class CharT>
static void test_valid_values_time_zone() {
  using namespace std::literals::chrono_literals;

  std::basic_string_view<CharT> fmt = SV("{:%%z='%z'%t%%Ez='%Ez'%t%%Oz='%Oz'%t%%Z='%Z'%n}");
  std::basic_string_view<CharT> lfmt = SV("{:L%%z='%z'%t%%Ez='%Ez'%t%%Oz='%Oz'%t%%Z='%Z'%n}");

  const std::locale loc(LOCALE_ja_JP_UTF_8);
  std::locale::global(std::locale(LOCALE_fr_FR_UTF_8));

  // Non localized output using C-locale
  check(fmt, SV("%z='+0000'\t%Ez='+0000'\t%Oz='+0000'\t%Z='UTC'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  // Use the global locale (fr_FR)
  check(lfmt, SV("%z='+0000'\t%Ez='+0000'\t%Oz='+0000'\t%Z='UTC'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  // Use supplied locale (ja_JP). This locale has a different alternate.a
  check(loc, lfmt, SV("%z='+0000'\t%Ez='+0000'\t%Oz='+〇'\t%Z='UTC'\n"),
        std::chrono::sys_seconds(0s)); // 00:00:00 UTC Thursday, 1 January 1970

  std::locale::global(std::locale::classic());
}

template <class CharT>
static void test_valid_values() {
  test_valid_values_year<CharT>();
  test_valid_values_month<CharT>();
  test_valid_values_day<CharT>();
  test_valid_values_weekday<CharT>();
  test_valid_values_day_of_year<CharT>();
  test_valid_values_week<CharT>();
  test_valid_values_iso_8601_week<CharT>();
  test_valid_values_date<CharT>();
  test_valid_values_time<CharT>();
  test_valid_values_date_time<CharT>();
  test_valid_values_time_zone<CharT>();
}

template <class CharT>
static void test_invalid_types() {
  const std::chrono::day arg{0};

  // *** No modifier
  // a - valid
  // A - valid
  // b - valid
  // B - valid
  // c - valid
  // C - valid
  // d - valid
  // D - valid
  // e - valid
  // E - the modifier is checked separately
  check_exception(SV("{:%f}"), "The date time type specifier is invalid", arg);
  // F - valid
  // g - valid
  // G - valid
  // h - valid
  // H - valid
  check_exception(SV("{:%i}"), "The date time type specifier is invalid", arg);
  // I - valid
  // j- valid
  check_exception(SV("{:%J}"), "The date time type specifier is invalid", arg);
  check_exception(SV("{:%k}"), "The date time type specifier is invalid", arg);
  check_exception(SV("{:%K}"), "The date time type specifier is invalid", arg);
  check_exception(SV("{:%l}"), "The date time type specifier is invalid", arg);
  check_exception(SV("{:%L}"), "The date time type specifier is invalid", arg);
  // m - valid
  // M - valid
  // n - valid
  check_exception(SV("{:%N}"), "The date time type specifier is invalid", arg);
  check_exception(SV("{:%o}"), "The date time type specifier is invalid", arg);
  // O - the modifier is checked separately
  // p - valid
  check_exception(SV("{:%P}"), "The date time type specifier is invalid", arg);
  check_exception(SV("{:%q}"), "The supplied date time doesn't contain a duration", arg);
  check_exception(SV("{:%Q}"), "The supplied date time doesn't contain a duration", arg);
  // r - valid
  // R - valid
  check_exception(SV("{:%s}"), "The date time type specifier is invalid", arg);
  // S - valid
  // t - valid
  // T - valid
  // u - valid
  // U - valid
  check_exception(SV("{:%v}"), "The date time type specifier is invalid", arg);
  // V - valid
  // w - valid
  // W - valid
  // x - valid
  // X - valid
  // y - valid
  // Y - valid
  // z - valid
  // Z - valid

  // *** E modifier
  check_exception(SV("{:%Ea}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%EA}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%Eb}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%EB}"), "The date time type specifier for modifier E is invalid", arg);
  // c - valid
  // C - valid
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
  // x - valid
  // X - valid
  // y - valid
  // Y - valid
  // z - valid
  check_exception(SV("{:%EZ}"), "The date time type specifier for modifier E is invalid", arg);
  check_exception(SV("{:%E%}"), "The date time type specifier for modifier E is invalid", arg);

  // *** O modifier
  check_exception(SV("{:%Oa}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OA}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%Ob}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OB}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%Oc}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OC}"), "The date time type specifier for modifier O is invalid", arg);
  // d - valid
  check_exception(SV("{:%OD}"), "The date time type specifier for modifier O is invalid", arg);
  // e - valid
  check_exception(SV("{:%OE}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%Of}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OF}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%Og}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OG}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%Oh}"), "The date time type specifier for modifier O is invalid", arg);
  // H - valid
  check_exception(SV("{:%Oi}"), "The date time type specifier for modifier O is invalid", arg);
  // I - valid
  check_exception(SV("{:%Oj}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OJ}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%Ok}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OK}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%Ol}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OL}"), "The date time type specifier for modifier O is invalid", arg);
  // m - valid
  // M - valid
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
  // S - valid
  check_exception(SV("{:%Ot}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OT}"), "The date time type specifier for modifier O is invalid", arg);
  // u - valid
  // U - valid
  check_exception(SV("{:%Ov}"), "The date time type specifier for modifier O is invalid", arg);
  // V - valid
  // w - valid
  // W - valid
  check_exception(SV("{:%Ox}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OX}"), "The date time type specifier for modifier O is invalid", arg);
  // y - valid
  check_exception(SV("{:%OY}"), "The date time type specifier for modifier O is invalid", arg);
  // z - valid
  check_exception(SV("{:%OZ}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%O%}"), "The date time type specifier for modifier O is invalid", arg);
}

template <class CharT>
static void test() {
  using namespace std::literals::chrono_literals;

  // https://godbolt.org/z/P6qP3azxq
  // fmt::print("Default format: {:%S} {:%S} {:%S}\n", 142.3s, 100ms, 100ns);
  // Default format: 22.300 00.100 00

  // XXX test with :%% no output.... intended??

  test_no_chrono_specs<CharT>();
  test_valid_values<CharT>();
  test_invalid_types<CharT>();

  check_exception(SV("{:A"), "The chrono-specs should start with a '%' or '}'", 0ms);
  check_exception(SV("{:%%{"), "The chrono-specs contains a '{'", 0ms);
  check_exception(SV("{:%"), "End of input while parsing the modifier chrono conversion-spec", 0ms);
  check_exception(SV("{:%E"), "End of input while parsing the modifier E", 0ms);
  check_exception(SV("{:%O"), "End of input while parsing the modifier O", 0ms);

  // Precision not allowed
  check_exception(SV("{:.3}"), "The chrono-specs should start with a '%' or '}'",
                  0ms); // XXX Message not entirely accurate } means not specs
}

int main(int, char**) {
  test<char>();

#ifndef TEST_HAS_NO_WIDE_CHARACTERS
  //  format_tests_char_to_wchar_t(test);
  test<wchar_t>();
#endif

  return 0;
}
