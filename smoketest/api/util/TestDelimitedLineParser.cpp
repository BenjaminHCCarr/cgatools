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
#include "cgatools/util/DelimitedLineParser.hpp"
#include "cgatools/util/Exception.hpp"

using std::string;
using std::vector;
using namespace cgatools::util;

namespace {
    void testVectorParser1()
    {
        cgatools::uint16_t jj = 0, ii = 0;
        vector<cgatools::uint16_t> xx(10, 12345);

        DelimitedLineParser parser;
        parser.addField(ValueField<cgatools::uint16_t>("jj", &jj));
        parser.addField(ValueVectorField<cgatools::uint16_t>("xx", ';', &xx));
        parser.addField(ValueField<cgatools::uint16_t>("ii", &ii));

        string line("1\t10;11;12\t2");
        parser.parseLine(line);
        CGA_ASSERT(1 == jj && 2 == ii);
        CGA_ASSERT(xx.size() == 3 && xx[0] == 10 && xx[1] == 11 && xx[2] == 12);

        line = "1\t\t2";
        parser.parseLine(line);
        CGA_ASSERT(1 == jj && 2 == ii);
        CGA_ASSERT(xx.size() == 0);

        line = "1\t10;\t2"; // empty string after ; is not a valid integer
        try
        {
            parser.parseLine(line);
        }
        catch(const std::exception& ee)
        {
            CGA_ASSERT(string::npos != string(ee.what()).find("failed to parse int"));
        }
    }

    void testVectorParser2()
    {
        vector<double> xx(10, 12345);

        DelimitedLineParser parser;
        parser.addField(IgnoreField("jj"));
        parser.addField(ValueVectorField<double>("xx", ';', &xx));
        parser.addField(IgnoreField("ii"));

        string line("1\t10;11;12\t2");
        parser.parseLine(line);
        CGA_ASSERT(xx.size() == 3 && xx[0] == 10.0 && xx[1] == 11.0 && xx[2] == 12.0);
    }

}

int TestDelimitedLineParser(int argc, char* argv[])
{
    cgatools::uint16_t jj = 0;
    cgatools::int32_t ii = 0;
    string str;
    DelimitedLineParser parser;
    parser.addField(ValueField<cgatools::uint16_t>("jj", &jj));
    parser.addField(ValueField<cgatools::int32_t>("ii", &ii).
                    exception("one", 1));
    parser.addField(IgnoreField("ignored"));
    parser.addField(StringField("str", &str));

    string line("40000\t-10\tblah blah,blah\tstrvalue");
    parser.parseLine(line);
    CGA_ASSERT(40000 == jj);
    CGA_ASSERT(-10 == ii);
    CGA_ASSERT("strvalue" == str);

    line = "40000\tone\tblah blah,blah\tstrvalue";
    parser.parseLine(line);
    CGA_ASSERT(40000 == jj);
    CGA_ASSERT(1 == ii);
    CGA_ASSERT("strvalue" == str);

    line = "123,-100000,dddd\t,new\tvalue";
    parser.parseLine(line, ',');
    CGA_ASSERT(123 == jj);
    CGA_ASSERT(-100000 == ii);
    CGA_ASSERT("new\tvalue" == str);

    line = "124,-100001,dddd\t,";
    parser.parseLine(line, ',');
    CGA_ASSERT(124 == jj);
    CGA_ASSERT(-100001 == ii);
    CGA_ASSERT("" == str);

    line = "12    567  ..abc Z";
    parser.parseLine(line, ' ', DelimitedLineParser::SKIP_EMPTY_FIELDS);
    CGA_ASSERT(12 == jj);
    CGA_ASSERT(567 == ii);
    CGA_ASSERT("Z" == str);

    line = "  7    8567  ..ab ZZ ";
    parser.parseLine(line, ' ', DelimitedLineParser::SKIP_EMPTY_FIELDS);
    CGA_ASSERT(7 == jj);
    CGA_ASSERT(8567 == ii);
    CGA_ASSERT("ZZ" == str);

    try
    {
        line = "123\t456";
        parser.parseLine(line);
    }
    catch(std::exception& ee)
    {
        CGA_ASSERT(string::npos != string(ee.what()).find("not enough fields"));
    }

    try
    {
        line = "40000\t-10\tblah blah,blah\tstr\tvalue";
        parser.parseLine(line);
    }
    catch(std::exception& ee)
    {
        CGA_ASSERT(string::npos != string(ee.what()).find("too many fields"));
    }

    try
    {
        line = "40000u\t-10\tblah blah,blah\tstrvalue";
        parser.parseLine(line);
    }
    catch(std::exception& ee)
    {
        CGA_ASSERT(string::npos != string(ee.what()).find("failed to parse int"));
    }

    testVectorParser1();
    testVectorParser2();

    return 0;
}
