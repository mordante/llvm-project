# 1 __FILE__ 1 3
// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

module;
#include <charconv>

export module std:charconv;
export namespace std {

  // floating-point format for primitive numerical conversion
  using std::chars_format;

  // [charconv.to.chars], primitive numerical output conversion
  using std::to_chars_result;

  using std::to_chars;

  // [charconv.from.chars], primitive numerical input conversion
  using std::from_chars_result;

  using std::from_chars;
} // namespace std
