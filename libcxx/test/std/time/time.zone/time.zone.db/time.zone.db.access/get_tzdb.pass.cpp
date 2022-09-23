//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17

// <chrono>

// const tzdb& get_tzdb();

#include <chrono>

#include <algorithm>
#include <cassert>

#include "test_macros.h"

int main(int, const char**) {
  const std::chrono::tzdb& db = std::chrono::get_tzdb();

  assert(!db.version.empty());
  assert(db.version >= "2022d"); // Version of the oldest database used in libc++.
  LIBCPP_ASSERT(!db.__rules.empty());
  assert(!db.zones.empty());
  assert(std::ranges::is_sorted(db.zones));
  assert(!db.links.empty());
  assert(std::ranges::is_sorted(db.links));
  assert(!db.leap_seconds.empty());

  return 0;
}
