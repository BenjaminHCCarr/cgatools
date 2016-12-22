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

#include "cgatools/core.hpp"
#include "cgatools/util/DelimitedFile.hpp"
#include "cgatools/util/Exception.hpp"
#include "cgatools/variants/Call.hpp"
#include "cgatools/variants/VariantFileIterator.hpp"
#include "cgatools/command/Fasta2Crr.hpp"

#include <iostream>
#include <sstream>
#include <string>

using namespace std;
using namespace cgatools::util;
using namespace cgatools::reference;
using namespace cgatools::variants;
using cgatools::command::Fasta2Crr;

void validateVariantFile(const CrrFile& crr,
                         const std::string& fn,
                         const std::string& msg)
{
    cout << "Looking for msg=\"" << msg << "\" in result for fn: " << fn << endl;
    try
    {
        VariantFileIterator locIt(crr);
        locIt.open(fn);
        while (!locIt.eof())
            ++locIt;
        CGA_ASSERT("" == msg);
    }
    catch(exception& ee)
    {
        cout << "Exception message: " << ee.what() << endl;
        CGA_ASSERT(msg != "");
        CGA_ASSERT(string::npos != string(ee.what()).find(msg));
        CGA_ASSERT(string::npos != string(ee.what()).find(fn));
    }
}

int TestCall(int argc, char* argv[])
{
    string srcDir = argv[1];
    string binDir = argv[2];

    // Basic straightforward test.
    {
        // make ref
        OutputStream out(binDir+"/TestCall.crr");
        CrrFileWriter writer(&out);
        writer.newChromosome("chr1", false);
        writer.addSequence("CATGACCCGCAAATCTGAAACTATCTGGCCCTTGGCAGGGGAAGTTTGCC"
                           "AACCCCTGCTCTAGAGAACCCTGAAACCAACCCTTGGCACCTGGGGTACT");
    }

    // Old call file (totalScore) parsing.
    {
        CrrFile crr(binDir+"/TestCall.crr");

        string str(">locus\tploidy\thaplotype\tchromosome\tbegin\tend\t"
                   "varType\treference\talleleSeq\ttotalScore\thapLink\txRef\n"
                   "1\t2\tall\tchr1\t0\t15\tno-call\t=\t?\t\t\t\n"
                   "2\t2\t1\tchr1\t15\t20\tsub\tTGAAA\tAA\t30\t123\tdbsnp:123\n"
                   "2\t2\t2\tchr1\t15\t20\tref\tTGAAA\tTGAAA\t40\t456\t\n"
                   "3\t?\tall\tchr1\t20\t30\tno-ref\t=\t?\t\t\t\n");

        istringstream iss (str, istringstream::in);
        DelimitedFile df(iss, "teststream");
        Call call;
        call.addFieldParsers(df, crr);
        CGA_ASSERT(df.next());
        CGA_ASSERT(1 == call.locus_);
        CGA_ASSERT(2 == call.ploidy_);
        CGA_ASSERT(Call::ALL_HAPLOTYPES == call.haplotype_);
        CGA_ASSERT(Range(0, 0, 15) == call.range_);
        CGA_ASSERT("no-call" == call.varType_);
        CGA_ASSERT("=" == call.reference_);
        CGA_ASSERT("CATGACCCGCAAATC" == call.refSequence(crr));
        CGA_ASSERT("?" == call.alleleSeq_);
        CGA_ASSERT("?" == call.calledSequence(crr));
        CGA_ASSERT(Call::EMPTY_SCORE == call.varScoreVAF_);
        CGA_ASSERT(Call::EMPTY_SCORE == call.varScoreEAF_);
        CGA_ASSERT(0 == call.varFilter_.size());
        CGA_ASSERT("" == call.hapLink_);
        CGA_ASSERT("" == call.xRef_);

        CGA_ASSERT(df.next());
        CGA_ASSERT(2 == call.locus_);
        CGA_ASSERT(2 == call.ploidy_);
        CGA_ASSERT(1 == call.haplotype_);
        CGA_ASSERT(Range(0, 15, 20) == call.range_);
        CGA_ASSERT("sub" == call.varType_);
        CGA_ASSERT("TGAAA" == call.reference_);
        CGA_ASSERT("TGAAA" == call.refSequence(crr));
        CGA_ASSERT("AA" == call.alleleSeq_);
        CGA_ASSERT("AA" == call.calledSequence(crr));
        CGA_ASSERT(30 == call.varScoreVAF_);
        CGA_ASSERT(30 == call.varScoreEAF_);
        CGA_ASSERT(0 == call.varFilter_.size());
        CGA_ASSERT("123" == call.hapLink_);
        CGA_ASSERT("dbsnp:123" == call.xRef_);

        CGA_ASSERT(df.next());
        CGA_ASSERT(2 == call.locus_);
        CGA_ASSERT(2 == call.ploidy_);
        CGA_ASSERT(2 == call.haplotype_);
        CGA_ASSERT(Range(0, 15, 20) == call.range_);
        CGA_ASSERT("ref" == call.varType_);
        CGA_ASSERT("TGAAA" == call.reference_);
        CGA_ASSERT("TGAAA" == call.refSequence(crr));
        CGA_ASSERT("TGAAA" == call.alleleSeq_);
        CGA_ASSERT("TGAAA" == call.calledSequence(crr));
        CGA_ASSERT(40 == call.varScoreVAF_);
        CGA_ASSERT(40 == call.varScoreEAF_);
        CGA_ASSERT(0 == call.varFilter_.size());
        CGA_ASSERT("456" == call.hapLink_);
        CGA_ASSERT("" == call.xRef_);

        CGA_ASSERT(df.next());
        CGA_ASSERT(3 == call.locus_);
        CGA_ASSERT(Call::UNKNOWN_PLOIDY == call.ploidy_);
        CGA_ASSERT(Call::ALL_HAPLOTYPES == call.haplotype_);
        CGA_ASSERT(Range(0, 20, 30) == call.range_);
        CGA_ASSERT("no-ref" == call.varType_);
        CGA_ASSERT("=" == call.reference_);
        CGA_ASSERT("CTATCTGGCC" == call.refSequence(crr));
        CGA_ASSERT("?" == call.alleleSeq_);
        CGA_ASSERT("?" == call.calledSequence(crr));
        CGA_ASSERT(Call::EMPTY_SCORE == call.varScoreVAF_);
        CGA_ASSERT(Call::EMPTY_SCORE == call.varScoreEAF_);
        CGA_ASSERT(0 == call.varFilter_.size());
        CGA_ASSERT("" == call.hapLink_);
        CGA_ASSERT("" == call.xRef_);

        CGA_ASSERT(!df.next());
    }

    // New call file (varScoreVAF, varScoreEAF, varQuality) parsing.
    {
        CrrFile crr(binDir+"/TestCall.crr");

        string str(">locus\tploidy\thaplotype\tchromosome\tbegin\tend\t"
                   "varType\treference\talleleSeq\tvarScoreVAF\tvarScoreEAF\tvarQuality\t"
                   "hapLink\txRef\n"
                   "1\t2\tall\tchr1\t0\t15\tno-call\t=\t?\t\t\t\t\t\n"
                   "2\t2\t1\tchr1\t15\t20\tsub\tTGAAA\tAA\t25\t30\tVQLOW;AMB\t123\tdbsnp:123\n"
                   "2\t2\t2\tchr1\t15\t20\tref\tTGAAA\tTGAAA\t50\t40\tVQHIGH\t456\t\n"
                   "3\t?\tall\tchr1\t20\t30\tno-ref\t=\t?\t\t\t\t\t\n");

        istringstream iss (str, istringstream::in);
        DelimitedFile df(iss, "teststream");
        Call call;
        call.addFieldParsers(df, crr);
        CGA_ASSERT(df.next());
        CGA_ASSERT(1 == call.locus_);
        CGA_ASSERT(2 == call.ploidy_);
        CGA_ASSERT(Call::ALL_HAPLOTYPES == call.haplotype_);
        CGA_ASSERT(Range(0, 0, 15) == call.range_);
        CGA_ASSERT("no-call" == call.varType_);
        CGA_ASSERT("=" == call.reference_);
        CGA_ASSERT("CATGACCCGCAAATC" == call.refSequence(crr));
        CGA_ASSERT("?" == call.alleleSeq_);
        CGA_ASSERT("?" == call.calledSequence(crr));
        CGA_ASSERT(Call::EMPTY_SCORE == call.varScoreVAF_);
        CGA_ASSERT(Call::EMPTY_SCORE == call.varScoreEAF_);
        CGA_ASSERT(0 == call.varFilter_.size());
        CGA_ASSERT("" == call.hapLink_);
        CGA_ASSERT("" == call.xRef_);

        CGA_ASSERT(df.next());
        CGA_ASSERT(2 == call.locus_);
        CGA_ASSERT(2 == call.ploidy_);
        CGA_ASSERT(1 == call.haplotype_);
        CGA_ASSERT(Range(0, 15, 20) == call.range_);
        CGA_ASSERT("sub" == call.varType_);
        CGA_ASSERT("TGAAA" == call.reference_);
        CGA_ASSERT("TGAAA" == call.refSequence(crr));
        CGA_ASSERT("AA" == call.alleleSeq_);
        CGA_ASSERT("AA" == call.calledSequence(crr));
        CGA_ASSERT(25 == call.varScoreVAF_);
        CGA_ASSERT(30 == call.varScoreEAF_);
        CGA_ASSERT(2 == call.varFilter_.size() &&
                   call.varFilter_[0] == "VQLOW" &&
                   call.varFilter_[1] == "AMB");
        CGA_ASSERT("123" == call.hapLink_);
        CGA_ASSERT("dbsnp:123" == call.xRef_);

        CGA_ASSERT(df.next());
        CGA_ASSERT(2 == call.locus_);
        CGA_ASSERT(2 == call.ploidy_);
        CGA_ASSERT(2 == call.haplotype_);
        CGA_ASSERT(Range(0, 15, 20) == call.range_);
        CGA_ASSERT("ref" == call.varType_);
        CGA_ASSERT("TGAAA" == call.reference_);
        CGA_ASSERT("TGAAA" == call.refSequence(crr));
        CGA_ASSERT("TGAAA" == call.alleleSeq_);
        CGA_ASSERT("TGAAA" == call.calledSequence(crr));
        CGA_ASSERT(50 == call.varScoreVAF_);
        CGA_ASSERT(40 == call.varScoreEAF_);
        CGA_ASSERT(1 == call.varFilter_.size() && call.varFilter_[0] == "PASS");
        CGA_ASSERT("456" == call.hapLink_);
        CGA_ASSERT("" == call.xRef_);

        CGA_ASSERT(df.next());
        CGA_ASSERT(3 == call.locus_);
        CGA_ASSERT(Call::UNKNOWN_PLOIDY == call.ploidy_);
        CGA_ASSERT(Call::ALL_HAPLOTYPES == call.haplotype_);
        CGA_ASSERT(Range(0, 20, 30) == call.range_);
        CGA_ASSERT("no-ref" == call.varType_);
        CGA_ASSERT("=" == call.reference_);
        CGA_ASSERT("CTATCTGGCC" == call.refSequence(crr));
        CGA_ASSERT("?" == call.alleleSeq_);
        CGA_ASSERT("?" == call.calledSequence(crr));
        CGA_ASSERT(Call::EMPTY_SCORE == call.varScoreVAF_);
        CGA_ASSERT(Call::EMPTY_SCORE == call.varScoreEAF_);
        CGA_ASSERT(0 == call.varFilter_.size());
        CGA_ASSERT("" == call.hapLink_);
        CGA_ASSERT("" == call.xRef_);

        CGA_ASSERT(!df.next());
    }

    // Some negative tests.
    string reference(binDir+"/TestRef.crr");
    {
        // make ref
        string cmd("cgatools");
        string arg0("fasta2crr");
        string inputRefStr("--input="+srcDir+"/variants/TestRef.fa");
        string outputRefStr("--output="+reference);
        char* args[] =
        {
            &cmd[0],
            &arg0[0],
            &inputRefStr[0],
            &outputRefStr[0],
            0
        };
        Fasta2Crr f2c(arg0);
        if (0 != f2c(sizeof(args)/sizeof(args[0])-1, args))
            return 1;
    }

    {
        CrrFile crr(binDir+"/TestRef.crr");

        // Start with a basic test that should work.
        validateVariantFile(crr, srcDir+"/variants/var-00.tsv", "");
        validateVariantFile(crr, srcDir+"/variants/var-01.tsv",
                            "missing required field: locus");
        validateVariantFile(crr, srcDir+"/variants/var-02.tsv",
                            "Not all haplotypes of locus have calls:\n"
                            "1\t2\t1\tchr1\t");
        validateVariantFile(crr, srcDir+"/variants/var-03.tsv",
                            "failed to parse int: chr1: 1\t1\tchr1");
        validateVariantFile(crr, srcDir+"/variants/var-04.tsv",
                            "failed to parse line: not enough fields: 1\t2\t1\tchr1");
        validateVariantFile(crr, srcDir+"/variants/var-05.tsv",
                            "var-05.tsv: Variant file ends before chromosome ends:\n"
                            "2\t2\tall\tchr1");
        validateVariantFile(crr, srcDir+"/variants/var-06.tsv",
                            "var-06.tsv: Not all reference bases covered by calls:\n"
                            "4\t2\tall\tchr1");
        validateVariantFile(crr, srcDir+"/variants/var-07.tsv",
                            "var-07.tsv: Locus has a call for all haplotypes and multiple calls:\n"
                            "2\t2\tall\tchr1");
        validateVariantFile(crr, srcDir+"/variants/var-08.tsv",
                            "var-08.tsv: Call reference sequence does not match reference:\n"
                            "1\t2\t1\tchr1");

        // Do we need more test cases? I've basically tested the
        // exception handling framework, validating that the file name
        // is in the error message, as well as a description and the
        // call or calls of interest.
    }

    return 0;
}
