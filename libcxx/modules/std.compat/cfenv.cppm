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
#include <cfenv>

export module std.compat:cfenv;
export {
  // types
  using ::fenv_t;
  using ::fexcept_t;

  // functions
  using ::feclearexcept;
  using ::fegetexceptflag;
  using ::feraiseexcept;
  using ::fesetexceptflag;
  using ::fetestexcept;

  using ::fegetround;
  using ::fesetround;

  using ::fegetenv;
  using ::feholdexcept;
  using ::fesetenv;
  using ::feupdateenv;

} // export
