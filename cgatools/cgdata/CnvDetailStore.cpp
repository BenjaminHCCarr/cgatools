// Copyright 2011 Complete Genomics, Inc.
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
#include "boost/lexical_cast.hpp"

#include "cgatools/cgdata/CnvDetailStore.hpp"

namespace cgatools { namespace cgdata {

using namespace reference;
using namespace util;

void CnvDetailStore::bindColumns(util::DelimitedFile& df,
                                  reference::Range& range,
                                  CnvDetailData& data)
{
    hasCalledPloidy_ = df.hasField("calledPloidy");
    hasCalledLevel_ = df.hasField("calledLevel");

    windowMidpoint_ = df.hasField("position");

    if(windowMidpoint_){
        CGA_ASSERT(df.getMetadata().hasKey("WINDOW_WIDTH"));
        windowWidth_ = boost::lexical_cast<int>(df.getMetadata().get("WINDOW_WIDTH"));
        bindRangeMidpointColumns(df, range, "chr", "position");
    } else {
        bindRangeColumns(df, range, "chr", "begin", "end");
    }

    df.addField(ValueField<double>(
                    "relativeCvg",
                    &data.relativeCoverage_).exception("N", -1.00) );
    if (hasCalledPloidy_)
        df.addField(StringField("calledPloidy", &data.calledPloidy_));
    if (hasCalledLevel_)
        df.addField(StringField("calledLevel", &data.calledLevel_));

    hasLAF_ = ( df.hasField("bestLAF") || df.hasField("bestLAFpaired") || df.hasField("bestLAFsingle") );
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


const CnvDetailData*
CnvDetailStore::getBestOverlappingDetail(const reference::Range& r) const
{
    std::vector<QueryResultType> buffer;
    intersect(r, buffer);

    QueryResultType best(0);
    int32_t overlap = -1;

    BOOST_FOREACH(QueryResultType ii, buffer)
    {
        Range ovr = r.overlappingRange(ii->first);
        int64_t len = ovr.length();
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

void CnvDetailStore::trimTree(){

    // destination for new store, a trimmed version of existing
    CnvDetailStore::DataStore trimmed;

    // find intervals overlapping all contigs
    std::vector<reference::Range> contigs = crr_.listContigs();
    BOOST_FOREACH(const reference::Range& contig, contigs){
        std::vector<QueryResultType> buffer;
        intersect(contig,buffer);
        int N = buffer.size();
        int n=0;
        BOOST_FOREACH(QueryResultType & origrng, buffer){
            n++;

            std::pair<reference::Range,CnvDetailData> r(origrng->first,origrng->second);

            r.first.begin_ -= windowWidth_/2;
            CGA_ASSERT(r.first.begin_ >= contig.begin_);

            r.first.end_ += (windowWidth_-windowWidth_/2);
            CGA_ASSERT(r.first.end_<=contig.end_);
            if(n==N){
                r.first.end_ = contig.end_;
            }

#if 0            
            // this code handles the possibility of a window extrapolated from the midpoint
            // running off the end of a contig ... not actually a concern 
            if(r.first.end_ > contig.end_){
                throw Exception("range end " + boost::lexical_cast<std::string>(r.first.end_)
                                + " beyond end of contig " + boost::lexical_cast<std::string>(contig.end_));
            }
            r.first.end_ += (windowWidth_-windowWidth_/2);
            if(r.first.end_ > contig.end_){
                //std::cerr << " trimming " << r.first.end_ << " to " << contig.end_ <<std::endl;
                r.first.end_ = contig.end_;
            }
#endif

            trimmed.put(r.first,r.second);
        }
    }
    data_.swap(trimmed);
}

}}
