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

import random
from os.path import join as pjoin

from tutil.tucore import generateChr, writeFasta

random.seed(12345)

chrPostfixes = [str(num) for num in xrange(1,23)]
chrPostfixes += ['X','Y','M']

chrs = []
for chrPostfix in chrPostfixes:
    chrs.append( ('chr'+chrPostfix, generateChr([10])))

writeFasta(pjoin(odir, 'reference.fa'), chrs)

tc.runCommand([ cgatools, 'fasta2crr',
                '--input='+pjoin(odir, 'reference.fa'),
                '--output='+pjoin(odir, 'reference.crr') ])


tc.runCommand([ cgatools, 'junctiondiff', '--beta',
                               '--reference='+pjoin(odir, 'reference.crr'),
                               '--junctionsA='+pjoin(idir, 'junctions/junctiondiff0/junctions0.tsv'),
                               '--junctionsB='+pjoin(idir, 'junctions/junctiondiff0/junctions1.tsv'),
                               '--output-prefix='+pjoin(odir,''),
                               '--statout'
                            ])


for fileName in ['diff-junctions0.tsv','report.tsv']:
    tc.textCompare(pjoin(idir, 'junctions/junctiondiff0/', fileName),
                   pjoin(odir, fileName),
                   ['#GENERATED_AT','#CGATOOLS_VERSION', '#SOFTWARE_VERSION', '#FORMAT_VERSION'])
