// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// For information see https://libcxx.llvm.org/DesignDocs/TimeZone.html

#ifndef _LIBCPP___CHRONO_TZDB_LIST_H
#define _LIBCPP___CHRONO_TZDB_LIST_H

#include <version>
// Enable the contents of the header only when libc++ was built with experimental features enabled.
#if !defined(_LIBCPP_HAS_NO_INCOMPLETE_TZDB)

#  include <__availability>
#  include <__chrono/tzdb.h>
#  include <forward_list>
#  include <shared_mutex>

#  if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#    pragma GCC system_header
#  endif

_LIBCPP_BEGIN_NAMESPACE_STD

#  if _LIBCPP_STD_VER > 17

namespace chrono {

class _LIBCPP_AVAILABILITY_TZDB tzdb_list {
public:
  _LIBCPP_HIDE_FROM_ABI explicit tzdb_list(tzdb&& __tzdb) { __tzdb_.push_front(std::move(__tzdb)); }

  tzdb_list(const tzdb_list&)            = delete;
  tzdb_list& operator=(const tzdb_list&) = delete;

  using const_iterator = forward_list<tzdb>::const_iterator;

  _LIBCPP_NODISCARD_EXT _LIBCPP_HIDE_FROM_ABI const tzdb& front() const noexcept {
    shared_lock __lock{__mutex_};
    return __tzdb_.front();
  }

  _LIBCPP_HIDE_FROM_ABI const_iterator erase_after(const_iterator __p) {
    unique_lock __lock{__mutex_};
    return __tzdb_.erase_after(__p);
  }

  _LIBCPP_NODISCARD_EXT _LIBCPP_HIDE_FROM_ABI tzdb& __emplace_front(tzdb&& __tzdb) {
    unique_lock __lock{__mutex_};
    return __tzdb_.emplace_front(std::move(__tzdb));
  }

  _LIBCPP_NODISCARD_EXT _LIBCPP_HIDE_FROM_ABI const_iterator begin() const noexcept {
    shared_lock __lock{__mutex_};
    return __tzdb_.begin();
  }
  _LIBCPP_NODISCARD_EXT _LIBCPP_HIDE_FROM_ABI const_iterator end() const noexcept {
    //  forward_list<T>::end does not access the list.
    return __tzdb_.end();
  }

  _LIBCPP_NODISCARD_EXT _LIBCPP_HIDE_FROM_ABI const_iterator cbegin() const noexcept {
    shared_lock __lock{__mutex_};
    return __tzdb_.cbegin();
  }
  _LIBCPP_NODISCARD_EXT _LIBCPP_HIDE_FROM_ABI const_iterator cend() const noexcept {
    //  forward_list<T>::end does not access the list.
    return __tzdb_.cend();
  }

private:
  mutable shared_mutex __mutex_;
  forward_list<tzdb> __tzdb_;
};

// TODO TZBD
// - Update synopsis
// - Add tests
_LIBCPP_NODISCARD_EXT _LIBCPP_AVAILABILITY_TZDB _LIBCPP_EXPORTED_FROM_ABI tzdb_list& get_tzdb_list();

// TODO TZBD
// - Update synopsis
// - Add tests
_LIBCPP_AVAILABILITY_TZDB _LIBCPP_EXPORTED_FROM_ABI const tzdb& reload_tzdb();

// TODO TZBD
// - Update synopsis
// - Add tests
_LIBCPP_NODISCARD_EXT _LIBCPP_AVAILABILITY_TZDB _LIBCPP_EXPORTED_FROM_ABI string remote_version();

// TODO TZBD
// - Update synopsis
// - Add tests
_LIBCPP_NODISCARD_EXT _LIBCPP_AVAILABILITY_TZDB _LIBCPP_HIDE_FROM_ABI inline const tzdb& get_tzdb() {
  return get_tzdb_list().front();
}

} // namespace chrono

#  endif //_LIBCPP_STD_VER > 17

_LIBCPP_END_NAMESPACE_STD

#endif // !defined(_LIBCPP_HAS_NO_INCOMPLETE_TZDB)

#endif // _LIBCPP___CHRONO_TZDB_LIST_H
