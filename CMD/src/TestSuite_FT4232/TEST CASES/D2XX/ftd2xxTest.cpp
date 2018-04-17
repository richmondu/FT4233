#include "ftd2xx.h"
#include "ftdiIds.h"
#include "../../TEST SUITE/TestSuite.h"
#include "../../TEST SUITE/TestLogger.h"
#pragma comment(lib, "ftd2xx.lib")
#include <stdio.h>



bool FT4232_D2XX_Info() {

	DWORD dwDLLVersion = 0;
	FT_STATUS ftStatus = FT_GetLibraryVersion(&dwDLLVersion);
	if (ftStatus != FT_OK) {
		CMD_LOG("FT_GetLibraryVersion failed! Cannot get library version!\n");
		return false;
	}
	CMD_LOG("D2XX Library version=0x%08x\n", dwDLLVersion);

	return true;
}

bool FT4232_D2XX_Enumerate() {

	FT_STATUS ftStatus = 0;
	boolean bFound = false;

	DWORD dwNumDevices = 0;
	ftStatus = FT_CreateDeviceInfoList(&dwNumDevices);
	if (ftStatus != FT_OK) {
		CMD_LOG("FT_CreateDeviceInfoList failed! No devices found!\n");
		return false;
	}

	CMD_LOG("DeviceCount=%d\n", dwNumDevices);
	if (!dwNumDevices) {
		CMD_LOG("FT_CreateDeviceInfoList failed! No devices found! dwNumDevices=%d\n", dwNumDevices);
		return false;
	}

	for (DWORD i = 0; i<dwNumDevices; ++i) {
		FT_DEVICE_LIST_INFO_NODE devInfo;
		memset(&devInfo, 0, sizeof(devInfo));

		ftStatus = FT_GetDeviceInfoDetail(i, &devInfo.Flags, &devInfo.Type,
			&devInfo.ID, &devInfo.LocId,
			devInfo.SerialNumber,
			devInfo.Description,
			&devInfo.ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_GetDeviceInfoDetail failed!\n");
			return false;
		}

		CMD_LOG("Dev %d:\n", i);
		CMD_LOG("  ID=0x%08x (%s)\n", devInfo.ID, FT_GetVidPidString(devInfo.ID));
		CMD_LOG("  LocId=0x%08x\n", devInfo.LocId);
		CMD_LOG("  Type=0x%08x\n", devInfo.Type);
		CMD_LOG("  Flags=0x%08x\n", devInfo.Flags);
		CMD_LOG("  SerialNumber=%s\n", devInfo.SerialNumber);
		CMD_LOG("  Description=%s\n", devInfo.Description);

		/*
		if (!FT_FindVidPid(devInfo.ID)) {
			CMD_LOG("FT_GetDeviceInfoDetail failed!\n");
			return false;
		}
		*/

		if (DeviceNameIsSet()) {
			if (DeviceNameCompare(devInfo.Description)) {
				bFound = true;
				CMD_LOG("  Specified device is found!\n");
			}
		}
	}

	if (DeviceNameIsSet()) {
		if (!bFound) {
			CMD_LOG("Specified device is NOT found!\n");
			DeviceNameNotFoundAbort();
			return false;
		}
	}


	return true;
}

bool FT4232_D2XX_Open() {

	FT_STATUS ftStatus = 0;
	FT_HANDLE ftHandle[4] = { NULL };

	DWORD dwNumDevices = 0;
	ftStatus = FT_CreateDeviceInfoList(&dwNumDevices);
	if (ftStatus != FT_OK) {
		CMD_LOG("FT_CreateDeviceInfoList failed! No devices found!\n");
		return false;
	}
	if (!dwNumDevices) {
		CMD_LOG("FT_CreateDeviceInfoList failed! No devices found! dwNumDevices=%d\n", dwNumDevices);
		return false;
	}

	for (DWORD i = 0; i<dwNumDevices; ++i) {
		FT_DEVICE_LIST_INFO_NODE devInfo;
		memset(&devInfo, 0, sizeof(devInfo));

		ftStatus = FT_GetDeviceInfoDetail(i, &devInfo.Flags, &devInfo.Type,
			&devInfo.ID, &devInfo.LocId,
			devInfo.SerialNumber,
			devInfo.Description,
			&devInfo.ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_GetDeviceInfoDetail failed!\n");
			return false;
		}

		if (DeviceNameIsSet()) {
			if (!DeviceNameCompare(devInfo.Description)) {
				continue;
			}
		}

		CMD_LOG("Dev %d:\n", i);
		CMD_LOG("  ID=0x%08x (%s)\n", devInfo.ID, FT_GetVidPidString(devInfo.ID));
		CMD_LOG("  Description=%s\n", devInfo.Description);

		ftStatus = FT_OpenEx(devInfo.SerialNumber, FT_OPEN_BY_SERIAL_NUMBER, &ftHandle[i]);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_OpenEx failed! Cannot open device by SerialNumber=%s\n", devInfo.SerialNumber);
			return false;
		}
		CMD_LOG("  Open device by SerialNumber=%s\n", devInfo.SerialNumber);
		FT_Close(ftHandle[i]);
		ftHandle[i] = NULL;

		ftStatus = FT_OpenEx(devInfo.Description, FT_OPEN_BY_DESCRIPTION, &ftHandle[i]);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_OpenEx failed! Cannot open device by Description=%s", devInfo.Description);
			return false;
		}
		CMD_LOG("  Open device by Description=%s\n", devInfo.Description);
		FT_Close(ftHandle[i]);
		ftHandle[i] = NULL;

		ftStatus = FT_Open(i, &ftHandle[i]);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_Open failed! Cannot open device by Index=%d", i);
			return false;
		}
		DWORD dwDriverVersion = 0;
		ftStatus = FT_GetDriverVersion(ftHandle[i], &dwDriverVersion);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_GetDriverVersion failed! Cannot get driver version!\n");
			return false;
		}
		CMD_LOG("  Open device by Index=%d\n", i);
		FT_Close(ftHandle[i]);
		ftHandle[i] = NULL;

		CMD_LOG("  Driver version=0x%08x\n", dwDriverVersion);
	}

	return true;
}

bool FT4232_D2XX_ResetPort() 
{
	FT_STATUS ftStatus = 0;
	FT_HANDLE ftHandle[4] = { NULL };

	DWORD dwNumDevices = 0;
	ftStatus = FT_CreateDeviceInfoList(&dwNumDevices);
	if (ftStatus != FT_OK) {
		CMD_LOG("FT_CreateDeviceInfoList failed! No devices found!\n");
		return false;
	}
	if (!dwNumDevices) {
		CMD_LOG("FT_CreateDeviceInfoList failed! No devices found! dwNumDevices=%d\n", dwNumDevices);
		return false;
	}

	for (DWORD i = 0; i<dwNumDevices; ++i) {
		FT_DEVICE_LIST_INFO_NODE devInfo;
		memset(&devInfo, 0, sizeof(devInfo));

		ftStatus = FT_GetDeviceInfoDetail(i, &devInfo.Flags, &devInfo.Type,
			&devInfo.ID, &devInfo.LocId,
			devInfo.SerialNumber,
			devInfo.Description,
			&devInfo.ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_GetDeviceInfoDetail failed!\n");
			return false;
		}

		if (DeviceNameIsSet()) {
			if (!DeviceNameCompare(devInfo.Description)) {
				continue;
			}
		}

		CMD_LOG("Dev %d:\n", i);
		CMD_LOG("  ID=0x%08x (%s)\n", devInfo.ID, FT_GetVidPidString(devInfo.ID));
		CMD_LOG("  Description=%s\n", devInfo.Description);

		ftStatus = FT_OpenEx(devInfo.Description, FT_OPEN_BY_DESCRIPTION, &ftHandle[i]);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_OpenEx failed! Cannot open device by Description=%s", devInfo.Description);
			return false;
		}

		ftStatus = FT_ResetPort(ftHandle[i]);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_ResetPort failed! Cannot reset port");
			return false;
		}

		CMD_LOG("  Reset port\n");

		DWORD dwDriverVersion = 0;
		ftStatus = FT_GetDriverVersion(ftHandle[i], &dwDriverVersion);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_GetDriverVersion failed! Device not working after reset port!\n");
			return false;
		}

		CMD_LOG("  Device is functional after reset port\n");

		FT_Close(ftHandle[i]);
		ftHandle[i] = NULL;
	}

	return true;
}

bool FT4232_D2XX_CyclePort()
{
	FT_STATUS ftStatus = 0;
	FT_HANDLE ftHandle[4] = { NULL };

	DWORD dwNumDevices = 0;
	ftStatus = FT_CreateDeviceInfoList(&dwNumDevices);
	if (ftStatus != FT_OK) {
		CMD_LOG("FT_CreateDeviceInfoList failed! No devices found!\n");
		return false;
	}
	if (!dwNumDevices) {
		CMD_LOG("FT_CreateDeviceInfoList failed! No devices found! dwNumDevices=%d\n", dwNumDevices);
		return false;
	}

	for (DWORD i = 0; i<dwNumDevices; ++i) {
		FT_DEVICE_LIST_INFO_NODE devInfo;
		memset(&devInfo, 0, sizeof(devInfo));

		ftStatus = FT_GetDeviceInfoDetail(i, &devInfo.Flags, &devInfo.Type,
			&devInfo.ID, &devInfo.LocId,
			devInfo.SerialNumber,
			devInfo.Description,
			&devInfo.ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_GetDeviceInfoDetail failed!\n");
			return false;
		}

		if (DeviceNameIsSet()) {
			if (!DeviceNameCompare(devInfo.Description)) {
				continue;
			}
		}

		CMD_LOG("Dev %d:\n", i);
		CMD_LOG("  ID=0x%08x (%s)\n", devInfo.ID, FT_GetVidPidString(devInfo.ID));
		CMD_LOG("  Description=%s\n", devInfo.Description);

		ftStatus = FT_OpenEx(devInfo.Description, FT_OPEN_BY_DESCRIPTION, &ftHandle[i]);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_OpenEx failed! Cannot open device by Description=%s", devInfo.Description);
			return false;
		}

		ftStatus = FT_CyclePort(ftHandle[i]);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_CyclePort failed! Cannot cycle port");
			return false;
		}
		FT_Close(ftHandle[i]);
		ftHandle[i] = NULL;

		CMD_LOG("  Cycle port\n");
		Sleep(3000);

		dwNumDevices = 0;
		while (1) {
			ftStatus = FT_ListDevices(&dwNumDevices, NULL, FT_LIST_NUMBER_ONLY);
			if (ftStatus != FT_OK) {
				CMD_LOG("FT_ListDevices failed after cycle port!\n");
				return false;
			}
			if (dwNumDevices == 0) {
				continue;
			}
			break;
		}

		CMD_LOG("  Device has reenumerated after cycle port\n");
		Sleep(3000);
		ftStatus = FT_CreateDeviceInfoList(&dwNumDevices);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_CreateDeviceInfoList failed after cycle port!\n");
			return false;
		}

		memset(&devInfo, 0, sizeof(devInfo));
		ftStatus = FT_GetDeviceInfoDetail(i, &devInfo.Flags, &devInfo.Type,
			&devInfo.ID, &devInfo.LocId,
			devInfo.SerialNumber,
			devInfo.Description,
			&devInfo.ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_GetDeviceInfoDetail failed after cycle port!\n");
			return false;
		}

		if (DeviceNameIsSet()) {
			if (!DeviceNameCompare(devInfo.Description)) {
				continue;
			}
		}

		CMD_LOG("  ID=0x%08x (%s)\n", devInfo.ID, FT_GetVidPidString(devInfo.ID));
		CMD_LOG("  Description=%s\n", devInfo.Description);

		/*
		if (!FT_FindVidPid(devInfo.ID)) {
			CMD_LOG("FT_GetDeviceInfoDetail failed after cycle port! Invalid ID detected!\n");
			return false;
		}
		*/

		ftStatus = FT_OpenEx(devInfo.Description, FT_OPEN_BY_DESCRIPTION, &ftHandle[i]);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_OpenEx failed after cycle port! Cannot open device by Description=%s", devInfo.Description);
			return false;
		}

		CMD_LOG("  Device can be opened after cycle port\n");
		FT_Close(ftHandle[i]);
		ftHandle[i] = NULL;
	}

	return true;
}

bool FT4232_D2XX_ResetDevice()
{
	FT_STATUS ftStatus = 0;
	FT_HANDLE ftHandle[4] = { NULL };

	DWORD dwNumDevices = 0;
	ftStatus = FT_CreateDeviceInfoList(&dwNumDevices);
	if (ftStatus != FT_OK) {
		CMD_LOG("FT_CreateDeviceInfoList failed! No devices found!\n");
		return false;
	}
	if (!dwNumDevices) {
		CMD_LOG("FT_CreateDeviceInfoList failed! No devices found! dwNumDevices=%d\n", dwNumDevices);
		return false;
	}

	for (DWORD i = 0; i<dwNumDevices; ++i) {
		FT_DEVICE_LIST_INFO_NODE devInfo;
		memset(&devInfo, 0, sizeof(devInfo));

		ftStatus = FT_GetDeviceInfoDetail(i, &devInfo.Flags, &devInfo.Type,
			&devInfo.ID, &devInfo.LocId,
			devInfo.SerialNumber,
			devInfo.Description,
			&devInfo.ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_GetDeviceInfoDetail failed!\n");
			return false;
		}

		if (DeviceNameIsSet()) {
			if (!DeviceNameCompare(devInfo.Description)) {
				continue;
			}
		}

		CMD_LOG("Dev %d:\n", i);
		CMD_LOG("  ID=0x%08x (%s)\n", devInfo.ID, FT_GetVidPidString(devInfo.ID));
		CMD_LOG("  Description=%s\n", devInfo.Description);

		ftStatus = FT_OpenEx(devInfo.Description, FT_OPEN_BY_DESCRIPTION, &ftHandle[i]);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_OpenEx failed! Cannot open device by Description=%s", devInfo.Description);
			return false;
		}

		ftStatus = FT_ResetDevice(ftHandle[i]);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_ResetDevice failed! Cannot reset device!");
			return false;
		}
		FT_Close(ftHandle[i]);
		ftHandle[i] = NULL;

		CMD_LOG("  Reset device\n");
		//Sleep(3000);

		dwNumDevices = 0;
		while (1) {
			ftStatus = FT_ListDevices(&dwNumDevices, NULL, FT_LIST_NUMBER_ONLY);
			if (ftStatus != FT_OK) {
				CMD_LOG("FT_ListDevices failed after reset device!\n");
				return false;
			}
			if (dwNumDevices == 0) {
				continue;
			}
			break;
		}

		CMD_LOG("  Device has reenumerated after reset device\n");
		//Sleep(3000);

		ftStatus = FT_CreateDeviceInfoList(&dwNumDevices);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_CreateDeviceInfoList failed after reset device!\n");
			return false;
		}

		memset(&devInfo, 0, sizeof(devInfo));
		ftStatus = FT_GetDeviceInfoDetail(i, &devInfo.Flags, &devInfo.Type,
			&devInfo.ID, &devInfo.LocId,
			devInfo.SerialNumber,
			devInfo.Description,
			&devInfo.ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_GetDeviceInfoDetail failed after reset device!\n");
			return false;
		}

		if (DeviceNameIsSet()) {
			if (!DeviceNameCompare(devInfo.Description)) {
				continue;
			}
		}

		CMD_LOG("  ID=0x%08x (%s)\n", devInfo.ID, FT_GetVidPidString(devInfo.ID));
		CMD_LOG("  Description=%s\n", devInfo.Description);

		/*
		if (!FT_FindVidPid(devInfo.ID)) {
			CMD_LOG("FT_GetDeviceInfoDetail failed after reset device! Invalid ID detected!\n");
			return false;
		}
		*/

		ftStatus = FT_OpenEx(devInfo.Description, FT_OPEN_BY_DESCRIPTION, &ftHandle[i]);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_OpenEx failed after reset device! Cannot open device by Description=%s", devInfo.Description);
			return false;
		}

		CMD_LOG("  Device can be opened after reset device\n");
		FT_Close(ftHandle[i]);
		ftHandle[i] = NULL;
	}

	return true;
}

bool FT4232_D2XX_UART()
{
	return true;
}

bool FT4232_D2XX_GPIO()
{
#if 0
	FT_STATUS ftStatus = 0;
	FT_HANDLE ftHandle[4] = { NULL };

	DWORD dwNumDevices = 0;
	ftStatus = FT_CreateDeviceInfoList(&dwNumDevices);
	if (ftStatus != FT_OK) {
		CMD_LOG("FT_CreateDeviceInfoList failed! No devices found!\n");
		return false;
	}

	if (dwNumDevices != 4) {
		CMD_LOG("FT_CreateDeviceInfoList failed! A FT_4232 should have 4 devices enumerated!\n");
		return false;
	}

	for (DWORD i = 0; i<dwNumDevices; ++i) {
		if (i != 3) {
			continue;
		}

		FT_DEVICE_LIST_INFO_NODE devInfo;
		memset(&devInfo, 0, sizeof(devInfo));

		ftStatus = FT_GetDeviceInfoDetail(i, &devInfo.Flags, &devInfo.Type,
			&devInfo.ID, &devInfo.LocId,
			devInfo.SerialNumber,
			devInfo.Description,
			&devInfo.ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_GetDeviceInfoDetail failed!\n");
			return false;
		}

		CMD_LOG("Dev %d:\n", i);

		ftStatus = FT_OpenEx(devInfo.Description, FT_OPEN_BY_DESCRIPTION, &ftHandle[i]);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_OpenEx failed! Cannot open device by Description=%s", devInfo.Description);
			return false;
		}

		DWORD dwBytesWritten;
		DWORD dwBytesRead;
		DWORD dwBufIdx;

		unsigned char dir = 0;
		unsigned char value = 0;;
		unsigned char ucBuffer[3];

		dwBufIdx = 0;
		ucBuffer[dwBufIdx++] = 0x82;
		ucBuffer[dwBufIdx++] = value;
		ucBuffer[dwBufIdx++] = dir;
		dwBytesWritten = 0;

		ftStatus = FT_Write(ftHandle[i], ucBuffer, dwBufIdx, &dwBytesWritten);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_Write failed! ftStatus = 0x%08x\n", ftStatus);
			FT_Close(ftHandle[i]);
			return false;
		}
		CMD_LOG("  FT_WriteGPIO value=%02x dir=%02x\n", value, dir);


		dwBufIdx = 0;
		ucBuffer[dwBufIdx++] = 0x83;
		ucBuffer[dwBufIdx++] = 0x87;
		ucBuffer[dwBufIdx] = 0x0;
		dwBytesWritten = 0;

		ftStatus = FT_Write(ftHandle[i], ucBuffer, dwBufIdx, &dwBytesWritten);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_Write failed! ftStatus = 0x%08x\n", ftStatus);
			FT_Close(ftHandle[i]);
			return false;
		}
		CMD_LOG("  FT_WriteGPIO value=%02x dir=%02x\n", value, dir);

		dwBytesRead = 0;
		UCHAR readBuffer[10] = { 0 };
		dwBufIdx = 1;
		ftStatus = FT_Read(ftHandle[i], readBuffer, dwBufIdx, &dwBytesRead);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_Read failed! ftStatus = 0x%08x\n", ftStatus);
			FT_Close(ftHandle[i]);
			return false;
		}
		value = readBuffer[0];
		CMD_LOG("  FT_ReadGPIO  value=%02x\n", value);


		FT_Close(ftHandle[i]);
		ftHandle[i] = NULL;
	}
#endif
	return true;
}

static BYTE globalWriteBuffer[64];
static int  globalWriteBufferCount = 0;

static void addToWriteBuffer(BYTE data) {
	globalWriteBuffer[globalWriteBufferCount] = data;
	globalWriteBufferCount++;
}

static bool writeWriteBuffer(FT_HANDLE *ftHandle) {

	DWORD bytesWritten;

	if (FT_Write(ftHandle, globalWriteBuffer, globalWriteBufferCount, &bytesWritten) != FT_OK) {
		printf("Error: Buffer write failed\n");
		return false;
	}

	// Check that the number of bytes written makes sense.
	if (bytesWritten != globalWriteBufferCount) {
		printf("Error: Number of bytes written didn't match expected\n");
		printf("   Expected : %0d. Got %0d.\n", globalWriteBufferCount, bytesWritten);
		return false;
	}

	globalWriteBufferCount = 0;

	return true;
};

// ---------------------------------------------------------------------
// Write a byte to the USB Device
// ---------------------------------------------------------------------
static FT_STATUS writeByte(FT_HANDLE handle, BYTE data)
{
	DWORD bytesWritten;
	char data_buffer[8];
	data_buffer[0] = data;
	return FT_Write(handle, data_buffer, 1, &bytesWritten);
}

bool FT4232_D2XX_SPIUnmanaged()
{
	FT_STATUS ftStatus = 0;
	FT_HANDLE ftHandle[4] = { NULL };

	DWORD dwNumDevices = 0;
	ftStatus = FT_CreateDeviceInfoList(&dwNumDevices);
	if (ftStatus != FT_OK) {
		CMD_LOG("FT_CreateDeviceInfoList failed! No devices found!\n");
		return false;
	}
	if (!dwNumDevices) {
		CMD_LOG("FT_CreateDeviceInfoList failed! No devices found! dwNumDevices=%d\n", dwNumDevices);
		return false;
	}

	for (DWORD i = 0; i<dwNumDevices; ++i) {
		FT_DEVICE_LIST_INFO_NODE devInfo;
		memset(&devInfo, 0, sizeof(devInfo));

		ftStatus = FT_GetDeviceInfoDetail(i, &devInfo.Flags, &devInfo.Type,
			&devInfo.ID, &devInfo.LocId,
			devInfo.SerialNumber,
			devInfo.Description,
			&devInfo.ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_GetDeviceInfoDetail failed!\n");
			return false;
		}

		if (DeviceNameIsSet()) {
			if (!DeviceNameCompare(devInfo.Description)) {
				continue;
			}
		}

		CMD_LOG("Dev %d:\n", i);
		CMD_LOG("  ID=0x%08x (%s)\n", devInfo.ID, FT_GetVidPidString(devInfo.ID));
		CMD_LOG("  Description=%s\n", devInfo.Description);

		ftStatus = FT_OpenEx(devInfo.Description, FT_OPEN_BY_DESCRIPTION, &ftHandle[i]);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_OpenEx failed! Cannot open device by Description=%s", devInfo.Description);
			return false;
		}

		ftStatus = FT_Purge(ftHandle[i], (ULONG)FT_PURGE_RX || FT_PURGE_TX);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_Purge failed! ftStatus=0x%08x", ftStatus);
			FT_Close(ftHandle[i]);
			return false;
		}

		ftStatus = FT_SetFlowControl(ftHandle[i], FT_FLOW_RTS_CTS, 0, 0);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_SetFlowControl failed! ftStatus=0x%08x", ftStatus);
			FT_Close(ftHandle[i]);
			return false;
		}

		ftStatus = FT_SetBitMode(ftHandle[i], 0, 0x40);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_SetBitMode failed! ftStatus=0x%08x", ftStatus);
			FT_Close(ftHandle[i]);
			return false;
		}

#define FAILURE -1
#define SUCCESS 0
#define CMD_WRITE 0x80
#define CMD_READ  0x40
		char retVal = SUCCESS;
		int unsigned  rBytes = 0;
		int unsigned  wBytes = 0;
		int  unsigned byteCount = 1024;
		int statusBuffer[1];
		unsigned char dummy;
		char  unsigned last_cmd = 0;
		char  unsigned cmd = CMD_READ;
		int unsigned  wBufferCount[8];
		char  rBufPtr = 0;
		char  wBufPtr = 0;
		DWORD bytesRead;
		char  unsigned rBuffer[8][128];
		char  unsigned wBuffer[8][128];

		while ((rBytes < byteCount) && (retVal == SUCCESS)) {

			// -----------------------------
			// Issue a Command. Ping pong between write and
			// read commands
			CMD_LOG("Awaiting a grant\n");
			statusBuffer[0] = 0;
			while (!statusBuffer[0]) {

				// The RTOS can be busy for an anticipated 500us (this is an
				// estimated from the top of Scot's head). The original design didn't
				// have the sleep delay in and the delay between commands (HSC is connected)
				// could be as low as 250us (2 frames). The RTOS can dissapear for this length
				// of time as the app has 6 threads going. I'm pretty sure there will always be
				// a 2 frame delay between adding commands so the Sleep command was used to
				// add an additiional delay of 250us.
				// We actually saw the occasional fail at a delay of 500us so an additional 
				// 250us was added.
				// The sleep was actually unreliable as whatever the driver does resulted in
				// gaps of 250us, even with the 500us delay. The FT_GetLatencyTimer command
				// was therefore added to ensure a gap. The gap is longer than anticipated ~5ms
				// but results in a regular gap so it has been used.
				//         usleep(500);
				FT_GetLatencyTimer(ftHandle[i], &dummy);
				FT_GetLatencyTimer(ftHandle[i], &dummy);
				FT_GetLatencyTimer(ftHandle[i], &dummy);
				FT_GetLatencyTimer(ftHandle[i], &dummy);
				FT_GetLatencyTimer(ftHandle[i], &dummy);
				FT_GetLatencyTimer(ftHandle[i], &dummy);


				last_cmd = cmd;
				// Last command was a read so do a write.
				if (cmd == CMD_READ && (wBytes < byteCount)) {
					do {
						wBufferCount[wBufPtr] = rand() % 65;
					} while (wBufferCount[wBufPtr] == 0);
					cmd = CMD_WRITE | (0x3f & (wBufferCount[wBufPtr] - 1));
				}
				else {
					cmd = CMD_READ;
				}


				if (writeByte(ftHandle[i], cmd) != FT_OK) {
					CMD_LOG("Error: Failed to write SPI Write command\n");
					FT_Close(ftHandle[i]);
					return false;
				}

				if (FT_GetQueueStatus(ftHandle[i], &bytesRead) != FT_OK) {
					CMD_LOG("Error: Didn't see SPI write command response\n");
					FT_Close(ftHandle[i]);
					return false;
				}
				if (!bytesRead) {
					CMD_LOG("Error: Didn't see SPI write command response\n");
					FT_Close(ftHandle[i]);
					return false;
				}

				if (FT_Read(ftHandle[i], statusBuffer, 1, &bytesRead) != FT_OK) {
					CMD_LOG("Error: Didn't see SPI write command response\n");
					FT_Close(ftHandle[i]);
					return false;
				}

				CMD_LOG("Last Command %0x, Current Command %0x, Response %0x\n",
					last_cmd & 0x000000ff, cmd & 0x000000ff, statusBuffer[0] & 0x000000ff);
				last_cmd = last_cmd & 0xC0;

				CMD_LOG(".");

			}

			// -----------------------------
			// Check that the command matches the response. Don't want
			// things to get out of sync
			//

			if (last_cmd != (statusBuffer[0] & 0xc0)) {

				CMD_LOG("Error: Command and response are out of sync\n");
				retVal = FAILURE;
				// These two extra FF byte writes were used to trigger the scope when problems
				// were seen with delays between commands (e.g.) they were too small. The data
				// content was also changed to incrementing, within 0 to 31, to ensure the FF
				// data is unique.
				writeByte(ftHandle[i], 0xff);
				writeByte(ftHandle[i], 0xff);


				// -----------------------------
				// A Write has been granted
				//
			}
			else if (last_cmd == CMD_WRITE) {

				// Need the same delay as above when starting the write. Without
				// the delay the gap between commands and a burst of write data
				// could be very small - i.e 250us, which is less than an
				// RTOS schedule time..
				FT_GetLatencyTimer(ftHandle[i], &dummy);
				FT_GetLatencyTimer(ftHandle[i], &dummy);
				FT_GetLatencyTimer(ftHandle[i], &dummy);
				FT_GetLatencyTimer(ftHandle[i], &dummy);
				FT_GetLatencyTimer(ftHandle[i], &dummy);
				FT_GetLatencyTimer(ftHandle[i], &dummy);

				CMD_LOG("Write Granted (0x%0x)\n", statusBuffer[0] & 0x000000ff);

				for (i = 0; i<wBufferCount[wBufPtr]; i++) {
					wBuffer[wBufPtr][i] = rand() % 256;
					addToWriteBuffer(wBuffer[wBufPtr][i]);
					CMD_LOG("SENDING: Byte %0d 0x%02x\n", i, wBuffer[wBufPtr][i] & 0x000000ff);
				}

				if (!writeWriteBuffer(&ftHandle[i])) {
					CMD_LOG("Error: Failed to Write SPI Unmanaged Data!\n");
					FT_Close(ftHandle[i]);
					return false;
				}

				// When we do a write the read buffer will automatically be filled,
				// so flush it.
				if (FT_Read(ftHandle[i], statusBuffer, wBufferCount[wBufPtr], &bytesRead) != FT_OK) {
					CMD_LOG("Error: Flush was unsuccessful!\n");
					FT_Close(ftHandle[i]);
					return false;
				}

				if (wBytes % 1024 > (wBufferCount[wBufPtr] + wBytes) % 1024)
					CMD_LOG("Bytes Sent = %dk.\n", (wBytes / 1024) + 1);

				// Move onto the next buffer
				wBytes += wBufferCount[wBufPtr];
				wBufPtr = (wBufPtr + 1) % 8;

			}


			// -----------------------------
			// A Read has been granted
			//

			else {

				// Need the same delay as above when starting the read. Without
				// the delay the gap between commands and a burst of read data
				// could be very small - i.e 250us, which is less than an
				// RTOS schedule time..
				FT_GetLatencyTimer(ftHandle[i], &dummy);
				FT_GetLatencyTimer(ftHandle[i], &dummy);
				FT_GetLatencyTimer(ftHandle[i], &dummy);
				FT_GetLatencyTimer(ftHandle[i], &dummy);
				FT_GetLatencyTimer(ftHandle[i], &dummy);
				FT_GetLatencyTimer(ftHandle[i], &dummy);

				CMD_LOG("Read Granted (0x%0x)\n", statusBuffer[0] & 0x000000ff);

				// To do a read we additionally need to do a write.
				for (i = 0; i<wBufferCount[rBufPtr]; i++)
					addToWriteBuffer(0);
				if (writeWriteBuffer(&ftHandle[i]) != SUCCESS) {
					CMD_LOG("Error: Failed to write empty data at a read!\n");
					FT_Close(ftHandle[i]);
					return false;
				}

				// Get the data sent by the dut
				if (FT_Read(ftHandle[i], rBuffer[rBufPtr], wBufferCount[rBufPtr], &bytesRead) != FT_OK) {
					CMD_LOG("Error: Didn't see expected read data coming back!\n");
					FT_Close(ftHandle[i]);
					return false;
				}

				CMD_LOG("Comparing WRITE and READ DATA\n");
				for (i = 0; i<wBufferCount[rBufPtr]; i++) {
					if (wBuffer[rBufPtr][i] != rBuffer[rBufPtr][i]) {
						CMD_LOG("Error: Byte %0d Expected WR - 0x%2x, Received RD - 0x%2x\n", i,
							wBuffer[rBufPtr][i] & 0x000000ff, rBuffer[rBufPtr][i] & 0x000000ff);
						retVal = FAILURE;
					}
					else {
						CMD_LOG("Success: Byte %0d Expected WR - 0x%2x, Received RD - 0x%2x\n", i,
							wBuffer[rBufPtr][i] & 0x000000ff, rBuffer[rBufPtr][i] & 0x000000ff);
					}

				}
				rBytes += wBufferCount[rBufPtr];
				rBufPtr = (rBufPtr + 1) % 8;

			}
		}

		if (ftHandle[i] != NULL) {
			FT_Close(ftHandle[i]);
			ftHandle[i] = NULL;
		}
	}

	return true;
}

static bool SetSync245(bool bSet)
{
	FT_STATUS ftStatus = 0;
	FT_HANDLE ftHandle[4] = { NULL };


	DWORD dwNumDevices = 0;
	ftStatus = FT_CreateDeviceInfoList(&dwNumDevices);
	if (ftStatus != FT_OK) {
		CMD_LOG("FT_CreateDeviceInfoList failed! No devices found!\n");
		return false;
	}
	if (!dwNumDevices) {
		CMD_LOG("FT_CreateDeviceInfoList failed! No devices found! dwNumDevices=%d\n", dwNumDevices);
		return false;
	}

	for (DWORD i = 0; i < dwNumDevices; ++i) {
		FT_DEVICE_LIST_INFO_NODE devInfo;
		memset(&devInfo, 0, sizeof(devInfo));

		ftStatus = FT_GetDeviceInfoDetail(i, &devInfo.Flags, &devInfo.Type,
			&devInfo.ID, &devInfo.LocId,
			devInfo.SerialNumber,
			devInfo.Description,
			&devInfo.ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_GetDeviceInfoDetail failed!\n");
			return false;
		}

		if (DeviceNameIsSet()) {
			if (!DeviceNameCompare(devInfo.Description)) {
				continue;
			}
		}

		CMD_LOG("Dev %d:\n", i);
		CMD_LOG("  ID=0x%08x (%s)\n", devInfo.ID, FT_GetVidPidString(devInfo.ID));
		CMD_LOG("  Description=%s\n", devInfo.Description);

		ftStatus = FT_OpenEx(devInfo.Description, FT_OPEN_BY_DESCRIPTION, &ftHandle[i]);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_OpenEx failed! Cannot open device by Description=%s", devInfo.Description);
			FT_Close(ftHandle[i]);
			return false;
		}

		if (FT_Purge(ftHandle[i], (ULONG)FT_PURGE_RX || FT_PURGE_TX) != FT_OK) {
			CMD_LOG("Error: Failed to purge  RX and TX Buffers!\n");
			FT_Close(ftHandle[i]);
			return false;
		}

		if (FT_SetFlowControl(ftHandle[i], FT_FLOW_RTS_CTS, 0, 0) != FT_OK) {
			CMD_LOG("Error: Failed to set flow control!\n");
			FT_Close(ftHandle[i]);
			return false;
		}

		if (bSet) {
			if (FT_SetBitMode(ftHandle[i], 0, 0x40) != FT_OK) {
				CMD_LOG("Error: Failed to enable Synchronous 245 FIFO mode!\n");
				FT_Close(ftHandle[i]);
				return false;
			}
			CMD_LOG("Enabled Synchronous 245 FIFO mode!\n");
		}
		else {
			if (FT_SetBitMode(ftHandle[i], 0, 0x00) != FT_OK) {
				CMD_LOG("Error: Failed to disable Synchronous 245 FIFO mode!\n");
				FT_Close(ftHandle[i]);
				return false;
			}
			CMD_LOG("Disabled Synchronous 245 FIFO mode!\n");
		}

		if (ftHandle[i] != NULL) {
			FT_Close(ftHandle[i]);
			ftHandle[i] = NULL;
		}
	}

	return true;
}

bool FT4232_D2XX_SetSync245()
{
	return SetSync245(true);
}

bool FT4232_D2XX_UnsetSync245()
{
	return SetSync245(false);
}

