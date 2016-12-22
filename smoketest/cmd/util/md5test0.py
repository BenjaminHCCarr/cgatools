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


import os,sys
sys.path = [ os.path.dirname(os.path.dirname(sys.argv[0])) ] + sys.path
from tutil import tucore as tc

idir = sys.argv[1]
odir = sys.argv[2]
cgatools = sys.argv[3]
cgatoolsapitest = sys.argv[4]

#####################################################################
# END BOILERPLATE
#####################################################################

import md5, random

def bin2hex(ss):
    result = []
    for ch in ss:
        result.append( hex(ord(ch) >> 4)[-1] )
        result.append( hex(ord(ch) & 0xf)[-1] )
    return ''.join(result)

def testmd5(ss):
    hh = md5.new()
    hh.update(ss)
    correct = bin2hex(hh.digest())
    actual = tc.getCommandStdout([ cgatoolsapitest, 'TestMd5' ], ss).strip()
    print 'finished test, length: %s, md5: %s' % (len(ss),correct)
    if (correct != actual):
        raise Exception('length: %s, correct=%s, actual=%s' % (len(ss),correct,actual))

random.seed(12345)
ss = ''
for ii in xrange(0,4096,31):
    ss += chr(random.randint(0, 255))
    testmd5(ss)
