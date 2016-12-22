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

def createJunctionFile(fn, data):
    META = [ ('FORMAT_VERSION', '1.5'),
             ('GENOME_REFERENCE', 'NCBI build 37'),
             ('SAMPLE', 'GS00258-DNA_H01'),
             ('TYPE', 'JUNCTIONS') ]
    HEADER = [  '>Id', 'LeftChr', 'LeftPosition', 'LeftStrand', 'LeftLength',
                'RightChr', 'RightPosition', 'RightStrand', 'RightLength',
                'StrandConsistent', 'Interchromosomal', 'Distance',
                'DiscordantMatePairAlignments', 'JunctionSequenceResolved',
                'TransitionSequence', 'TransitionLength', 'LeftRepeatClassification',
                'RightRepeatClassification', 'LeftGenes', 'RightGenes', 'XRef',
                'DeletedTransposableElement', 'KnownUnderrepresentedRepeat',
                'FrequencyInBaselineGenomeSet', 'AssembledSequence' ]
    f = open(fn, 'w')
    for m in META:
        print >>f, "#%s\t%s" % m
    print >>f, '\t'.join(HEADER)
    cid = 1
    for lchr, lpos, lstrand, rchr, rpos, rstrand in data:
        distance = ''
        if rchr == lchr:
            distance = str(rpos - lpos)
        line = [  cid,
                  lchr, lpos, (lstrand and '-' or '+'), 321,
                  rchr, rpos, (rstrand and '-' or '+'), 345,
                  (lstrand == rstrand) and 'Y' or 'N',
                  (lchr != rchr) and 'Y' or 'N',
                  distance,
                  42, 'Y', '', 0, '', '', '', '', '', '', '', '0.1', ''
                ]
        assert len(line) == len(HEADER)
        print >>f, '\t'.join([str(x) for x in line])
        cid += 1
    f.close()

TESTS = (
(
    ('chr1', 100, 0, 'chr1', 500, 0),   # del
    ('chr1', 2000, 1, 'chr1', 2500, 1), # tandemdup
    ('chr1', 4000, 0, 'chr1', 4500, 1), # lone inv
    ('chr1', 6000, 0, 'chr1', 6500, 1),
    ('chr1', 6002, 1, 'chr1', 6495, 0), # confirmed inv
    ('chr1', 8000, 0, 'chr1', 9000, 0),
    ('chr1', 8600, 0, 'chr1', 9600, 0),
    ('chr1', 10200, 0, 'chr1', 11200, 0), # mess
    ('chr1', 14000, 0, 'chr2', 1000, 0), # xchr
    ('chr1', 16000, 1, 'chr2', 3000, 1),
    ('chr1', 18000, 0, 'chr2', 2995, 0), # xloc
    ('chr1', 20000, 1, 'chr2', 4000, 0),
    ('chr1', 22000, 0, 'chr2', 3995, 1), # inv xloc
),
)

# Generate reference
random.seed(12345)
chrs = [ (x, generateChr([25000])) for x in ('chr1', 'chr2') ]
writeFasta(pjoin(odir, 'reference.fa'), chrs)
tc.runCommand([ cgatools, 'fasta2crr',
                '--input='+pjoin(odir, 'reference.fa'),
                '--output='+pjoin(odir, 'reference.crr') ])

# Run tests
for i, test in enumerate(TESTS):
    infn = pjoin(odir, 'junctions%d.tsv' % i)
    createJunctionFile(infn, test)
    tc.runCommand([ cgatools, 'junctions2events',
                    '--beta',
                    '--reference=' + pjoin(odir, 'reference.crr'),
                    '--junctions=' + infn,
                    '--repmask-data=' + pjoin(idir, 'junctions/junctions2events/repmask.tsv'),
                    '--gene-data=' + pjoin(idir, 'junctions/junctions2events/genes.tsv'),
                    '--output-prefix=' + pjoin(odir, 'out%d-' % i),
                  ])
    for outfn in ['Events.tsv','AnnotatedJunctions.tsv']:
        tc.textCompare(pjoin(idir, ('junctions/junctions2events/out%d-' % i) + outfn),
                       pjoin(odir, ('out%d-' % i) + outfn),
                       ['#GENERATED_AT','#CGATOOLS_VERSION',
                        '#SOFTWARE_VERSION','#FORMAT_VERSION'])
