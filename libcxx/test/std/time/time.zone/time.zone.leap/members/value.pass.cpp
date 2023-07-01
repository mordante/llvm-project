//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17
// UNSUPPORTED: no-filesystem

// XFAIL: availability-tzdb-missing

// <chrono>

// class leap_second;

// constexpr seconds value() const noexcept;

#include <cassert>
#include <chrono>

#include "test_macros.h"

constexpr bool test(const std::chrono::leap_second leap_second, std::chrono::seconds expected) {
  std::same_as<std::chrono::seconds> auto value = leap_second.value();
  assert(value == expected);
  static_assert(noexcept(leap_second.value()));

  return true;
}

int main(int, const char**) {
  const std::chrono::tzdb& tzdb = std::chrono::get_tzdb();
  assert(!tzdb.leap_seconds.empty());
  test(tzdb.leap_seconds[0], tzdb.leap_seconds[0].value());

#ifdef _LIBCPP_VERSION
  static_assert(
      test(std::chrono::leap_second(std::chrono::sys_seconds{std::chrono::seconds{0}}, std::chrono::seconds{1}),
           std::chrono::seconds{1}));
#endif

  return 0;
}
