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

#include "cgatools/util/Exception.hpp"
#include <string>

using namespace std;
using cgatools::util::Exception;

int TestException(int argc, char* argv[])
{
    string str("test string");
    Exception ee(str);
    CGA_ASSERT(ee.what() == str);

    try
    {
        throw ee;
    }
    catch(std::exception& e2)
    {
        CGA_ASSERT(e2.what() == str);
    }

    return 0;
}
