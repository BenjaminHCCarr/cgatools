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

errors = [ ]
for always in [ True,False ]:
    cmdFlags = []
    oFlag = ''
    if always:
        cmdFlags = [ '-a' ]
        oFlag = '-always'
    for oMode in [ 'full','compact', 'compact-pct' ]:
        fn = 'join-'+oMode+oFlag+'.tsv'
        tc.runCommand([ cgatools, 'join',
                        '--beta',
                        '-m', oMode,
                        '--match=chromosome:chromosome',
                        '--overlap=begin,end:begin,end',
                        pjoin(idir, 'misc', 'join0', 'var.tsv'),
                        pjoin(idir, 'misc', 'join0', 'ann.tsv'),
                        '--output='+pjoin(odir, fn) ] +
                      cmdFlags)
        p1 = pjoin(idir, 'misc', 'join0', fn)
        p2 = pjoin(odir, fn)
        try:
            tc.textCompare(p1 , p2)
        except:
            print 'compared failed: ',p1,p2
            errors.append(fn)

if len(errors) > 0:
    raise Exception('errors: '+' '.join(errors))
