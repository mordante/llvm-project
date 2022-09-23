//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// For information see https://libcxx.llvm.org/DesignDocs/TimeZone.html

#include <__algorithm/ranges_sort.h>
#include <__memory/addressof.h>
#include <__variant/monostate.h>
#include <algorithm>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <map>
#include <optional> // TODO TZDB remove
#include <stdexcept>
#include <string>

//
//
//
//
//
#include <iostream>

// Contains a parser for the IANA time zone data files.
//
// These can be found at https://data.iana.org/time-zones/ and are in the
// public domain. Information regarding the input can be found at
// https://data.iana.org/time-zones/tz-how-to.html and
// https://man7.org/linux/man-pages/man8/zic.8.html .
//
// Note the data files shipped only do not use all allowed features. For
// example maximum may be abbreviated in every unique form, which would be: ma,
// max, maxi, maxim, and maximu. In the data files the only value used is max,
// so the parser only tests for that value. The name comparison should be
// case-insensitive, but only lower case is used.

_LIBCPP_BEGIN_NAMESPACE_STD

namespace chrono {
#if 0
// *** Parser helpers ***

// Strips the leading whitespace of the input.
// The result indicates whether the line contains data.
[[nodiscard]] static bool santize_input(string::const_iterator& first, string::const_iterator last) {
  while (first != last && (*first == '\t' || *first == ' '))
    ++first;

  return first != last && *first != '#';
}

[[nodiscard]] static bool
starts_with(string::const_iterator& first, string::const_iterator last, std::string_view expected) {
  if (last - first < static_cast<ptrdiff_t>(expected.size()))
    return false;

  for (auto c : expected)
    if (*first++ != c)
      return false;

  return true;
}

static void skip_white_space(string::const_iterator& first, string::const_iterator last) {
  string::const_iterator begin = first;
  while (first != last && (*first == '\t' || *first == ' '))
    ++first;

  if (begin == first)
    std::__throw_runtime_error("corrupt tzdb: no white space between fields"); // TEST

  if (first == last)
    std::__throw_runtime_error("corrupt tzdb: no new field after white space"); // TEST
}

[[nodiscard]] static string_view parse_string(string::const_iterator& first, string::const_iterator last) {
  string::const_iterator begin = first;
  while (first != last && *first != '\t' && *first != ' ')
    ++first;

  // XXX This should be impossible
  if (begin == first)
    std::__throw_runtime_error("corrupt tzdb: empty string field"); // TEST

  return {begin, first};
}

[[nodiscard]] static year parse_year_value(string::const_iterator& first, string::const_iterator last) {
  bool negative = *first == '-';
  if (negative && ++first == last)
    std::__throw_runtime_error("corrupt tzdb: year only has a minus sign");

  if (*first < '0' || *first > '9')
    std::__throw_runtime_error("corrupt tzdb year: expected a digit");

  int result             = 0;
  bool reached_threshold = false;

  while (first != last && *first >= '0' && *first <= '9') {
    if (!reached_threshold) {
      result *= 10;
      result += *first - '0';

      // It is possible for the database to contain years outside the range
      // supported by chrono. When that happens assign the valid threshold and
      // skip the other digits. The threshold value fits in a signed short so
      // the value will never exceed the range of an int. Thus no int overflow
      // detection is needed.
      if (negative && result <= static_cast<int>(year::min())) {
        result            = static_cast<int>(year::min());
        reached_threshold = true;
      } else if (result >= static_cast<int>(year::max())) {
        result            = static_cast<int>(year::max());
        reached_threshold = true;
      }
    }
    ++first;
  }

  return year{negative ? -result : result};
}

[[nodiscard]] static year parse_year(string::const_iterator& first, string::const_iterator last) {
  if (*first == 'm') {
    if (last - first >= 3) {
      if (first[1] == 'i' && first[2] == 'n') {
        first += 3;
        return year::min();
      }
      if (first[1] == 'a' && first[2] == 'x') {
        first += 3;
        return year::max();
      }
    }
    std::__throw_runtime_error("corrupt tzdb year: min or max expected");
  }

  return chrono::parse_year_value(first, last);
}

[[nodiscard]] static year
parse_year_including_only(string::const_iterator& first, string::const_iterator last, year only) {
  if (*first != 'o')
    return parse_year(first, last);

  ++first;
  if (!chrono::starts_with(first, last, "nly"))
    std::__throw_runtime_error("corrupt tzdb: expected value only");

  return only;
}

[[nodiscard]] static month parse_month(string::const_iterator& first, string::const_iterator last) {
  if (last - first >= 3) {
    const char* s = std::addressof(*first);
    first += 3;
    switch (s[0]) {
    case 'A':
      if (s[1] == 'p' && s[2] == 'r')
        return April;
      if (s[1] == 'u' && s[2] == 'g')
        return August;
      break;

    case 'D':
      if (s[1] == 'e' && s[2] == 'c')
        return December;
      break;

    case 'F':
      if (s[1] == 'e' && s[2] == 'b')
        return February;
      break;

    case 'J':
      if (s[1] == 'a' && s[2] == 'n')
        return January;
      if (s[1] == 'u') {
        if (s[2] == 'n')
          return June;
        if (s[2] == 'l')
          return July;
      }
      break;

    case 'M':
      if (s[1] == 'a') {
        if (s[2] == 'y')
          return May;
        if (s[2] == 'r')
          return March;
      }
      break;

    case 'N':
      if (s[1] == 'o' && s[2] == 'v')
        return November;
      break;

    case 'O':
      if (s[1] == 'c' && s[2] == 't')
        return October;
      break;

    case 'S':
      if (s[1] == 'e' && s[2] == 'p')
        return September;
      break;
    }
  }
  std::__throw_runtime_error("corrupt tzdb month: invalid abbreviated name");
}

[[nodiscard]] static day parse_day(string::const_iterator& first, string::const_iterator last) {
  unsigned result = 0;

  while (first != last && *first >= '0' && *first <= '9') {
    result *= 10;
    result += *first++ - '0';
    if (result > 31)
      std::__throw_runtime_error("corrupt tzdb day: overflow");
  }

  if (result == 0)
    std::__throw_runtime_error("corrupt tzdb day: underflow");

  return day{result};
}

// TODO TZDB parse all possible on variants
[[nodiscard]] static optional<day> parse_on(string::const_iterator& first, string::const_iterator last) {
  if (*first < '0' || *first > '9')
    return {};

  unsigned result = 0;
  while (first != last && *first >= '0' && *first <= '9') {
    result *= 10;
    result += *first - '0';
    ++first;
  }

  if (result)
    return day{result};

  return {};
}

// The current database contains 1/100 th of a second resolution at most.
/*
                    2            time in hours
                    2:00         time in hours and minutes
                    01:28:14     time in hours, minutes, and seconds
                    00:19:32.13  time with fractional seconds
                    12:00        midday, 12 hours after 00:00
                    15:00        3 PM, 15 hours after 00:00
                    24:00        end of day, 24 hours after 00:00
                    260:00       260 hours after 00:00
                    -2:30        2.5 hours before 00:00
                    -            equivalent to 0
*/
// a time field of often followed by a character so the routine stops when an
// unexpected value is found.  Validation is left to the caller.
[[nodiscard]] static milliseconds parse_duration(string::const_iterator& first, string::const_iterator last) {
  milliseconds result{0};
  bool negative = *first == '-';
  if (negative)
    ++first;

  enum { hour, minute, second, fractional } state = hour;
  int value                                       = 0;
  string::const_iterator begin;
  // the time field is never the last field so no need to do a past loop part
  // when terminates at the end of the input. The error will be done by the
  // next parser.
  for (bool done = false; first != last; ++first) {
    if (*first >= '0' && *first <= '9') {
      value *= 10;
      value += *first - '0';
      continue;
    }

    switch (state) {
    case hour:
      result = hours{value};
      value  = 0;
      state  = minute;
      done   = *first != ':';
      break;

    case minute:
      if (value > 60)
        std::__throw_runtime_error("corrupt tzdb: a time has more than 60 minutes");
      result += minutes{value};
      value = 0;
      state = second;
      done  = *first != ':';
      break;

    case second:
      if (value > 60)
        std::__throw_runtime_error("corrupt tzdb: a time has more than 60 seconds");
      result += seconds{value};
      value = 0;
      state = fractional;
      begin = first + 1;
      done  = *first != '.';
      break;

    case fractional:
      switch (first - begin) {
      case 0: // no digits accept this case.
      case 3: // no adjustment needed.
        break;
      case 1:
        value *= 100;
        break;
      case 2:
        value *= 10;
        break;

      default:
        std::__throw_runtime_error("corrupt tzdb: a time has 1000 or more milli seconds");
      }
      result += milliseconds{value};
      done = true;
      break;
    }
    if (done)
      break;
  }

  return negative ? -result : result;
}

[[nodiscard]] __time_zone_rule::__clock parse_clock(string::const_iterator& first) {
  if (*first == '\t' || *first == ' ')
    return __time_zone_rule::__clock::__local;

  switch (*first++) {
  case 'w':
    return __time_zone_rule::__clock::__local;

  case 's':
    return __time_zone_rule::__clock::__standard;

  case 'u':
  case 'g':
  case 'z':
    return __time_zone_rule::__clock::__universal;

  default:
    std::__throw_runtime_error("corrupt tzdb clock: invalid abbreviated type");
  }
}

[[nodiscard]] bool parse_dst_flag(string::const_iterator& first) {
  if (*first == '\t' || *first == ' ')
    return false;

  switch (*first++) {
  case 's':
    return false;
  case 'd':
    return true;

  default:
    std::__throw_runtime_error("corrupt tzdb dst: invalid abbreviated type");
  }
}

[[nodiscard]] static optional<time_zone::__element>
parse_zone_element(std::map<string, size_t>& rules, string::const_iterator first, string::const_iterator last) {
  time_zone::__element result;

  result.__stdoff_ = chrono::parse_duration(first, last);
  chrono::skip_white_space(first, last);

  // TODO TZDB accept SAVE fields too.
  string rule{chrono::parse_string(first, last)};
  if (rule != "-") {
    if (auto it = rules.find(rule); it != rules.end())
      result.__rules_ = it->second;
    else
      return {};
  }
  chrono::skip_white_space(first, last);

  result.__fmt_ = chrono::parse_string(first, last);
  if (first != last) {
    // TODO TZDB need to parse more optional fields.
    chrono::skip_white_space(first, last);
    result.__until_ = parse_year(first, last);
  }

  // When not parsed elements are available return nothing.
  // Just to make sure the parser will include these fields eventually.
  if (first != last)
    return {};

  return result;
}

[[nodiscard]] static bool
parse_zone(tzdb& db, std::map<string, size_t>& rules, string::const_iterator first, string::const_iterator last) {
  if (!chrono::starts_with(first, last, "one"))
    std::__throw_runtime_error("corrupt tzdb zone: doesn't contain the magic header");
  chrono::skip_white_space(first, last);

  string name{chrono::parse_string(first, last)};
  chrono::skip_white_space(first, last);

  optional<time_zone::__element> element = parse_zone_element(rules, first, last);
  if (!element)
    return false;

  db.zones.push_back(time_zone{std::move(name), std::vector{*element}});
  return element->__until_.has_value();
}

[[nodiscard]] static bool parse_zone_continuation(
    tzdb& db, std::map<string, size_t>& rules, string::const_iterator first, string::const_iterator last) {
  optional<time_zone::__element> element = parse_zone_element(rules, first, last);
  if (!element)
    return false;

  db.zones.back().__elements_.emplace_back(*element);
  return element->__until_.has_value();
}

static void parse_leap(tzdb& db, string::const_iterator first, string::const_iterator last) {
  if (!chrono::starts_with(first, last, "eap"))
    std::__throw_runtime_error("corrupt tzdb leap: doesn't contain the magic header");
  chrono::skip_white_space(first, last);

  leap_second leap;
  leap.__year_ = chrono::parse_year_value(first, last);
  chrono::skip_white_space(first, last);

  leap.__month_ = chrono::parse_month(first, last);
  chrono::skip_white_space(first, last);

  leap.__day_ = chrono::parse_day(first, last);
  chrono::skip_white_space(first, last);

  leap.__hms_ = chrono::parse_string(first, last);
  chrono::skip_white_space(first, last);

  switch (*first++) {
  case '+':
    leap.__corr_ = leap_second::__plus;
    break;
  case '-':
    leap.__corr_ = leap_second::__minus;
    break;
  default:
    std::__throw_runtime_error("corrupt tzdb leap: invalid correction field");
  }
  chrono::skip_white_space(first, last);

  switch (*first) {
  case 'R':
    leap.__rs_ = leap_second::__r;
    break;
  case 'S':
    leap.__rs_ = leap_second::__s;
    break;
  default:
    std::__throw_runtime_error("corrupt tzdb leap: invalid R/S field");
  }

  db.leap_seconds.emplace_back(std::move(leap));
}

// *** Main parsers ***
static void parse_link(tzdb& db, string::const_iterator first, string::const_iterator last) {
  if (!chrono::starts_with(first, last, "ink"))
    std::__throw_runtime_error("corrupt tzdb link: doesn't contain the magic header");
  chrono::skip_white_space(first, last);

  string_view target = chrono::parse_string(first, last);
  chrono::skip_white_space(first, last);

  string_view name = chrono::parse_string(first, last);

  db.links.emplace_back(name, target);
}

static void
parse_rule(tzdb& db, std::map<string, size_t>& rules, string::const_iterator first, string::const_iterator last) {
  if (!chrono::starts_with(first, last, "ule"))
    std::__throw_runtime_error("corrupt tzdb rule: doesn't contain the magic header");
  chrono::skip_white_space(first, last);

  string name{chrono::parse_string(first, last)};
  chrono::skip_white_space(first, last);

  __time_zone_rule rule;
  rule.__from = chrono::parse_year(first, last);
  chrono::skip_white_space(first, last);

  rule.__to = chrono::parse_year_including_only(first, last, rule.__from);
  chrono::skip_white_space(first, last);

  if (*first != '-')
    std::__throw_runtime_error("corrupt tzdb rule: doesn't contain the type field"); // TODO TEST
  chrono::skip_white_space(++first, last);

  rule.__in_month = chrono::parse_month(first, last);
  chrono::skip_white_space(first, last);

  optional<day> on = chrono::parse_on(first, last);
  if (!on)
    return;
  rule.__on = *on;
  chrono::skip_white_space(first, last);

  rule.__at_time  = chrono::parse_duration(first, last);
  rule.__at_clock = chrono::parse_clock(first);
  chrono::skip_white_space(first, last);

  rule.__save_time   = chrono::parse_duration(first, last);
  rule.__save_is_dst = chrono::parse_dst_flag(first);
  chrono::skip_white_space(first, last);

  string_view letters = parse_string(first, last);
  if (letters != "-")
    rule.__letters = letters;

  // Either the element to the existing bucket or create a new bucket.
  auto it = rules.find(name);
  if (it == rules.end()) {
    it = rules.emplace(std::move(name), db.__rules.size()).first;
    db.__rules.emplace_back();
  }

  db.__rules[it->second].emplace_back(std::move(rule));
}

static void parse_time_zone(tzdb& __db, istream& time_zone) {
  string line;
  bool parsing_zone = false;

  // Contains the mapping of a rule name to the index in the array.
  std::map<string, size_t> rules;
  while (getline(time_zone, line)) {
    // std::cerr << line << '\n';
    string::const_iterator first = line.begin();
    string::const_iterator last  = line.end();

    if (!santize_input(first, last))
      continue;

    if (parsing_zone)
      parsing_zone = parse_zone_continuation(__db, rules, first, last);
    else
      switch (*first++) {
      case 'L':
        parse_link(__db, first, last);
        break;

      case 'R':
        parse_rule(__db, rules, first, last);
        break;

/*      case 'Z':
        parsing_zone = parse_zone(__db, rules, first, last);
        break;
*/
      default:
        // Note not all rules are implemented yet, therefore ignore lines with
        // invalid input.
        // TODO TZDB Enable the exception after improving the parser.
        // std::__throw_runtime_error("corrupt tzdb zone: invalid first column");
        break;
      }
  }
}

static void parse_leap_seconds(tzdb& __db, istream& leap_seconds) {
  string line;
  while (getline(leap_seconds, line)) {
    string::const_iterator first = line.begin();
    string::const_iterator last  = line.end();

    if (!santize_input(first, last))
      continue;
    /*    if (line.empty())
      continue;
*/
    switch (*first++) {
      //    switch (line[0]) {
    case 'L':
      parse_leap(__db, first, last);
      break;

      // TODO TZDB Implement expires field.
    case '#':
      // Skip comment
      break;

    default:
      std::__throw_runtime_error("corrupt tzdb leap: invalid first column");
    }
  }
}

// *** Public functions ***

tzdb __make_tzdb(istream& __version, span<istream*> __time_zones, istream& __leap_seconds) {
  tzdb result;

  result.version = std::string(istreambuf_iterator<char>{__version}, {});
  for (auto& time_zone : __time_zones)
    parse_time_zone(result, *time_zone);

  std::ranges::sort(result.zones);
  std::ranges::sort(result.links);
  /*
  std::cerr << "Zones:\n";
  for (const auto& zone : result.zones)
    std::cerr << "\t" << zone.name() << "\n";
*/
  /*
  std::cerr << "Links:\n";
  for (const auto& link : result.links)
    std::cerr << "\t" << link.name() << "\n";
*/
  parse_leap_seconds(result, __leap_seconds);

  return result;
}

tzdb __make_tzdb(string_view __directory) {
  filesystem::path root = __directory;

  ifstream version{root / "version"};

  // Files with time zone information in the IANA database.
  array time_zones{
      // Continents
      ifstream{root / "africa"},
      ifstream{root / "antarctica"},
      ifstream{root / "asia"},
      ifstream{root / "australasia"},
      ifstream{root / "europe"},
      ifstream{root / "northamerica"},
      ifstream{root / "southamerica"},
      // Special files
      ifstream{root / "etcetera"}, // for ships at sea and other miscellany

      // Zones that go back beyond the scope of the tz database
      // This file contains some duplicated entries that may conflict with
      // existing entries. For example Europe/Amsterdam links to
      // Europe/Brussels, but in this file Europe/Amsterdam has its own time
      // zone. From the warning in the header of this file it may happen this
      // file will not observe the latest changes in the database,
      //
      // Since the file is less reliable and may conflict with the main
      // database it is not loaded. It would be nice to have a flag to load
      // this file first and then filter the duplicates.
      //
      // TODO TZDB Define the policy for this file.
      // ifstream{root / "backzone"},

      // links for backward compatibility
      // Even when libc++ never shipped with links from this file it still
      // needs to support these links. When requestion the name of the current
      // time zone on the users system, these names may be returned.
      ifstream{root / "backward"}};

  // As indicated at https://howardhinnant.github.io/date/tz.html#Installation
  // For Windows another file seems to be required
  // https://raw.githubusercontent.com/unicode-org/cldr/master/common/supplemental/windowsZones.xml
  // This file seems to contain the mapping of Windows time zone name to IANA
  // time zone names.
  //
  // However this article mentions another way to do the mapping on Windows
  // https://devblogs.microsoft.com/oldnewthing/20210527-00/?p=105255
  // This requires Windows 10 Version 1903, which was released in May of 2019
  // and considered end of life in December 2020
  // https://learn.microsoft.com/en-us/lifecycle/announcements/windows-10-1903-end-of-servicing
  //
  // TODO TZDB Implement the Windows mapping in tzdb::current_zone

  array<istream*, time_zones.size()> time_zone_ptrs;
  ranges::transform(time_zones, time_zone_ptrs.begin(), [](ifstream& time_zone) { return std::addressof(time_zone); });

  ifstream leap_seconds{root / "leapseconds"};

  return __make_tzdb(version, span{time_zone_ptrs}, leap_seconds);
}



const tzdb& get_tzdb() {
  // TODO TZDB This function should use get_tzdb_list
  // The construction of tzdb_list may only throw runtime_error
  // TODO TZDB honour the contract when throwing.
  static tzdb result = __make_tzdb("/data/src/llvm-mordante/libcxx/utils/data/tzdb/data");
  return result;
}
#else

_LIBCPP_WEAK string_view __libcpp_tzdb_directory() {
#  ifndef _LIBCPP_CHRONO_TZDB_DIRECTORY
#    define _LIBCPP_CHRONO_TZDB_DIRECTORY "/usr/share/zoneinfo/"
#  endif
  return _LIBCPP_CHRONO_TZDB_DIRECTORY;
}

static void __matches(istream& __input, char __expected) {
  std::cerr << __PRETTY_FUNCTION__ << " '" << char(__input.peek()) << '/' << __input.peek() << "' == '" << __expected
            << "'?\n";

  if (__input.get() != __expected)
    std::__throw_runtime_error((string("corrupt tzdb: expected character '") + __expected + '\'').c_str());
}

static void __matches(istream& __input, string_view __expected) {
  std::cerr << __PRETTY_FUNCTION__ << " '" << __expected << "'\n";
  for (auto __c : __expected)
    if (__input.get() != __c)
      std::__throw_runtime_error((string("corrupt tzdb: expected string '") + string(__expected) + '\'').c_str());
}

[[nodiscard]] static bool __is_whitespace(int __c) {
  std::cerr << __PRETTY_FUNCTION__ << " '" << char(__c) << '/' << __c << "'\n";
  return __c == ' ' || __c == '\t';
}

[[nodiscard]] static bool __is_eol(int __c) {
  std::cerr << __PRETTY_FUNCTION__ << " '" << char(__c) << '/' << __c << "'\n";
  return __c == '\n' || __c == std::char_traits<char>::eof();
}

static void __skip_optional_whitespace(istream& __input) {
  std::cerr << __PRETTY_FUNCTION__ << "\n";
  while (__is_whitespace(__input.peek()))
    __input.get();
}

static void __skip_mandatory_whitespace(istream& __input) {
  std::cerr << __PRETTY_FUNCTION__ << "\n";
  if (!__is_whitespace(__input.get()))
    std::__throw_runtime_error("corrupt tzdb: expected whitespace");

  __skip_optional_whitespace(__input);
}

static void __skip_line(istream& __input) {
  std::cerr << __PRETTY_FUNCTION__ << "'\n";
  while (!__is_eol(__input.peek())) {
    __input.get();
  }
}

static void __skip(istream& __input, char __suffix) {
  std::cerr << __PRETTY_FUNCTION__ << "'" << __suffix << "'\n";
  if (__input.peek() == __suffix)
    __input.get();
}

static void __skip(istream& __input, string_view __suffix) {
  std::cerr << __PRETTY_FUNCTION__ << "'" << __suffix << "'\n";
  for (auto __c : __suffix)
    if (__input.peek() == __c)
      __input.get();
}

[[nodiscard]] static string __parse_string(istream& __input) {
  std::cerr << __PRETTY_FUNCTION__ << "\n";
  string __result;
  while (true) {
    int __c = __input.get();
    switch (__c) {
    case ' ':
    case '\t':
    case '\n':
      __input.unget();
      [[fallthrough]];
    case istream::traits_type::eof():
      if (__result.empty())
        std::__throw_runtime_error("corrupt tzdb: expected a string");

      std::cerr << __PRETTY_FUNCTION__ << " '" << __result << "'\n";
      return __result;

    default:
      __result.push_back(__c);
    }
  }
}

[[nodiscard]] static int __parse_integral(istream& __input, bool __leading_zero_allowed) {
  std::cerr << __PRETTY_FUNCTION__ << "\n";
  int __result = __input.get();
  if (__leading_zero_allowed) {
    if (__result < '0' || __result > '9')
      std::__throw_runtime_error("corrupt tzdb: expected a digit");
  } else {
    if (__result < '1' || __result > '9')
      std::__throw_runtime_error("corrupt tzdb: expected a non-zero digit");
  }
  __result -= '0';
  while (true) {
    if (__input.peek() < '0' || __input.peek() > '9') {
      std::cerr << __PRETTY_FUNCTION__ << " '" << __result << "'\n";
      return __result;
    }

    // The values are expected to be much smaller.
    // The expected maxima are:
    // - 8784 hours in a year
    // - 31 days in a month
    // - year no real maximum, these values are expected to be less than
    //   the range of the year type.
    if (__result > (std::numeric_limits<int>::max() / 16))
      std::__throw_runtime_error("corrupt tzdb: integral too large");

    __result *= 10;
    __result += __input.get() - '0';
  }
}

// The first line contains # version YYYYw
// The parser expects this pattern
// #\s*version\s*\(.*)
static string __parse_version(istream& __input) {
  chrono::__matches(__input, '#');
  chrono::__skip_optional_whitespace(__input);
  chrono::__matches(__input, "version");
  chrono::__skip_mandatory_whitespace(__input);
  return chrono::__parse_string(__input);
}

[[nodiscard]] static day __parse_day(istream& __input) {
  std::cerr << __PRETTY_FUNCTION__ << "\n";
  unsigned __result = __parse_integral(__input, false);
  if (__result > 31)
    std::__throw_runtime_error("corrupt tzdb day: value too large");
  return day{__result};
}

[[nodiscard]] static weekday __parse_weekday(istream& __input) {
  std::cerr << __PRETTY_FUNCTION__ << "\n";

  switch (__input.get()) {
  case 'F':
    chrono::__skip(__input, "riday");
    return Friday;

  case 'M':
    chrono::__skip(__input, "onday");
    return Monday;

  case 'S':
    switch (__input.get()) {
    case 'a':
      chrono::__skip(__input, "turday");
      return Saturday;

    case 'u':
      chrono::__skip(__input, "unday");
      return Sunday;
    }
    break;

  case 'T':
    switch (__input.get()) {
    case 'h':
      chrono::__skip(__input, "ursday");
      return Thursday;

    case 'u':
      chrono::__skip(__input, "esday");
      return Tuesday;
    }
    break;
  case 'W':
    chrono::__skip(__input, "ednesday");
    return Wednesday;
  }

  std::__throw_runtime_error("corrupt tzdb weekday: invalid name");
}

[[nodiscard]] static month __parse_month(istream& __input) {
  std::cerr << __PRETTY_FUNCTION__ << "\n";
  switch (__input.get()) {
  case 'A':
    switch (__input.get()) {
    case 'p':
      chrono::__skip(__input, "ril");
      return April;

    case 'u':
      chrono::__skip(__input, "gust");
      return August;
    }
    break;

  case 'D':
    chrono::__skip(__input, "ecember");
    return December;

  case 'F':
    chrono::__skip(__input, "ebruary");
    return February;

  case 'J':
    switch (__input.get()) {
    case 'a':
      chrono::__skip(__input, "nuary");
      return January;

    case 'u':
      switch (__input.get()) {
      case 'n':
        chrono::__skip(__input, 'e');
        return June;

      case 'l':
        chrono::__skip(__input, 'y');
        return July;
      }
    }
    break;

  case 'M':
    if (__input.get() == 'a')
      switch (__input.get()) {
      case 'y':
        return May;

      case 'r':
        chrono::__skip(__input, "ch");
        return March;
      }
    break;

  case 'N':
    chrono::__skip(__input, "ovember");
    return November;

  case 'O':
    chrono::__skip(__input, "ctober");
    return October;

  case 'S':
    chrono::__skip(__input, "eptember");
    return September;
  }
  std::__throw_runtime_error("corrupt tzdb month: invalid name");
}

[[nodiscard]] static year __parse_year_value(istream& __input) {
  std::cerr << __PRETTY_FUNCTION__ << "\n";
  bool __negative = __input.peek() == '-';
  if (__negative) [[unlikely]]
    __input.get();

  static_assert(year::min() == -year::max());
  int __result = std::min(__parse_integral(__input, true), static_cast<int>(year::max()));

  return year{__negative ? -__result : __result};
}

[[nodiscard]] static year __parse_year(istream& __input) {
  std::cerr << __PRETTY_FUNCTION__ << "\n";

  // Per man page
  //   Names must be in English and are case insensitive.  They appear in sev‐
  //   eral contexts, and include month and weekday names and keywords such as
  //   maximum,  only,  Rolling, and Zone.  A name can be abbreviated by omit‐
  //   ting all but an initial prefix; any abbreviation must be unambiguous in
  //   context.
  // Testing revealed. max hase been shipped as Max
  //
  // This is fixed in https://github.com/eggert/tz/commit/af242d11b62584808a66851b8707148bf1ee8d0a
  //
  // Allow Max too.
  //
  // TODO TZDB we need to parse more different databases to test for
  // more possible "corrupt" databases.
  if (__input.peek() == 'M') {
    std::cerr << __PRETTY_FUNCTION__ << " corrupt Max entry\n";
    __matches(__input, "Max");
    return year::max();
  }

  if (__input.peek() != 'm') [[likely]]
    return chrono::__parse_year_value(__input);

  std::cerr << __PRETTY_FUNCTION__ << " min or max\n";
  __input.get();
  switch (__input.peek()) {
  case 'i':
    __input.get();
    chrono::__skip(__input, 'n');
    [[fallthrough]];

  case ' ':
    // The m is minimum, even when that is ambigious.
    return year::min();

  case 'a':
    __input.get();
    chrono::__skip(__input, 'x');
    return year::max();
  }

  std::__throw_runtime_error("corrupt tzdb year: expected 'min' or 'max'");
}

[[nodiscard]] static year __parse_year(istream& __input, year __only) {
  std::cerr << __PRETTY_FUNCTION__ << "\n";
  if (__input.peek() != 'o')
    return chrono::__parse_year(__input);

  std::cerr << __PRETTY_FUNCTION__ << " only\n";
  __input.get();
  chrono::__skip(__input, "nly");
  return __only;
}

[[nodiscard]] static __constrained_weekday::__comparison_t __parse_comparison(istream& __input) {
  std::cerr << __PRETTY_FUNCTION__ << "\n";
  switch (__input.get()) {
  case '>':
    __matches(__input, '=');
    return __constrained_weekday::__ge;

  case '<':
    __matches(__input, '=');
    return __constrained_weekday::__le;
  }
  std::__throw_runtime_error("corrupt tzdb on: expected '>=' or '<='");
}

[[nodiscard]] static __time_zone_rule::__on_t __parse_on(istream& __input) {
  std::cerr << __PRETTY_FUNCTION__ << "\n";
  if (std::isdigit(__input.peek()))
    return __parse_day(__input);

  if (__input.peek() == 'l') {
    __matches(__input, "last");
    return weekday_last(__parse_weekday(__input));
  }

  return __constrained_weekday{
      __parse_weekday(__input),
      __parse_comparison(__input),
      __parse_day(__input),
  };
}

// The current database contains 1/100 th of a second resolution at most.
// However the input file tzdata.zi has rounded these values.
// This algorithms truncates the value.
/*
                    2            time in hours
                    2:00         time in hours and minutes
                    01:28:14     time in hours, minutes, and seconds
                    00:19:32.13  time with fractional seconds
                    12:00        midday, 12 hours after 00:00
                    15:00        3 PM, 15 hours after 00:00
                    24:00        end of day, 24 hours after 00:00
                    260:00       260 hours after 00:00
                    -2:30        2.5 hours before 00:00
                    -            equivalent to 0
*/
// a time field of often followed by a character so the routine stops when an
// unexpected value is found.  Validation is left to the caller.
[[nodiscard]] static seconds __parse_duration(istream& __input) {
  std::cerr << __PRETTY_FUNCTION__ << "\n";
  seconds __result{0};
  int __c         = __input.peek();
  bool __negative = __c == '-';
  if (__negative) {
    __input.get();
    // Negative is either a negative value or a single -.
    // The latter means 0 and the parsing is complete.
    if (!std::isdigit(__input.peek()))
      return __result;
  }

  __result += hours(__parse_integral(__input, true));
  if (__input.peek() != ':')
    return __negative ? -__result : __result;

  __input.get();
  __result += minutes(__parse_integral(__input, true));
  if (__input.peek() != ':')
    return __negative ? -__result : __result;

  __input.get();
  __result += seconds(__parse_integral(__input, true));
  if (__input.peek() != '.')
    return __negative ? -__result : __result;

  __input.get();
  (void)__parse_integral(__input, true); // Truncate the digits.

  return __negative ? -__result : __result;
}

[[nodiscard]] static __time_zone_rule::__clock __parse_clock(istream& __input) {
  std::cerr << __PRETTY_FUNCTION__ << "\n";
  switch (__input.get()) {
  case 'w':
    return __time_zone_rule::__clock::__local;
  case 's':
    return __time_zone_rule::__clock::__standard;

  case 'u':
  case 'g':
  case 'z':
    return __time_zone_rule::__clock::__universal;
  }

  __input.unget();
  return __time_zone_rule::__clock::__local;
}

[[nodiscard]] static bool __parse_dst(istream& __input, seconds __offset) {
  std::cerr << __PRETTY_FUNCTION__ << "\n";
  switch (__input.get()) {
  case 's':
    return false;

  case 'd':
    return true;
  }

  __input.unget();
  return __offset != 0s;
}

[[nodiscard]] static __time_zone_rule::__save_t __parse_save(istream& __input) {
  std::cerr << __PRETTY_FUNCTION__ << "\n";

  seconds __time = chrono::__parse_duration(__input);
  return {__time, chrono::__parse_dst(__input, __time)};
}

[[nodiscard]] static string __parse_letters(istream& __input) {
  std::cerr << __PRETTY_FUNCTION__ << "\n";
  string __result = __parse_string(__input);

  return __result != "-" ? __result : "";
}

static void __parse_rule(tzdb& __tzdb, istream& __input) {
  std::cerr << __PRETTY_FUNCTION__ << "\n";
  chrono::__skip_mandatory_whitespace(__input);
  string __name = chrono::__parse_string(__input);

  if (__tzdb.__rules.empty() || __tzdb.__rules.back().first != __name)
    __tzdb.__rules.emplace_back(__name, vector<__time_zone_rule>{});

  __time_zone_rule& __rule = __tzdb.__rules.back().second.emplace_back();

  chrono::__skip_mandatory_whitespace(__input);
  __rule.__from = chrono::__parse_year(__input);
  chrono::__skip_mandatory_whitespace(__input);
  __rule.__to = chrono::__parse_year(__input, __rule.__from);
  chrono::__skip_mandatory_whitespace(__input);
  chrono::__matches(__input, '-');
  chrono::__skip_mandatory_whitespace(__input);
  __rule.__in_month = chrono::__parse_month(__input);
  chrono::__skip_mandatory_whitespace(__input);
  __rule.__on = chrono::__parse_on(__input);
  chrono::__skip_mandatory_whitespace(__input);
  __rule.__at_time  = __parse_duration(__input);
  __rule.__at_clock = __parse_clock(__input);
  chrono::__skip_mandatory_whitespace(__input);
  __rule.__save = __parse_save(__input);
  chrono::__skip_mandatory_whitespace(__input);
  __rule.__letters = chrono::__parse_letters(__input);
  __skip_line(__input);
}

[[nodiscard]] static time_zone::__element::__rules_t __parse_rules(istream& __input) {
  int __c = __input.peek();
  // A single -  is not a SAVE but a special case.
  if (__c == '-') {
    __input.get();
    if (chrono::__is_whitespace(__input.peek()))
      return monostate{};
    __input.unget();
    return __parse_save(__input);
  }

  if (isdigit(__c) || __c == '+')
    return __parse_save(__input);

  return __parse_string(__input);
}

static void __parse_zone(tzdb& __tzdb, istream& __input) {
  std::cerr << __PRETTY_FUNCTION__ << "\n";

  // XXX should skip "one", similar for Rule this needs a test too

  chrono::__skip_mandatory_whitespace(__input);
  vector<time_zone::__element>& __elements = __tzdb.zones.emplace_back(chrono::__parse_string(__input)).__elements();
  chrono::__skip_mandatory_whitespace(__input);

  do {
    // The first line must be valid, continuations are optional.
    time_zone::__element& __element = __elements.emplace_back();

    // Note STDOFF is specified as
    //   This field has the same format as the AT and SAVE fields of rule lines;
    // These fields have different suffix letters, these letters seem
    // not to be used so do not allow any of them.

    __element.__stdoff = chrono::__parse_duration(__input);
    chrono::__skip_mandatory_whitespace(__input);
    __element.__rules = chrono::__parse_rules(__input);
    chrono::__skip_mandatory_whitespace(__input);
    __element.__format = chrono::__parse_string(__input);
    chrono::__skip_optional_whitespace(__input);
    // TODO TZDB improve this code.
    // We can't continue since the loop has more to do, however this
    // code is ugly.
    if (!__is_eol(__input.peek())) {
      __element.__year = chrono::__parse_year(__input);
      chrono::__skip_optional_whitespace(__input);
      if (!__is_eol(__input.peek())) {
        __element.__in_month = chrono::__parse_month(__input);
        chrono::__skip_optional_whitespace(__input);
        if (!__is_eol(__input.peek())) {
          __element.__on = chrono::__parse_on(__input);
          chrono::__skip_optional_whitespace(__input);
          if (!__is_eol(__input.peek())) {
            __element.__at_time  = __parse_duration(__input);
            __element.__at_clock = __parse_clock(__input);
          }
        }
      }
    }

    __skip_line(__input);
    chrono::__skip_optional_whitespace(__input);
  } while (isdigit(__input.peek()) || __input.peek() == '-');
}

static void __parse_link(tzdb& __tzdb, istream& __input) {
  std::cerr << __PRETTY_FUNCTION__ << "\n";

  chrono::__skip_mandatory_whitespace(__input);
  string __name = chrono::__parse_string(__input);
  chrono::__skip_mandatory_whitespace(__input);
  string __target = chrono::__parse_string(__input);
  __skip_line(__input);

  __tzdb.links.emplace_back(std::move(__name), std::move(__target));
}

static void __parse_tzdata(tzdb& __db, istream& __input) {
  std::cerr << __PRETTY_FUNCTION__ << "\n";
  while (true) {
    int __c = __input.get();

    switch (__c) {
    case istream::traits_type::eof():
      return;

    case '\n':
      break;

    case '#':
      chrono::__skip_line(__input);
      break;

    case 'R':
      chrono::__parse_rule(__db, __input);
      break;

    case 'Z':
      chrono::__parse_zone(__db, __input);
      break;

    case 'L':
      chrono::__parse_link(__db, __input);
      break;

    default:
      __skip_line(__input);
      break; // should be an error but skip for now.
    }
  }
}

static tzdb __make_tzdb() {
  std::cerr << __PRETTY_FUNCTION__ << " dir " << chrono::__libcpp_tzdb_directory() << '\n';

  tzdb __result;

  filesystem::path __root = chrono::__libcpp_tzdb_directory();
  //  ifstream __leap_seconds{__root / "leap_seconds"};

  ifstream __tzdata{__root / "tzdata.zi"};

  __result.version = chrono::__parse_version(__tzdata);
  chrono::__parse_tzdata(__result, __tzdata);
  std::ranges::sort(__result.__rules, {}, [](const auto& p) { return p.first; });
  std::ranges::sort(__result.zones);
  std::ranges::sort(__result.links);
  /*
        __parse_leap_seconds(__result, __tzdata);
    */

  return __result;
}

_LIBCPP_NODISCARD_EXT _LIBCPP_AVAILABILITY_TZDB _LIBCPP_EXPORTED_FROM_ABI tzdb_list& get_tzdb_list() {
  static tzdb_list __result{chrono::__make_tzdb()};
  return __result;
}

_LIBCPP_AVAILABILITY_TZDB _LIBCPP_EXPORTED_FROM_ABI const tzdb& reload_tzdb() {
  if (remote_version() == tzdb().version)
    return get_tzdb_list().front();

  return get_tzdb_list().__emplace_front(chrono::__make_tzdb());
}

// TODO TZBD
// - Update synopsis
// - Add tests
_LIBCPP_NODISCARD_EXT _LIBCPP_AVAILABILITY_TZDB _LIBCPP_EXPORTED_FROM_ABI string remote_version() {
  std::cerr << __PRETTY_FUNCTION__ << " dir " << chrono::__libcpp_tzdb_directory() << '\n';
  filesystem::path __root = chrono::__libcpp_tzdb_directory();
  ifstream __tzdata{__root / "tzdata.zi"};
  return chrono::__parse_version(__tzdata);
}

#endif
} // namespace chrono

_LIBCPP_END_NAMESPACE_STD
