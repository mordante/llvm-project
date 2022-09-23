// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// For information see https://libcxx.llvm.org/DesignDocs/TimeZone.html

#ifndef _LIBCPP___CHRONO_TIME_ZONE_H
#define _LIBCPP___CHRONO_TIME_ZONE_H

#include <version>
// Enable the contents of the header only when libc++ was built with experimental features enabled.
#if !defined(_LIBCPP_HAS_NO_INCOMPLETE_TZDB)

#  include <__chrono/duration.h>
#  include <__chrono/time_zone_rule.h>
#  include <__chrono/year.h>
#  include <__compare/strong_order.h>
#  include <__config>
#  include <optional> // TODO TZDB remove
#  include <string>
#  include <string_view>
#  include <vector>

#  if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#    pragma GCC system_header
#  endif

_LIBCPP_BEGIN_NAMESPACE_STD

#  if _LIBCPP_STD_VER > 17

namespace chrono {

// TODO TZBD
// - implement according to Standard
// - Update synopsis
// - Add tests
struct time_zone {
public:
  explicit time_zone(string&& __name) : __name_(std::move(__name)) {}

  // TODO TZDB Add tests
  _LIBCPP_HIDE_FROM_ABI string_view name() const noexcept { return __name_; }

  // private:
  //  TODO TZDB create a proper constructor instead of using public members.
  string __name_;

  struct __element {
    seconds __stdoff;

    // The RULES is either a SAVE or a NAME.
    // The size_t is used as cache. After loading the rules they are
    // sorted and remain stable, then an index in the vector can be
    // used.
    // If this field is - then standard  time  always  applies, this is
    // indicated by the monostate.
    using __rules_t = variant<monostate, __time_zone_rule::__save_t, string, size_t>;

    __rules_t __rules;

    string __format;
    // TODO TZDB until can be contain more than just a year.
    // Parts of the UNTIL, the optional parts are default initialized
    //    optional<year> __until_;
    year __year = year::min();
    month __in_month{January}; // __in is a reserved name
    __time_zone_rule::__on_t __on{chrono::day(1)};

    seconds __at_time{0};
    __time_zone_rule::__clock __at_clock{__time_zone_rule::__clock::__local};
  };

  vector<__element> __elements_;

  _LIBCPP_HIDE_FROM_ABI vector<__element>& __elements() { return __elements_; }
  _LIBCPP_HIDE_FROM_ABI const vector<__element>& __elements() const { return __elements_; }
};

// TODO TZDB Add tests
_LIBCPP_HIDE_FROM_ABI inline bool operator==(const time_zone& __x, const time_zone& __y) noexcept {
  return __x.name() == __y.name();
}

// TODO TZDB Add tests
_LIBCPP_HIDE_FROM_ABI inline strong_ordering operator<=>(const time_zone& __x, const time_zone& __y) noexcept {
  return __x.name() <=> __y.name();
}

} // namespace chrono

#  endif //_LIBCPP_STD_VER > 17

_LIBCPP_END_NAMESPACE_STD

#endif // !defined(_LIBCPP_HAS_NO_INCOMPLETE_TZDB)

#endif // _LIBCPP___CHRONO_TIME_ZONE_H
