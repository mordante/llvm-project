//===----------------------------------------------------------------------===//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef TEST_STD_UTILITIES_TEMPLATE_BITSET_BITSET_BITSET_REFERENCE_FORMAT_FORMAT_FUNCTIONS_TESTS_H
#define TEST_STD_UTILITIES_TEMPLATE_BITSET_BITSET_BITSET_REFERENCE_FORMAT_FORMAT_FUNCTIONS_TESTS_H

#include <concepts>
#include <format>
#include <bitset>

#include "format.functions.common.h"

template <class CharT, class TestFunction, class ExceptionTest>
void format_test_bitset_reference(TestFunction check, ExceptionTest check_exception, auto&& input) {
  check(SV("true"), SV("{}"), input);

  // *** align-fill & width ***
  check(SV("answer is 'true   '"), SV("answer is '{:7}'"), true);
  check(SV("answer is '   true'"), SV("answer is '{:>7}'"), true);
  check(SV("answer is 'true   '"), SV("answer is '{:<7}'"), true);
  check(SV("answer is ' true  '"), SV("answer is '{:^7}'"), true);

  check(SV("answer is 'false   '"), SV("answer is '{:8s}'"), false);
  check(SV("answer is '   false'"), SV("answer is '{:>8s}'"), false);
  check(SV("answer is 'false   '"), SV("answer is '{:<8s}'"), false);
  check(SV("answer is ' false  '"), SV("answer is '{:^8s}'"), false);

  // The fill character ':' is allowed here (P0645) but not in ranges (P2286).
  check(SV("answer is ':::true'"), SV("answer is '{::>7}'"), true);
  check(SV("answer is 'true:::'"), SV("answer is '{::<7}'"), true);
  check(SV("answer is ':true::'"), SV("answer is '{::^7}'"), true);

  check(SV("answer is '---false'"), SV("answer is '{:->8s}'"), false);
  check(SV("answer is 'false---'"), SV("answer is '{:-<8s}'"), false);
  check(SV("answer is '-false--'"), SV("answer is '{:-^8s}'"), false);

  // *** Sign ***
  check_exception("A sign field isn't allowed in this format-spec", SV("{:-}"), true);
  check_exception("A sign field isn't allowed in this format-spec", SV("{:+}"), true);
  check_exception("A sign field isn't allowed in this format-spec", SV("{: }"), true);

  check_exception("A sign field isn't allowed in this format-spec", SV("{:-s}"), true);
  check_exception("A sign field isn't allowed in this format-spec", SV("{:+s}"), true);
  check_exception("A sign field isn't allowed in this format-spec", SV("{: s}"), true);

  // *** alternate form ***
  check_exception("An alternate form field isn't allowed in this format-spec", SV("{:#}"), true);
  check_exception("An alternate form field isn't allowed in this format-spec", SV("{:#s}"), true);

  // *** zero-padding ***
  check_exception("A zero-padding field isn't allowed in this format-spec", SV("{:0}"), true);
  check_exception("A zero-padding field isn't allowed in this format-spec", SV("{:0s}"), true);

  // *** precision ***
  check_exception("The format-spec should consume the input or end with a '}'", SV("{:.}"), true);
  check_exception("The format-spec should consume the input or end with a '}'", SV("{:.0}"), true);
  check_exception("The format-spec should consume the input or end with a '}'", SV("{:.42}"), true);

  check_exception("The format-spec should consume the input or end with a '}'", SV("{:.s}"), true);
  check_exception("The format-spec should consume the input or end with a '}'", SV("{:.0s}"), true);
  check_exception("The format-spec should consume the input or end with a '}'", SV("{:.42s}"), true);

  // *** locale-specific form ***
  check(SV("true"), SV("{:L}"), input);

  // *** type ***
#if 0
  for (const auto& fmt : fmt_invalid_types<CharT>("bBdosxX"))
    check_exception("The format-spec type has a type not supported for a bool argument", fmt, true);
#endif
}

template <class CharT, class TestFunction, class ExceptionTest>
void run_tests(TestFunction check, ExceptionTest check_exception) {
  format_test_bitset_reference<CharT>(check, check_exception, std::bitset<1>{1}[0]);
  format_test_bitset_reference<CharT>(check, check_exception, std::bitset<10>{1}[0]);
  format_test_bitset_reference<CharT>(check, check_exception, std::bitset<1000>{1}[0]);
}

#endif // TEST_STD_UTILITIES_TEMPLATE_BITSET_BITSET_BITSET_REFERENCE_FORMAT_FORMAT_FUNCTIONS_TESTS_H
