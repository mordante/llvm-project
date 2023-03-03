//===----------------------------------------------------------------------===//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20, c++23
// XFAIL: availability-fp_to_chars-missing

// <complex>

// template <class T, class charT>
//   struct formatter<complex<T>, charT>;

// constexpr void constexpr void set_brackets(basic_string_view<charT> opening,
//                                            basic_string_view<charT> closing) noexcept;

#include <cassert>
#include <complex>
#include <utility>

#include "make_string.h"
#include "test_format_context.h"

#define SV(S) MAKE_STRING_VIEW(CharT, S)

template <class CharT, class Arg>
constexpr void test() {
  std::formatter<Arg, CharT> formatter;
  formatter.set_brackets(SV("open"), SV("close"));

  // Note the SV macro may throw, so can't use it.
  static_assert(noexcept(formatter.set_brackets(std::basic_string_view<CharT>{}, std::basic_string_view<CharT>{})));

  // Note there is no direct way to validate this function modified the object.
  if (!std::is_constant_evaluated()) {
    using String     = std::basic_string<CharT>;
    using OutIt      = std::back_insert_iterator<String>;
    using FormatCtxT = std::basic_format_context<OutIt, CharT>;

    String result;
    OutIt out             = std::back_inserter(result);
    FormatCtxT format_ctx = test_format_context_create<OutIt, CharT>(out, std::make_format_args<FormatCtxT>());
    formatter.format(Arg{1, 2}, format_ctx);
    assert(result == SV("open1+2iclose"));
  }
}

template <class CharT>
constexpr void test() {
  test<CharT, std::complex<float>>();
  test<CharT, std::complex<double>>();
  test<CharT, std::complex<long double>>();
}

constexpr bool test() {
  test<char>();
#ifndef TEST_HAS_NO_WIDE_CHARACTERS
  test<wchar_t>();
#endif

  return true;
}

int main(int, char**) {
  test();
  static_assert(test());

  return 0;
}
