//===----------------------------------------------------------------------===//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17
// UNSUPPORTED: libcpp-no-concepts
// UNSUPPORTED: libcpp-has-no-incomplete-format
// TODO FMT Evaluate gcc-11 status
// UNSUPPORTED: gcc-11

// Note this formatter shows additional information when tests are failing.
// This aids the development. Since other formatters fail in the same fashion
// they don't have this additional output.

// <format>

// template<class... Args>
//   string format(string_view fmt, const Args&... args);
// template<class... Args>
//   wstring format(wstring_view fmt, const Args&... args);

#include <format>
#include <cassert>
#ifndef _LIBCPP_HAS_NO_LOCALIZATION
#  include <iostream>
#endif
#include <vector>

#include "test_macros.h"

#ifdef _LIBCPP_VERSION
#  define test_basic_format_string std::__basic_format_string
#else
#  error "Please add a define for your platform"
#endif

#define check(e, fmt, ...)                                                                                             \
  {                                                                                                                    \
    /* Force the proper type for expected. */                                                                          \
    /* When using a check function its argument does the conversion, here do it explicitly. */                         \
    std::basic_string<CharT> expected = e;                                                                             \
    std::basic_string<CharT> out = std::format(fmt __VA_OPT__(, __VA_ARGS__));                                         \
    assert(out == expected);                                                                                           \
  }                                                                                                                    \
  /* */

#ifdef TEST_HAS_NO_EXCEPTIONS
#  define check_exception(...)
#else
#  define check_exception(what, fmt, ...)                                                                              \
    {                                                                                                                  \
      try {                                                                                                            \
        std::format(fmt, __VA_OPT__(, __VA_ARGS__));                                                                   \
        assert(false);                                                                                                 \
      } catch (std::format_error & e) {                                                                                \
        LIBCPP_ASSERT(e.what() == what);                                                                               \
        return;                                                                                                        \
      }                                                                                                                \
      assert(false);                                                                                                   \
    }                                                                                                                  \
    /* */
#endif

#define TEST_FORMAT_USE_COMPILE_TIME_CHECK 1
#include "format_tests.h"

int main(int, char**) {
  format_tests<char>();
#ifndef TEST_HAS_NO_WIDE_CHARACTERS
  format_tests_char_to_wchar_t();
//  format_tests<wchar_t>();
#endif
}
