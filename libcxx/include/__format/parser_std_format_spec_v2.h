// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___FORMAT_PARSER_STD_FORMAT_SPEC_V2_H
#define _LIBCPP___FORMAT_PARSER_STD_FORMAT_SPEC_V2_H

#include <__algorithm/min.h>
#include <__config>
#include <__format/format_error.h>
#include <__format/format_fwd.h>
#include <__format/format_parse_context.h>
#include <__format/format_string.h>
#include <__format/parser_std_format_spec.h>
#include <string_view>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

#if _LIBCPP_STD_VER > 17

/// \file Contains the std-format-spec parser.
///
/// Most of the code can be reused in the chrono-format-spec.
/// This header has some support for the chrono-format-spec since it doesn't
/// affect the std-format-spec.

namespace __format_spec {

/// These fields are a filter for which elements to parse.
///
/// They default to false so when a new field is added it needs to be opted in
/// explicitly. That way the behaviour of user-defined formatters won't change.
///
/// @note Since this stuct is part of the ABI it might be good to reserver some
/// unreserved space.
///
/// @note Type bool or unspecified allowing bit-fields?
struct __fields {
  uint8_t __sign_ : 1 {false};
  uint8_t __alternate_form_ : 1 {false};
  uint8_t __zero_padding_ : 1 {false};
  uint8_t __precision_ : 1 {false};
  uint8_t __locale_specific_form_ : 1 {false};
  // This field is special it's always true for the std-format-spec.
  // Since the chrono-format-spec doesn't use this field it can be disabled.
  uint8_t __type_ : 1 {true};
};

// By not placing this constant in the formatter class it's not duplicated for char and wchar_t
inline constexpr __fields __fields_integral{
    .__sign_ = true, .__alternate_form_ = true, .__zero_padding_ = true, .__locale_specific_form_ = true};
inline constexpr __fields __fields_floating_point{
    .__sign_ = true,
    .__alternate_form_ = true,
    .__zero_padding_ = true,
    .__precision_ = true,
    .__locale_specific_form_ = true,
};
inline constexpr __fields __fields_string{
    .__precision_ = true,
};
inline constexpr __fields __fields_pointer{};

enum class _LIBCPP_ENUM_VIS __alignment : uint8_t {
  /// No alignment is set in the format string.
  ///
  /// Zero-padding is ignored when an alignment is selected.
  /// The default alignment depends on the selected format-type.
  __default,
  __left,
  __center,
  __right
};
enum class _LIBCPP_ENUM_VIS __sign : uint8_t {
  /// No sign is set in the format string.
  ///
  /// The sign isn't allowed for certain format-types. By using this value
  /// it's possible to detect whether or not the user explicitly set the sign
  /// flag. For formatting purposes it behaves the same as @ref __minus.
  __default,
  __minus,
  __plus,
  __space
};

enum class _LIBCPP_ENUM_VIS __type : uint8_t {
  __default,
  __string,
  __binary_lower_case,
  __binary_upper_case,
  __octal,
  __decimal,
  __hexadecimal_lower_case,
  __hexadecimal_upper_case,
  __pointer,
  __char,
  __float_hexadecimal_lower_case,
  __float_hexadecimal_upper_case,
  __scientific_lower_case,
  __scientific_upper_case,
  __fixed_lower_case,
  __fixed_upper_case,
  __general_lower_case,
  __general_upper_case
};

template <class _CharT>
class _LIBCPP_TEMPLATE_VIS __parser {
public:
  _LIBCPP_HIDE_FROM_ABI constexpr auto __parse(basic_format_parse_context<_CharT>& __parse_ctx, __fields __fields)
      -> decltype(__parse_ctx.begin()) {

    const _CharT* __begin = __parse_ctx.begin();
    const _CharT* __end = __parse_ctx.end();
    if (__begin == __end)
      return __begin;

    if (__parse_fill_align(__begin, __end) && __begin == __end)
      return __begin;

    if (__fields.__sign_ && __parse_sign(__begin) && __begin == __end)
      return __begin;

    if (__fields.__alternate_form_ && __parse_alternate_form(__begin) && __begin == __end)
      return __begin;

    if (__fields.__zero_padding_ && __parse_zero_padding(__begin) && __begin == __end)
      return __begin;

    if (__parse_width(__begin, __end, __parse_ctx) && __begin == __end)
      return __begin;

    if (__fields.__precision_ && __parse_precision(__begin, __end, __parse_ctx) && __begin == __end)
      return __begin;

    if (__fields.__locale_specific_form_ && __parse_locale_specific_form(__begin) && __begin == __end)
      return __begin;

    if (__fields.__type_) {
      __parse_type(__begin);

      if (__begin != __end && *__begin != _CharT('}'))
        __throw_format_error("The format-spec should consume the input or end with a '}'");
    }

    return __begin;
  }

  _LIBCPP_HIDE_FROM_ABI constexpr bool __has_width() const {
    _LIBCPP_ASSERT(!__width_as_arg_, "the function should only be used to test __width_");
    return __width_ > 0;
  }

  _LIBCPP_HIDE_FROM_ABI constexpr bool __has_precision() const {
    _LIBCPP_ASSERT(!__precision_as_arg_, "the function should only be used to test __precision_");
    return __precision_ >= 0;
  }

  /// \returns a copy of the parser with the resolved dynamic sizes.
  ///
  /// Returning a copy makes it possible to create a const qualified format
  /// member function. The parser is cheap to copy.
  _LIBCPP_HIDE_FROM_ABI
  __parser __resolve_dynamic_sizes(auto& __ctx) const {
    __parser __result{*this};

    if (__result.__width_as_arg_)
      __result.__substitute_width_arg_id(__ctx.arg(__result.__width_));

    if (__result.__precision_as_arg_)
      __result.__substitute_precision_arg_id(__ctx.arg(__result.__precision_));

    return __result;
  }

  __alignment __alignment_ : 2 {__alignment::__default};
  __sign __sign_ : 2 {__sign::__default};
  uint8_t __alternate_form_ : 1 {false};
  uint8_t __zero_padding_ : 1 {false};
  uint8_t __locale_specific_form_ : 1 {false};
  __type __type_ : 5 {__type::__default};

  uint8_t __width_as_arg_ : 1 {false};
  uint8_t __precision_as_arg_ : 1 {false};

  // These two flags are used for formatting chrono. Since the struct has
  // padding space left it's added to this structure.
  uint8_t __weekday_name_ : 1 {false};
  uint8_t __month_name_ : 1 {false};
  // LWG 3576 will probably change this to always accept a Unicode code point
  // To avoid changing the size with that change align the field so when it
  // becomes 32-bit its alignment will remain the same. That also means the
  // size will remain the same. (D2572 addresses the solution for LWG 3576.)
  alignas(4) _CharT __fill_{_CharT(' ')};
  int32_t __width_{0};
  int32_t __precision_{-1};

private:
  _LIBCPP_HIDE_FROM_ABI constexpr bool __parse_alignment(_CharT __c) {
    switch (__c) {
    case _CharT('<'):
      __alignment_ = __alignment::__left;
      return true;

    case _CharT('^'):
      __alignment_ = __alignment::__center;
      return true;

    case _CharT('>'):
      __alignment_ = __alignment::__right;
      return true;
    }
    return false;
  }

  _LIBCPP_HIDE_FROM_ABI constexpr bool __parse_fill_align(const _CharT*& __begin, const _CharT* __end) {
    _LIBCPP_ASSERT(__begin != __end, "when called with an empty input the function will cause "
                                     "undefined behavior by evaluating data not in the input");
    if (__begin + 1 != __end) {
      if (__parse_alignment(*(__begin + 1))) {
        if (*__begin == _CharT('{') || *__begin == _CharT('}'))
          __throw_format_error("The format-spec fill field contains an invalid character");

        __fill_ = *__begin;
        __begin += 2;
        return true;
      }
    }

    if (!__parse_alignment(*__begin))
      return false;

    ++__begin;
    return true;
  }

  _LIBCPP_HIDE_FROM_ABI constexpr bool __parse_sign(const _CharT*& __begin) {
    switch (*__begin) {
    case _CharT('-'):
      __sign_ = __sign::__minus;
      break;
    case _CharT('+'):
      __sign_ = __sign::__plus;
      break;
    case _CharT(' '):
      __sign_ = __sign::__space;
      break;
    default:
      return false;
    }
    ++__begin;
    return true;
  }

  _LIBCPP_HIDE_FROM_ABI constexpr bool __parse_alternate_form(const _CharT*& __begin) {
    if (*__begin != _CharT('#'))
      return false;

    __alternate_form_ = true;
    ++__begin;
    return true;
  }

  _LIBCPP_HIDE_FROM_ABI constexpr bool __parse_zero_padding(const _CharT*& __begin) {
    if (*__begin != _CharT('0'))
      return false;

    __zero_padding_ = true;
    ++__begin;
    return true;
  }

  _LIBCPP_HIDE_FROM_ABI constexpr bool __parse_width(const _CharT*& __begin, const _CharT* __end, auto& __parse_ctx) {
    if (*__begin == _CharT('0'))
      __throw_format_error("A format-spec width field shouldn't have a leading zero");

    if (*__begin == _CharT('{')) {
      __format::__parse_number_result __r = __format_spec::__parse_arg_id(++__begin, __end, __parse_ctx);
      __width_as_arg_ = true;
      __width_ = __r.__value;
      __begin = __r.__ptr;
      return true;
    }

    if (*__begin < _CharT('0') || *__begin > _CharT('9'))
      return false;

    __format::__parse_number_result __r = __format::__parse_number(__begin, __end);
    __width_ = __r.__value;
    _LIBCPP_ASSERT(__width_ != 0, "A zero value isn't allowed and should be impossible, "
                                  "due to validations in this function");
    __begin = __r.__ptr;
    return true;
  }

  _LIBCPP_HIDE_FROM_ABI constexpr bool __parse_precision(const _CharT*& __begin, const _CharT* __end,
                                                         auto& __parse_ctx) {
    if (*__begin != _CharT('.'))
      return false;

    ++__begin;
    if (__begin == __end)
      __throw_format_error("End of input while parsing format-spec precision");

    if (*__begin == _CharT('{')) {
      __format::__parse_number_result __arg_id = __format_spec::__parse_arg_id(++__begin, __end, __parse_ctx);
      __precision_as_arg_ = true;
      __precision_ = __arg_id.__value;
      __begin = __arg_id.__ptr;
      return true;
    }

    if (*__begin < _CharT('0') || *__begin > _CharT('9'))
      __throw_format_error("The format-spec precision field doesn't contain a value or arg-id");

    __format::__parse_number_result __r = __format::__parse_number(__begin, __end);
    __precision_ = __r.__value;
    __precision_as_arg_ = false;
    __begin = __r.__ptr;
    return true;
  }

  _LIBCPP_HIDE_FROM_ABI constexpr bool __parse_locale_specific_form(const _CharT*& __begin) {
    if (*__begin != _CharT('L'))
      return false;

    __locale_specific_form_ = true;
    ++__begin;
    return true;
  }

  _LIBCPP_HIDE_FROM_ABI constexpr void __parse_type(const _CharT*& __begin) {
    // Determines the type. It does not validate whether the selected type is
    // valid. Most formatters have optional fields that are only allowed for
    // certain types. These parsers need to do validation after the type has
    // been parsed. So its easier to implement the validation for all types in
    // the specific parse function.
    switch (*__begin) {
    case 'A':
      __type_ = __type::__float_hexadecimal_upper_case;
      break;
    case 'B':
      __type_ = __type::__binary_upper_case;
      break;
    case 'E':
      __type_ = __type::__scientific_upper_case;
      break;
    case 'F':
      __type_ = __type::__fixed_upper_case;
      break;
    case 'G':
      __type_ = __type::__general_upper_case;
      break;
    case 'X':
      __type_ = __type::__hexadecimal_upper_case;
      break;
    case 'a':
      __type_ = __type::__float_hexadecimal_lower_case;
      break;
    case 'b':
      __type_ = __type::__binary_lower_case;
      break;
    case 'c':
      __type_ = __type::__char;
      break;
    case 'd':
      __type_ = __type::__decimal;
      break;
    case 'e':
      __type_ = __type::__scientific_lower_case;
      break;
    case 'f':
      __type_ = __type::__fixed_lower_case;
      break;
    case 'g':
      __type_ = __type::__general_lower_case;
      break;
    case 'o':
      __type_ = __type::__octal;
      break;
    case 'p':
      __type_ = __type::__pointer;
      break;
    case 's':
      __type_ = __type::__string;
      break;
    case 'x':
      __type_ = __type::__hexadecimal_lower_case;
      break;
    default:
      return;
    }
    ++__begin;
  }

  _LIBCPP_HIDE_FROM_ABI constexpr void __substitute_width_arg_id(auto __arg) {
    _LIBCPP_ASSERT(__width_as_arg_ == true, "Substitute width called when no substitution is required");

    __width_as_arg_ = 0;
    __width_ = __format_spec::__substitute_arg_id(__arg);
    if (__width_ == 0)
      __throw_format_error("A format-spec width field replacement should have a positive value");
  }

  _LIBCPP_HIDE_FROM_ABI constexpr void __substitute_precision_arg_id(auto __arg) {
    _LIBCPP_ASSERT(__precision_as_arg_ == true, "Substitute width called when no substitution is required");

    __precision_as_arg_ = false;
    __precision_ = __format_spec::__substitute_arg_id(__arg);
  }
};

_LIBCPP_HIDE_FROM_ABI constexpr void __process_display_type_string(__format_spec::__type __type) {
  switch (__type) {
  case __format_spec::__type::__default:
  case __format_spec::__type::__string:
    break;

  default:
    __throw_format_error("The format-spec type has a type not supported for "
                         "a string argument");
  }
}

template <class _CharT>
_LIBCPP_HIDE_FROM_ABI constexpr void __process_display_type_bool_string(__parser<_CharT>& __parser) {
  if (__parser.__sign_ != __sign::__default)
    __throw_format_error("A sign field isn't allowed in this format-spec");

  if (__parser.__alternate_form_)
    __throw_format_error("An alternate form field isn't allowed in this format-spec");

  if (__parser.__zero_padding_)
    __throw_format_error("A zero-padding field isn't allowed in this format-spec");

  if (__parser.__alignment_ == __alignment::__default)
    __parser.__alignment_ = __alignment::__left;
}

template <class _CharT>
_LIBCPP_HIDE_FROM_ABI constexpr void __process_display_type_char(__parser<_CharT>& __parser) {
  __process_display_type_bool_string(__parser);
}

template <class _CharT>
_LIBCPP_HIDE_FROM_ABI constexpr void __resolve_zero_padding(__parser<_CharT>& __parser) {
  __parser.__zero_padding_ &= __parser.__alignment_ == __alignment::__default;
  if (!__parser.__zero_padding_ && __parser.__alignment_ == __alignment::__default)
    __parser.__alignment_ = __alignment::__right;
}

template <class _CharT>
_LIBCPP_HIDE_FROM_ABI constexpr void __process_display_type_integer(__parser<_CharT>& __parser) {
  __resolve_zero_padding(__parser);
}

template <class _CharT>
_LIBCPP_HIDE_FROM_ABI constexpr void __process_parsed_bool(__parser<_CharT>& __parser) {
  switch (__parser.__type_) {
  case __format_spec::__type::__default:
    __parser.__type_ = __format_spec::__type::__string;
    [[fallthrough]];
  case __format_spec::__type::__string:
    __process_display_type_bool_string(__parser);
    break;

  case __format_spec::__type::__binary_lower_case:
  case __format_spec::__type::__binary_upper_case:
  case __format_spec::__type::__octal:
  case __format_spec::__type::__decimal:
  case __format_spec::__type::__hexadecimal_lower_case:
  case __format_spec::__type::__hexadecimal_upper_case:
    __process_display_type_integer(__parser);
    break;

  default:
    __throw_format_error("The format-spec type has a type not supported for a bool argument");
  }
}

template <class _CharT>
_LIBCPP_HIDE_FROM_ABI constexpr void __process_parsed_char(__parser<_CharT>& __parser) {
  switch (__parser.__type_) {
  case __format_spec::__type::__default:
    __parser.__type_ = __format_spec::__type::__char;
    [[fallthrough]];
  case __format_spec::__type::__char:
    __process_display_type_char(__parser);
    break;

  case __format_spec::__type::__binary_lower_case:
  case __format_spec::__type::__binary_upper_case:
  case __format_spec::__type::__octal:
  case __format_spec::__type::__decimal:
  case __format_spec::__type::__hexadecimal_lower_case:
  case __format_spec::__type::__hexadecimal_upper_case:
    __process_display_type_integer(__parser);
    break;

  default:
    __throw_format_error("The format-spec type has a type not supported for a char argument");
  }
}

template <class _CharT>
_LIBCPP_HIDE_FROM_ABI constexpr void __process_parsed_integer(__parser<_CharT>& __parser) {
  switch (__parser.__type_) {
  case __format_spec::__type::__default:
    __parser.__type_ = __format_spec::__type::__decimal;
    [[fallthrough]];
  case __format_spec::__type::__binary_lower_case:
  case __format_spec::__type::__binary_upper_case:
  case __format_spec::__type::__octal:
  case __format_spec::__type::__decimal:
  case __format_spec::__type::__hexadecimal_lower_case:
  case __format_spec::__type::__hexadecimal_upper_case:
    __process_display_type_integer(__parser);
    break;

  case __format_spec::__type::__char:
    __process_display_type_char(__parser);
    break;

  default:
    __throw_format_error("The format-spec type has a type not supported for "
                         "an integer argument");
  }
}

template <class _CharT>
_LIBCPP_HIDE_FROM_ABI constexpr void __process_parsed_floating_point(__parser<_CharT>& __parser) {
  __resolve_zero_padding(__parser);

  switch (__parser.__type_) {
  case __format_spec::__type::__default:
    // When no precision specified then it keeps default since that
    // formatting differs from the other types.
    if (__parser.__precision_as_arg_ || __parser.__has_precision())
      __parser.__type_ = __format_spec::__type::__general_lower_case;
    break;
  case __format_spec::__type::__float_hexadecimal_lower_case:
  case __format_spec::__type::__float_hexadecimal_upper_case:
    // Precision specific behavior will be handled later.
    break;
  case __format_spec::__type::__scientific_lower_case:
  case __format_spec::__type::__scientific_upper_case:
  case __format_spec::__type::__fixed_lower_case:
  case __format_spec::__type::__fixed_upper_case:
  case __format_spec::__type::__general_lower_case:
  case __format_spec::__type::__general_upper_case:
    if (!__parser.__precision_as_arg_ && !__parser.__has_precision())
      // Set the default precision for the call to to_chars.
      __parser.__precision_ = 6;
    break;

  default:
    __throw_format_error("The format-spec type has a type not supported for "
                         "a floating-point argument");
  }
}

_LIBCPP_HIDE_FROM_ABI constexpr void __process_display_type_pointer(__format_spec::__type __type) {
  switch (__type) {
  case __format_spec::__type::__default:
  case __format_spec::__type::__pointer:
    break;

  default:
    __throw_format_error("The format-spec type has a type not supported for "
                         "a pointer argument");
  }
}

} // namespace __format_spec

#endif //_LIBCPP_STD_VER > 17

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif // _LIBCPP___FORMAT_PARSER_STD_FORMAT_SPEC_V2_H
