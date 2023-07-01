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

// Tests the IANA database leap seconds parsing and operations.
// This is not part of the public tzdb interface.

#include <cassert>
#include <chrono>
#include <fstream>
#include <string>
#include <string_view>

#include "assert_macros.h"
#include "concat_macros.h"
#include "filesystem_test_helper.h"

scoped_test_env env;
[[maybe_unused]] const std::filesystem::path dir = env.create_dir("zoneinfo");
const std::filesystem::path tzdata               = env.create_file("zoneinfo/tzdata.zi");
const std::filesystem::path leap_seconds         = env.create_file("zoneinfo/leap-seconds.list");

std::string_view std::chrono::__libcpp_tzdb_directory() {
  static std::string result = dir.string();
  return result;
}

void write(std::string_view input) {
  static int version = 0;

  std::ofstream f{tzdata};
  f << "# version " << version++ << '\n';
  std::ofstream{leap_seconds}.write(input.data(), input.size());
}

static const std::chrono::tzdb& parse(std::string_view input) {
  write(input);
  return std::chrono::reload_tzdb();
}

static void test_exception(std::string_view input, [[maybe_unused]] std::string_view what) {
  write(input);

  TEST_VALIDATE_EXCEPTION(
      std::runtime_error,
      [&]([[maybe_unused]] const std::runtime_error& e) {
        TEST_LIBCPP_REQUIRE(
            e.what() == what,
            TEST_WRITE_CONCATENATED("\nExpected exception ", what, "\nActual exception   ", e.what(), '\n'));
      },
      TEST_IGNORE_NODISCARD std::chrono::reload_tzdb());
}

static void test_invalid() {
  test_exception("0", "corrupt tzdb: expected a non-zero digit");

  test_exception("1", "corrupt tzdb: expected whitespace");

  test_exception("1 ", "corrupt tzdb: expected a non-zero digit");
}

static void test_leap_seconds() {
  const std::chrono::tzdb& result = parse(
      R"(
2272060800	10	# 1 Jan 1972
2287785600	11	# 1 Jul 1972
2303683200	12	# 1 Jan 1973
)");

  using namespace std::chrono;
#if 0
  // This test uses the real data, the historic database is fixds.
  // The 28th leap seconds was added 2016 Dec 31 23:59:60 so it seems
  // safe to assume systems for 2023 have this update, else their
  // clock would be wrong.
  const tzdb& result = get_tzdb();
#endif
  assert(result.leap_seconds.size() == 3);

  assert(result.leap_seconds[0].date() == sys_seconds{sys_days{1972y / January / 1}});
  assert(result.leap_seconds[0].value() == 10s);

  assert(result.leap_seconds[1].date() == sys_seconds{sys_days{1972y / July / 1}});
  assert(result.leap_seconds[1].value() == 11s);

  assert(result.leap_seconds[2].date() == sys_seconds{sys_days{1973y / January / 1}});
  assert(result.leap_seconds[2].value() == 12s);
}

int main(int, const char**) {
  test_invalid();
  test_leap_seconds();

  return 0;
}
