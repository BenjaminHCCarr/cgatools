#!/usr/bin/env python
import os, sys, time

'''
Common functions used by data preparation utilities
'''

def openFileByExtension(fn, operation='r'):
    '''Open a text file that may be compressed.

    Returns the file-like object of appropriate type depending on the file
    extension (.bz2 and .gz are supported).'''

    _, ext = os.path.splitext(fn)
    if ext == '.bz2':
        import bz2
        return bz2.BZ2File(fn, operation, 2*1024*1024)
    elif ext == '.gz':
        import gzip
        return gzip.GzipFile(fn, operation)
    else:
        return open(fn, operation)

def copyHeader(ifile, ofile):
    '''
    Copy CGI-specific file header line-by-line and leave the files open with the seek position right after the header
    ''' 
    for line in ifile:
        print >>ofile,line.rstrip()
        if line[0]=='>':
            return
        if line[0]!='#' and line[0]!='\n':
            break 
    raise Error('Invalid file format:'+ifile)

