#pragma once

enum customsdr_rfic_channel 
{
	CUSTOMSDR_RFIC_RX1 = 0, CUSTOMSDR_RFIC_RX2, CUSTOMSDR_RFIC_RX3, CUSTOMSDR_RFIC_RX4,
	CUSTOMSDR_RFIC_RX5 = 4, CUSTOMSDR_RFIC_RX6, 
	CUSTOMSDR_RFIC_TX1 = 16, CUSTOMSDR_RFIC_TX2, CUSTOMSDR_RFIC_TX3, _RFIC_TX4
};
enum customsdr_rfic_sample_rate
{
	CUSTOMSDR_RFIC_SR_61_44M = 0,  CUSTOMSDR_RFIC_SR_92_16M = 1, 
	CUSTOMSDR_RFIC_SR_122_88M = 2, CUSTOMSDR_RFIC_SR_184_32M = 3,
	CUSTOMSDR_RFIC_SR_245_76M = 4, CUSTOMSDR_RFIC_SR_368_64M = 5, 
	CUSTOMSDR_RFIC_SR_491_52M = 6, CUSTOMSDR_RFIC_SR_737_28M = 7
};
enum customsdr_rfic_bandwidth 
{
	CUSTOMSDR_RFIC_AFLT_100M = 0, 
	CUSTOMSDR_RFIC_AFLT_200M = 1, 
	CUSTOMSDR_RFIC_AFLT_300M = 2
};
enum customsdr_rfic_low_if 
{
	CUSTOMSDR_RFIC_LOW_IF_x1 = 0, CUSTOMSDR_RFIC_LOW_IF_x2 = 1, 
	CUSTOMSDR_RFIC_LOW_IF_x3 = 2, CUSTOMSDR_RFIC_LOW_IF_x1_5 = 3
};
enum customsdr_rfic_band
{	
	CUSTOMSDR_RFIC_BAND_8_U,  CUSTOMSDR_RFIC_BAND_8_D,  CUSTOMSDR_RFIC_BAND_20_U, CUSTOMSDR_RFIC_BAND_20_D,
	CUSTOMSDR_RFIC_BAND_26_U, CUSTOMSDR_RFIC_BAND_26_D, CUSTOMSDR_RFIC_BAND_28_U, CUSTOMSDR_RFIC_BAND_28_D,
	CUSTOMSDR_RFIC_BAND_5_U,  CUSTOMSDR_RFIC_BAND_5_D,  CUSTOMSDR_RFIC_BAND_44,
	CUSTOMSDR_RFIC_BAND_3_U,  CUSTOMSDR_RFIC_BAND_3_D,  CUSTOMSDR_RFIC_BAND_1_U,  CUSTOMSDR_RFIC_BAND_1_D,
	CUSTOMSDR_RFIC_BAND_65_U, CUSTOMSDR_RFIC_BAND_65_D, CUSTOMSDR_RFIC_BAND_25_U, CUSTOMSDR_RFIC_BAND_25_D,
	CUSTOMSDR_RFIC_BAND_33,   CUSTOMSDR_RFIC_BAND_34,   CUSTOMSDR_RFIC_BAND_39, 
	CUSTOMSDR_RFIC_BAND_7_U,  CUSTOMSDR_RFIC_BAND_7_D,  
	CUSTOMSDR_RFIC_BAND_38,   CUSTOMSDR_RFIC_BAND_40,	CUSTOMSDR_RFIC_BAND_41
};
enum customsdr_rfic_gain_mode 
{
	CUSTOMSDR_GAIN_MANUAL, CUSTOMSDR_GAIN_DYNAMIC_MANUAL,
	CUSTOMSDR_GAIN_AGC
};

typedef struct 
{
} customsdr_rfic_agc_param;