//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17

// XFAIL: availability-tzdb-missing

// <chrono>

//  struct tzdb {
//    string                 version;
//    vector<time_zone>      zones;
//    vector<time_zone_link> links;
//    vector<leap_second>    leap_seconds;
//
//    ...
//  };

#include <chrono>
#include <concepts>

#include "assert_macros.h"

int main(int, const char**) {
  std::chrono::tzdb tzdb;

  { [[maybe_unused]] std::same_as<std::string> auto _ = tzdb.version = "version"; }
  { [[maybe_unused]] std::same_as<std::vector<std::chrono::time_zone>> auto& _ = tzdb.zones; }
  tzdb.zones.clear(); // is it a non-const member?
  { [[maybe_unused]] std::same_as<std::vector<std::chrono::time_zone_link>> auto& _ = tzdb.links; }
  tzdb.links.clear(); // is it a non-const member?

  // TODO TZDB add the leap data member

  return 0;
}
