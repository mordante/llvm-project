//===----------------------------------------------------------------------===//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17
// UNSUPPORTED: libcpp-no-concepts
// UNSUPPORTED: libcpp-has-no-incomplete-format

// <format>

#include <format>
#include <cassert>

#include "test_macros.h"
#include "make_string.h"

#define SV(S) MAKE_STRING_VIEW(CharT, S)

template <class F>
constexpr void check_exception(F&& f) {
#ifndef TEST_HAS_NO_EXCEPTIONS
  if (std::is_constant_evaluated())
    return;

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

template <class CharT>
constexpr void test_align_fill() {
  {
    std::format_spec::parser<CharT> parser;
    assert(parser.get_fill() == CharT(' '));

    std::basic_format_parse_context parse_ctx{SV(""), 100};
    parser.parse(parse_ctx);

    assert(parser.get_fill() == CharT(' '));
  }
  {
    std::format_spec::parser<CharT> parser;
    assert(parser.get_fill() == CharT(' '));

    std::basic_format_parse_context parse_ctx{SV("a<"), 100};
    parser.parse(parse_ctx);

    assert(parser.get_fill() == CharT('a'));
  }
}

template <class CharT>
constexpr void test_sign() {
  {
    std::format_spec::parser<CharT> parser;
    assert(parser.get_sign() == std::__format_spec::_Flags::_Sign::__default);

    std::basic_format_parse_context parse_ctx{SV(""), 100};
    parser.parse(parse_ctx);
    assert(parser.get_sign() == std::__format_spec::_Flags::_Sign::__default);
  }
  {
    std::format_spec::parser<CharT> parser;
    assert(parser.get_sign() == std::__format_spec::_Flags::_Sign::__default);
    {
      std::basic_format_parse_context parse_ctx{SV("-"), 100};
      parser.parse(parse_ctx);
      assert(parser.get_sign() == std::__format_spec::_Flags::_Sign::__minus);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("+"), 100};
      parser.parse(parse_ctx);
      assert(parser.get_sign() == std::__format_spec::_Flags::_Sign::__plus);
    }
    {
      std::basic_format_parse_context parse_ctx{SV(" "), 100};
      parser.parse(parse_ctx);
      assert(parser.get_sign() == std::__format_spec::_Flags::_Sign::__space);
    }
    parser.set_sign(std::__format_spec::_Flags::_Sign::__minus);
    assert(parser.get_sign() == std::__format_spec::_Flags::_Sign::__minus);
  }
  {
    std::format_spec::parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV(""), 100};
    parser.parse(parse_ctx, std::format_spec::options{.sign = false});
  }
  check_exception([] {
    std::format_spec::parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV("-"), 100};
    parser.parse(parse_ctx, std::format_spec::options{.sign = false});
  });
}

template <class CharT>
constexpr void test_alternate_form() {
  {
    std::format_spec::parser<CharT> parser;
    assert(parser.get_alternate_form() == false);

    std::basic_format_parse_context parse_ctx{SV(""), 100};
    parser.parse(parse_ctx);
    assert(parser.get_alternate_form() == false);
  }
  {
    std::format_spec::parser<CharT> parser;

    std::basic_format_parse_context parse_ctx{SV("#"), 100};
    parser.parse(parse_ctx);
    assert(parser.get_alternate_form() == true);

    parser.set_alternate_form(false);
    assert(parser.get_alternate_form() == false);
  }
  {
    std::format_spec::parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV(""), 100};
    parser.parse(parse_ctx, std::format_spec::options{.alternate_form = false});
  }
  check_exception([] {
    std::format_spec::parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV("#"), 100};
    parser.parse(parse_ctx, std::format_spec::options{.alternate_form = false});
  });
}

template <class CharT>
constexpr void test_zero_padding() {
  {
    std::format_spec::parser<CharT> parser;
    assert(parser.get_zero_padding() == false);

    std::basic_format_parse_context parse_ctx{SV(""), 100};
    parser.parse(parse_ctx);
    assert(parser.get_zero_padding() == false);
  }
  {
    std::format_spec::parser<CharT> parser;

    std::basic_format_parse_context parse_ctx{SV("0"), 100};
    parser.parse(parse_ctx);
    assert(parser.get_zero_padding() == true);

    parser.set_zero_padding(false);
    assert(parser.get_zero_padding() == false);
  }
  {
    std::format_spec::parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV(""), 100};
    parser.parse(parse_ctx, std::format_spec::options{.zero_padding = false});
  }
  check_exception([] {
    std::format_spec::parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV("0"), 100};
    parser.parse(parse_ctx, std::format_spec::options{.zero_padding = false});
  });
}

template <class CharT>
constexpr void test_width() {
  {
    std::format_spec::parser<CharT> parser;
    assert(!parser.has_width());
    assert(!parser.has_dynamic_width());
    LIBCPP_ASSERT(parser.get_width() == 0);
  }
  {
    std::format_spec::parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV("1"), 100};
    parser.parse(parse_ctx);

    assert(parser.has_width());
    assert(!parser.has_dynamic_width());
    assert(parser.get_width() == 1);
  }
  {
    std::format_spec::parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV("{}"), 100};
    parser.parse(parse_ctx);

    assert(parser.has_width());
    assert(parser.has_dynamic_width());
    assert(parser.get_width() == 0);

    parser.set_width(42);
    assert(parser.has_width());
    assert(!parser.has_dynamic_width());
    assert(parser.get_width() == 42);
  }
  {
    std::format_spec::parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV("{42}"), 100};
    parser.parse(parse_ctx);

    assert(parser.has_width());
    assert(parser.has_dynamic_width());
    assert(parser.get_width() == 42);

    parser.clear_width();
    assert(!parser.has_width());
    assert(!parser.has_dynamic_width());
    LIBCPP_ASSERT(parser.get_width() == 0);
  }
}

template <class CharT>
constexpr void test_precision() {
  {
    std::format_spec::parser<CharT> parser;
    assert(!parser.has_precision());
    assert(!parser.has_dynamic_precision());
    LIBCPP_ASSERT(parser.get_precision() == std::__format::__number_max);
  }
  {
    std::format_spec::parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV(".1"), 100};
    parser.parse(parse_ctx);

    assert(parser.has_precision());
    assert(!parser.has_dynamic_precision());
    assert(parser.get_precision() == 1);
  }
  {
    std::format_spec::parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV(".{}"), 100};
    parser.parse(parse_ctx);

    assert(parser.has_precision());
    assert(parser.has_dynamic_precision());
    assert(parser.get_precision() == 0);

    parser.set_precision(42);
    assert(parser.has_precision());
    assert(!parser.has_dynamic_precision());
    assert(parser.get_precision() == 42);
  }
  {
    std::format_spec::parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV("{}.{}"), 100};
    parser.parse(parse_ctx);

    assert(parser.has_width());
    assert(parser.has_dynamic_width());
    assert(parser.get_width() == 0);

    assert(parser.has_precision());
    assert(parser.has_dynamic_precision());
    assert(parser.get_precision() == 1);
  }
  {
    std::format_spec::parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV(".{42}"), 100};
    parser.parse(parse_ctx);

    assert(parser.has_precision());
    assert(parser.has_dynamic_precision());
    assert(parser.get_precision() == 42);

    parser.clear_precision();
    assert(!parser.has_precision());
    assert(!parser.has_dynamic_precision());
    LIBCPP_ASSERT(parser.get_precision() == std::__format::__number_max);
  }
  {
    std::format_spec::parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV(""), 100};
    parser.parse(parse_ctx, std::format_spec::options{.precision = false});
  }
  check_exception([] {
    std::format_spec::parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV(".1"), 100};
    parser.parse(parse_ctx, std::format_spec::options{.precision = false});
  });
}

template <class CharT>
constexpr void test_locale_specific_form() {
  {
    std::format_spec::parser<CharT> parser;
    assert(parser.get_locale_specific_form() == false);

    std::basic_format_parse_context parse_ctx{SV(""), 100};
    parser.parse(parse_ctx);
    assert(parser.get_locale_specific_form() == false);
  }
  {
    std::format_spec::parser<CharT> parser;

    std::basic_format_parse_context parse_ctx{SV("L"), 100};
    parser.parse(parse_ctx);
    assert(parser.get_locale_specific_form() == true);

    parser.set_locale_specific_form(false);
    assert(parser.get_locale_specific_form() == false);
  }
  {
    std::format_spec::parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV(""), 100};
    parser.parse(parse_ctx, std::format_spec::options{.locale_specific_form = false});
  }
  check_exception([] {
    std::format_spec::parser<CharT> parser;
    std::basic_format_parse_context parse_ctx{SV("L"), 100};
    parser.parse(parse_ctx, std::format_spec::options{.locale_specific_form = false});
  });
}

template <class CharT>
constexpr void test_type() {
  {
    std::format_spec::parser<CharT> parser;
    assert(parser.get_type() == std::__format_spec::_Flags::_Type::__default);

    std::basic_format_parse_context parse_ctx{SV(""), 100};
    parser.parse(parse_ctx);
    assert(parser.get_type() == std::__format_spec::_Flags::_Type::__default);
  }
  {
    std::format_spec::parser<CharT> parser;
    assert(parser.get_type() == std::__format_spec::_Flags::_Type::__default);
    {
      std::basic_format_parse_context parse_ctx{SV("a"), 100};
      parser.parse(parse_ctx);
      assert(parser.get_type() == std::__format_spec::_Flags::_Type::__float_hexadecimal_lower_case);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("A"), 100};
      parser.parse(parse_ctx);
      assert(parser.get_type() == std::__format_spec::_Flags::_Type::__float_hexadecimal_upper_case);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("b"), 100};
      parser.parse(parse_ctx);
      assert(parser.get_type() == std::__format_spec::_Flags::_Type::__binary_lower_case);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("B"), 100};
      parser.parse(parse_ctx);
      assert(parser.get_type() == std::__format_spec::_Flags::_Type::__binary_upper_case);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("c"), 100};
      parser.parse(parse_ctx);
      assert(parser.get_type() == std::__format_spec::_Flags::_Type::__char);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("d"), 100};
      parser.parse(parse_ctx);
      assert(parser.get_type() == std::__format_spec::_Flags::_Type::__decimal);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("e"), 100};
      parser.parse(parse_ctx);
      assert(parser.get_type() == std::__format_spec::_Flags::_Type::__scientific_lower_case);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("E"), 100};
      parser.parse(parse_ctx);
      assert(parser.get_type() == std::__format_spec::_Flags::_Type::__scientific_upper_case);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("f"), 100};
      parser.parse(parse_ctx);
      assert(parser.get_type() == std::__format_spec::_Flags::_Type::__fixed_lower_case);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("F"), 100};
      parser.parse(parse_ctx);
      assert(parser.get_type() == std::__format_spec::_Flags::_Type::__fixed_upper_case);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("g"), 100};
      parser.parse(parse_ctx);
      assert(parser.get_type() == std::__format_spec::_Flags::_Type::__general_lower_case);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("G"), 100};
      parser.parse(parse_ctx);
      assert(parser.get_type() == std::__format_spec::_Flags::_Type::__general_upper_case);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("o"), 100};
      parser.parse(parse_ctx);
      assert(parser.get_type() == std::__format_spec::_Flags::_Type::__octal);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("p"), 100};
      parser.parse(parse_ctx);
      assert(parser.get_type() == std::__format_spec::_Flags::_Type::__pointer);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("s"), 100};
      parser.parse(parse_ctx);
      assert(parser.get_type() == std::__format_spec::_Flags::_Type::__string);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("x"), 100};
      parser.parse(parse_ctx);
      assert(parser.get_type() == std::__format_spec::_Flags::_Type::__hexadecimal_lower_case);
    }
    {
      std::basic_format_parse_context parse_ctx{SV("X"), 100};
      parser.parse(parse_ctx);
      assert(parser.get_type() == std::__format_spec::_Flags::_Type::__hexadecimal_upper_case);
    }
    parser.set_type(std::__format_spec::_Flags::_Type::__char);
    assert(parser.get_type() == std::__format_spec::_Flags::_Type::__char);
  }
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

  static_assert(sizeof(std::format_spec::parser<CharT>) <= 2 * sizeof(int64_t));
}

constexpr bool test() {
  test<char>();
#ifndef TEST_HAS_NO_WIDE_CHARACTERS
  test<wchar_t>();
#endif

  return true;
}

// Belongs somewhere else
#if 1
// It's possible to aggregate initialize a formatter. That means a formatter
// can use other formatters when they fancy.
void test_formatter() {
  std::format_spec::parser<char> parser;
  std::formatter<std::string_view> formatter{parser};
  (void)formatter;
}

#endif
int main(int, char**) {
  test();
  static_assert(test());

  test_formatter();

  return 0;
}
