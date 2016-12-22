#!/usr/bin/env python
import os, sys
import optparse

from prepLib import openFileByExtension,copyHeader 

parser = optparse.OptionParser("usage: %prog [options]")
parser.add_option("-m", "--input-maps", dest="inputMap",
                  help="input mappings file")
parser.add_option("-r", "--input-reads", dest="inputRead",
                  help="input reads file")
parser.add_option("-o", "--output-prefix", dest="outputPrefix",
                  help="output prefix to be added to extend the input names")
parser.add_option("-k", "--keep-range", dest="keepRange", default=None,
                  help="the genomic reference range (half-open) to keep the reads & mappings from in the format: chr1,500,700")
(options, args) = parser.parse_args()
if len(args) != 0:
    parser.error("unexpected arguments: "+args)

keepChr = None
keepFrom = None
keepTo = None

if options.keepRange:
    (keepChr,keepFrom,keepTo) = options.keepRange.split(",")

keepFrom = int(keepFrom)
keepTo = int(keepTo)
    
reads = openFileByExtension(options.inputRead)
maps = openFileByExtension(options.inputMap)

oreads = openFileByExtension(options.outputPrefix+os.path.basename(options.inputRead),'w')
omaps = openFileByExtension(options.outputPrefix+os.path.basename(options.inputMap),'w')

copyHeader(reads, oreads)
copyHeader(maps, omaps)

defReadRecord = '5'+'\t'+'A'*70+'\t'+chr(33)*70

readNo = -1
for line in reads:
    readNo += 1
    line = line.rstrip()
    readFlags = int(line[0])
    if (readFlags & 0x03) != 0 and (readFlags & 0x0C) != 0:
        print >>oreads, defReadRecord
        continue
    selectedMaps = []
    indexPointers = {}
    index = -1
    for map in maps:
        index += 1
        map = map.rstrip()
        mapFields = map.split('\t') 
        if not keepChr:
            selectedMaps.append(mapFields)
        else:
            pos = int(mapFields[2])
            if mapFields[1]==keepChr and pos>=keepFrom and pos<keepTo:  
                selectedMaps.append(mapFields)
                indexPointers[index] = len(selectedMaps)-1
        if int(mapFields[0]) & 0x01 == 1:
            break
    if len(selectedMaps):
        selectedMaps[-1][0] = str(int(selectedMaps[-1][0]) | 0x01)
        
        #update mate indices in reduced list
        index = -1
        for map in selectedMaps:
            index+=1
            # mate is outside the range - remove mate
            if indexPointers.has_key(int(map[7])):
                map[7]=indexPointers[int(map[7])]
            else:
                map[7]=index
        
        #find and fix brocken pairs
        index = -1
        for map in selectedMaps:
            index+=1
            if map[7]!=index:
                other = selectedMaps[map[7]]
                if other[7]==map[7]:
                    other[7]=index
                
            map[7]=str(map[7])
            print >>omaps, '\t'.join(map)
                
        print >>oreads, line
    else:
        print >>oreads, defReadRecord
    
reads.close()
maps.close()

oreads.close()
omaps.close()
