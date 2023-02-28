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

export module std:cinttypes;
export namespace std {
  using std::imaxdiv_t;

  using std::imaxabs;
  using std::imaxdiv;
  using std::strtoimax;
  using std::strtoumax;
  using std::wcstoimax;
  using std::wcstoumax;

  // abs is conditionally here, but always present in std-cmath.cppm. To avoid
  // conflicing declarations omit the using here.
  using std::div _LIBCPP_USING_IF_EXISTS;
} // namespace std
