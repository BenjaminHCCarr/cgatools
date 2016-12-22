// Copyright 2010 Complete Genomics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you
// may not use this file except in compliance with the License. You
// may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
// implied. See the License for the specific language governing
// permissions and limitations under the License.

//! @file cgatoolscommandmap.hpp

#include "cgatools/core.hpp"

CGA_COMMAND_MAP("help",           HelpCommand)
CGA_COMMAND_MAP("man",            ManCommand)
CGA_COMMAND_MAP("fasta2crr",      cgatools::command::Fasta2Crr)
CGA_COMMAND_MAP("crr2fasta",      cgatools::command::Crr2Fasta)
CGA_COMMAND_MAP("listcrr",        cgatools::command::ListCrr)
CGA_COMMAND_MAP("decodecrr",      cgatools::command::DecodeCrr)
CGA_COMMAND_MAP("snpdiff",        cgatools::command::SnpDiff)
CGA_COMMAND_MAP("calldiff",       cgatools::command::CallDiff)
CGA_COMMAND_MAP("listvariants",   cgatools::command::ListVariants)
CGA_COMMAND_MAP("testvariants",   cgatools::command::TestVariants)
CGA_COMMAND_MAP("map2sam",        cgatools::command::Map2Sam)
#if CGA_TOOLS_IS_PIPELINE
CGA_COMMAND_MAP("mergedmap2sam",  cgatools::command::MergedMap2Sam)
#endif // CGA_TOOLS_IS_PIPELINE
CGA_COMMAND_MAP("evidence2sam",   cgatools::command::Evidence2Sam)
#if CGA_TOOLS_IS_PIPELINE
CGA_COMMAND_MAP("evidence2cache", cgatools::command::Evidence2Cache)
CGA_COMMAND_MAP("sam2reads",      cgatools::command::Sam2Reads)
#endif // CGA_TOOLS_IS_PIPELINE
CGA_COMMAND_MAP("join",           cgatools::command::Join)
CGA_COMMAND_MAP("junctiondiff",   cgatools::command::JunctionDiff)
#if CGA_TOOLS_IS_PIPELINE
CGA_COMMAND_MAP("junctioncmp",   cgatools::command::JunctionCmp)
#endif // CGA_TOOLS_IS_PIPELINE
CGA_COMMAND_MAP("junctions2events",   cgatools::command::JunctionType)
CGA_COMMAND_MAP("generatemastervar",     cgatools::command::VarFileCombine)
CGA_COMMAND_MAP("varfilter",      cgatools::command::VarFilter)
#if CGA_TOOLS_IS_PIPELINE
CGA_COMMAND_MAP("junction2vcf",   cgatools::command::Junction2Vcf)
#endif // CGA_TOOLS_IS_PIPELINE
CGA_COMMAND_MAP("mkvcf",          cgatools::command::MkVcf)
