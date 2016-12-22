#Complete Genomics Analysis Tools
##  Import from abandoned https://sourceforge.net/projects/cgatools/files/1.8.0/
## This is the Complete Genomics Analysis Tools, 1.8.0 (2013) is current:
## http://cgatools.sourceforge.net/
## http://www.completegenomics.com/public-data/analysis-tools/cgatools/

### However it is impossible to compile this from source as the source code specifically calls for`BOOST_FILESYSTEM_VERSION 2`

This was a known issue as `cgatools-1.8.0.1-source/cgatools/core.hpp:[43-46]` notes:
```
// Use version 2 of the Boost Filesystem library.
// This stopped being the default as of Boost 1.44 and
// will no longer be supported in Boost 1.48.
#define BOOST_FILESYSTEM_VERSION 2
```
boost is currently at `1.62.0`. `1.48.0` was released in 2011.

### However Complete Genomics data are just as common now as they were then, if not more so. This binary version is the only way to install `cgatools` on a modern machine without also using a version of `boost-1.47.0` around simply to compile this tool.

### This fork is inteded to port `cgatools` from `BOOST_FILESYSTEM_VERSION 2` to `BOOST_FILESYSTEM_VERSION 3`
#### This will continue to be a problem as [@galaxyproject](https://github.com/galaxyproject), one-of, if not THE, most popular bioinformatics platorms still relies on the pre-compiled binaries available at (http://sourceforge.net/projects/cgatools/files/) for both linux and osx deployments, including their cloud offerings. [@galaxyproject](https://github.com/galaxyproject) have gone so far as to create their own repo for many of these abandoned projects (https://github.com/galaxyproject/cargo-port)

### It is imperitive for the BioInformatics community that we update `cgatools` to support a modern `boost` implementation 

---

CGA Tools is an open source project to provide tools for downstream
analysis of Complete Genomics data.  The CGA Tools source code, as
well as precompiled binaries for Linux and Mac OS X are available from
SourceForge.  The source code can be compiled and run on Linux, Unix,
or Mac OS X systems. 


Installation

For detailed install instructions, please see cgatools-user-guide.pdf,
included in the CGA Tools release.


Tested Environments

cgatools was tested on these platforms:

* GCC 4.1.1 on CentOS with cmake 2.8, boost 1.46.1, python 2.4.3. 
* GCC 4.2.1 on Mac OS X 10.6.0 with cmake 2.8.2, boost 1.46.1, python
  2.6.1.


Changelog

For a list of changes, see cgatools-release-notes.pdf, included in the
cgatools release.


License

Copyright 2016 Benjamin Carr

Copyright 2013 Complete Genomics, Inc.

Licensed under the Apache License, Version 2.0 (the "License"); you
may not use this file except in compliance with the License.  You may
obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied.  See the License for the specific language governing
permissions and limitations under the License.
