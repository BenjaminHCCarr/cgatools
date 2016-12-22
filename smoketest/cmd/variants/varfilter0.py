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

tc.runCommand([ cgatools, 'fasta2crr',
                '--input='+pjoin(idir, 'variants/TestRef.fa'),
                '--output='+pjoin(odir, 'TestRef.crr') ])

def doCompare(a, b):
    tc.textCompare(a, b, ignorePrefixes=['#GENERATED_AT',
                                         '#FORMAT_VERSION',
                                         '#SOFTWARE_VERSION',
                                         '#CGATOOLS_VERSION'])

def convertFile(fn):
    direct = pjoin(odir, 'a-' + fn)
    tc.runCommand([ cgatools, 'varfilter',
                    '--beta',
                    '--reference='+pjoin(odir, 'TestRef.crr'),
                    '--input='+pjoin(idir, 'variants', fn),
                    '--output='+direct,
                    ])

    indirect = pjoin(odir, 'b-' + fn)
    tc.runCommand([ cgatools, 'varfilter',
                    '--beta',
                    '--reference='+pjoin(odir, 'TestRef.crr'),
                    '--input='+pjoin(idir, 'variants',
                                     fn+'#varType=snp:het:varScoreVAF<100,'+
                                     'varType=ins:het:varScoreVAF<500'),
                    '--output='+indirect,
                    ])
    return direct, indirect

# test varfile with holes
a, b = convertFile('var-13.tsv')
doCompare(a, pjoin(idir, 'variants', 'var-13.tsv'))
doCompare(a, pjoin(idir, 'variants', 'varfilter0', 'a-var-13.tsv'))
doCompare(b, pjoin(idir, 'variants', 'varfilter0', 'b-var-13.tsv'))
