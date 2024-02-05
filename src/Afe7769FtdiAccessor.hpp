#pragma once
#include <memory>

#include "Afe7769RficTypes.hpp"
#include "RficSystemDeviceParam.hpp"

#include "AbstractAccessor.hpp"
#include "FtdiSpiAccessProvider.hpp"

struct Afe7769ChipInfo {
    using Ptr = std::shared_ptr<Afe7769ChipInfo>;

    uint8_t chip_type;
    uint16_t chip_id;
    uint8_t chip_version;
    uint16_t vendor_id;
};

class Afe7769FtdiAccessor: public AbstractAccessor, 
                           protected FtdiSpiAccessProvider
{
public:
    Afe7769FtdiAccessor(FtdiDeviceInfoList::Ptr infoList);
    AccessorType type() const override;

    bool init();
    void* handle();

    bool write(uint16_t address, uint8_t value);
    bool read(uint16_t address, uint8_t& value);

    bool burstRead(uint16_t address, uint8_t* values, uint32_t length);
    bool burstWrite(uint16_t address, uint8_t* values, uint32_t length);

    bool waitUntilMacroReady(int sec = 5);
    bool waitUntilMacroDone(int sec = 5);
    bool executeMacro(uint32_t* operands, uint16_t len, uint8_t opcode);
    bool readMacroResultRegister(uint8_t numOfResReg, uint32_t &result);

private: 
    void wait(int millisec = 1);
    Afe7769ChipInfo::Ptr getChipInfo();

public:
    bool setRficSampleRate(customsdr_rfic_channel ch, customsdr_rfic_sample_rate rate);
    bool getRficSampleRate(customsdr_rfic_channel ch, customsdr_rfic_sample_rate& samplerate);

    bool setRficBandwidth(customsdr_rfic_channel ch, customsdr_rfic_bandwidth bandwidth);
    bool getRficBandwidth(customsdr_rfic_channel ch, customsdr_rfic_bandwidth& bandwidth);

    bool setRficFrequency(customsdr_rfic_channel ch, double frequency);
    bool getRficFrequency(customsdr_rfic_channel ch, double& frequency);
    bool getRficFrequencyRange(customsdr_rfic_channel ch, Range& range);

    bool setRficLowIfMode(customsdr_rfic_channel ch, customsdr_rfic_low_if mode);
    bool getRficLowIfMode(customsdr_rfic_channel ch, customsdr_rfic_low_if& mode);

    bool setRficNcoFrequency(customsdr_rfic_channel ch, double frequency);
    bool getRficNcoFrequency(customsdr_rfic_channel ch, double& frequency);
    bool getRficNcoFrequencyRange(customsdr_rfic_channel ch, Range& range);

    bool setRficBand(customsdr_rfic_channel ch, customsdr_rfic_band band);

    bool setRficGain(customsdr_rfic_channel ch, float gain);
    bool getRficGain(customsdr_rfic_channel ch, float& gain);
    bool getRficGainRange(customsdr_rfic_channel ch, Range& range);

    bool setRficGainMode(customsdr_rfic_channel ch, customsdr_rfic_gain_mode mode);
    bool getRficGainMode(customsdr_rfic_channel ch, customsdr_rfic_gain_mode& mode);

    bool setRficAgcParam(customsdr_rfic_channel ch, customsdr_rfic_agc_param  param);
    bool getRficAgcParam(customsdr_rfic_channel ch, customsdr_rfic_agc_param& param);

    bool setRficDgcMode(customsdr_rfic_channel ch, int mode);
    bool getRficDgcMode(customsdr_rfic_channel ch, int& mode);

    bool rficEnableModule(customsdr_rfic_channel ch, bool enable);
};

