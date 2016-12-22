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

#include "cgatools/util/RangeIntersector.hpp"
#include "cgatools/reference/range.hpp"
#include <string>
#include <boost/random.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace cgatools::util;
using namespace cgatools::reference;

#define TEST_INTERSECTION(ri, range, aray) \
    testIntersection(ri, range, vector<int>(aray, aray+sizeof(aray)/sizeof(aray[0])))

namespace {

    class SimpleRangeIntersector
    {
    public:
        typedef std::vector< std::pair<Range,int> > MapType;

        void put(const Range& range, const int& value)
        {
            data_.push_back(make_pair(range, value));
        }

        void intersect(const Range& range,
                       std::vector< MapType::const_iterator >& result) const
        {
            for(size_t ii=0; ii<data_.size(); ii++)
            {
                if (data_[ii].first.intersects(range))
                    result.push_back(data_.begin()+ii);
            }
        }

    private:
        MapType data_;
    };

    template <typename T>
    ostream& operator<<(ostream& out, const vector<T>& x)
    {
        out << "[";
        for (size_t ii = 0; ii < x.size(); ++ii) {
            if (ii) out << ",";
            out << x[ii];
        }
        out << "]";
        return out;
    }


    typedef IntervalTree<Range, Location, int,
                         RangeOverlap,
                         GetRangeBoundary> ST;

    void testRI(cgatools::uint32_t seed,
                int rangeCount,
                int testCount,
                int chromosomeCount,
                int regionLength,
                double exponent,
                int maxRangeLength)
    {
        typedef boost::mt19937 RandomGenerator;
        RandomGenerator rng(seed);
        boost::uniform_real<double> uu(0.0, 1.0);
        boost::variate_generator<RandomGenerator&, boost::uniform_real<double> > gen(rng, uu);

        ST ri;
        SimpleRangeIntersector sri;
        for(int ii=0; ii<rangeCount; ii++)
        {
            int chromosome = int(gen() * chromosomeCount);
            int pos = int(gen() * regionLength);
            int len = int(std::pow(gen(), exponent) * maxRangeLength);
            Range range(chromosome, pos, pos+len);
//             cout << "db: " << ii << ":"
//                  << range.chromosome_ << ","
//                  << range.begin_ << ","
//                  << range.end_ << endl;
            ri.put(range, ii);
//            cout << ii << ":" << ri.getMaxDepth() << endl;
            sri.put(range, ii);
        }

        cout << "Size: " << ri.size() << ", Max. Depth: " << ri.getMaxDepth() << endl;

        for(int ii=0; ii<testCount; ii++)
        {
            int chromosome = int(gen() * chromosomeCount);
            int pos = int(gen() * regionLength);
            int len = int(std::pow(gen(), exponent) * maxRangeLength);
            Range range(chromosome, pos, pos+len);
//             cout << "tt: "
//                  << range.chromosome_ << ","
//                  << range.begin_ << ","
//                  << range.end_ << " ";

            vector<ST::QueryResultType> rvals;
            ri.intersect(range, rvals);
            vector<int> v0;
            for(size_t jj=0; jj<rvals.size(); jj++)
            {
                v0.push_back(rvals[jj]->second);
            }
            std::sort(v0.begin(), v0.end());

            vector<SimpleRangeIntersector::MapType::const_iterator> svals;
            sri.intersect(range, svals);
            vector<int> v1;
            for(size_t jj=0; jj<svals.size(); jj++)
                v1.push_back(svals[jj]->second);
            std::sort(v1.begin(), v1.end());

            if (v0 != v1)
            {
                cout << "actual: " << v0 << endl;
                cout << "expected: " << v1 << endl;
                CGA_ASSERT(false);
            }
        }
    }

    void testIntersection(const ST& st,
                          const Range& range,
                          const std::vector<int>& expected)
    {
        vector<const ST::value_type*> results;
        st.intersect(range, results);
        // CGA_ASSERT( st.intersects(range) == (0 != results.size()) );
        vector<int> actual;
        for(size_t ii=0; ii<results.size(); ii++)
        {
            actual.push_back(results[ii]->second);
        }
        std::sort(actual.begin(), actual.end());
        if (actual != expected)
        {
            cout << "actual: " << actual << endl;
            cout << "expected: " << expected << endl;
            CGA_ASSERT(false);
        }
    }

    void basicTestST()
    {
        ST st;

        st.put(Range(0, 0, 10), 1);
        st.put(Range(0, 10, 10), 2);
        st.put(Range(0, 10, 20), 3);
        st.put(Range(0, 5, 15), 4);
        st.put(Range(0, 5, 25), 5);

        { int aray[] = { 1, 2, 4, 5 }; TEST_INTERSECTION(st, Range(0, 0, 10), aray); }
        { int aray[] = { 1, 2, 3, 4, 5 }; TEST_INTERSECTION(st, Range(0, 10, 10), aray); }
        { int aray[] = { 2, 3, 4, 5 }; TEST_INTERSECTION(st, Range(0, 10, 11), aray); }

        // test that assignment works
        ST st2;
        st2 = st;
        { int aray[] = { 1, 2, 4, 5 }; TEST_INTERSECTION(st2, Range(0, 0, 10), aray); }

        // test that assignment is a deep copy
        st2.clear();
        { int aray[] = { 1, 2, 4, 5 }; TEST_INTERSECTION(st, Range(0, 0, 10), aray); }
    }

    double log2(double s)
    {
        return std::log(s) / std::log(2.0);
    }

    void testSearchTreeConstruction(size_t size)
    {
        ST st;
        for (size_t ii = 0; ii < size; ++ii)
            st.put(Range(0, ii + 1, ii + 3), ii+1);
        size_t depth = st.getMaxDepth();
        CGA_ASSERT(depth >= log2(size));
        CGA_ASSERT(depth < 2 * log2(size));

        st.clear();
        for (size_t ii = 0; ii < size; ++ii)
            st.put(Range(0, size + 10 - ii, size + 20 - ii), ii+1);
        depth = st.getMaxDepth();
        CGA_ASSERT(depth >= log2(size));
        CGA_ASSERT(depth < 2 * log2(size));

        st.clear();
        for (size_t ii = 0; ii < size; ++ii)
            st.put(Range(0, 10, 20), ii+1);
        depth = st.getMaxDepth();
        CGA_ASSERT(depth >= log2(size));
        CGA_ASSERT(depth < 2 * log2(size));
    }

    size_t overlapCounter = 0;
    struct CountingRangeOverlap
    {
        bool operator()(const Range& a, const Range& b) const
        {
            ++overlapCounter;
            return a.intersects(b);
        }
    };
    typedef IntervalTree<Range, Location, int, CountingRangeOverlap, GetRangeBoundary> CST;

    void testSearchTreeSearch(size_t size)
    {
        double h = log2(size);
        CST st;
        for (size_t ii = 0; ii < size; ++ii)
            st.put(Range(0, 10*ii + 1, 10*ii + 5), ii+1);
        size_t depth = st.getMaxDepth();
        CGA_ASSERT(depth >= h && depth < 2*h);
        cout << "depth: " << depth << ", log2(size): " << h << endl;

        vector<ST::QueryResultType> r;
        for (size_t ii = 0; ii < size; ++ii)
        {
            r.clear();
            overlapCounter = 0;
            st.intersect(Range(0, 10*ii, 10*ii + 2), r);
            CGA_ASSERT(r.size() == 1 && r.front()->second == int(ii+1));
            if (overlapCounter > depth)
            {
                cout << overlapCounter << endl;
                CGA_ASSERT(false);
            }
        }

        // add overlapping ranges

        st.put(Range(0, 0, 10*size + 10), size+1);
        st.put(Range(0, 0, 5*size + 5), size+2);
        st.put(Range(0, 5*size + 5, 10*size + 10), size+3);
        h = log2(size+3);
        depth = st.getMaxDepth();
        CGA_ASSERT(depth >= h && depth < 2*h);
        cout << "depth: " << depth << ", log2(size): " << h << endl;

        size_t maxCount = 0;
        for (size_t ii = 0; ii < size; ++ii)
        {
            r.clear();
            overlapCounter = 0;
            st.intersect(Range(0, 10*ii, 10*ii + 2), r);
            CGA_ASSERT(r.size() == 3);
            if (overlapCounter > maxCount)
                maxCount = overlapCounter;
            if (overlapCounter > depth * r.size())
            {
                cout << overlapCounter << endl;
                CGA_ASSERT(false);
            }
        }

        cout << "max overlap count: " << maxCount << endl;
    }

    class StopWatch
    {
    public:

        StopWatch ()
            : start_(now())
        {}

        double elapsed () const
        {
            return now() - start_;
        }
    private:

        double start_;
        double now() const
        {
            namespace pt = boost::posix_time;
            pt::ptime t = pt::microsec_clock::local_time();
            boost::int64_t ticks = (t - pt::ptime(pt::min_date_time)).ticks();
            return (double)ticks / pt::time_duration::ticks_per_second();
        }
    };

    template <typename V>
    void comparePerformance(size_t size, size_t randomCount)
    {
        V st;

        cout << "tilesize: " << size << ", random: " << randomCount << endl;

        StopWatch sw;
        // Add short non-overlapping segments
        for (size_t ii = 0; ii < size; ++ii)
            st.put(Range(0, 10*ii + 1, 10*ii + 9), ii+1);

        // Tile them with another non-overlapping set that overlaps
        // previous set
        for (size_t ii = 0; ii < size; ++ii)
            st.put(Range(0, 10*ii + 6, 10*ii + 14), size+ii+1);

        // Add random large segments
        size_t maxEnd = 10*size + 20;
        typedef boost::mt19937 RandomGenerator;
        RandomGenerator rng(12345);
        boost::uniform_real<double> uu(0.0, 1.0);
        boost::variate_generator<RandomGenerator&, boost::uniform_real<double> > gen(rng, uu);

        for (size_t ii = 0; ii < randomCount; ++ii)
        {
            size_t x = size_t(gen()*maxEnd);
            st.put(Range(0, x, x+200), 2*size+ii+1);
        }

        double constime = sw.elapsed();

        vector<typename V::QueryResultType> rst;
        for (size_t ii = 0; ii < size; ++ii)
        {
            rst.clear();
            size_t x = size_t(gen()*maxEnd);
            st.intersect(Range(0, x, x+100), rst);
        }

        double qtime = sw.elapsed() - constime;

        cout << "depth: " << st.getMaxDepth() << ", treecount: " << st.getTreeCount() << endl;
        cout << "construction: " << constime << ", query: " << qtime << endl;
    }

} // anonymous

int TestRangeIntersector(int argc, char* argv[])
{
    basicTestST();

    for(size_t ii=0; ii<10; ii++)
        testRI(ii*12345, 5000, 1500, 2, 100000, 3.0, 1000);

    testSearchTreeConstruction(10000);
    testSearchTreeSearch(10001);

#if 0
    cout << "interval tree test:" << endl;
    comparePerformance<ST>(2000000, 0);

    cout << "interval tree test:" << endl;
    comparePerformance<ST>(1000000, 10000);

    cout << "interval tree test:" << endl;
    comparePerformance<ST>(1000000, 100000);

    cout << "interval tree test:" << endl;
    comparePerformance<ST>(1000000, 300000);
#endif

    return 0;
}
