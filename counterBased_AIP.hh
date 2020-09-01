/**
 * @file
 * Declaration of a Counter Based - AIP replacement policy.
 */

#ifndef __MEM_CACHE_REPLACEMENT_POLICIES_CB_AIP_RP_HH__
#define __MEM_CACHE_REPLACEMENT_POLICIES_CB_AIP_RP_HH__

#include "mem/cache/replacement_policies/base.hh"

struct CBAIPRPParams;

/** Prediction table struct*/
    struct PredictionTableUnit
    {
      /* data */
      uint8_t maxCstored : 4;
      bool ptConfBit;

      PredictionTableUnit() : maxCstored(0), ptConfBit(false) {}
    };
    
    static PredictionTableUnit PredictionTable[256][256];


class CBAIPRP : public BaseReplacementPolicy
{
  protected:
    /** CB-AIP-specific implementation of replacement data. */
    struct CBAIPReplData : ReplacementData
    {
        uint8_t hashedPC;
        uint8_t eventCntr : 4;
        uint8_t maxCpresent : 4;
        uint8_t maxCpast : 4;
        bool confBit;

        /**
         * Default constructor. Invalidate data.
         */
        CBAIPReplData() : hashedPC(0), eventCntr(0), maxCpresent(0), maxCpast(0), confBit(false) {}
    };

  private:

    //Calculate Hashed value for indexing Prediction Table
    uint8_t GetHashValue(uint64_t) const;

    //Increment event counter for each block of the set on access to set.
    void IncrementEventCntrOFAllBlocksInSet(const std::shared_ptr<ReplacementData>& replacement_data) const;

  public:
    /** Convenience typedef. */
    typedef CBAIPRPParams Params;

    /**
     * Construct and initiliaze this replacement policy.
     */
    CBAIPRP(const Params *p);

    /**
     * Destructor.
     */
    ~CBAIPRP() {}

    /**
     * Invalidate replacement data to set it as the next probable victim.
     * Sets its last touch tick as the starting tick.
     *
     * @param replacement_data Replacement data to be invalidated.
     */
    void invalidate(const std::shared_ptr<ReplacementData>& replacement_data)
                                                              const override;

    /**
     * Touch an entry to update its replacement data.
     * Sets its last touch tick as the current tick.
     *
     * @param replacement_data Replacement data to be touched.
     */
    void touch(const std::shared_ptr<ReplacementData>& replacement_data) const
                                                                     override;

    /**
     * Reset replacement data. Used when an entry is inserted.
     * Sets 
     * @param candidates Replacement candidates, selected by indexing policy.
     * @return Replacement entry to be replaced.
     */
    ReplaceableEntry* getVictim(const ReplacementCandidates& candidates) const
                                                                     override;

    /**
     * Instantiate a replacement data entry.
     *
     * @return A its last touch tick as the current tick.
     *
     * @param replacement_data Replacement data to be reset.
     */
    void reset(const std::shared_ptr<ReplacementData>& replacement_data) const
                                                                     override;

    /**
     * Find replacement victim.
     *shared pointer to the new replacement data.
     */
    std::shared_ptr<ReplacementData> instantiateEntry() override;
};

#endif // __MEM_CACHE_REPLACEMENT_POLICIES_CBAIP_RP_HH__
