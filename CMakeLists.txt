cmake_minimum_required(VERSION 3.16)
project(wofost C)

# Strict ISO C11 (-std=c11, not gnu11), so non-standard functions are not
# silently available. POSIX functions used by the code (strdup, strnlen) are
# requested explicitly via the feature-test macro below instead.
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
add_compile_definitions(_POSIX_C_SOURCE=200809L)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wformat=2 -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wshadow)
endif()

# netCDF and bmi-c come from the pixi environment (see pixi.toml).
# bmi-c only ships a header (bmi.h).
find_package(netCDF REQUIRED)
find_path(BMI_INCLUDE_DIR NAMES bmi.h)
if(NOT BMI_INCLUDE_DIR)
  message(FATAL_ERROR "Could not find bmi.h. Configure through pixi "
                      "('pixi run configure-c', or from within 'pixi shell') "
                      "so the pixi environment is on the search path.")
endif()

set(WOFOST_BMI_INCLUDE_DIRS
  include
  include/bmi
  include/simulation
  include/wofost
)
file(GLOB_RECURSE WOFOST_BMI_SOURCES CONFIGURE_DEPENDS "src/*.c")
# Main.c holds main() for the executable below and must stay out of the library.
list(REMOVE_ITEM WOFOST_BMI_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/Main.c")

# babel.toml links its Python wrapper against this library (see babel.toml).
add_library(bmiwofost SHARED ${WOFOST_BMI_SOURCES})
target_include_directories(bmiwofost PRIVATE
  ${WOFOST_BMI_INCLUDE_DIRS}
  ${BMI_INCLUDE_DIR}
)
target_link_libraries(bmiwofost PRIVATE
  netCDF::netcdf
  m
)
# Enables the DBG() macro in include/defs.h for Debug builds
target_compile_definitions(bmiwofost PRIVATE
  $<$<CONFIG:Debug>:DEBUG>
)

# Standalone CLI; a thin wrapper around the library above.
add_executable(wofost src/Main.c)
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
install(FILES include/bmi/bmi_wofost.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS wofost
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
