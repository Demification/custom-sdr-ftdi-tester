#pragma once
#include <string>
#include <fstream> 

#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/bind.hpp>
#include <boost/property_tree/ptree.hpp>

#include "PllSystemDeviceParam.hpp"

class Settings
{
public:
	~Settings();
	static Settings& instance();

    PllSystemDeviceParam::Ptr getPllSystemDeviceConfig() const;

    void setInitFreq(double value);
    void setVcxcoTrim(uint16_t value);

private:
    Settings();
    void save();

    std::string m_path;
    boost::property_tree::ptree m_settings;
};