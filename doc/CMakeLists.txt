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

set (DOXYGEN_OUTPUT_DIR api)

include(FindDoxygen)

if (DOXYGEN)
  configure_file (doxygen.cfg ${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg)
  add_custom_target (doc
    ${DOXYGEN} ${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg ${lib_hdrs})
else (DOXYGEN)
  message (STATUS "WARNING: doxygen not found - api docs will not be created")
endif (DOXYGEN)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/index.html)
  configure_file (index.html ${CMAKE_CURRENT_BINARY_DIR}/index.html)
endif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/index.html)

set (doc_root "share/cgatools-${CGATOOLS_VERSION}/doc")

install (FILES ${README_PATH} LICENSE.txt
  DESTINATION "${doc_root}")

install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${DOXYGEN_OUTPUT_DIR}/html
  DESTINATION "${doc_root}/api"
  OPTIONAL)

install (FILES cgatools-user-guide.pdf
  ${CMAKE_CURRENT_BINARY_DIR}/index.html
  ${CMAKE_CURRENT_BINARY_DIR}/cgatools-command-line-reference.html
  cgatools-release-notes.pdf cg-logo.png style.css
  ScoreCalibrationMethods.pdf
  DESTINATION "${doc_root}"
  OPTIONAL)
