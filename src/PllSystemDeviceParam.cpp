#include "PllSystemDeviceParam.hpp"

using ptree = boost::property_tree::ptree;

void PllSystemDeviceParam::fill(uint8_t select, ptree &root) const
{
    if(select == SelectNone) return;

    if(select & SelectInitFreq)
        if(init_freq != 0.0) 
            root.put<double>("init_freq", init_freq);

    if(select & SelectVcxcoTrim)
        if(vcxco_trim != UINT16_MAX) 
            root.put<uint16_t>("vcxco_trim", vcxco_trim);
}

void PllSystemDeviceParam::from(uint8_t select, const ptree &root)
{
    if(select & SelectInitFreq)
        init_freq = root.get<double>("init_freq", 0.0);

    if(select & SelectVcxcoTrim)
        vcxco_trim = root.get<uint16_t>("vcxco_trim", UINT16_MAX);
}