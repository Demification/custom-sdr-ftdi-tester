#include "FtdiDeviceInfoList.hpp"
#include <string.h>
#include <sstream>

#include "spi.h"
#include "ftdi.h"
#include "Debug.hpp"

FtdiDeviceInfoList::Ptr FtdiDeviceInfoList::create() {
    return FtdiDeviceInfoList::Ptr(new FtdiDeviceInfoList());
}

FtdiDeviceInfoList::FtdiDeviceInfoList() {
    updateFtdiDeviceInfoData();
}

void FtdiDeviceInfoList::printAllDeviceInfo() const {
    __DEBUG_INFO__("Found FTDI devices:");
    for(const auto& info: m_ftdiDevices) 
        __DEBUG_INFO__("\t" + std::to_string(info.first) + " " 
            + info.second->description);
}

bool FtdiDeviceInfoList::empty() const {
    return m_ftdiDevices.empty();
}

std::map<int, FtdiDeviceInfo::Ptr> FtdiDeviceInfoList::get() const {
    return m_ftdiDevices;
}

FtdiDeviceInfo::Ptr FtdiDeviceInfoList::getByIndex(int index) const {
    if(m_ftdiDevices.empty() || m_ftdiDevices.find(index) == m_ftdiDevices.end()) 
        return nullptr;
        
    return m_ftdiDevices.at(index);
}

FtdiDeviceInfo::Ptr FtdiDeviceInfoList::getByDescription(
    const std::string &description) const 
{
    for(auto info: m_ftdiDevices){
        if(info.second->description == description)
            return info.second;
    }
    return nullptr;
}

bool FtdiDeviceInfoList::updateFtdiDeviceInfoData() {
    DWORD count, result;
    if(FT_OK != (result = FTD2_CreateDeviceInfoList(&count))) {
        __DEBUG_ERROR__("Can`t create device info list."
            " Result: " + std::to_string(result));
        return false;
    }

    if(!count){
        __DEBUG_ERROR__("No SPI devices found.");
        return false;
    }

    auto bsz = sizeof(FT_DEVICE_LIST_INFO_NODE);
    auto info = (FT_DEVICE_LIST_INFO_NODE*)malloc(bsz * count);

    if(FT_OK != FTD2_GetDeviceInfoList (info, &count)) {
        __DEBUG_ERROR__("Can`t get device info list.");
        return false;
    }

    m_ftdiDevices.clear();
    for (size_t i = 0; i < count; i++) {
        auto ftdiDevInfo = FtdiDeviceInfo::Ptr(new FtdiDeviceInfo);

        memcpy(ftdiDevInfo.get(), &info[i], 4 * sizeof(unsigned int));
        ftdiDevInfo->serialNumber = info[i].SerialNumber;
        ftdiDevInfo->description = info[i].Description;
        ftdiDevInfo->handle = info[i].ftHandle;

        m_ftdiDevices.emplace(i, ftdiDevInfo);
    }
    
    free(info);
    return true;
}

std::string FtdiDeviceInfo::string() const {
    std::string text = "\n\t ";

    if(flags & FT_FLAGS_OPENED) text += "opened, ";
    if(flags & FT_FLAGS_HISPEED) text += "high speed, ";

    std::stringstream stream;
    stream << std::hex << handle;

    text +=
	    "type: " + std::to_string(type) +
	    "\n\t id: " + std::to_string(id) +
	    ", lid: " + std::to_string(locId) +
        ", handle: " + stream.str() +
	    "\n\t s/n: " + serialNumber +
	    "\n\t description: " + description;

    return text;
}
