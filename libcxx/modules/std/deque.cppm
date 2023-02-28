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
#include <deque>

export module std:deque;
export namespace std {

  // [deque], class template deque
  using std::deque;

  using std::operator==;
  using std::operator<=>;

  using std::swap;

  // [deque.erasure], erasure
  using std::erase;
  using std::erase_if;

  namespace pmr {
    using std::pmr::deque;
  }

  // Note formatter not in synopsis
  // TODO Test whether the ommission fails tests.

} // namespace std
