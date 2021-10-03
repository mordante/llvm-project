// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___FORMAT_FORMAT_ARG_STORE_H
#define _LIBCPP___FORMAT_FORMAT_ARG_STORE_H

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

#include <__config>
#include <__format/format_arg.h>
#include <__iterator/data.h>
#include <__iterator/size.h>
#include <array>
#include <cstring>
#include <string>
#include <string_view>
#include <type_traits>

_LIBCPP_BEGIN_NAMESPACE_STD

#if _LIBCPP_STD_VER > 17

// TODO FMT Remove this once we require compilers with proper C++20 support.
// If the compiler has no concepts support, the format header will be disabled.
// Without concepts support enable_if needs to be used and that too much effort
// to support compilers with partial C++20 support.
#  if !defined(_LIBCPP_HAS_NO_CONCEPTS)

namespace __format {

/// Contains the meta data of a formatting argument.
///
/// [format.args]/1
/// ... Implementations should optimize the representation of basic_format_args
/// for a small number of formatting arguments.
///
/// The format-arg-store contains this optimization by storing an array with the
/// packed meta data and an array with the packed data. The meta data array
/// contains one element per formatting argument. This meta data holds the
/// offset of the argument in the packed data array and the type of the
/// argument. (By knowing the type the size of the argument is known.)
///
/// To reduce the size required the meta data is packed. This limits the size of
/// the formatting argument types and the number of elements. The rational:
/// - The number of \ref __type used is 16. This fits in 4 bits. Using 4 bits will
///   lead to an ABI break when a new type is added. Adding new types is
///   probably rare, but adding one shouldn't result in an ABI break. To avoid
///   this issue the number of possible types is doubled.
/// - The offset is stored in 27 bits. The largest element requires 128 bits of
///   storage. This means when using the largest type 2^20 elements can be used.
///   It's unlikely the compiler can handle this number of arguments.
struct _LIBCPP_TYPE_VIS __arg_meta_data {
  uint32_t __offset : 27;
  __arg_t __type : 5;
};

static_assert(sizeof(__arg_meta_data) == sizeof(uint32_t));

// TODO FMT consider a variable storage type.
// It would be possible to optimize the size based on the size of the
// storage of arguments:
// - offset < 8   => uint8_t  with 3 bits for the offset (1  large element)
// - offset < 512 => uint16_t with 9 bits for the offset (32 large elements)
// Where a large element takes 16 bytes of storage.
// (Note offset means we discard the size of the last argument.)
//
// We can then store the meta-data pointer as a tagged pointer
// ptr & 0x03:
// 0 => __arg_meta_data_8
// 1 => __arg_meta_data_16
// 2 => __arg_meta_data
// 3 could be used for future expansion to a 64-bit type.

/// \returns the type based in the stored type
///
/// \pre \c _Tp is a valid type for the "variant".
///
/// \note Whether the \c char of \c wchar_t type is "active" depends on the
/// context used. But since this function doesn't need to do validation there's
/// no dependency on the context.
template <class _Tp>
consteval __arg_t __get_arg_t() {
  if constexpr (same_as<_Tp, bool>)
    return __arg_t::__boolean;
  else if constexpr (same_as<_Tp, char> _LIBCPP_IF_WIDE_CHARACTERS(|| same_as<_Tp, wchar_t>))
    return __arg_t::__char_type;
  else if constexpr (same_as<_Tp, int>)
    return __arg_t::__int;
  else if constexpr (same_as<_Tp, long long>)
    return __arg_t::__long_long;
#    ifndef _LIBCPP_HAS_NO_INT128
  else if constexpr (same_as<_Tp, __int128_t>)
    return __arg_t::__i128;
#    endif
  else if constexpr (same_as<_Tp, unsigned>)
    return __arg_t::__unsigned;
  else if constexpr (same_as<_Tp, unsigned long long>)
    return __arg_t::__unsigned_long_long;
#    ifndef _LIBCPP_HAS_NO_INT128
  else if constexpr (same_as<_Tp, __uint128_t>)
    return __arg_t::__u128;
#    endif
  else if constexpr (same_as<_Tp, float>)
    return __arg_t::__float;
  else if constexpr (same_as<_Tp, double>)
    return __arg_t::__double;
  else if constexpr (same_as<_Tp, long double>)
    return __arg_t::__long_double;
  else if constexpr (same_as<_Tp, const char*> _LIBCPP_IF_WIDE_CHARACTERS(|| same_as<_Tp, const wchar_t*>))
    return __arg_t::__const_char_type_ptr;
  else if constexpr (same_as<_Tp, basic_string_view<char>> _LIBCPP_IF_WIDE_CHARACTERS(
                         || same_as<_Tp, basic_string_view<wchar_t>>))
    return __arg_t::__string_view;
  else if constexpr (same_as<_Tp, const void*>)
    return __arg_t::__ptr;
  else
    return __arg_t::__handle;
}

template <class _Tp>
struct __storage_type {
  using type = _Tp;
  static constexpr __arg_t __arg = __get_arg_t<_Tp>();
};

template <class _Context, class _Tp>
consteval auto __make_storage_type();

// The types that:
// - are valid regardless of the context
// - are stored without conversion
template <class, class _Tp>
requires(same_as<_Tp, bool> || same_as<_Tp, float> || same_as<_Tp, double> || same_as<_Tp, long double>) //
    consteval auto __make_storage_type() {
  return __storage_type<_Tp>{};
}

// The character class conversions.
template <class _Context, class _CharT>
requires(same_as<typename _Context::char_type, _CharT>) //
    consteval auto __make_storage_type() {
  return __storage_type<_CharT>{};
}

#    ifndef _LIBCPP_HAS_NO_WIDE_CHARACTERS
template <class _Context, class _CharT>
requires(same_as<typename _Context::char_type, wchar_t>&& same_as<_CharT, char>) //
    consteval auto __make_storage_type() {
  return __storage_type<wchar_t>{};
}
#    endif

// The signed integer types
template <class, __libcpp_signed_integer _Tp>
consteval auto __make_storage_type() {
  if constexpr (sizeof(_Tp) <= sizeof(int))
    return __storage_type<int>{};
  else if constexpr (sizeof(_Tp) <= sizeof(long long))
    return __storage_type<long long>{};
#    ifndef _LIBCPP_HAS_NO_INT128
  else if constexpr (sizeof(_Tp) == sizeof(__int128_t))
    return __storage_type<__int128_t>{};
#    endif
  else
    static_assert(sizeof(_Tp) == 0, "An unsupported signed integer was used");
}

// The unsigned integer types
template <class, __libcpp_unsigned_integer _Tp>
consteval auto __make_storage_type() {
  if constexpr (sizeof(_Tp) <= sizeof(unsigned))
    return __storage_type<unsigned>{};
  else if constexpr (sizeof(_Tp) <= sizeof(unsigned long long))
    return __storage_type<unsigned long long>{};
#    ifndef _LIBCPP_HAS_NO_INT128
  else if constexpr (sizeof(_Tp) == sizeof(__uint128_t))
    return __storage_type<__uint128_t>{};
#    endif
  else
    static_assert(sizeof(_Tp) == 0, "An unsupported unsigned integer was used");
}

// Char pointer
template <class _Context, class _Tp>
requires(same_as<typename _Context::char_type*, _Tp> || same_as<const typename _Context::char_type*, _Tp>) //
    consteval auto __make_storage_type() {
  return __storage_type<const typename _Context::char_type*>{};
}

// Char array
template <class _Context, class _Tp>
requires(is_array_v<_Tp>&& same_as<_Tp, typename _Context::char_type[extent_v<_Tp>]>) //
    consteval auto __make_storage_type() {
  return __storage_type<basic_string_view<typename _Context::char_type>>{};
}

// String view
template <class _Context, class _Tp>
requires(same_as<typename _Context::char_type, typename _Tp::value_type>&&
             same_as<_Tp, basic_string_view<typename _Tp::value_type,
                                            typename _Tp::traits_type>>) //
    consteval auto __make_storage_type() {
  return __storage_type<basic_string_view<typename _Context::char_type>>{};
}

// String to string_view
template <class _Context, class _Tp>
requires(same_as<typename _Context::char_type, typename _Tp::value_type>&&
             same_as<_Tp, basic_string<typename _Tp::value_type, typename _Tp::traits_type,
                                       typename _Tp::allocator_type>>) //
    consteval auto __make_storage_type() {
  return __storage_type<basic_string_view<typename _Context::char_type>>{};
}

// Pointers
template <class, class _Ptr>
requires(same_as<_Ptr, void*> || same_as<_Ptr, const void*> || same_as<_Ptr, nullptr_t>) //
    consteval auto __make_storage_type() {
  return __storage_type<const void*>{};
}

// Handle.
// Note this version can't be constrained avoiding ambiguous selections.
// That means it can be instantiated by disabled formatters. To solve this a
// constrained version for disabled formatters is added. This overload is
// marked as deleted to fail creating a storage type for a disabled formatter.
template <class _Context, class _Tp>
consteval auto __make_storage_type() {
  return __storage_type<typename basic_format_arg<_Context>::handle>{};
}

// TODO FMT Use P2286's formattable concept.
// The current implementation tests whether the formatter is disabled, this works
// but having a proper formattable concept would be the proper test.
template <class _Context, class _Tp>
requires(!is_default_constructible_v<typename _Context::template formatter_type<_Tp>>) //
    consteval auto __make_storage_type() = delete;

template <class _Context, class _Tp, class... _Args>
consteval void __get_meta_data(uint32_t __offset, __arg_meta_data* __meta_data) {
  auto __s = __make_storage_type<_Context, _Tp>();
  *__meta_data = {__offset, __s.__arg};
  if constexpr (sizeof...(_Args))
    __get_meta_data<_Context, _Args...>(__offset + sizeof(typename decltype(__s)::type), ++__meta_data);
}

template <class _Context, class... _Args>
consteval array<__arg_meta_data, sizeof...(_Args)> __get_meta_data() {
  if constexpr (sizeof...(_Args) == 0)
    return array<__arg_meta_data, 0>{};
  else {
    array<__arg_meta_data, sizeof...(_Args)> __result;
    __get_meta_data<_Context, _Args...>(0, __result.data());
    return __result;
  }
}

template <class _Context, class _Tp, class... _Args>
consteval size_t __get_storage_size(size_t __size) {
  auto __s = __make_storage_type<_Context, _Tp>();
  __size += sizeof(typename decltype(__s)::type);
  if constexpr (sizeof...(_Args))
    return __get_storage_size<_Context, _Args...>(__size);
  else
    return __size;
}

template <class _Context, class... _Args>
consteval size_t __get_storage_size() {
  if constexpr (sizeof...(_Args))
    return __get_storage_size<_Context, _Args...>(0);
  else
    // A C-array shouldn't be zero sized return a dummy element.
    return 1;
}

template <class _Sp>
struct _LIBCPP_TYPE_VIS __storage {
  _Sp __value;
};

// Most exposition only constructors in [format.arg] are noexcept.
template <class _Context, class _Tp, class... _Args>
_LIBCPP_HIDE_FROM_ABI constexpr void __store_data(char* __data, _Tp&& __v, _Args&&... __args) noexcept {
  using _Sp = typename decltype(__make_storage_type<_Context, remove_cvref_t<_Tp>>())::type;
  if constexpr (same_as<_Sp, basic_string_view<typename _Context::char_type>>) {
    // When the _Traits or _Allocator are different an implicit conversion will
    // fail. Instead of adding special cases to __storage<_Sp> handle the
    // special case here. (This leads to a small code duplication.)
    //
    // Note since the input can be an array use the non-member functions to
    // extract the constructor arguments.
    basic_string_view<typename _Context::char_type> __s{_VSTD::data(__v), _VSTD::size(__v)};
    _VSTD::memcpy(static_cast<void*>(__data), static_cast<const void*>(_VSTD::addressof(__s)), sizeof(_Sp));
  } else if constexpr (same_as<_Sp, typename basic_format_arg<_Context>::handle>) {
    auto __handle = basic_format_arg<_Context>::handle::__create(_VSTD::forward<_Tp>(__v));
    _VSTD::memcpy(static_cast<void*>(__data), static_cast<const void*>(_VSTD::addressof(__handle)), sizeof(_Sp));
  } else {
    __storage<_Sp> __s{_VSTD::forward<_Tp>(__v)};
    _VSTD::memcpy(static_cast<void*>(__data), static_cast<const void*>(_VSTD::addressof(__s.__value)), sizeof(_Sp));
  }

  if constexpr (sizeof...(_Args))
    __store_data<_Context>(__data + sizeof(_Sp), _VSTD::forward<_Args>(__args)...);
}

} // namespace __format

template <class _Context, class... _Args>
struct _LIBCPP_TEMPLATE_VIS __format_arg_store {
  // Most exposition only constructors in [format.arg] are noexcept.
  _LIBCPP_HIDE_FROM_ABI
  constexpr __format_arg_store(_Args&&... __args) noexcept {
    if constexpr (sizeof...(_Args))
      __format::__store_data<_Context>(__data, _VSTD::forward<_Args>(__args)...);
  }

  static constexpr array<__format::__arg_meta_data, sizeof...(_Args)> __meta_data{
      __format::__get_meta_data<_Context, remove_cvref_t<_Args>...>()};

  /// Contains the packed arguments of the formatting function.
  ///
  /// When formatting some argument types are converted. For example \c short is
  /// promoted to an \c int. A \c std::string is converted to a
  /// \c std::string_view. This buffer contains all elements already converted
  /// to the type used in the \c std::formatter.
  char __data[__format::__get_storage_size<_Context, remove_cvref_t<_Args>...>()];
};

#  endif // !defined(_LIBCPP_HAS_NO_CONCEPTS)

#endif //_LIBCPP_STD_VER > 17

_LIBCPP_END_NAMESPACE_STD

#endif // _LIBCPP___FORMAT_FORMAT_ARG_STORE_H
