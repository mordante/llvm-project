
# The headers of Table 24: C++ library headers [tab:headers.cpp]
# and the headers of Table 25: C++ headers for C library facilities [tab:headers.cpp.c]
set(LIBCXX_SOURCES_MODULE_STD
  std.cppm
  std/algorithm.cppm
  std/any.cppm
  std/array.cppm
  std/atomic.cppm
  std/barrier.cppm
  std/bit.cppm
  std/bitset.cppm
  std/cassert.cppm
  std/cctype.cppm
  std/cerrno.cppm
  std/cfenv.cppm
  std/cfloat.cppm
  std/charconv.cppm
  std/chrono.cppm
  std/cinttypes.cppm
  std/climits.cppm
  std/clocale.cppm
  std/cmath.cppm
  std/codecvt.cppm
  std/compare.cppm
  std/complex.cppm
  std/concepts.cppm
  std/coroutine.cppm
  std/csetjmp.cppm
  std/csignal.cppm
  std/cstdarg.cppm
  std/cstddef.cppm
  std/cstdio.cppm
  std/cstdlib.cppm
  std/cstdlib.cppm
  std/cstring.cppm
  std/ctime.cppm
  std/cuchar.cppm
  std/cwchar.cppm
  std/cwctype.cppm
  std/deque.cppm
  std/exception.cppm
  std/execution.cppm
  std/expected.cppm
  std/filesystem.cppm
  std/flat_map.cppm
  std/flat_set.cppm
  std/format.cppm
  std/forward_list.cppm
  std/fstream.cppm
  std/functional.cppm
  std/future.cppm
  std/generator.cppm
  std/initializer_list.cppm
  std/iomanip.cppm
  std/ios.cppm
  std/iosfwd.cppm
  std/iostream.cppm
  std/iostream.cppm
  std/istream.cppm
  std/iterator.cppm
  std/latch.cppm
  std/limits.cppm
  std/list.cppm
  std/locale.cppm
  std/map.cppm
  std/mdspan.cppm
  std/memory.cppm
  std/memory_resource.cppm
  std/mutex.cppm
  std/new.cppm
  std/numbers.cppm
  std/numeric.cppm
  std/optional.cppm
  std/ostream.cppm
  std/print.cppm
  std/queue.cppm
  std/random.cppm
  std/ranges.cppm
  std/ratio.cppm
  std/regex.cppm
  std/scoped_allocator.cppm
  std/semaphore.cppm
  std/set.cppm
  std/shared_mutex.cppm
  std/source_location.cppm
  std/span.cppm
  std/spanstream.cppm
  std/sstream.cppm
  std/stack.cppm
  std/stacktrace.cppm
  std/stdexcept.cppm
  std/stdexcept.cppm
  std/stdfloat.cppm
  std/stop_token.cppm
  std/streambuf.cppm
  std/string.cppm
  std/string_view.cppm
  std/strstream.cppm
  std/syncstream.cppm
  std/system_error.cppm
  std/thread.cppm
  std/tuple.cppm
  std/type_traits.cppm
  std/typeindex.cppm
  std/typeinfo.cppm
  std/unordered_map.cppm
  std/unordered_set.cppm
  std/utility.cppm
  std/valarray.cppm
  std/variant.cppm
  std/vector.cppm
  std/version.cppm
)

# The headers of Table 25: C++ headers for C library facilities [tab:headers.cpp.c]
set(LIBCXX_SOURCES_MODULE_STD_COMPAT
  std.compat.cppm
  std.compat/cassert.cppm
  std.compat/cctype.cppm
  std.compat/cerrno.cppm
  std.compat/cfenv.cppm
  std.compat/cfloat.cppm
  std.compat/cinttypes.cppm
  std.compat/climits.cppm
  std.compat/clocale.cppm
  std.compat/cmath.cppm
  std.compat/csetjmp.cppm
  std.compat/csignal.cppm
  std.compat/cstdarg.cppm
  std.compat/cstddef.cppm
  std.compat/cstdint.cppm
  std.compat/cstdio.cppm
  std.compat/cstdlib.cppm
  std.compat/cstring.cppm
  std.compat/ctime.cppm
  std.compat/cuchar.cppm
  std.compat/cwchar.cppm
  std.compat/cwctype.cppm
)

# TODO MODULES the CMakeLists.txt in the install directory is only temporary
# When that is removed the configured file can use the substitution
# LIBCXX_GENERATED_INCLUDE_TARGET_DIR avoiding this set.
# Also clean up the parts needed to generate the install version.
set(LIBCXX_CONFIGURED_INCLUDE_DIR ${LIBCXX_GENERATED_INCLUDE_TARGET_DIR})
configure_file(
  "CMakeLists.txt.in"
  "${LIBCXX_GENERATED_MODULE_DIR}/CMakeLists.txt"
  @ONLY
)
set(
  LIBCXX_CONFIGURED_INCLUDE_DIR
  "${CMAKE_INSTALL_PREFIX}/${LIBCXX_INSTALL_INCLUDE_DIR}"
)
configure_file(
  "CMakeLists.txt.in"
  "${LIBCXX_GENERATED_MODULE_DIR}/install/CMakeLists.txt"
  @ONLY
)

set(_all_modules "${LIBCXX_GENERATED_MODULE_DIR}/CMakeLists.txt")
foreach(file ${LIBCXX_SOURCES_MODULE_STD} ${LIBCXX_SOURCES_MODULE_STD_COMPAT})
  set(src "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
  set(dst "${LIBCXX_GENERATED_MODULE_DIR}/${file}")
  add_custom_command(OUTPUT ${dst}
    DEPENDS ${src}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${src} ${dst}
    COMMENT "Copying CXX module ${file}")
  list(APPEND _all_modules "${dst}")
endforeach()

add_custom_target(generate-cxx-modules
  ALL DEPENDS
    ${_all_modules}
)

if (LIBCXX_INSTALL_MODULES)
  foreach(file ${LIBCXX_SOURCES_MODULE_STD} ${LIBCXX_SOURCES_MODULE_STD_COMPAT})
    get_filename_component(dir ${file} DIRECTORY)
    install(FILES ${file}
      DESTINATION "${LIBCXX_INSTALL_MODULE_DIR}/${dir}"
      PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
    )
  endforeach()

  # Install the generated CMake file to the generic include dir.
  # TODO MODULES Remove this file
  install(FILES "${LIBCXX_GENERATED_MODULE_DIR}/install/CMakeLists.txt"
    DESTINATION "${LIBCXX_INSTALL_MODULE_DIR}"
    PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
  )
endif()
