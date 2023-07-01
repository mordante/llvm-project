// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// For information see https://libcxx.llvm.org/DesignDocs/TimeZone.html

#ifndef _LIBCPP___CHRONO_TIME_ZONE_LINK_H
#define _LIBCPP___CHRONO_TIME_ZONE_LINK_H

#include <version>
// Enable the contents of the header only when libc++ was built with experimental features enabled.
#if !defined(_LIBCPP_HAS_NO_INCOMPLETE_TZDB)

#  include <__compare/strong_order.h>
#  include <__config>
#  include <string>
#  include <string_view>

#  if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#    pragma GCC system_header
#  endif

_LIBCPP_BEGIN_NAMESPACE_STD

#  if _LIBCPP_STD_VER >= 20

namespace chrono {

class time_zone_link {
public:
  _LIBCPP_HIDE_FROM_ABI explicit time_zone_link(string_view __name, string_view __target)
      : __name_{__name}, __target_{__target} {}

  _LIBCPP_HIDE_FROM_ABI explicit time_zone_link(string&& __name, string&& __target)
      : __name_{std::move(__name)}, __target_{std::move(__target)} {}

  time_zone_link(time_zone_link&&)            = default;
  time_zone_link& operator=(time_zone_link&&) = default;

  _LIBCPP_HIDE_FROM_ABI string_view name() const noexcept { return __name_; }
  _LIBCPP_HIDE_FROM_ABI string_view target() const noexcept { return __target_; }

private:
  string __name_;
  // TODO TZDB instead of the name we can store the pointer to a zone. These
  // pointers are immutable. This makes it possible to directly return a
  // pointer in the time_zone in the 'locate_zone' function.
  string __target_;
};

_LIBCPP_HIDE_FROM_ABI inline bool operator==(const time_zone_link& __x, const time_zone_link& __y) noexcept {
  return __x.name() == __y.name();
}

_LIBCPP_HIDE_FROM_ABI inline strong_ordering
operator<=>(const time_zone_link& __x, const time_zone_link& __y) noexcept {
  return __x.name() <=> __y.name();
}

} // namespace chrono

#  endif //_LIBCPP_STD_VER >= 20

_LIBCPP_END_NAMESPACE_STD

#endif // !defined(_LIBCPP_HAS_NO_INCOMPLETE_TZDB)

#endif // _LIBCPP___CHRONO_TIME_ZONE_LINK_H
