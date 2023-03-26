//===----------------------------------------------------------------------===//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// TODO FMT c++23 and synopsis

// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20, c++23
// XFAIL: availability-fp_to_chars-missing

// <regex>

// template<class BidirectionalIterator, class charT>
//   struct formatter<sub_match<BidirectionalIterator>, charT>

// template<class FormatContext>
//   typename FormatContext::iterator
//     format(sub_match<_BidirectionalIterator> s, FormatContext& ctx) const;

// Note this tests the basics of this function. It's tested in more detail in
// the format functions tests.

#include <cassert>
#include <concepts>
#include <format>
#include <iterator>
#include <regex>

#include "test_format_context.h"
#include "test_macros.h"
#include "make_string.h"

#define SV(S) MAKE_STRING_VIEW(CharT, S)

template <class StringViewT, class Arg>
void test(StringViewT expected, Arg arg) {
  using CharT      = typename StringViewT::value_type;
  using String     = std::basic_string<CharT>;
  using OutIt      = std::back_insert_iterator<String>;
  using FormatCtxT = std::basic_format_context<OutIt, CharT>;

  const std::formatter<Arg, CharT> formatter;

  String result;
  OutIt out             = std::back_inserter(result);
  FormatCtxT format_ctx = test_format_context_create<OutIt, CharT>(out, std::make_format_args<FormatCtxT>(arg));
  formatter.format(arg, format_ctx);
  assert(result == expected);
}

template <class CharT>
void test() {
  std::basic_string_view<CharT> input = SV("abc");
  std::sub_match<const CharT*> match;
  match.first   = input.data();
  match.second  = input.data() + input.size();
  match.matched = true;
  test(SV("abc"), match);
}

void test() {
  test<char>();
#ifndef TEST_HAS_NO_WIDE_CHARACTERS
  test<wchar_t>();
#endif
}

int main(int, char**) {
  test();

  return 0;
}
