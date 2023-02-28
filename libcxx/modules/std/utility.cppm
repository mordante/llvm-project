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
#include <utility>
export module std:utility;
export namespace std {
  using std::swap;

  namespace rel_ops {
    using rel_ops::operator!=;
    using rel_ops::operator>;
    using rel_ops::operator<=;
    using rel_ops::operator>=;
  } // namespace rel_ops

  using std::as_const;
  using std::cmp_equal;
  using std::cmp_greater;       // C++20
  using std::cmp_greater_equal; // C++20
  using std::cmp_less;          // C++20
  using std::cmp_less_equal;    // C++20
  using std::cmp_not_equal;     // C++20
  using std::declval;
  using std::forward;
// TODO Even when we don't want to provide std module in C++20 this is kept for now.
// This allows testing whether this way works properly. That will be required for
// symbols that will be added in C++26.
#if _LIBCPP_STD_VER >= 23 // TODO Do we want to provide std module in C++20
  using std::forward_like;
#endif
  using std::in_range; // C++20
  using std::move;
  using std::move_if_noexcept;

  using std::pair;
  using std::operator==;

  using std::make_pair;
  using std::piecewise_construct_t;
  // FIXME: We can't export non-inline constexpr variables.
  // using std::piecewise_construct;

  using std::tuple_element;
  using std::tuple_size;

  using std::get;
  using std::index_sequence;
  using std::index_sequence_for;
  using std::integer_sequence;
  using std::make_index_sequence;
  using std::make_integer_sequence;

  using std::exchange;
  using std::in_place;
  using std::in_place_t;

  using std::in_place_index_t;
  using std::in_place_type;
  using std::in_place_type_t;

  using std::in_place_index;
#if _LIBCPP_STD_VER >= 23 // TODO Do we want to provide std module in C++20
  using std::to_underlying;
#endif
} // namespace std
