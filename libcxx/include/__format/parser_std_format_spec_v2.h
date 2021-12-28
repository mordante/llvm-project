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

#include <__config>
#include <__algorithm/min.h>
#include <__format/parser_std_format_spec.h>
#include <__format/format_parse_context.h>

// format related
#include <__format/format_error.h>
#include <__format/format_fwd.h>
#include <__format/format_string.h>
#include <__format/formatter.h>
#include <string_view>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

#if _LIBCPP_STD_VER > 17

// TODO FMT Remove this once we require compilers with proper C++20 support.
// If the compiler has no concepts support, the format header will be disabled.
// Without concepts support enable_if needs to be used and that too much effort
// to support compilers with partial C++20 support.
#  if !defined(_LIBCPP_HAS_NO_CONCEPTS)

namespace format_spec { // chrono::format_spec can be used for the chrono parser

/**
 * These fields are a filter for which elements to process.
 *
 * They default to false so when a new field is added it needs to be opted in
 * explicitly. That way the behaviour of user-defined formatters won't change.
 *
 * @note Since this stuct is part of the ABI it might be good to reserver some
 * unreserved space.
 *
 * @note Type bool or unspecified allowing bit-fields?
 *
 * @note P2286 proposes a new debug field.
 */
struct fields {
  bool sign{false};
  bool alternate_form{false};
  bool zero_padding{false};
  bool precision{false};
  bool locale_specific_form{false};
};

// By not placing this constant in the formatter class it's not duplicated for char and wchar_t
inline constexpr fields __fields_string{.precision = true};
inline constexpr fields __fields_integral{
    .sign = true, .alternate_form = true, .zero_padding = true, .locale_specific_form = true};

template <class _CharT>
class _LIBCPP_TEMPLATE_VIS parser
    : public __format_spec::__parser_width,              // provides __width(|as_arg)
      public __format_spec::__parser_precision,          // provides __precision(|as_arg)
      public __format_spec::__parser_fill_align<_CharT>, // provides __fill and uses __flags
      public __format_spec::_Flags                       // provides __flags
{
  _LIBCPP_HIDE_FROM_ABI constexpr bool __parse_fill_align(const _CharT*& __begin, const _CharT* __end) {

    __begin = __format_spec::__parser_fill_align<_CharT>::__parse(__begin, __end, static_cast<_Flags&>(*this));

    return __begin == __end;
  }

  // TODO Return status whether or not found, so end not needed
  //  if (__fields.sign && __parse_sign(__begin)) && __begin == __end()) return __begin;
  _LIBCPP_HIDE_FROM_ABI constexpr bool __parse_sign(const _CharT*& __begin /*, const _CharT* __end*/) {
    switch (*__begin) {
    case _CharT('-'):
      __sign = _Flags::_Sign::__minus;
      break;
    case _CharT('+'):
      __sign = _Flags::_Sign::__plus;
      break;
    case _CharT(' '):
      __sign = _Flags::_Sign::__space;
      break;
    default:
      return false;
    }
    ++__begin;
    return true;
  }

public:
  using char_type = _CharT;

  _LIBCPP_HIDE_FROM_ABI constexpr parser(_Flags::_Alignment __alignment = _Flags::_Alignment::__default) {
    this->__alignment = __alignment;
  }

  _LIBCPP_HIDE_FROM_ABI constexpr auto parse(basic_format_parse_context<_CharT>& __parse_ctx, fields __fields
                                             // = {/* All true or make not defaulted? */}
                                             ) -> decltype(__parse_ctx.begin()) {

    const _CharT* __begin = __parse_ctx.begin();
    const _CharT* __end = __parse_ctx.end();
    if (__begin == __end)
      return __begin;

#    if 0
    __begin = __format_spec::__parser_fill_align<_CharT>::__parse(__begin, __end, static_cast<_Flags&>(*this));
    if (__begin == __end)
      return __begin;

    if (__fields.sign) {
      __begin = __format_spec::__parse_sign(__begin, static_cast<_Flags&>(*this));
      if (__begin == __end)
        return __begin;
    }
#    else
    // Want a function like
    // XXX compare assembly before and after
    if (__parse_fill_align(__begin, __end))
      return __begin;

    if (__fields.sign && __parse_sign(__begin) && __begin == __end)
      return __begin;
#    endif

    if (__fields.alternate_form) {
      __begin = __format_spec::__parse_alternate_form(__begin, static_cast<_Flags&>(*this));
      if (__begin == __end)
        return __begin;
    }

    if (__fields.zero_padding) {
      __begin = __format_spec::__parse_zero_padding(__begin, static_cast<_Flags&>(*this));
      if (__begin == __end)
        return __begin;
    }

    __begin = __format_spec::__parser_width::__parse(__begin, __end, __parse_ctx);
    if (__begin == __end)
      return __begin;

    if (__fields.precision) {
      __begin = __format_spec::__parser_precision::__parse(__begin, __end, __parse_ctx);
      if (__begin == __end)
        return __begin;
    }

    if (__fields.locale_specific_form) {
      __begin = __format_spec::__parse_locale_specific_form(__begin, static_cast<_Flags&>(*this));
      if (__begin == __end)
        return __begin;
    }

    __begin = __format_spec::__parse_type(__begin, static_cast<_Flags&>(*this));

    if (__begin != __end && *__begin != _CharT('}'))
      __throw_format_error("The format-spec should consume the input or end with a '}'");

    return __begin;
  }
  // TODO Add test for alignment
  _LIBCPP_HIDE_FROM_ABI constexpr __format_spec::_Flags::_Alignment get_alignment() const { return this->__alignment; }
  _LIBCPP_HIDE_FROM_ABI constexpr void set_alignment(__format_spec::_Flags::_Alignment __alignment) {
    this->__alignment = __alignment;
  }

  _LIBCPP_HIDE_FROM_ABI constexpr _CharT get_fill() const { return this->__fill; }
  _LIBCPP_HIDE_FROM_ABI constexpr void set_fill(_CharT __fill) { this->__fill = __fill; }

  _LIBCPP_HIDE_FROM_ABI constexpr __format_spec::_Flags::_Sign get_sign() const { return __sign; }
  _LIBCPP_HIDE_FROM_ABI constexpr void set_sign(__format_spec::_Flags::_Sign __sign) { this->__sign = __sign; }

  _LIBCPP_HIDE_FROM_ABI constexpr bool get_alternate_form() const { return __alternate_form; }
  _LIBCPP_HIDE_FROM_ABI constexpr void set_alternate_form(bool __alternate_form) {
    this->__alternate_form = __alternate_form;
  }

  _LIBCPP_HIDE_FROM_ABI constexpr bool get_zero_padding() const { return __zero_padding; }
  _LIBCPP_HIDE_FROM_ABI constexpr void set_zero_padding(bool __zero_padding) { this->__zero_padding = __zero_padding; }

  // If has_width true -> width value
  // Else if has_dynamic_width() -> arg-id of the argument with the width.
  // Else -> Unspecified
  //
  // TODO Width an int or size_t
  // TODO After resolving width store as int internally?
  _LIBCPP_HIDE_FROM_ABI constexpr size_t get_width() const { return __width; }
  _LIBCPP_HIDE_FROM_ABI constexpr bool has_width() const { return __has_width_field(); }
  _LIBCPP_HIDE_FROM_ABI constexpr bool has_dynamic_width() const { return __width_needs_substitution(); }

  // Not possible to reassign the dynamic width
  _LIBCPP_HIDE_FROM_ABI constexpr void set_width(size_t width) {
    this->__width = width;
    this->__width_as_arg = false;
  }
  _LIBCPP_HIDE_FROM_ABI constexpr void clear_width() { set_width(0); }

  // If has_precision true -> precision value
  // Else if has_dynamic_precision() -> arg-id of the argument with the precision.
  // Else -> Unspecified
  _LIBCPP_HIDE_FROM_ABI constexpr size_t get_precision() const { return __precision; }
  _LIBCPP_HIDE_FROM_ABI constexpr bool has_precision() const { return __has_precision_field(); }
  _LIBCPP_HIDE_FROM_ABI constexpr bool has_dynamic_precision() const { return __precision_needs_substitution(); }

  // Not possible to reassign the dynamic precision
  _LIBCPP_HIDE_FROM_ABI constexpr void set_precision(size_t precision) {
    this->__precision = precision;
    __precision_as_arg = false;
  }
  _LIBCPP_HIDE_FROM_ABI constexpr void clear_precision() {
    __precision = __format::__number_max;
    __precision_as_arg = true;
  }

  _LIBCPP_HIDE_FROM_ABI constexpr bool get_locale_specific_form() const { return __locale_specific_form; }
  _LIBCPP_HIDE_FROM_ABI constexpr void set_locale_specific_form(bool __locale_specific_form) {
    this->__locale_specific_form = __locale_specific_form;
  }

  _LIBCPP_HIDE_FROM_ABI constexpr __format_spec::_Flags::_Type get_type() const { return __type; }
  _LIBCPP_HIDE_FROM_ABI constexpr void set_type(__format_spec::_Flags::_Type __type) { this->__type = __type; }

  //
#    if 1
  // returns a copy of the parser with the resolved sizes.
  // copy so can be used in a const format function.
  // the parser is cheap to copy.
  _LIBCPP_HIDE_FROM_ABI
  parser resolve_dynamic_sizes(auto& __ctx) const {
    parser __result{*this};

    if (__result.__width_needs_substitution())
      __result.__substitute_width_arg_id(__ctx.arg(__result.__width));

    if (__result.__precision_needs_substitution())
      __result.__substitute_precision_arg_id(__ctx.arg(__result.__precision));

    return __result;
  }
#    endif
};

// To formatter string??
_LIBCPP_HIDE_FROM_ABI constexpr void __validate_display_type_string(__format_spec::_Flags::_Type __type) {
  switch (__type) {
  case __format_spec::_Flags::_Type::__default:
  case __format_spec::_Flags::_Type::__string:
    break;

  default:
    __throw_format_error("The format-spec type has a type not supported for "
                         "a string argument");
  }
}

// TODO Evaluate this following approach
// struct __parser_base{
// - flags     32 bit
// - width     32 bit
// - precision 32 bit
// };
// template<class _CharT>
// struct __parser : __parser_base
// {
// - fill
// };
// then some of these functions only need the base and no duplication for char + wchar

// XXX to bool since no reuse?
template <class _CharT>
_LIBCPP_HIDE_FROM_ABI constexpr void __process_display_type_bool_string(parser<_CharT>& __parser) {
  if (__parser.get_sign() != __format_spec::_Flags::_Sign::__default)
    __throw_format_error("A sign field isn't allowed in this format-spec");

  if (__parser.get_alternate_form())
    __throw_format_error("An alternate form field isn't allowed in this format-spec");

  // get -> has?
  if (__parser.get_zero_padding())
    __throw_format_error("A zero-padding field isn't allowed in this format-spec");

  if (__parser.get_alignment() == __format_spec::_Flags::_Alignment::__default)
    __parser.set_alignment(__format_spec::_Flags::_Alignment::__left);
}

template <class _CharT>
_LIBCPP_HIDE_FROM_ABI constexpr void __process_display_type_char(parser<_CharT>& __parser) {

  // XXX should we set the default alignment to right?

  // Note identical to boolean validation.
  // XXX when single use can change it in __process_parsed_bool
  __process_display_type_bool_string(__parser);
}

template <class _CharT>
_LIBCPP_HIDE_FROM_ABI constexpr void __process_display_type_integer(parser<_CharT>& __parser) {

  // XXX maybe a helper function in parser
  __parser.__zero_padding &= __parser.__alignment == __format_spec::_Flags::_Alignment::__default;
  if (!__parser.__zero_padding && __parser.__alignment == __format_spec::_Flags::_Alignment::__default)
    __parser.__alignment = __format_spec::_Flags::_Alignment::__right;
}

template <class _CharT>
_LIBCPP_HIDE_FROM_ABI constexpr void __process_parsed_bool(parser<_CharT>& __parser) {
  switch (__parser.get_type()) {
  case __format_spec::_Flags::_Type::__default:
    __parser.set_type(__format_spec::_Flags::_Type::__string);
    [[fallthrough]];
  case __format_spec::_Flags::_Type::__string:
    __process_display_type_bool_string(__parser);
    break;

  case __format_spec::_Flags::_Type::__char:
    __process_display_type_char(__parser);
    break;

  case __format_spec::_Flags::_Type::__binary_lower_case:
  case __format_spec::_Flags::_Type::__binary_upper_case:
  case __format_spec::_Flags::_Type::__octal:
  case __format_spec::_Flags::_Type::__decimal:
  case __format_spec::_Flags::_Type::__hexadecimal_lower_case:
  case __format_spec::_Flags::_Type::__hexadecimal_upper_case:
    __process_display_type_integer(__parser);
    break;

  default:
    __throw_format_error("The format-spec type has a type not supported for a bool argument");
  }
}

template <class _CharT>
_LIBCPP_HIDE_FROM_ABI constexpr void __process_parsed_char(parser<_CharT>& __parser) {
  switch (__parser.get_type()) {
  case __format_spec::_Flags::_Type::__default:
    __parser.set_type(__format_spec::_Flags::_Type::__char);
    [[fallthrough]];
  case __format_spec::_Flags::_Type::__char:
    __process_display_type_char(__parser);
    break;

  case __format_spec::_Flags::_Type::__binary_lower_case:
  case __format_spec::_Flags::_Type::__binary_upper_case:
  case __format_spec::_Flags::_Type::__octal:
  case __format_spec::_Flags::_Type::__decimal:
  case __format_spec::_Flags::_Type::__hexadecimal_lower_case:
  case __format_spec::_Flags::_Type::__hexadecimal_upper_case:
    __process_display_type_integer(__parser);
    break;

  default:
    __throw_format_error("The format-spec type has a type not supported for a char argument");
  }
}

template <class _CharT>
_LIBCPP_HIDE_FROM_ABI constexpr void __process_parsed_integer(parser<_CharT>& __parser) {
  switch (__parser.get_type()) {
  case __format_spec::_Flags::_Type::__default:
    __parser.set_type(__format_spec::_Flags::_Type::__decimal);
    [[fallthrough]];
  case __format_spec::_Flags::_Type::__binary_lower_case:
  case __format_spec::_Flags::_Type::__binary_upper_case:
  case __format_spec::_Flags::_Type::__octal:
  case __format_spec::_Flags::_Type::__decimal:
  case __format_spec::_Flags::_Type::__hexadecimal_lower_case:
  case __format_spec::_Flags::_Type::__hexadecimal_upper_case:
    __process_display_type_integer(__parser);
    break;

  case __format_spec::_Flags::_Type::__char:
    __process_display_type_char(__parser);
    break;

  default:
    __throw_format_error("The format-spec type has a type not supported for "
                         "an integer argument");
  }
}

} // namespace format_spec

template <class _CharT>
_LIBCPP_HIDE_FROM_ABI auto __format_char(integral auto __value, output_iterator<const _CharT&> auto __out_it,
                                         format_spec::parser<_CharT> __parser) -> decltype(__out_it) {
  using _Tp = decltype(__value);
  if constexpr (!same_as<_CharT, _Tp>) {
    // cmp_less and cmp_greater can't be used for character types.
    if constexpr (signed_integral<_CharT> == signed_integral<_Tp>) {
      if (__value < numeric_limits<_CharT>::min() || __value > numeric_limits<_CharT>::max())
        __throw_format_error("Integral value outside the range of the char type");
    } else if constexpr (signed_integral<_CharT>) {
      // _CharT is signed _Tp is unsigned
      if (__value > static_cast<make_unsigned_t<_CharT>>(numeric_limits<_CharT>::max()))
        __throw_format_error("Integral value outside the range of the char type");
    } else {
      // _CharT is unsigned _Tp is signed
      if (__value < 0 || static_cast<make_unsigned_t<_Tp>>(__value) > numeric_limits<_CharT>::max())
        __throw_format_error("Integral value outside the range of the char type");
    }
  }

  const auto __c = static_cast<_CharT>(__value);
  constexpr unsigned __size = 1;
  // this epilogue is the same as boolean...
  if (__size >= __parser.__width)
    return _VSTD::copy(_VSTD::addressof(__c), _VSTD::addressof(__c) + __size, _VSTD::move(__out_it));

  return __formatter::__write(_VSTD::move(__out_it), _VSTD::addressof(__c), _VSTD::addressof(__c) + __size, __size,
                              __parser.__width, __parser.__fill, __parser.__alignment);
}

template <class _OutIt, class _CharT>
_LIBCPP_HIDE_FROM_ABI _OutIt __format_grouping(_OutIt __out_it, const char* __begin, const char* __first,
                                               const char* __last, string&& __grouping, _CharT __sep,
                                               format_spec::parser<_CharT> __parser) {

  // TODO FMT This function duplicates some functionality of the normal
  // output routines. Evaluate whether these parts can be efficiently
  // combined with the existing routines.

  unsigned __size = (__first - __begin) +    // [sign][prefix]
                    (__last - __first) +     // data
                    (__grouping.size() - 1); // number of separator characters

  __formatter::__padding_size_result __padding = {0, 0};
  if (__parser.get_alignment() == __format_spec::_Flags::_Alignment::__default) {
    // Write [sign][prefix].
    __out_it = _VSTD::copy(__begin, __first, _VSTD::move(__out_it));

    if (__parser.get_width() > __size) {
      // Write zero padding.
      __padding.__before = __parser.get_width() - __size;
      __out_it = _VSTD::fill_n(_VSTD::move(__out_it), __parser.get_width() - __size, _CharT('0'));
    }
  } else {
    if (__parser.get_width() > __size) {
      // Determine padding and write padding.
      __padding = __formatter::__padding_size(__size, __parser.get_width(), __parser.get_alignment());

      __out_it = _VSTD::fill_n(_VSTD::move(__out_it), __padding.__before, __parser.get_fill());
    }
    // Write [sign][prefix].
    __out_it = _VSTD::copy(__begin, __first, _VSTD::move(__out_it));
  }

  auto __r = __grouping.rbegin();
  auto __e = __grouping.rend() - 1;
  _LIBCPP_ASSERT(__r != __e, "The slow grouping formatting is used while "
                             "there will be no separators written.");
  // The output is divided in small groups of numbers to write:
  // - A group before the first separator.
  // - A separator and a group, repeated for the number of separators.
  // - A group after the last separator.
  // This loop achieves that process by testing the termination condition
  // midway in the loop.
  //
  // TODO FMT This loop evaluates the loop invariant `__parser.__type !=
  // _Flags::_Type::__hexadecimal_upper_case` for every iteration. (This test
  // happens in the __write call.) Benchmark whether making two loops and
  // hoisting the invariant is worth the effort.
  while (true) {
    if (__parser.get_type() == __format_spec::_Flags::_Type::__hexadecimal_upper_case) {
      __last = __first + *__r;
      __out_it = _VSTD::transform(__first, __last, _VSTD::move(__out_it), __format_spec::__hex_to_upper);
      __first = __last;
    } else {
      __out_it = _VSTD::copy_n(__first, *__r, _VSTD::move(__out_it));
      __first += *__r;
    }

    if (__r == __e)
      break;

    ++__r;
    *__out_it++ = __sep;
  }

  return _VSTD::fill_n(_VSTD::move(__out_it), __padding.__after, __parser.get_fill());
}

// Internal in __detail ns?
template <unsigned_integral _Tp, class _CharT>
_LIBCPP_HIDE_FROM_ABI auto __format_integer(char* __begin, char* __end, _Tp __value, bool __negative, int __base,
                                            auto& __ctx, const char* __prefix, format_spec::parser<_CharT> __parser)
    -> decltype(__ctx.out()) {
  char* __first = __insert_sign(__begin, __negative, __parser.get_sign());
  if (__parser.get_alternate_form() && __prefix)
    while (*__prefix)
      *__first++ = *__prefix++;

  using __format_spec::__to_buffer;
  char* __last = __to_buffer(__first, __end, __value, __base);

#    ifndef _LIBCPP_HAS_NO_LOCALIZATION
  if (__parser.get_locale_specific_form()) {
    const auto& __np = use_facet<numpunct<_CharT>>(__ctx.locale());
    string __grouping = __np.grouping();
    ptrdiff_t __size = __last - __first;
    // Writing the grouped form has more overhead than the normal output
    // routines. If there will be no separators written the locale-specific
    // form is identical to the normal routine. Test whether to grouped form
    // is required.
    if (!__grouping.empty() && __size > __grouping[0])
      return __format_grouping(__ctx.out(), __begin, __first, __last,
                               __format_spec::__determine_grouping(__size, __grouping), __np.thousands_sep(), __parser);
  }
#    endif
  auto __out_it = __ctx.out();
  if (__parser.get_alignment() != __format_spec::_Flags::_Alignment::__default)
    __first = __begin;
  else {
    // __buf contains [sign][prefix]data
    //                              ^ location of __first
    // The zero padding is done like:
    // - Write [sign][prefix]
    // - Write data right aligned with '0' as fill character.
    __out_it = _VSTD::copy(__begin, __first, _VSTD::move(__out_it));
    __parser.set_alignment(__format_spec::_Flags::_Alignment::__right);
    __parser.set_fill(_CharT('0'));
    uint32_t __size = __first - __begin;

    __parser.set_width(__parser.get_width() - _VSTD::min<uint32_t /* XXX */ >(__size, __parser.get_width()));
    //    this->__width -= _VSTD::min(__size, this->__width);
  }

#    if 0
  // this epilogue is the same as boolean and char...

  if (__size >= __parser.__width)
    return _VSTD::copy(_VSTD::addressof(__c), _VSTD::addressof(__c) + __size, _VSTD::move(__out_it));

  return __formatter::__write(_VSTD::move(__out_it), _VSTD::addressof(__c), _VSTD::addressof(__c) + __size, __size,
                              __parser.__width, __parser.__fill, __parser.__alignment);
#    else

  unsigned __size = __last - __first;
  if (__parser.get_type() != __format_spec::_Flags::_Type::__hexadecimal_upper_case) [[likely]] {
    if (__size >= __parser.__width)
      return _VSTD::copy(__first, __last, __ctx.out());

    return __formatter::__write(__ctx.out(), __first, __last, __size, __parser.__width, __parser.__fill,
                                __parser.__alignment);
  }

  // this->__type == _Flags::_Type::__hexadecimal_upper_case
  // This means all characters in the range [a-f] need to be changed to their
  // uppercase representation. The transformation is done as transformation
  // in the output routine instead of before. This avoids another pass over
  // the data.
  // TODO FMT See whether it's possible to do this transformation during the
  // conversion. (This probably requires changing std::to_chars' alphabet.)
  if (__size >= __parser.__width)
    return _VSTD::transform(__first, __last, __ctx.out(), __format_spec::__hex_to_upper);

  return __formatter::__write(__ctx.out(), __first, __last, __size, __format_spec::__hex_to_upper, __parser.__width,
                              __parser.__fill, __parser.__alignment);

#    endif
}

// XXX should we first promote integrals reducing code size?
template <unsigned_integral _Tp, class _CharT>
_LIBCPP_HIDE_FROM_ABI auto __format_integer(_Tp __value, auto& __ctx, format_spec::parser<_CharT> __parser,
                                            bool __negative = false) -> decltype(__ctx.out()) {

  using __format_spec::__buffer_size;
  using __format_spec::__to_buffer;

  // TODO Instantiated twice for char and wchar_t ?
  // XXX reorder args to match this function?
  switch (__parser.get_type()) {
  case __format_spec::_Flags::_Type::__binary_lower_case: {
    array<char, __buffer_size<decltype(__value), 2>()> __array;
    return __format_integer(__array.begin(), __array.end(), __value, __negative, 2, __ctx, "0b", __parser);
  }
  case __format_spec::_Flags::_Type::__binary_upper_case: {
    array<char, __buffer_size<decltype(__value), 2>()> __array;
    return __format_integer(__array.begin(), __array.end(), __value, __negative, 2, __ctx, "0B", __parser);
  }
  case __format_spec::_Flags::_Type::__octal: {
    // Octal is special; if __value == 0 there's no prefix.
    array<char, __buffer_size<decltype(__value), 8>()> __array;
    return __format_integer(__array.begin(), __array.end(), __value, __negative, 8, __ctx, __value != 0 ? "0" : nullptr,
                            __parser);
  }
  case __format_spec::_Flags::_Type::__decimal: {
    array<char, __buffer_size<decltype(__value), 10>()> __array;
    return __format_integer(__array.begin(), __array.end(), __value, __negative, 10, __ctx, nullptr, __parser);
  }
  case __format_spec::_Flags::_Type::__hexadecimal_lower_case: {
    array<char, __buffer_size<decltype(__value), 16>()> __array;
    return __format_integer(__array.begin(), __array.end(), __value, __negative, 16, __ctx, "0x", __parser);
  }
  case __format_spec::_Flags::_Type::__hexadecimal_upper_case: {
    array<char, __buffer_size<decltype(__value), 16>()> __array;
    return __format_integer(__array.begin(), __array.end(), __value, __negative, 16, __ctx, "0X", __parser);
  }
  default:
    _LIBCPP_ASSERT(false, "The parse function should have validated the type");
    _LIBCPP_UNREACHABLE();
  }
}

template <signed_integral _Tp, class _CharT>
_LIBCPP_HIDE_FROM_ABI auto __format_integer(_Tp __value, auto& __ctx, format_spec::parser<_CharT> __parser)
    -> decltype(__ctx.out()) {
  // Depending on the std-format-spec string the sign and the value
  // might not be outputted together:
  // - alternate form may insert a prefix string.
  // - zero-padding may insert additional '0' characters.
  // Therefore the value is processed as a positive unsigned value.
  // The function @ref __insert_sign will a '-' when the value was negative.
  auto __r = __to_unsigned_like(__value);
  bool __negative = __value < 0;
  if (__negative)
    __r = __complement(__r);

  return __format_integer(__r, __ctx, __parser, __negative);
}

#  endif // !defined(_LIBCPP_HAS_NO_CONCEPTS)

#endif //_LIBCPP_STD_VER > 17

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif // _LIBCPP___FORMAT_PARSER_STD_FORMAT_SPEC_V2_H
