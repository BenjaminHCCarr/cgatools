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

#ifndef CGATOOLS_VARIANTS_CALL_HPP_
#define CGATOOLS_VARIANTS_CALL_HPP_ 1

//! @file Call.hpp

#include "cgatools/core.hpp"
#include "cgatools/reference/CrrFile.hpp"
#include "cgatools/util/DelimitedFile.hpp"
#include <string>

namespace cgatools { namespace variants {

    //! A struct that corresponds to a single line of a Complete
    //! Genomics variant file.
    struct Call
    {
    public:
        class VarQualityField : public util::DelimitedFieldParser
        {
        public:
            VarQualityField(const std::string& name, std::vector<std::string>* varFilter);

            void parse(const char* first, const char* last);

        private:
            std::vector<std::string>* varFilter_;
        };

        class VarFilterField : public util::DelimitedFieldParser
        {
        public:
            VarFilterField(const std::string& name, std::vector<std::string>* varFilter);

            void parse(const char* first, const char* last);

        private:
            std::vector<std::string>* varFilter_;
        };

        class TotalScoreField : public util::DelimitedFieldParser
        {
        public:
            TotalScoreField(
                const std::string& name,
                int32_t* varScoreVAF,
                int32_t* varScoreEAF);

            void parse(const char* first, const char* last);

        private:
            int32_t* varScoreVAF_;
            int32_t* varScoreEAF_;
        };

        struct AlternativeCall
        {
            AlternativeCall()
                : score_(0)
            {
            }

            AlternativeCall(const std::string& alleleSeq, int32_t score)
                : alleleSeq_(alleleSeq),
                  score_(score)
            {
            }

            std::string alleleSeq_;
            int32_t     score_;
        };

        class AlternativeCallsField : public util::DelimitedFieldParser
        {
        public:
            AlternativeCallsField(
                const std::string& name,
                std::vector<AlternativeCall>* alternativeCalls);

            void parse(const char* first, const char* last);

        private:
            std::vector<AlternativeCall>* alternativeCalls_;
        };

        //! The "all" haplotype == [1..ploidy_] haplotypes.
        static const uint16_t ALL_HAPLOTYPES;

        //! Unknown ploidy, normally in no-ref regions on genome.
        static const uint16_t UNKNOWN_PLOIDY;

        //! The in-memory score corresponding to empty score on disk.
        static const int32_t EMPTY_SCORE;

        //! Construct an empty call.
        Call ();

        //! Returns the called sequence as a string, replacing "=" with
        //! the corresponding reference sequence.
        std::string calledSequence(const reference::CrrFile& crr) const;

        //! Returns the reference sequence as a string, replacing "="
        //! with the corresponding reference sequence.
        std::string refSequence(const reference::CrrFile& crr) const;

        //! Returns true iff the alleleSeq_ is consistent with the
        //! reference.
        bool isRefConsistent(const reference::CrrFile& crr) const;

        //! Returns true iff this call's sequence sequence contains N or
        //! ? characters.
        bool hasNoCalls() const;

        //! Add fields to parser so that it can parse lines, putting
        //! data in this call.
        void addFieldParsers(util::DelimitedFile& df, const reference::CrrFile& crr);

        //! Adds a filter string to varFilter_ vector
        void addVarFilter(const std::string& filter);

        //! Writes the call out, using the given separator charactor.
        std::ostream& write(std::ostream& out,
                            const reference::CrrFile& crr,
                            const char sep = '\t') const;

        //! Parse alternativeCalls field.
        static void parseAlternativeCalls(
            const char* first, const char* last, std::vector<AlternativeCall>& alternativeCalls);

        //! Parse varFilter field.
        static void parseVarFilter(
            const char* first, const char* last, std::vector<std::string>& varFilter);

        //! Returns header for a call output.
        static std::string getHeader (const char separator = '\t');

        //! The locus column in the variant file.
        uint32_t locus_;

        //! The ploidy column in the variant file. The value is
        //! Call::UNKNOWN_PLOIDY if the variant file specifies "?".
        uint16_t ploidy_;

        //! The haplotype column in the variant file. The value is
        //! Call::ALL_HAPLOTYPES if the variant file specifies "all".
        uint16_t haplotype_;

        //! The chromosome, begin, and end columns in the variant file.
        reference::Range range_;

        //! The varType column in the variant file.
        std::string varType_;

        //! The reference column in the variant file.
        std::string reference_;

        //! The alleleSeq column in the variant file.
        std::string alleleSeq_;

        //! The varScoreVAF column in the variant file.
        int32_t varScoreVAF_;

        //! The varScoreEAF column in the variant file. For older var
        //! files that have totalScore instead of varScoreVAF and
        //! varScoreEAF, the totalScore is filled into the varScoreEAF
        //! column.
        int32_t varScoreEAF_;

        //! The varFilter column in the variant file. For old var files
        //! that have no varFilter or varQuality, this field is
        //! empty. For old var files with varQuality instead of
        //! varFilter, this field may be empty, VQLOW or PASS.
        std::vector<std::string> varFilter_;

        //! The hapLink column in the variant file.
        std::string hapLink_;

        //! The xRef column in the variant file.
        std::string xRef_;

        //! The alleleFreq column in the variant file.
        std::string alleleFreq_;

        //! The alternativeCalls column in the variant file.
        std::vector<AlternativeCall> alternativeCalls_;
    };

    //! Convert some field values that exist only in old variation files
    //! to their contemporary equivalents
    void upgradeOldFieldValues(Call& c);

} } // cgatools::variants

#endif // CGATOOLS_VARIANTS_CALL_HPP_
