.. _ModulesInLibcxx:

=================
Modules in libc++
=================

This page contains information regarding C++23 module support in libc++.
There are two kinds of modules available in Clang

 * `Clang specific modules <https://clang.llvm.org/docs/Modules.html>`_
 * `C++ modules <https://clang.llvm.org/docs/StandardCPlusPlusModules.html>`_

This page mainly discusses the C++ modules. In C++ there are also header units,
these are not in this patch.

This information is mainly to describe some of the features of this work in
progress patch. It is not intended to be committed in this form.

Overview
========

The module sources are stored in ``.cppm`` files. These need to be available as
BMIs, which are ``.pcm`` files for Clang. These files are not portable, they
depend on the compiler used and its compilation flags. Therefore there needs to
be a way to distribute the ``.cppm`` files to the user and offer a way for them
to build and use the ``.pcm`` files.

What works
~~~~~~~~~~

 * Building BMIs
 * Running tests using modules, not all tests pass yet.
 * Using the ``std`` module in external projects
 * Using the ``std.compat`` module in external projects

Some of the current limitations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 * For now everything requires CMake.
 * The files are not installed, but available in the build tree.
 * The ``pcm`` files are regenerated when running a ``lit`` invocation.
 * There is no Buildkite CI job yet. Work on this has started, but the CI Docker
   image is not updated yet. (There are issues with the official CI image which
   need to be resolved before adding new features.)
 * The ``std`` module is almost complete.
 * The ``std.compat`` module is almost complete.
 * The modules are not tested.
 * The modules are not reviewed for correctness.
 * Requires CMake 3.26
 * Requires Ninja 1.11
 * Requires Clang 17 (Clang 16 should work, but is not tested)
 * The path to the compiler may not be a symlink, ``clang-scan-deps`` does
   not handle that case properly.
 * Configurations with parts disable do not work properly. The ``#ifdef``
   are not in the ``.cppm`` files yet.

Blockers
~~~~~~~~

  * libc++

    * Tests depend on ``uint32_t``, ``size_t`` in the global namespace.
      Fixing this is WIP.
    * There are a lot of failing tests, this needs to be investigated after
      the global namespace issues have been fixed.
      Currently 222 of the 7243 test in the directory ``libcxx/test/std`` fail
      when using the ``std.compat`` module.
    * CMake 3.26 doesn't work with ``libunwind``. For ``libc++`` and
      ``libc++abi`` it ignores a lot of compilation flags.
      `Patch <https://llvm.org/D145596>`__
    * The ``-pthread`` flag is not used a compile flag. This flag affects
      the preprocessor so it should be a compile and linker flag.
      There is a WIP patch.
    * There are symbols with internal linkage in the header.

      * Due to older language versions don't support ``inline constexpr``.
        `Patch <https://reviews.llvm.org/D143797>`__
      * The code contains ``_LIBCPP_HIDE_FROM_ABI static constexpr``. There
        might be a patch.

  * Clang

    * ``__synth_three_way_result`` does not work. There is a work-around in libc++.

      * `bug report <https://github.com/llvm/llvm-project/issues/57222>`__
      * `patch <https://reviews.llvm.org/D140002>`__

    * The ranges Niebloids doen't work properly. There is no bug report, this
      might be related to ``__synth_three_way_result``. Changing
      ``inline constexpr auto foo = foo_t{}`` to
      ``inline constexpr foo_t foo{}`` seems to fix it in libc++.

    * Including headers after importing the ``std`` module may fail. This is
      hard to solve and there is a work-around by first including all headers.
      `bug report <https://github.com/llvm/llvm-project/issues/61329>`__

Discussion points
~~~~~~~~~~~~~~~~~

 * The ``std`` module is a C++23 feature, currently it's available in both
   ``C++20`` and ``C++23``. The simple reason is that this allows testing with
   different language versions. (This will be needed once the first ``C++26``
   features become available.) Does libc++ want to retroactively supply the
   module in ``C++23`` or restrict it to ``C++23`` and later?
 * Do we want the feature behind a configuration flag or not? Louis already
   mentioned he prefers it generally available. Then we need to restrict it on
   the CMake version used.
 * I'm not too unhappy with the generic approach for user project. I am quite
   unhappy with how we need to do tests. I have some ideas how we can improve
   this, basically by caching multiple configurations. However it would be good
   to discuss that approach before spending effort in it.
 * Currently the ``.cppm`` file and the generated ``CMakeLists.txt`` are not
   installed. Before doing that it would be good to determine what the best
   location for these files is.
 * The current approach to make the tests work with modules add a ``#ifdef``
   to select between including headers or importing the module ``std``.
   This works great for testing during development, however the libc++
   maintainers need to decide to keep using this approach in production or
   that a different solution needs to be used.


Design choices
==============

Shipping
--------

Since BMIs are not portable libc++ will ship its ``cppm`` files. Users need to
compile them to BMIs. Since the feature is under heavy development there is no
support for build tools. For that reason a CMake file will be shipped. Once
build tool vendors have support for libc++'s modules this support file will be
removed. (This file is used for the libc++ tests too, so maybe it remains build
it will not longer be shipped.)

The modules will shipped in a directory structure similar to the include.
Includes are stored in ``<PREFIX>/modules/c++/v1`` modules will be stored in
``<PREFIX>/modules/c++/v1/``.

To make it easy for build tool vendors the directory
``<PREFIX>/modules/c++/v1/`` will have the following contents

  * ``std.cppm`` the main ``std`` module source.
  * ``std/`` all other sources for the ``std`` module. Using this directory
    makes it easy for vendors to glob all files belonging the the ``std``
    module.
  * ``std.compat.cppm`` the main ``std.compat`` module source.
  * ``std.compat/`` like ``std/`` but for the ``std.compat`` module.

If in the future the C++ Standard defines more modules they will get a similar
treatment. (For example modules for experimental library features.)

.. note:: This matches the current proposal on `discourse
   <https://discourse.llvm.org/t/rfc-about-the-default-location-for-std-modules/69191/29>`__
   so it may still change.

Testing
-------

To properly test all tests with modules all tests need to be converted. The
script ``libcxx/utils/use_modules_in_test.py`` can do this conversion. Since not
all developers are happy with the output this script will be executed in the CI
and then the CI will run on the modified sources.

Running tests
=============

In order to run the tests with C++23 modules the libc++ needs to be build with
the following configuration option in CMake ``LIBCXX_ENABLE_STD_MODULE``.

The tests are executed like

.. code-block:: bash

  $ <build>/bin/llvm-lit -sv libcxx/test/std/containers # Run the tests without modules
  $ <build>/bin/llvm-lit -sv -Denable_modules=clang libcxx/test/std/containers # Run the tests with Clang modules
  $ <build>/bin/llvm-lit -sv -Denable_modules=std libcxx/test/std/containers # Run the tests with Standard std module
  $ <build>/bin/llvm-lit -sv -Denable_modules=std.compat libcxx/test/std/containers # Run the tests with Standard std.compat module

The Clang modules are the modules that have been available for many libc++ releases.
The Standard modules are the new feature.

Using in external projects
==========================

Users need to be able to build their own BMI files. Currently this requires a
local build of libc++ with modules enabled. "Importing" libc++ is a project can
be done with the following CMake code.

.. code-block:: cmake

  include(FetchContent)
  FetchContent_Declare(
    std
    URL file://<build>/include/c++/modules/
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )
  FetchContent_GetProperties(std)
  if(NOT std_POPULATED)
    # The C++ version used in your project must match the version used here.
    set(CMAKE_CXX_STANDARD 23)
    # These compiler flags must match the flags used in your project.
    # When they don't match the generated BMIs can't be used in your project.
    set(CMAKE_CXX_FLAGS "-fexperimental-library")
    FetchContent_Populate(std)
    add_subdirectory(${std_SOURCE_DIR} ${std_BINARY_DIR} EXCLUDE_FROM_ALL)
  endif()

The libraries and/or executables that use the ``std`` module need to link
against the ``std`` library. This is needed for CMake to get the proper module
dependencies. Similar for the ``std.compat`` module.

.. warning:: There is a bug in Clang where including headers after an import
   may fail. Instead include all headers first and then include the module.


.. code-block:: cmake

   #include <vector>

   #include "A header that may include other headers.h"

   import std;

   ...
