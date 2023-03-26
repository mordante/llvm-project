//===----------------------------------------------------------------------===//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef TEST_STD_RE_RE_SUBMATCH_RE_SUBMATCH_FORMAT_FORMAT_FUNCTIONS_TESTS_H
#define TEST_STD_RE_RE_SUBMATCH_RE_SUBMATCH_FORMAT_FORMAT_FUNCTIONS_TESTS_H

#include <concepts>
#include <array>
#include <format>
#include <list>
#include <regex>

#include "format.functions.common.h"

template <class CharT, class TestFunction, class ExceptionTest, class Container>
void test_sub_match(TestFunction check, ExceptionTest check_exception, Container input) {
  std::sub_match<typename Container::const_iterator> match;
  match.first  = input.begin();
  match.second = input.end();

  // Matched == false -> no output.
  check(SV(""), SV("{}"), match);
  check(SV("*****"), SV("{:*<5}"), match);

  match.matched = true;

  // *** Valid input tests ***
  check(SV("hello world"), SV("{}"), match);

  // *** align-fill & width ***
  // The fill character ':' is allowed here (P0645) but not in ranges (P2286).
  check(SV("hello world     "), SV("{:16}"), match);
  check(SV("hello world*****"), SV("{:*<16}"), match);
  check(SV("__hello world___"), SV("{:_^16}"), match);
  check(SV(":::::hello world"), SV("{::>16}"), match);

  check(SV("hello world     "), SV("{:{}}"), match, 16);
  check(SV("hello world*****"), SV("{:*<{}}"), match, 16);
  check(SV("__hello world___"), SV("{:_^{}}"), match, 16);
  check(SV(":::::hello world"), SV("{::>{}}"), match, 16);

  // *** sign ***
  check_exception("The format-spec should consume the input or end with a '}'", SV("{:-}"), match);

  // *** alternate form ***
  check_exception("The format-spec should consume the input or end with a '}'", SV("{:#}"), match);

  // *** zero-padding ***
  check_exception("A format-spec width field shouldn't have a leading zero", SV("{:0}"), match);

  // *** precision ***
  check(SV("hello"), SV("{:.5}"), match);
  check(SV("hello"), SV("{:.{}}"), match, 5);

  // *** locale-specific form ***
  check_exception("The format-spec should consume the input or end with a '}'", SV("{:L}"), match);

  // *** type ***

  check(SV("hello world"), SV("{:s}"), match);
  check(SV("\"hello world\""), SV("{:?}"), match);

  for (std::basic_string_view<CharT> fmt : fmt_invalid_types<CharT>("s?"))
    check_exception("The format-spec type has a type not supported for a string argument", fmt, match);
}

#ifndef _LIBCPP_HAS_NO_UNICODE
// Tests that the width estimation algorithm works as expected.
template <class CharT, class TestFunction, class Container>
void test_sub_match_unicode(TestFunction check, Container input) {
  std::sub_match<typename Container::const_iterator> match;
  match.first   = input.begin();
  match.second  = input.end();
  match.matched = true;

#  define FAMILY "\U0001f468\u200d\U0001F469\u200d\U0001F467\u200d\U0001F466"

  // *** Valid input tests ***
  check(SV(FAMILY), SV("{}"), match);

  // *** align-fill & width ***
  // The fill character ':' is allowed here (P0645) but not in ranges (P2286).
  check(SV(FAMILY "     "), SV("{:7}"), match);
  check(SV(FAMILY "*****"), SV("{:*<7}"), match);
  check(SV("__" FAMILY "___"), SV("{:_^7}"), match);
  check(SV(":::::" FAMILY), SV("{::>7}"), match);

  // *** precision ***
  check(SV(""), SV("{:.1}"), match);
  check(SV(FAMILY), SV("{:.2}"), match);
  check(SV(FAMILY), SV("{:.3}"), match);

  // *** type ***

  check(SV(FAMILY), SV("{:s}"), match);
  check(SV("\"👨\\u{200d}👩\\u{200d}👧\\u{200d}👦\""), SV("{:?}"), match);

#  undef FAMILY
}
#endif // _LIBCPP_HAS_NO_UNICODE

template <class CharT, class TestFunction, class ExceptionTest>
void run_tests(TestFunction check, ExceptionTest check_exception) {
  std::basic_string_view input = SV("hello world");
  test_sub_match<CharT>(check, check_exception, input);
  test_sub_match<CharT>(check, check_exception, std::basic_string<CharT>{input});
  // libc++ has optimizations for a contiguous iterator, also test with non-contiguous.
  test_sub_match<CharT>(check, check_exception, std::list<CharT>{input.begin(), input.end()});

#ifndef _LIBCPP_HAS_NO_UNICODE
  // { Family: Man, Woman, Girl, Boy } 2 columns
  test_sub_match_unicode<CharT>(check, SV("\U0001f468\u200d\U0001F469\u200d\U0001F467\u200d\U0001F466"));
#endif
}

#endif // TEST_STD_RE_RE_SUBMATCH_RE_SUBMATCH_FORMAT_FORMAT_FUNCTIONS_TESTS_H
