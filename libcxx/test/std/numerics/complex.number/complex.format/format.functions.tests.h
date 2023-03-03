//===----------------------------------------------------------------------===//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef TEST_STD_NUMERICS_COMPLEX_NUMBER_COMPLEX_FORMAT_FORMAT_FUNCTIONS_TESTS_H
#define TEST_STD_NUMERICS_COMPLEX_NUMBER_COMPLEX_FORMAT_FORMAT_FUNCTIONS_TESTS_H

#include <concepts>
#include <array>
#include <format>
#include <complex>

#include "format.functions.common.h"

template <class CharT, class F, class TestFunction, class ExceptionTest>
void test_complex(TestFunction check, ExceptionTest check_exception) {
  std::complex<F> input{0, 0};
  check(SV("(0+0i)"), SV("{}"), input);

  // ***** underlying has no format-spec

  // *** align-fill & width ***
  check(SV("     (0+0i)"), SV("{:11}"), input);
  check(SV("(0+0i)*****"), SV("{:*<11}"), input);
  check(SV("__(0+0i)___"), SV("{:_^11}"), input);
  check(SV("#####(0+0i)"), SV("{:#>11}"), input);

  check(SV("     (0+0i)"), SV("{:{}}"), input, 11);
  check(SV("(0+0i)*****"), SV("{:*<{}}"), input, 11);
  check(SV("__(0+0i)___"), SV("{:_^{}}"), input, 11);
  check(SV("#####(0+0i)"), SV("{:#>{}}"), input, 11);

  check_exception("The format-spec range-fill field contains an invalid character", SV("{:}<}"), input);
  check_exception("The format-spec range-fill field contains an invalid character", SV("{:{<}"), input);
  check_exception("The format-spec range-fill field contains an invalid character", SV("{::<}"), input);

  // *** sign ***
  check_exception("The format-spec should consume the input or end with a '}'", SV("{:-}"), input);
  check_exception("The format-spec should consume the input or end with a '}'", SV("{:+}"), input);
  check_exception("The format-spec should consume the input or end with a '}'", SV("{: }"), input);

  // *** alternate form ***
  check_exception("The format-spec should consume the input or end with a '}'", SV("{:#}"), input);

  // *** zero-padding ***
  check_exception("A format-spec width field shouldn't have a leading zero", SV("{:0}"), input);

  // *** precision ***
  check_exception("The format-spec should consume the input or end with a '}'", SV("{:.}"), input);

  // *** locale-specific form ***
  check_exception("The format-spec should consume the input or end with a '}'", SV("{:L}"), input);

  // *** n ***
  check(SV("0+0i"), SV("{:n}"), input);
  check(SV("0+0i"), SV("{:nr}"), input);
  check(SV("0,0"), SV("{:nS}"), input);

  // *** type ***
  check(SV("(0+0i)"), SV("{:r}"), input);
  check(SV("(0,0)"), SV("{:S}"), input);
  for (std::basic_string_view<CharT> fmt : fmt_invalid_types<CharT>("rS"))
    check_exception("The format-spec type has a type not supported for a complex type", fmt, input);

  // ***** Only underlying has a format-spec
  check(SV("(   0 +0i)"), SV("{::4}"), input);
  check(SV("(0***+0i*)"), SV("{::*<4}"), input);
  check(SV("(_0__+0i_)"), SV("{::_^4}"), input);
  check(SV("(:::0:+0i)"), SV("{:::>4}"), input);

  check(SV("(   0 +0i)"), SV("{::{}}"), input, 4);
  check(SV("(0***+0i*)"), SV("{::*<{}}"), input, 4);
  check(SV("(_0__+0i_)"), SV("{::_^{}}"), input, 4);
  check(SV("(:::0:+0i)"), SV("{:::>{}}"), input, 4);

  check_exception("The format-spec fill field contains an invalid character", SV("{::}<}"), input);
  check_exception("The format-spec fill field contains an invalid character", SV("{::{<}"), input);

  // *** sign ***
  check(SV("(0+0i)"), SV("{::-}"), input);
  check(SV("(+0+0i)"), SV("{::+}"), input);
  check(SV("( 0 0i)"), SV("{:: }"), input);

  // *** alternate form ***
  check(SV("(0.+0.i)"), SV("{::#}"), input);

  // *** zero-padding & width ***
  check(SV("(0000+00i)"), SV("{::04}"), input);

  // *** precision ***
  // depending on the type the precision gets different output
  check(SV("(0+0i)"), SV("{::.3}"), input);
  check(SV("(0+0i)"), SV("{::.{}}"), input, 3);

  // *** locale-specific form ***
  // No effect for this value
  check(SV("(0+0i)"), SV("{::L}"), input);

  // *** type ***
  for (std::basic_string_view<CharT> fmt : fmt_invalid_nested_types<CharT>("aAeEfFgG"))
    check_exception("The format-spec type has a type not supported for a floating-point argument", fmt, input);

  // ***** Both have a format-spec
  check(SV("^^^(:::0:+0i)^^^"), SV("{:^^16::>4}"), input);
  check(SV("^^^(:::0:+0i)^^^"), SV("{:^^{}::>4}"), input, 16);
  check(SV("^^^(:::0:+0i)^^^"), SV("{:^^{}::>{}}"), input, 16, 4);

  check_exception("Argument index out of bounds", SV("{:^^{}::>4}"), input);
  check_exception("Argument index out of bounds", SV("{:^^{}::>{}}"), input, 16);
}

template <class CharT, class TestFunction, class ExceptionTest>
void test_complex_non_finite(TestFunction check, ExceptionTest check_exception) {
  using F = double;

  std::complex<F> nan{std::numeric_limits<F>::quiet_NaN(), std::numeric_limits<F>::quiet_NaN()};
  std::array input = {
      nan,
      std::complex<F>{std::copysign(nan.real(), static_cast<F>(-1.0)), std::copysign(nan.real(), static_cast<F>(-1.0))},
      std::complex<F>{std::numeric_limits<F>::infinity(), std::numeric_limits<F>::infinity()},
      std::complex<F>{-std::numeric_limits<F>::infinity(), -std::numeric_limits<F>::infinity()}};

  check(SV("[(nan+nan i), (-nan-nan i), (inf+inf i), (-inf-inf i)]"), SV("{}"), input);

  // ***** underlying has no format-spec

  // *** align-fill & width ***
  check(SV("[(nan+nan i), (-nan-nan i), (inf+inf i), (-inf-inf i)]     "), SV("{:59}"), input);
  check(SV("[(nan+nan i), (-nan-nan i), (inf+inf i), (-inf-inf i)]*****"), SV("{:*<59}"), input);
  check(SV("__[(nan+nan i), (-nan-nan i), (inf+inf i), (-inf-inf i)]___"), SV("{:_^59}"), input);
  check(SV("#####[(nan+nan i), (-nan-nan i), (inf+inf i), (-inf-inf i)]"), SV("{:#>59}"), input);

  check(SV("[(nan+nan i), (-nan-nan i), (inf+inf i), (-inf-inf i)]     "), SV("{:{}}"), input, 59);
  check(SV("[(nan+nan i), (-nan-nan i), (inf+inf i), (-inf-inf i)]*****"), SV("{:*<{}}"), input, 59);
  check(SV("__[(nan+nan i), (-nan-nan i), (inf+inf i), (-inf-inf i)]___"), SV("{:_^{}}"), input, 59);
  check(SV("#####[(nan+nan i), (-nan-nan i), (inf+inf i), (-inf-inf i)]"), SV("{:#>{}}"), input, 59);

  check_exception("The format-spec range-fill field contains an invalid character", SV("{:}<}"), input);
  check_exception("The format-spec range-fill field contains an invalid character", SV("{:{<}"), input);
  check_exception("The format-spec range-fill field contains an invalid character", SV("{::<}"), input);

  // *** sign ***
  check_exception("The format-spec should consume the input or end with a '}'", SV("{:-}"), input);
  check_exception("The format-spec should consume the input or end with a '}'", SV("{:+}"), input);
  check_exception("The format-spec should consume the input or end with a '}'", SV("{: }"), input);

  // *** alternate form ***
  check_exception("The format-spec should consume the input or end with a '}'", SV("{:#}"), input);

  // *** zero-padding ***
  check_exception("A format-spec width field shouldn't have a leading zero", SV("{:0}"), input);

  // *** precision ***
  check_exception("The format-spec should consume the input or end with a '}'", SV("{:.}"), input);

  // *** locale-specific form ***
  check_exception("The format-spec should consume the input or end with a '}'", SV("{:L}"), input);

  // *** n ***
  check(SV("(nan+nan i), (-nan-nan i), (inf+inf i), (-inf-inf i)"), SV("{:n}"), input);

  // *** type ***
  check_exception("The range-format-spec type s requires formatting a character type", SV("{:s}"), input);
  check_exception("The range-format-spec type ?s requires formatting a character type", SV("{:?s}"), input);
  for (std::basic_string_view<CharT> fmt : fmt_invalid_types<CharT>("fnos"))
    check_exception("The format-spec should consume the input or end with a '}'", fmt, input);

  // ***** Only underlying has a format-spec

  // *** align-fill & width ***
  check(SV("[      (nan+nan i),      (-nan-nan i),       (inf+inf i),      (-inf-inf i)]"), SV("{::17}"), input);
  check(SV("[(nan+nan i)******, (-nan-nan i)*****, (inf+inf i)******, (-inf-inf i)*****]"), SV("{::*<17}"), input);
  check(SV("[___(nan+nan i)___, __(-nan-nan i)___, ___(inf+inf i)___, __(-inf-inf i)___]"), SV("{::_^17}"), input);
  check(SV("[######(nan+nan i), #####(-nan-nan i), ######(inf+inf i), #####(-inf-inf i)]"), SV("{::#>17}"), input);

  check(SV("[      (nan+nan i),      (-nan-nan i),       (inf+inf i),      (-inf-inf i)]"), SV("{::{}}"), input, 17);
  check(SV("[(nan+nan i)******, (-nan-nan i)*****, (inf+inf i)******, (-inf-inf i)*****]"), SV("{::*<{}}"), input, 17);
  check(SV("[___(nan+nan i)___, __(-nan-nan i)___, ___(inf+inf i)___, __(-inf-inf i)___]"), SV("{::_^{}}"), input, 17);
  check(SV("[######(nan+nan i), #####(-nan-nan i), ######(inf+inf i), #####(-inf-inf i)]"), SV("{::#>{}}"), input, 17);

  check_exception("The format-spec range-fill field contains an invalid character", SV("{::}<}"), input);
  check_exception("The format-spec range-fill field contains an invalid character", SV("{::{<}"), input);
  check_exception("The format-spec range-fill field contains an invalid character", SV("{:::<}"), input);

  // *** sign ***
  check_exception("The format-spec should consume the input or end with a '}'", SV("{::-}"), input);
  check_exception("The format-spec should consume the input or end with a '}'", SV("{::+}"), input);
  check_exception("The format-spec should consume the input or end with a '}'", SV("{:: }"), input);

  // *** alternate form ***
  check_exception("The format-spec should consume the input or end with a '}'", SV("{::#}"), input);

  // *** zero-padding ***
  check_exception("A format-spec width field shouldn't have a leading zero", SV("{::0}"), input);

  // *** precision ***
  check_exception("The format-spec should consume the input or end with a '}'", SV("{::.}"), input);

  // *** locale-specific form ***
  check_exception("The format-spec should consume the input or end with a '}'", SV("{::L}"), input);

  // *** n ***
  check(SV("[nan+nan i, -nan-nan i, inf+inf i, -inf-inf i]"), SV("{::n}"), input);

  // *** type ***
  check(SV("[(nan+nan i), (-nan-nan i), (inf+inf i), (-inf-inf i)]"), SV("{::r}"), input);
  check(SV("[(nan,nan), (-nan,-nan), (inf,inf), (-inf,-inf)]"), SV("{::S}"), input);
  for (std::basic_string_view<CharT> fmt : fmt_invalid_nested_types<CharT>("rS"))
    check_exception("The format-spec type has a type not supported for a complex type", fmt, input);

  // ***** Only underlying's underlying has a format-spec
  // *** align-fill & width ***
  check(SV("[(     nan  +nan i), (    -nan  -nan i), (     inf  +inf i), (    -inf  -inf i)]"), SV("{:::8}"), input);
  check(SV("[(nan*****+nan i**), (-nan****-nan i**), (inf*****+inf i**), (-inf****-inf i**)]"), SV("{:::*<8}"), input);
  check(SV("[(__nan____+nan i_), (__-nan___-nan i_), (__inf____+inf i_), (__-inf___-inf i_)]"), SV("{:::_^8}"), input);
  check(SV("[(:::::nan::+nan i), (::::-nan::-nan i), (:::::inf::+inf i), (::::-inf::-inf i)]"), SV("{::::>8}"), input);

  check(
      SV("[(     nan  +nan i), (    -nan  -nan i), (     inf  +inf i), (    -inf  -inf i)]"), SV("{:::{}}"), input, 8);
  check(SV("[(nan*****+nan i**), (-nan****-nan i**), (inf*****+inf i**), (-inf****-inf i**)]"),
        SV("{:::*<{}}"),
        input,
        8);
  check(SV("[(__nan____+nan i_), (__-nan___-nan i_), (__inf____+inf i_), (__-inf___-inf i_)]"),
        SV("{:::_^{}}"),
        input,
        8);
  check(SV("[(:::::nan::+nan i), (::::-nan::-nan i), (:::::inf::+inf i), (::::-inf::-inf i)]"),
        SV("{::::>{}}"),
        input,
        8);

  check_exception("The format-spec fill field contains an invalid character", SV("{:::}<}"), input);
  check_exception("The format-spec fill field contains an invalid character", SV("{:::{<}"), input);

  // *** sign ***
  check(SV("[(nan+nan i), (-nan-nan i), (inf+inf i), (-inf-inf i)]"), SV("{:::-}"), input);
  check(SV("[(+nan+nan i), (-nan-nan i), (+inf+inf i), (-inf-inf i)]"), SV("{:::+}"), input);
  check(SV("[( nan nan i), (-nan-nan i), ( inf inf i), (-inf-inf i)]"), SV("{::: }"), input);

  // *** alternate form ***
  check(SV("[(nan+nan i), (-nan-nan i), (inf+inf i), (-inf-inf i)]"), SV("{:::#}"), input);

  // *** zero-padding ***
  // No zeros for INF and NaN
  check(SV("[(     nan  +nan i), (    -nan  -nan i), (     inf  +inf i), (    -inf  -inf i)]"), SV("{:::08}"), input);

  // *** precision ***
  check(SV("[(nan+nan i), (-nan-nan i), (inf+inf i), (-inf-inf i)]"), SV("{:::.10}"), input);
  check(SV("[(nan+nan i), (-nan-nan i), (inf+inf i), (-inf-inf i)]"), SV("{:::.{}}"), input, 10);

  // *** locale-specific form ***
  check(SV("[(nan+nan i), (-nan-nan i), (inf+inf i), (-inf-inf i)]"), SV("{:::L}"), input);

  // *** n ***
  check_exception("The format-spec should consume the input or end with a '}'", SV("{:::n}"), input);

  // *** type ***
  check(SV("[(nan+nan i), (-nan-nan i), (inf+inf i), (-inf-inf i)]"), SV("{:::a}"), input);
  check(SV("[(NAN+NAN i), (-NAN-NAN i), (INF+INF i), (-INF-INF i)]"), SV("{:::A}"), input);
  check(SV("[(nan+nan i), (-nan-nan i), (inf+inf i), (-inf-inf i)]"), SV("{:::e}"), input);
  check(SV("[(NAN+NAN i), (-NAN-NAN i), (INF+INF i), (-INF-INF i)]"), SV("{:::E}"), input);
  check(SV("[(nan+nan i), (-nan-nan i), (inf+inf i), (-inf-inf i)]"), SV("{:::f}"), input);
  check(SV("[(NAN+NAN i), (-NAN-NAN i), (INF+INF i), (-INF-INF i)]"), SV("{:::F}"), input);
  check(SV("[(nan+nan i), (-nan-nan i), (inf+inf i), (-inf-inf i)]"), SV("{:::g}"), input);
  check(SV("[(NAN+NAN i), (-NAN-NAN i), (INF+INF i), (-INF-INF i)]"), SV("{:::G}"), input);
  for (std::basic_string_view<CharT> fmt : detail::fmt_invalid_types<CharT, 3>("aAeEfFgG"))
    check_exception("The format-spec type has a type not supported for a floating-point argument", fmt, input);
}

template <class CharT, class TestFunction, class ExceptionTest>
void run_tests(TestFunction check, ExceptionTest check_exception) {
  test_complex<CharT, float>(check, check_exception);
  test_complex<CharT, double>(check, check_exception);
  test_complex<CharT, long double>(check, check_exception);

  test_complex_non_finite<CharT>(check, check_exception);
}

#endif //TEST_STD_NUMERICS_COMPLEX_NUMBER_COMPLEX_FORMAT_FORMAT_FUNCTIONS_TESTS_H
