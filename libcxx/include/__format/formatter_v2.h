// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___FORMAT_FORMATTER_V2_H
#define _LIBCPP___FORMAT_FORMATTER_V2_H

#include <__algorithm/copy.h>
#include <__algorithm/fill_n.h>
#include <__config>
#include <__format/format_error.h>
#include <__format/format_fwd.h> // for __char_type TODO FMT Move the concept?
#include <__format/format_parse_context.h>
#include <__format/formatter_floating_point.h>
#include <__format/formatter_integral.h>
#include <__format/formatter_output.h>
#include <__format/parser_std_format_spec.h>
#include <__format/parser_std_format_spec_v2.h>
#include <__utility/move.h>
#include <__utility/unreachable.h>
#include <charconv>
#include <limits>
#include <string_view>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

#if _LIBCPP_STD_VER > 17

namespace __formatter {

//
// Formatter string
//

template <class _CharT>
_LIBCPP_HIDE_FROM_ABI auto __format_string(basic_string_view<_CharT> __str,
                                           output_iterator<const _CharT&> auto __out_it,
                                           __format_spec::__parser<_CharT> __parser) {
  return __formatter::__write_unicode(__str, _VSTD::move(__out_it), __parser);
}
//
// Formatter pointer
//

template <class _CharT>
_LIBCPP_HIDE_FROM_ABI auto __format_pointer(const void* __ptr, auto& __ctx, __format_spec::__parser<_CharT> __parser) {
  _LIBCPP_ASSERT(__parser.__alignment_ != __format_spec::__alignment::__default,
                 "The call to parse should have updated the alignment");

  __parser.__alternate_form_ = true;
  __parser.__type_ = __format_spec::__type::__hexadecimal_lower_case;
  return __format_integer(reinterpret_cast<uintptr_t>(__ptr), __ctx, __parser);
}

} // namespace __formatter

/// The default formatter template.
///
/// [format.formatter.spec]/5
/// If F is a disabled specialization of formatter, these values are false:
/// - is_default_constructible_v<F>,
/// - is_copy_constructible_v<F>,
/// - is_move_constructible_v<F>,
/// - is_copy_assignable<F>, and
/// - is_move_assignable<F>.
template <class _Tp, class _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter {
  formatter() = delete;
  formatter(const formatter&) = delete;
  formatter& operator=(const formatter&) = delete;
};

//
// Formatter char
//

template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT __formatter_char {
public:
  _LIBCPP_HIDE_FROM_ABI constexpr auto parse(basic_format_parse_context<_CharT>& __parse_ctx)
      -> decltype(__parse_ctx.begin()) {
    auto __result = __parser_.__parse(__parse_ctx, __format_spec::__fields_integral);
    __format_spec::__process_parsed_char(__parser_);
    return __result;
  }

  _LIBCPP_HIDE_FROM_ABI auto format(_CharT __value, auto& __ctx) const -> decltype(__ctx.out()) {
    if (__parser_.__type_ == __format_spec::__type::__char)
      return __formatter::__format_char(__value, __ctx.out(), __parser_.__resolve_dynamic_sizes(__ctx));

    if constexpr (sizeof(_CharT) <= sizeof(int)) {
      // Promotes _CharT to an integral type. This reduces the number of
      // instantiations of __format_integer reducing code size.
      if constexpr (is_signed_v<_CharT>)
        return __formatter::__format_integer(static_cast<int>(__value), __ctx,
                                             __parser_.__resolve_dynamic_sizes(__ctx));
      else
        return __formatter::__format_integer(static_cast<unsigned>(__value), __ctx,
                                             __parser_.__resolve_dynamic_sizes(__ctx));
    } else
      return __formatter::__format_integer(__value, __ctx, __parser_.__resolve_dynamic_sizes(__ctx));
  }

  _LIBCPP_HIDE_FROM_ABI auto format(char __value, auto& __ctx) const -> decltype(__ctx.out())
    requires(same_as<_CharT, wchar_t>)
  {
    return format(static_cast<wchar_t>(__value), __ctx);
  }

  __format_spec::__parser<_CharT> __parser_;
};

template <>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<char, char> : public __formatter_char<char> {};

#    ifndef _LIBCPP_HAS_NO_WIDE_CHARACTERS
template <>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<char, wchar_t> : public __formatter_char<wchar_t> {};

template <>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<wchar_t, wchar_t> : public __formatter_char<wchar_t> {
};

#    endif // _LIBCPP_HAS_NO_WIDE_CHARACTERS

//
// Formatter arithmetic (bool)
//

template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<bool, _CharT> {
public:
  _LIBCPP_HIDE_FROM_ABI constexpr auto parse(basic_format_parse_context<_CharT>& __parse_ctx)
      -> decltype(__parse_ctx.begin()) {
    auto __result = __parser_.__parse(__parse_ctx, __format_spec::__fields_integral);
    __format_spec::__process_parsed_bool(__parser_);
    return __result;
  }

  _LIBCPP_HIDE_FROM_ABI auto format(bool __value, auto& __ctx) const -> decltype(__ctx.out()) {
    switch (__parser_.__type_) {
    case __format_spec::__type::__string:
      return __formatter::__format_bool(__value, __ctx, __parser_.__resolve_dynamic_sizes(__ctx));

    case __format_spec::__type::__binary_lower_case:
    case __format_spec::__type::__binary_upper_case:
    case __format_spec::__type::__octal:
    case __format_spec::__type::__decimal:
    case __format_spec::__type::__hexadecimal_lower_case:
    case __format_spec::__type::__hexadecimal_upper_case:
      // Casting this to a formatter more likely being used reduces the binary size.
      return __formatter::__format_integer(static_cast<unsigned >(__value), __ctx,
                                           __parser_.__resolve_dynamic_sizes(__ctx));

    default:
      _LIBCPP_ASSERT(false, "The parse function should have validated the type");
      __libcpp_unreachable();
    }
  }

  __format_spec::__parser<_CharT> __parser_;
};

//
// Formatter arithmetic (integer)
//

template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT __formatter_integer {
public:
  _LIBCPP_HIDE_FROM_ABI constexpr auto parse(basic_format_parse_context<_CharT>& __parse_ctx)
      -> decltype(__parse_ctx.begin()) {
    auto __result = __parser_.__parse(__parse_ctx, __format_spec::__fields_integral);
    __format_spec::__process_parsed_integer(__parser_);
    return __result;
  }

  // TODO might want to reduce the number of instantiations
  // but need to measure the size of them (eg 64 bit and 128 bit version)
  // One formatter integral and char
  template <integral _Tp>
  _LIBCPP_HIDE_FROM_ABI auto format(_Tp __value, auto& __ctx) const -> decltype(__ctx.out()) {
    return format(__value, __ctx, __parser_.__resolve_dynamic_sizes(__ctx));
  }

  template <integral _Tp>
  static _LIBCPP_HIDE_FROM_ABI auto format(_Tp __value, auto& __ctx, __format_spec::__parser<_CharT> __parser)
      -> decltype(__ctx.out()) {
    if (__parser.__type_ == __format_spec::__type::__char)
      return __formatter::__format_char(__value, __ctx.out(), __parser);

    // Reduce the number of instantiation of the integer formatter
    if constexpr (is_signed_v<_Tp>) {
      if constexpr (sizeof(_Tp) <= sizeof(int))
        return __formatter::__format_integer(static_cast<int>(__value), __ctx, __parser);
      else if constexpr (sizeof(_Tp) <= sizeof(long long))
        return __formatter::__format_integer(static_cast<long long>(__value), __ctx, __parser);
#    ifndef _LIBCPP_HAS_NO_INT128
      else if constexpr (sizeof(_Tp) <= sizeof(__int128_t))
        return __formatter::__format_integer(static_cast<__int128_t>(__value), __ctx, __parser);
#    endif
      else
        static_assert(sizeof(_Tp) == 0, "Unsupported type");
    } else {
      if constexpr (sizeof(_Tp) <= sizeof(unsigned))
        return __formatter::__format_integer(static_cast<unsigned>(__value), __ctx, __parser);
      else if constexpr (sizeof(_Tp) <= sizeof(unsigned long long))
        return __formatter::__format_integer(static_cast<unsigned long long>(__value), __ctx, __parser);
#    ifndef _LIBCPP_HAS_NO_INT128
      else if constexpr (sizeof(_Tp) <= sizeof(__uint128_t))
        return __formatter::__format_integer(static_cast<__uint128_t>(__value), __ctx, __parser);
#    endif
      else
        static_assert(sizeof(_Tp) == 0, "Unsupported type");
    }
  }

  __format_spec::__parser<_CharT> __parser_;
};

// Signed integral types.
template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<signed char, _CharT>
    : public __formatter_integer<_CharT> {};
template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<short, _CharT> : public __formatter_integer<_CharT> {
};
template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<int, _CharT> : public __formatter_integer<_CharT> {};
template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<long, _CharT> : public __formatter_integer<_CharT> {};
template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<long long, _CharT>
    : public __formatter_integer<_CharT> {};
#    ifndef _LIBCPP_HAS_NO_INT128
template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<__int128_t, _CharT>
    : public __formatter_integer<_CharT> {
  using _Base = __formatter_integer<_CharT>;

  _LIBCPP_HIDE_FROM_ABI auto format(__int128_t __value, auto& __ctx) const -> decltype(__ctx.out()) {
    // TODO FMT Implement full 128 bit support.
    using _To = long long;
    if (__value < numeric_limits<_To>::min() || __value > numeric_limits<_To>::max())
      __throw_format_error("128-bit value is outside of implemented range");

    return _Base::format(static_cast<_To>(__value), __ctx);
  }
};
#    endif

// Unsigned integral types.
template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<unsigned char, _CharT>
    : public __formatter_integer<_CharT> {};
template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<unsigned short, _CharT>
    : public __formatter_integer<_CharT> {};
template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<unsigned, _CharT>
    : public __formatter_integer<_CharT> {};
template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<unsigned long, _CharT>
    : public __formatter_integer<_CharT> {};
template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<unsigned long long, _CharT>
    : public __formatter_integer<_CharT> {};
#    ifndef _LIBCPP_HAS_NO_INT128
template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<__uint128_t, _CharT>
    : public __formatter_integer<_CharT> {
  using _Base = __formatter_integer<_CharT>;

  _LIBCPP_HIDE_FROM_ABI auto format(__uint128_t __value, auto& __ctx) const -> decltype(__ctx.out()) {
    // TODO FMT Implement full 128 bit support.
    using _To = unsigned long long;
    if (__value < numeric_limits<_To>::min() || __value > numeric_limits<_To>::max())
      __throw_format_error("128-bit value is outside of implemented range");

    return _Base::format(static_cast<_To>(__value), __ctx);
  }
};
#    endif

//
// Formatter arithmetic (floating-point)
//

template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS __formatter_floating_point {
public:
  _LIBCPP_HIDE_FROM_ABI constexpr auto parse(basic_format_parse_context<_CharT>& __parse_ctx)
      -> decltype(__parse_ctx.begin()) {
    auto __result = __parser_.__parse(__parse_ctx, __format_spec::__fields_floating_point);
    __format_spec::__process_parsed_floating_point(__parser_);
    return __result;
  }

  template <floating_point _Tp>
  _LIBCPP_HIDE_FROM_ABI auto format(_Tp __value, auto& __ctx) const -> decltype(__ctx.out()) {
    return __formatter::__format_floating_point(__value, __ctx, __parser_.__resolve_dynamic_sizes(__ctx));
  }

  __format_spec::__parser<_CharT> __parser_;
};

template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<float, _CharT>
    : public __formatter_floating_point<_CharT> {};
template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<double, _CharT>
    : public __formatter_floating_point<_CharT> {};
template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<long double, _CharT>
    : public __formatter_floating_point<_CharT> {};

//
// Formatter string
//

template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS __formatter_string {
public:
  _LIBCPP_HIDE_FROM_ABI constexpr auto parse(basic_format_parse_context<_CharT>& __parse_ctx)
      -> decltype(__parse_ctx.begin()) {
    auto __result = __parser_.__parse(__parse_ctx, __format_spec::__fields_string);
    __format_spec::__process_display_type_string(__parser_.__type_);
    return __result;
  }

  _LIBCPP_HIDE_FROM_ABI auto format(basic_string_view<_CharT> __str, auto& __ctx) const -> decltype(__ctx.out()) {
    return __formatter::__format_string(__str, __ctx.out(), __parser_.__resolve_dynamic_sizes(__ctx));
  }

  __format_spec::__parser<_CharT> __parser_;
};

template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<const _CharT*, _CharT>
    : public __formatter_string<_CharT> {
  using _Base = __formatter_string<_CharT>;

  // XXX const qualify
  _LIBCPP_HIDE_FROM_ABI auto format(const _CharT* __str, auto& __ctx) const -> decltype(__ctx.out()) {
    _LIBCPP_ASSERT(__str, "The basic_format_arg constructor should have "
                          "prevented an invalid pointer.");

    // When using a center or right alignment and the width option the length
    // of __str must be known to add the padding upfront. This case is handled
    // by the base class by converting the argument to a basic_string_view.
    //
    // When using left alignment and the width option the padding is added
    // after outputting __str so the length can be determined while outputting
    // __str. The same holds true for the precision, during outputting __str it
    // can be validated whether the precision threshold has been reached. For
    // now these optimizations aren't implemented. Instead the base class
    // handles these options.
    // TODO FMT Implement these improvements.
    if (_Base::__parser_.__width_as_arg_ || _Base::__parser_.__precision_as_arg_ || //
        _Base::__parser_.__has_width() || _Base::__parser_.__has_precision())
      return _Base::format(basic_string_view<_CharT>(__str), __ctx);

    // No formatting required, copy the string to the output.
    auto __out_it = __ctx.out();
    while (*__str)
      *__out_it++ = *__str++;
    return __out_it;
  }
};

// Formatter char*.
template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<_CharT*, _CharT>
    : public formatter<const _CharT*, _CharT> {
  using _Base = formatter<const _CharT*, _CharT>;

  _LIBCPP_HIDE_FROM_ABI auto format(_CharT* __str, auto& __ctx) const -> decltype(__ctx.out()) {
    return _Base::format(__str, __ctx);
  }
};

// Formatter const char[].
template <__formatter::__char_type _CharT, size_t _Size>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<const _CharT[_Size], _CharT>
    : public __formatter_string<_CharT> {
  using _Base = __formatter_string<_CharT>;

  _LIBCPP_HIDE_FROM_ABI auto format(const _CharT __str[_Size], auto& __ctx) const -> decltype(__ctx.out()) {
    return _Base::format(basic_string_view<_CharT>(__str, _Size), __ctx);
  }
};

// Formatter std::string.
template <__formatter::__char_type _CharT, class _Traits, class _Allocator>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<basic_string<_CharT, _Traits, _Allocator>, _CharT>
    : public __formatter_string<_CharT> {
  using _Base = __formatter_string<_CharT>;

  _LIBCPP_HIDE_FROM_ABI auto format(const basic_string<_CharT, _Traits, _Allocator>& __str, auto& __ctx) const
      -> decltype(__ctx.out()) {
    // drop _Traits and _Allocator
    return _Base::format(basic_string_view<_CharT>(__str.data(), __str.size()), __ctx);
  }
};

// Formatter std::string_view.
template <__formatter::__char_type _CharT, class _Traits>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<basic_string_view<_CharT, _Traits>, _CharT>
    : public __formatter_string<_CharT> {
  using _Base = __formatter_string<_CharT>;

  _LIBCPP_HIDE_FROM_ABI auto format(basic_string_view<_CharT, _Traits> __str, auto& __ctx) const
      -> decltype(__ctx.out()) {
    // drop _Traits
    return _Base::format(basic_string_view<_CharT>(__str.data(), __str.size()), __ctx);
  }
};

//
// Formatter pointer
//

template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS __formatter_pointer {
public:
  __formatter_pointer() { __parser_.__alignment_ = __format_spec::__alignment::__right; }

  _LIBCPP_HIDE_FROM_ABI constexpr auto parse(basic_format_parse_context<_CharT>& __parse_ctx)
      -> decltype(__parse_ctx.begin()) {
    auto __result = __parser_.__parse(__parse_ctx, __format_spec::__fields_pointer);
    __format_spec::__process_display_type_pointer(__parser_.__type_);
    return __result;
  }

  _LIBCPP_HIDE_FROM_ABI auto format(const void* __ptr, auto& __ctx) const -> decltype(__ctx.out()) {
    return __formatter::__format_pointer(__ptr, __ctx, __parser_.__resolve_dynamic_sizes(__ctx));
  }

  __format_spec::__parser<_CharT> __parser_;
};

// [format.formatter.spec]/2.4
// For each charT, the pointer type specializations template<>
// - struct formatter<nullptr_t, charT>;
// - template<> struct formatter<void*, charT>;
// - template<> struct formatter<const void*, charT>;
template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<nullptr_t, _CharT>
    : public __formatter_pointer<_CharT> {};
template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<void*, _CharT> : public __formatter_pointer<_CharT> {
};
template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<const void*, _CharT>
    : public __formatter_pointer<_CharT> {};

#endif //_LIBCPP_STD_VER > 17

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif // _LIBCPP___FORMAT_FORMATTER_V2_H
