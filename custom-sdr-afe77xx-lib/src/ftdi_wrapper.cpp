#include "ftdi_wrapper.h"
#include <chrono>
#include <thread>

#include "ftdi.h"

int FTDIRegProgrammer::set(void* handle)
{
	// char handleName[1024];
	// strcpy(handleName, name.c_str());
	//FT_STATUS Status = FTD2_OpenEx(name, FT_OPEN_BY_DESCRIPTION, &ftHandle);
	ftHandle = handle;
	if(handle == NULL) return FT_OTHER_ERROR;
	return FT_OK;
}


FTDIRegProgrammer::FTDIRegProgrammer(void* handle,int *status)
{
	ftStatus =  set(handle);
	if (ftStatus != FT_OK){
		/*std::cout<< name << " device not found" << std::endl;*/
		/*std::exit(0);*/
		*status=1;
		return;
	}
	clkBit = 0;
	enableBit = 3;
	dataBit = 1;
	maskArray.fill(1);
	valueArray.fill(0);
	addressLen = 16;
	packetLen = 24;
	dataFirst = 0;
	msbFirst = 1;
	enableHigh = 0;
	clkEdge = 1;
	dataOutBit = 2;
	setPin(2,6);
	readClkEdge = 1;
	readOutMode = 0;
	packetOrder = 0;
	FTD2_SetChars(ftHandle, 'a', 0, 'a', 0);
	setMask();
	// FT_SetBaudRate(ftHandle, 9600);
	// FT_SetDivisor(ftHandle, 0);
	// FT_SetTimeouts(ftHandle, 2000, 2000);
	// FT_SetLatencyTimer(ftHandle, 10);
}


std::string FTDIRegProgrammer::readFromFTDI(std::string data)
{
	//purge();
	char data_char[1024];
	// strcpy(data_char, data.c_str());
	for(int i = 0; i<data.size(); i++){
		data_char[i] = data[i];
	}
	DWORD bytesWritten;
	ftStatus = FTD2_Write(ftHandle, data_char, data.size(), &bytesWritten);
	if (ftStatus == FT_OK)
	{
		DWORD EventDWord, TxBytes, RxBytes, BytesReceived;
		char RxBuffer[256];
		 std::this_thread::sleep_for(std::chrono::milliseconds(20));
		ftStatus=FTD2_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
		//std::cout<<"TxBytes "<<TxBytes<<"RxBYtes "<< RxBytes<<std::endl;
		// while(RxBytes > 0 && ftStatus == FT_OK){
			// std::cout<< "TxBytes is greater than 0" << std::endl;
			// FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
		// }
		FTD2_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
		
		if(RxBytes > 0)
		{
			ftStatus = FTD2_Read(ftHandle,RxBuffer,RxBytes,&BytesReceived);
			if(ftStatus == FT_OK)
			{
				std::string retVal;
				for (int i = 0; i < RxBytes; i++)
				{
					retVal += RxBuffer[i];
				}
				return (retVal); 
			}

			std::cout << "Cannot read" << std::endl;
			
			return ("ERROR");

		}

		std::cout << "Nothing to read" << std::endl;

	std::string errorString = "ERROR";

	return errorString;
		
	}

	return "ERROR";

}


int FTDIRegProgrammer::writeToFTDI(std::string data)
{
	//std::cout << data << "data in write ftdi" << std::endl;
	char data_char[1024]; 
	// strcpy(data_char, data.c_str());
	for (int a = 0 ; a<data.size() ; a++){
		data_char[a] = data[a];
	}
	// data_char = "1010101010";
	// for (int a = 0 ; a<data.size() ; a++){
		// std::cout << int(data_char[a])<< " bit " <<myConvertToBin(int(data_char[a]),8) <<" data ";
		// std::cout << int(data[a]) << std::endl;
	// }
	data_char[data.size()] = '\0';
	// std::cout << data_char << "is sent to ftdi" << std::endl;
	std::string retVal;
	DWORD bytesWritten;
	FT_STATUS retStatus = FTD2_Write(ftHandle, data_char, data.size(), &bytesWritten);
	DWORD EventDWord, TxBytes, RxBytes;
	ftStatus = FTD2_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
	//std::cout<<"TxBytes "<<TxBytes<<"RxBYtes "<< RxBytes<<std::endl;
	// while(TxBytes > 0 && ftStatus == FT_OK){
		// std::cout<< "TxBytes is greater than 0" << std::endl;
		// FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
	// }
	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	// ftStatus = FT_Write(ftHandle, data_char, 11, &bytesWritten);

	//std::cout <<"Bytes written is " << bytesWritten <<std::endl;
	return retStatus;
}

int FTDIRegProgrammer::reset()
{
	clkBit = 0;
	enableBit = 2;
	dataBit = 1;
	maskArray.fill(1);
	valueArray.fill(0);
	addressLen = 8;
	packetLen = 16;
	dataFirst = 0;
	msbFirst = 0;
	enableHigh = 0;
	clkEdge = 0;
	dataOutBit = 0; 
	readClkEdge = 0;
	readOutMode = 0;
	packetOrder = 0;
	msbFirst = 0;

	ftStatus = FTD2_ResetDevice(ftHandle);
	return ftStatus;
}


int FTDIRegProgrammer::purge()
{
	ftStatus = FTD2_Purge(ftHandle, FT_PURGE_TX | FT_PURGE_RX);
	// int retVal = ftStatus;
	// DWORD EventDWord, TxBytes, RxBytes, BytesReceived;
	// ftStatus=FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
	// while(TxBytes > 0 && ftStatus == FT_OK){
		// std::cout<< "TxBytes is greater than 0" << std::endl;
		// FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
	// }
	// std::this_thread::sleep_for(std::chrono::milliseconds(400));
	// return retVal;
	return ftStatus;
}

int FTDIRegProgrammer::purgeRX()
{
	ftStatus = FTD2_Purge(ftHandle,FT_PURGE_RX);
	return ftStatus;
}

int FTDIRegProgrammer::purgeTX()
{
	ftStatus = FTD2_Purge(ftHandle, FT_PURGE_TX);
	return ftStatus;
}

FTDIRegProgrammer::~FTDIRegProgrammer()
{
	//If there is an error, we don't care about it
	//FTD2_Close(ftHandle);
}

int FTDIRegProgrammer::close()
{
	//If there is an error, we don't care about it
	ftStatus=FTD2_Close(ftHandle);
	return ftStatus;
}

int FTDIRegProgrammer::setPin(int index, int val)
{
	if(clkBit == index)
		clkBit = -1;
	if(dataBit == index)
		dataBit = -1;
	if(enableBit == index)
		enableBit = -1;
	if(dataOutBit == index)
		dataOutBit = -1;

	if(val == 0)
		clkBit = index;
	if(val == 1)
		dataBit = index;
	if(val==2)
		enableBit = index;

	if(val ==3)
		valueArray[index] = 0;

	if(val == 4)
		valueArray[index] = 1;

	if(val == 6)
		dataOutBit = index;

	if(val < 5)
	{
		maskArray[index] = 1;
	}
	else
	{
		maskArray[index] = 0;
	}

	setMask();
	writeValue();
	return ftStatus;
}

int FTDIRegProgrammer::setMask()
{
	UCHAR mask = convertBinaryArrayToInt<UCHAR>(maskArray);
	//std::cout << "mask being set : " <<myConvertToBin(mask,8 )<< std::endl;
	ftStatus = FTD2_SetBitmode(ftHandle, mask, 0x4);
	if (ftStatus != FT_OK){
		std::cout << "mask and mode not set properly functionality may not come correct" << std::endl;
		UCHAR BitMode;
		FT_STATUS getStatus = FTD2_GetBitMode(ftHandle, &BitMode);
		if (getStatus != FT_OK){
		std::cout << "[error] : mask and mode not get properly please reconnect FTDI" << std::endl;
		}
		std::cout << "bit mode is  " << myConvertToBin(BitMode,0 ) << std::endl;
	}
		
	return ftStatus;
	
}

int FTDIRegProgrammer::writeValue()
{
	int v = convertBinaryArrayToInt<int>(valueArray);
	std::string toWrite = std::to_string(v);
	writeToFTDI(toWrite);
	return ftStatus;
}

int FTDIRegProgrammer::setWritePacket(int val)
{
	if(clkBit < 0 || dataBit < 0 || enableBit < 0)
		return FT_OTHER_ERROR;
	std::string data = (myConvertToBin(val, 0));
	return setWritePacketString(data);
}

int FTDIRegProgrammer::setWritePacketString(std::string data)
{
	// std::cout << "enable:clk:data" << enableBit << clkBit << dataBit << std::endl;
	if(msbFirst == 0)
	{
		reverse(data.begin(),data.end());
	}

	int clkFirstState = int(!clkEdge);
	int clkSecondState = clkEdge;

	valueArray[enableBit] = enableHigh;
	valueArray[clkBit] = 0;
	valueArray[dataBit] = 0;

	std::string stringArray = "";

	stringArray += char(convertBinaryArrayToInt<int>(valueArray));

	for (auto a:data)
	{
		valueArray[clkBit] = clkFirstState;
		valueArray[dataBit] = int(a) - int('0');
		stringArray += char(convertBinaryArrayToInt<int>(valueArray));
		valueArray[clkBit] = clkSecondState;
		stringArray += char(convertBinaryArrayToInt<int>(valueArray));
	}

	valueArray[dataBit] = 0;
	valueArray[clkBit] = 0;
	stringArray += char(convertBinaryArrayToInt<int>(valueArray));
	valueArray[enableBit] = !enableHigh;
	stringArray += char(convertBinaryArrayToInt<int>(valueArray));
	// std::cout << "Writing "; 
	// for (auto a:stringArray)
	// {
		// std::cout << int(a);
	// }
	// std::cout <<std::endl<< " to FTDI string array" << stringArray << std::endl;
	ftStatus = writeToFTDI(stringArray);
	return ftStatus;
}

int FTDIRegProgrammer::rawWritePins(int data)
{
	std::string s = std::to_string(char(data));
	int ret =  writeToFTDI(s);
	return ret;
}

int FTDIRegProgrammer::writeReg(int addr, int val)
{
	std::string addresstoWrite = myConvertToBin(addr, addressLen);
	std::string valueToWrite = myConvertToBin(val, packetLen - addressLen);
	std::string data;
	if(packetOrder)
	{
		data = valueToWrite + addresstoWrite;
	}
	else
	{
		data = addresstoWrite + valueToWrite;
	}

	return setWritePacketString(data);

}

int FTDIRegProgrammer::readReg(int addr)
{
	if(clkBit < 0 || dataBit < 0 || enableBit < 0)
	{
		std::cout << "One of the bits is not set. Check" << std::endl; 
		return FT_OTHER_ERROR;
	}
	std::string data = myConvertToBin(addr, addressLen);
	//for (auto i :data){
	// 	std::cout << i ;
	//}

	//std::cout << "this is data in bin " << std::endl;
	if(msbFirst == 0)
		reverse(data.begin(),data.end());

	int clkFirstState = int(!clkEdge);
	int clkSecondState = clkEdge;
	valueArray[enableBit] = enableHigh;
	valueArray[clkBit] = 0;
	valueArray[dataBit] = 0;
	std::string stringArray = "";

	stringArray += char(convertBinaryArrayToInt<int>(valueArray));

	for (auto a:data)
	{
		//std::cout << (int(a-'0'))  << " data "  << std::endl;
		valueArray[clkBit] = clkFirstState;
		valueArray[dataBit] = int(a - '0');
		stringArray += char(convertBinaryArrayToInt<int>(valueArray));
		valueArray[clkBit] = clkSecondState;
		stringArray += char(convertBinaryArrayToInt<int>(valueArray));
	}
	
	clkFirstState = int(!readClkEdge);
	clkSecondState = readClkEdge;
	valueArray[clkBit] = 0;
	valueArray[dataBit] = 0;
	stringArray += char(convertBinaryArrayToInt<int>(valueArray));
	for(auto i = 0; i < (packetLen - addressLen); i++)
	{
		if(readOutMode)
		{
			valueArray[clkBit] = clkFirstState;
			valueArray[dataBit] = i%2;
			stringArray += char(convertBinaryArrayToInt<int>(valueArray));
			valueArray[clkBit] = clkSecondState;
			stringArray += char(convertBinaryArrayToInt<int>(valueArray));
		}
		else
		{
			valueArray[clkBit] = clkFirstState;
			valueArray[dataBit] = i%2;
			stringArray += char(convertBinaryArrayToInt<int>(valueArray));
			valueArray[clkBit] = clkSecondState;
			stringArray += char(convertBinaryArrayToInt<int>(valueArray));
		}

	}

	valueArray[clkBit] = 0;
	valueArray[dataBit] = 0;
	stringArray += char(convertBinaryArrayToInt<int>(valueArray));
	valueArray[enableBit] = !(enableHigh);
	stringArray += char(convertBinaryArrayToInt<int>(valueArray));
	setMask();
	ftStatus=purge();
	if(ftStatus){
		std::cout<<"some error in purging"<<std::endl;
	}else{
		//std::cout<<"correctly purging 11"<<std::endl;
	}
	purgeRX();

	std::string output = readFromFTDI(stringArray);
	std::string r = output.substr(output.size() - 2 + readClkEdge-2*(packetLen -addressLen), 2*(packetLen - addressLen));
	std::string ret = "";
	for (int i = 0; i < r.size(); i+=2)
	{
		// std::cout << myConvertToBin(int(r[i]),8)<<"   "<<int(r[i])<< " r of i dataout :"<< dataOutBit<< std::endl;
		ret += std::to_string((int(r[i]) >> dataOutBit) & 1);
	}
	// std::cout << "ret string is : " << ret << std::endl;
	// return bin2dec(ret);
	// return convertBinaryArrayToInt<int>(retArray);
	return stoi(ret, 0, 2);


}

void FTDIRegProgrammer::setAddressLen(int value)
{
	addressLen = value;
}

void FTDIRegProgrammer::setPacketLen(int value)
{
	packetLen = value;
}

void FTDIRegProgrammer::setPacketOrder(int value)
{
	dataFirst = value;
}

void FTDIRegProgrammer::setMsbFirst(int value)
{
	msbFirst= value;
}

void FTDIRegProgrammer::setEnableType(int value)
{
	enableHigh = value;
}

void FTDIRegProgrammer::setClkEdge(int value)
{
	clkEdge = value;
}

std::string FTDIRegProgrammer::readSerialData(int size)
{
	purge();
	if(clkBit== -1 || enableBit == -1)
	{
		std::cout << "Either Clock or enable bit is not set. Cannot write the packet" << std::endl;
		return std::to_string(FT_OTHER_ERROR);
	}

	int clkFirstState = !(clkEdge);
	int clkSecondState = clkEdge;

	valueArray[enableBit] = enableHigh;
	valueArray[clkBit] = 0;
	std::string stringArray = "";
	stringArray += char(convertBinaryArrayToInt<int>(valueArray));
	for (int i= 0; i < size; i++)
	{
		valueArray[clkBit] = clkFirstState;
		stringArray += char(convertBinaryArrayToInt<int>(valueArray));
		valueArray[clkBit] = clkSecondState;
		stringArray += char(convertBinaryArrayToInt<int>(valueArray));
		valueArray[clkBit] = 0;
		valueArray[dataBit] = 0;
		stringArray += char(convertBinaryArrayToInt<int>(valueArray));
	}

	setMask();
	return readFromFTDI(stringArray);

}

int FTDIRegProgrammer::setBaudRate(uint32_t baudRate)
{
	return FTD2_SetBaudRate(ftHandle, baudRate);
}

int FTDIRegProgrammer::setDivisor(uint8_t divisor)
{
	return FTD2_SetDivisor(ftHandle, divisor);
}