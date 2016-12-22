#!/usr/bin/env python
import os, sys, time

from prepLib import openFileByExtension

if len(sys.argv) != 3:
    print 'usage: <input> "NCBI build 36"'
    raise SystemExit(1)

print '#TYPE\tGENE-DATA'
print '#GENERATED_AT\t' + time.strftime('%Y-%b-%d %H:%M')
print '#GENOME_REFERENCE\t' + sys.argv[2]
print
print '>mrnaAcc\tproteinAcc\tgeneId\tsymbol\torientation\tchromosome\t' + \
      'genomeStart\tgenomeEnd\texonCount\texonStarts\texonEnds\t' + \
      'genomeCdsStart\tgenomeCdsEnd'

# Prepare chromosome translation table
CHR_TRANS = {}
for i in range(1, 23):
    CHR_TRANS[str(i)] = 'chr' + str(i)
for i in 'XY':
    CHR_TRANS[i] = 'chr' + i
# For now, ignore "genes" on the mitochondrion
# CHR_TRANS['MT'] = 'chrM'

f = openFileByExtension(sys.argv[1])
for line in f:
    if line[0] == '#' or line.startswith('mrnaAcc') or not line.strip():
        continue # headers
    tokens = line.split(',')
    #if tokens[19] == 'PARSE-ERROR':
    #    continue

    mrnaAcc, proteinAcc, geneId, symbol, \
    orientation, exonCount, chromosome, exonStarts, exonEnds, \
    genomeCdsStart, genomeCdsEnd = tokens[:11]

    if chromosome not in CHR_TRANS:
        continue # not a contig that made it into final reference
    chromosome = CHR_TRANS[chromosome]
    if orientation not in '-+':
        raise RuntimeError('bad orientation: ' + line)
    # Note that the source file uses closed, 1-base coordinates which
    # are converted below to half-open, zero based coordinates
    exonCount = int(exonCount)
    exonStarts = [ int(x) - 1 for x in exonStarts.split(';') ]
    exonEnds = [ int(x) for x in exonEnds.split(';') ]
    genomeCdsStart = int(genomeCdsStart) - 1
    genomeCdsEnd = int(genomeCdsEnd)
    if exonCount != len(exonStarts) or exonCount != len(exonEnds):
        raise RuntimeError('bad exon list: ' + line)
    if genomeCdsStart < 0 and genomeCdsEnd != -1:
        raise RuntimeError('bad CDS range: ' + line)
    if genomeCdsStart < 0:
        genomeCdsStart = 1
        genomeCdsEnd = 0

    print '%s\t%s\t%s\t%s\t%s\t%s\t%d\t%d\t%d\t%s\t%s\t%d\t%d' % (
        mrnaAcc, proteinAcc, geneId, symbol, orientation, chromosome,
        exonStarts[0], exonEnds[-1], exonCount,
        ';'.join([str(i) for i in exonStarts]),
        ';'.join([str(i) for i in exonEnds]),
        genomeCdsStart, genomeCdsEnd )
