//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// Tests in test/std should work with all Standard library implementations.
// This test greps for the usage of some libc++ internal defines that have a
// TEST macro that achieves the same in a portable fashion.
//
// Upon failure the found results are part of the test's failure output. This
// makes finding the occurrences easy.

// Some items are whitelisted
// - _LIBCPP_VERSION is used to test whether we're testing against libc++
// - The language.support directory uses internal macros by generated scripts
// - The experimental directory is a testing ground

RUN: not grep -R --exclude-dir language.support --exclude-dir experimental '#if.*[ (]_LIBCPP' %S/../../std |grep -v ':.*_LIBCPP_VERSION'|grep -v ':.*_LIBCPP[A-Z_]*_ABI_'
