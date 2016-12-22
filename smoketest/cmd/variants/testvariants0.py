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

from os.path import join as pjoin

tc.runCommand([ cgatoolsapitest, 'TestCall', idir, odir ])

tc.runCommand([ cgatools, 'testvariants',
                '--beta',
                '--reference='+pjoin(odir, 'TestRef.crr'),
                '--output='+pjoin(odir, 'tested-variants-00-09.tsv'),
                '--input='+pjoin(idir, 'variants/testvariants0/variant-listing-00.tsv'),
                '--variants='+pjoin(idir, 'variants/var-00.tsv'),
                '--variants='+pjoin(idir, 'variants/var-09.tsv'),
                ])

tc.textCompare(pjoin(idir, 'variants/testvariants0/tested-variants-00-09.tsv'),
               pjoin(odir, 'tested-variants-00-09.tsv'))
