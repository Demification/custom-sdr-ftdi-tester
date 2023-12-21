
#ifndef FTDI_H
#define FTDI_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef FTDIMPSSE_API
#ifdef _WIN32
	// The following ifdef block is the standard way of creating macros
	// which make exporting from a DLL simpler.  All files within this DLL
	// are compiled with the FTDIMPSSE_EXPORTS symbol defined on the command line.
	// This symbol should not be defined on any project that uses this DLL.
	// This way any other project whose source files include this file see
	// FTDIMPSSE_API functions as being imported from a DLL, whereas this DLL
	// sees symbols defined with this macro as being exported.

#ifdef FTDIMPSSE_EXPORTS
#define FTDIMPSSE_API __declspec(dllexport)
#elif defined(FTDIMPSSE_STATIC)
	// Avoid decorations when linking statically.
#define FTDIMPSSE_API
#else // FTDIMPSSE_EXPORTS
#define FTDIMPSSE_API __declspec(dllimport)
#endif // FTDIMPSSE_EXPORTS

#else // _WIN32

	// Compiling on non-Windows platform.
#include "wintypes.h"
#include "ftd2xx.h"
	// No decorations needed.
#define FTDIMPSSE_API

#endif // _WIN32
#endif // FTDIMPSSE_API

FTDIMPSSE_API FT_STATUS  FTD2_GetLibraryVersion(LPDWORD lpdwVersion);
FTDIMPSSE_API FT_STATUS  FTD2_GetNumChannel(LPDWORD lpdwNumDevs);
FTDIMPSSE_API FT_STATUS  FTD2_GetDeviceInfoList(FT_DEVICE_LIST_INFO_NODE *pDest, LPDWORD lpdwNumDevs);
FTDIMPSSE_API FT_STATUS  FTD2_Open (int iDevice, FT_HANDLE *ftHandle);
FTDIMPSSE_API FT_STATUS  FTD2_Close (FT_HANDLE ftHandle);
FTDIMPSSE_API FT_STATUS  FTD2_ResetDevice (FT_HANDLE ftHandle);
FTDIMPSSE_API FT_STATUS  FTD2_Purge (FT_HANDLE ftHandle, DWORD dwMask);
FTDIMPSSE_API FT_STATUS  FTD2_SetUSBParameters (FT_HANDLE ftHandle, DWORD dwInTransferSize, DWORD dwOutTransferSize);
FTDIMPSSE_API FT_STATUS  FTD2_SetChars (FT_HANDLE ftHandle, UCHAR uEventCh, UCHAR uEventChEn, UCHAR uErrorCh, UCHAR uErrorChEn);
FTDIMPSSE_API FT_STATUS  FTD2_SetTimeouts (FT_HANDLE ftHandle, DWORD dwReadTimeout, DWORD dwWriteTimeout);
FTDIMPSSE_API FT_STATUS  FTD2_SetLatencyTimer (FT_HANDLE ftHandle, UCHAR ucTimer);
FTDIMPSSE_API FT_STATUS  FTD2_GetLatencyTimer (FT_HANDLE ftHandle, UCHAR *ucTimer);
FTDIMPSSE_API FT_STATUS  FTD2_SetBitmode (FT_HANDLE ftHandle, UCHAR ucMask, UCHAR ucMode);
FTDIMPSSE_API FT_STATUS  FTD2_GetQueueStatus (FT_HANDLE ftHandle, LPDWORD lpdwAmountInRxQueue);
FTDIMPSSE_API FT_STATUS  FTD2_Read (FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD dwBytesToRead, LPDWORD lpdwBytesReturned);
FTDIMPSSE_API FT_STATUS  FTD2_Write (FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD dwBytesToWrite, LPDWORD lpdwBytesWritten);
FTDIMPSSE_API FT_STATUS  FTD2_GetDeviceInfo(FT_HANDLE ftHandle, FT_DEVICE *lpftDevice, LPDWORD lpdwID, PCHAR SerialNumber, PCHAR Description, LPVOID Dummy);
FTDIMPSSE_API FT_STATUS  FTD2_CreateDeviceInfoList(LPDWORD lpdwNumDevs);
FTDIMPSSE_API FT_STATUS  FTD2_OpenEx(PVOID pArg1, DWORD Flags, FT_HANDLE *pHandle);

#ifdef __cplusplus
}
#endif

#endif	/*FTDI_H*/


