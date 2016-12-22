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
#include "cgatools/util/BaseUtil.hpp"
#include "cgatools/util/Exception.hpp"

#include <iostream>
#include <string>
#include <vector>

#include <boost/array.hpp>

using namespace std;
using namespace cgatools::util;
using boost::array;

namespace bu = baseutil;

class MyBaseUtil
{
public:
    static bool isValidBase(char base)
    {
        switch(toupper(base))
        {
        case 'A': case 'C': case 'G': case 'T':
            return true;
        default:
            return false;
        }
    }

    static bool isValidIupacCode(char base)
    {
        switch(toupper(base))
        {
        case 'A':
        case 'C':
        case 'G':
        case 'T':
        case 'R':
        case 'Y':
        case 'M':
        case 'K':
        case 'W':
        case 'S':
        case 'B':
        case 'D':
        case 'H':
        case 'V':
        case 'N':
            return true;
        default:
            return false;
        }
    }
};

void testConsistency(const char* lhs, const char* rhs, bool expected)
{
    cout << "compare: " << lhs << " " << rhs
         << "expected: " << (expected ? "true" : "false") << endl;
    CGA_ASSERT(bu::isConsistent(lhs, rhs) == expected);
    CGA_ASSERT(bu::isConsistent(rhs, lhs) == expected);
    CGA_ASSERT(bu::isConsistent(lhs, lhs));
    CGA_ASSERT(bu::isConsistent(rhs, rhs));
}

int TestBaseUtil(int argc, char* argv[])
{
    vector<string> baseNConsistent;
    baseNConsistent.push_back("AA");
    baseNConsistent.push_back("CC");
    baseNConsistent.push_back("GG");
    baseNConsistent.push_back("TT");
    baseNConsistent.push_back("AN");
    baseNConsistent.push_back("CN");
    baseNConsistent.push_back("GN");
    baseNConsistent.push_back("NT");
    baseNConsistent.push_back("NN");

    array<int, 256> compatCount, complementCount;
    compatCount.assign(0);
    complementCount.assign(0);
    for(int ii=0; ii<256; ii++)
    {
        CGA_ASSERT(bu::isValidBase(ii) == MyBaseUtil::isValidBase(ii));
        CGA_ASSERT(bu::isValidIupacCode(ii) == MyBaseUtil::isValidIupacCode(ii));

        if (bu::isValidBase(ii))
        {
            CGA_ASSERT(bu::unpack(bu::pack(ii)) == toupper(ii));
            CGA_ASSERT(bu::isValidBase(bu::complement(ii)));
            CGA_ASSERT(bu::complement(ii) != ii);
        }

        if (bu::isValidIupacCode(ii))
        {
            cout << ii << " " << char(ii) << endl;
            CGA_ASSERT(bu::isValidBase(bu::disambiguate(ii)));
            CGA_ASSERT(bu::isConsistent(ii, bu::disambiguate(ii)));
            CGA_ASSERT(bu::isValidIupacCode(bu::complement(ii)));
            CGA_ASSERT(bu::complement(bu::complement(ii)) == toupper(ii));
            CGA_ASSERT('N' == toupper(ii) || 'W' == toupper(ii) ||
                       'S' == toupper(ii) || ii != bu::complement(ii));
        }

        compatCount[bu::BASE_COMPATIBILITY[ii]]++;
        complementCount[bu::BASE_COMPLEMENT[ii]]++;

        for(int jj=0; jj<256; jj++)
        {
            string sortedUpper(2, 0);
            sortedUpper[0] = toupper(ii);
            sortedUpper[1] = toupper(jj);
            std::sort(sortedUpper.begin(), sortedUpper.end());
            if ( ('N' == toupper(ii) || bu::isValidBase(ii)) &&
                 ('N' == toupper(jj) || bu::isValidBase(jj)) &&
                 bu::isConsistent(ii, jj) )
            {
                CGA_ASSERT(std::find(baseNConsistent.begin(), baseNConsistent.end(), sortedUpper) !=
                           baseNConsistent.end());
            }
            else
            {
                CGA_ASSERT(std::find(baseNConsistent.begin(), baseNConsistent.end(), sortedUpper) ==
                           baseNConsistent.end());
            }
        }
    }

    for(int ii=0; ii<256; ii++)
    {
        if (1 <= ii && ii <= 15)
            CGA_ASSERT(2 == compatCount[ii]);
        else if (15 < ii)
            CGA_ASSERT(0 == compatCount[ii]);

        if (bu::isValidIupacCode(ii))
            CGA_ASSERT(2 == complementCount[toupper(ii)]);
        else if (0 != ii)
            CGA_ASSERT(0 == complementCount[ii]);
    }

    testConsistency("?", "", true);
    testConsistency("N", "", false);
    testConsistency("ACGT",  "ACGT", true);
    testConsistency("ACGTA", "ACGT", false);
    testConsistency("ACGT?", "ACGT", true);
    testConsistency("?ACGT", "ACGT", true);
    testConsistency("AC?GT", "ACGT", true);
    testConsistency("AC?GT", "ACCGT", true);
    testConsistency("AC?GT", "ACGTC", false);
    testConsistency("AC?GT", "CACGT", false);

    testConsistency("ACGTNTAG", "ACGTNTAR", true);
    testConsistency("ACGTNTAG", "ACGTCTAR", true);
    testConsistency("ACGTNTAT", "ACGTNTAR", false);
    testConsistency("ACTTNTAG", "ACGTNTAR", false);

    testConsistency("ACGTNTAG", "ACG?TCTAR", true);
    testConsistency("ACGTNTAG", "ACG??TCTAR", true);
    testConsistency("ACGTNTAG", "AC?TCTAR", true);
    testConsistency("ACGTNTAG", "A?TCTAR", true);
    testConsistency("ACGTNTAG", "?TCTAR", true);
    testConsistency("ACGTNTAG", "ACGT?", true);
    testConsistency("ACGTCTAG", "AC?GT?AR", true);

    testConsistency("ACGTNTCG", "ACG?TCTAR", false);
    testConsistency("ATGTNTAG", "ACG??TCTAR", false);
    testConsistency("ACGTNTAG", "AC?GCTAR", false);
    testConsistency("ACGTNAAG", "A?TCTAR", false);
    testConsistency("ACGTNTAG", "?TCTAC", false);
    testConsistency("ACGTNTAG", "ACGC?", false);
    testConsistency("ACGTCTAG", "AC?AT?AR", false);

    testConsistency("ACGT?CTAG", "AC?AT?AR", true);
    testConsistency("ACGT?CTTG", "AC?AT?AR", false);
    testConsistency("ATGT?CTAG", "AC?AT?AR", false);

    return 0;
}
