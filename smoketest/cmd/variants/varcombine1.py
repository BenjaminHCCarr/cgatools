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

def convertFile(fn):
    newfn = pjoin(odir, 'flat-' + fn)
    tc.runCommand([ cgatools, 'generatemastervar',
                    '--beta',
                    '--reference='+pjoin(odir, 'TestRef.crr'),
                    '--variants='+pjoin(idir, 'variants', fn),
                    '--output='+newfn,
                    '--repmask-data='+pjoin(idir, 'variants/TestRmsk.tsv'),
                    '--annotations=copy,repeat',
                    ])
    return newfn

def runCallDiff(a, b, prefix):
    reports = 'SuperlocusOutput,SuperlocusStats,LocusOutput,LocusStats'
    tc.runCommand([ cgatools, 'calldiff',
                    '--reference='+pjoin(odir, 'TestRef.crr'),
                    '--variantsA='+a,
                    '--variantsB='+b,
                    '--output-prefix='+pjoin(odir, prefix),
                    '--reports='+reports,
                    ])
    for key in reports.split(','):
        tc.textCompare(pjoin(idir, 'variants/varcombine1/' + prefix + key + '.tsv'),
                       pjoin(odir, prefix+key+'.tsv'))


tc.runCommand([ cgatools, 'fasta2crr',
                '--input='+pjoin(idir, 'variants/TestRef.fa'),
                '--output='+pjoin(odir, 'TestRef.crr') ])

# Convert both varfiles
a = convertFile('var-00.tsv')
b = convertFile('var-09.tsv')

# calldiff tests
runCallDiff(a, a, 'cd-00-00-')
runCallDiff(a, b, 'cd-00-09-')

# snpdiff test
reports = 'Output,Verbose,Stats'
tc.runCommand([ cgatools, 'snpdiff',
                '--reference='+pjoin(odir, 'TestRef.crr'),
                '--variants='+a,
                '--genotypes='+pjoin(idir, 'variants/snpdiff0/genotypes.tsv'),
                '--output-prefix='+pjoin(odir, 'gen-'),
                ])

for key in reports.split(','):
    tc.textCompare(pjoin(idir, 'variants/varcombine1/gen-'+key+'.tsv'),
                   pjoin(odir, 'gen-'+key+'.tsv'))

# listvariants test
tc.runCommand([ cgatools, 'listvariants',
                '--beta',
                '--reference='+pjoin(odir, 'TestRef.crr'),
                '--output='+pjoin(odir, 'variant-listing-00-09.tsv'),
                '--variants='+a,
                '--variants='+b,
                ])
tc.textCompare(pjoin(idir, 'variants/listvariants0/variant-listing-00-09.tsv'),
               pjoin(odir, 'variant-listing-00-09.tsv'))

# testvariants test
tc.runCommand([ cgatools, 'testvariants',
                '--beta',
                '--reference='+pjoin(odir, 'TestRef.crr'),
                '--output='+pjoin(odir, 'tested-variants-00-09.tsv'),
                '--input='+pjoin(idir, 'variants/testvariants0/variant-listing-00.tsv'),
                '--variants='+a,
                '--variants='+b,
                ])

tc.textCompare(pjoin(idir, 'variants/testvariants0/tested-variants-00-09.tsv'),
               pjoin(odir, 'tested-variants-00-09.tsv'))

