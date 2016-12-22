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
#include "cgatools/util/Exception.hpp"
#include "cgatools/util/parse.hpp"
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

using namespace std;
using cgatools::util::parseValue;

namespace cgatools { namespace {
    template <typename Value>
    void testParseInt(const char* intTestCase)
    {
        bool exn1 = false;
        Value val1(0);
        try
        {
            val1 = parseValue<Value>(intTestCase);
        }
        catch(const std::exception&)
        {
            exn1 = true;
        }

        bool exn2 = false;
        int64_t val2(0);
        try
        {
            val2 = boost::lexical_cast<int64_t>(intTestCase);
            if (val2 < std::numeric_limits<Value>::min() || val2 > std::numeric_limits<Value>::max())
            {
                val2 = 0;
                exn2 = true;
            }
        }
        catch(const std::exception&)
        {
            exn2 = true;
        }

        std::cout << sizeof(Value) << "i: " << intTestCase << ": " <<
            exn1 << " " << exn2 << " " << int64_t(val1) << " " << val2 << std::endl;
        CGA_ASSERT(exn1 == exn2 && val1 == val2);
    }

    template <typename Value>
    void testParseUInt(const char* intTestCase)
    {
        bool exn1 = false;
        Value val1(0);
        try
        {
            val1 = parseValue<Value>(intTestCase);
        }
        catch(const std::exception&)
        {
            exn1 = true;
        }

        bool exn2 = false;
        uint64_t val2(0);
        try
        {
            val2 = boost::lexical_cast<uint64_t>(intTestCase);

            if (val2 > std::numeric_limits<Value>::max())
            {
                val2 = 0;
                exn2 = true;
            }
        }
        catch(const std::exception&)
        {
            exn2 = true;
        }

        // Some versions of boost on Windows get the wrong answer.
        if ('-' == *intTestCase)
        {
            val2 = 0;
            exn2 = true;
        }
        if (0 == strcmp("18446744073709551615", intTestCase) &&
            8 == sizeof(Value))
        {
            val2 = UINT64_C(18446744073709551615);
            exn2 = false;
        }

        std::cout << sizeof(Value) << "u: " << intTestCase << ": " <<
            exn1 << " " << exn2 << " " << uint64_t(val1) << " " << val2 << std::endl;
        CGA_ASSERT(exn1 == exn2 && val1 == val2);
    }

    template <typename Value>
    void testParseFloat(const char* floatTestCase)
    {
        bool exn1 = false;
        Value val1(0);
        try
        {
            val1 = parseValue<Value>(floatTestCase);
        }
        catch(const std::exception&)
        {
            exn1 = true;
        }

        bool exn2 = false;
        Value val2(0);
        try
        {
            val2 = boost::lexical_cast<Value>(floatTestCase);
        }
        catch(const std::exception&)
        {
            exn2 = true;
        }

        std::cout << sizeof(Value) << "f: " << floatTestCase << ": " <<
            exn1 << " " << exn2 << " " << val1 << " " << val2 << std::endl;
        CGA_ASSERT_MSG(exn1 == exn2 && val1 == val2, 
            CGA_VOUT(exn1)<<CGA_VOUT(exn2)<<CGA_VOUT(val1)<<CGA_VOUT(val2)<<CGA_VOUT(floatTestCase));
    }

    void doTestParse()
    {
        const char* intTestCases[] =
            {
                "0", "1", "-1", "37", "127", "128", "-128", "-129", //  int8
                "255", "256",                                       // uint8
                "32767", "32768", "-32768", "-32769",               //  int16
                "65535", "65536",                                   // uint16
                "2147483647", "2147483648", "-2147483648", "-2147483649", //  int32
                "4294967295", "4294967296",                               // uint32
                "9223372036854775807", "9223372036854775808",
                "-9223372036854775808", "-9223372036854775809", //  int64
                "18446744073709551615", "18446744073709551616", // uint64
                "abc", // fail
            };
        BOOST_FOREACH(const char* intTestCase, intTestCases)
        {
            testParseInt<int8_t> (intTestCase);
            testParseInt<int16_t>(intTestCase);
            testParseInt<int32_t>(intTestCase);
            testParseInt<int64_t>(intTestCase);
            testParseInt<char>(intTestCase);
            testParseInt<short>(intTestCase);
            testParseInt<int>(intTestCase);
            testParseInt<long>(intTestCase);
            testParseInt<long long>(intTestCase);

            testParseUInt<uint8_t> (intTestCase);
            testParseUInt<uint16_t>(intTestCase);
            testParseUInt<uint32_t>(intTestCase);
            testParseUInt<uint64_t>(intTestCase);
            testParseUInt<unsigned char>(intTestCase);
            testParseUInt<unsigned short>(intTestCase);
            testParseUInt<unsigned int>(intTestCase);
            testParseUInt<unsigned long>(intTestCase);
            testParseUInt<unsigned long long>(intTestCase);
        }

        const char* floatTestCases[] =
            {
                "0", "0.0", "1.0", "-1.0", "123e10", "123.5e10", "def"
            };
        BOOST_FOREACH(const char* floatTestCase, floatTestCases)
        {
            testParseFloat<float> (floatTestCase);
            testParseFloat<double>(floatTestCase);
        }

        // Selective uncomment for perf test.
//         std::string val("12345678");
//         for(size_t ii=0; ii<100000000; ii++)
//             parseValue<long>(val.c_str());
// //             parseValue<long>(val.c_str(), val.c_str()+val.length());
// //             strtol(val.c_str(), 0, 10);
    }

} } // cgatools::anonymous

int TestParse(int argc, char* argv[])
{
    cgatools::doTestParse();

    return 0;
}
