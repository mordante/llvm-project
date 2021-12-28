//===----------------------------------------------------------------------===//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17
// UNSUPPORTED: libcpp-no-concepts
// UNSUPPORTED: libcpp-has-no-incomplete-format

// needs handle clas

// Not a real test just to play with some examples

#include <format>

#include <cassert>

template <class F>
void check_exception(F&& f) {
  try {
    f();
    assert(false);
  } catch (std::format_error& e) {
    return;
  }
  assert(false);
}

// [format.formatter.spec]/6
namespace example_1 {
enum color { red, green, blue };
const char* color_names[] = {"red", "green", "blue"};
} // namespace example_1
template <>
struct std ::formatter<example_1::color> : std::formatter<const char*> {
  auto format(example_1::color c, format_context& ctx) {
    return formatter<const char*>::format(example_1::color_names[c], ctx);
  }
};

namespace example_1 {
void test() {
  assert(std::format("{}", red) == "red");
  assert(std::format("{:.1}", red) == "r");
}

} // namespace example_1

// [format.formatter.spec]/6
// with validation before
namespace example_2 {
enum color { red, green, blue };
const char* color_names[] = {"red", "green", "blue"};
} // namespace example_2

template <>
struct std::formatter<example_2::color> : std::formatter<const char*> {
  constexpr auto parse(format_parse_context& ctx) {
    auto begin = ctx.begin();
    auto end = std::formatter<const char*>::parse(ctx);

    // When there's a valid format string and a point it's either
    // - a fill
    // - a precision
    // both cases require at least one extra character.
    if (end - begin > 1) {
      // Fill?
      if (*begin == '.') {
        ++begin;
        // Note LWGXXXX can break this assumption
        if (*begin != '<' && *begin != '^' && *begin != '>')
          throw std::format_error("I don't want to be truncateD");

        ++begin;
      }
      // Search for . as precision.
      if (std::find(begin, end, '.') != end)
        throw std::format_error("I don't want to be truncated");
    }
    return end;
  }
  auto format(example_2::color c, std::format_context& ctx) {
    return formatter<const char*>::format(example_2::color_names[c], ctx);
  }
};

namespace example_2 {
void test() {
  assert(std::format("{}", red) == "red");
  assert(std::format("{:.<}", red) == "red");
  assert(std::format("{:.^}", red) == "red");
  assert(std::format("{:.>}", red) == "red");

  check_exception([] { std::format("{:.1}", red); });
  check_exception([] { std::format("{:.<.1}", red); });
  check_exception([] { std::format("{:.^.1}", red); });
  check_exception([] { std::format("{:.>.1}", red); });
}
} // namespace example_2

// [format.formatter.spec]/6
// with validation before
namespace example_3 {
enum color { red, green, blue };
const char* color_names[] = {"red", "green", "blue"};
} // namespace example_3

template <>
struct std::formatter<example_3::color> : std::formatter<const char*> {
  constexpr auto parse(format_parse_context& ctx) {
    // HMMM
    using Base = typename std::formatter<const char*>::_Base;
    auto result = std::formatter<const char*>::parse(ctx);
    if (parser.has_precision())
      throw std::format_error("I don't want to be truncated");
    return result;
  }
  auto format(example_3::color c, std::format_context& ctx) {
    return formatter<const char*>::format(example_3::color_names[c], ctx);
  }
};

namespace example_3 {
void test() {
  assert(std::format("{}", red) == "red");
  assert(std::format("{:.<}", red) == "red");
  assert(std::format("{:.^}", red) == "red");
  assert(std::format("{:.>}", red) == "red");

  check_exception([] { std::format("{:.1}", red); });
  check_exception([] { std::format("{:.<.1}", red); });
  check_exception([] { std::format("{:.^.1}", red); });
  check_exception([] { std::format("{:.>.1}", red); });
}
} // namespace example_3

// A more fancy formatter with more options:
#if 0
formatters available xXdDs

none|x|X:
• align: default none (uses zero padding)
• sign: forbidden always none
• alternate form: forbidden always true
• zero padding: default true
• width: default 8
• precision: forbidden
• locale-specific form: forbidden always false
d|D should be formatted as [r, g, b] using formatting ranges
s:
• align: As string
• sign: As string
• alternate form: As string
• zero padding: As string
• width: As string
• precision: forbidden
• locale-specific form: As string

Note we could do more for X when asuming it's a container
#endif
namespace example_4 {
enum class color { black = 0x000000, red = 0xFF0000, gold = 0xFFCC00 };
} // namespace example_4

template <>
struct std::formatter<example_4::color> {
  constexpr auto parse(format_parse_context& ctx) {
    // fields is same as default
    auto result = parser.parse(ctx, format_spec::fields{.sign = false,
                                                        .alternate_form = false,
                                                        .zero_padding = false,
                                                        .precision = false,
                                                        .locale_specific_form = false});

    switch (parser.get_type()) {
    case std::__format_spec::_Flags::_Type::__default:
      parser.set_type(std::__format_spec::_Flags::_Type::__hexadecimal_lower_case);
      [[fallthrough]];
    case std::__format_spec::_Flags::_Type::__hexadecimal_lower_case:
    case std::__format_spec::_Flags::_Type::__hexadecimal_upper_case:
      parser.set_alternate_form(true);
      parser.set_zero_padding(true);
      parser.set_width(8);
      break;

    case std::__format_spec::_Flags::_Type::__decimal:
      throw std::format_error("Not implemented P2286");

    case std::__format_spec::_Flags::_Type::__string:
      /* Do noting */
      break;

    default:
      throw std::format_error("Unsuported");
    }
    return result;
  }

  auto format(example_4::color c, std::format_context& ctx) {
    switch (parser.get_type()) {
    case std::__format_spec::_Flags::_Type::__default:
      parser.set_type(std::__format_spec::_Flags::_Type::__hexadecimal_lower_case);
      [[fallthrough]];
    case std::__format_spec::_Flags::_Type::__hexadecimal_lower_case:
    case std::__format_spec::_Flags::_Type::__hexadecimal_upper_case: {
#if 1
      // Formatter hasn't been implemented yet.
      std::formatter<unsigned> formatter{parser};
      return formatter.format(static_cast<unsigned>(c), ctx);
#else
      return ctx.out();
#endif
    }
    case std::__format_spec::_Flags::_Type::__decimal:
      throw std::format_error("Not implemented yet");

    case std::__format_spec::_Flags::_Type::__string: {
      static constexpr const char* names[] = {"black", "red", "gold"};
      std::string_view sv;
      switch (c) {
      case example_4::color::black:
        sv = names[0];
        break;

      case example_4::color::red:
        sv = names[1];
        break;

      case example_4::color::gold:
        sv = names[2];
        break;
      }
      std::formatter<std::string_view> formatter{parser};
      return formatter.format(sv, ctx);
    }
    default:
      throw std::format_error("Unsuported");
    }
  }
  format_spec::parser<char> parser;
};

namespace example_4 {
void test() {}
} // namespace example_4

int main(int, char**) {
  example_1::test();
  example_2::test();
  example_3::test();
  example_4::test();

  return 0;
}
