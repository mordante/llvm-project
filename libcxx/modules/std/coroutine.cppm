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
#include <coroutine>
#include <functional>
export module std:coroutine;
export namespace std {
  using std::coroutine_handle;
  using std::coroutine_traits;
  using std::operator==;
  using std::operator<=>;
  using std::noop_coroutine;
  using std::suspend_always;
  using std::suspend_never;

  using std::hash;
} // namespace std
