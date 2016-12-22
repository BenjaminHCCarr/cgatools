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

reports = 'SuperlocusOutput,SuperlocusStats,LocusOutput,LocusStats,DebugCallOutput,DebugSuperlocusOutput'

tc.runCommand([ cgatools, 'calldiff',
                '--reference='+pjoin(odir, 'TestRef.crr'),
                '--variantsA='+pjoin(idir, 'variants/var-00.tsv'),
                '--variantsB='+pjoin(idir, 'variants/var-00.tsv'),
                '--output-prefix='+pjoin(odir, 'cd-00-00-'),
                '--reports='+reports,
                ])

for key in reports.split(','):
    tc.textCompare(pjoin(idir, 'variants/calldiff0/cd-00-00-'+key+'.tsv'),
                   pjoin(odir, 'cd-00-00-'+key+'.tsv'))

tc.runCommand([ cgatools, 'calldiff',
                '--reference='+pjoin(odir, 'TestRef.crr'),
                '--variantsA='+pjoin(idir, 'variants/var-00.tsv'),
                '--variantsB='+pjoin(idir, 'variants/var-09.tsv'),
                '--output-prefix='+pjoin(odir, 'cd-00-09-'),
                '--reports='+reports,
                ])

for key in reports.split(','):
    tc.textCompare(pjoin(idir, 'variants/calldiff0/cd-00-09-'+key+'.tsv'),
                   pjoin(odir, 'cd-00-09-'+key+'.tsv'))
