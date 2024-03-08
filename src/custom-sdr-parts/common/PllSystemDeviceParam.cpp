#include "PllSystemDeviceParam.hpp"

using ptree = boost::property_tree::ptree;

void PllSystemDeviceParam::fill(uint8_t select, ptree &root) const
{
    if(select == SelectNone) return;

    if(select & SelectInitFreq)
        if(init_freq != 0.0) 
            root.put<double>("init_freq", init_freq);

    if(select & SelectVcxcoTrimPpm)
        if(vcxco_trim_ppm != 0.0) 
            root.put<double>("vcxco_trim_ppm", vcxco_trim_ppm);
}

void PllSystemDeviceParam::from(uint8_t select, const ptree &root)
{
    if(select & SelectInitFreq)
        init_freq = root.get<double>("init_freq", 0.0);

    if(select & SelectVcxcoTrimPpm)
        vcxco_trim_ppm = root.get<double>("vcxco_trim_ppm", 0.0);
}