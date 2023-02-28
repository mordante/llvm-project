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
#include <functional> // needed for unwrap_ref_decay (is this a bug?)
#include <type_traits>
export module std:type_traits;
export namespace std {
  using std::bool_constant;
  using std::false_type;
  using std::integral_constant;
  using std::true_type;

  // helper traits
  using std::conditional;
  using std::enable_if;

  // Primary classification traits:
  using std::is_array;
  using std::is_class;
  using std::is_enum;
  using std::is_floating_point;
  using std::is_function;
  using std::is_integral;
  using std::is_lvalue_reference;
  using std::is_member_function_pointer;
  using std::is_member_object_pointer;
  using std::is_null_pointer; // C++14
  using std::is_pointer;
  using std::is_rvalue_reference;
  using std::is_union;
  using std::is_void;

  // Secondary classification traits:
  using std::is_arithmetic;
  using std::is_compound;
  using std::is_fundamental;
  using std::is_member_pointer;
  using std::is_object;
  using std::is_reference;
  using std::is_scalar;
#if _LIBCPP_STD_VER >= 23    // TODO Do we want to provide std module in C++20
  using std::is_scoped_enum; // C++2b
#endif

  // Const-volatile properties and transformations:
  using std::add_const;
  using std::add_cv;
  using std::add_volatile;
  using std::is_const;
  using std::is_volatile;
  using std::remove_const;
  using std::remove_cv;
  using std::remove_volatile;

  // Reference transformations:
  using std::add_lvalue_reference;
  using std::add_rvalue_reference;
  using std::remove_reference;

  // Pointer transformations:
  using std::add_pointer;
  using std::remove_pointer;

  using std::type_identity; // C++20
  using std::type_identity_t;

  // Integral properties:
  using std::is_signed;
  using std::is_unsigned;
  using std::make_signed;
  using std::make_unsigned;

  // Array properties and transformations:
  using std::extent;
  using std::rank;
  using std::remove_all_extents;
  using std::remove_extent;

  using std::is_bounded_array;   // C++20
  using std::is_unbounded_array; // C++20

  // Member introspection:
  using std::is_abstract;
  using std::is_aggregate; // C++17
  using std::is_empty;
  using std::is_final;     // C++14
  using std::is_pod;
  using std::is_polymorphic;
  using std::is_standard_layout;
  using std::is_trivial;
  using std::is_trivially_copyable;

  using std::is_assignable;
  using std::is_constructible;
  using std::is_copy_assignable;
  using std::is_copy_constructible;
  using std::is_default_constructible;
  using std::is_destructible;
  using std::is_move_assignable;
  using std::is_move_constructible;
  using std::is_swappable;      // C++17
  using std::is_swappable_with; // C++17

  using std::is_trivially_assignable;
  using std::is_trivially_constructible;
  using std::is_trivially_copy_assignable;
  using std::is_trivially_copy_constructible;
  using std::is_trivially_default_constructible;
  using std::is_trivially_destructible;
  using std::is_trivially_move_assignable;
  using std::is_trivially_move_constructible;

  using std::is_nothrow_assignable;
  using std::is_nothrow_constructible;
  using std::is_nothrow_copy_assignable;
  using std::is_nothrow_copy_constructible;
  using std::is_nothrow_default_constructible;
  using std::is_nothrow_destructible;
  using std::is_nothrow_move_assignable;
  using std::is_nothrow_move_constructible;
  using std::is_nothrow_swappable;      // C++17
  using std::is_nothrow_swappable_with; // C++17

  using std::has_virtual_destructor;

  using std::has_unique_object_representations; // C++17

  // Relationships between types:
  using std::is_base_of;
  using std::is_same;

  using std::is_convertible;
  using std::is_nothrow_convertible;   // C++20
  using std::is_nothrow_convertible_v; // C++20

  using std::is_invocable;
  using std::is_invocable_r;

  using std::is_nothrow_invocable;
  using std::is_nothrow_invocable_r;

  // Alignment properties and transformations:
  using std::aligned_storage;
  using std::aligned_union;
  using std::alignment_of;
  using std::remove_cvref; // C++20

  using std::common_type;
  using std::decay;
  using std::invoke_result; // C++17
  using std::underlying_type;
  using std::unwrap_ref_decay;

  // const-volatile modifications:
  using std::add_const_t;
  using std::add_cv_t;
  using std::add_volatile_t;
  using std::remove_const_t;
  using std::remove_cv_t;
  using std::remove_volatile_t;

  // reference modifications:
  using std::add_lvalue_reference_t;
  using std::add_rvalue_reference_t;
  using std::remove_reference_t;

  // sign modifications:
  using std::make_signed_t;
  using std::make_unsigned_t;

  // array modifications:
  using std::remove_all_extents_t;
  using std::remove_extent_t;

  using std::is_bounded_array_v;   // C++20
  using std::is_unbounded_array_v; // C++20

  // pointer modifications:
  using std::add_pointer_t;    // C++14
  using std::remove_pointer_t; // C++14

  // other transformations:
  using std::aligned_storage_t; // C++14
  using std::aligned_union_t;   // C++14
  using std::common_type_t;     // C++14
  using std::conditional_t;     // C++14
  using std::decay_t;           // C++14
  using std::enable_if_t;       // C++14
  using std::invoke_result_t;
  using std::remove_cvref_t;    // C++20
  using std::underlying_type_t;
  using std::unwrap_ref_decay_t;

  using std::void_t;

  // See C++14 20.10.4.1, primary type categories
  using std::is_array_v;
  using std::is_class_v;
  using std::is_enum_v;
  using std::is_floating_point_v;
  using std::is_function_v;
  using std::is_integral_v;
  using std::is_lvalue_reference_v;
  using std::is_member_function_pointer_v;
  using std::is_member_object_pointer_v;
  using std::is_null_pointer_v;
  using std::is_pointer_v;
  using std::is_rvalue_reference_v;
  using std::is_union_v;
  using std::is_void_v;

  // See C++14 20.10.4.2, composite type categories
  using std::is_arithmetic_v;
  using std::is_compound_v;
  using std::is_fundamental_v;
  using std::is_member_pointer_v;
  using std::is_object_v;
  using std::is_reference_v;
  using std::is_scalar_v;

#if _LIBCPP_STD_VER >= 23 // TODO Do we want to provide std module in C++20
  using std::is_scoped_enum_v;
#endif

  // See C++14 20.10.4.3, type properties
  using std::has_unique_object_representations_v; // C++17;
  using std::has_virtual_destructor_v;
  using std::is_abstract_v;
  using std::is_aggregate_v;
  using std::is_assignable_v;
  using std::is_const_v;
  using std::is_constructible_v;
  using std::is_copy_assignable_v;
  using std::is_copy_constructible_v;
  using std::is_default_constructible_v;
  using std::is_destructible_v;
  using std::is_empty_v;
  using std::is_final_v;
  using std::is_move_assignable_v;
  using std::is_move_constructible_v;
  using std::is_nothrow_assignable_v;
  using std::is_nothrow_constructible_v;
  using std::is_nothrow_copy_assignable_v;
  using std::is_nothrow_copy_constructible_v;
  using std::is_nothrow_default_constructible_v;
  using std::is_nothrow_destructible_v;
  using std::is_nothrow_move_assignable_v;
  using std::is_nothrow_move_constructible_v;
  using std::is_nothrow_swappable_v;
  using std::is_nothrow_swappable_with_v;
  using std::is_pod_v;
  using std::is_polymorphic_v;
  using std::is_signed_v;
  using std::is_standard_layout_v;
  using std::is_swappable_v;
  using std::is_swappable_with_v;
  using std::is_trivial_v;
  using std::is_trivially_assignable_v;
  using std::is_trivially_constructible_v;
  using std::is_trivially_copy_assignable_v;
  using std::is_trivially_copy_constructible_v;
  using std::is_trivially_copyable_v;
  using std::is_trivially_default_constructible_v;
  using std::is_trivially_destructible_v;
  using std::is_trivially_move_assignable_v;
  using std::is_trivially_move_constructible_v;
  using std::is_unsigned_v;
  using std::is_volatile_v;

  // See C++14 20.10.5, type property queries
  using std::alignment_of_v; // C++17
  using std::extent_v;       // C++17
  using std::rank_v;         // C++17

  // See C++14 20.10.6, type relations
  using std::is_base_of_v;
  using std::is_convertible_v;
  using std::is_invocable_r_v;
  using std::is_invocable_v;
  using std::is_nothrow_invocable_r_v;
  using std::is_nothrow_invocable_v;
  using std::is_same_v;

  // [meta.logical], logical operator traits:                        // C++17
  using std::conjunction_v;
  using std::disjunction_v;
  using std::negation_v;

  // Move to proper section
  using std::is_constant_evaluated;
  using std::negation;
} // namespace std
