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
import shutil

#os.system(command)
expDirName = 'GS19240-180-36-21-ASM'
shutil.rmtree(pjoin(odir, expDirName), True)
tc.copytree(pjoin(idir, 'common', expDirName),pjoin(odir, expDirName))
readsFile = pjoin(odir, expDirName, 'GS00028-DNA_C01','MAP','GS10364-FS3-L01','reads_GS10364-FS3-L01_001.tsv')
mappingsFile = pjoin(odir, expDirName, 'GS00028-DNA_C01','MAP','GS10364-FS3-L01','mapping_GS10364-FS3-L01_001.tsv') 
shutil.copy(pjoin(idir, 'conv','map2sam0','reads_GS10364-FS3-L01_001.tsv'), readsFile)
shutil.copy(pjoin(idir, 'conv','map2sam0','mapping_GS10364-FS3-L01_001.tsv'), mappingsFile)

tc.runCommand([ cgatools, 'fasta2crr',
                '--input='+pjoin(idir, 'conv','map2sam0','TestRef.fa'),
                '--output='+pjoin(odir, 'TestRef.crr'), 
                '--circular=chrM',
                ])

tc.runCommand( [ cgatools, 'map2sam',
                 '--reads='+readsFile,
                 '--mappings='+mappingsFile,
                 '--genome-root='+pjoin(odir, expDirName, 'GS00028-DNA_C01'),
                 '--reference='+pjoin(odir, 'TestRef.crr'),
                 '--output='+pjoin(odir, 'GS10364-FS3-L01_001.sam'),
                 '--cgi-bam-caveat-acknowledgement'
                 #'--skip-not-mapped',
                 ] )

tc.textCompare(pjoin(idir, 'conv/map2sam0/GS10364-FS3-L01_001.sam'),
               pjoin(odir, 'GS10364-FS3-L01_001.sam'),
               [ '@PG\tID:cgatools\t','@SQ\tSN:' ])
