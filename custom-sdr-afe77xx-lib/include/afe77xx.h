#ifndef LIB_AFE77XX_HEADER_FILE
#define LIB_AFE77XX_HEADER_FILE

#ifdef LIBAFE77XX
#define API_EXPORT __attribute__((visibility("default")))
#else
#define API_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef unsigned char 		U8;
typedef unsigned short 		U16;
typedef unsigned int 		U32;
typedef unsigned long long 	U64;
typedef int 				S32;

API_EXPORT int AFE77xx_RegIfSet(void* handle);
API_EXPORT int AFE77xx_RegIfGetFd();
API_EXPORT int AFE77xx_RegWrite(int fd,int address, U32 data);
API_EXPORT int AFE77xx_RegRead(int fd,int address, U32 *data);
API_EXPORT int AFE77xx_RegReadWrite(int fd, U16 address, U8 data, U8 lsb, U8 msb);
API_EXPORT U32 SerdesWrite(int fd, U32 serdesInstance, U32 address, U32 data);
API_EXPORT U32 SerdesLaneWrite(int fd, U32 ulLaneno, U32 address, U32 data);
API_EXPORT U32 SerdesRead(int fd, U32 serdesInstance, U32 address);
API_EXPORT U32 SerdesLaneRead(int fd, U32 ulLaneno, U32 address);
API_EXPORT U32 RXDsaAttn(int fd, U32 ulChan, U32 ulRXDsa);
API_EXPORT U32 RXDsaSwapAttn(int fd, U32 ulChan, U32 ulRXDsa);
API_EXPORT U32 TxDsaAttn(int fd, U32 ulChan, U32 ulTXDsaA, U32 ulTXDsaB);
API_EXPORT U32 TxDsaDigAttn(int fd, U32 ulChan, U32 ulTXDsa, double ulTXDigAttn);
API_EXPORT U32 TxDsaSwapAttn(int fd, U32 ulChan, U32 ulTXDsaA, U32 ulTXDsaB);
API_EXPORT U32 FbDsaAttn(int fd, U32 ulChan, U32 ulFbDsa);
API_EXPORT U32 EnableInternalAgc(int fd, U32 ulChan, U32 ulEnable);
API_EXPORT U32 SetDefAttenuation(int fd, U32 ulChan, U32 ulDefGain);
API_EXPORT U32 SetMaxAttenuation(int fd, U32 ulChan, U32 ulMaxAttn);
API_EXPORT API_EXPORT U32 SetMinAttenuation(int fd, U32 ulChan, U32 ulMinAttn);
U32 SetMaxMinAttenuation(int fd, U32 ulChan, U32 ulMinAttn, U32 ulMaxAttn);
API_EXPORT U32 BigStepAttackConfig(int fd, U32 ulChan, U32 ulEnable, U32 ulAttackStepSize, double ulAttackThreshold, U64 ulWindowLen, U32 ulHitCount);
API_EXPORT U32 SmallStepAttackConfig(int fd, U32 ulChan, U32 ulEnable, U32 ulAttackStepSize, double ulAttackThreshold, U64 ulWindowLen, U32 ulHitCount);
API_EXPORT U32 SmallStepDecayConfig(int fd, U32 ulChan, U32 ulEnable, U32 ulDecayStepSize, double ulDecayThreshold, U64 ulWindowLen, U32 ulHitCount);
API_EXPORT U32 BigStepDecayConfig(int fd, U32 ulChan, U32 ulEnable, U32 ulDecayStepSize, double ulDecayThreshold, U64 ulWindowLen, U32 ulHitCount);
API_EXPORT U32 ExtLnaControlConfig(int fd, U32 ulChan, U32 ulEnable, U32 ulLnaGain, U32 ulGainMargin, U32 ulBlankingTime);
API_EXPORT U32 OverrideLnaPinControl(int fd, U32 ulChan, U32 ulOverride, U32 ulOverrideValue);
API_EXPORT U32 EnableDgc(int fd, U32 ulChan, U32 ulEnable);
API_EXPORT U32 readEnableDgc(int fd, U32 ulChan);
API_EXPORT U32 GetCurrentRxAttenuation(int fd, U32 ulChan);
API_EXPORT U32 GetCurrentFbAttenuation(int fd, U32 ulChan);
API_EXPORT U32 GetCurrentTxAttenuation(int fd, U32 ulChan);
API_EXPORT U32 FreezeAgc(int fd, U32 ulChan, U32 ulFreeze);
API_EXPORT U32 SetSerdesTxCursor(int fd, U32 ulLaneno, U32 mainCursorSetting, U32 preCursorSetting, U32 postCursorSetting);
API_EXPORT U32 clearSpiAlarms(int fd);
API_EXPORT U32 clearMacroAlarms(int fd);
API_EXPORT U32 clearJesdRxAlarms(int fd);
API_EXPORT U32 clearJesdTxAlarms(int fd);
API_EXPORT U32 clearPllAlarms(int fd);
API_EXPORT U32 clearPapAlarms(int fd);
API_EXPORT U32 clearAllAlarms(int fd);
API_EXPORT U32 getSerdesStatus(int fd,int laneNo);
API_EXPORT U32 getJesdRxLaneErrors(int fd, U32 ulLaneno);
API_EXPORT U32 getJesdRxLaneFifoErrors(int fd, U32 ulLaneno);
API_EXPORT U32 getJesdRxMiscSerdesErrors(int fd, U32 jesdNo);
API_EXPORT U32 getJesdRxAlarms(int fd);
API_EXPORT U32 getJesdRxLinkStatus(int fd);
API_EXPORT U32 getJesdTxFifoErrors(int fd, U32 jesdNo);
API_EXPORT U32 jesdTxSetSyncOverride(int fd, int override, int sendData);
API_EXPORT U32 maskJesdRxLaneErrors(int fd, U32 ulLaneno,U32 maskValue);
API_EXPORT U32 maskJesdRxLaneFifoErrors(int fd, U32 ulLaneno,U32 maskValue);
API_EXPORT U32 maskJesdRxMiscSerdesErrors(int fd, U32 jesdNo,U32 miscErrorsMaskValue,U32 serdesErrorsMaskValue);
API_EXPORT U32 maskJesdTxFifoErrors(int fd, U32 jesdNo,U32 maskValue);
API_EXPORT U32 maskAllJesdAlarms(int fd, U32 alarmNo, U32 maskValue);
API_EXPORT U32 maskSrPapAlarms(int fd, U32 alarmNo, U32 maskValue,U32 txChNo);
API_EXPORT U32 maskPllAlarms(int fd, U32 alarmNo, U32 pllNo, U32 maskValue);
API_EXPORT U32 maskSpiAlarms(int fd, U32 alarmNo, U32 maskValue);
API_EXPORT U32 rxDcBw(int fd, U32 ulChan, double rxOutputDataRate, double dcFilterBw);
API_EXPORT U32 tempSensorEnable(int fd, U32 Enable);
API_EXPORT U32 tempSensorReadOut(int fd);
API_EXPORT U32 RXDsaSwapAttnMacro(int fd, U32 ulChan, U32 ulRXDsa);
API_EXPORT U32 TXDsaSwapAttnMacro(int fd, U32 ulChan, U32 ulTXDsaA, U32 ulTXDsaB);
API_EXPORT U32 OnlyEnableInternalAgc(int fd, U32 ulChan, U32 ulEnable);
API_EXPORT U32 programLnaGainPhaseCorrection(int fd, U32 ulChan, U32 ulIndex, float gainValue, float phaseValue);
API_EXPORT U32 selectLnaCorrectionIndex(int fd, U32 ulChan, U32 ulIndex);
API_EXPORT U32 configurePll(int fd, U32 ulChan, double ulfreq, double ulRefFreq);
API_EXPORT U32 setPllLoFbNcoFreezeQec(int fd, double Fout, double Fref, int ulPll, int ulFbnco);
API_EXPORT U32 configurePLLmode(int fd, U32 ulPLLMode);
API_EXPORT U32 checkPllLockStatus(int fd, U32 ulChan);
API_EXPORT U32 findpllFrequency(int fd, U8 ulChan, double ulRefFreq);
API_EXPORT U32 findLoNcoPossibleFrequency(double ulfreq, double ulRefFreq);
API_EXPORT U32 getSerdesRxLaneEyeMarginValue(int fd, int laneNo);
API_EXPORT U32 enableSerdesRxPrbsCheck(int fd, int laneNo, int prbsMode, int enable);
API_EXPORT U32 getSerdesRxPrbsError(int fd, int laneNo);
API_EXPORT U32 sendSerdesTxPrbs(int fd, int laneNo, int prbsMode, int enable);
API_EXPORT U32 txSigGenTone(int fd, int channelNo, int enable, double power, double frequency);
API_EXPORT int loadTxDsaPacket(int fd, U8 *array, U16 arraySize);
API_EXPORT int loadRxDsaPacket(int fd, U8 *array, U16 arraySize);
API_EXPORT U32 FbNCO(int fd, double ulNcoFreq,U8 ucFbChan);
API_EXPORT U32 setLowIfNcoForAllTxFb(int fd, unsigned long int ulNcoFreq);
API_EXPORT U32 setPllLoFbNco(int fd, double Fout, double Fref);
API_EXPORT U32 readFbNco(int fd, int chNo);
API_EXPORT U32 configSrBlock(int fd,int chNo,int GainStepSize,int AttnStepSize,int AmplUpdateCycles,int threshold, int rampDownWaitTime, int rampUpWaitTime);
API_EXPORT U32 checkAllAlarms(int fd);
API_EXPORT U32 freezeTxIqmc(int fd,int freeze);
API_EXPORT U32 freezeTxLol(int fd,int freeze);
API_EXPORT U32 getSrAlarm(int fd,int chN0);
API_EXPORT U32 getIntAlarms(int fd,int chN0);
API_EXPORT U32 overrideAlarmPin(int fd,int alarmNo, int overrideSel, int overrideVal);
API_EXPORT U32 overrideRelDetPin(int fd,int chNo, int overrideSel, int overrideVal);
API_EXPORT U32 overrideDigPkDetPin(int fd,int chNo, int overrideSel, int overrideVal);
API_EXPORT U32 setTxDsaUpdateMode(int fd, int immediateUpdate, int delay);
API_EXPORT U32 getTxDsaModeConfig(int fd);
API_EXPORT U32 setTxDsaAttn(int fd, int dsaSetting, int chainSel);
API_EXPORT U32 RXDsaAttnOffset(int fd, U32 ulChan, U32 ulRXDsa);
API_EXPORT int sleepModeConfiguration(int fd, U8 rx_chain,U8 tx_chain,U8 fb_chain);
API_EXPORT int deepSleepModeConfiguration(int fd, U8 rx_chain,U8 tx_chain,U8 fb_chain,U8 pll,U8 serdes);
API_EXPORT void overrideSleep(int fd, int overrideEn, int overrideVal, int rxTDD, int txTDD, int fbTDD);
API_EXPORT void deepSleepEn(int fd, int sleepEn);
API_EXPORT void overrideRxTdd(int fd, int overrideEn, int overrideVal);
API_EXPORT int controlBranchMute(int fd, U8 rx_chain,U8 tx_chain,U8 mute);
API_EXPORT U32 configureFovr(int fd, U32 ulChan, U32 ulEnable, U32 ulAttackStepSize, double ulAttackThreshold, U64 ulWindowLen, U32 ulHitCount);
API_EXPORT U32 readFovrSettings(int fd, U32 ulChan);
API_EXPORT unsigned int rxDsaGainRead(int fd, U32 ch, U32 dsaLimit);
API_EXPORT unsigned int rxDsaPhaseRead(int fd, U32 ch, U32 dsaLimit);
API_EXPORT U32 adcJesdRamp(int fd, U32 RxFb, U32 en, U32 step);
API_EXPORT int setAdcJesdRampPattern(int fd,int chNo,int enable);
API_EXPORT U32 rxLolEn(int fd, U32 en, U32 ch);
API_EXPORT U32 rxQmcEn(int fd, U32 en, U32 ch);
API_EXPORT int twos_comp(int val,int bits);


#ifdef __cplusplus
}
#endif
#endif