#! /usr/bin/env python

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

import os, re, sys

fn = sys.argv[1]
lineNumber = 0
errors = []

MAX_LINE_LENGTH = 110
INCLUDE_RE = re.compile(r'^\s*#\s*include\s*[<"](.*)[>"]')
DOXYGEN_FILE_RE = re.compile(r'\s*//! *@file %s' % os.path.basename(fn))

LICENSE = [ r'// Copyright 201[0-9]([,-]201[0-9])* Complete Genomics, Inc.',
            r'//',
            r'// Licensed under the Apache License, Version 2.0 \(the "License"\); you',
            r'// may not use this file except in compliance with the License. You',
            r'// may obtain a copy of the License at',
            r'//',
            r'// http://www.apache.org/licenses/LICENSE-2.0',
            r'//',
            r'// Unless required by applicable law or agreed to in writing, software',
            r'// distributed under the License is distributed on an "AS IS" BASIS,',
            r'// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or',
            r'// implied. See the License for the specific language governing',
            r'// permissions and limitations under the License.',
            ]

def addError(msg):
    errors.append( '%s:%d: cpplint error: %s' % (fn, lineNumber, msg) )


isHeader = fn.endswith('.hpp')
includeSeen = False
doxygenFile = False
licenseMatch = 0

for line in open(fn):
    lineNumber += 1
    if line.endswith('\r\n'):
        line = line[:-2]
    elif line.endswith('\n'):
        line = line[:-1]
    elif line.endswith('\r'):
        line = line[:-1]
    else:
        addError('unrecognized line ending')
    if line.find('\t') != -1:
        addError('line contains tab(s)')
    if len(line) > MAX_LINE_LENGTH:
        addError('line is too long')
    if line.find('TODO') != -1:
        addError('line contains TODO')
    if line.find('FIXME') != -1:
        addError('line contains FIXME')
    if line.find('XXX') != -1:
        addError('line contains XXX')
    if lineNumber <= len(LICENSE):
        m = re.match(LICENSE[lineNumber-1], line)
        if m:
            licenseMatch += 1
        else:
            addError('license line expected: '+LICENSE[lineNumber-1])
    mm = INCLUDE_RE.match(line)
    if mm:
        includeFn = mm.group(1)
        if not includeSeen and not fn.endswith('cgatools/core.hpp') and includeFn != 'cgatools/core.hpp':
            addError('first include is not cgatools/core.hpp')
        includeSeen = True
    mm = DOXYGEN_FILE_RE.match(line)
    if mm:
        doxygenFile = True
if not includeSeen:
    addError('file does not include cgatools/core.hpp')
if isHeader and not doxygenFile:
    addError('header file does not contain @file documentation comment')
if licenseMatch != len(LICENSE):
    addError('license not found at top of file')

for msg in errors:
    print msg

if len(errors) > 0:
    sys.exit(1)
