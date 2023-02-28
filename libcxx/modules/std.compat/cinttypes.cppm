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
#include <cinttypes>

export module std.compat:cinttypes;
export {
  using ::imaxdiv_t;

  using ::imaxabs;
  using ::imaxdiv;
  using ::strtoimax;
  using ::strtoumax;
  using ::wcstoimax;
  using ::wcstoumax;

  using ::abs _LIBCPP_USING_IF_EXISTS;
  using ::div _LIBCPP_USING_IF_EXISTS;
} // export
