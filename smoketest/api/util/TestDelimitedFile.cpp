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

#include <iostream>
#include <sstream>
#include <string>

using namespace std;
using namespace cgatools::util;

namespace cgatools { namespace {

    void delimitedFileExample()
    {
        // A test to ensure the example code in DelimitedFile.hpp
        // compiles.
        string chromosome;
        uint32_t offset;
        DelimitedFile df(cin, "STDIN");
        df.addField(StringField("chromosome", &chromosome));
        df.addField(ValueField<uint32_t>("offset", &offset));
        while (df.next())
            cout << chromosome << "\t" << offset << endl;
    }

    void doTestDelimitedFile()
    {
        // Basic straightforward test. Uses header best practice.
        {
            string str("#ASSEMBLY_ID\tt00\n"
                       "#FORMAT_VERSION\t0.6\n"
                       "\n"
                       ">chromosome\toffset\n"
                       "chr1\t123\n"
                       "chrX\t456\n");

            string chromosome;
            uint32_t offset;
            istringstream iss (str, istringstream::in);
            DelimitedFile df(iss, "teststream");
            CGA_ASSERT(df.getMetadata().get("ASSEMBLY_ID") == "t00");
            CGA_ASSERT(df.getMetadata().get("FORMAT_VERSION") == "0.6");
            df.addField(StringField("chromosome", &chromosome));
            df.addField(ValueField<uint32_t>("offset", &offset));
            CGA_ASSERT(df.next());
            CGA_ASSERT("chr1" == chromosome);
            CGA_ASSERT(123 == offset);
            CGA_ASSERT(df.next());
            CGA_ASSERT("chrX" == chromosome);
            CGA_ASSERT(456 == offset);
            CGA_ASSERT(!df.next());
        }

        // Duplicate header keys. No > in header. No blank line.
        {
            string str("#Key1\tval1_a\n"
                       "#Key1\tval1_b\n"
                       "chromosome\toffset\n"
                       "chr1\t1234\n"
                       "chrX\t4567\n");

            string chromosome;
            uint32_t offset;
            istringstream iss (str, istringstream::in);
            DelimitedFile df(iss, "teststream");
            CGA_ASSERT(df.getMetadata().get("Key1") == "val1_a");

            vector< pair<string,string> > kv;
            kv.push_back(make_pair("Key1", "val1_a"));
            kv.push_back(make_pair("Key1", "val1_b"));
            CGA_ASSERT(kv == df.getMetadata().getMap());

            df.addField(StringField("chromosome", &chromosome));
            df.addField(ValueField<uint32_t>("offset", &offset));
            CGA_ASSERT(df.next());
            CGA_ASSERT("chr1" == chromosome);
            CGA_ASSERT(1234 == offset);
            CGA_ASSERT(df.next());
            CGA_ASSERT("chrX" == chromosome);
            CGA_ASSERT(4567 == offset);
            CGA_ASSERT(!df.next());
        }

        // No header.
        {
            string str("chromosome\toffset\n"
                       "chr1\t12345\n"
                       "chrX\t45678\n");

            string chromosome;
            uint32_t offset;
            istringstream iss (str, istringstream::in);
            DelimitedFile df(iss, "teststream");
            vector< pair<string,string> > kv;
            CGA_ASSERT(kv == df.getMetadata().getMap());

            df.addField(StringField("chromosome", &chromosome));
            df.addField(ValueField<uint32_t>("offset", &offset));
            CGA_ASSERT(df.next());
            CGA_ASSERT("chr1" == chromosome);
            CGA_ASSERT(12345 == offset);
            CGA_ASSERT(df.next());
            CGA_ASSERT("chrX" == chromosome);
            CGA_ASSERT(45678 == offset);
            CGA_ASSERT(!df.next());
        }

        // Skip empty.
        {
            string str("ChromosomeId Chromosome    Length Circular Md5\n"
                       "0       chr1      1210    false ae01b065d40ecd38c80184a082f53cdf \n"
                       " 1       chrM      1140     true acdee758f0fa4a7cf91b66e1df472f0d\n");

            uint16_t chromosomeId;
            string chromosome;
            uint32_t length;
            string circular;
            string md5;
            istringstream iss (str, istringstream::in);
            DelimitedFile df(iss, "teststream", ' ', DelimitedLineParser::SKIP_EMPTY_FIELDS);
            vector< pair<string,string> > kv;
            CGA_ASSERT(kv == df.getMetadata().getMap());

            df.addField(ValueField<uint16_t>("ChromosomeId", &chromosomeId));
            df.addField(StringField("Chromosome", &chromosome));
            df.addField(ValueField<uint32_t>("Length", &length));
            df.addField(StringField("Circular", &circular));
            df.addField(StringField("Md5", &md5));
            CGA_ASSERT(df.next());
            CGA_ASSERT(0 == chromosomeId);
            CGA_ASSERT("chr1" == chromosome);
            CGA_ASSERT(1210 == length);
            CGA_ASSERT("false" == circular);
            CGA_ASSERT("ae01b065d40ecd38c80184a082f53cdf" == md5);
            CGA_ASSERT(df.next());
            CGA_ASSERT(1 == chromosomeId);
            CGA_ASSERT("chrM" == chromosome);
            CGA_ASSERT(1140 == length);
            CGA_ASSERT("true" == circular);
            CGA_ASSERT("acdee758f0fa4a7cf91b66e1df472f0d" == md5);
            CGA_ASSERT(!df.next());
        }
    }

} } // cgatools::anonymous

int TestDelimitedFile(int argc, char* argv[])
{
    cgatools::doTestDelimitedFile();

    return 0;
}
