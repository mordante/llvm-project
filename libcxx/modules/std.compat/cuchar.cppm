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
#include <cuchar>

export module std.compat:cuchar;
export {
  // Note the Standard does not mark these symbols optional, but libc++'s header
  // does. So this seems strictly not to be conforming.

  using ::mbstate_t _LIBCPP_USING_IF_EXISTS;
  using ::size_t _LIBCPP_USING_IF_EXISTS;

  using ::mbrtoc8 _LIBCPP_USING_IF_EXISTS;
  using ::c8rtomb _LIBCPP_USING_IF_EXISTS;
  using ::mbrtoc16 _LIBCPP_USING_IF_EXISTS;
  using ::c16rtomb _LIBCPP_USING_IF_EXISTS;
  using ::mbrtoc32 _LIBCPP_USING_IF_EXISTS;
  using ::c32rtomb _LIBCPP_USING_IF_EXISTS;
} // export
