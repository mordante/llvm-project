// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___CHRONO_FORMATTER
#define _LIBCPP___CHRONO_FORMATTER

// TODO FMT
// - Update synopsis in chrono
// - Update the formattable concept patch (not in main yet)
// - Use Unicode sequences in the tests
// - Add ill-formed tests for format (depends on P2216)

// Make sure all feature-test macros are available.
#include <version>
// Enable the contents of the header only when libc++ was built with LIBCXX_ENABLE_INCOMPLETE_FEATURES.
#if !defined(_LIBCPP_HAS_NO_INCOMPLETE_FORMAT)

#  include <__config>
#  include <__chrono/calendar.h>
#  include <__chrono/day.h>
#  include <__chrono/parser_std_format_spec.h>
#  include <__chrono/weekday.h>
#  include <__concepts/arithmetic.h>
#  include <__concepts/same_as.h>
#  include <__memory/addressof.h>
#  include <cmath>
#  include <ctime>
#  include <format> // Need entire header for stream operators
#  include <iterator>
#  include <ratio>
#  include <sstream>
#  include <string_view>

#  if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#    pragma GCC system_header
#  endif

_LIBCPP_BEGIN_NAMESPACE_STD

// The time format functions use ostream which requires locale support.
#  if _LIBCPP_STD_VER > 17 && !defined(_LIBCPP_HAS_NO_LOCALIZATION)

#    ifndef _LIBCPP_HAS_NO_WIDE_CHARACTERS
template <__formatter::__char_type _CharT>
constexpr const _CharT* __statically_widen(const char* __str, const wchar_t* __wstr) {
  if constexpr (same_as<_CharT, char>)
    return __str;
  else
    return __wstr;
}
#      define _STATICALLY_WIDEN(__str) (__statically_widen<_CharT>(__str, L##__str))
#    else // _LIBCPP_HAS_NO_WIDE_CHARACTERS
#      define _STATICALLY_WIDEN(__str) (__str)
#    endif // _LIBCPP_HAS_NO_WIDE_CHARACTERS

namespace chrono {

template <class _CharT, class _Period>
_LIBCPP_HIDE_FROM_ABI auto __units_suffix() {
  // LWG the suffixes are always char and not STATICALLY-WIDEN'ed
  // TODO FMT The code implements LWG-xxxx because the wording in the
  if constexpr (same_as<typename _Period::type, atto>)
    return _STATICALLY_WIDEN("as");
  else if constexpr (same_as<typename _Period::type, femto>)
    return _STATICALLY_WIDEN("fs");
  else if constexpr (same_as<typename _Period::type, pico>)
    return _STATICALLY_WIDEN("ps");
  else if constexpr (same_as<typename _Period::type, nano>)
    return _STATICALLY_WIDEN("ns");
  else if constexpr (same_as<typename _Period::type, micro>)
#    ifndef _LIBCPP_HAS_NO_UNICODE
    return _STATICALLY_WIDEN("\u00b5s");
#    else
    return _STATICALLY_WIDEN("us");
#    endif
  else if constexpr (same_as<typename _Period::type, milli>)
    return _STATICALLY_WIDEN("ms");
  else if constexpr (same_as<typename _Period::type, centi>)
    return _STATICALLY_WIDEN("cs");
  else if constexpr (same_as<typename _Period::type, deci>)
    return _STATICALLY_WIDEN("ds");
  else if constexpr (same_as<typename _Period::type, ratio<1>>)
    return _STATICALLY_WIDEN("s");
  else if constexpr (same_as<typename _Period::type, deca>)
    return _STATICALLY_WIDEN("das");
  else if constexpr (same_as<typename _Period::type, hecto>)
    return _STATICALLY_WIDEN("hs");
  else if constexpr (same_as<typename _Period::type, kilo>)
    return _STATICALLY_WIDEN("ks");
  else if constexpr (same_as<typename _Period::type, mega>)
    return _STATICALLY_WIDEN("Ms");
  else if constexpr (same_as<typename _Period::type, giga>)
    return _STATICALLY_WIDEN("Gs");
  else if constexpr (same_as<typename _Period::type, tera>)
    return _STATICALLY_WIDEN("Ts");
  else if constexpr (same_as<typename _Period::type, peta>)
    return _STATICALLY_WIDEN("Ps");
  else if constexpr (same_as<typename _Period::type, exa>)
    return _STATICALLY_WIDEN("Es");
  else if constexpr (same_as<typename _Period::type, ratio<60>>)
    return _STATICALLY_WIDEN("min");
  else if constexpr (same_as<typename _Period::type, ratio<3600>>)
    return _STATICALLY_WIDEN("h");
  else if constexpr (same_as<typename _Period::type, ratio<86400>>)
    return _STATICALLY_WIDEN("d");
  else if constexpr (_Period::den == 1)
    return format(_STATICALLY_WIDEN("[{}]s"), _Period::num);
  else
    return format(_STATICALLY_WIDEN("[{}/{}]s"), _Period::num, _Period::den);
}
} // namespace chrono

namespace __formatter {

/// Formats a time based on a tm struct.
///
/// This formatter passes the formatting to time_put which uses strftime. When
/// the value is outside the valid range it's unspecified what strftime will
/// output. For example weekday 8, can print 1 when the day is processed module
/// 7 since that handles the Sunday for 0-based weekday. It can also print 8 if
/// 7 is handled as a special case.)
///
/// The Standard doesn't specify what to do in this case so the result depends
/// on the result of the underlaying code.
///
/// \pre When the (abbreviated) weekday on month name are used the caller
///      validates the value is valid. So the caller handles that requirement
///      of Table 97: Meaning of conversion specifiers [tab:time.format.spec].
template <class _CharT>
_LIBCPP_HIDE_FROM_ABI auto __format_chrono(const tm& __t, auto& __ctx, __format_spec::__parser<_CharT> __parser,
                                           basic_string_view<_CharT> __chrono_specs) -> decltype(__ctx.out()) {
  basic_stringstream<_CharT> __sstr;
  // http://eel.is/c++draft/time.format#2
  // 2.1 - the "C" locale if the L option is not present in chrono-format-spec, otherwise
  // 2.2 - the locale passed to the formatting function if any, otherwise
  // 2.3 - the global locale.
  // Note that the __ctx's locale() call does 2.2 and 2.3.
  if (__parser.__locale_specific_form_)
    __sstr.imbue(__ctx.locale());
  else
    __sstr.imbue(locale::classic());

  // TODO FMT when there's no width see whether it's possible to directly write to the output.
  const auto& __facet = std::use_facet<time_put<_CharT>>(__sstr.getloc());
  // TODO FMT When there's no width the output can directly be written to the output.
  __facet.put({__sstr}, __sstr, _CharT(' '), std::addressof(__t), __chrono_specs.data(),
              __chrono_specs.data() + __chrono_specs.size());

  // TODO FMT Use view after P0408R7 has been implemented.
  basic_string<_CharT> __str = __sstr.str();
  return __formatter::__format_string(basic_string_view<_CharT>{__str}, __ctx.out(), __parser);
}

/// Formats a time based on a tm struct.
///
/// When no chrono-specs are provided it uses the stream formatter.
template <class _CharT, class _Tp>
_LIBCPP_HIDE_FROM_ABI auto __format_chrono_no_chrono_specs(_Tp __value, auto& __ctx,
                                                           __format_spec::__parser<_CharT> __parser)
    -> decltype(__ctx.out()) {

  // TODO FMT see whether can directly write to output when width == 0
  // TODO FMT for duration with precision need to clear precision before sending to __format_string else gets truncated

  basic_stringstream<_CharT> __sstr;
  // http://eel.is/c++draft/time.format#2
  // 2.1 - the "C" locale if the L option is not present in chrono-format-spec, otherwise
  // 2.2 - the locale passed to the formatting function if any, otherwise
  // 2.3 - the global locale.
  // Note that the __ctx's locale() call does 2.2 and 2.3.
  if (__parser.__locale_specific_form_)
    __sstr.imbue(__ctx.locale());
  else
    __sstr.imbue(locale::classic());

  __sstr << __value;

  // TODO FMT Use view after P0408R7 has been implemented.
  basic_string<_CharT> __str = __sstr.str();
  return __formatter::__format_string(basic_string_view<_CharT>{__str}, __ctx.out(), __parser);
}
// If the _Rep is a floating point or the period < 1 seconds need a special handling
// [tab:time.format.spec] If the precision of the input cannot be exactly
// represented with seconds, then the format is a decimal floating-point number
// with a fixed format and a precision matching that of the precision of the
// input (or to a microseconds precision if the conversion to floating-point
// decimal seconds cannot be made within 18 fractional digits).
//
// This wording is a bit "opaque" for now the following rules are used
// - floating_point<_Rep> -> 6 digits
// - _Period::type, milli -> 3 digits
// - _Period::type, centi -> 2 digits
// - _Period::type, deci -> 1 digits
// - Otherwise 6 digits
// The exact number of digits might be "hard" to determine 3 - 1/3 = 1 so would
// require no digits, but the conversion to float might not exactly give 1.0.
// chrono::duration<double, std::milli>
template <class _CharT, class _Rep, class _Period>
_LIBCPP_HIDE_FROM_ABI void __format_seconds_with_fraction(chrono::duration<_Rep, _Period> __value,
                                                          basic_stringstream<_CharT>& __sstr) {
  using _Fraction = conditional_t<floating_point<_Rep>, _Rep, double>;
  _Fraction __sec = fmod(static_cast<_Fraction>(__value.count()) * _Period::num / _Period::den, _Fraction(60));

  int __decimals = 6;
  if constexpr (same_as<typename _Period::type, milli>)
    __decimals = 3;
  else if constexpr (same_as<typename _Period::type, centi>)
    __decimals = 2;
  else if constexpr (same_as<typename _Period::type, deci>)
    __decimals = 1;

  if (__sec < 10)
    __sstr << _CharT('0');

  std::format_to(std::ostreambuf_iterator<_CharT>{__sstr}, __sstr.getloc(), _STATICALLY_WIDEN("{:.{}Lf}"), __sec,
                 __decimals);
}

template <class _CharT, class _Rep, class _Period>
_LIBCPP_HIDE_FROM_ABI auto __format_duration(chrono::duration<_Rep, _Period> __value, auto& __ctx,
                                             __format_spec::__parser<_CharT> __parser,
                                             basic_string_view<_CharT> __chrono_specs) -> decltype(__ctx.out()) {
  constexpr bool __use_fraction = floating_point<_Rep> | ratio_less_v<_Period, ratio<1>>;

  basic_stringstream<_CharT> __sstr;
  // http://eel.is/c++draft/time.format#2
  // 2.1 - the "C" locale if the L option is not present in chrono-format-spec, otherwise
  // 2.2 - the locale passed to the formatting function if any, otherwise
  // 2.3 - the global locale.
  // Note that the __ctx's locale() call does 2.2 and 2.3.
  if (__parser.__locale_specific_form_)
    __sstr.imbue(__ctx.locale());
  else
    __sstr.imbue(locale::classic());

  // TODO FMT when there's no width see whether it's possible to directly write to the output.
  const auto& __facet = std::use_facet<time_put<_CharT>>(__sstr.getloc());
  // TODO FMT When there's no width the output can directly be written to the output.

  // [time.format]/4 ... with a STATICALLY-WIDEN<charT>("-") character sequence
  // placed before the replacement of the initial conversion specifier.
  // [time.format]/1 requires the chrono-specs to start with a conversion specifier.
  // Therefore the sign can be written first without any additional tests.
  if (__value.count() < 0) {
    __sstr << _CharT('-');
    __value = -__value;
  }

  // This "reparses" the __chrono_specs. The original parse function made sure
  // the input is valid so this function doesn't need to handle validation.

  // TODO FMT These fields could be evaluated lazily
  // TODO FMT This seconds calulatation is crude and might overflow.
  uint64_t __sec = __value.count() * _Period::num / _Period::den;
  uint64_t __days = __sec / (24 * 3600);
  __sec %= 24 * 3600;

  tm __t{};
  __t.tm_hour = __sec / 3600;
  __sec %= 3600;
  __t.tm_min = __sec / 60;
  __t.tm_sec = __sec % 60;

  for (auto __it = __chrono_specs.begin(); __it != __chrono_specs.end(); ++__it) {
    if (*__it == _CharT('%')) {
      auto __s = __it;
      ++__it;
      // We only handle the types that can't be directly handled by time_put.
      // As optimization %%, %t, and %n are also processed here.
      switch (*__it) {
      case _CharT('%'):
        __sstr << *__it;
        break;
      case _CharT('t'):
        __sstr << _CharT('\t');
        break;
      case _CharT('n'):
        __sstr << _CharT('\n');
        break;
      case _CharT('j'):
        __sstr << __days;
        break;
      case _CharT('q'):
        __sstr << chrono::__units_suffix<_CharT, _Period>();
        break;
      case _CharT('Q'):
        __sstr << __value.count();
        break;
      case _CharT('S'):
        if constexpr (__use_fraction)
          __format_seconds_with_fraction(__value, __sstr);
        else
          __facet.put({__sstr}, __sstr, _CharT(' '), std::addressof(__t), __s, __it + 1);
        break;
      case _CharT('T'):
        if constexpr (__use_fraction) {
          __facet.put({__sstr}, __sstr, _CharT(' '), std::addressof(__t), 'R');
          __sstr << _CharT(':');
          __format_seconds_with_fraction(__value, __sstr);
        } else
          __facet.put({__sstr}, __sstr, _CharT(' '), std::addressof(__t), __s, __it + 1);
        break;
      case _CharT('E'):
      case _CharT('O'):
        ++__it;
        // TODO FMT special case for OS and fractional
        [[fallthrough]];
      default:
        __facet.put({__sstr}, __sstr, _CharT(' '), std::addressof(__t), __s, __it + 1);
        break;
      }
    } else {
      // TODO FMT Copying a substring is probably cheaper.
      __sstr << *__it;
    }
  }

  // TODO FMT Use view after P0408R7 has been implemented.
  basic_string<_CharT> __str = __sstr.str();
  return __formatter::__format_string(basic_string_view<_CharT>{__str}, __ctx.out(), __parser);
}

template <class _CharT, class _Duration>
_LIBCPP_HIDE_FROM_ABI auto __format_sys_time(chrono::sys_time<_Duration> __value, auto& __ctx,
                                             __format_spec::__parser<_CharT> __parser,
                                             basic_string_view<_CharT> __chrono_specs) -> decltype(__ctx.out()) {
  basic_stringstream<_CharT> __sstr;
  // http://eel.is/c++draft/time.format#2
  // 2.1 - the "C" locale if the L option is not present in chrono-format-spec, otherwise
  // 2.2 - the locale passed to the formatting function if any, otherwise
  // 2.3 - the global locale.
  // Note that the __ctx's locale() call does 2.2 and 2.3.
  if (__parser.__locale_specific_form_)
    __sstr.imbue(__ctx.locale());
  else
    __sstr.imbue(locale::classic());

  // XXX helper function???
  time_t __tt = chrono::system_clock::to_time_t(__value);
  tm __t{};
  gmtime_r(std::addressof(__tt), std::addressof(__t));

  // TODO FMT when there's no width see whether it's possible to directly write to the output.
  const auto& __facet = std::use_facet<time_put<_CharT>>(__sstr.getloc());
#    if __GLIBC__
  __t.tm_zone = "UTC";
  __facet.put({__sstr}, __sstr, _CharT(' '), std::addressof(__t), __chrono_specs.data(),
              __chrono_specs.data() + __chrono_specs.size());
#    else
  for (auto __it = __chrono_specs.begin(); __it != __chrono_specs.end(); ++__it) {
    if (*__it == _CharT('%')) {
      auto __s = __it;
      ++__it;
      // We only handle the types that can't be directly handled by time_put.
      // As optimization %%, %t, and %n are also processed here.
      switch (*__it) {
      case _CharT('%'):
        __sstr << *__it;
        break;
      case _CharT('t'):
        __sstr << _CharT('\t');
        break;
      case _CharT('n'):
        __sstr << _CharT('\n');
        break;
      case _CharT('Z'):
        __sstr << _STATICALLY_WIDEN("UTC");
        break;
      case _CharT('E'):
      case _CharT('O'):
        ++__it;
        [[fallthrough]];
      default:
        __facet.put({__sstr}, __sstr, _CharT(' '), std::addressof(__t), __s, __it + 1);
        break;
      }
    } else {
      // TODO FMT Copying a substring is probably cheaper.
      __sstr << *__it;
    }
  }
#    endif
  // TODO FMT Use view after P0408R7 has been implemented.
  basic_string<_CharT> __str = __sstr.str();
  return __formatter::__format_string(basic_string_view<_CharT>{__str}, __ctx.out(), __parser);
}

} // namespace __formatter

template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT __formatter_chrono {
public:
  _LIBCPP_HIDE_FROM_ABI constexpr auto parse(basic_format_parse_context<_CharT>& __parse_ctx,
                                             __format_spec::__fields __fields, __format_spec::__flags __flags)
      -> decltype(__parse_ctx.begin()) {

    return __parser_.__parse(__parse_ctx, __fields, __flags);
  }

  _LIBCPP_HIDE_FROM_ABI auto format(const tm& __value, auto& __ctx) const -> decltype(__ctx.out()) const {
    return __formatter::__format_chrono(__value, __ctx, __parser_.__parser_.__resolve_dynamic_sizes(__ctx),
                                        __parser_.__chrono_specs_);
  }

  _LIBCPP_HIDE_FROM_ABI auto format(chrono::day __value, auto& __ctx) const -> decltype(__ctx.out()) {
    if (__parser_.__chrono_specs_.empty())
      return __formatter::__format_chrono_no_chrono_specs(__value, __ctx,
                                                          __parser_.__parser_.__resolve_dynamic_sizes(__ctx));

    tm __t{};
    __t.tm_mday = static_cast<unsigned>(__value);
    return format(__t, __ctx);
  }

  _LIBCPP_HIDE_FROM_ABI auto format(chrono::weekday __value, auto& __ctx) const -> decltype(__ctx.out()) {
    if (__parser_.__chrono_specs_.empty())
      return __formatter::__format_chrono_no_chrono_specs(__value, __ctx,
                                                          __parser_.__parser_.__resolve_dynamic_sizes(__ctx));

    if (this->__weekday_name() && !__value.ok())
      __throw_format_error("formatting a weekday name needs a valid value");

    tm __t{};
    __t.tm_wday = __value.c_encoding();
    return format(__t, __ctx);
  }

  // XXX add a do_format and move `__parser_.__chrono_specs_.empty()` test to a generic part?
  template <class _Rep, class _Period>
  _LIBCPP_HIDE_FROM_ABI auto format(chrono::duration<_Rep, _Period> __value, auto& __ctx) const
      -> decltype(__ctx.out()) {
    if (__parser_.__chrono_specs_.empty())
      return __formatter::__format_chrono_no_chrono_specs(__value, __ctx,
                                                          __parser_.__parser_.__resolve_dynamic_sizes(__ctx));

    return __formatter::__format_duration(__value, __ctx, __parser_.__parser_.__resolve_dynamic_sizes(__ctx),
                                          __parser_.__chrono_specs_);
  }

  template <class _Duration>
  _LIBCPP_HIDE_FROM_ABI auto format(chrono::sys_time<_Duration> __value, auto& __ctx) const -> decltype(__ctx.out()) {
    if (__parser_.__chrono_specs_.empty())
      return __formatter::__format_chrono_no_chrono_specs(__value, __ctx,
                                                          __parser_.__parser_.__resolve_dynamic_sizes(__ctx));

    return __formatter::__format_sys_time(__value, __ctx, __parser_.__parser_.__resolve_dynamic_sizes(__ctx),
                                          __parser_.__chrono_specs_);
  }

  _LIBCPP_HIDE_FROM_ABI constexpr bool __weekday_name() const { return __parser_.__parser_.__weekday_name_; }
  _LIBCPP_HIDE_FROM_ABI constexpr bool __month_name() const { return __parser_.__parser_.__month_name_; }

  __format_spec::__parser_chrono<_CharT> __parser_;
};

// TODO FMT Move to day.h
template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<chrono::day, _CharT>
    : public __formatter_chrono<_CharT> {
public:
  using _Base = __formatter_chrono<_CharT>;

  _LIBCPP_HIDE_FROM_ABI constexpr auto parse(basic_format_parse_context<_CharT>& __parse_ctx)
      -> decltype(__parse_ctx.begin()) {
    return _Base::parse(__parse_ctx, __format_spec::__fields_chrono, __format_spec::__flags::__day);
  }
};

namespace chrono {
// TODO add tests for operator<<
template <class _CharT, class _Traits>
_LIBCPP_HIDE_FROM_ABI _LIBCPP_AVAILABILITY_FORMAT basic_ostream<_CharT, _Traits>&
operator<<(basic_ostream<_CharT, _Traits>& __os, const day& __d) {
  return __os << (__d.ok() ? std::format(_STATICALLY_WIDEN("{:%d}"), __d)
                           : std::format(_STATICALLY_WIDEN("{:%d} is not a valid day"), __d));
}

} // namespace chrono

// TODO FMT Move
template <__formatter::__char_type _CharT>
struct _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT formatter<chrono::weekday, _CharT>
    : public __formatter_chrono<_CharT> {
public:
  using _Base = __formatter_chrono<_CharT>;

  _LIBCPP_HIDE_FROM_ABI constexpr auto parse(basic_format_parse_context<_CharT>& __parse_ctx)
      -> decltype(__parse_ctx.begin()) {
    return _Base::parse(__parse_ctx, __format_spec::__fields_chrono, __format_spec::__flags::__weekday);
  }
};

namespace chrono {
// TODO add tests for operator<<
template <class _CharT, class _Traits>
_LIBCPP_HIDE_FROM_ABI _LIBCPP_AVAILABILITY_FORMAT basic_ostream<_CharT, _Traits>&
operator<<(basic_ostream<_CharT, _Traits>& __os, const weekday& __wd) {
  // LWG
  // Why %a instead of %A
  // Month has the same
  // Seems to be no LWG issue yet
  return __os << (__wd.ok() ? std::format(_STATICALLY_WIDEN("{:L%a}"), __wd)
                            : std::format(_STATICALLY_WIDEN("{} is not a valid weekday"),
                                          static_cast<unsigned>(__wd.c_encoding())));
}

} // namespace chrono

// TODO FMT Move
template <class _Rep, class _Period, __formatter::__char_type _CharT>
struct formatter<chrono::duration<_Rep, _Period>, _CharT> : public __formatter_chrono<_CharT> {
public:
  using _Base = __formatter_chrono<_CharT>;

  _LIBCPP_HIDE_FROM_ABI constexpr auto parse(basic_format_parse_context<_CharT>& __parse_ctx)
      -> decltype(__parse_ctx.begin()) {
    if constexpr (floating_point<_Rep>)
      // XXX fraction not only be floating point allowed, also for ns.
      // TODO FMT this value seems to be ignored... at least it's not tested
      return _Base::parse(__parse_ctx, __format_spec::__fields_chrono_fractional, __format_spec::__flags::__duration);
    else
      return _Base::parse(__parse_ctx, __format_spec::__fields_chrono, __format_spec::__flags::__duration);
  }
};

namespace chrono {
// TODO add tests for operator<<
template <class _CharT, class _Traits, class _Rep, class _Period>
_LIBCPP_HIDE_FROM_ABI _LIBCPP_AVAILABILITY_FORMAT basic_ostream<_CharT, _Traits>&
operator<<(basic_ostream<_CharT, _Traits>& __os, const duration<_Rep, _Period>& __d) {

  basic_ostringstream<_CharT, _Traits> __s;
  __s.flags(__os.flags());
  __s.imbue(__os.getloc());
  __s.precision(__os.precision()); // XXX Used??
  __s << __d.count() << __units_suffix<_CharT, _Period>();
  return __os << __s.str();
}

} // namespace chrono

// TODO FMT Move
template <class _Duration, __formatter::__char_type _CharT>
struct formatter<chrono::sys_time<_Duration>, _CharT> : public __formatter_chrono<_CharT> {
public:
  using _Base = __formatter_chrono<_CharT>;

  _LIBCPP_HIDE_FROM_ABI constexpr auto parse(basic_format_parse_context<_CharT>& __parse_ctx)
      -> decltype(__parse_ctx.begin()) {
    /*    if constexpr (floating_point<_Rep>)
      return _Base::parse(__parse_ctx, __format_spec::__fields_chrono_fractional, __format_spec::__flags::__duration);
    else*/
    return _Base::parse(__parse_ctx, __format_spec::__fields_chrono, __format_spec::__flags::__clock);
  }
};

namespace chrono {
// TODO add tests for operator<<
template <class _CharT, class _Traits, class _Duration>
_LIBCPP_HIDE_FROM_ABI _LIBCPP_AVAILABILITY_FORMAT basic_ostream<_CharT, _Traits>&
operator<<(basic_ostream<_CharT, _Traits>& __os, const sys_time<_Duration> __tp) {
  return __os << std::format(_STATICALLY_WIDEN("{:L%F %T}"), __tp);
}

} // namespace chrono

#  endif //if _LIBCPP_STD_VER > 17 && !defined(_LIBCPP_HAS_NO_LOCALIZATION)

_LIBCPP_END_NAMESPACE_STD

#endif // !defined(_LIBCPP_HAS_NO_INCOMPLETE_FORMAT)

#endif //  _LIBCPP___CHRONO_FORMATTER
