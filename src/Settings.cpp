#include "Settings.hpp"

#include <libgen.h>
#include <unistd.h>
#include <linux/limits.h>

#include <boost/filesystem.hpp>
#include <boost/property_tree/json_parser.hpp>

//#include "Version.hpp"
#include "Debug.hpp"

Settings::~Settings() {
    save();
}

Settings &Settings::instance() {
	static Settings settings;
	return settings;
}

PllConfig::Ptr Settings::getPllConfig() const {
    auto config = PllConfig::Ptr(new PllConfig);
    config->from(PllConfig::Type::All, m_settings);

    return config;
}

void Settings::setInitFreq(double value) {
    PllConfig config {.init_freq = value};
    config.fill(PllConfig::Type::InitFreq, m_settings);

    save();
}

void Settings::setVcxcoTrim(uint16_t value) {
    PllConfig config {.vcxco_trim = value};
    config.fill(PllConfig::Type::VcxcoTrim, m_settings);

    save();
}

Settings::Settings(): m_settings() {
    char result[PATH_MAX];
    readlink("/proc/self/exe", result, PATH_MAX);
    m_path = std::string(dirname(result)) + "/settings.json";

    if(!boost::filesystem::exists(m_path)){
        boost::property_tree::ptree root;
        root.put("version", /*getVersion()*/ "version");

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
