#include "RficSystemDeviceParam.hpp"

void Range::fill(boost::property_tree::ptree &root) const {
    root.put("range_max", max);
    root.put("range_min", min);
}

void Range::from(const boost::property_tree::ptree &root) {
    max = root.get<double>("range_max", std::nan("uninit"));
    min = root.get<double>("range_min", std::nan("uninit"));
}

bool Range::valid() const {
    return max != std::nan("uninit") && min != std::nan("uninit");
}

void AgcParam::fill(boost::property_tree::ptree &root) const {
}

void AgcParam::from(const boost::property_tree::ptree &root) {
}

bool AgcParam::valid() const {
    return false;
}

void RficSystemDeviceParam::fill(uint16_t select, boost::property_tree::ptree &root) const {
    if(channel != -1) root.put<int>("channel", channel);

    if(select == SelectNone) return;

    if((select & SelectBand) && band != -1) root.put<int>("band", band);
    if((select & SelectBandwidth) && bandwidth != -1) root.put<int>("bandwidth", bandwidth);
    if((select & SelectSampleRate) && sample_rate != -1) root.put<int>("sample_rate", sample_rate);
    if((select & SelectDgcMode) && dgc_mode != -1) root.put<int>("dgc_mode", dgc_mode);
    if((select & SelectGainMode) && gain_mode != -1) root.put<int>("gain_mode", gain_mode);
    if((select & SelectLowIfMode) && low_if_mode != -1) root.put<int>("low_if_mode", low_if_mode);
    if((select & SelectGain) && gain != std::nan("uninit")) root.put<double>("gain", gain);
    if((select & SelectNcoFreq) && nco_freq != std::nan("uninit")) root.put<double>("nco_freq", nco_freq);
    if((select & SelectRficFreq) && rfic_freq != std::nan("uninit")) root.put<double>("rfic_freq", rfic_freq);

    auto fillRange = [](
        const std::string& key, boost::property_tree::ptree& root, const Range& range) 
    {
        boost::property_tree::ptree tree;
        range.fill(tree);
        root.add_child(key, tree);
    };

    if((select & SelectGainRange) && gain_range.valid()) fillRange("gain_range", root, gain_range);
    if((select & SelectNcoRange) && nco_range.valid()) fillRange("nco_range", root, nco_range);
    if((select & SelectRficFreq) && freq_range.valid()) fillRange("freq_range", root, freq_range);

    if((select & SelectAgcParam) && agc_value.valid()) {
        boost::property_tree::ptree tree;
        agc_value.fill(tree);
        root.add_child("agc_value", tree);
    }

    if((select & SelectEnableMod) && enable_module != false) root.put<double>("enable_module", enable_module);
}

void RficSystemDeviceParam::from(
    uint16_t select, const boost::property_tree::ptree &root) 
{
    channel = root.get<int>("channel", -1);

    if(select == SelectNone) return;

    if(select & SelectBand) band = root.get<int>("band", -1);
    if(select & SelectBandwidth) bandwidth = root.get<int>("bandwidth", -1);
    if(select & SelectSampleRate) sample_rate = root.get<int>("sample_rate", -1);
    if(select & SelectDgcMode) dgc_mode = root.get<int>("dgc_mode", -1);
    if(select & SelectGainMode) gain_mode = root.get<int>("gain_mode", -1);
    if(select & SelectLowIfMode) low_if_mode = root.get<int>("low_if_mode", -1);
    if(select & SelectGain) gain = root.get<double>("gain", std::nan("uninit"));
    if(select & SelectNcoFreq) nco_freq = root.get<double>("nco_freq", std::nan("uninit"));
    if(select & SelectRficFreq) rfic_freq = root.get<double>("rfic_freq", std::nan("uninit"));

    auto fromRange = [](
        const std::string& key, const boost::property_tree::ptree &root) -> Range 
    {
        Range range;
        auto it = root.find(key);
        if(it != root.not_found()) range.from(it->second);
        return range;
    };

    if(select & SelectGainRange) gain_range = fromRange("gain_range", root);
    if(select & SelectNcoRange) nco_range = fromRange("nco_range", root);
    if(select & SelectFreqRange) freq_range = fromRange("freq_range", root);

    if(select & SelectAgcParam) {
        auto it = root.find("agc_value");
        if(it != root.not_found()) agc_value.from(it->second);
    }

    if(select & SelectEnableMod) enable_module = root.get<bool>("enable_module", false);
}
