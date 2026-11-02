cmake_minimum_required(VERSION 3.16)
project(shore-backend-wtl VERSION 0.1.0 LANGUAGES CXX)

if(NOT WIN32 OR NOT MSVC)
  message(FATAL_ERROR "The WTL backend requires a Windows MSVC toolchain")
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

find_package(shore CONFIG REQUIRED)
find_path(WTL_INCLUDE_DIR atlapp.h
  HINTS "${WTL_ROOT}" "$ENV{WTL_ROOT}"
  PATH_SUFFIXES Include include WTL WTL/Include
)
if(NOT WTL_INCLUDE_DIR)
  message(FATAL_ERROR "WTL headers were not found. Set WTL_ROOT to the WTL installation directory.")
endif()

add_library(shore_backend_wtl SHARED src/wtl_backend.cpp)
add_library(shore-backend-wtl::shore-backend-wtl ALIAS shore_backend_wtl)
set_target_properties(shore_backend_wtl PROPERTIES
  WINDOWS_EXPORT_ALL_SYMBOLS ON
  EXPORT_NAME shore-backend-wtl
)
target_compile_definitions(shore_backend_wtl PUBLIC UNICODE _UNICODE)
target_include_directories(shore_backend_wtl
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE src "${WTL_INCLUDE_DIR}"
)
target_link_libraries(shore_backend_wtl PUBLIC shore::shore comctl32)

install(TARGETS shore_backend_wtl EXPORT shoreBackendWtlTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT shoreBackendWtlTargets
  FILE shoreBackendWtlTargets.cmake
  NAMESPACE shore-backend-wtl::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/shore-backend-wtl
)

configure_package_config_file(
  cmake/shore-backend-wtlConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/shore-backend-wtlConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/shore-backend-wtl
)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/shore-backend-wtlConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/shore-backend-wtlConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/shore-backend-wtlConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/shore-backend-wtl
)
