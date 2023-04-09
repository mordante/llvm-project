//===----------------------------------------------------------------------===//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20, c++23

// <system_error>

// tested in the format functions
//
// string vformat(string_view fmt, format_args args);
// wstring vformat(wstring_view fmt, wformat_args args);

#include <format>
#include <cassert>

#include "test_macros.h"
#include "format.functions.tests.h"
#include "assert_macros.h"
#include "concat_macros.h"

auto test = []<class... Args>(std::string_view expected, std::string_view fmt, Args&&... args) {
  std::string out = std::vformat(fmt, std::make_format_args(args...));
  TEST_REQUIRE(out == expected,
               TEST_WRITE_CONCATENATED(
                   "\nFormat string   ", fmt, "\nExpected output ", expected, "\nActual output   ", out, '\n'));
};

auto test_exception = []<class... Args>([[maybe_unused]] std::string_view what,
                                        [[maybe_unused]] std::string_view fmt,
                                        [[maybe_unused]] Args&&... args) {
  TEST_VALIDATE_EXCEPTION(
      std::format_error,
      [&]([[maybe_unused]] const std::format_error& e) {
        TEST_LIBCPP_REQUIRE(
            e.what() == what,
            TEST_WRITE_CONCATENATED(
                "\nFormat string   ", fmt, "\nExpected exception ", what, "\nActual exception   ", e.what(), '\n'));
      },
      TEST_IGNORE_NODISCARD std::vformat(fmt, std::make_format_args(args...)));
};

int main(int, char**) {
  run_tests(test, test_exception);

  return 0;
}
