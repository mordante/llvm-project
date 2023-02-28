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
#include <span>

export module std:span;
export namespace std {

  // constants
  using std::dynamic_extent;

  // [views.span], class template span
  using std::span;

  using std::ranges::enable_borrowed_range;
  using std::ranges::enable_view;

  // [span.objectrep], views of object representation
  using std::as_bytes;

  using std::as_writable_bytes;

} // namespace std
