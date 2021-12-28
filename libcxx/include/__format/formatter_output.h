// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___FORMAT_FORMATTER_OUTPUT_H
#define _LIBCPP___FORMAT_FORMATTER_OUTPUT_H

#include <__algorithm/copy.h>
#include <__algorithm/copy_n.h>
#include <__algorithm/fill_n.h>
#include <__algorithm/transform.h>
#include <__concepts/same_as.h>
#include <__config>
#include <__format/format_string.h>
#include <__format/parser_std_format_spec.h>
#include <__format/parser_std_format_spec_v2.h>
#include <__utility/unreachable.h>
#include <cstddef>
#include <string>
#include <string_view>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

#if _LIBCPP_STD_VER > 17

namespace __formatter {

_LIBCPP_HIDE_FROM_ABI constexpr char __hex_to_upper(char c) {
  switch (c) {
  case 'a':
    return 'A';
  case 'b':
    return 'B';
  case 'c':
    return 'C';
  case 'd':
    return 'D';
  case 'e':
    return 'E';
  case 'f':
    return 'F';
  }
  return c;
}

struct _LIBCPP_TYPE_VIS __padding_size_result {
  size_t __before_;
  size_t __after_;
};

_LIBCPP_HIDE_FROM_ABI constexpr __padding_size_result __padding_size(size_t __size, size_t __width,
                                                                     __format_spec::__alignment __align) {
  _LIBCPP_ASSERT(__width > __size, "Don't call this function when no padding is required");
  _LIBCPP_ASSERT(__align != __format_spec::__alignment::__default,
                 "Caller should adjust the default to the value required by the type");

  size_t __fill = __width - __size;
  switch (__align) {
  case __format_spec::__alignment::__default:
    __libcpp_unreachable();

  case __format_spec::__alignment::__left:
    return {0, __fill};

  case __format_spec::__alignment::__center: {
    // The extra padding is divided per [format.string.std]/3
    // __before = floor(__fill, 2);
    // __after = ceil(__fill, 2);
    size_t __before = __fill / 2;
    size_t __after = __fill - __before;
    return {__before, __after};
  }
  case __format_spec::__alignment::__right:
    return {__fill, 0};
  }
  __libcpp_unreachable();
}

// XXX write groupping
template <class _OutIt, class _CharT>
_LIBCPP_HIDE_FROM_ABI _OutIt __format_grouping(_OutIt __out_it, const char* __begin, const char* __first,
                                               const char* __last, string&& __grouping, _CharT __sep,
                                               __format_spec::__parser<_CharT> __parser) {

  // TODO FMT This function duplicates some functionality of the normal
  // output routines. Evaluate whether these parts can be efficiently
  // combined with the existing routines.

  int __size = (__first - __begin) +    // [sign][prefix]
               (__last - __first) +     // data
               (__grouping.size() - 1); // number of separator characters

  __padding_size_result __padding = {0, 0};
  if (__parser.__alignment_ == __format_spec::__alignment::__default) {
    // Write [sign][prefix].
    __out_it = _VSTD::copy(__begin, __first, _VSTD::move(__out_it));

    if (__parser.__width_ > __size) {
      // Write zero padding.
      __padding.__before_ = __parser.__width_ - __size;
      __out_it = _VSTD::fill_n(_VSTD::move(__out_it), __parser.__width_ - __size, _CharT('0'));
    }
  } else {
    if (__parser.__width_ > __size) {
      // Determine padding and write padding.
      __padding = __padding_size(__size, __parser.__width_, __parser.__alignment_);

      __out_it = _VSTD::fill_n(_VSTD::move(__out_it), __padding.__before_, __parser.__fill_);
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
    if (__parser.__type_ == __format_spec::__type::__hexadecimal_upper_case) {
      __last = __first + *__r;
      __out_it = _VSTD::transform(__first, __last, _VSTD::move(__out_it), __hex_to_upper);
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

  return _VSTD::fill_n(_VSTD::move(__out_it), __padding.__after_, __parser.__fill_);
}

/**
 * Writes the input to the output with the required padding.
 *
 * Since the output column width is specified the function can be used for
 * ASCII and Unicode input.
 *
 * @pre [@a __first, @a __last) is a valid range.
 * @pre @a __size <= @a __width. Using this function when this pre-condition
 *      doesn't hold incurs an unwanted overhead.
 *
 * @param __out_it    The output iterator to write to.
 * @param __first     Pointer to the first element to write.
 * @param __last      Pointer beyond the last element to write.
 * @param __size      The (estimated) output column width. When the elements
 *                    to be written are ASCII the following condition holds
 *                    @a __size == @a __last - @a __first.
 * @param __width     The number of output columns to write.
 * @param __fill      The character used for the alignment of the output.
 *                    TODO FMT Will probably change to support Unicode grapheme
 *                    cluster.
 * @param __alignment The requested alignment.
 *
 * @returns           An iterator pointing beyond the last element written.
 *
 * @note The type of the elements in range [@a __first, @a __last) can differ
 * from the type of @a __fill. Integer output uses @c std::to_chars for its
 * conversion, which means the [@a __first, @a __last) always contains elements
 * of the type @c char.
 */
template <class _CharT, class _ParserCharT>
_LIBCPP_HIDE_FROM_ABI auto __write(const _CharT* __first, const _CharT* __last,
                                   output_iterator<const _CharT&> auto __out_it,
                                   __format_spec::__parser<_ParserCharT> __parser, ptrdiff_t __size) {
  _LIBCPP_ASSERT(__first <= __last, "Not a valid range");

  if (__size >= __parser.__width_)
    return _VSTD::copy(__first, __last, _VSTD::move(__out_it));

  __padding_size_result __padding = __formatter::__padding_size(__size, __parser.__width_, __parser.__alignment_);
  __out_it = _VSTD::fill_n(_VSTD::move(__out_it), __padding.__before_, __parser.__fill_);
  __out_it = _VSTD::copy(__first, __last, _VSTD::move(__out_it));
  return _VSTD::fill_n(_VSTD::move(__out_it), __padding.__after_, __parser.__fill_);
}

/** XXX update comment
 * Writes the input to the output with the required padding.
 *
 * Since the output column width is specified the function can be used for
 * ASCII and Unicode input.
 *
 * @pre [@a __first, @a __last) is a valid range.
 * @pre @a __size <= @a __width. Using this function when this pre-condition
 *      doesn't hold incurs an unwanted overhead.
 *
 * @param __out_it    The output iterator to write to.
 * @param __first     Pointer to the first element to write.
 * @param __last      Pointer beyond the last element to write.
 * @param __size      The (estimated) output column width. When the elements
 *                    to be written are ASCII the following condition holds
 *                    @a __size == @a __last - @a __first.
 * @param __width     The number of output columns to write.
 * @param __fill      The character used for the alignment of the output.
 *                    TODO FMT Will probably change to support Unicode grapheme
 *                    cluster.
 * @param __alignment The requested alignment.
 *
 * @returns           An iterator pointing beyond the last element written.
 *
 * @note The type of the elements in range [@a __first, @a __last) can differ
 * from the type of @a __fill. Integer output uses @c std::to_chars for its
 * conversion, which means the [@a __first, @a __last) always contains elements
 * of the type @c char.
 */
// Ignores precision
template <class _CharT, class _ParserCharT>
_LIBCPP_HIDE_FROM_ABI auto __write(const _CharT* __first, const _CharT* __last,
                                   output_iterator<const _CharT&> auto __out_it,
                                   __format_spec::__parser<_ParserCharT> __parser) {
  return __write(__first, __last, _VSTD::move(__out_it), __parser, __last - __first);
}

// Ignores precision
template <class _CharT, class _ParserCharT, class _UnaryOperation>
_LIBCPP_HIDE_FROM_ABI auto __write_transformed(const _CharT* __first, const _CharT* __last,
                                               output_iterator<const _CharT&> auto __out_it,
                                               __format_spec::__parser<_ParserCharT> __parser, _UnaryOperation __op) {
  _LIBCPP_ASSERT(__first <= __last, "Not a valid range");
  _LIBCPP_ASSERT(!__parser.__has_precision(), "Use XXXX");

  ptrdiff_t __size = __last - __first;
  if (__size >= __parser.__width_)
    return _VSTD::transform(__first, __last, _VSTD::move(__out_it), __op);

  __padding_size_result __padding = __padding_size(__size, __parser.__width_, __parser.__alignment_);
  __out_it = _VSTD::fill_n(_VSTD::move(__out_it), __padding.__before_, __parser.__fill_);
  __out_it = _VSTD::transform(__first, __last, _VSTD::move(__out_it), __op);
  return _VSTD::fill_n(_VSTD::move(__out_it), __padding.__after_, __parser.__fill_);
}

/**
 * @overload
 *
 * Writes additional zero's for the precision before the exponent.
 * This is used when the precision requested in the format string is larger
 * than the maximum precision of the floating-point type. These precision
 * digits are always 0.
 *
 * @param __exponent           The location of the exponent character.
 * @param __num_trailing_zeros The number of 0's to write before the exponent
 *                             character.
 */
template <class _CharT, class _ParserCharT>
_LIBCPP_HIDE_FROM_ABI auto __write_using_trailing_zeros(const _CharT* __first, const _CharT* __last,
                                                        output_iterator<const _CharT&> auto __out_it,
                                                        __format_spec::__parser<_ParserCharT> __parser, size_t __size,
                                                        const _CharT* __exponent, size_t __num_trailing_zeros) {
  _LIBCPP_ASSERT(__first <= __last, "Not a valid range");
  _LIBCPP_ASSERT(__num_trailing_zeros > 0, "The overload not writing trailing zeros should have been used");

  __padding_size_result __padding =
      __padding_size(__size + __num_trailing_zeros, __parser.__width_, __parser.__alignment_);
  __out_it = _VSTD::fill_n(_VSTD::move(__out_it), __padding.__before_, __parser.__fill_);
  __out_it = _VSTD::copy(__first, __exponent, _VSTD::move(__out_it));
  __out_it = _VSTD::fill_n(_VSTD::move(__out_it), __num_trailing_zeros, _CharT('0'));
  __out_it = _VSTD::copy(__exponent, __last, _VSTD::move(__out_it));
  return _VSTD::fill_n(_VSTD::move(__out_it), __padding.__after_, __parser.__fill_);
}

#    ifndef _LIBCPP_HAS_NO_UNICODE
template <class _CharT>
_LIBCPP_HIDE_FROM_ABI auto __write_unicode_no_precision(basic_string_view<_CharT> __str,
                                                        output_iterator<const _CharT&> auto __out_it,
                                                        __format_spec::__parser<_CharT> __parser) {
  _LIBCPP_ASSERT(!__parser.__has_precision(), "use __write_unicode");
  // No padding -> copy the string
  if (!__parser.__has_width())
    return _VSTD::copy(__str.begin(), __str.end(), _VSTD::move(__out_it));

  // Non Unicode part larger than width -> copy the string
  auto __last = __format_spec::__detail::__estimate_column_width_fast(__str.begin(), __str.end());
  ptrdiff_t __size = __last - __str.begin();
  if (__size >= __parser.__width_)
    return _VSTD::copy(__str.begin(), __str.end(), _VSTD::move(__out_it));

  // Is there a non Unicode part?
  if (__last != __str.end()) {
    // Non Unicode and Unicode part larger than width -> copy the string
    __format_spec::__detail::__column_width_result __column_width =
        __format_spec::__detail::__estimate_column_width(__last, __str.end(), __parser.__width_);
    __size += __column_width.__width; // Note this new size is used when __size < __parser.__width_
    if (__size >= __parser.__width_)
      return _VSTD::copy(__str.begin(), __str.end(), _VSTD::move(__out_it));
  }

  return __formatter::__write(__str.begin(), __str.end(), _VSTD::move(__out_it), __parser, __size);
}
#    endif

template <class _CharT>
_LIBCPP_HIDE_FROM_ABI auto __write_unicode(basic_string_view<_CharT> __str,
                                           output_iterator<const _CharT&> auto __out_it,
                                           __format_spec::__parser<_CharT> __parser) {
#    ifndef _LIBCPP_HAS_NO_UNICODE
  if (!__parser.__has_precision())
    return __formatter::__write_unicode_no_precision(__str, _VSTD::move(__out_it), __parser);

  // Non unicode part larger than precision -> truncate the output and use the normal write operation.
  auto __last = __format_spec::__detail::__estimate_column_width_fast(__str.begin(), __str.end());
  ptrdiff_t __size = __last - __str.begin();
  if (__size >= __parser.__precision_)
    return __formatter::__write(__str.begin(), __str.begin() + __parser.__precision_, _VSTD::move(__out_it), __parser,
                                __parser.__precision_);

  // No non Unicode part, implies __size <  __parser.__precision_ -> use normal write operation
  if (__last == __str.end())
    return __formatter::__write(__str.begin(), __str.end(), _VSTD::move(__out_it), __parser, __str.size());

  __format_spec::__detail::__column_width_result __column_width =
      __format_spec::__detail::__estimate_column_width(__last, __str.end(), __parser.__precision_ - __size);
  __size += __column_width.__width;
  // Truncate the output
  if (__column_width.__ptr != __str.end())
    __str.remove_suffix(__str.end() - __column_width.__ptr);

  return __formatter::__write(__str.begin(), __str.end(), _VSTD::move(__out_it), __parser, __size);

#    else
  if (__parser.__has_precision()) {
    ptrdiff_t __size = __str.size();
    if (__size > __parser.__precision_)
      return __formatter::__write(__str.begin(), __str.begin() + __parser.__precision_, _VSTD::move(__out_it), __parser,
                                  __parser.__precision_);
  }
  return __formatter::__write(__str.begin(), __str.end(), _VSTD::move(__out_it), __parser, __str.size());

#    endif
}

} // namespace __formatter

#endif //_LIBCPP_STD_VER > 17

_LIBCPP_END_NAMESPACE_STD

#endif // _LIBCPP___FORMAT_FORMATTER_V2_H
