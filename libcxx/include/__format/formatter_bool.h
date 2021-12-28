// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___FORMAT_FORMATTER_BOOL_H
#define _LIBCPP___FORMAT_FORMATTER_BOOL_H

#include <__availability>
#include <__config>
#include <__format/format_error.h>
#include <__format/format_fwd.h>
#include <__format/formatter.h>
#include <__format/formatter_integral.h>
#include <__format/parser_std_format_spec.h>
#include <__format/parser_std_format_spec_v2.h>
#include <string_view>

#ifndef _LIBCPP_HAS_NO_LOCALIZATION
#  include <locale>
#endif

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

#if _LIBCPP_STD_VER > 17

// TODO FMT Remove this once we require compilers with proper C++20 support.
// If the compiler has no concepts support, the format header will be disabled.
// Without concepts support enable_if needs to be used and that too much effort
// to support compilers with partial C++20 support.
#  if !defined(_LIBCPP_HAS_NO_CONCEPTS)
#    if 0
namespace __format_spec {

template <class _CharT>
class _LIBCPP_TEMPLATE_VIS __parser_bool : public __parser_integral<_CharT> {
public:
  _LIBCPP_HIDE_FROM_ABI constexpr auto parse(auto& __parse_ctx)
      -> decltype(__parse_ctx.begin()) {
    auto __it = __parser_integral<_CharT>::__parse(__parse_ctx);

    switch (this->__type) {
    case _Flags::_Type::__default:
      this->__type = _Flags::_Type::__string;
      [[fallthrough]];
    case _Flags::_Type::__string:
      this->__handle_bool();
      break;

    case _Flags::_Type::__char:
      this->__handle_char();
      break;

    case _Flags::_Type::__binary_lower_case:
    case _Flags::_Type::__binary_upper_case:
    case _Flags::_Type::__octal:
    case _Flags::_Type::__decimal:
    case _Flags::_Type::__hexadecimal_lower_case:
    case _Flags::_Type::__hexadecimal_upper_case:
      this->__handle_integer();
      break;

    default:
      __throw_format_error(
          "The format-spec type has a type not supported for a bool argument");
    }

    return __it;
  }
};

template <class _CharT>
struct _LIBCPP_TEMPLATE_VIS __bool_strings;

template <>
struct _LIBCPP_TEMPLATE_VIS __bool_strings<char> {
  static constexpr string_view __true{"true"};
  static constexpr string_view __false{"false"};
};

#      ifndef _LIBCPP_HAS_NO_WIDE_CHARACTERS
template <>
struct _LIBCPP_TEMPLATE_VIS __bool_strings<wchar_t> {
  static constexpr wstring_view __true{L"true"};
  static constexpr wstring_view __false{L"false"};
};
#      endif

template <class _CharT>
using __formatter_bool = __formatter_integral<__parser_bool<_CharT>>;

} //namespace __format_spec

// [format.formatter.spec]/2.3
// For each charT, for each cv-unqualified arithmetic type ArithmeticT other
// than char, wchar_t, char8_t, char16_t, or char32_t, a specialization

template <class _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<bool, _CharT>
    : public __format_spec::__formatter_bool<_CharT> {
  using _Base = __format_spec::__formatter_bool<_CharT>;

  _LIBCPP_HIDE_FROM_ABI auto format(bool __value, auto& __ctx)
      -> decltype(__ctx.out()) {
    if (this->__type != __format_spec::_Flags::_Type::__string)
      return _Base::format(static_cast<unsigned char>(__value), __ctx);

    if (this->__width_needs_substitution())
      this->__substitute_width_arg_id(__ctx.arg(this->__width));

#      ifndef _LIBCPP_HAS_NO_LOCALIZATION
    if (this->__locale_specific_form) {
      const auto& __np = use_facet<numpunct<_CharT>>(__ctx.locale());
      basic_string<_CharT> __str = __value ? __np.truename() : __np.falsename();
      return __formatter::__write_unicode(
          __ctx.out(), basic_string_view<_CharT>{__str}, this->__width, -1,
          this->__fill, this->__alignment);
    }
#      endif
    basic_string_view<_CharT> __str =
        __value ? __format_spec::__bool_strings<_CharT>::__true
                : __format_spec::__bool_strings<_CharT>::__false;

    // The output only uses ASCII so every character is one column.
    unsigned __size = __str.size();
    if (__size >= this->__width)
      return _VSTD::copy(__str.begin(), __str.end(), __ctx.out());

    return __formatter::__write(__ctx.out(), __str.begin(), __str.end(), __size,
                                this->__width, this->__fill, this->__alignment);
  }
};
#    else

template <class _CharT>
struct _LIBCPP_TEMPLATE_VIS __bool_strings;

template <>
struct _LIBCPP_TEMPLATE_VIS __bool_strings<char> {
  static constexpr string_view __true{"true"};
  static constexpr string_view __false{"false"};
};

#      ifndef _LIBCPP_HAS_NO_WIDE_CHARACTERS
template <>
struct _LIBCPP_TEMPLATE_VIS __bool_strings<wchar_t> {
  static constexpr wstring_view __true{L"true"};
  static constexpr wstring_view __false{L"false"};
};
#      endif

// rename to __format_bool
template <class _CharT>
_LIBCPP_HIDE_FROM_ABI auto __write_bool_string(bool __value, auto& __ctx, format_spec::parser<_CharT> __parser) {

#      ifndef _LIBCPP_HAS_NO_LOCALIZATION
  if (__parser.__locale_specific_form) {
    const auto& __np = use_facet<numpunct<_CharT>>(__ctx.locale());
    basic_string<_CharT> __str = __value ? __np.truename() : __np.falsename();
    return __formatter::__write_unicode(__ctx.out(), basic_string_view<_CharT>{__str}, __parser.__width, -1,
                                        __parser.__fill, __parser.__alignment);
  }
#      endif
  basic_string_view<_CharT> __str = __value ? __bool_strings<_CharT>::__true : __bool_strings<_CharT>::__false;

  // The output only uses ASCII so every character is one column.
  unsigned __size = __str.size();
  if (__size >= __parser.__width)
    return _VSTD::copy(__str.begin(), __str.end(), __ctx.out());

  return __formatter::__write(__ctx.out(), __str.begin(), __str.end(), __size, __parser.__width, __parser.__fill,
                              __parser.__alignment);
}

template <class _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<bool, _CharT> {
public:
  _LIBCPP_HIDE_FROM_ABI constexpr auto parse(basic_format_parse_context<_CharT>& __parse_ctx)
      -> decltype(__parse_ctx.begin()) {
    auto __result = parser.parse(__parse_ctx, format_spec::__fields_integral);
    format_spec::__process_parsed_bool(parser);
    return __result;
  }

  _LIBCPP_HIDE_FROM_ABI auto format(bool __value, auto& __ctx) -> decltype(__ctx.out()) const {
    switch (parser.get_type()) {
    case __format_spec::_Flags::_Type::__string:
      return __write_bool_string(__value, __ctx, parser.resolve_dynamic_sizes(__ctx));

    case __format_spec::_Flags::_Type::__char:
      return __format_char(static_cast<unsigned char>(__value), __ctx.out(), parser.resolve_dynamic_sizes(__ctx));

    case __format_spec::_Flags::_Type::__binary_lower_case:
    case __format_spec::_Flags::_Type::__binary_upper_case:
    case __format_spec::_Flags::_Type::__octal:
    case __format_spec::_Flags::_Type::__decimal:
    case __format_spec::_Flags::_Type::__hexadecimal_lower_case:
    case __format_spec::_Flags::_Type::__hexadecimal_upper_case:
      // Casting this to a formatter more likely being used reduces the binary size.
      return __format_integer(static_cast<unsigned >(__value), __ctx, parser.resolve_dynamic_sizes(__ctx));

    default:
      _LIBCPP_ASSERT(false, "The parse function should have validated the type");
      _LIBCPP_UNREACHABLE();
    }
  }

  format_spec::parser<_CharT> parser;
};

#    endif
#  endif // !defined(_LIBCPP_HAS_NO_CONCEPTS)

#endif //_LIBCPP_STD_VER > 17

_LIBCPP_END_NAMESPACE_STD

#endif // _LIBCPP___FORMAT_FORMATTER_BOOL_H
