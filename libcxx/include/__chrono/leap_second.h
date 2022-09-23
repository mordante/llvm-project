// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// For information see https://libcxx.llvm.org/DesignDocs/TimeZone.html

#ifndef _LIBCPP___CHRONO_LEAP_SECOND_H
#define _LIBCPP___CHRONO_LEAP_SECOND_H

#include <version>
// Enable the contents of the header only when libc++ was built with experimental features enabled.
#if !defined(_LIBCPP_HAS_NO_INCOMPLETE_TZDB)

#  include <__chrono/day.h>
#  include <__chrono/month.h>
#  include <__chrono/year.h>
#  include <__config>
#  include <string> // TODO TZDB remove

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
class leap_second {
public:
  year __year_;
  month __month_;
  day __day_;

  // TODO TZDB validate the hms type can handle 60 seconds.
  string __hms_;

  enum : bool { __plus, __minus } __corr_;

  enum : bool { __r, __s } __rs_;
};

} // namespace chrono

#  endif //_LIBCPP_STD_VER > 17

_LIBCPP_END_NAMESPACE_STD

#endif // !defined(_LIBCPP_HAS_NO_INCOMPLETE_TZDB)

#endif // _LIBCPP___CHRONO_LEAP_SECOND_H
