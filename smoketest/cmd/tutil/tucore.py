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


import sys
from subprocess import Popen, PIPE, call
from glob import glob
from os.path import join as pjoin
import random
import os, shutil


def _tcerr(fn1, fn2, count, msg):
    raise Exception('compare %s %s failed near line %d: %s' % (fn1,fn2,count,msg))


def _tcopen(fn):
    try:
        if fn.endswith('.gz'):
            return Popen(['zcat',  fn], stdout=PIPE).stdout
        if fn.endswith('.bz2'):
            return Popen(['bzcat', fn], stdout=PIPE).stdout
        return open(fn)
    except Exception, ee:
        raise Exception('failed to open file: %s' % fn)


def _stripnewline(line):
    if line.endswith('\n'):
        if line.endswith('\r\n'):
            return line[:-2]
        return line[:-1]
    if line.endswith('\r'):
        return line[:-1]
    return line

BASES = [ 'A', 'C', 'G', 'T' ]

def randomBases(count):
    result = []
    for ii in xrange(count):
        result.append(BASES[random.randint(0, 3)])
    return ''.join(result)

def generateChr(seqList):
    result = []
    for seq in seqList:
        if type(seq) == str:
            result.append(seq)
        else:
            result.append(randomBases(seq))
    return ''.join(result)

def writeFasta(fn, chrs):
    ff = open(fn, 'w')
    for (name,bases) in chrs:
        ff.write('>'+name+'\n')
        pos = 0
        while pos < len(bases):
            posEnd = min(pos+50, len(bases))
            ff.write(bases[pos:posEnd]+'\n')
            pos += 50
    ff.close()


def readLine(ff, ignorePrefixes):
    while True:
        ll = ff.readline()
        if '' == ll:
            return ll
        pfxMatch = False
        for pfx in ignorePrefixes:
            if ll.startswith(pfx):
                pfxMatch = True
        if not pfxMatch:
            return ll


def textCompare(fn1, fn2, ignorePrefixes = []):
    print 'cmp %s %s' % (fn1, fn2)

    # Try fast check for equality.
    try:
        runCommand(['zcmp', fn1, fn2])
        return
    except:
        pass

    ff1 = _tcopen(fn1)
    ff2 = _tcopen(fn2)
    count = 0
    while True:
        l1 = readLine(ff1, ignorePrefixes)
        l2 = readLine(ff2, ignorePrefixes)
        count += 1
        if l1 != l2:
            if '' == l1:
                _tcerr(fn1, fn2, count, '%s ends before %s' % (fn1, fn2))
            if '' == l2:
                _tcerr(fn1, fn2, count, '%s ends before %s' % (fn2, fn1))
            if _stripnewline(l1) != _stripnewline(l2):
                _tcerr(fn1, fn2, count, 'lines differ:\n< %s\n> %s' %
                       (_stripnewline(l1),_stripnewline(l2)))
        if '' == l1 and '' == l2:
            break
    ff1.close()
    ff2.close()


def runCommand(args):
    if ( len(args) > 1 and
         os.getenv('CGATOOLS_IS_PIPELINE') and
         'cgatools' == os.path.basename(args[0]) ):
        args = [ args[0], args[1], '--pipeline-version-string=1.1.1.1' ] + args[2:]
    pretty = '"' + '" "'.join(args) + '"'
    print
    print pretty
    print
    sys.stdout.flush()
    sys.stderr.flush()
    sts = call(args)
    if sts != 0:
        raise Exception('command execution failed: '+pretty)


def getCommandStdout(args, input=''):
    pretty = '"' + '" "'.join(args) + '"'
    print
    print pretty
    print
    sys.stdout.flush()
    sys.stderr.flush()
    return Popen(args, stdin=PIPE, stdout=PIPE).communicate(input)[0]

def glob1(arg):
    result = glob(arg)
    if len(result) != 1:
        raise Exception('for pattern %s: found %d results, expected 1' % (arg,len(result)))
    return result[0]

class DelimitedFileMetadata:
    def __init__(self):
        self.kv = []

    def add(self, key, value):
        self.kv.append( (key,value) )

    def hasKey(self, key):
        for (kk,vv) in self.kv:
            if key == kk:
                return True
        return False

    def get(self, key):
        for (kk,vv) in self.kv:
            if key == kk:
                return vv
        raise Exception('metadata key not found: '+key)

    def getFormatVersion(self):
        if self.hasKey('FORMAT_VERSION'):
            version = self.get('FORMAT_VERSION')
        elif self.hasKey('VERSION'):
            version = self.get('VERSION')
        else:
            raise Exception('file has no version header')
        dot = version.find('.')
        if -1 == dot:
            raise Exception('file contains bad version header value: '+version)
        major = int(version[:dot])
        minor = int(version[dot+1:])
        if minor > 1000 or minor < 0 or major < 0:
            raise Exception('file contains bad version header value: '+version)
        return major * 1000 + minor

    def getReferenceBuild(self):
        if self.getFormatVersion() < 1003:
            return '36'
        refBuild = self.get('GENOME_REFERENCE')
        if refBuild == 'NCBI build 36':
            return '36'
        if refBuild == 'NCBI build 37':
            return '37'
        raise Exception('failed to determine ref build')


def readMetadataHeader(fn):
    meta = DelimitedFileMetadata()
    ff = _tcopen(fn)
    while True:
        line = ff.readline()
        if '' == line:
            break
        line = _stripnewline(line)
        if '' == line:
            continue
        if '#' != line[0]:
            break
        line = line[1:]
        idxTab = line.find('\t')
        if -1 == idxTab:
            meta.add(line, '')
        else:
            meta.add(line[:idxTab], line[idxTab+1:])
    ff.close()
    return meta


def getReferenceBuildForRoot(root):
    fn = glob1(pjoin(root, 'ASM', 'var-*.tsv*'))
    return getReferenceBuildForFn(fn)


def getReferenceBuildForFn(fn):
    return readMetadataHeader(fn).getReferenceBuild()


def getFormatVersionForRoot(root):
    fn = glob1(pjoin(root, 'ASM', 'var-*.tsv*'))
    return getFormatVersionForFn(fn)


def getFormatVersionForFn(fn):
    return readMetadataHeader(fn).getFormatVersion()

def copytree(src, dst, symlinks=False, ignore=None):
    names = os.listdir(src)
    if ignore is not None:
        ignored_names = ignore(src, names)
    else:
        ignored_names = set()

    os.makedirs(dst)
    errors = []
    for name in names:
        if name in ignored_names:
            continue
        srcname = os.path.join(src, name)
        dstname = os.path.join(dst, name)
        try:
            if symlinks and os.path.islink(srcname):
                linkto = os.readlink(srcname)
                os.symlink(linkto, dstname)
            elif os.path.isdir(srcname):
                copytree(srcname, dstname, symlinks, ignore)
            else:
                shutil.copyfile(srcname, dstname)
        except (IOError, os.error), why:
            errors.append((srcname, dstname, str(why)))
        # catch the Error from the recursive copytree so that we can
        # continue with other files
        except shutil.Error, err:
            errors.extend(err.args[0])
