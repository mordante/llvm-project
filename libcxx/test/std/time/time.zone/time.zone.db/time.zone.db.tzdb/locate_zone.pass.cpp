//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17

// <chrono>

// struct tzdb
// const time_zone* locate_zone(string_view tz_name) const;

#include <chrono>

#include <string_view>
#include <cassert>

#include "test_macros.h"

static void test_zone(std::string_view zone) {
  const std::chrono::time_zone* tz = std::chrono::get_tzdb().locate_zone(zone);
  assert(tz);
  assert(tz->name() == zone);
}

static void test_link(std::string_view link, std::string_view zone) {
  const std::chrono::time_zone* tz = std::chrono::get_tzdb().locate_zone(link);
  assert(tz);
  assert(tz->name() == zone);
}

static void test_exception([[maybe_unused]] std::string_view zone) {
#ifndef TEST_HAS_NO_EXCEPTIONS
  try {
    std::chrono::get_tzdb().locate_zone(zone);
    assert(false);
  } catch ([[maybe_unused]] const std::runtime_error& e) {
    LIBCPP_ASSERT(e.what() == std::string_view{"tzdb: requested time zone not found"});
    return;
  }
  assert(false);
#endif
}

int main(int, const char**) {
  test_zone("America/Vancouver");
  test_zone("Etc/GMT");
  test_zone("Europe/Brussels");

  test_link("Europe/Amsterdam", "Europe/Brussels");
  test_link("America/Nassau", "America/Toronto");
  test_link("Atlantic/St_Helena", "Africa/Abidjan");

  test_exception("This is not a time zone");

  return 0;
}
