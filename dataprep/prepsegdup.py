#!/usr/bin/env python
import os, sys, time

from prepLib import openFileByExtension

if len(sys.argv) != 3:
    print 'usage: <input> "NCBI build 36"'
    raise SystemExit(1)

chroms = {}
f = openFileByExtension(sys.argv[1])
f.readline() # header
for line in f:
    chrom, start, end = line.split('\t')[1:4]
    chrom = chrom.strip()
    start = int(start)
    end = int(end)
    if chrom in chroms:
        chroms[chrom] += [(start, False), (end, True)]
    else:
        chroms[chrom] = [(start, False), (end, True)]

print '#TYPE\tUCSC-SEGDUP'
print '#GENERATED_AT\t' + time.strftime('%Y-%b-%d %H:%M')
print '#GENOME_REFERENCE\t' + sys.argv[2]
print
print '>chromosome\tbegin\tend\tcount'

cleanChroms = [ 'chr' + str(i) for i in range(1, 23) ] + ['chrX', 'chrY']
for c in cleanChroms:
    if c not in chroms:
        continue
    data = chroms[c]
    last = -1
    count = 0
    data.sort()

    for coord, isend in data:
        if count != 0 and last != coord:
            print '%s\t%d\t%d\t%d' % (c, last, coord, count)
        last = coord
        if isend:
            count -= 1
        else:
            count += 1
    assert count == 0
