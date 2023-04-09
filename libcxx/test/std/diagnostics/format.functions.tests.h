//===----------------------------------------------------------------------===//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef TEST_STD_DIAGNOSTICS_FORMAT_FUNCTIONS_TESTS_H
#define TEST_STD_DIAGNOSTICS_FORMAT_FUNCTIONS_TESTS_H

#include <concepts>
#include <format>
#include <future>
#include <filesystem>
#include <system_error>

#include "format.functions.common.h"

template <class TestFunction, class ExceptionTest>
void test_error_category(TestFunction check, ExceptionTest check_exception) {
  assert(std::format("{}", std::generic_category()) == "generic");
  assert(std::format("{}", std::system_category()) == "system");
  assert(std::format("{}", std::iostream_category()) == "iostream");
  assert(std::format("{}", std::future_category()) == "future");

  const std::error_category& input = std::generic_category();

  // *** align-fill & width ***
  // The fill character ':' is allowed here (P0645) but not in ranges (P2286).
  check("generic     ", "{:12}", input);
  check("generic*****", "{:*<12}", input);
  check("__generic___", "{:_^12}", input);
  check(":::::generic", "{::>12}", input);

  check("generic     ", "{:{}}", input, 12);
  check("generic*****", "{:*<{}}", input, 12);
  check("__generic___", "{:_^{}}", input, 12);
  check(":::::generic", "{::>{}}", input, 12);

  check_exception("The format-spec fill field contains an invalid character", "{:}<}", input);
  check_exception("The format-spec fill field contains an invalid character", "{:{<}", input);

  // *** sign ***
  check_exception("The replacement field misses a terminating '}'", "{:-}", input);
  check_exception("The replacement field misses a terminating '}'", "{:+}", input);
  check_exception("The replacement field misses a terminating '}'", "{: }", input);

  // *** alternate form ***
  check_exception("The replacement field misses a terminating '}'", "{:#}", input);

  // *** zero-padding ***
  check_exception("A format-spec width field shouldn't have a leading zero", "{:0}", input);

  // *** precision ***
  check_exception("The replacement field misses a terminating '}'", "{:.5}", input);
  check_exception("The replacement field misses a terminating '}'", "{:.{}}", input, 5);

  // *** locale-specific form ***
  check_exception("The replacement field misses a terminating '}'", "{:L}", input);

  // *** type ***

  check("generic", "{:s}", input);
  check("\"generic\"", "{:?}", input);

  for (std::string_view fmt : fmt_invalid_types<char>("s?"))
    check_exception("The format-spec type has a type not supported for a string argument", fmt, input);
}

/******************/
template <class TestFunction, class ExceptionTest>
void test_error_default(TestFunction check, ExceptionTest check_exception, auto input) {
  check(strerror(EOVERFLOW), "{}", input);

  // *** align-fill & width ***
  // Not tested since the length of the message is not specified.

  check_exception("The format-spec fill field contains an invalid character", "{:}<}", input);
  check_exception("The format-spec fill field contains an invalid character", "{:{<}", input);

  // *** sign ***
  check_exception("The format specification for an error-class does not allow the sign option", "{:-}", input);
  check_exception("The format specification for an error-class does not allow the sign option", "{:+}", input);
  check_exception("The format specification for an error-class does not allow the sign option", "{: }", input);

  // *** alternate form ***
  check_exception(
      "The format specification for an error-class does not allow the alternate form option", "{:#}", input);

  // *** zero-padding ***
  check_exception("The format specification for an error-class does not allow the zero-padding option", "{:0}", input);

  // *** precision ***
  check_exception("The format specification for an error-class does not allow the precision option", "{:.5}", input);
  check_exception(
      "The format specification for an error-class does not allow the precision option", "{:.{}}", input, 5);

  // *** locale-specific form ***
  check_exception(
      "The format specification for an error-class does not allow the locale-specific form option", "{:L}", input);
}

template <class TestFunction, class ExceptionTest>
void test_error_string(TestFunction check, ExceptionTest check_exception, auto input) {
  check(strerror(EOVERFLOW), "{:s}", input);

  // *** align-fill & width ***
  // Not tested since the length of the message is not specified.

  check_exception("The format-spec fill field contains an invalid character", "{:}<s}", input);
  check_exception("The format-spec fill field contains an invalid character", "{:{<s}", input);

  // *** sign ***
  check_exception("The format specification for an error-class does not allow the sign option", "{:-s}", input);
  check_exception("The format specification for an error-class does not allow the sign option", "{:+s}", input);
  check_exception("The format specification for an error-class does not allow the sign option", "{: s}", input);

  // *** alternate form ***
  check_exception(
      "The format specification for an error-class does not allow the alternate form option", "{:#s}", input);

  // *** zero-padding ***
  check_exception("The format specification for an error-class does not allow the zero-padding option", "{:0s}", input);

  // *** precision ***
  check_exception("The format specification for an error-class does not allow the precision option", "{:.5s}", input);
  check_exception(
      "The format specification for an error-class does not allow the precision option", "{:.{}s}", input, 5);

  // *** locale-specific form ***
  check_exception(
      "The format specification for an error-class does not allow the locale-specific form option", "{:Ls}", input);
}

template <class TestFunction, class ExceptionTest>
void test_error_debug(TestFunction check, ExceptionTest check_exception, auto input) {
  check(std::format(R"("{}")", strerror(EOVERFLOW)), "{:?}", input);

  // *** align-fill & width ***
  // Not tested since the length of the message is not specified.

  check_exception("The format-spec fill field contains an invalid character", "{:}<?}", input);
  check_exception("The format-spec fill field contains an invalid character", "{:{<?}", input);

  // *** sign ***
  check_exception("The format specification for an error-class does not allow the sign option", "{:-?}", input);
  check_exception("The format specification for an error-class does not allow the sign option", "{:+?}", input);
  check_exception("The format specification for an error-class does not allow the sign option", "{: ?}", input);

  // *** alternate form ***
  check_exception(
      "The format specification for an error-class does not allow the alternate form option", "{:#?}", input);

  // *** zero-padding ***
  check_exception("The format specification for an error-class does not allow the zero-padding option", "{:0?}", input);

  // *** precision ***
  check_exception("The format specification for an error-class does not allow the precision option", "{:.5?}", input);
  check_exception(
      "The format specification for an error-class does not allow the precision option", "{:.{}?}", input, 5);

  // *** locale-specific form ***
  check_exception(
      "The format specification for an error-class does not allow the locale-specific form option", "{:L?}", input);
}

template <class TestFunction, class ExceptionTest>
void test_error_integral(TestFunction check, ExceptionTest check_exception, auto input) {
  std::string value = std::to_string(input.value());
  std::size_t size  = value.size();

  check(value, "{:d}", input);

  // *** align-fill & width ***
  // The fill character ':' is allowed here (P0645) but not in ranges (P2286).
  if (size == 2) {
    check(std::format("     {}", value), "{:7d}", input);
    check(std::format("{}*****", value), "{:*<7d}", input);
    check(std::format("__{}___", value), "{:_^7d}", input);
    check(std::format(":::::{}", value), "{::>7d}", input);
  }

  check(std::format("     {}", value), "{:{}d}", input, size + 5);
  check(std::format("{}*****", value), "{:*<{}d}", input, size + 5);
  check(std::format("__{}___", value), "{:_^{}d}", input, size + 5);
  check(std::format(":::::{}", value), "{::>{}d}", input, size + 5);

  check_exception("The format-spec fill field contains an invalid character", "{:}<d}", input);
  check_exception("The format-spec fill field contains an invalid character", "{:{<d}", input);

  // *** sign ***
  check(std::format("{}", value), "{:-d}", input);
  check(std::format("+{}", value), "{:+d}", input);
  check(std::format(" {}", value), "{: d}", input);

  // *** alternate form ***
  check(std::format("{:#b}", input.value()), "{:#b}", input);
  check(std::format("{:#B}", input.value()), "{:#B}", input);
  check(std::format("{:#d}", input.value()), "{:#d}", input);
  check(std::format("{:#o}", input.value()), "{:#o}", input);
  check(std::format("{:#x}", input.value()), "{:#x}", input);
  check(std::format("{:#X}", input.value()), "{:#X}", input);

  // *** zero-padding ***
  check(std::format("{:012}", input.value()), "{:012d}", input);

  // *** precision ***
  check_exception("The format specification for an error-class does not allow the precision option", "{:.5d}", input);
  check_exception(
      "The format specification for an error-class does not allow the precision option", "{:.{}d}", input, 5);

  // *** locale-specific form ***
  check(std::format("{:L}", input.value()), "{:Ld}", input);
}

template <class TestFunction, class ExceptionTest>
void test_error_stream(TestFunction check, ExceptionTest check_exception, auto input) {
  std::string value = "generic:" + std::to_string(EOVERFLOW);
  std::size_t size  = value.size();

  check(value, "{:S}", input);

  // *** align-fill & width ***
  // The fill character ':' is allowed here (P0645) but not in ranges (P2286).
  if (size == 10) {
    check(std::format("     {}", value), "{:15S}", input);
    check(std::format("{}*****", value), "{:*<15S}", input);
    check(std::format("__{}___", value), "{:_^15S}", input);
    check(std::format(":::::{}", value), "{::>15S}", input);
  }

  check(std::format("     {}", value), "{:{}S}", input, size + 5);
  check(std::format("{}*****", value), "{:*<{}S}", input, size + 5);
  check(std::format("__{}___", value), "{:_^{}S}", input, size + 5);
  check(std::format(":::::{}", value), "{::>{}S}", input, size + 5);

  check_exception("The format-spec fill field contains an invalid character", "{:}<S}", input);
  check_exception("The format-spec fill field contains an invalid character", "{:{<S}", input);

  // *** sign ***
  check_exception("The format specification for an error-class does not allow the sign option", "{:-S}", input);
  check_exception("The format specification for an error-class does not allow the sign option", "{:+S}", input);
  check_exception("The format specification for an error-class does not allow the sign option", "{: S}", input);

  // *** alternate form ***
  check_exception(
      "The format specification for an error-class does not allow the alternate form option", "{:#S}", input);

  // *** zero-padding ***
  check_exception("The format specification for an error-class does not allow the zero-padding option", "{:0S}", input);

  // *** precision ***
  check_exception("The format specification for an error-class does not allow the precision option", "{:.5S}", input);
  check_exception(
      "The format specification for an error-class does not allow the precision option", "{:.{}S}", input, 5);

  // *** locale-specific form ***
  check_exception(
      "The format specification for an error-class does not allow the locale-specific form option", "{:LS}", input);
}

template <class TestFunction, class ExceptionTest>
void test_error(TestFunction check, ExceptionTest check_exception, auto input) {
  test_error_default(check, check_exception, input);
  test_error_string(check, check_exception, input);
  test_error_debug(check, check_exception, input);
  test_error_integral(check, check_exception, input);
  test_error_stream(check, check_exception, input);

  for (std::string_view fmt : fmt_invalid_types<char>("bBdoxXsS?"))
    check_exception("The format specification for an error-class uses an unsupported display type", fmt, input);
}
template <class TestFunction, class ExceptionTest>
void run_tests(TestFunction check, ExceptionTest check_exception) {
  test_error_category(check, check_exception);
  test_error(check, check_exception, std::make_error_code(std::errc::value_too_large));
  test_error(check, check_exception, std::make_error_condition(std::errc::value_too_large));
}

#endif // TEST_STD_DIAGNOSTICS_FORMAT_FUNCTIONS_TESTS_H
