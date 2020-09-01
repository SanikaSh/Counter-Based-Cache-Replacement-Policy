#include "mem/cache/replacement_policies/counterBased_AIP.hh"

#include <cassert>
#include <memory>

#include "params/CBAIPRP.hh"

CBAIPRP::CBAIPRP(const Params *p)
    : BaseReplacementPolicy(p)
{
}

void
CBAIPRP::invalidate(const std::shared_ptr<ReplacementData>& replacement_data)
const
{
    //std::cout << "invalidate" << std::endl;
}

void
CBAIPRP::touch(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    //std::cout << "touch" << std::endl;

    //Incrementing event counter
    IncrementEventCntrOFAllBlocksInSet(replacement_data);

    auto&& data = std::static_pointer_cast<CBAIPReplData>(replacement_data);

    if(data->eventCntr > data->maxCpresent)
        data->maxCpresent = data->eventCntr;

    //std::cout << "Hit: data[" << data->getSet() << "][" << data->getWay() << "].maxCpresent=" << (int)data->maxCpresent << std::endl;
    
    data->eventCntr = 0;
}

void
CBAIPRP::reset(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    //std::cout << "reset" << std::endl;

    auto&& data = std::static_pointer_cast<CBAIPReplData>(replacement_data);

    uint8_t blkAddr = GetHashValue(((uint64_t)data->getSet() << 32) | data->getWay() );
    PredictionTableUnit unit = PredictionTable[data->hashedPC][blkAddr];
    
    data->eventCntr = 0;
    data->maxCpresent = 0;
    data->maxCpast = unit.maxCstored;
    data->confBit = unit.ptConfBit;
    
}

ReplaceableEntry*
CBAIPRP::getVictim(const ReplacementCandidates& candidates) const
{
    //std::cout << "It is a miss" << std::endl;

    // Since block is accessed, increment the cntr.
    IncrementEventCntrOFAllBlocksInSet(candidates[0]->replacementData);
    
    std::vector<ReplaceableEntry*> setEntries = candidates[0]->replacementData->getSetAssocEntry();
 
     // There must be at least one replacement candidate
    assert(setEntries.size() > 0);

    // Visit all candidates to find victim
    ReplaceableEntry* victim = setEntries[0];
    for (const auto& candidate : setEntries) {
        auto&& data = std::static_pointer_cast<CBAIPReplData>(candidate->replacementData);
        
        if(data->confBit == 1 && (data->eventCntr > data->maxCpresent) && (data->eventCntr > data->maxCpast) )
        {
             // Update victim entry if necessary
            auto&& victimData = std::static_pointer_cast<CBAIPReplData>(victim->replacementData);
            if(data->eventCntr > victimData->eventCntr)
            {
                victim = candidate;
                //std::cout << "Updated Victim" << std::endl;
            }
            
        }

        
    }

    //Update Prediction table
    auto&& victimData = std::static_pointer_cast<CBAIPReplData>(victim->replacementData);
    victimData->hashedPC = GetHashValue(victimData->getPC());
    uint8_t blkAddr = GetHashValue(((uint64_t)victimData->getSet() << 32) | victimData->getWay());

    auto&& unit =  PredictionTable[victimData->hashedPC][blkAddr];
    unit.maxCstored = victimData->maxCpresent;

    if(victimData->maxCpresent == victimData->maxCpast)
        unit.ptConfBit = 1;
    else
        unit.ptConfBit = 0;
    
    PredictionTable[victimData->hashedPC][blkAddr] = unit;

    //std::cout << "Updated PredictionTable Values: PT[" << (int)victimData->hashedPC << "][" << (int)blkAddr << "] - maxCstored=" << (int)unit.maxCstored << std::endl;
    
    return victim;
}

std::shared_ptr<ReplacementData>
CBAIPRP::instantiateEntry()
{
    return std::shared_ptr<ReplacementData>(new CBAIPReplData());
}

CBAIPRP*
CBAIPRPParams::create()
{
    return new CBAIPRP(this);
}

uint8_t CBAIPRP::GetHashValue(uint64_t value) const
{
    uint8_t temp = 0;
    uint8_t hashVal = 0;

    for (int i = 0; i < 8; i++)
    {
        temp = value & 0xFF;
        value = value >> 8;
        hashVal = hashVal ^ temp;
    }
    
    return hashVal;
}

void CBAIPRP::IncrementEventCntrOFAllBlocksInSet(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    std::vector<ReplaceableEntry*> setEntries = replacement_data->getSetAssocEntry();
    for (auto& candidate : setEntries)
    {
        auto&& data = std::static_pointer_cast<CBAIPReplData>(candidate->replacementData);

        uint8_t C = data->eventCntr;
        if(C < 15)
            data->eventCntr = C + 1;
    }
}
