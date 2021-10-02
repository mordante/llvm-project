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

// <format>

#include <format>

// TODO std::format extensive char and wchar_t other functions just a few.

void XXX() { std::format("{}", "hello"); }

#if 1
void test() {
  // Invalid indexing
  std::format("{}");          // expected-error {{is not a constant expression}}
  std::format("{0}");         // expected-error {{is not a constant expression}}
  std::format("{1}", 1);      // expected-error {{is not a constant expression}}
  std::format("{0}{}", 1, 2); // expected-error {{is not a constant expression}}
  std::format("{}{0}", 1, 2); // expected-error {{is not a constant expression}}

  // invalid std-format-spec integer
  std::format("{:%d}", "hello"); // expected-error {{is not a constant expression}}
  std::format("{:.1}", 42);      // expected-error {{is not a constant expression}}

  std::format("{0:{1}}", 42, "hello"); // expected-error {{is not a constant expression}}

  // invalid std-format-spec string
  std::format("{0:{1}}", 42, "hello");  // expected-error {{is not a constant expression}}
  std::format("{0:.{1}}", 42, "hello"); // expected-error {{is not a constant expression}}

  // TODO Add more errors when can't use the current tests.
}
#endif
