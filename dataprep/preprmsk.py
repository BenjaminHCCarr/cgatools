#!/usr/bin/env python
import os, sys, time

from prepLib import openFileByExtension

if len(sys.argv) != 3:
    print 'usage: <input> "NCBI build 36"'
    raise SystemExit(1)

print '#TYPE\tUCSC-REPMASK'
print '#GENERATED_AT\t' + time.strftime('%Y-%b-%d %H:%M')
print '#GENOME_REFERENCE\t' + sys.argv[2]
print
print '>chromosome\tbegin\tend\trepName\trepFamily\tdivergence\tstrand'

cleanChroms = [ 'chr' + str(i) for i in range(1, 23) ] + ['chrX', 'chrY']

chroms = {}
for x in cleanChroms:
    chroms[x] = []

f = openFileByExtension(sys.argv[1])
f.readline() # header
for line in f:
    tokens = line.split('\t')
    d1, d2, d3, chrom, start, end = tokens[2:8]
    strand = tokens[9]
    repname = tokens[10].strip()
    repfam = tokens[12].strip()
    chrom = chrom.strip()
    start = int(start)
    end = int(end)
    div = int(d1) + int(d2) + int(d3)

    if chrom in chroms:
        chroms[chrom] += [(start, False, repname, repfam, div, strand),
                          (end, True, repname, repfam, div, strand)]

for c in cleanChroms:
    if c not in chroms:
        continue
    data = chroms[c]
    last = -1
    active = list()
    data.sort()

    for coord, isend, repname, repfam, div, strand in data:
        if active and last != coord:
            adiv, arepname, arepfam = sorted(active)[0]
            print '%s\t%d\t%d\t%s\t%s\t%.1f\t%s' % (c, last, coord, arepname, arepfam, float(adiv)/10.0, strand)
        last = coord
        t = (div, repname, repfam)
        if isend:
            active.remove(t)
        else:
            active.append(t)
    assert not active

