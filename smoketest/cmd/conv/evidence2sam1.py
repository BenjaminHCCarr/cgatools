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


# Each string represents the following fields:
# <no of adaptors>-<half dnb length>-<{read lengths}>

adaptorConfigs = [
    "2-30-10-10-10",
    "4-35-10-10-10-5" 
]

refSources = [
    "/prod/pv-01/pipeline/REF/HUMAN37-M_10-REF/reference.crr"
]

tc.runCommand([ cgatools, 'evidence2sam',
                '--beta',
                '--reference=/prod/pv-01/pipeline/REF/HUMAN37-M_10-REF/reference.crr',
                '--evidence-dnbs='+pjoin(idir, 'conv/evidence2sam1/evidenceDnbs-chr21-human37-'+adaptorConfigs[0]+'.tsv'),
                '--keep-duplicates',
                '--add-allele-id',
                '--output='+pjoin(odir, 'evidenceDnbs-chr21-human37-'+adaptorConfigs[0]+'.sam')
                ])

tc.runCommand([ cgatools, 'evidence2sam',
                '--beta',
                '--reference=/prod/pv-01/pipeline/REF/HUMAN37-M_10-REF/reference.crr',
                '--evidence-dnbs='+pjoin(idir, 'conv/evidence2sam1/evidenceDnbs-chr1-human37-'+adaptorConfigs[1]+'.tsv'),
                '--keep-duplicates',
                '--add-allele-id',
                '--output='+pjoin(odir, 'evidenceDnbs-chr1-human37-'+adaptorConfigs[1]+'.sam')
                ])

tc.textCompare(pjoin(idir, 'conv/evidence2sam1/evidenceDnbs-chr21-human37-'+adaptorConfigs[0]+'.sam'),
               pjoin(odir, 'evidenceDnbs-chr21-human37-'+adaptorConfigs[0]+'.sam'),
               [ '@PG\t', '@SQ\t' ]
)

tc.textCompare(pjoin(idir, 'conv/evidence2sam1/evidenceDnbs-chr1-human37-'+adaptorConfigs[1]+'.sam'),
               pjoin(odir, 'evidenceDnbs-chr1-human37-'+adaptorConfigs[1]+'.sam'),
               [ '@PG\t', '@SQ\t' ]
)

