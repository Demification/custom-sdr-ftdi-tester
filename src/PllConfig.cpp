#include "PllConfig.hpp"

using ptree = boost::property_tree::ptree;

void PllConfig::fill(uint8_t type, ptree &root) const
{
    if(type == Type::None) return;

    if(type & Type::InitFreq)
        if(init_freq != 0.0) 
            root.put<double>("init_freq", init_freq);

    if(type & Type::VcxcoTrim)
        if(vcxco_trim != UINT16_MAX) 
            root.put<uint16_t>("vcxco_trim", vcxco_trim);
}

void PllConfig::from(uint8_t type, const ptree &root)
{
    if(type & Type::InitFreq)
        init_freq = root.get<double>("init_freq", 0.0);

    if(type & Type::VcxcoTrim)
        vcxco_trim = root.get<uint16_t>("vcxco_trim", UINT16_MAX);
}