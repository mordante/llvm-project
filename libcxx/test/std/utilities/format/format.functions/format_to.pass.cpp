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

// The tests write fixed size buffer, make sure it remains in bounds.
// ADDITIONAL_COMPILE_FLAGS: -D_LIBCPP_DEBUG=1
// UNSUPPORTED: libcxx-no-debug-mode

// <format>

// template<class Out, class... Args>
//   Out format_to(Out out, string_view fmt, const Args&... args);
// template<class Out, class... Args>
//   Out format_to(Out out, wstring_view fmt, const Args&... args);

#include <format>
#include <algorithm>
#include <cassert>
#include <list>
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
    {                                                                                                                  \
      std::basic_string<CharT> out(expected.size(), CharT(' '));                                                       \
      auto it = std::format_to(out.begin(), fmt __VA_OPT__(, __VA_ARGS__));                                            \
      assert(it == out.end());                                                                                         \
      assert(out == expected);                                                                                         \
    }                                                                                                                  \
    {                                                                                                                  \
      std::list<CharT> out;                                                                                            \
      std::format_to(std::back_inserter(out), fmt __VA_OPT__(, __VA_ARGS__));                                          \
      assert(std::equal(out.begin(), out.end(), expected.begin(), expected.end()));                                    \
    }                                                                                                                  \
    {                                                                                                                  \
      std::vector<CharT> out;                                                                                          \
      std::format_to(std::back_inserter(out), fmt __VA_OPT__(, __VA_ARGS__));                                          \
      assert(std::equal(out.begin(), out.end(), expected.begin(), expected.end()));                                    \
    }                                                                                                                  \
    {                                                                                                                  \
      assert(expected.size() < 4096 && "Update the size of the buffer.");                                              \
      CharT out[4096];                                                                                                 \
      CharT* it = std::format_to(out, fmt __VA_OPT__(, __VA_ARGS__));                                                  \
      assert(std::distance(out, it) == int(expected.size()));                                                          \
      /* Convert to std::string since output contains '\0' for boolean tests. */                                       \
      assert(std::basic_string<CharT>(out, it) == expected);                                                           \
    }                                                                                                                  \
  }                                                                                                                    \
  /* */

#ifdef TEST_HAS_NO_EXCEPTIONS
#  define check_exception(...)
#else
#  define check_exception(what, fmt, ...)                                                                              \
    {                                                                                                                  \
      try {                                                                                                            \
        std::basic_string<CharT> out;                                                                                  \
        std::format_to(std::back_inserter(out), fmt __VA_OPT__(, __VA_ARGS__));                                        \
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
  format_tests<wchar_t>();
#endif

  return 0;
}
