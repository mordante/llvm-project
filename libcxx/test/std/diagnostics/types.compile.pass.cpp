//===----------------------------------------------------------------------===//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20, c++23

#include <concepts>
#include <system_error>

#include "test_macros.h"

static_assert(std::semiregular<std::formatter<std::error_category, char>>);
static_assert(std::semiregular<std::formatter<std::error_code, char>>);
static_assert(std::semiregular<std::formatter<std::error_condition, char>>);

#ifndef TEST_HAS_NO_WIDE_CHARACTERS
static_assert(!std::semiregular<std::formatter<std::error_category, wchar_t>>);
static_assert(!std::semiregular<std::formatter<std::error_code, wchar_t>>);
static_assert(!std::semiregular<std::formatter<std::error_condition, wchar_t>>);
#endif
