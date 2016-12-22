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

#include <boost/foreach.hpp>

#include "cgatools/cgdata/CnvSegmentStore.hpp"

namespace cgatools { namespace cgdata {

using namespace reference;
using namespace util;

void CnvSegmentStore::bindColumns(util::DelimitedFile& df,
                                  reference::Range& range,
                                  CnvSegmentData& data)
{
    hasCalledPloidy_ = df.hasField("calledPloidy");
    hasCalledLevel_ = df.hasField("calledLevel");
    bindRangeColumns(df, range, "chr", "begin", "end");
    df.addField(ValueField<double>(
                    "relativeCvg",
                    &data.relativeCoverage_).exception("N", -1.00) );
    if (hasCalledPloidy_)
        df.addField(StringField("calledPloidy", &data.calledPloidy_));
    if (hasCalledLevel_)
        df.addField(StringField("calledLevel", &data.calledLevel_));
    
    hasLAF_ = ( df.hasField("bestLAF") || df.hasField("bestLAFsingle") || df.hasField("bestLAFpaired") );

    if ( df.hasField("bestLAFsingle") ){
        if ( df.hasField("bestLAF")|| df.hasField("bestLAFpaired") ){
            throw Exception("multiple LAF types in " + df.getMetadata().getFileName());
        }
        df.addField(StringField("bestLAFsingle", &data.bestLAF_));
        df.addField(StringField("lowLAFsingle", &data.lowLAF_));
        df.addField(StringField("highLAFsingle", &data.highLAF_));
    } else if ( df.hasField("bestLAFpaired") ) {
        if ( df.hasField("bestLAF")|| df.hasField("bestLAFsingle") ){
            throw Exception("multiple LAF types in " + df.getMetadata().getFileName());
        }
        if ( (! df.hasField("lowLAFpaired")) || (! df.hasField("highLAFpaired")) ){
            throw Exception("low/highLAFpaired columns expected in " + df.getMetadata().getFileName());
        }
        df.addField(StringField("bestLAFpaired", &data.bestLAF_));
        df.addField(StringField("lowLAFpaired", &data.lowLAF_));
        df.addField(StringField("highLAFpaired", &data.highLAF_));
    } else if ( df.hasField("bestLAF") ) {
        if ( df.hasField("bestLAFsingle")|| df.hasField("bestLAFpaired") ){
            throw Exception("multiple LAF types in " + df.getMetadata().getFileName());
        }
        if ( (! df.hasField("lowLAF")) || (! df.hasField("highLAF")) ){
            throw Exception("low/highLAF columns expected in " + df.getMetadata().getFileName());
        }
        df.addField(StringField("bestLAF", &data.bestLAF_));
        df.addField(StringField("lowLAF", &data.lowLAF_));
        df.addField(StringField("highLAF", &data.highLAF_));
    }

}


const CnvSegmentData*
CnvSegmentStore::getBestOverlappingSegment(const reference::Range& r) const
{
    std::vector<QueryResultType> buffer;
    intersect(r, buffer);

    QueryResultType best(0);
    int32_t overlap = -1;

    BOOST_FOREACH(QueryResultType ii, buffer)
    {
        Range ovr = r.overlappingRange(ii->first);
        int32_t len = ovr.length();
        if (len > overlap || (len == overlap && ii->first < best->first))
        {
            overlap = len;
            best = ii;
        }
    }

    if (overlap >= 0)
        return &(best->second);
    else
        return 0;
}

}}
