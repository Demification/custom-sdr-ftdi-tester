#pragma once
#include <vector>
#include <boost/property_tree/ptree.hpp>

struct PllSystemDeviceParam {
    using Ptr = std::shared_ptr<PllSystemDeviceParam>;
    enum Select: uint8_t 
    { 
        SelectNone      = 0x00, 
        SelectInitFreq  = 0x01, 
        SelectVcxcoTrim = 0x02, 
        SelectAll       = 0xff
    };

    double init_freq = 0.0;
    uint16_t vcxco_trim = UINT16_MAX;

    void fill(uint8_t select, boost::property_tree::ptree& root) const;
    void from(uint8_t select, const boost::property_tree::ptree& root);
};

template <uint8_t select_type> struct PllSystemDeviceParamTyped: public PllSystemDeviceParam {
    static void fill(boost::property_tree::ptree& root, const PllSystemDeviceParam& params);
    static void from(PllSystemDeviceParam& params, const boost::property_tree::ptree& root);
};

template <uint8_t select_type>
inline void PllSystemDeviceParamTyped<select_type>::fill(
    boost::property_tree::ptree &root, const PllSystemDeviceParam &params)
{ 
    params.fill(select_type, root); 
}

template <uint8_t select_type>
inline void PllSystemDeviceParamTyped<select_type>::from(
    PllSystemDeviceParam &params, const boost::property_tree::ptree &root)
{
    params.from(select_type, root);
}
