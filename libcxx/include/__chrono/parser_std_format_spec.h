// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___CHRONO_PARSER_STD_FORMAT_SPEC_H
#define _LIBCPP___CHRONO_PARSER_STD_FORMAT_SPEC_H

// Make sure all feature-test macros are available.
#include <version>
// Enable the contents of the header only when libc++ was built with LIBCXX_ENABLE_INCOMPLETE_FEATURES.
#if !defined(_LIBCPP_HAS_NO_INCOMPLETE_FORMAT)

#  include <__config>
#  include <__format/format_error.h>
#  include <__format/format_fwd.h> // for __char_type TODO FMT Move the concept?
#  include <__format/format_parse_context.h>
#  include <__format/formatter_v2.h> // __formatter::__format_string
#  include <__format/parser_std_format_spec_v2.h>
#  include <string_view>

#  if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#    pragma GCC system_header
#  endif

_LIBCPP_BEGIN_NAMESPACE_STD

// This code doesn't require locales, but the formatters do.
// Therefore this code is unused when no locale support is available.
#  if _LIBCPP_STD_VER > 17 && !defined(_LIBCPP_HAS_NO_LOCALIZATION)

namespace __format_spec {

// By not placing this constant in the formatter class it's not duplicated for char and wchar_t
inline constexpr __fields __fields_chrono_fractional{
    .__precision_ = true, .__locale_specific_form_ = true, .__type_ = false};
inline constexpr __fields __fields_chrono{.__locale_specific_form_ = true, .__type_ = false};

/// Flags available or required in a chrono type.
///
/// The caller of the chrono formatter lists the types it has available and the
/// validation tests whether the requested type spec (e.g. %M) is available in
/// the formatter.
/// When the type in the chrono-format-spec isn't present in the data a
/// format_error is thrown.
enum class __flags {
  __second = 0x1,
  __minute = 0x2,
  __hour = 0x4,
  __time = __hour | __minute | __second,

  __day = 0x8,
  __month = 0x10,
  __year = 0x20,

  __weekday = 0x40,

  __date = __day | __month | __year | __weekday,

  __date_time = __date | __time,

  __duration = 0x80 | __time,

  __time_zone = 0x100,

  __clock = __date_time | __time_zone
};

_LIBCPP_HIDE_FROM_ABI inline constexpr __flags operator&(__flags __lhs, __flags __rhs) {
  return static_cast<__flags>(static_cast<unsigned>(__lhs) & static_cast<unsigned>(__rhs));
}

_LIBCPP_HIDE_FROM_ABI inline void __validate_second(__flags __flags) {
  if ((__flags & __flags::__second) != __flags::__second)
    std::__throw_format_error("The supplied date time doesn't contain a second");
}

_LIBCPP_HIDE_FROM_ABI inline void __validate_minute(__flags __flags) {
  if ((__flags & __flags::__minute) != __flags::__minute)
    std::__throw_format_error("The supplied date time doesn't contain a minute");
}

_LIBCPP_HIDE_FROM_ABI inline void __validate_hour(__flags __flags) {
  if ((__flags & __flags::__hour) != __flags::__hour)
    std::__throw_format_error("The supplied date time doesn't contain a hour");
}

_LIBCPP_HIDE_FROM_ABI inline void __validate_time(__flags __flags) {
  if ((__flags & __flags::__time) != __flags::__time)
    std::__throw_format_error("The supplied date time doesn't contain a time");
}

_LIBCPP_HIDE_FROM_ABI inline void __validate_day(__flags __flags) {
  if ((__flags & __flags::__day) != __flags::__day)
    std::__throw_format_error("The supplied date time doesn't contain a day");
}

_LIBCPP_HIDE_FROM_ABI inline void __validate_month(__flags __flags) {
  if ((__flags & __flags::__month) != __flags::__month)
    std::__throw_format_error("The supplied date time doesn't contain a month");
}

_LIBCPP_HIDE_FROM_ABI inline void __validate_year(__flags __flags) {
  if ((__flags & __flags::__year) != __flags::__year)
    std::__throw_format_error("The supplied date time doesn't contain a year");
}

_LIBCPP_HIDE_FROM_ABI inline void __validate_date(__flags __flags) {
  if ((__flags & __flags::__date) != __flags::__date)
    std::__throw_format_error("The supplied date time doesn't contain a date");
}

_LIBCPP_HIDE_FROM_ABI inline void __validate_date_or_duration(__flags __flags) {
  if (((__flags & __flags::__date) != __flags::__date) && ((__flags & __flags::__duration) != __flags::__duration))
    std::__throw_format_error("The supplied date time doesn't contain a date or duration");
}

_LIBCPP_HIDE_FROM_ABI inline void __validate_date_time(__flags __flags) {
  if ((__flags & __flags::__date_time) != __flags::__date_time)
    std::__throw_format_error("The supplied date time doesn't contain a date and time");
}

_LIBCPP_HIDE_FROM_ABI inline void __validate_weekday(__flags __flags) {
  if ((__flags & __flags::__weekday) != __flags::__weekday)
    std::__throw_format_error("The supplied date time doesn't contain a weekday");
}

_LIBCPP_HIDE_FROM_ABI inline void __validate_duration(__flags __flags) {
  if ((__flags & __flags::__duration) != __flags::__duration)
    std::__throw_format_error("The supplied date time doesn't contain a duration");
}

_LIBCPP_HIDE_FROM_ABI inline void __validate_time_zone(__flags __flags) {
  if ((__flags & __flags::__time_zone) != __flags::__time_zone)
    std::__throw_format_error("The supplied date time doesn't contain a time zone");
}

template <class _CharT>
class _LIBCPP_TEMPLATE_VIS __parser_chrono {
public:
  _LIBCPP_HIDE_FROM_ABI constexpr auto __parse(basic_format_parse_context<_CharT>& __parse_ctx, __fields __fields,
                                               __flags __flags) -> decltype(__parse_ctx.begin()) {

    const _CharT* __begin = __parser_.__parse(__parse_ctx, __fields);
    const _CharT* __end = __parse_ctx.end();
    if (__begin == __end)
      return __begin;

    const _CharT* __last = __parse_chrono_specs(__begin, __end, __flags);
    __chrono_specs_ = basic_string_view<_CharT>{__begin, __last};

    return __last;
  }

  __parser<_CharT> __parser_;
  basic_string_view<_CharT> __chrono_specs_;

private:
  _LIBCPP_HIDE_FROM_ABI constexpr const _CharT* __parse_chrono_specs(const _CharT* __begin, const _CharT* __end,
                                                                     __flags __flags) {
    _LIBCPP_ASSERT(__begin != __end, "When called with an empty input the function will cause "
                                     "undefined behavior by evaluating data not in the input");

    if (*__begin != _CharT('%') && *__begin != _CharT('}'))
      __throw_format_error("The chrono-specs should start with a '%' or '}'");

    do {
      switch (*__begin) {
      case _CharT('{'):
        __throw_format_error("The chrono-specs contains a '{'");

      case _CharT('}'):
        return __begin;

      case _CharT('%'):
        __parse_escape(__begin, __end, __flags);
        [[fallthrough]];

      default:
        // All other literals
        ++__begin;
      }

    } while (__begin != __end && *__begin != _CharT('}'));

    return __begin;
  }

  // TODO FMT Evaluate __parse_escape
  // Another approach is to have a set of valid escape sequences based on the
  // type being parsed this selection can be done based on the flags or the
  // type. Doing it based on the type may cause the binary size to increment.

  // pre __begin == %
  // post __end = ++__begin
  // When a weekday or monthname are required the __parser_'s status is updated.
  _LIBCPP_HIDE_FROM_ABI constexpr void __parse_escape(const _CharT*& __begin, const _CharT* __end, __flags __flags) {
    ++__begin;
    if (__begin == __end)
      __throw_format_error("End of input while parsing the modifier chrono conversion-spec");

    switch (*__begin) {
    case _CharT('n'):
    case _CharT('t'):
    case _CharT('%'):
      break;

    case _CharT('S'):
      __validate_second(__flags);
      break;

    case _CharT('M'):
      __validate_minute(__flags);
      break;

    case _CharT('p'): // TODO FMT does the formater require an hour or a time?
    case _CharT('H'):
    case _CharT('I'):
      __validate_hour(__flags);
      break;

    case _CharT('r'):
    case _CharT('R'):
    case _CharT('T'):
    case _CharT('X'):
      __validate_time(__flags);
      break;

    case _CharT('d'):
    case _CharT('e'):
      __validate_day(__flags);
      break;

    case _CharT('b'):
    case _CharT('B'):
      __parser_.__month_name_ = true;
      [[fallthrough]];
    case _CharT('h'):
    case _CharT('m'):
      __validate_month(__flags);
      break;

    case _CharT('y'):
    case _CharT('C'):
    case _CharT('Y'):
      __validate_year(__flags);
      break;

    case _CharT('j'):
      __validate_date_or_duration(__flags);
      break;

    case _CharT('g'):
    case _CharT('x'):
    case _CharT('D'):
    case _CharT('F'):
    case _CharT('G'):
    case _CharT('U'):
    case _CharT('V'):
    case _CharT('W'):
      __validate_date(__flags);
      break;

    case _CharT('c'):
      __validate_date_time(__flags);
      break;

    case _CharT('a'):
    case _CharT('A'):
      __parser_.__weekday_name_ = true;
      [[fallthrough]];
    case _CharT('u'):
    case _CharT('w'):
      __validate_weekday(__flags);
      break;

    case _CharT('q'):
    case _CharT('Q'):
      __validate_duration(__flags);
      break;

    case _CharT('E'):
      __parse_modifier_E(__begin, __end, __flags);
      break;

    case _CharT('O'):
      __parse_modifier_O(__begin, __end, __flags);
      break;

    case _CharT('z'):
    case _CharT('Z'):
      // Currently there's no time zone information. However some clocks have a
      // hard-coded "time zone", for these clocks the information can be used.
      // TODO FMT implement time zones.
      __validate_time_zone(__flags);
      break;

    default: // unknown type;
      std::__throw_format_error("The date time type specifier is invalid");
    }
  }

  // pre __begin == E
  // post __end = ++__begin
  _LIBCPP_HIDE_FROM_ABI constexpr void __parse_modifier_E(const _CharT*& __begin, const _CharT* __end,
                                                          __flags __flags) const {
    ++__begin;
    if (__begin == __end)
      __throw_format_error("End of input while parsing the modifier E");

    switch (*__begin) {
    case _CharT('X'):
      __validate_time(__flags);
      break;

    case _CharT('y'):
    case _CharT('C'):
    case _CharT('Y'):
      __validate_year(__flags);
      break;

    case _CharT('x'):
      __validate_date(__flags);
      break;

    case _CharT('c'):
      __validate_date_time(__flags);
      break;

    case _CharT('z'):
      // Currently there's no time zone information. However some clocks have a
      // hard-coded "time zone", for these clocks the information can be used.
      // TODO FMT implement time zones.
      __validate_time_zone(__flags);
      break;

    default:
      std::__throw_format_error("The date time type specifier for modifier E is invalid");
    }
  }

  // pre __begin == O
  // post __end = ++__begin
  _LIBCPP_HIDE_FROM_ABI constexpr void __parse_modifier_O(const _CharT*& __begin, const _CharT* __end,
                                                          __flags __flags) const {
    ++__begin;
    if (__begin == __end)
      __throw_format_error("End of input while parsing the modifier O");

    switch (*__begin) {
    case _CharT('S'):
      __validate_second(__flags);
      break;

    case _CharT('M'):
      __validate_minute(__flags);
      break;

    case _CharT('I'):
    case _CharT('H'):
      __validate_hour(__flags);
      break;

    case _CharT('d'):
    case _CharT('e'):
      __validate_day(__flags);
      break;

    case _CharT('m'):
      __validate_month(__flags);
      break;

    case _CharT('y'):
      __validate_year(__flags);
      break;

    case _CharT('U'):
    case _CharT('V'):
    case _CharT('W'):
      __validate_date(__flags);
      break;

    case _CharT('u'):
    case _CharT('w'):
      __validate_weekday(__flags);
      break;

    case _CharT('z'):
      // Currently there's no time zone information. However some clocks have a
      // hard-coded "time zone", for these clocks the information can be used.
      // TODO FMT implement time zones.
      __validate_time_zone(__flags);
      break;

    default:
      std::__throw_format_error("The date time type specifier for modifier O is invalid");
    }
  }
};

} // namespace __format_spec

#  endif //_LIBCPP_STD_VER > 17 && !defined(_LIBCPP_HAS_NO_LOCALIZATION)

_LIBCPP_END_NAMESPACE_STD

#endif // !defined(_LIBCPP_HAS_NO_INCOMPLETE_FORMAT)

#endif // _LIBCPP___CHRONO_PARSER_STD_FORMAT_SPEC_H
