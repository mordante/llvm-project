// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// For information see https://libcxx.llvm.org/DesignDocs/TimeZone.html

#ifndef _LIBCPP___CHRONO_TIME_ZONE_RULE_H
#define _LIBCPP___CHRONO_TIME_ZONE_RULE_H

#include <version>
// Enable the contents of the header only when libc++ was built with experimental features enabled.
#if !defined(_LIBCPP_HAS_NO_INCOMPLETE_TZDB)

#  include <__chrono/day.h>
#  include <__chrono/duration.h>
#  include <__chrono/month.h>
#  include <__chrono/year.h>
#  include <__config>
#  include <string>
#  include <variant>

#  if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#    pragma GCC system_header
#  endif

_LIBCPP_BEGIN_NAMESPACE_STD

#  if _LIBCPP_STD_VER > 17

namespace chrono {

// Sun>=8   first Sunday on or after the eighth
// Sun<=25  last Sunday on or before the 25th
struct __constrained_weekday {
  year_month_day operator()(year __year, month __month);

  weekday __weekday;
  enum __comparison_t { __le, __ge } __comparison;
  day __day;
};

// The names of the fields match the fields of a Rule.
struct __time_zone_rule {
  year __from;
  year __to;

  month __in_month; // __in is a reserved name

  // The on field has a few alternative presentations
  //  5        the fifth of the month
  //  lastSun  the last Sunday in the month
  //  lastMon  the last Monday in the month
  //  Sun>=8   first Sunday on or after the eighth
  //  Sun<=25  last Sunday on or before the 25th

  using __on_t = variant<day, weekday_last, __constrained_weekday>;
  __on_t __on;

  enum class __clock { __local, __standard, __universal };

  seconds __at_time;
  __clock __at_clock;

  struct __save_t {
    seconds __time;
    bool __is_dst;
  };

  __save_t __save;

  string __letters;
};

} // namespace chrono

#  endif //_LIBCPP_STD_VER > 17

_LIBCPP_END_NAMESPACE_STD

#endif // !defined(_LIBCPP_HAS_NO_INCOMPLETE_TZDB)

#endif // _LIBCPP___CHRONO_TIME_ZONE_RULE_H
