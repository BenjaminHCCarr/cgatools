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
#include "cgatools/util/Md5.hpp"
#include "cgatools/util/Exception.hpp"
#include <iostream>

#ifdef CGA_USE_WIN_API
#include <io.h>
#include <fcntl.h>
#endif

using cgatools::util::Md5Context;
using namespace std;

int TestMd5(int argc, char* argv[])
{
#ifdef CGA_USE_WIN_API
    if (_setmode(_fileno(stdin), _O_BINARY) == -1)
    {
        cout << "ERROR: while converting cin to binary:" << strerror(errno) << endl;
        return 1;
    }
#endif

    Md5Context md5;
    char buf[1024];
    size_t totalCount = 0;
    while (cin)
    {
        cin.read(buf, 1024);
        size_t count = cin.gcount();
        totalCount += count;
        md5.update(buf, count);
    }

    string digest = md5.hexDigest();
    CGA_ASSERT(digest == md5.hexDigest());
    cout << md5.hexDigest() << endl;

    return 0;
}
