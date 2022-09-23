//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17

// ADDITIONAL_COMPILE_FLAGS: -O0 -g

// <chrono>

// Tests the IANA database leap seconds parsing and operations.
// This is not part of the public tzdb interface.

#include <chrono>

#include <cassert>
#include <iostream>
#include <span>
#include <sstream>
#include <string>

#include "test_macros.h"

std::chrono::tzdb make_tzdb(std::string leap_seconds) {
  std::istringstream v;
  std::istringstream ls{leap_seconds};

  return std::chrono::__make_tzdb(v, std::span<std::istream*>{}, ls);
}

void test_exception([[maybe_unused]] std::string expected, [[maybe_unused]] std::string leap_seconds) {
#ifndef TEST_HAS_NO_EXCEPTIONS
  try {
    make_tzdb(leap_seconds);
    assert(false);
  } catch (const std::runtime_error& e) {
    if (e.what() != expected)
      std::cerr << "Actual:   " << e.what() << "\nExpected: " << expected << '\n';
    assert(e.what() == expected);
    return;
  }
  assert(false);
#endif
};

static void test_parsing_year() {
  // min, max, only is not allowed in year
  test_exception("corrupt tzdb year: expected a digit", "Leap\tmin\tJun\t30\t23:59:60\t+\tS");
  test_exception("corrupt tzdb year: expected a digit", "Leap\tmax\tJun\t30\t23:59:60\t+\tS");
  test_exception("corrupt tzdb year: expected a digit", "Leap\tonly\tJun\t30\t23:59:60\t+\tS");

  test_exception("corrupt tzdb year: expected a digit", "Leap\t-only\tJun\t30\t23:59:60\t+\tS");
  test_exception("corrupt tzdb year: expected a digit", "Leap\t-\tJun\t30\t23:59:60\t+\tS");

  assert(make_tzdb("Leap\t-12345\tJun\t30\t23:59:60\t+\tS").leap_seconds[0].__year_ == std::chrono::year{-12345});
  assert(make_tzdb("Leap\t-1234\tJun\t30\t23:59:60\t+\tS").leap_seconds[0].__year_ == std::chrono::year{-1234});
  assert(make_tzdb("Leap\t-123\tJun\t30\t23:59:60\t+\tS").leap_seconds[0].__year_ == std::chrono::year{-123});
  assert(make_tzdb("Leap\t-12\tJun\t30\t23:59:60\t+\tS").leap_seconds[0].__year_ == std::chrono::year{-12});
  assert(make_tzdb("Leap\t-1\tJun\t30\t23:59:60\t+\tS").leap_seconds[0].__year_ == std::chrono::year{-1});
  assert(make_tzdb("Leap\t0\tJun\t30\t23:59:60\t+\tS").leap_seconds[0].__year_ == std::chrono::year{0});
  assert(make_tzdb("Leap\t1\tJun\t30\t23:59:60\t+\tS").leap_seconds[0].__year_ == std::chrono::year{1});
  assert(make_tzdb("Leap\t12\tJun\t30\t23:59:60\t+\tS").leap_seconds[0].__year_ == std::chrono::year{12});
  assert(make_tzdb("Leap\t123\tJun\t30\t23:59:60\t+\tS").leap_seconds[0].__year_ == std::chrono::year{123});
  assert(make_tzdb("Leap\t1234\tJun\t30\t23:59:60\t+\tS").leap_seconds[0].__year_ == std::chrono::year{1234});
  assert(make_tzdb("Leap\t12345\tJun\t30\t23:59:60\t+\tS").leap_seconds[0].__year_ == std::chrono::year{12345});

  // Values are clamped between std::chrono::year::min() and  std::chrono::year::max().
  static_assert(
      std::chrono::year::min() == std::chrono::year{-32767} && std::chrono::year::max() == std::chrono::year{32767},
      "Update limits in the test.");
  assert(
      make_tzdb("Leap\t-9999999999999999999999999999999999999999\tJun\t30\t23:59:60\t+\tS").leap_seconds[0].__year_ ==
      std::chrono::year::min());
  assert(make_tzdb("Leap\t-32768\tJun\t30\t23:59:60\t+\tS").leap_seconds[0].__year_ == std::chrono::year::min());
  assert(make_tzdb("Leap\t32768\tJun\t30\t23:59:60\t+\tS").leap_seconds[0].__year_ == std::chrono::year::max());
  assert(make_tzdb("Leap\t9999999999999999999999999999999999999999\tJun\t30\t23:59:60\t+\tS").leap_seconds[0].__year_ ==
         std::chrono::year::max());
}

static void test_parsing_month() {
  assert(make_tzdb("Leap\t1972\tJan\t30\t23:59:60\t+\tS").leap_seconds[0].__month_ == std::chrono::January);
  assert(make_tzdb("Leap\t1972\tFeb\t30\t23:59:60\t+\tS").leap_seconds[0].__month_ == std::chrono::February);
  assert(make_tzdb("Leap\t1972\tMar\t30\t23:59:60\t+\tS").leap_seconds[0].__month_ == std::chrono::March);
  assert(make_tzdb("Leap\t1972\tApr\t30\t23:59:60\t+\tS").leap_seconds[0].__month_ == std::chrono::April);
  assert(make_tzdb("Leap\t1972\tMay\t30\t23:59:60\t+\tS").leap_seconds[0].__month_ == std::chrono::May);
  assert(make_tzdb("Leap\t1972\tJun\t30\t23:59:60\t+\tS").leap_seconds[0].__month_ == std::chrono::June);
  assert(make_tzdb("Leap\t1972\tJul\t30\t23:59:60\t+\tS").leap_seconds[0].__month_ == std::chrono::July);
  assert(make_tzdb("Leap\t1972\tAug\t30\t23:59:60\t+\tS").leap_seconds[0].__month_ == std::chrono::August);
  assert(make_tzdb("Leap\t1972\tSep\t30\t23:59:60\t+\tS").leap_seconds[0].__month_ == std::chrono::September);
  assert(make_tzdb("Leap\t1972\tOct\t30\t23:59:60\t+\tS").leap_seconds[0].__month_ == std::chrono::October);
  assert(make_tzdb("Leap\t1972\tNov\t30\t23:59:60\t+\tS").leap_seconds[0].__month_ == std::chrono::November);
  assert(make_tzdb("Leap\t1972\tDec\t30\t23:59:60\t+\tS").leap_seconds[0].__month_ == std::chrono::December);

  test_exception("corrupt tzdb month: invalid abbreviated name", "Leap\t1972\tD");
  test_exception("corrupt tzdb month: invalid abbreviated name", "Leap\t1972\tDe");

  test_exception("corrupt tzdb month: invalid abbreviated name", "Leap\t1972\tD\t30\t23:59:60\t+\tS");
  test_exception("corrupt tzdb month: invalid abbreviated name", "Leap\t1972\tDe\t30\t23:59:60\t+\tS");
  test_exception("corrupt tzdb month: invalid abbreviated name", "Leap\t1972\tDez\t30\t23:59:60\t+\tS");

  test_exception("corrupt tzdb: no white space between fields", "Leap\t1972\tDece");
}

static void test_parsing_day() {
  test_exception("corrupt tzdb day: underflow", "Leap\t1972\tJun\ta\t23:59:60\t+\tS");
  test_exception("corrupt tzdb day: underflow", "Leap\t1972\tJun\t-\t23:59:60\t+\tS");
  test_exception("corrupt tzdb day: underflow", "Leap\t1972\tJun\t0\t23:59:60\t+\tS");

  assert(make_tzdb("Leap\t1972\tJun\t1\t23:59:60\t+\tS").leap_seconds[0].__day_ == std::chrono::day{1});
  assert(make_tzdb("Leap\t1972\tJun\t2\t23:59:60\t+\tS").leap_seconds[0].__day_ == std::chrono::day{2});
  assert(make_tzdb("Leap\t1972\tJun\t30\t23:59:60\t+\tS").leap_seconds[0].__day_ == std::chrono::day{30});
  assert(make_tzdb("Leap\t1972\tJun\t31\t23:59:60\t+\tS").leap_seconds[0].__day_ == std::chrono::day{31});

  test_exception("corrupt tzdb day: overflow", "Leap\t1972\tJun\t32\t23:59:60\t+\tS");
}

static void test_parsing_hms() {
  // TODO TZDB implement
}

static void test_parsing_corr() {
  assert(make_tzdb("Leap\t1972\tJun\t30\t23:59:60\t+\tS").leap_seconds[0].__corr_ == std::chrono::leap_second::__plus);
  assert(make_tzdb("Leap\t1972\tJun\t30\t23:59:60\t-\tS").leap_seconds[0].__corr_ == std::chrono::leap_second::__minus);

  test_exception("corrupt tzdb leap: invalid correction field", "Leap\t1972\tJun\t30\t23:59:60\t*\tS");
}

static void test_parsing_rs() {
  assert(make_tzdb("Leap\t1972\tJun\t30\t23:59:60\t+\tR").leap_seconds[0].__rs_ == std::chrono::leap_second::__r);
  assert(make_tzdb("Leap\t1972\tJun\t30\t23:59:60\t+\tS").leap_seconds[0].__rs_ == std::chrono::leap_second::__s);

  test_exception("corrupt tzdb leap: invalid R/S field", "Leap\t1972\tJun\t30\t23:59:60\t+\tT");
}

static void test_parsing() {
  assert(make_tzdb("").__rules.empty());
  assert(make_tzdb("# \n").__rules.empty());

  test_exception("corrupt tzdb leap: doesn't contain the magic header", "L");
  test_exception("corrupt tzdb leap: doesn't contain the magic header", "Le");
  test_exception("corrupt tzdb leap: doesn't contain the magic header", "Lea");
  test_exception("corrupt tzdb: no white space between fields", "Leaps");

  // A sample of a valid Leap entry.
  std::chrono::tzdb db = make_tzdb("Leap\t1972\tJun\t30\t23:59:60\t+\tS");
  assert(db.leap_seconds.size() == 1);
  assert(db.leap_seconds[0].__year_ == std::chrono::year{1972});
  assert(db.leap_seconds[0].__month_ == std::chrono::June);
  assert(db.leap_seconds[0].__day_ == std::chrono::day{30});
  assert(db.leap_seconds[0].__hms_ == "23:59:60");
  assert(db.leap_seconds[0].__corr_ == std::chrono::leap_second::__plus);
  assert(db.leap_seconds[0].__rs_ == std::chrono::leap_second::__s);

  test_parsing_year();
  test_parsing_month();
  test_parsing_day();
  test_parsing_hms();
  test_parsing_corr();
  test_parsing_rs();
}

int main(int, const char**) {
  test_parsing();

  return 0;
}
