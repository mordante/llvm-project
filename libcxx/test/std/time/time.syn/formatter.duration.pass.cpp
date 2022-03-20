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

  check(SV("{}"), SV("1as"), std::chrono::duration<int, std::atto>(1));
  check(SV("{}"), SV("1fs"), std::chrono::duration<int, std::femto>(1));
  check(SV("{}"), SV("1ps"), std::chrono::duration<int, std::pico>(1));
  check(SV("{}"), SV("1ns"), 1ns);
#ifndef TEST_HAS_NO_UNICODE
  check(SV("{}"), SV("1\u00b5s"), 1us);
#else
  check(SV("{}"), SV("1us"), 1us);
#endif
  check(SV("{}"), SV("1ms"), 1ms);
  check(SV("{}"), SV("1cs"), std::chrono::duration<int, std::centi>(1));
  check(SV("{}"), SV("1ds"), std::chrono::duration<int, std::deci>(1));

  check(SV("{}"), SV("1s"), 1s);

  check(SV("{}"), SV("1das"), std::chrono::duration<int, std::deca>(1));
  check(SV("{}"), SV("1hs"), std::chrono::duration<int, std::hecto>(1));
  check(SV("{}"), SV("1ks"), std::chrono::duration<int, std::kilo>(1));
  check(SV("{}"), SV("1Ms"), std::chrono::duration<int, std::mega>(1));
  check(SV("{}"), SV("1Gs"), std::chrono::duration<int, std::giga>(1));
  check(SV("{}"), SV("1Ts"), std::chrono::duration<int, std::tera>(1));
  check(SV("{}"), SV("1Ps"), std::chrono::duration<int, std::peta>(1));
  check(SV("{}"), SV("1Es"), std::chrono::duration<int, std::exa>(1));

  check(SV("{}"), SV("1min"), 1min);
  check(SV("{}"), SV("1h"), 1h);
  check(SV("{}"), SV("1d"), std::chrono::duration<int, std::ratio<86400>>(1));

  check(SV("{}"), SV("1[42]s"), std::chrono::duration<int, std::ratio<42>>(1));
  check(SV("{}"), SV("1[11]s"), std::chrono::duration<int, std::ratio<33, 3>>(1));
  check(SV("{}"), SV("1[11/9]s"), std::chrono::duration<int, std::ratio<11, 9>>(1));
}

template <class CharT>
static void test_valid_positive_integral_values() {
  using namespace std::literals::chrono_literals;

  std::basic_string_view<CharT> fmt = SV("{:"
                                         "%%H='%H'%t"
                                         "%%OH='%OH'%t"
                                         "%%I='%I'%t"
                                         "%%OI='%OI'%t"
                                         "%%M='%M'%t"
                                         "%%OM='%OM'%t"
                                         "%%S='%S'%t"
                                         "%%OS='%OS'%t"
                                         "%%p='%p'%t"
                                         "%%R='%R'%t"
                                         "%%T='%T'%t"
                                         "%%r='%r'%t"
                                         "%%X='%X'%t"
                                         "%%EX='%EX'%t"
                                         "%%j='%j'%t"
                                         "%%Q='%Q'%t"
                                         "%%q='%q'%t"
                                         "%n}");
  //  std::basic_string_view<CharT> lfmt = SV("{:L%%u='%u'%t%%Ou='%Ou'%t%%w='%w'%t%%Ow='%Ow'%t%%a='%a'%t%%A='%A'%n}");

  // XXX FRACTIONAL
  // XXX NEGATIVE

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
           "%j='0'\t"
           "%Q='0'\t"
           "%q='s'\t"
           "\n"),
        0s);

  check(fmt,
        SV("%H='11'\t"
           "%OH='11'\t"
           "%I='11'\t"
           "%OI='11'\t"
           "%M='59'\t"
           "%OM='59'\t"
           "%S='59'\t"
           "%OS='59'\t"
           "%p='AM'\t"
           "%R='11:59'\t"
           "%T='11:59:59'\t"
           "%r='11:59:59 AM'\t"
           "%X='11:59:59'\t"
           "%EX='11:59:59'\t"
           "%j='0'\t"
           "%Q='43199'\t"
           "%q='s'\t"
           "\n"),
        11h + 59min + 59s);

  check(fmt,
        SV("%H='12'\t"
           "%OH='12'\t"
           "%I='12'\t"
           "%OI='12'\t"
           "%M='00'\t"
           "%OM='00'\t"
           "%S='00'\t"
           "%OS='00'\t"
           "%p='PM'\t"
           "%R='12:00'\t"
           "%T='12:00:00'\t"
           "%r='12:00:00 PM'\t"
           "%X='12:00:00'\t"
           "%EX='12:00:00'\t"
           "%j='0'\t"
           "%Q='12'\t"
           "%q='h'\t"
           "\n"),
        12h);

  check(fmt,
        SV("%H='23'\t"
           "%OH='23'\t"
           "%I='11'\t"
           "%OI='11'\t"
           "%M='59'\t"
           "%OM='59'\t"
           "%S='59'\t"
           "%OS='59'\t"
           "%p='PM'\t"
           "%R='23:59'\t"
           "%T='23:59:59'\t"
           "%r='11:59:59 PM'\t"
           "%X='23:59:59'\t"
           "%EX='23:59:59'\t"
           "%j='0'\t"
           "%Q='86399'\t"
           "%q='s'\t"
           "\n"),
        23h + 59min + 59s);

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
           "%j='7'\t"
           "%Q='7'\t"
           "%q='d'\t"
           "\n"),
        std::chrono::duration<int, std::ratio<86400>>(7));

  /*
  The result of formatting a std​::​chrono​::​duration instance holding a negative value, or an hh_­mm_­ss object h for which h.is_­negative() is true, is equivalent to the output of the corresponding positive value, with a STATICALLY-WIDEN<charT>("-") character sequence placed before the replacement of the initial conversion specifier.
   
   */

/*  check(fmt, SV("%u='1'\t%Ou='1'\t%w='1'\t%Ow='1'\t%a='Mon'\t%A='Monday'\n"), std::chrono::weekday(1));
  check(fmt, SV("%u='2'\t%Ou='2'\t%w='2'\t%Ow='2'\t%a='Tue'\t%A='Tuesday'\n"), std::chrono::weekday(2));
  check(fmt, SV("%u='3'\t%Ou='3'\t%w='3'\t%Ow='3'\t%a='Wed'\t%A='Wednesday'\n"), std::chrono::weekday(3));
  check(fmt, SV("%u='4'\t%Ou='4'\t%w='4'\t%Ow='4'\t%a='Thu'\t%A='Thursday'\n"), std::chrono::weekday(4));
  check(fmt, SV("%u='5'\t%Ou='5'\t%w='5'\t%Ow='5'\t%a='Fri'\t%A='Friday'\n"), std::chrono::weekday(5));
  check(fmt, SV("%u='6'\t%Ou='6'\t%w='6'\t%Ow='6'\t%a='Sat'\t%A='Saturday'\n"), std::chrono::weekday(6));
  check(fmt, SV("%u='7'\t%Ou='7'\t%w='0'\t%Ow='0'\t%a='Sun'\t%A='Sunday'\n"), std::chrono::weekday(7));
*/
#if 0
  // Use the global locale (fr_FR)
  check(lfmt, SV("%u='7'\t%Ou='7'\t%w='0'\t%Ow='0'\t%a='dim.'\t%A='dimanche'\n"), std::chrono::weekday(0));
  check(lfmt, SV("%u='1'\t%Ou='1'\t%w='1'\t%Ow='1'\t%a='lun.'\t%A='lundi'\n"), std::chrono::weekday(1));
  check(lfmt, SV("%u='2'\t%Ou='2'\t%w='2'\t%Ow='2'\t%a='mar.'\t%A='mardi'\n"), std::chrono::weekday(2));
  check(lfmt, SV("%u='3'\t%Ou='3'\t%w='3'\t%Ow='3'\t%a='mer.'\t%A='mercredi'\n"), std::chrono::weekday(3));
  check(lfmt, SV("%u='4'\t%Ou='4'\t%w='4'\t%Ow='4'\t%a='jeu.'\t%A='jeudi'\n"), std::chrono::weekday(4));
  check(lfmt, SV("%u='5'\t%Ou='5'\t%w='5'\t%Ow='5'\t%a='ven.'\t%A='vendredi'\n"), std::chrono::weekday(5));
  check(lfmt, SV("%u='6'\t%Ou='6'\t%w='6'\t%Ow='6'\t%a='sam.'\t%A='samedi'\n"), std::chrono::weekday(6));
  check(lfmt, SV("%u='7'\t%Ou='7'\t%w='0'\t%Ow='0'\t%a='dim.'\t%A='dimanche'\n"), std::chrono::weekday(7));

  // Use supplied locale (ja_JP). This locale has a different alternate.
  check(loc, lfmt, SV("%u='7'\t%Ou='七'\t%w='0'\t%Ow='〇'\t%a='日'\t%A='日曜日'\n"), std::chrono::weekday(0));
  check(loc, lfmt, SV("%u='1'\t%Ou='一'\t%w='1'\t%Ow='一'\t%a='月'\t%A='月曜日'\n"), std::chrono::weekday(1));
  check(loc, lfmt, SV("%u='2'\t%Ou='二'\t%w='2'\t%Ow='二'\t%a='火'\t%A='火曜日'\n"), std::chrono::weekday(2));
  check(loc, lfmt, SV("%u='3'\t%Ou='三'\t%w='3'\t%Ow='三'\t%a='水'\t%A='水曜日'\n"), std::chrono::weekday(3));
  check(loc, lfmt, SV("%u='4'\t%Ou='四'\t%w='4'\t%Ow='四'\t%a='木'\t%A='木曜日'\n"), std::chrono::weekday(4));
  check(loc, lfmt, SV("%u='5'\t%Ou='五'\t%w='5'\t%Ow='五'\t%a='金'\t%A='金曜日'\n"), std::chrono::weekday(5));
  check(loc, lfmt, SV("%u='6'\t%Ou='六'\t%w='6'\t%Ow='六'\t%a='土'\t%A='土曜日'\n"), std::chrono::weekday(6));
  check(loc, lfmt, SV("%u='7'\t%Ou='七'\t%w='0'\t%Ow='〇'\t%a='日'\t%A='日曜日'\n"), std::chrono::weekday(7));
#endif

  std::locale::global(std::locale::classic());
}

template <class CharT>
static void test_valid_negative_integral_values() {
  using namespace std::literals::chrono_literals;

  std::basic_string_view<CharT> fmt = SV("{:"
                                         "%%H='%H'%t"
                                         "%%OH='%OH'%t"
                                         "%%I='%I'%t"
                                         "%%OI='%OI'%t"
                                         "%%M='%M'%t"
                                         "%%OM='%OM'%t"
                                         "%%S='%S'%t"
                                         "%%OS='%OS'%t"
                                         "%%p='%p'%t"
                                         "%%R='%R'%t"
                                         "%%T='%T'%t"
                                         "%%r='%r'%t"
                                         "%%X='%X'%t"
                                         "%%EX='%EX'%t"
                                         "%%j='%j'%t"
                                         "%%Q='%Q'%t"
                                         "%%q='%q'%t"
                                         "%n}");

  // [time.format]/4 The result of formatting a std::chrono::duration instance
  // holding a negative value, or an hh_mm_ss object h for which
  // h.is_negative() is true, is equivalent to the output of the corresponding
  // positive value, with a STATICALLY-WIDEN<charT>("-") character sequence
  // placed before the replacement of the initial conversion specifier.
  //
  // Note in this case %% is the initial conversion specifier.
  check(fmt,
        SV("-%H='23'\t"
           "%OH='23'\t"
           "%I='11'\t"
           "%OI='11'\t"
           "%M='59'\t"
           "%OM='59'\t"
           "%S='59'\t"
           "%OS='59'\t"
           "%p='PM'\t"
           "%R='23:59'\t"
           "%T='23:59:59'\t"
           "%r='11:59:59 PM'\t"
           "%X='23:59:59'\t"
           "%EX='23:59:59'\t"
           "%j='0'\t"
           "%Q='86399'\t"
           "%q='s'\t"
           "\n"),
        -(23h + 59min + 59s));
}

template <class CharT>
static void test_valid_fractional_values() {
  using namespace std::literals::chrono_literals;

  std::locale::global(std::locale(LOCALE_fr_FR_UTF_8));

  // Non localized output using C-locale
  check(SV("{:%S}"), SV("00.000000"), 1ns);
  check(SV("{:%S}"), SV("00.000001"), 501ns); // rounded
  check(SV("{:%S}"), SV("00.000001"), 1000ns);

  check(SV("{:%S}"), SV("00.001"), 1ms);
  check(SV("{:%S}"), SV("00.01"), std::chrono::duration<int, std::centi>(1));
  check(SV("{:%S}"), SV("00.1"), std::chrono::duration<int, std::deci>(1));
  check(SV("{:%S}"), SV("01.1"), std::chrono::duration<int, std::deci>(11));

  check(SV("{:%S}"), SV("01.123457"), std::chrono::duration<float>(1.123456789));
  check(SV("{:%S}"), SV("11.123457"), std::chrono::duration<double>(11.123456789));
  check(SV("{:%S}"), SV("01.123457"), std::chrono::duration<long double>(61.123456789));

  // TODO FMT Implement OS 

  check(SV("{:%T}"), SV("01:05:06.000000"), 1h + 5min + 6s + 1ns);
  check(SV("{:%T}"), SV("01:05:06.000001"), 1h + 5min + 6s + 501ns); // rounded
  check(SV("{:%T}"), SV("01:05:06.000001"), 1h + 5min + 6s + 1000ns);

  check(SV("{:%T}"), SV("01:05:06.001"), 1h + 5min + 6s + 1ms);
  check(SV("{:%T}"), SV("01:05:06.01"), 1h + 5min + 6s + std::chrono::duration<int, std::centi>(1));
  check(SV("{:%T}"), SV("01:05:06.1"), 1h + 5min + 6s + std::chrono::duration<int, std::deci>(1));
  check(SV("{:%T}"), SV("01:05:07.1"), 1h + 5min + 6s + std::chrono::duration<int, std::deci>(11));

  check(SV("{:%T}"), SV("00:01:02.500000"),
        std::chrono::duration<float, std::ratio<60>>(1) + std::chrono::duration<float>(2.5));
  check(SV("{:%T}"), SV("01:05:11.123457"),
        std::chrono::duration<double, std::ratio<3600>>(1) + std::chrono::duration<double, std::ratio<60>>(5) +
            std::chrono::duration<double>(11.123456789));
  check(SV("{:%T}"), SV("01:06:01.123457"),
        std::chrono::duration<long double, std::ratio<3600>>(1) +
            std::chrono::duration<long double, std::ratio<60>>(5) + std::chrono::duration<long double>(61.123456789));

  // Use the global locale (fr_FR)
  check(SV("{:L%S}"), SV("00,000000"), 1ns);
  check(SV("{:L%S}"), SV("00,000001"), 501ns); // rounded
  check(SV("{:L%S}"), SV("00,000001"), 1000ns);

  check(SV("{:L%S}"), SV("00,001"), 1ms);
  check(SV("{:L%S}"), SV("00,01"), std::chrono::duration<int, std::centi>(1));
  check(SV("{:L%S}"), SV("00,1"), std::chrono::duration<int, std::deci>(1));
  check(SV("{:L%S}"), SV("01,1"), std::chrono::duration<int, std::deci>(11));

  check(SV("{:L%S}"), SV("01,123457"), std::chrono::duration<float>(1.123456789));
  check(SV("{:L%S}"), SV("11,123457"), std::chrono::duration<double>(11.123456789));
  check(SV("{:L%S}"), SV("01,123457"), std::chrono::duration<long double>(61.123456789));

  check(SV("{:L%T}"), SV("01:05:06,000000"), 1h + 5min + 6s + 1ns);
  check(SV("{:L%T}"), SV("01:05:06,000001"), 1h + 5min + 6s + 501ns); // rounded
  check(SV("{:L%T}"), SV("01:05:06,000001"), 1h + 5min + 6s + 1000ns);

  check(SV("{:L%T}"), SV("01:05:06,001"), 1h + 5min + 6s + 1ms);
  check(SV("{:L%T}"), SV("01:05:06,01"), 1h + 5min + 6s + std::chrono::duration<int, std::centi>(1));
  check(SV("{:L%T}"), SV("01:05:06,1"), 1h + 5min + 6s + std::chrono::duration<int, std::deci>(1));
  check(SV("{:L%T}"), SV("01:05:07,1"), 1h + 5min + 6s + std::chrono::duration<int, std::deci>(11));

  check(SV("{:L%T}"), SV("00:01:02,500000"),
        std::chrono::duration<float, std::ratio<60>>(1) + std::chrono::duration<float>(2.5));
  check(SV("{:L%T}"), SV("01:05:11,123457"),
        std::chrono::duration<double, std::ratio<3600>>(1) + std::chrono::duration<double, std::ratio<60>>(5) +
            std::chrono::duration<double>(11.123456789));
  check(SV("{:L%T}"), SV("01:06:01,123457"),
        std::chrono::duration<long double, std::ratio<3600>>(1) +
            std::chrono::duration<long double, std::ratio<60>>(5) + std::chrono::duration<long double>(61.123456789));

  std::locale::global(std::locale::classic());
}

template <class CharT>
static void test_valid_values() {
  test_valid_positive_integral_values<CharT>();
  test_valid_negative_integral_values<CharT>();
  test_valid_fractional_values<CharT>();
}

template <class CharT>
static void test_invalid_types() {
  using namespace std::literals::chrono_literals;
  const std::chrono::duration arg{1s};

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
  // H - valid
  check_exception(SV("{:%i}"), "The date time type specifier is invalid", arg);
  // I - valid
  // j - valid
  check_exception(SV("{:%J}"), "The date time type specifier is invalid", arg);
  check_exception(SV("{:%k}"), "The date time type specifier is invalid", arg);
  check_exception(SV("{:%K}"), "The date time type specifier is invalid", arg);
  check_exception(SV("{:%l}"), "The date time type specifier is invalid", arg);
  check_exception(SV("{:%L}"), "The date time type specifier is invalid", arg);
  check_exception(SV("{:%m}"), "The supplied date time doesn't contain a month", arg);
  // M - valid
  // n - valid
  check_exception(SV("{:%N}"), "The date time type specifier is invalid", arg);
  check_exception(SV("{:%o}"), "The date time type specifier is invalid", arg);
  // O - the modifier is checked separately
  // p - valid
  check_exception(SV("{:%P}"), "The date time type specifier is invalid", arg);
  // q - valid
  // Q - valid
  // r - valid
  // R - valid
  check_exception(SV("{:%s}"), "The date time type specifier is invalid", arg);
  // S - valid
  // t - valid
  // T - valid
  check_exception(SV("{:%u}"), "The supplied date time doesn't contain a weekday", arg);
  check_exception(SV("{:%U}"), "The supplied date time doesn't contain a date", arg);
  check_exception(SV("{:%v}"), "The date time type specifier is invalid", arg);
  check_exception(SV("{:%V}"), "The supplied date time doesn't contain a date", arg);
  check_exception(SV("{:%w}"), "The supplied date time doesn't contain a weekday", arg);
  check_exception(SV("{:%W}"), "The supplied date time doesn't contain a date", arg);
  check_exception(SV("{:%x}"), "The supplied date time doesn't contain a date", arg);
  // X - valid
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
  // EX - valid
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
  // OH -valid
  check_exception(SV("{:%Oi}"), "The date time type specifier for modifier O is invalid", arg);
  // OI - valid
  check_exception(SV("{:%Oj}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OJ}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%Ok}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OK}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%Ol}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%OL}"), "The date time type specifier for modifier O is invalid", arg);
  check_exception(SV("{:%Om}"), "The supplied date time doesn't contain a month", arg);
  // OM - valid
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
  // Os - valid
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
