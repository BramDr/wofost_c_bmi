cmake_minimum_required(VERSION 3.16)
project(wofost_bmi C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
# Extensions stay on (GNU dialect) so strnlen() etc. are visible without
# extra feature-test-macro juggling.

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wformat=2 -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -O2)
endif()

# Lets DBG() output be turned on in a non-Debug build (e.g. Release) without
# a full reconfigure to CMAKE_BUILD_TYPE=Debug. defs.h treats NDEBUG as an
# unconditional kill switch for DBG(), matching assert()'s convention, so any
# NDEBUG that CMake's default Release/RelWithDebInfo/MinSizeRel flags add has
# to be stripped back out for this to take effect.
option(WOFOST_BMI_VERBOSE "Enable DBG() debug output regardless of build type" OFF)
if(WOFOST_BMI_VERBOSE)
  foreach(cfg RELEASE RELWITHDEBINFO MINSIZEREL)
    string(REPLACE "-DNDEBUG" "" CMAKE_C_FLAGS_${cfg} "${CMAKE_C_FLAGS_${cfg}}")
  endforeach()
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

add_executable(wofost_bmi ${WOFOST_BMI_SOURCES})

target_include_directories(wofost_bmi PRIVATE
  ${WOFOST_BMI_INCLUDE_DIRS}
  ${BMI_INCLUDE_DIR}
)

target_link_libraries(wofost_bmi PRIVATE
  netCDF::netcdf
  m
)

# Enables the DBG() macro in include/defs.h for Debug builds, or any build
# when WOFOST_BMI_VERBOSE is set, regardless of whether the generator is
# single-config (Makefiles/Ninja) or multi-config (Visual Studio).
target_compile_definitions(wofost_bmi PRIVATE
  $<$<OR:$<CONFIG:Debug>,$<BOOL:${WOFOST_BMI_VERBOSE}>>:DEBUG>
)
