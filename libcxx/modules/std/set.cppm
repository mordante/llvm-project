# 1 __FILE__ 1 3
// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

module;
#include <set>

export module std:set;
export namespace std {
  // [set], class template set
  using std::set;

  using std::operator==;
  using std::operator<=>;

  using std::swap;

  // [set.erasure], erasure for set
  using std::erase_if;

  // [multiset], class template multiset
  using std::multiset;

  // [multiset.erasure], erasure for multiset
  using std::erase_if;

  namespace pmr {
    using std::pmr::set;

    using std::pmr::multiset;
  } // namespace pmr

} // namespace std
