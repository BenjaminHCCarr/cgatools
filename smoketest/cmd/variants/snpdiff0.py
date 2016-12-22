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

reports = 'Output,Verbose,Stats'

tc.runCommand([ cgatools, 'snpdiff',
                '--reference='+pjoin(odir, 'TestRef.crr'),
                '--variants='+pjoin(idir, 'variants/var-00.tsv'),
                '--genotypes='+pjoin(idir, 'variants/snpdiff0/genotypes.tsv'),
                '--output-prefix='+pjoin(odir, 'gen-'),
                ])

for key in reports.split(','):
    tc.textCompare(pjoin(idir, 'variants/snpdiff0/gen-'+key+'.tsv'),
                   pjoin(odir, 'gen-'+key+'.tsv'))

tc.runCommand([ cgatools, 'snpdiff',
                '--reference='+pjoin(odir, 'TestRef.crr'),
                '--variants='+pjoin(idir, 'variants/var-00.tsv'),
                '--genotypes='+pjoin(idir, 'variants/snpdiff0/genotypes2.tsv'),
                '--output-prefix='+pjoin(odir, 'gen2-'),
                '--reports='+reports,
                ])

for key in reports.split(','):
    tc.textCompare(pjoin(idir, 'variants/snpdiff0/gen2-'+key+'.tsv'),
                   pjoin(odir, 'gen2-'+key+'.tsv'))

for xx in [ 3, 4 ]:
    exn = False
    try:
        tc.runCommand([ cgatools, 'snpdiff',
                        '--reference='+pjoin(odir, 'TestRef.crr'),
                        '--variants='+pjoin(idir, 'variants/var-00.tsv'),
                        '--genotypes='+pjoin(idir, 'variants/snpdiff0/genotypes%d.tsv' % xx),
                        '--output-prefix='+pjoin(odir, 'gen%d-' % xx),
                        '--reports='+reports,
                        ])
    except:
        exn = True
    if not exn:
        raise Exception('invalid genotypes not rejected')
