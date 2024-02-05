#pragma once
#include <cmath>
#include <vector>
#include <boost/property_tree/ptree.hpp>

struct Range {
    double min = std::nan("uninit");
    double max = std::nan("uninit");

    void fill(boost::property_tree::ptree& root) const;
    void from(const boost::property_tree::ptree& root);
    bool valid() const;
};

struct AgcParam {
    void fill(boost::property_tree::ptree& root) const;
    void from(const boost::property_tree::ptree& root);
    bool valid() const;
};

struct RficSystemDeviceParam {
    using Ptr = std::shared_ptr<RficSystemDeviceParam>;
    enum Select : uint16_t
    {
        SelectNone       = 0x0000,
        SelectBand       = 0x0001,
        SelectBandwidth  = 0x0002,
        SelectSampleRate = 0x0004,
        SelectDgcMode    = 0x0008,
        SelectGainMode   = 0x0010,
        SelectLowIfMode  = 0x0020,
        SelectGain       = 0x0040,
        SelectNcoFreq    = 0x0080,
        SelectRficFreq   = 0x0100,
        SelectGainRange  = 0x0200,
        SelectNcoRange   = 0x0400,
        SelectFreqRange  = 0x0800,
        SelectAgcParam   = 0x1000,
        SelectEnableMod  = 0x2000,
        SelectAll        = 0xffff
    };

    int channel = -1;

    int band = -1;
    int bandwidth  = -1;
    int sample_rate = -1;
    int dgc_mode = -1;
    int gain_mode = -1;
    int low_if_mode = -1;
    double gain = std::nan("uninit");
    double nco_freq = std::nan("uninit");
    double rfic_freq = std::nan("uninit");
    Range gain_range;
    Range nco_range;
    Range freq_range;
    AgcParam agc_value;
    bool enable_module = false;
    
    void fill(uint16_t select, boost::property_tree::ptree& root) const;
    void from(uint16_t select, const boost::property_tree::ptree& root);
};

template <uint16_t select_type> 
struct RficSystemDeviceParamTyped: public RficSystemDeviceParam {
    static void fill(boost::property_tree::ptree& root, const RficSystemDeviceParam& params);
    static void from(RficSystemDeviceParam& params, const boost::property_tree::ptree& root);

    static std::string string();
    template <typename V> void setValue(V& value);
};

template <uint16_t select_type>
inline void RficSystemDeviceParamTyped<select_type>::fill(
    boost::property_tree::ptree &root, const RficSystemDeviceParam &params) { 
    params.fill(select_type, root); 
}

template <uint16_t select_type>
inline void RficSystemDeviceParamTyped<select_type>::from(
    RficSystemDeviceParam &params, const boost::property_tree::ptree &root) {
    params.from(select_type, root);
}

template <uint16_t select_type>
inline std::string RficSystemDeviceParamTyped<select_type>::string() {
    switch (select_type)
    {
    case SelectNone: return "none";
    case SelectBand: return "band";
    case SelectBandwidth: return "bandwidth";
    case SelectSampleRate: return "sample_rate";
    case SelectDgcMode: return "dgc_mode";
    case SelectGainMode: return "gain_mode";
    case SelectLowIfMode: return "low_if_mode";
    case SelectGain: return "gain";
    case SelectNcoFreq: return "nco_freq";
    case SelectRficFreq: return "rfic_freq";
    case SelectGainRange: return "gain_range";
    case SelectNcoRange: return "nco_range";
    case SelectFreqRange: return "freq_range";
    case SelectAgcParam: return "agc_value";
    case SelectEnableMod: return "enable_module";
    default:
        break;
    }
    return std::string();
}

template <uint16_t select_type> template <typename V>
inline void RficSystemDeviceParamTyped<select_type>::setValue(V &value) {
    if constexpr (SelectNone == select_type) return;
    else if constexpr (SelectBand == select_type) band = value;
    else if constexpr (SelectBandwidth == select_type) bandwidth = value;
    else if constexpr (SelectSampleRate == select_type) sample_rate = value; 
    else if constexpr (SelectDgcMode == select_type) dgc_mode = value; 
    else if constexpr (SelectGainMode == select_type) gain_mode = value;
    else if constexpr (SelectLowIfMode == select_type) low_if_mode = value;
    else if constexpr (SelectGain == select_type) gain = value;
    else if constexpr (SelectNcoFreq == select_type) nco_freq = value;
    else if constexpr (SelectRficFreq == select_type) rfic_freq = value;
    else if constexpr (SelectGainRange == select_type) gain_range = value;
    else if constexpr (SelectNcoRange == select_type) nco_range = value;
    else if constexpr (SelectFreqRange == select_type) freq_range = value;
    else if constexpr (SelectAgcParam == select_type) agc_value = value;
    else if constexpr (SelectEnableMod == select_type) enable_module = value;
}
