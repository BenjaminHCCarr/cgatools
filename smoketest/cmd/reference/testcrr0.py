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

from tutil.tucore import randomBases, generateChr, writeFasta

def strToMixedCase(ss):
    result = []
    for ch in ss:
        if 0 == random.randint(0,1):
            result.append(ch.lower())
        else:
            result.append(ch)
    return ''.join(result)

def toMixedCase(chrs):
    result = []
    for (name,bases) in chrs:
        result.append( (name, strToMixedCase(bases)) )
    return result

random.seed(12345)

chrs = []
chrs.append( ('chr1', generateChr([10, 'N' * 10, 40, 'N' * 50, 1000, 'N' * 100]) ) )
chrs.append( ('chrM', generateChr(['N' * 100, 500, 'R' * 2, 30, 'M', 'Y' * 7, 500]) ) )

writeFasta(pjoin(odir, 'tmp_upper.fa'), chrs)
writeFasta(pjoin(odir, 'tmp_mixedcase.fa'), toMixedCase(chrs))

tc.runCommand([ cgatools, 'fasta2crr',
                '--input='+pjoin(odir, 'tmp_mixedcase.fa'),
                '--output='+pjoin(odir, 'tmp.crr') ])

tc.runCommand([ cgatools, 'crr2fasta',
                '--input='+pjoin(odir, 'tmp.crr'),
                '--output='+pjoin(odir, 'tmp_after.fa') ])

tc.textCompare(pjoin(odir, 'tmp_upper.fa'),
               pjoin(odir, 'tmp_after.fa'))

actual = tc.getCommandStdout([ cgatools, 'listcrr',
                               '--reference='+pjoin(odir, 'tmp.crr'), ])
actual = actual.replace('\r\n', '\n')
expected = '''ChromosomeId Chromosome    Length Circular Md5
           0       chr1      1210    false ae01b065d40ecd38c80184a082f53cdf
           1       chrM      1140     true acdee758f0fa4a7cf91b66e1df472f0d
'''
if actual != expected:
    print actual,len(actual)
    print expected,len(expected)
    raise 'listcrr failed'

actual = tc.getCommandStdout([ cgatools, 'listcrr',
                               '--mode=contig',
                               '--reference='+pjoin(odir, 'tmp.crr'), ])
actual = actual.replace('\r\n', '\n')
expected = '''ChromosomeId Chromosome   Type    Offset    Length
           0       chr1 CONTIG         0        60
           0       chr1    GAP        60        50
           0       chr1 CONTIG       110      1000
           0       chr1    GAP      1110       100
           1       chrM    GAP         0       100
           1       chrM CONTIG       100      1040
'''
if actual != expected:
    raise 'listcrr contig failed'

actual = tc.getCommandStdout([ cgatools, 'listcrr',
                               '--mode=ambiguity',
                               '--reference='+pjoin(odir, 'tmp.crr'), ])
actual = actual.replace('\r\n', '\n')
expected = '''ChromosomeId Chromosome Code    Offset    Length
           0       chr1    N        10        10
           0       chr1    N        60        50
           0       chr1    N      1110       100
           1       chrM    N         0       100
           1       chrM    R       600         2
           1       chrM    M       632         1
           1       chrM    Y       633         7
'''
if actual != expected:
    raise 'listcrr contig failed'

actual = tc.getCommandStdout([ cgatools, 'decodecrr',
                               '--range=chr1,100,150',
                               '--reference='+pjoin(odir, 'tmp.crr'), ])
actual = actual.replace('\r\n', '\n')
expected = '''NNNNNNNNNNGTAATTAAGACAGATTTTCGTCAGCGCTATGGAATAACGG
'''
if actual != expected:
    raise 'decodecrr failed'

# test circular contig wraparound
actual = tc.getCommandStdout([ cgatools, 'decodecrr',
                               '--range=chrM,1100,1150',
                               '--reference='+pjoin(odir, 'tmp.crr'), ])
actual = actual.replace('\r\n', '\n')
expected = '''CGGTACTAACAACGGTCCTAGATATCTGTAGGCGGTTTCANNNNNNNNNN
'''
if actual != expected:
    raise 'decodecrr failed'
