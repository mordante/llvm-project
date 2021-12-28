//===----------------------------------------------------------------------===//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17
// UNSUPPORTED: libcpp-has-no-incomplete-format

// <format>

#include <format>
#include <cassert>
#include <type_traits>

#include "test_macros.h"
#include "make_string.h"

#define SV(S) MAKE_STRING_VIEW(CharT, S)

template <class F>
constexpr void check_exception(F&& f) {
  if (std::is_constant_evaluated())
    return;

#ifndef TEST_HAS_NO_EXCEPTIONS
  try {
    f();
    assert(false);
  } catch (std::format_error&) {
    return;
  }
  assert(false);
#else
  (void)f;
#endif
}

// Valiate the values of the default constructed fields and the provided named field selections.
static_assert(std::__format_spec::__fields{}.__sign_ == false &&
              std::__format_spec::__fields{}.__alternate_form_ == false &&
              std::__format_spec::__fields{}.__zero_padding_ == false &&
              std::__format_spec::__fields{}.__precision_ == false &&
              std::__format_spec::__fields{}.__locale_specific_form_ == false);

static_assert(std::__format_spec::__fields_integral.__sign_ == true &&
              std::__format_spec::__fields_integral.__alternate_form_ == true &&
              std::__format_spec::__fields_integral.__zero_padding_ == true &&
              std::__format_spec::__fields_integral.__precision_ == false &&
              std::__format_spec::__fields_integral.__locale_specific_form_ == true);

static_assert(std::__format_spec::__fields_floating_point.__sign_ == true &&
              std::__format_spec::__fields_floating_point.__alternate_form_ == true &&
              std::__format_spec::__fields_floating_point.__zero_padding_ == true &&
              std::__format_spec::__fields_floating_point.__precision_ == true &&
              std::__format_spec::__fields_floating_point.__locale_specific_form_ == true);

static_assert(std::__format_spec::__fields_pointer.__sign_ == false &&
              std::__format_spec::__fields_pointer.__alternate_form_ == false &&
              std::__format_spec::__fields_pointer.__zero_padding_ == false &&
              std::__format_spec::__fields_pointer.__precision_ == false &&
              std::__format_spec::__fields_pointer.__locale_specific_form_ == false);

static_assert(std::__format_spec::__fields_string.__sign_ == false &&
              std::__format_spec::__fields_string.__alternate_form_ == false &&
              std::__format_spec::__fields_string.__zero_padding_ == false &&
              std::__format_spec::__fields_string.__precision_ == true &&
              std::__format_spec::__fields_string.__locale_specific_form_ == false);

inline constexpr std::__format_spec::__fields all_fields{true, true, true, true, true};

// A std::basic_format_parse_context needs a number of arguments.
// This number is large enough for the parser not to fail on a lack of arguments.
constexpr size_t num_args = 100;

template <class CharT>
constexpr void test_align_fill(std::__format_spec::__fields fields) {
  {
    std::__format_spec::__parser<CharT> parser;
    assert(parser.__fill_ == CharT(' '));

    std::basic_format_parse_context parse_ctx{SV(""), num_args};
    parser.__parse(parse_ctx, fields);

    assert(parser.__fill_ == CharT(' '));
  }
  {
    std::__format_spec::__parser<CharT> parser;
    assert(parser.__fill_ == CharT(' '));

    std::basic_format_parse_context parse_ctx{SV("a<"), num_args};
    parser.__parse(parse_ctx, fields);

    assert(parser.__fill_ == CharT('a'));
  }
}

template <class CharT>
constexpr void test_align_fill() {
  test_align_fill<CharT>(std::__format_spec::__fields{});
  test_align_fill<CharT>(all_fields);
}

template <class CharT>
constexpr void test_sign(std::__format_spec::__fields fields) {
  {
    std::__format_spec::__parser<CharT> parser;
    assert(parser.__sign_ == std::__format_spec::__sign::__default);

    std::basic_format_parse_context parse_ctx{SV(""), num_args};
    parser.__parse(parse_ctx, fields);
    assert(parser.__sign_ == std::__format_spec::__sign::__default);
  }
  {
    std::__format_spec::__parser<CharT> parser;
    assert(parser.__sign_ == std::__format_spec::__sign::__default);
    {
      std::basic_format_parse_context parse_ctx{SV("-"), num_args};
      parser.__parse(parse_ctx, fields);
      assert(parser.__sign_ == std::__format_spec::__sign::__minus);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("+"), num_args};
      parser.__parse(parse_ctx, fields);
      assert(parser.__sign_ == std::__format_spec::__sign::__plus);
    }
    {
      std::basic_format_parse_context parse_ctx{SV(" "), num_args};
      parser.__parse(parse_ctx, fields);
      assert(parser.__sign_ == std::__format_spec::__sign::__space);
    }
  }
}

template <class CharT>
constexpr void test_sign() {
  test_sign<CharT>(std::__format_spec::__fields{.__sign_ = true});
  test_sign<CharT>(all_fields);

  {
    std::__format_spec::__parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV(""), num_args};
    parser.__parse(parse_ctx, std::__format_spec::__fields{.__sign_ = false});
  }
  check_exception([] {
    std::__format_spec::__parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV("-"), num_args};
    parser.__parse(parse_ctx, std::__format_spec::__fields{.__sign_ = false});
  });
}

template <class CharT>
constexpr void test_alternate_form(std::__format_spec::__fields fields) {
  {
    std::__format_spec::__parser<CharT> parser;
    assert(parser.__alternate_form_ == false);

    std::basic_format_parse_context parse_ctx{SV(""), num_args};
    parser.__parse(parse_ctx, fields);
    assert(parser.__alternate_form_ == false);
  }
  {
    std::__format_spec::__parser<CharT> parser;

    std::basic_format_parse_context parse_ctx{SV("#"), num_args};
    parser.__parse(parse_ctx, fields);
    assert(parser.__alternate_form_ == true);
  }
}

template <class CharT>
constexpr void test_alternate_form() {
  test_alternate_form<CharT>(std::__format_spec::__fields{.__alternate_form_ = true});
  test_alternate_form<CharT>(all_fields);

  {
    std::__format_spec::__parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV(""), num_args};
    parser.__parse(parse_ctx, std::__format_spec::__fields{.__alternate_form_ = false});
  }
  check_exception([] {
    std::__format_spec::__parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV("#"), num_args};
    parser.__parse(parse_ctx, std::__format_spec::__fields{.__alternate_form_ = false});
  });
}

template <class CharT>
constexpr void test_zero_padding(std::__format_spec::__fields fields) {
  {
    std::__format_spec::__parser<CharT> parser;
    assert(parser.__zero_padding_ == false);

    std::basic_format_parse_context parse_ctx{SV(""), num_args};
    parser.__parse(parse_ctx, fields);
    assert(parser.__zero_padding_ == false);
  }
  {
    std::__format_spec::__parser<CharT> parser;

    std::basic_format_parse_context parse_ctx{SV("0"), num_args};
    parser.__parse(parse_ctx, fields);
    assert(parser.__zero_padding_ == true);
  }
}

template <class CharT>
constexpr void test_zero_padding() {
  test_zero_padding<CharT>(std::__format_spec::__fields{.__zero_padding_ = true});
  test_zero_padding<CharT>(all_fields);

  {
    std::__format_spec::__parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV(""), num_args};
    parser.__parse(parse_ctx, std::__format_spec::__fields{.__zero_padding_ = false});
  }
  check_exception([] {
    std::__format_spec::__parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV("0"), num_args};
    parser.__parse(parse_ctx, std::__format_spec::__fields{.__zero_padding_ = false});
  });
}

template <class CharT>
constexpr void test_width(std::__format_spec::__fields fields) {
  // XXX test limits int32_max
  {
    std::__format_spec::__parser<CharT> parser;
    assert(!parser.__has_width());
    assert(!parser.__width_as_arg_);
    assert(parser.__width_ == 0);
  }
  {
    std::__format_spec::__parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV("1"), num_args};
    parser.__parse(parse_ctx, fields);

    assert(parser.__zero_padding_ == false);
    assert(parser.__has_width());
    assert(!parser.__width_as_arg_);
    assert(parser.__width_ == 1);
  }
  if (fields.__zero_padding_) {
    std::__format_spec::__parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV("09"), num_args};
    parser.__parse(parse_ctx, fields);

    assert(parser.__zero_padding_ == true);
    assert(parser.__has_width());
    assert(!parser.__width_as_arg_);
    assert(parser.__width_ == 9);
  }
  if (fields.__zero_padding_)
    check_exception([fields] {
      std::__format_spec::__parser<CharT> parser;
      std::basic_format_parse_context parse_ctx{SV("00"), num_args};
      parser.__parse(parse_ctx, fields);
    });
  else
    check_exception([fields] {
      std::__format_spec::__parser<CharT> parser;
      std::basic_format_parse_context parse_ctx{SV("0"), num_args};
      parser.__parse(parse_ctx, fields);
    });
  {
    std::__format_spec::__parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV("{}"), num_args};
    parser.__parse(parse_ctx, fields);

    assert(parser.__width_as_arg_);
    assert(parser.__width_ == 0);
  }
  {
    std::__format_spec::__parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV("{42}"), 43};
    parser.__parse(parse_ctx, fields);

    assert(parser.__width_as_arg_);
    assert(parser.__width_ == 42);
  }
}

template <class CharT>
constexpr void test_width() {
  test_width<CharT>(std::__format_spec::__fields{});
  test_width<CharT>(all_fields);
}

template <class CharT>
constexpr void test_precision(std::__format_spec::__fields fields) {
  // XXX test limits int32_max
  {
    std::__format_spec::__parser<CharT> parser;
    assert(!parser.__has_precision());
    assert(!parser.__precision_as_arg_);
    LIBCPP_ASSERT(parser.__precision_ == -1 /*std::__format::__number_max*/);
  }
  {
    std::__format_spec::__parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV(".0"), num_args};
    parser.__parse(parse_ctx, fields);

    assert(parser.__has_precision());
    assert(!parser.__precision_as_arg_);
    assert(parser.__precision_ == 0);
  }
  {
    std::__format_spec::__parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV(".{}"), num_args};
    parser.__parse(parse_ctx, fields);

    assert(parser.__precision_as_arg_);
    assert(parser.__precision_ == 0);
    /*
    parser.__set_precision(42);
    assert(parser.__has_precision());
    assert(!parser.__precision_as_arg_);
    assert(parser.__precision_ == 42);*/
  }
  {
    std::__format_spec::__parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV("{}.{}"), num_args};
    parser.__parse(parse_ctx, fields);

    assert(parser.__width_as_arg_);
    assert(parser.__width_ == 0);

    assert(parser.__precision_as_arg_);
    assert(parser.__precision_ == 1);
  }
  {
    std::__format_spec::__parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV(".{42}"), num_args};
    parser.__parse(parse_ctx, fields);

    assert(parser.__precision_as_arg_);
    assert(parser.__precision_ == 42);
  }
}

template <class CharT>
constexpr void test_precision() {
  test_precision<CharT>(std::__format_spec::__fields{.__precision_ = true});
  test_precision<CharT>(all_fields);

  {
    std::__format_spec::__parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV(""), num_args};
    parser.__parse(parse_ctx, std::__format_spec::__fields{.__precision_ = false});
  }
  check_exception([] {
    std::__format_spec::__parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV(".1"), num_args};
    parser.__parse(parse_ctx, std::__format_spec::__fields{.__precision_ = false});
  });
}

template <class CharT>
constexpr void test_locale_specific_form(std::__format_spec::__fields fields) {
  {
    std::__format_spec::__parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV(""), num_args};
    parser.__parse(parse_ctx, fields);

    assert(parser.__locale_specific_form_ == false);
  }
  {
    std::__format_spec::__parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV("L"), num_args};
    parser.__parse(parse_ctx, fields);

    assert(parser.__locale_specific_form_ == true);
  }
}

template <class CharT>
constexpr void test_locale_specific_form() {
  test_locale_specific_form<CharT>(std::__format_spec::__fields{.__locale_specific_form_ = true});
  test_locale_specific_form<CharT>(all_fields);

  {
    std::__format_spec::__parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV(""), num_args};
    parser.__parse(parse_ctx, std::__format_spec::__fields{.__locale_specific_form_ = false});
  }
  check_exception([] {
    std::__format_spec::__parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV("L"), num_args};
    parser.__parse(parse_ctx, std::__format_spec::__fields{.__locale_specific_form_ = false});
  });
}

template <class CharT>
constexpr void test_type(std::__format_spec::__fields fields) {
  {
    std::__format_spec::__parser<CharT> parser;
    assert(parser.__type_ == std::__format_spec::__type::__default);

    std::basic_format_parse_context parse_ctx{SV(""), num_args};
    parser.__parse(parse_ctx, fields);
    assert(parser.__type_ == std::__format_spec::__type::__default);
  }
  {
    std::__format_spec::__parser<CharT> parser;
    assert(parser.__type_ == std::__format_spec::__type::__default);
    {
      std::basic_format_parse_context parse_ctx{SV("a"), num_args};
      parser.__parse(parse_ctx, fields);
      assert(parser.__type_ == std::__format_spec::__type::__float_hexadecimal_lower_case);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("A"), num_args};
      parser.__parse(parse_ctx, fields);
      assert(parser.__type_ == std::__format_spec::__type::__float_hexadecimal_upper_case);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("b"), num_args};
      parser.__parse(parse_ctx, fields);
      assert(parser.__type_ == std::__format_spec::__type::__binary_lower_case);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("B"), num_args};
      parser.__parse(parse_ctx, fields);
      assert(parser.__type_ == std::__format_spec::__type::__binary_upper_case);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("c"), num_args};
      parser.__parse(parse_ctx, fields);
      assert(parser.__type_ == std::__format_spec::__type::__char);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("d"), num_args};
      parser.__parse(parse_ctx, fields);
      assert(parser.__type_ == std::__format_spec::__type::__decimal);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("e"), num_args};
      parser.__parse(parse_ctx, fields);
      assert(parser.__type_ == std::__format_spec::__type::__scientific_lower_case);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("E"), num_args};
      parser.__parse(parse_ctx, fields);
      assert(parser.__type_ == std::__format_spec::__type::__scientific_upper_case);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("f"), num_args};
      parser.__parse(parse_ctx, fields);
      assert(parser.__type_ == std::__format_spec::__type::__fixed_lower_case);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("F"), num_args};
      parser.__parse(parse_ctx, fields);
      assert(parser.__type_ == std::__format_spec::__type::__fixed_upper_case);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("g"), num_args};
      parser.__parse(parse_ctx, fields);
      assert(parser.__type_ == std::__format_spec::__type::__general_lower_case);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("G"), num_args};
      parser.__parse(parse_ctx, fields);
      assert(parser.__type_ == std::__format_spec::__type::__general_upper_case);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("o"), num_args};
      parser.__parse(parse_ctx, fields);
      assert(parser.__type_ == std::__format_spec::__type::__octal);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("p"), num_args};
      parser.__parse(parse_ctx, fields);
      assert(parser.__type_ == std::__format_spec::__type::__pointer);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("s"), num_args};
      parser.__parse(parse_ctx, fields);
      assert(parser.__type_ == std::__format_spec::__type::__string);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("x"), num_args};
      parser.__parse(parse_ctx, fields);
      assert(parser.__type_ == std::__format_spec::__type::__hexadecimal_lower_case);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("X"), num_args};
      parser.__parse(parse_ctx, fields);
      assert(parser.__type_ == std::__format_spec::__type::__hexadecimal_upper_case);
    }
  }
}

template <class CharT>
constexpr void test_type() {
  test_type<CharT>(std::__format_spec::__fields{});
  test_type<CharT>(all_fields);
}

template <class CharT>
constexpr void test() {

  test_align_fill<CharT>();
  test_sign<CharT>();
  test_alternate_form<CharT>();
  test_zero_padding<CharT>();

  test_width<CharT>();
  test_precision<CharT>();
  test_locale_specific_form<CharT>();
  test_type<CharT>();

  // The parser should fit in 128-bit allowing it to be passed in the registers on 64-bit platforms.
  static_assert(sizeof(std::__format_spec::__parser<CharT>) == 2 * sizeof(int64_t));
  static_assert(std::is_trivially_copyable_v<std::__format_spec::__parser<CharT>>);
}

constexpr bool test() {
  test<char>();
#ifndef TEST_HAS_NO_WIDE_CHARACTERS
  test<wchar_t>();
#endif

  return true;
}

int main(int, char**) {
  test();
  static_assert(test());

  return 0;
}
