//===----------------------------------------------------------------------===//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef TEST_STD_UTILITIES_TEMPLATE_BITSET_BITSET_BITSET_FORMAT_FORMAT_FUNCTIONS_TESTS_H
#define TEST_STD_UTILITIES_TEMPLATE_BITSET_BITSET_BITSET_FORMAT_FORMAT_FUNCTIONS_TESTS_H

#include <concepts>
#include <array>
#include <format>
#include <bitset>
#include <stdexcept>

#include "format.functions.common.h"

#define CHECK_FORMAT_EXCEPTION(...) check_exception.template operator()<std::format_error>(__VA_ARGS__)
#define CHECK_OVERFLOW_EXCEPTION(...) check_exception.template operator()<std::overflow_error>(__VA_ARGS__)

template <class CharT, class TestFunction, class ExceptionTest>
void test_bitset_0(TestFunction check, ExceptionTest check_exception) {
  std::bitset<0> input;
  check(SV(""), SV("{}"), input);

  // ***** underlying has no format-spec

  // *** align-fill & width ***
  check(SV("     "), SV("{:5}"), input);
  check(SV("*****"), SV("{:*<5}"), input);
  check(SV("_____"), SV("{:_^5}"), input);
  check(SV("#####"), SV("{:#>5}"), input);

  check(SV("     "), SV("{:{}}"), input, 5);
  check(SV("*****"), SV("{:*<{}}"), input, 5);
  check(SV("_____"), SV("{:_^{}}"), input, 5);
  check(SV("#####"), SV("{:#>{}}"), input, 5);

  CHECK_FORMAT_EXCEPTION("The format-spec range-fill field contains an invalid character", SV("{:}<}"), input);
  CHECK_FORMAT_EXCEPTION("The format-spec range-fill field contains an invalid character", SV("{:{<}"), input);
  CHECK_FORMAT_EXCEPTION(
      "The format-spec range-fill field contains an invalid character", SV("{::<}"), input); // range-fill not allowed

  // *** sign ***
  CHECK_FORMAT_EXCEPTION("A sign field isn't allowed in this format-spec", SV("{:-}"), input);
  CHECK_FORMAT_EXCEPTION("A sign field isn't allowed in this format-spec", SV("{:+}"), input);
  CHECK_FORMAT_EXCEPTION("A sign field isn't allowed in this format-spec", SV("{: }"), input);

  // *** alternate form ***
  CHECK_FORMAT_EXCEPTION("An alternate form field isn't allowed in this format-spec", SV("{:#}"), input);

  // *** zero-padding ***
  CHECK_FORMAT_EXCEPTION("A zero-padding field isn't allowed in this format-spec", SV("{:0}"), input);

  // *** precision ***
  CHECK_FORMAT_EXCEPTION("The format-spec should consume the input or end with a '}'", SV("{:.}"), input);

  // *** locale-specific form ***
  CHECK_FORMAT_EXCEPTION("A locale-specific form field isn't allowed in this format-spec", SV("{:L}"), input);

  // *** n ***
  CHECK_FORMAT_EXCEPTION("The n field is only allowed for the range type", SV("{:n}"), input);
  CHECK_FORMAT_EXCEPTION("The n field is only allowed for the range type", SV("{:nb}"), input);
  CHECK_FORMAT_EXCEPTION("The n field is only allowed for the range type", SV("{:nB}"), input);
  CHECK_FORMAT_EXCEPTION("The n field is only allowed for the range type", SV("{:nd}"), input);
  CHECK_FORMAT_EXCEPTION("The n field is only allowed for the range type", SV("{:no}"), input);
  CHECK_FORMAT_EXCEPTION("The n field is only allowed for the range type", SV("{:nx}"), input);
  CHECK_FORMAT_EXCEPTION("The n field is only allowed for the range type", SV("{:nX}"), input);
  check(SV(""), SV("{:nr}"), input);
  CHECK_FORMAT_EXCEPTION("The n field is only allowed for the range type", SV("{:ns}"), input);

  // *** type ***
  check(SV("0"), SV("{:b}"), input);
  check(SV("0"), SV("{:B}"), input);
  check(SV("0"), SV("{:o}"), input);
  check(SV("0"), SV("{:d}"), input);
  check(SV("0"), SV("{:x}"), input);
  check(SV("0"), SV("{:X}"), input);
  check(SV("[]"), SV("{:r}"), input);
  check(SV(""), SV("{:s}"), input);
  for (std::basic_string_view<CharT> fmt : fmt_invalid_types<CharT>("bBodxXsr")) // don't allow debug, looks silly here.
    CHECK_FORMAT_EXCEPTION("The format-spec type has a type not supported for a bitset argument", fmt, input);
}

template <class CharT, class TestFunction, class ExceptionTest>
void test_bitset_1(TestFunction check, ExceptionTest check_exception) {
  std::bitset<1> input{0};
  check(SV("0"), SV("{}"), input);

  // *** align-fill & width ***
  check(SV("0     "), SV("{:6}"), input);
  check(SV("0*****"), SV("{:*<6}"), input);
  check(SV("__0___"), SV("{:_^6}"), input);
  check(SV("#####0"), SV("{:#>6}"), input);

  check(SV("0     "), SV("{:{}}"), input, 6);
  check(SV("0*****"), SV("{:*<{}}"), input, 6);
  check(SV("__0___"), SV("{:_^{}}"), input, 6);
  check(SV("#####0"), SV("{:#>{}}"), input, 6);

  CHECK_FORMAT_EXCEPTION("The format-spec range-fill field contains an invalid character", SV("{:}<}"), input);
  CHECK_FORMAT_EXCEPTION("The format-spec range-fill field contains an invalid character", SV("{:{<}"), input);
  CHECK_FORMAT_EXCEPTION(
      "The format-spec range-fill field contains an invalid character", SV("{::<}"), input); // range-fill not allowed

  // *** sign ***
  CHECK_FORMAT_EXCEPTION("A sign field isn't allowed in this format-spec", SV("{:-}"), input);
  CHECK_FORMAT_EXCEPTION("A sign field isn't allowed in this format-spec", SV("{:+}"), input);
  CHECK_FORMAT_EXCEPTION("A sign field isn't allowed in this format-spec", SV("{: }"), input);

  // *** alternate form ***
  CHECK_FORMAT_EXCEPTION("An alternate form field isn't allowed in this format-spec", SV("{:#}"), input);

  // *** zero-padding ***
  CHECK_FORMAT_EXCEPTION("A zero-padding field isn't allowed in this format-spec", SV("{:0}"), input);

  // *** precision ***
  CHECK_FORMAT_EXCEPTION("The format-spec should consume the input or end with a '}'", SV("{:.}"), input);

  // *** locale-specific form ***
  CHECK_FORMAT_EXCEPTION("A locale-specific form field isn't allowed in this format-spec", SV("{:L}"), input);

  // *** n ***
  CHECK_FORMAT_EXCEPTION("The n field is only allowed for the range type", SV("{:n}"), input);
  CHECK_FORMAT_EXCEPTION("The n field is only allowed for the range type", SV("{:nb}"), input);
  CHECK_FORMAT_EXCEPTION("The n field is only allowed for the range type", SV("{:nB}"), input);
  CHECK_FORMAT_EXCEPTION("The n field is only allowed for the range type", SV("{:nd}"), input);
  CHECK_FORMAT_EXCEPTION("The n field is only allowed for the range type", SV("{:no}"), input);
  CHECK_FORMAT_EXCEPTION("The n field is only allowed for the range type", SV("{:nx}"), input);
  CHECK_FORMAT_EXCEPTION("The n field is only allowed for the range type", SV("{:nX}"), input);
  check(SV("false"), SV("{:nr}"), input);
  CHECK_FORMAT_EXCEPTION("The n field is only allowed for the range type", SV("{:ns}"), input);

  // *** type ***
  check(SV("0"), SV("{:b}"), input);
  check(SV("0"), SV("{:B}"), input);
  check(SV("0"), SV("{:o}"), input);
  check(SV("0"), SV("{:d}"), input);
  check(SV("0"), SV("{:x}"), input);
  check(SV("0"), SV("{:s}"), input);
  check(SV("0"), SV("{:X}"), input);
  check(SV("[false]"), SV("{:r}"), input);
  for (std::basic_string_view<CharT> fmt : fmt_invalid_types<CharT>("bBodxXsr")) // don't allow debug, looks silly here.
    CHECK_FORMAT_EXCEPTION("The format-spec type has a type not supported for a bitset argument", fmt, input);
}

template <class CharT, class TestFunction>
void test_bitset_8(TestFunction check) {
  std::bitset<8> input{0x5a};
  check(SV("01011010"), SV("{}"), input);
  check(SV("01011010"), SV("{:s}"), input);

  check(SV("0b1011010"), SV("{:#b}"), input);
  check(SV("0B1011010"), SV("{:#B}"), input);
  check(SV("0132"), SV("{:#o}"), input);
  check(SV("90"), SV("{:d}"), input);
  check(SV("0x5a"), SV("{:#x}"), input);
  check(SV("0X5A"), SV("{:#X}"), input);

  check(SV("[false, true, false, true, true, false, true, false]"), SV("{:r}"), input);
  check(SV("[false, true, false, true, true, false, true, false]"), SV("{:r:s}"), input);
  check(SV("[0, 1, 0, 1, 1, 0, 1, 0]"), SV("{:r:b}"), input);
  check(SV("[0b0, 0b1, 0b0, 0b1, 0b1, 0b0, 0b1, 0b0]"), SV("{:r:#b}"), input);
  check(SV("[0B0, 0B1, 0B0, 0B1, 0B1, 0B0, 0B1, 0B0]"), SV("{:r:#B}"), input);
  check(SV("[0, 01, 0, 01, 01, 0, 01, 0]"), SV("{:r:#o}"), input);
  check(SV("[0, 1, 0, 1, 1, 0, 1, 0]"), SV("{:r:d}"), input);
  check(SV("[0x0, 0x1, 0x0, 0x1, 0x1, 0x0, 0x1, 0x0]"), SV("{:r:#x}"), input);
  check(SV("[0X0, 0X1, 0X0, 0X1, 0X1, 0X0, 0X1, 0X0]"), SV("{:r:#X}"), input);

  check(SV("false, true, false, true, true, false, true, false"), SV("{:nr}"), input);
  check(SV("false, true, false, true, true, false, true, false"), SV("{:nr:s}"), input);
  check(SV("0, 1, 0, 1, 1, 0, 1, 0"), SV("{:nr:b}"), input);
  check(SV("0b0, 0b1, 0b0, 0b1, 0b1, 0b0, 0b1, 0b0"), SV("{:nr:#b}"), input);
  check(SV("0B0, 0B1, 0B0, 0B1, 0B1, 0B0, 0B1, 0B0"), SV("{:nr:#B}"), input);
  check(SV("0, 01, 0, 01, 01, 0, 01, 0"), SV("{:nr:#o}"), input);
  check(SV("0, 1, 0, 1, 1, 0, 1, 0"), SV("{:nr:d}"), input);
  check(SV("0x0, 0x1, 0x0, 0x1, 0x1, 0x0, 0x1, 0x0"), SV("{:nr:#x}"), input);
  check(SV("0X0, 0X1, 0X0, 0X1, 0X1, 0X0, 0X1, 0X0"), SV("{:nr:#X}"), input);
}

template <class CharT, class TestFunction>
void test_bitset_256_zero(TestFunction check) {
  std::bitset<256> input{
      "0000000000000000000000000000000000000000000000000000000000000000"
      "0000000000000000000000000000000000000000000000000000000000000000"
      "0000000000000000000000000000000000000000000000000000000000000000"
      "0000000000000000000000000000000000000000000000000000000000000000"};

  check(SV("0000000000000000000000000000000000000000000000000000000000000000"
           "0000000000000000000000000000000000000000000000000000000000000000"
           "0000000000000000000000000000000000000000000000000000000000000000"
           "0000000000000000000000000000000000000000000000000000000000000000"),
        SV("{}"),
        input);

  check(SV("0"), SV("{:b}"), input);
  check(SV("0"), SV("{:B}"), input);
  check(SV("0"), SV("{:o}"), input);
  check(SV("0"), SV("{:d}"), input);
  check(SV("0"), SV("{:x}"), input);
  check(SV("0"), SV("{:X}"), input);
  check(SV("0000000000000000000000000000000000000000000000000000000000000000"
           "0000000000000000000000000000000000000000000000000000000000000000"
           "0000000000000000000000000000000000000000000000000000000000000000"
           "0000000000000000000000000000000000000000000000000000000000000000"),
        SV("{:s}"),
        input);
  check(SV("["
           // clang-format off
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false]"),
        // clang-format on
        SV("{:r}"),
        input);
}

template <class CharT, class TestFunction>
void test_bitset_256_input_00_00_00_0a(TestFunction check) {
  static_assert(sizeof(unsigned long long) >= 4);
  std::bitset<256> input{
      "0000000000000000000000000000000000000000000000000000000000000000"
      "0000000000000000000000000000000000000000000000000000000000000000"
      "0000000000000000000000000000000000000000000000000000000000000000"
      "0000000000000000000000000000000010101010101010101010101010101010"};

  check(SV("0000000000000000000000000000000000000000000000000000000000000000"
           "0000000000000000000000000000000000000000000000000000000000000000"
           "0000000000000000000000000000000000000000000000000000000000000000"
           "0000000000000000000000000000000010101010101010101010101010101010"),
        SV("{}"),
        input);

  check(SV("10101010101010101010101010101010"), SV("{:b}"), input);
  check(SV("10101010101010101010101010101010"), SV("{:B}"), input);
  check(SV("25252525252"), SV("{:o}"), input);
  check(SV("2863311530"), SV("{:d}"), input);
  check(SV("aaaaaaaa"), SV("{:x}"), input);
  check(SV("AAAAAAAA"), SV("{:X}"), input);
  check(SV("0000000000000000000000000000000000000000000000000000000000000000"
           "0000000000000000000000000000000000000000000000000000000000000000"
           "0000000000000000000000000000000000000000000000000000000000000000"
           "0000000000000000000000000000000010101010101010101010101010101010"),
        SV("{:s}"),
        input);

  check(SV("["
           // clang-format off
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"true , false, true , false, true , false, true , false, true , false, true , false, true , false, true , false, "
"true , false, true , false, true , false, true , false, true , false, true , false, true , false, true , false]"
),
        // clang-format on
        SV("{:r:5}"),
        input);
}

template <class CharT, class TestFunction>
void test_bitset_256_input_00_00_00_aa(TestFunction check) {
  // Whether values that don't fit in a unsigned long long are implemented is
  // implementation defined.
#ifdef _LIBCPP_VERSION
  constexpr bool test_bool = true;
#else
  constexpr bool test_bool = sizeof(unsigned long long) >= 8;
#endif
  constexpr bool test_non_bool = sizeof(unsigned long long) >= 8;

  std::bitset<256> input{
      "0000000000000000000000000000000000000000000000000000000000000000"
      "0000000000000000000000000000000000000000000000000000000000000000"
      "0000000000000000000000000000000000000000000000000000000000000000"
      "1010101010101010101010101010101010101010101010101010101010101010"};

  check(SV("0000000000000000000000000000000000000000000000000000000000000000"
           "0000000000000000000000000000000000000000000000000000000000000000"
           "0000000000000000000000000000000000000000000000000000000000000000"
           "1010101010101010101010101010101010101010101010101010101010101010"),
        SV("{}"),
        input);

  if constexpr (test_bool) {
    check(SV("1010101010101010101010101010101010101010101010101010101010101010"), SV("{:b}"), input);
    check(SV("1010101010101010101010101010101010101010101010101010101010101010"), SV("{:B}"), input);
  }
  if constexpr (test_non_bool) {
    check(SV("1252525252525252525252"), SV("{:o}"), input);
    check(SV("12297829382473034410"), SV("{:d}"), input);
    check(SV("aaaaaaaaaaaaaaaa"), SV("{:x}"), input);
    check(SV("AAAAAAAAAAAAAAAA"), SV("{:X}"), input);
  }
  check(SV("0000000000000000000000000000000000000000000000000000000000000000"
           "0000000000000000000000000000000000000000000000000000000000000000"
           "0000000000000000000000000000000000000000000000000000000000000000"
           "1010101010101010101010101010101010101010101010101010101010101010"),
        SV("{:s}"),
        input);

  check(SV("["
           // clang-format off
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"true , false, true , false, true , false, true , false, true , false, true , false, true , false, true , false, "
"true , false, true , false, true , false, true , false, true , false, true , false, true , false, true , false, "
"true , false, true , false, true , false, true , false, true , false, true , false, true , false, true , false, "
"true , false, true , false, true , false, true , false, true , false, true , false, true , false, true , false]"
),
        // clang-format on
        SV("{:r:5}"),
        input);
}

template <class CharT, class TestFunction, class ExceptionTest>
void test_bitset_256_input_00_00_aa_77(TestFunction check, [[maybe_unused]] ExceptionTest check_exception) {
  // Whether values that don't fit in a unsigned long long are implemented is
  // implementation defined.
#ifdef _LIBCPP_VERSION
  constexpr bool test_bool = true;
#  ifdef TEST_HAS_NO_EXCEPTIONS
  constexpr bool test_non_bool_throw = false;
#  else
  constexpr bool test_non_bool_throw = true;
#  endif
#else
  constexpr bool test_bool           = sizeof(unsigned long long) >= 16;
  constexpr bool test_non_bool_throw = false;
#endif

  std::bitset<256> input{
      "0000000000000000000000000000000000000000000000000000000000000000"
      "0000000000000000000000000000000000000000000000000000000000000000"
      "1010101010101010101010101010101010101010101010101010101010101010"
      "0111011101110111011101110111011101110111011101110111011101110111"};

  check(SV("0000000000000000000000000000000000000000000000000000000000000000"
           "0000000000000000000000000000000000000000000000000000000000000000"
           "1010101010101010101010101010101010101010101010101010101010101010"
           "0111011101110111011101110111011101110111011101110111011101110111"),
        SV("{}"),
        input);

  if constexpr (test_bool) {
    check(SV("1010101010101010101010101010101010101010101010101010101010101010"
             "0111011101110111011101110111011101110111011101110111011101110111"),
          SV("{:b}"),
          input);
    check(SV("1010101010101010101010101010101010101010101010101010101010101010"
             "0111011101110111011101110111011101110111011101110111011101110111"),
          SV("{:B}"),
          input);
  }
  if constexpr (test_non_bool_throw) {
    CHECK_OVERFLOW_EXCEPTION("bitset octal converion overflow error", SV("{:o}"), input);
    CHECK_OVERFLOW_EXCEPTION("bitset decimal converion overflow error", SV("{:d}"), input);
    CHECK_OVERFLOW_EXCEPTION("bitset hexadecimal converion overflow error", SV("{:x}"), input);
    CHECK_OVERFLOW_EXCEPTION("bitset hexadecimal converion overflow error", SV("{:X}"), input);
  }
  check(SV("0000000000000000000000000000000000000000000000000000000000000000"
           "0000000000000000000000000000000000000000000000000000000000000000"
           "1010101010101010101010101010101010101010101010101010101010101010"
           "0111011101110111011101110111011101110111011101110111011101110111"),
        SV("{:s}"),
        input);

  check(SV("["
           // clang-format off
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, "
"true , false, true , false, true , false, true , false, true , false, true , false, true , false, true , false, "
"true , false, true , false, true , false, true , false, true , false, true , false, true , false, true , false, "
"true , false, true , false, true , false, true , false, true , false, true , false, true , false, true , false, "
"true , false, true , false, true , false, true , false, true , false, true , false, true , false, true , false, "
"false, true , true , true , false, true , true , true , false, true , true , true , false, true , true , true , "
"false, true , true , true , false, true , true , true , false, true , true , true , false, true , true , true , "
"false, true , true , true , false, true , true , true , false, true , true , true , false, true , true , true , "
"false, true , true , true , false, true , true , true , false, true , true , true , false, true , true , true ]"),
        // clang-format on
        SV("{:r:5}"),
        input);
}

template <class CharT, class TestFunction, class ExceptionTest>
void test_bitset_256_input_ff_aa_77_11(TestFunction check, [[maybe_unused]] ExceptionTest check_exception) {
  // Whether values that don't fit in a unsigned long long are implemented is
  // implementation defined.
#ifdef _LIBCPP_VERSION
  constexpr bool test_bool = true;
#  ifdef TEST_HAS_NO_EXCEPTIONS
  constexpr bool test_non_bool_throw = false;
#  else
  constexpr bool test_non_bool_throw = true;
#  endif
#else
  constexpr bool test_bool           = sizeof(unsigned long long) >= 16;
  constexpr bool test_non_bool_throw = false;
#endif

  std::bitset<256> input{
      "1111111111111111111111111111111111111111111111111111111111111111"
      "1010101010101010101010101010101010101010101010101010101010101010"
      "0111011101110111011101110111011101110111011101110111011101110111"
      "0001000100010001000100010001000100010001000100010001000100010001"};

  check(SV(

            "1111111111111111111111111111111111111111111111111111111111111111"
            "1010101010101010101010101010101010101010101010101010101010101010"
            "0111011101110111011101110111011101110111011101110111011101110111"
            "0001000100010001000100010001000100010001000100010001000100010001"

            ),
        SV("{}"),
        input);
  if constexpr (test_bool) {
    check(SV("1111111111111111111111111111111111111111111111111111111111111111"
             "1010101010101010101010101010101010101010101010101010101010101010"
             "0111011101110111011101110111011101110111011101110111011101110111"
             "0001000100010001000100010001000100010001000100010001000100010001"),
          SV("{:b}"),
          input);
    check(SV("1111111111111111111111111111111111111111111111111111111111111111"
             "1010101010101010101010101010101010101010101010101010101010101010"
             "0111011101110111011101110111011101110111011101110111011101110111"
             "0001000100010001000100010001000100010001000100010001000100010001"),
          SV("{:B}"),
          input);
  }
  if constexpr (test_non_bool_throw) {
    CHECK_OVERFLOW_EXCEPTION("bitset octal converion overflow error", SV("{:o}"), input);
    CHECK_OVERFLOW_EXCEPTION("bitset decimal converion overflow error", SV("{:d}"), input);
    CHECK_OVERFLOW_EXCEPTION("bitset hexadecimal converion overflow error", SV("{:x}"), input);
    CHECK_OVERFLOW_EXCEPTION("bitset hexadecimal converion overflow error", SV("{:X}"), input);
  }
  check(SV(

            "1111111111111111111111111111111111111111111111111111111111111111"
            "1010101010101010101010101010101010101010101010101010101010101010"
            "0111011101110111011101110111011101110111011101110111011101110111"
            "0001000100010001000100010001000100010001000100010001000100010001"),
        SV("{:s}"),
        input);

  check(SV("["
           // clang-format off
"true , true , true , true , true , true , true , true , true , true , true , true , true , true , true , true , "
"true , true , true , true , true , true , true , true , true , true , true , true , true , true , true , true , "
"true , true , true , true , true , true , true , true , true , true , true , true , true , true , true , true , "
"true , true , true , true , true , true , true , true , true , true , true , true , true , true , true , true , "
"true , false, true , false, true , false, true , false, true , false, true , false, true , false, true , false, "
"true , false, true , false, true , false, true , false, true , false, true , false, true , false, true , false, "
"true , false, true , false, true , false, true , false, true , false, true , false, true , false, true , false, "
"true , false, true , false, true , false, true , false, true , false, true , false, true , false, true , false, "
"false, true , true , true , false, true , true , true , false, true , true , true , false, true , true , true , "
"false, true , true , true , false, true , true , true , false, true , true , true , false, true , true , true , "
"false, true , true , true , false, true , true , true , false, true , true , true , false, true , true , true , "
"false, true , true , true , false, true , true , true , false, true , true , true , false, true , true , true , "
"false, false, false, true , false, false, false, true , false, false, false, true , false, false, false, true , "
"false, false, false, true , false, false, false, true , false, false, false, true , false, false, false, true , "
"false, false, false, true , false, false, false, true , false, false, false, true , false, false, false, true , "
"false, false, false, true , false, false, false, true , false, false, false, true , false, false, false, true ]"),
        // clang-format on
        SV("{:r:5}"),
        input);
}

template <class CharT, class TestFunction, class ExceptionTest>
void test_bitset_256(TestFunction check, ExceptionTest check_exception) {
  test_bitset_256_zero<CharT>(check);
  test_bitset_256_input_00_00_00_0a<CharT>(check);                  // 32 bits used
  test_bitset_256_input_00_00_00_aa<CharT>(check);                  // 64 bits used
  test_bitset_256_input_00_00_aa_77<CharT>(check, check_exception); // 128 bits used
  test_bitset_256_input_ff_aa_77_11<CharT>(check, check_exception); // 256 bits used
}

template <class CharT, class TestFunction, class ExceptionTest>
void run_tests(TestFunction check, ExceptionTest check_exception) {
  test_bitset_0<CharT>(check, check_exception);
  test_bitset_1<CharT>(check, check_exception);
  test_bitset_8<CharT>(check);
  test_bitset_256<CharT>(check, check_exception);
}

#endif // TEST_STD_UTILITIES_TEMPLATE_BITSET_BITSET_BITSET_FORMAT_FORMAT_FUNCTIONS_TESTS_H
