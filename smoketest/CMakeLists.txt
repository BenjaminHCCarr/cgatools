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

#######################################################################
# smoketest
#
# Cgatools smoke test cmake file. Emphasis of the smoke tests is to
# test as many code paths (especially to achieve platform
# independence) as is reasonable using small data and in a very short
# amount of time. These tests are packaged with cgatools source code,
# so size is a premium.
#######################################################################

set(TEST_DATA_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/data")

# Adds a command line program that depends on cgatools api.
macro(add_api_command category testname)
  list(APPEND sm_api_test_names ${testname})
  list(APPEND sm_api_test_srcs ${CMAKE_CURRENT_SOURCE_DIR}/api/${category}/${testname}.cpp)
endmacro(add_api_command)

# Adds a command line program that depends on cgatools api, and adds a
# test case that runs it.
macro(add_api_test category testname)
  add_api_command(${category} ${testname})
  set(workdir "${CMAKE_CURRENT_BINARY_DIR}/testout/api/${category}/${testname}")
  file(MAKE_DIRECTORY ${workdir})
  add_test (NAME sm_api_${category}_${testname}
    COMMAND cmake -E chdir ${workdir}
    $<TARGET_FILE:cgatoolsapitest> ${testname} ${TEST_DATA_ROOT} ${workdir})
endmacro(add_api_test)

# Finishing step to create the command line program defined by all the
# add_api_command and add_api_test calls.
macro(mk_cgatoolsapitest)
  create_test_sourcelist(sm_api_tests SmokeApiTests.cpp ${sm_api_test_names})
  add_executable(cgatoolsapitest SmokeApiTests.cpp ${sm_api_test_srcs})
  target_link_libraries(cgatoolsapitest cgatools ${Boost_LIBRARIES} ${CGATOOLS_EXTRA_LIBS})
endmacro(mk_cgatoolsapitest)

# Adds a test that runs a python script.
macro(add_cmd_test category testname)
  set(workdir "${CMAKE_CURRENT_BINARY_DIR}/testout/cmd/${category}/${testname}")
  file(MAKE_DIRECTORY ${workdir})
  add_test (NAME sm_cmd_${category}_${testname}
    COMMAND cmake -E chdir ${workdir}
    python ${CMAKE_CURRENT_SOURCE_DIR}/cmd/${category}/${testname}.py
    ${TEST_DATA_ROOT} ${workdir} $<TARGET_FILE:cgatoolsexe> $<TARGET_FILE:cgatoolsapitest>)
endmacro(add_cmd_test)

# Header compilation tests.
foreach (lib_hdr_relative ${lib_hdrs_relative})
  configure_file (CompileHeader.cpp ${CMAKE_CURRENT_BINARY_DIR}/${lib_hdr_relative}.cpp)
  list (APPEND lib_compileheader_srcs ${CMAKE_CURRENT_BINARY_DIR}/${lib_hdr_relative}.cpp)
endforeach (lib_hdr_relative)
add_library(cgatoolscompileheader ${lib_compileheader_srcs})

# Code readability and license tests.
foreach (src ${lib_srcs} ${exe_srcs})
  get_filename_component (src_name ${src} NAME)
  add_test (lint_${src_name} python ${CMAKE_CURRENT_SOURCE_DIR}/cpplint.py ${src})
endforeach (src)

#######################################################################
# API tests.
#
# To add a new test Foo that runs C++ code against the cgatools API,
# create a source file Foo.cpp in a subdirectory of the api directory
# with a main-like function of the same name, then add the
# add_api_test call below. The first parameter to add_api_test is the
# directory the source file is in, and the second is Foo. Your
# main-like function is called by a program in an initially empty
# directory, and the path to smoketest/data is passed in as the first
# argument, and the path to the empty directory is passed in as the
# second argument.
#######################################################################

add_api_test(util TestAssertPositive)
add_api_test(util TestBaseUtil)
add_api_test(util TestDelimitedFile)
add_api_test(util TestDelimitedLineParser)
add_api_test(util TestException)
add_api_test(util TestParse)
add_api_test(util TestRangeIntersector)
add_api_command(util TestMd5)
add_api_test(variants TestCall)
mk_cgatoolsapitest()

#######################################################################
# Command line tests.
#
# To add a new test foo that runs python code against cgatools
# program, create a source file foo.py in a subdirectory of the cmd
# directory, copying the boilerplate from the top of another python
# test case, then add the add_cmd_test call below. The first paramter
# to add_cmd_test is the directory the source file is in, and the
# second is foo. Your foo.py script has access to smoketest/data
# (idir), an empty directory to write temporary files to, and the path
# to cgatools executable and the cgatoolsapitest executable.
#######################################################################
add_cmd_test(util md5test0)
add_cmd_test(reference testcrr0)
add_cmd_test(variants snpdiff0)
add_cmd_test(variants calldiff0)
add_cmd_test(variants listvariants0)
add_cmd_test(variants testvariants0)
add_cmd_test(variants varcombine0)
add_cmd_test(variants varcombine1)
add_cmd_test(variants varfilter0)
add_cmd_test(conv map2sam0)
add_cmd_test(conv evidence2sam0)
add_cmd_test(conv evidence2sam1)
add_cmd_test(misc join0)
add_cmd_test(junctions testjunctdiff)
add_cmd_test(junctions testj2events)

set_target_properties (cgatoolsapitest
  PROPERTIES
  VS_SCC_PROJECTNAME "Perforce Project"
  VS_SCC_LOCALPATH   "../.."
  VS_SCC_PROVIDER    "MSSCCI:Perforce SCM"
)


