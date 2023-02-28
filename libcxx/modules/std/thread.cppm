# 1 __FILE__ 1 3
// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

module;
#include <thread>

export module std:thread;
export namespace std {
  // [thread.thread.class], class thread
  using std::thread;

  using std::swap;

#if 0 // TODO Enable when implementing P0660
  // [thread.jthread.class], class jthread
  using std::jthread;
#endif

  // [thread.thread.this], namespace this_­thread
  namespace this_thread {
    using std::this_thread::get_id;

    using std::this_thread::sleep_for;
    using std::this_thread::sleep_until;
    using std::this_thread::yield;
  } // namespace this_thread

} // namespace std
