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
#include <unordered_set>

export module std:unordered_set;
export namespace std {
  // [unord.set], class template unordered_­set
  using std::unordered_set;

  // [unord.multiset], class template unordered_­multiset
  using std::unordered_multiset;

  using std::operator==;

  using std::swap;

  // [unord.set.erasure], erasure for unordered_­set
  using std::erase_if;

  // [unord.multiset.erasure], erasure for unordered_­multiset

  namespace pmr {
    using std::pmr::unordered_set;

    using std::pmr::unordered_multiset;
  } // namespace pmr
} // namespace std
