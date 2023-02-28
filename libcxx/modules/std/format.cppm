# 1 __FILE__ 1 3
// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// TODO This file is two-fold
// - look at how we want to integrate a synopsis
// - test whether the experimental library works properly

module;
#include <format> // Since C++20

export module std:format;
#if !defined(_LIBCPP_HAS_NO_INCOMPLETE_FORMAT)
export namespace std {

  // [format.context], class template basic_format_context
  // template <class Out, class charT>
  // class basic_format_context;
  using std::basic_format_context;
  /*
    template<class Out, class charT>
    class basic_format_context {
      basic_format_args<basic_format_context> args_;      // exposition only
      Out out_;                                           // exposition only

    public:
      using iterator = Out;
      using char_type = charT;
      template<class T> using formatter_type = formatter<T, charT>;

      basic_format_arg<basic_format_context> arg(size_t id) const noexcept;
      std::locale locale();

      iterator out();
      void advance_to(iterator it);
    };
  */
  // using format_context  = basic_format_context<unspecified, char>;
  // using wformat_context = basic_format_context<unspecified, wchar_t>;
  using std::format_context;
#  ifndef _LIBCPP_HAS_NO_WIDE_CHARACTERS
  using std::wformat_context;
#  endif

  // [format.args], class template basic_format_args
  // template <class Context>
  // class basic_format_args;
  using std::basic_format_args;
  /*
    template<class Context>
    class basic_format_args {
      size_t size_;                               // exposition only
      const basic_format_arg<Context>* data_;     // exposition only

    public:
      basic_format_args() noexcept;

      template<class... Args>
        basic_format_args(const format-arg-store<Context, Args...>& store) noexcept;

      basic_format_arg<Context> get(size_t i) const noexcept;
    };
  */
  // using format_args  = basic_format_args<format_context>;
  // using wformat_args = basic_format_args<wformat_context>;
  using std::format_args;
#  ifndef _LIBCPP_HAS_NO_WIDE_CHARACTERS
  using std::wformat_args;
#  endif

  // [format.fmt.string], class template basic_format_string
  // template <class charT, class... Args>
  // struct basic_format_string;
  using std::basic_format_string;

  // template <class... Args>
  // using format_string = basic_format_string<char, type_identity_t<Args>...>;
  // template <class... Args>
  // using wformat_string = basic_format_string<wchar_t, type_identity_t<Args>...>;
  using std::format_string;
#  ifndef _LIBCPP_HAS_NO_WIDE_CHARACTERS
  using std::wformat_string;
#  endif

  // [format.functions], formatting functions
  // template <class... Args>
  // string format(format_string<Args...> fmt, Args&&... args);
  // template <class... Args>
  // wstring format(wformat_string<Args...> fmt, Args&&... args);
  // template <class... Args>
  // string format(const locale& loc, format_string<Args...> fmt, Args&&... args);
  // template <class... Args>
  // wstring format(const locale& loc, wformat_string<Args...> fmt, Args&&... args);
  using std::format;

  // string vformat(string_view fmt, format_args args);
  // wstring vformat(wstring_view fmt, wformat_args args);
  // string vformat(const locale& loc, string_view fmt, format_args args);
  // wstring vformat(const locale& loc, wstring_view fmt, wformat_args args);
  using std::vformat;

  // template <class Out, class... Args>
  // Out format_to(Out out, format_string<Args...> fmt, Args&&... args);
  // template <class Out, class... Args>
  // Out format_to(Out out, wformat_string<Args...> fmt, Args&&... args);
  // template <class Out, class... Args>
  // Out format_to(Out out, const locale& loc, format_string<Args...> fmt, Args&&... args);
  // template <class Out, class... Args>
  // Out format_to(Out out, const locale& loc, wformat_string<Args...> fmt, Args&&... args);
  using std::format_to;

  // template <class Out>
  // Out vformat_to(Out out, string_view fmt, format_args args);
  // template <class Out>
  // Out vformat_to(Out out, wstring_view fmt, wformat_args args);
  // template <class Out>
  // Out vformat_to(Out out, const locale& loc, string_view fmt, format_args args);
  // template <class Out>
  // Out vformat_to(Out out, const locale& loc, wstring_view fmt, wformat_args args);
  using std::vformat_to;

  // template <class Out>
  // struct format_to_n_result {
  //   Out out;
  //   iter_difference_t<Out> size;
  // };
  using std::format_to_n_result;
  // template <class Out, class... Args>
  // format_to_n_result<Out> format_to_n(Out out, iter_difference_t<Out> n, format_string<Args...> fmt, Args&&... args);
  // template <class Out, class... Args>
  // format_to_n_result<Out> format_to_n(Out out, iter_difference_t<Out> n, wformat_string<Args...> fmt, Args&&...
  // args); template <class Out, class... Args> format_to_n_result<Out> format_to_n(Out out, iter_difference_t<Out> n,
  // const locale& loc, format_string<Args...> fmt, Args&&... args); template <class Out, class... Args>
  // format_to_n_result<Out>
  // format_to_n(Out out, iter_difference_t<Out> n, const locale& loc, wformat_string<Args...> fmt, Args&&... args);
  using std::format_to_n;

  // template <class... Args>
  // size_t formatted_size(format_string<Args...> fmt, Args&&... args);
  // template <class... Args>
  // size_t formatted_size(wformat_string<Args...> fmt, Args&&... args);
  // template <class... Args>
  // size_t formatted_size(const locale& loc, format_string<Args...> fmt, Args&&... args);
  // template <class... Args>
  // size_t formatted_size(const locale& loc, wformat_string<Args...> fmt, Args&&... args);
  using std::formatted_size;

  // [format.formatter], formatter
  // template <class T, class charT = char>
  // struct formatter;
  using std::formatter;

  // [format.formattable], concept formattable
  // template <class T, class charT>
  // concept formattable = see below; // Since C++23
  using std::formattable;

  // template <class R, class charT>
  //     concept const-formattable-range = // exposition only
  //     ranges::input_range<const R> && formattable<ranges::range_reference_t<const R>, charT>;

  // template <class R, class charT>
  //     using fmt-maybe-const = // exposition only
  //     conditional_t<const-formattable-range<R, charT>, const R, R>;

  // [format.parse.ctx], class template basic_format_parse_context
  // template <class charT>a
  // class basic_format_parse_context;
  using std::basic_format_parse_context;

  /*
    template<class charT>
    class basic_format_parse_context {
    public:
      using char_type = charT;
      using const_iterator = typename basic_string_view<charT>::const_iterator;
      using iterator = const_iterator;

    private:
      iterator begin_;                                    // exposition only
      iterator end_;                                      // exposition only
      enum indexing { unknown, manual, automatic };       // exposition only
      indexing indexing_;                                 // exposition only
      size_t next_arg_id_;                                // exposition only
      size_t num_args_;                                   // exposition only

    public:
      constexpr explicit basic_format_parse_context(basic_string_view<charT> fmt,
                                                    size_t num_args = 0) noexcept;
      basic_format_parse_context(const basic_format_parse_context&) = delete;
      basic_format_parse_context& operator=(const basic_format_parse_context&) = delete;

      constexpr const_iterator begin() const noexcept;
      constexpr const_iterator end() const noexcept;
      constexpr void advance_to(const_iterator it);

      constexpr size_t next_arg_id();
      constexpr void check_arg_id(size_t id);
    };
  */

  // using format_parse_context  = basic_format_parse_context<char>;
  // using wformat_parse_context = basic_format_parse_context<wchar_t>;
  using std::format_parse_context;
#  ifndef _LIBCPP_HAS_NO_WIDE_CHARACTERS
  using std::wformat_parse_context;
#  endif

  // [format.range], formatting of ranges
  // [format.range.fmtkind], variable template format_kind
  // enum class range_format { disabled, map, set, sequence, string, debug_string }; // Since C++23
  using std::range_format;

  // template <class R>
  // constexpr unspecified format_kind = unspecified;
  using std::format_kind;

  // template <ranges::input_range R>
  //   requires same_as<R, remove_cvref_t<R>>
  // constexpr range_format format_kind<R> = see below;
  using std::format_kind;

  // [format.range.formatter], class template range_formatter
  // template <class T, class charT = char>
  //  requires same_as<remove_cvref_t<T>, T> && formattable<T, charT>
  // class range_formatter;
  using std::range_formatter;

  /*
    template<class T, class charT = char>
      requires same_as<remove_cvref_t<T>, T> && formattable<T, charT>
    class range_formatter {
      formatter<T, charT> underlying_;                                          // exposition only
      basic_string_view<charT> separator_ = STATICALLY-WIDEN<charT>(", ");      // exposition only
      basic_string_view<charT> opening-bracket_ = STATICALLY-WIDEN<charT>("["); // exposition only
      basic_string_view<charT> closing-bracket_ = STATICALLY-WIDEN<charT>("]"); // exposition only

    public:
      constexpr void set_separator(basic_string_view<charT> sep);
      constexpr void set_brackets(basic_string_view<charT> opening,
                                  basic_string_view<charT> closing);
      constexpr formatter<T, charT>& underlying() { return underlying_; }
      constexpr const formatter<T, charT>& underlying() const { return underlying_; }

      template<class ParseContext>
        constexpr typename ParseContext::iterator
          parse(ParseContext& ctx);

      template<ranges::input_range R, class FormatContext>
          requires formattable<ranges::range_reference_t<R>, charT> &&
                   same_as<remove_cvref_t<ranges::range_reference_t<R>>, T>
        typename FormatContext::iterator
          format(R&& r, FormatContext& ctx) const;
    };
  */

  // [format.range.fmtdef], class template range-default-formatter
  // template <range_format K, ranges::input_range R, class charT>
  //    struct range-default-formatter; // exposition only
  /*
    template<ranges::input_range R, class charT>
    struct range-default-formatter<range_format::sequence, R, charT> {    // exposition only
    private:
      using maybe-const-r = fmt-maybe-const<R, charT>;                    // exposition only
      range_formatter<remove_cvref_t<ranges::range_reference_t<maybe-const-r>>,
                      charT> underlying_;                                 // exposition only

    public:
      constexpr void set_separator(basic_string_view<charT> sep);
      constexpr void set_brackets(basic_string_view<charT> opening,
                                  basic_string_view<charT> closing);

      template<class ParseContext>
        constexpr typename ParseContext::iterator
          parse(ParseContext& ctx);

      template<class FormatContext>
        typename FormatContext::iterator
          format(maybe-const-r& elems, FormatContext& ctx) const;
    };
  */

  // [format.range.fmtmap], [format.range.fmtset], [format.range.fmtstr], specializations for maps, sets, and strings
  /*
    template<ranges::input_range R, class charT>                         // Since C++23
    struct range-default-formatter<range_format::map, R, charT> {
    private:
      using maybe-const-map = fmt-maybe-const<R, charT>;                  // exposition only
      using element-type =                                                // exposition only
        remove_cvref_t<ranges::range_reference_t<maybe-const-map>>;
      range_formatter<element-type, charT> underlying_;                   // exposition only

    public:
      constexpr range-default-formatter();

      template<class ParseContext>
        constexpr typename ParseContext::iterator
          parse(ParseContext& ctx);

      template<class FormatContext>
        typename FormatContext::iterator
          format(maybe-const-map& r, FormatContext& ctx) const;
    };

    template<ranges::input_range R, class charT>
    struct range-default-formatter<range_format::set, R, charT> {         // Since C++23
    private:
      using maybe-const-set = fmt-maybe-const<R, charT>;                  // exposition only
      range_formatter<remove_cvref_t<ranges::range_reference_t<maybe-const-set>>,
                      charT> underlying_;                                 // exposition only

    public:
      constexpr range-default-formatter();

      template<class ParseContext>
        constexpr typename ParseContext::iterator
          parse(ParseContext& ctx);

      template<class FormatContext>
        typename FormatContext::iterator
          format(maybe-const-set& r, FormatContext& ctx) const;
    };

    template<range_format K, ranges::input_range R, class charT>
      requires (K == range_format::string || K == range_format::debug_string)
    struct range-default-formatter<K, R, charT> {                         // Since C++23
    private:
      formatter<basic_string<charT>, charT> underlying_;                  // exposition only

    public:
      template<class ParseContext>
        constexpr typename ParseContext::iterator
          parse(ParseContext& ctx);

      template<class FormatContext>
        typename FormatContext::iterator
          format(see below& str, FormatContext& ctx) const;
    };
  */
  // template <ranges::input_range R, class charT>
  //   requires(format_kind<R> != range_format::disabled) &&
  //           formattable<ranges::range_reference_t<R>, charT>
  //           struct formatter<R, charT> : range-default-formatter<format_kind<R>, R, charT> {};

  // [format.arguments], arguments
  // [format.arg], class template basic_format_arg
  // template <class Context>
  // class basic_format_arg;
  using std::basic_format_arg;

  /*
    template<class Context>
    class basic_format_arg {
    public:
      class handle;

    private:
      using char_type = typename Context::char_type;                              // exposition only

      variant<monostate, bool, char_type,
              int, unsigned int, long long int, unsigned long long int,
              float, double, long double,
              const char_type*, basic_string_view<char_type>,
              const void*, handle> value;                                         // exposition only

      template<class T> explicit basic_format_arg(T&& v) noexcept;                // exposition only
      explicit basic_format_arg(float n) noexcept;                                // exposition only
      explicit basic_format_arg(double n) noexcept;                               // exposition only
      explicit basic_format_arg(long double n) noexcept;                          // exposition only
      explicit basic_format_arg(const char_type* s);                              // exposition only

      template<class traits>
        explicit basic_format_arg(
          basic_string_view<char_type, traits> s) noexcept;                       // exposition only

      template<class traits, class Allocator>
        explicit basic_format_arg(
          const basic_string<char_type, traits, Allocator>& s) noexcept;          // exposition only

      explicit basic_format_arg(nullptr_t) noexcept;                              // exposition only

      template<class T>
        explicit basic_format_arg(T* p) noexcept;                                 // exposition only

    public:
      basic_format_arg() noexcept;

      explicit operator bool() const noexcept;
    };

    template<class Context>
    class basic_format_arg<Context>::handle {
      const void* ptr_;                                           // exposition only
      void (*format_)(basic_format_parse_context<char_type>&,
                      Context&, const void*);                     // exposition only

      template<class T> explicit handle(T&& val) noexcept;        // exposition only

      friend class basic_format_arg<Context>;                     // exposition only

    public:
      void format(basic_format_parse_context<char_type>&, Context& ctx) const;
    };
  */

  // template <class Visitor, class Context>
  // decltype(auto) visit_format_arg(Visitor&& vis, basic_format_arg<Context> arg);
  using std::visit_format_arg;

  // [format.arg.store], class template format-arg-store
  // template <class Context, class... Args>
  //    class format-arg-store; // exposition only
  /*
    template<class Context, class... Args>
    class format-arg-store {                                      // exposition only
      array<basic_format_arg<Context>, sizeof...(Args)> args;     // exposition only
    };
  */

  // template <class Context = format_context, class... Args>
  //     format-arg-store<Context, Args...> make_format_args(Args&&... fmt_args);
  // template <class... Args>
  //     format-arg-store<wformat_context, Args...> make_wformat_args(Args&&... args);
  using std::make_format_args;
#  ifndef _LIBCPP_HAS_NO_WIDE_CHARACTERS
  using std::make_wformat_args;
#  endif

  // [format.error], class format_error
  // class format_error;
  using std::format_error;
  /*
    class format_error : public runtime_error {
    public:
      explicit format_error(const string& what_arg);
      explicit format_error(const char* what_arg);
    };
  */

  /*
    template<class charT, formattable<charT>... Ts>
    struct formatter<pair-or-tuple<Ts...>, charT> {                             // Since C++23
    private:
      tuple<formatter<remove_cvref_t<Ts>, charT>...> underlying_;               // exposition only
      basic_string_view<charT> separator_ = STATICALLY-WIDEN<charT>(", ");      // exposition only
      basic_string_view<charT> opening-bracket_ = STATICALLY-WIDEN<charT>("("); // exposition only
      basic_string_view<charT> closing-bracket_ = STATICALLY-WIDEN<charT>(")"); // exposition only

    public:
      constexpr void set_separator(basic_string_view<charT> sep);
      constexpr void set_brackets(basic_string_view<charT> opening,
                                  basic_string_view<charT> closing);

      template<class ParseContext>
        constexpr typename ParseContext::iterator
          parse(ParseContext& ctx);

      template<class FormatContext>
        typename FormatContext::iterator
          format(see below& elems, FormatContext& ctx) const;
    };
  */
} // namespace std
#endif // !defined(_LIBCPP_HAS_NO_INCOMPLETE_FORMAT)
