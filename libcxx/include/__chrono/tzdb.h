// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// For information see https://libcxx.llvm.org/DesignDocs/TimeZone.html

#ifndef _LIBCPP___CHRONO_TZDB_H
#define _LIBCPP___CHRONO_TZDB_H

#include <version>
// Enable the contents of the header only when libc++ was built with experimental features enabled.
#if !defined(_LIBCPP_HAS_NO_INCOMPLETE_TZDB)

#  include <__algorithm/ranges_lower_bound.h>
#  include <__availability>
#  include <__chrono/leap_second.h>
#  include <__chrono/time_zone.h>
#  include <__chrono/time_zone_link.h>
#  include <__chrono/time_zone_rule.h>
#  include <__config>
#  include <__memory/addressof.h>
#  include <iosfwd>
#  include <span>
#  include <stdexcept>
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
struct _LIBCPP_AVAILABILITY_TZDB tzdb {
  string version;
  vector<pair<string, vector<__time_zone_rule>>> __rules;
  vector<time_zone> zones;
  vector<time_zone_link> links;
  vector<leap_second> leap_seconds;

  _LIBCPP_HIDE_FROM_ABI const time_zone* locate_zone(string_view __name) const {
    if (const time_zone* __result = __find_in_zone(__name); __result)
      return __result;

    if (auto __it = ranges::lower_bound(links, __name, {}, &time_zone_link::name);
        __it != links.end() && __it->name() == __name)
      if (const time_zone* __result = __find_in_zone(__it->target()); __result)
        return __result;

    std::__throw_runtime_error("tzdb: requested time zone not found");
  }

private:
  _LIBCPP_HIDE_FROM_ABI const time_zone* __find_in_zone(string_view __name) const noexcept {
    if (auto __it = ranges::lower_bound(zones, __name, {}, &time_zone::name);
        __it != zones.end() && __it->name() == __name)
      return std::addressof(*__it);

    return nullptr;
  }
};

// Creates a tzdb object from the files in __directory.
//
// This directory must be valid and contain the unpacked contents of a tzdata
// tarball as distributed by IANA. For example this file contains the expected
// data https://data.iana.org/time-zones/releases/tzdata2022d.tar.gz
// TODO TZDB make this a constructor for tzdb_list.
_LIBCPP_AVAILABILITY_TZDB tzdb __make_tzdb(string_view __directory);

// TODO TZDB make this a constructor for tzdb_list.
_LIBCPP_AVAILABILITY_TZDB tzdb __make_tzdb(istream& __version, span<istream*> __time_zones, istream& __leap_seconds);

// This function allows users to override the directory where the TZDB
// files are stored. This should rarely be needed, a better solution
// would be to set the proper value at build time in
// _LIBCPP_CHRONO_TZDB_DIRECTORY
_LIBCPP_AVAILABILITY_TZDB _LIBCPP_OVERRIDABLE_FUNC_VIS string_view __libcpp_tzdb_directory();

} // namespace chrono

#  endif //_LIBCPP_STD_VER > 17

_LIBCPP_END_NAMESPACE_STD

#endif // !defined(_LIBCPP_HAS_NO_INCOMPLETE_TZDB)

#endif // _LIBCPP___CHRONO_TZDB_H
