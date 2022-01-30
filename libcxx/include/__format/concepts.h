// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___FORMAT_CONCEPTS_H
#define _LIBCPP___FORMAT_CONCEPTS_H

#include <__concepts/semiregular.h>
#include <__config>
#include <__format/format_context.h>
#include <__format/format_fwd.h>
#include <__format/format_parse_context.h>
#include <type_traits>

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

// Based on P2286R6
template <class _Tp, class _CharT>
concept __formattable = semiregular<formatter<remove_cvref_t<_Tp>, _CharT>> &&
    requires(formatter<remove_cvref_t<_Tp>, _CharT> __f,
             // No LWG-3636 support yet
             /*const*/ formatter<remove_cvref_t<_Tp>, _CharT> __cf, //
             _Tp __t,                                               //
             basic_format_context<_CharT*, _CharT> __fc,            //
             basic_format_parse_context<_CharT> __pc)               //
{
  { __f.parse(__pc) } -> same_as< typename basic_format_parse_context<_CharT>::iterator>;
  { __cf.format(__t, __fc) } -> same_as<_CharT*>;
};

#  endif // !defined(_LIBCPP_HAS_NO_CONCEPTS)

#endif //_LIBCPP_STD_VER > 17

_LIBCPP_END_NAMESPACE_STD

#endif // _LIBCPP___FORMAT_CONCEPTS_H
