# Copyright 2010 Complete Genomics, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License"); you
# may not use this file except in compliance with the License. You
# may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
# implied. See the License for the specific language governing
# permissions and limitations under the License.

# cmake file

cmake_minimum_required(VERSION 2.6)
project (cgatools)
enable_testing()
include (CTest)

set (CGATOOLS_VERSION "1.8.0")
set (CGATOOLS_BUILD "1")
set (CGATOOLS_DEVBUILD 1 CACHE STRING "Turned off by the official build process.")
option (BUILD_SHARED_LIBS "Create shared libs (ON) or static libs (OFF)." OFF)
option (CGATOOLS_IS_PIPELINE "Is this cgatools to run in pipeline (ON) or not (OFF)." OFF)

if (BUILD_SHARED_LIBS)
  set (Boost_USE_STATIC_LIBS OFF)
else (BUILD_SHARED_LIBS)
  set (Boost_USE_STATIC_LIBS ON)
endif (BUILD_SHARED_LIBS)

if (CMAKE_COMPILER_IS_GNUCXX)
  if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    if (NOT BUILD_SHARED_LIBS)
      set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    endif (NOT BUILD_SHARED_LIBS)
  endif (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
endif (CMAKE_COMPILER_IS_GNUCXX)

if (CMAKE_HOST_WIN32)
  set (cgatools_extra_boost_libs bzip2 zlib)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_SCL_SECURE_NO_WARNINGS")
  set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   /D_SCL_SECURE_NO_WARNINGS")
  set (cgatools_vdefs "/DCGA_TOOLS_VERSION=\\\"${CGATOOLS_VERSION}\\\"")
  if (CGATOOLS_IS_PIPELINE)
    set(cgatools_vdefs "${cgatools_vdefs} /DCGA_TOOLS_IS_PIPELINE=1")
  else (CGATOOLS_IS_PIPELINE)
    set(cgatools_vdefs "${cgatools_vdefs} /DCGA_TOOLS_IS_PIPELINE=0")
  endif (CGATOOLS_IS_PIPELINE)
  set (cgatools_vdefs "${cgatools_vdefs} /DCGA_TOOLS_BUILD=\\\"${CGATOOLS_BUILD}\\\"")
  set (cgatools_vdefs "${cgatools_vdefs} /DCGA_TOOLS_DEVBUILD=${CGATOOLS_DEVBUILD}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${cgatools_vdefs}")
  set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${cgatools_vdefs}")
  set (CGATOOLS_EXTRA_LIBS )
else (CMAKE_HOST_WIN32)
  set (cgatools_extra_boost_libs )
  set (cgatools_vdefs "-DCGA_TOOLS_VERSION=\\\"${CGATOOLS_VERSION}\\\"")
  if (CGATOOLS_IS_PIPELINE)
    set(cgatools_vdefs "${cgatools_vdefs} -DCGA_TOOLS_IS_PIPELINE=1")
  else (CGATOOLS_IS_PIPELINE)
    set(cgatools_vdefs "${cgatools_vdefs} -DCGA_TOOLS_IS_PIPELINE=0")
  endif (CGATOOLS_IS_PIPELINE)
  set (cgatools_vdefs "${cgatools_vdefs} -DCGA_TOOLS_BUILD=\\\"${CGATOOLS_BUILD}\\\"")
  set (cgatools_vdefs "${cgatools_vdefs} -DCGA_TOOLS_DEVBUILD=${CGATOOLS_DEVBUILD}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${cgatools_vdefs}")
  set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${cgatools_vdefs}")
  set (CGATOOLS_EXTRA_LIBS bz2 z pthread)
endif (CMAKE_HOST_WIN32)

set (Boost_USE_MULTITHREADED ON)
set (Boost_ADDITIONAL_VERSIONS "1.41" "1.41.0" "1.42" "1.42.0" "1.46.1")
find_package (Boost 1.35.0 COMPONENTS regex iostreams filesystem program_options date_time
  system thread ${cgatools_extra_boost_libs} REQUIRED)
include_directories (${Boost_INCLUDE_DIRS})

include_directories (.)

if (CMAKE_COMPILER_IS_GNUCXX)

  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

endif (CMAKE_COMPILER_IS_GNUCXX)

file (GLOB_RECURSE lib_hdrs                   cgatools/*.hpp)
file (GLOB_RECURSE lib_srcs cgatools/*.cpp    cgatools/*.hpp)
file (GLOB_RECURSE exe_srcs cgatoolsexe/*.cpp cgatoolsexe/*.hpp)

foreach (lib_hdr ${lib_hdrs})
  file (RELATIVE_PATH lib_hdr_relative ${CMAKE_CURRENT_SOURCE_DIR} ${lib_hdr})
  list (APPEND lib_hdrs_relative ${lib_hdr_relative})
endforeach (lib_hdr)

if (CMAKE_HOST_WIN32)
  #set (groups "cgdata" "CNV\\CNVp" "CNV\\CNVprep" "CNV\\CNVutils" "command" "junctions" "mapping" "reference" "util" "variants")
  foreach (lib_src ${lib_srcs})
    file (RELATIVE_PATH lib_src_rel "${CMAKE_CURRENT_SOURCE_DIR}/cgatools" ${lib_src} )
    string ( REGEX REPLACE "[/][^/]+[.][ch]pp$" "" cga_group ${lib_src_rel} )
    string ( REGEX REPLACE "[^/]+[.][ch]pp$" "" cga_group ${cga_group} ) # eliminate files not in directories
    list (APPEND groups ${cga_group})
  endforeach (lib_src ${lib_srcs})
  list (REMOVE_DUPLICATES groups)
  list (REMOVE_ITEM groups "")
  message ("-- Created the following Visual Studio filters:")
  foreach (grp ${groups})
    file (GLOB_RECURSE grp_list cgatools/${grp}/*.hpp cgatools/${grp}/*.cpp)
    string ( REPLACE "/" "\\" grp_filter ${grp} )
    SOURCE_GROUP(${grp_filter} FILES ${grp_list})
    message ("--   ${grp_filter}")
  endforeach (grp)
  file (GLOB grp_list cgatools/*.hpp  cgatools/*.cpp)
  SOURCE_GROUP("core" FILES ${grp_list})
endif (CMAKE_HOST_WIN32)

add_library (cgatools ${lib_srcs})
target_link_libraries (cgatools)

add_executable (cgatoolsexe ${exe_srcs})
target_link_libraries (cgatoolsexe cgatools ${Boost_LIBRARIES} ${CGATOOLS_EXTRA_LIBS})
set_target_properties (cgatoolsexe PROPERTIES OUTPUT_NAME cgatools)
set_target_properties (cgatools cgatoolsexe
  PROPERTIES
  VS_SCC_PROJECTNAME "Perforce Project"
  VS_SCC_LOCALPATH   ".."
  VS_SCC_PROVIDER    "MSSCCI:Perforce SCM"
)

add_custom_command (
  TARGET cgatoolsexe
  POST_BUILD
  COMMAND cgatoolsexe
  ARGS man --format=html
  --output=${CMAKE_CURRENT_BINARY_DIR}/doc/cgatools-command-line-reference.html)

set (doc_root "share/cgatools-${CGATOOLS_VERSION}/doc")
install (TARGETS cgatoolsexe DESTINATION bin)
install (TARGETS cgatools DESTINATION lib)
install (DIRECTORY cgatools DESTINATION include
  FILES_MATCHING PATTERN *.hpp)

get_filename_component(DOXYGEN_INPUT_LIST cgatools ABSOLUTE)
get_filename_component(README_PATH README.txt ABSOLUTE)
add_subdirectory(doc)
add_subdirectory(smoketest)
