#!/usr/bin/env python
# ===------------------------------------------------------------------------------===##
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# ===------------------------------------------------------------------------------===##

# This script converts a regular test to a test using modules.
#
# Note this script is mainly used to test the patch https://reviews.llvm.org/D144994
# It has not been decided that libc++ will use this approach for modules. Therefore
# the transformation uses a simple algorithm and generates not the prettiest output.
#
# The main goal is to quickly convert a batch of tests to the use modules to test
# whether the std module works for the headers that have been converted. The
# script can't be executed twice on the same header, so it's intended this
# script is executed on a not-yet converted code base.
#
# Usage
#   use_modules_in_test.py <file>
#
# If the file contains at least one of the headers that is available in the std module
# it will add a guarded import statement. All headers that are in the module std will
# be included conditionally.

import re
import sys
from dataclasses import dataclass, field


HEADERS_MODULE_STD = [
    "algorithm",
    "any",
    "array",
    "atomic",
    "barrier",
    "bit",
    "bitset",
    "cctype",
    "cfenv",
    "charconv",
    "chrono",
    "cinttypes",
    "climits",
    "clocale",
    "cmath",
    "codecvt",
    "compare",
    "complex",
    "concepts",
    "coroutines",
    "csetjmp",
    "csignal",
    "cstdarg",
    "cstddef",
    "cstdio",
    "cstdlib",
    "cstlib",
    "cstring",
    "ctime",
    "ctype.h",
    "cuchar",
    "cwchar",
    "cwctype",
    "deque",
    "exception",
    "execution",
    "expected",
    "fenv.h",
    "filesystem",
    "flat_map",
    "flat_set",
    "float.h",
    "format",
    "forward_list",
    "fstream",
    "functional",
    "generator",
    "initializer_list",
    "inttypes.h",
    "iomanip",
    "ios",
    "iostream",
    "istream",
    "iterator",
    "latch",
    "limits",
    "list",
    "locale",
    "locale.h",
    "map",
    "math.h",
    "mdspan",
    "memory",
    "memory_resource",
    "mutex",
    "new",
    "numbers",
    "numeric",
    "optional",
    "ostream",
    "print",
    "queue",
    "random",
    "ranges",
    "ratio",
    "regex",
    "scoped_allocator",
    "semaphore",
    "set",
    "setjmp.h",
    "shared_mutex",
    "signal.h",
    "source_location",
    "span",
    "spanstream",
    "sstream",
    "stack",
    "stacktrace",
    "stdarg.h",
    "stddef.h",
    "stdexcept",
    "stdfloat",
    "stdint.h",
    "stdio.h",
    "stdlib.h",
    "stop_token",
    "streambuf",
    "string",
    "string.h",
    "string_view",
    "strstream",
    "syncstream",
    "system_error",
    "thread",
    "time.h",
    "tuple",
    "type_traits",
    "typeindex",
    "typeinfo",
    "uchar.h",
    "unordered_map",
    "unordered_set",
    "utility",
    "valarray",
    "variant",
    "vector",
    "wchar.h",
    "wctype.h",
]

# Tests that need to be marked as XFAIL for various reasons
#
# For example is_literal type has been removed from C++20, the tests reenables it using
# test specific compile flags. This is not supported by modules.
UNSUPPORTED_TESTS = [
    "libcxx/test/std/utilities/meta/meta.unary/meta.unary.prop/is_literal_type.pass.cpp",
]
# Due to a Clang limitation the import needs to be after all includes.
# Therefore non-system includes are parsed too.
INCLUDE_REGEX = re.compile(r"^\s*#\s*include\s*[\"<](?P<include>[^\">]+)[\">]")

# Ignore tests that issue deprecation warnings.
#
# An alternative would be just to remove this option, this would be possible
# when the script is only used to convert in the CI.
DISABLE_DEPRECATION_WARNINGS_REGEX = re.compile(
    r"//\s*ADDITIONAL_COMPILE_FLAGS:.*-D_LIBCPP_DISABLE_DEPRECATION_WARNINGS"
)


# Marks a test as unsupported.
#
# Since the script runs on a not yet converted set of tests the script does not
# test existing markers. In the future it should look at:
# - tests with (XFAIL|UNSUPPORTED) use_module_std(|.compat)
# - tests with an import statement
def is_unsupported(filename: str) -> bool:
    file = open(filename)
    lines = file.readlines()
    file.close()

    for index, line in enumerate(lines):
        if DISABLE_DEPRECATION_WARNINGS_REGEX.search(line):
            lines.insert(
                index + 1, "// UNSUPPORTED: use_module_std, use_module_std.compat\n"
            )

            file = open(filename, "w")
            for line in lines:
                file.write(line)

            return True
    return False


@dataclass
class Include:
    line: int = -1
    is_system_header: bool = False


def gather_includes(lines: list[str]) -> list[Include]:
    result = list()
    for index, line in enumerate(lines):
        if m := INCLUDE_REGEX.match(line):
            result.append(Include(index, m.group("include") in HEADERS_MODULE_STD))
    return result


def insert_imports(lines: list[str], includes: list[Include]) -> None:
    assert len(lines)
    assert len(includes)

    # The imports are always added after the last header
    # Note this fails when a test has an #include in the middle of the file

    line = includes[-1].line + 1

    imports = list()
    lines.insert(
        line,
        """
#if defined(TEST_USE_MODULE_STD)
import std;
#elif defined(TEST_USE_MODULE_STD_COMPAT)
import std.compat;
#endif
""",
    )


def make_modularized_headers_conditionally(
    lines: list[str], includes: list[Include]
) -> None:
    assert len(lines)
    assert len(includes)

    inside = False
    # Iterate backwards so the line numbers of the includes remain stable.
    for include in reversed(includes):
        if include.is_system_header:
            if not inside:
                lines.insert(include.line + 1, "#endif // TEST_USE_MODULE\n")
                inside = True
        else:
            if inside:
                lines.insert(include.line + 1, "#ifdef TEST_USE_MODULE\n")
                inside = False

    # Add the ifdef for the first entry if needed.
    if inside:
        for include in includes:
            if include.is_system_header:
                lines.insert(include.line, "#ifdef TEST_USE_MODULE\n")
                return
        assert False


def process_supported(filename: str) -> None:
    import_written = False

    file = open(filename)
    lines = file.readlines()
    file.close()
    includes = gather_includes(lines)
    # for include in includes:
    #    print(f"{include.is_system_header} {include.line:4}: {lines[include.line]}")
    if len(includes) == 0:
        return

    # TODO Once Clang works properly with includes after imports this import can be
    # at the top of the file.
    insert_imports(lines, includes)
    # for line in lines:
    #    print(line)

    make_modularized_headers_conditionally(lines, includes)
    # for line in lines:
    #    print(line)

    file = open(filename, "w")
    for line in lines:
        file.write(line)


def process(filename: str) -> None:
    if not is_unsupported(filename):
        process_supported(filename)


if __name__ == "__main__":
    if len(sys.argv) == 2:
        process(sys.argv[1])
