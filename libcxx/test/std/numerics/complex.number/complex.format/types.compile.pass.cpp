//===----------------------------------------------------------------------===//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20, c++23

// [format.formatter.spec]/4
//   If the library provides an explicit or partial specialization of
//   formatter<T, charT>, that specialization is enabled and meets the
//   Formatter requirements except as noted otherwise.
//
// Tests parts of the BasicFormatter requirements. Like the formattable concept
// it uses the semiregular concept. It test does not use the formattable
// concept since it is the intention the formatter is available without
// including the format header.

#include <concepts>
#include <complex>

#include "test_macros.h"

static_assert(std::semiregular<std::formatter<std::complex<float>, char>>);
static_assert(std::semiregular<std::formatter<std::complex<double>, char>>);
static_assert(std::semiregular<std::formatter<std::complex<long double>, char>>);

#ifndef TEST_HAS_NO_WIDE_CHARACTERS
static_assert(std::semiregular<std::formatter<std::complex<float>, wchar_t>>);
static_assert(std::semiregular<std::formatter<std::complex<double>, wchar_t>>);
static_assert(std::semiregular<std::formatter<std::complex<long double>, wchar_t>>);
#endif // TEST_HAS_NO_WIDE_CHARACTERS
