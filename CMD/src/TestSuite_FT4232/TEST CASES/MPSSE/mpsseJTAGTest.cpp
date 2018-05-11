#include "FTCJTAG.h"
#include "../../TEST SUITE/TestSuite.h"
#include "../../TEST SUITE/TestLogger.h"
#pragma comment(lib, "FTCJTAG.lib")
#include <stdio.h>



#define TISN74BCT8244A_IRDEFAULT 0x81
#define TISN74BCT8244A_BYPASS 0x02



static FT_STATUS readIR(FTC_HANDLE ftHandle, BYTE* byteIRdefault, BYTE* byteBypass)
{
	FTC_STATUS ftStatus = FT_OK;

	DWORD dwNumBitsToWrite = 0;
	DWORD dwNumBytesReturned = 0;
	WriteDataByteBuffer oWriteBuf = { 0 };
	ReadDataByteBuffer oReadBuf = { 0 };


	//
	// Go to Shift-IR, write 0x7F and read then go to IDLE 
	//

	memset(oWriteBuf, 0, sizeof(oWriteBuf));
	memset(oReadBuf, 0, sizeof(oReadBuf));
	dwNumBytesReturned = 0;
	dwNumBitsToWrite = 7;
	oWriteBuf[0] = 0x7F; // 1111111

	ftStatus = JTAG_WriteRead(ftHandle, TRUE, dwNumBitsToWrite, &oWriteBuf, 1, &oReadBuf, &dwNumBytesReturned, RUN_TEST_IDLE_STATE);
	if (ftStatus != FT_OK) {
		CMD_LOG("readIR JTAG_Write failed! 0x7F to ShiftIR then go to RTI! ftStatus=0x%08x\n", ftStatus);
		return ftStatus;
	}
	CMD_LOG("readIR: ShiftIR TDI 11111111 expecting TDO 10000001! dwNumBytesReturned=%d\n", dwNumBytesReturned);
	if (dwNumBytesReturned == 1) {
		CMD_LOG("readIR: buf[0]=0x%02x\n", oReadBuf[0]);
		*byteIRdefault = oReadBuf[0];
	}
	else if (dwNumBytesReturned == 2) {
		CMD_LOG("readIR: buf[0]=0x%02x buf[1]=0x%02x\n", oReadBuf[0], oReadBuf[1]);
		*byteIRdefault = oReadBuf[1];
	}


	//
	// Go to Shift-DR, write 0x01 and read then go to IDLE 
	//

	memset(oWriteBuf, 0, sizeof(oWriteBuf));
	memset(oReadBuf, 0, sizeof(oReadBuf));
	dwNumBytesReturned = 0;
	dwNumBitsToWrite = 2;
	oWriteBuf[0] = 0x1; // 01

	ftStatus = JTAG_WriteRead(ftHandle, FALSE, dwNumBitsToWrite, &oWriteBuf, 1, &oReadBuf, &dwNumBytesReturned, TEST_LOGIC_STATE);
	if (ftStatus != FT_OK) {
		CMD_LOG("readIR JTAG_Write failed! 0x01 to ShiftDR then go to TLS! ftStatus=0x%08x\n", ftStatus);
		return ftStatus;
	}
	CMD_LOG("readIR: ShiftDR TDI 101 expecting TDO 010! dwNumBytesReturned=%d\n", dwNumBytesReturned);
	if (dwNumBytesReturned == 1) {
		CMD_LOG("readIR: buf[0]=0x%02x\n", oReadBuf[0]);
		*byteBypass = oReadBuf[0] >> 5;
	}
	else if (dwNumBytesReturned == 2) {
		CMD_LOG("readIR: buf[0]=0x%02x buf[1]=0x%02x\n", oReadBuf[0], oReadBuf[1]);
		*byteBypass = oReadBuf[1] >> 5;
	}

#if 0
	//
	// Generate 3 clock pulses
	//

	DWORD dwNumClockPulses = 3;
	ftStatus = JTAG_GenerateClockPulses(ftHandle, dwNumClockPulses);
	if (ftStatus != FT_OK) {
		CMD_LOG("readIR JTAG_Write failed! JTAG_GenerateClockPulses %d! ftStatus=0x%08x\n", dwNumClockPulses, ftStatus);
		return ftStatus;
	}
#endif
	return ftStatus;
}

bool FT4232_MPSSE_JTAG_TISN74BCT8244A()
{
	FTC_STATUS ftStatus = FTC_SUCCESS;
	FTC_HANDLE ftHandle = NULL;
	BYTE byteIRdefault = 0;
	BYTE byteBypass = 0;
	DWORD dwNumDevices = 0;
	bool bFound = false;
	bool bHiSpeed = true;


	ftStatus = JTAG_GetNumHiSpeedDevices(&dwNumDevices);
	if (ftStatus != FTC_SUCCESS) {
		CMD_LOG("JTAG_GetNumHiSpeedDevices failed! ftStatus=0x%08x\n", ftStatus);
		return ftStatus;
	}
	CMD_LOG("DeviceCount=%d\n", dwNumDevices);
	if (!dwNumDevices) {
		ftStatus = JTAG_GetNumDevices(&dwNumDevices);
		if (ftStatus != FTC_SUCCESS) {
			CMD_LOG("JTAG_GetNumDevices failed! ftStatus=0x%08x\n", ftStatus);
			return ftStatus;
		}
		if (!dwNumDevices) {
			CMD_LOG("JTAG_GetNumHiSpeedDevices and JTAG_GetNumDevices failed! No devices found!\n");
			return false;
		}
		bHiSpeed = false;
	}
	CMD_LOG("\n");

	for (DWORD i=0; i<dwNumDevices; i++) {

		CHAR Description[32] = { 0 };
		CHAR Channel[32] = { 0 };
		DWORD LocId = 0;
		DWORD Type = 0;

		if (bHiSpeed) {
			ftStatus = JTAG_GetHiSpeedDeviceNameLocIDChannel(i, Description, sizeof(Description), &LocId, Channel, sizeof(Channel), &Type);
			if (ftStatus != FTC_SUCCESS) {
				CMD_LOG("JTAG_GetHiSpeedDeviceNameLocIDChannel failed! ftStatus=0x%08x\n", ftStatus);
				return ftStatus;
			}
		}
		else {
			ftStatus = JTAG_GetDeviceNameLocID(i, Description, sizeof(Description), &LocId);
			if (ftStatus != FTC_SUCCESS) {
				CMD_LOG("JTAG_GetDeviceNameLocID failed! ftStatus=0x%08x\n", ftStatus);
				return ftStatus;
			}
		}

		CMD_LOG("Dev %d:\n", i);
		CMD_LOG("  LocId=0x%08x\n", LocId);
		CMD_LOG("  Type=0x%08x\n", Type);
		CMD_LOG("  Channel=%s\n", Channel);
		CMD_LOG("  Description=%s\n", Description);

		if (DeviceNameIsSet()) {
			if (!DeviceNameCompare(Description)) {
				continue;
			}
			else {
				CMD_LOG("  Specified device is found!\n");
				bFound = true;
			}
		}
		CMD_LOG("\n");

		if (bHiSpeed) {
			ftStatus = JTAG_OpenHiSpeedDevice(Description, LocId, Channel, &ftHandle);
			if (ftStatus != FTC_SUCCESS) {
				CMD_LOG("JTAG_OpenHiSpeedDevice failed! ftStatus=0x%08x\n", ftStatus);
				return ftStatus;
			}
		}
		else {
			ftStatus = JTAG_OpenEx(Description, LocId, &ftHandle);
			if (ftStatus != FTC_SUCCESS) {
				CMD_LOG("JTAG_OpenEx failed! ftStatus=0x%08x\n", ftStatus);
				return ftStatus;
			}
		}

		// TODO: Check if clock divisor is correct
		DWORD dwClockDivisor = 0x05DB; // Value of clock divisor, SCL Frequency = 60/((1+0x05DB)*2) (MHz) = 20khz
		//DWORD dwClockDivisor = 0x012B; // Value of clock divisor, SCL Frequency = 12/((1+0x012B)*2) (MHz) = 20khz
		CMD_LOG("JTAG_InitDevice dwClockDivisor=%08x\n", dwClockDivisor);
		ftStatus = JTAG_InitDevice(ftHandle, dwClockDivisor);
		if (ftStatus != FTC_SUCCESS) {
			CMD_LOG("JTAG_InitDevice failed! ftStatus=0x%08x\n", ftStatus);
			goto exit;
		}

		/*
		// TODO: Is this needed?
		FTC_INPUT_OUTPUT_PINS pinsLow = { FALSE };
		pinsLow.bPin1InputOutputState = TRUE;
		pinsLow.bPin1LowHighState = FALSE;
		pinsLow.bPin2InputOutputState = TRUE;
		pinsLow.bPin2LowHighState = FALSE;
		pinsLow.bPin3InputOutputState = FALSE;
		pinsLow.bPin3LowHighState = FALSE;
		pinsLow.bPin4InputOutputState = TRUE;
		pinsLow.bPin4LowHighState = TRUE;
		FTH_INPUT_OUTPUT_PINS pinsHigh = { FALSE };
		pinsHigh.bPin1InputOutputState = FALSE;
		pinsHigh.bPin1LowHighState = FALSE;
		pinsHigh.bPin2InputOutputState = FALSE;
		pinsHigh.bPin2LowHighState = FALSE;
		pinsHigh.bPin3InputOutputState = FALSE;
		pinsHigh.bPin3LowHighState = FALSE;
		pinsHigh.bPin4InputOutputState = FALSE;
		pinsHigh.bPin4LowHighState = FALSE;
		pinsHigh.bPin5InputOutputState = FALSE;
		pinsHigh.bPin5LowHighState = FALSE;
		pinsHigh.bPin6InputOutputState = FALSE;
		pinsHigh.bPin6LowHighState = FALSE;
		pinsHigh.bPin7InputOutputState = FALSE;
		pinsHigh.bPin7LowHighState = FALSE;
		pinsHigh.bPin8InputOutputState = FALSE;
		pinsHigh.bPin8LowHighState = FALSE;
		ftStatus = JTAG_SetHiSpeedDeviceGPIOs(ftHandle, TRUE, &pinsLow, TRUE, &pinsHigh);
		if (ftStatus != FTC_SUCCESS) {
			CMD_LOG("JTAG_SetHiSpeedDeviceGPIOs failed! ftStatus=0x%08x\n", ftStatus);
			goto exit;
		}
		*/

		CMD_LOG("Reading IR and bypass register...\n");
		ftStatus = readIR(ftHandle, &byteIRdefault, &byteBypass);
		if (ftStatus != FT_OK) {
			CMD_LOG("Reading IR and bypass register failed with error 0x%08x\n", ftStatus);
			goto exit;
		}

		CMD_LOG("\n");
		CMD_LOG("TI SN74BCT8244A\n");
		CMD_LOG("Expected IR Default value is 0x%02x. The scanned value is 0x%02x.\n", TISN74BCT8244A_IRDEFAULT, byteIRdefault);
		CMD_LOG("Expected BYPASS value is 0x%02x. The scanned value is 0x%02x.\n", TISN74BCT8244A_BYPASS, byteBypass);
		CMD_LOG("\n");
		if (byteIRdefault != TISN74BCT8244A_IRDEFAULT ||
			byteBypass != TISN74BCT8244A_BYPASS) {
			CMD_LOG("Error detected! Incorrect scanned values! Please check connection setup.\n");
			CMD_LOG("\n");
			ftStatus = FT_OTHER_ERROR;
			//goto exit;
		}

		JTAG_Close(ftHandle);
		ftHandle = NULL;

		if (bFound) {
			break;
		}
	}
	CMD_LOG("\n");

	if (DeviceNameIsSet()) {
		if (!bFound) {
			CMD_LOG("Specified device is NOT found!\n");
			ftStatus = FT_OTHER_ERROR;
		}
	}

exit:
	if (ftHandle != NULL) {
		JTAG_Close(ftHandle);
		ftHandle = NULL;
	}
	return (ftStatus == FTC_SUCCESS);
}

static FT_STATUS readIR2(FTC_HANDLE ftHandle, BYTE* byteIRdefault, BYTE* byteBypass)
{
	FTC_STATUS ftStatus = FT_OK;

	DWORD dwNumBitsToWrite = 0;
	DWORD dwNumBytesReturned = 0;
	WriteDataByteBuffer oWriteBuf = { 0 };
	ReadDataByteBuffer oReadBuf = { 0 };


	//
	// Go to Shift-IR, write 0x7F and read then go to IDLE 
	//

	memset(oWriteBuf, 0, sizeof(oWriteBuf));
	memset(oReadBuf, 0, sizeof(oReadBuf));
	dwNumBytesReturned = 0;
	dwNumBitsToWrite = 8;
	oWriteBuf[0] = 0xFF; // 11111111

	ftStatus = JTAG_WriteRead(ftHandle, TRUE, dwNumBitsToWrite, &oWriteBuf, 1, &oReadBuf, &dwNumBytesReturned, RUN_TEST_IDLE_STATE);
	if (ftStatus != FT_OK) {
		CMD_LOG("readIR JTAG_Write failed! 0xFF to ShiftIR then go to RTI! ftStatus=0x%08x\n", ftStatus);
		return ftStatus;
	}
	CMD_LOG("  readIR: ShiftIR TDI 11111111 expecting TDO 10000001! dwNumBytesReturned=%d\n", dwNumBytesReturned);
	if (dwNumBytesReturned == 1) {
		CMD_LOG("  readIR: buf[0]=0x%02x\n", oReadBuf[0]);
		*byteIRdefault = oReadBuf[0];
	}
	else if (dwNumBytesReturned == 2) {
		CMD_LOG("  readIR: buf[0]=0x%02x buf[1]=0x%02x\n", oReadBuf[0], oReadBuf[1]);
		*byteIRdefault = oReadBuf[1];
	}


	//
	// Go to Shift-DR, write 0x01 and read then go to IDLE 
	//

	memset(oWriteBuf, 0, sizeof(oWriteBuf));
	memset(oReadBuf, 0, sizeof(oReadBuf));
	dwNumBytesReturned = 0;
	dwNumBitsToWrite = 3;
	oWriteBuf[0] = 0x5; // 101

	ftStatus = JTAG_WriteRead(ftHandle, FALSE, dwNumBitsToWrite, &oWriteBuf, 1, &oReadBuf, &dwNumBytesReturned, TEST_LOGIC_STATE);
	if (ftStatus != FT_OK) {
		CMD_LOG("readIR JTAG_Write failed! 0x5 to ShiftDR then go to TLS! ftStatus=0x%08x\n", ftStatus);
		return ftStatus;
	}
	CMD_LOG("  readIR: ShiftDR TDI 101 expecting TDO 010! dwNumBytesReturned=%d\n", dwNumBytesReturned);
	if (dwNumBytesReturned == 1) {
		CMD_LOG("  readIR: buf[0]=0x%02x\n", oReadBuf[0]);
		*byteBypass = oReadBuf[0];// >> 5;
	}
	else if (dwNumBytesReturned == 2) {
		CMD_LOG("  readIR: buf[0]=0x%02x buf[1]=0x%02x\n", oReadBuf[0], oReadBuf[1]);
		*byteBypass = oReadBuf[1];// >> 5;
	}

#if 0
	//
	// Generate 3 clock pulses
	//

	DWORD dwNumClockPulses = 3;
	ftStatus = JTAG_GenerateClockPulses(ftHandle, dwNumClockPulses);
	if (ftStatus != FT_OK) {
		CMD_LOG("readIR JTAG_Write failed! JTAG_GenerateClockPulses %d! ftStatus=0x%08x\n", dwNumClockPulses, ftStatus);
		return ftStatus;
	}
#endif
	return ftStatus;
}

bool FT4232_MPSSE_JTAG_TISN74BCT8244A_2()
{
	FTC_STATUS ftStatus = FTC_SUCCESS;
	FTC_HANDLE ftHandle = NULL;
	BYTE byteIRdefault = 0;
	BYTE byteBypass = 0;
	DWORD dwNumDevices = 0;
	bool bFound = false;
	bool bHiSpeed = true;


	ftStatus = JTAG_GetNumHiSpeedDevices(&dwNumDevices);
	if (ftStatus != FTC_SUCCESS) {
		CMD_LOG("JTAG_GetNumHiSpeedDevices failed! ftStatus=0x%08x\n", ftStatus);
		return ftStatus;
	}
	CMD_LOG("DeviceCount=%d\n", dwNumDevices);
	if (!dwNumDevices) {
		ftStatus = JTAG_GetNumDevices(&dwNumDevices);
		if (ftStatus != FTC_SUCCESS) {
			CMD_LOG("JTAG_GetNumDevices failed! ftStatus=0x%08x\n", ftStatus);
			return ftStatus;
		}
		if (!dwNumDevices) {
			CMD_LOG("JTAG_GetNumHiSpeedDevices and JTAG_GetNumDevices failed! No devices found!\n");
			return false;
		}
		bHiSpeed = false;
	}
	CMD_LOG("\n");

	for (DWORD i = 0; i<dwNumDevices; i++) {

		CHAR Description[32] = { 0 };
		CHAR Channel[32] = { 0 };
		DWORD LocId = 0;
		DWORD Type = 0;

		if (bHiSpeed) {
			ftStatus = JTAG_GetHiSpeedDeviceNameLocIDChannel(i, Description, sizeof(Description), &LocId, Channel, sizeof(Channel), &Type);
			if (ftStatus != FTC_SUCCESS) {
				CMD_LOG("JTAG_GetHiSpeedDeviceNameLocIDChannel failed! ftStatus=0x%08x\n", ftStatus);
				return ftStatus;
			}
		}
		else {
			ftStatus = JTAG_GetDeviceNameLocID(i, Description, sizeof(Description), &LocId);
			if (ftStatus != FTC_SUCCESS) {
				CMD_LOG("JTAG_GetDeviceNameLocID failed! ftStatus=0x%08x\n", ftStatus);
				return ftStatus;
			}
		}
		CMD_LOG("\n");
		CMD_LOG("Dev %d:\n", i);
		CMD_LOG("  LocId=0x%08x\n", LocId);
		CMD_LOG("  Type=0x%08x\n", Type);
		CMD_LOG("  Channel=%s\n", Channel);
		CMD_LOG("  Description=%s\n", Description);

		if (DeviceNameIsSet()) {
			if (!DeviceNameCompare(Description)) {
				continue;
			}
			else {
				CMD_LOG("  Specified device is found!\n");
				bFound = true;
			}
		}
		CMD_LOG("\n");

		if (bHiSpeed) {
			ftStatus = JTAG_OpenHiSpeedDevice(Description, LocId, Channel, &ftHandle);
			if (ftStatus != FTC_SUCCESS) {
				CMD_LOG("JTAG_OpenHiSpeedDevice failed! ftStatus=0x%08x\n", ftStatus);
				return ftStatus;
			}
		}
		else {
			ftStatus = JTAG_OpenEx(Description, LocId, &ftHandle);
			if (ftStatus != FTC_SUCCESS) {
				CMD_LOG("JTAG_OpenEx failed! ftStatus=0x%08x\n", ftStatus);
				return ftStatus;
			}
		}

		// TODO: Check if clock divisor is correct
		DWORD dwClockDivisor = 0x05DB; // Value of clock divisor, SCL Frequency = 60/((1+0x05DB)*2) (MHz) = 20khz
		// DWORD dwClockDivisor = 0x012B; // Value of clock divisor, SCL Frequency = 12/((1+0x012B)*2) (MHz) = 20khz
		CMD_LOG("JTAG_InitDevice dwClockDivisor=%08x\n", dwClockDivisor);
		ftStatus = JTAG_InitDevice(ftHandle, dwClockDivisor);
		if (ftStatus != FTC_SUCCESS) {
			CMD_LOG("JTAG_InitDevice failed! ftStatus=0x%08x\n", ftStatus);
			goto exit;
		}

		CMD_LOG("Reading IR and bypass register...\n");
		ftStatus = readIR2(ftHandle, &byteIRdefault, &byteBypass);
		if (ftStatus != FT_OK) {
			CMD_LOG("Reading IR and bypass register failed with error 0x%08x\n", ftStatus);
			goto exit;
		}

		CMD_LOG("\n");
		CMD_LOG("TI SN74BCT8244A\n");
		CMD_LOG("Expected IR Default value is 0x%02x. The scanned value is 0x%02x.\n", TISN74BCT8244A_IRDEFAULT, byteIRdefault);
		CMD_LOG("Expected BYPASS value is 0x%02x. The scanned value is 0x%02x.\n", TISN74BCT8244A_BYPASS, byteBypass);
		CMD_LOG("\n");
		if (byteIRdefault != TISN74BCT8244A_IRDEFAULT ||
			byteBypass != TISN74BCT8244A_BYPASS) {
			CMD_LOG("Error detected! Incorrect scanned values! Please check connection setup.\n");
			CMD_LOG("\n");
			ftStatus = FT_OTHER_ERROR;
			//goto exit;
		}

		JTAG_Close(ftHandle);
		ftHandle = NULL;

		if (bFound) {
			break;
		}
	}
	CMD_LOG("\n");

	if (DeviceNameIsSet()) {
		if (!bFound) {
			CMD_LOG("Specified device is NOT found!\n");
			ftStatus = FT_OTHER_ERROR;
		}
	}

exit:
	if (ftHandle != NULL) {
		JTAG_Close(ftHandle);
		ftHandle = NULL;
	}
	return (ftStatus == FTC_SUCCESS);
}

bool FT4232_MPSSE_JTAG_TISN74BCT8244A_3()
{
	FTC_STATUS ftStatus = FTC_SUCCESS;
	FTC_HANDLE ftHandle = NULL;
	BYTE byteIRdefault = 0;
	BYTE byteBypass = 0;
	DWORD dwNumDevices = 0;
	bool bFound = false;
	bool bHiSpeed = true;


	ftStatus = JTAG_GetNumHiSpeedDevices(&dwNumDevices);
	if (ftStatus != FTC_SUCCESS) {
		CMD_LOG("JTAG_GetNumHiSpeedDevices failed! ftStatus=0x%08x\n", ftStatus);
		return ftStatus;
	}
	CMD_LOG("DeviceCount=%d\n", dwNumDevices);
	if (!dwNumDevices) {
		ftStatus = JTAG_GetNumDevices(&dwNumDevices);
		if (ftStatus != FTC_SUCCESS) {
			CMD_LOG("JTAG_GetNumDevices failed! ftStatus=0x%08x\n", ftStatus);
			return ftStatus;
		}
		if (!dwNumDevices) {
			CMD_LOG("JTAG_GetNumHiSpeedDevices and JTAG_GetNumDevices failed! No devices found!\n");
			return false;
		}
		bHiSpeed = false;
	}
	CMD_LOG("\n");

	for (DWORD i = 0; i<dwNumDevices; i++) {

		CHAR Description[32] = { 0 };
		CHAR Channel[32] = { 0 };
		DWORD LocId = 0;
		DWORD Type = 0;

		if (bHiSpeed) {
			ftStatus = JTAG_GetHiSpeedDeviceNameLocIDChannel(i, Description, sizeof(Description), &LocId, Channel, sizeof(Channel), &Type);
			if (ftStatus != FTC_SUCCESS) {
				CMD_LOG("JTAG_GetHiSpeedDeviceNameLocIDChannel failed! ftStatus=0x%08x\n", ftStatus);
				return ftStatus;
			}
		}
		else {
			ftStatus = JTAG_GetDeviceNameLocID(i, Description, sizeof(Description), &LocId);
			if (ftStatus != FTC_SUCCESS) {
				CMD_LOG("JTAG_GetDeviceNameLocID failed! ftStatus=0x%08x\n", ftStatus);
				return ftStatus;
			}
		}
		CMD_LOG("\n");
		CMD_LOG("Dev %d:\n", i);
		CMD_LOG("  LocId=0x%08x\n", LocId);
		CMD_LOG("  Type=0x%08x\n", Type);
		CMD_LOG("  Channel=%s\n", Channel);
		CMD_LOG("  Description=%s\n", Description);

		if (DeviceNameIsSet()) {
			if (!DeviceNameCompare(Description)) {
				continue;
			}
			else {
				CMD_LOG("  Specified device is found!\n");
				bFound = true;
			}
		}
		CMD_LOG("\n");

		if (bHiSpeed) {
			ftStatus = JTAG_OpenHiSpeedDevice(Description, LocId, Channel, &ftHandle);
			if (ftStatus != FTC_SUCCESS) {
				CMD_LOG("JTAG_OpenHiSpeedDevice failed! ftStatus=0x%08x\n", ftStatus);
				return ftStatus;
			}
		}
		else {
			ftStatus = JTAG_OpenEx(Description, LocId, &ftHandle);
			if (ftStatus != FTC_SUCCESS) {
				CMD_LOG("JTAG_OpenEx failed! ftStatus=0x%08x\n", ftStatus);
				return ftStatus;
			}
		}

		// TODO: Check if clock divisor is correct
		//DWORD dwClockDivisor = 0x05DB; // Value of clock divisor, SCL Frequency = 60/((1+0x05DB)*2) (MHz) = 20khz
		DWORD dwClockDivisor = 0x012B; // Value of clock divisor, SCL Frequency = 12/((1+0x012B)*2) (MHz) = 20khz
		CMD_LOG("JTAG_InitDevice dwClockDivisor=%08x\n", dwClockDivisor);
		ftStatus = JTAG_InitDevice(ftHandle, dwClockDivisor);
		if (ftStatus != FTC_SUCCESS) {
			CMD_LOG("JTAG_InitDevice failed! ftStatus=0x%08x\n", ftStatus);
			goto exit;
		}

		CMD_LOG("Reading IR and bypass register...\n");
		ftStatus = readIR2(ftHandle, &byteIRdefault, &byteBypass);
		if (ftStatus != FT_OK) {
			CMD_LOG("Reading IR and bypass register failed with error 0x%08x\n", ftStatus);
			goto exit;
		}

		CMD_LOG("\n");
		CMD_LOG("TI SN74BCT8244A\n");
		CMD_LOG("Expected IR Default value is 0x%02x. The scanned value is 0x%02x.\n", TISN74BCT8244A_IRDEFAULT, byteIRdefault);
		CMD_LOG("Expected BYPASS value is 0x%02x. The scanned value is 0x%02x.\n", TISN74BCT8244A_BYPASS, byteBypass);
		CMD_LOG("\n");
		if (byteIRdefault != TISN74BCT8244A_IRDEFAULT ||
			byteBypass != TISN74BCT8244A_BYPASS) {
			CMD_LOG("Error detected! Incorrect scanned values! Please check connection setup.\n");
			CMD_LOG("\n");
			ftStatus = FT_OTHER_ERROR;
			//goto exit;
		}

		JTAG_Close(ftHandle);
		ftHandle = NULL;

		if (bFound) {
			break;
		}
	}
	CMD_LOG("\n");

	if (DeviceNameIsSet()) {
		if (!bFound) {
			CMD_LOG("Specified device is NOT found!\n");
			ftStatus = FT_OTHER_ERROR;
		}
	}

exit:
	if (ftHandle != NULL) {
		JTAG_Close(ftHandle);
		ftHandle = NULL;
	}
	return (ftStatus == FTC_SUCCESS);
}


