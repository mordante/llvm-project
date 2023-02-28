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
#include <expected> // Since C++23

export module std:expected;
export namespace std {

  // [expected.unexpected], class template unexpected
  using std::unexpected;

  // [expected.bad], class template bad_expected_access
  using std::bad_expected_access;

  // [expected.bad.void], specialization for void

  // in-place construction of unexpected values
  using std::unexpect_t;

  using std::unexpect;

  // [expected.expected], class template expected
  using std::expected;

  // [expected.void], partial specialization of expected for void types

} // namespace std
