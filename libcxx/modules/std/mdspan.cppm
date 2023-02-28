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
#if __has_include(<mdspan>)
#  error "include this header unconditionally and uncomment the exported symbols"
#  include <mdspan>
#endif

export module std:mdspan;
export namespace std {
#if 0
  // [mdspan.extents], class template extents
  using std::extents;

  // [mdspan.extents.dextents], alias template dextents
  using std::dextents;

  // [mdspan.layout], layout mapping
  using std::layout_left;
  using std::layout_right;
  using std::layout_stride;

  // [mdspan.accessor.default], class template default_­accessor
  using std::default_accessor;

  // [mdspan.mdspan], class template mdspan
  using std::mdspan;
#endif
} // namespace std
