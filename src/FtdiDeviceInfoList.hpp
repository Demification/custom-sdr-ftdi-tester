#pragma once
#include <map>
#include <string>
#include <memory>

struct FtdiDeviceInfo {
    using Ptr = std::shared_ptr<FtdiDeviceInfo>;

    unsigned int flags;
	unsigned int type;
	unsigned int id;
	unsigned int locId;
	std::string serialNumber;
	std::string description;
    void* handle;

    std::string string() const;
};

class FtdiDeviceInfoList {
public:
    using Ptr = std::shared_ptr<FtdiDeviceInfoList>;

    static FtdiDeviceInfoList::Ptr create();
    FtdiDeviceInfoList();

    void printAllDeviceInfo() const;

    bool empty() const;
    std::map<int, FtdiDeviceInfo::Ptr> get() const;

    FtdiDeviceInfo::Ptr getByIndex(int index) const;
    FtdiDeviceInfo::Ptr getByDescription(const std::string& description) const;

private:
    std::map<int, FtdiDeviceInfo::Ptr> m_ftdiDevices;

    bool updateFtdiDeviceInfoData();
};

