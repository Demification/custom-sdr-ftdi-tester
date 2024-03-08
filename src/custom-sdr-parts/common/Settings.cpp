#include "Settings.hpp"
#include <boost/filesystem.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "Debug.hpp"
#include "Utilits.hpp"

Settings::~Settings() {
    save();
}

Settings &Settings::instance() {
	static Settings settings;
	return settings;
}

PllSystemDeviceParam::Ptr Settings::getPllSystemDeviceConfig() const {
    auto config = PllSystemDeviceParam::Ptr(new PllSystemDeviceParam);
    config->from(PllSystemDeviceParam::SelectAll, m_settings);

    return config;
}

void Settings::setInitFreq(double value) {
    PllSystemDeviceParam config {.init_freq = value};
    config.fill(PllSystemDeviceParam::SelectInitFreq, m_settings);

    save();
}

void Settings::setVcxcoTrimPpm(double value) {
    PllSystemDeviceParam config {.vcxco_trim_ppm = value};
    config.fill(PllSystemDeviceParam::SelectVcxcoTrimPpm, m_settings);

    save();
}

Settings::Settings(): m_settings() {
    m_path = getApplicationDirPath() + "/settings.json";

    if(!boost::filesystem::exists(m_path)){
        boost::property_tree::ptree root;

        PllSystemDeviceParam config {.init_freq = 10000000, 
                                     .vcxco_trim_ppm = 0.0};
                                     
        config.fill(PllSystemDeviceParam::SelectAll, root);

        std::ofstream ofs(m_path); 
        boost::property_tree::write_json(ofs, root);
    }

    std::fstream file;
    file.open(m_path, std::ios::in | std::ios::out);
    if(!file.is_open()) {
        __DEBUG_ERROR__("Can`t open file: " + m_path);
        return;
    }

    boost::property_tree::read_json(file, m_settings);
    file.flush();
    file.close();
}

void Settings::save() {
    std::fstream file;
    file.open(m_path, std::ios::in | std::ios::out | std::ios::trunc);
    if(!file.is_open()) {
        __DEBUG_ERROR__("Can`t open file: " + m_path);
        return;
    }

    boost::property_tree::write_json(file, m_settings);
    file.flush();
    file.close();
}
