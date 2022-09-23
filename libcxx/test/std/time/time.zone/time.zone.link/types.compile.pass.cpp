//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17

// XFAIL: availability-tzdb-missing

// <chrono>

// class time_zone_link
// {
//   time_zone_link(time_zone_link&&)            = default;
//   time_zone_link& operator=(time_zone_link&&) = default;
//
//   ...
// };

#include <chrono>
#include <concepts>

static_assert(std::move_constructible<std::chrono::time_zone_link>);
