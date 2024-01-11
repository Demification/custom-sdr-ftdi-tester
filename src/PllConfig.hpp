#pragma once
#include <vector>
#include <boost/property_tree/ptree.hpp>

struct PllConfig {
    using Ptr = std::shared_ptr<PllConfig>;
    enum Type: uint8_t { 
        None     = 0x00, 
        InitFreq = 0x01, 
        VcxcoTrim= 0x02, 
        All      = 0xff
    };

    double init_freq = 0.0;
    uint16_t vcxco_trim = UINT16_MAX;

    void fill(uint8_t type, boost::property_tree::ptree& root) const;
    void from(uint8_t type, const boost::property_tree::ptree& root);
};

template <uint8_t config_type> struct PllConfigTyped: public PllConfig {
    static void fill(boost::property_tree::ptree& root, const PllConfig& params);
    static void from(PllConfig& params, const boost::property_tree::ptree& root);
};

template <uint8_t config_type>
inline void PllConfigTyped<config_type>::fill(
    boost::property_tree::ptree &root, const PllConfig &params)
{ 
    params.fill(config_type, root); 
}

template <uint8_t config_type>
inline void PllConfigTyped<config_type>::from(
    PllConfig &params, const boost::property_tree::ptree &root)
{
    params.from(config_type, root);
}
