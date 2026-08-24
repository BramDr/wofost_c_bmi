cmake_minimum_required(VERSION 3.16)
project(wofost C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
# Extensions stay on (GNU dialect) so strnlen() etc. are visible without
# extra feature-test-macro juggling.

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wformat=2 -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wshadow -O2)
endif()

# netCDF is on conda-forge with a CMake package config; picked up automatically
# once the conda environment (see environment.yml) is active.
find_package(netCDF REQUIRED)

# bmi-c only ships a header (bmi.h), no library to link against.
find_path(BMI_INCLUDE_DIR
  NAMES bmi.h
  HINTS "$ENV{CONDA_PREFIX}/include"
)
if(NOT BMI_INCLUDE_DIR)
  message(FATAL_ERROR "Could not find bmi.h. Activate the wofost_bmi conda "
                       "environment (see environment.yml) before configuring.")
endif()

# Headers are included by bare filename (e.g. "simulation.h") regardless of
# which include/ subdirectory they actually live in, so all four need to be
# on the search path at once.
set(WOFOST_BMI_INCLUDE_DIRS
  include
  include/bmi
  include/simulation
  include/wofost
)

# GLOB + CONFIGURE_DEPENDS: source files change often right now; this picks up
# new/removed .c files on the next build without having to touch this list by
# hand. Re-run cmake manually if you ever notice stale sources after a rename.
file(GLOB_RECURSE WOFOST_BMI_SOURCES CONFIGURE_DEPENDS "src/*.c")
list(REMOVE_ITEM WOFOST_BMI_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/bmi/BmiMain.c")

# babel_wofost.toml links its Python wrapper against this library (see
# [library.WOFOST] library = "bmi_wofost"), so it needs to be install()-ed
# onto the linker/header search path, e.g.
#   cmake --install build --prefix "$CONDA_PREFIX"
add_library(bmiwofost SHARED ${WOFOST_BMI_SOURCES})

target_include_directories(bmiwofost PRIVATE
  ${WOFOST_BMI_INCLUDE_DIRS}
  ${BMI_INCLUDE_DIR}
)

target_link_libraries(bmiwofost PRIVATE
  netCDF::netcdf
  m
)

# Enables the DBG() macro in include/defs.h for Debug builds, regardless of
# whether the generator is single-config (Makefiles/Ninja) or multi-config
# (Visual Studio).
target_compile_definitions(bmiwofost PRIVATE
  $<$<CONFIG:Debug>:DEBUG>
)

# Standalone CLI (see run_ci.sh); a thin wrapper around the library above.
add_executable(wofost src/bmi/BmiMain.c)

target_include_directories(wofost PRIVATE
  ${WOFOST_BMI_INCLUDE_DIRS}
  ${BMI_INCLUDE_DIR}
)

target_link_libraries(wofost PRIVATE
  bmiwofost
)

include(GNUInstallDirs)
install(TARGETS bmiwofost
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
# bmi_wofost.h itself #includes "defs.h", so it has to travel with it.
install(FILES include/bmi/bmi_wofost.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
