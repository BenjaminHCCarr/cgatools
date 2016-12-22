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
                '--input='+pjoin(idir, 'conv/evidence2sam0/TestRef.fa'),
                '--output='+pjoin(odir, 'TestRef.crr') ])

tc.runCommand( [ cgatools, 'evidence2sam',
                 '--beta',
                 '--evidence-dnbs='+pjoin(idir, 'conv/evidence2sam0/evidenceDnbs-chr22-male36_100.tsv'),
                 '--output='+pjoin(odir, 'evidenceDnbs-chr22-male36_100.sam'),
                 '--reference='+pjoin(odir, 'TestRef.crr'),
                 ] )
tc.textCompare(pjoin(idir, 'conv/evidence2sam0/evidenceDnbs-chr22-male36_100.sam'),
               pjoin(odir, 'evidenceDnbs-chr22-male36_100.sam'),
               [ '@PG\t', '@SQ\t' ] )
