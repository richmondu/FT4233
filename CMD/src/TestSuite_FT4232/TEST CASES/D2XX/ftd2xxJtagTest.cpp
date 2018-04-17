#include "ftd2xx.h"
#include "ftdiIds.h"
#include "../../TEST SUITE/TestSuite.h"
#include "../../TEST SUITE/TestLogger.h"
#pragma comment(lib, "ftd2xx.lib")
#include <stdio.h>



#define TISN74BCT8244A_IRDEFAULT 0x81
#define TISN74BCT8244A_BYPASS 0x02



static FT_STATUS configurePortAsMPSSE(FT_HANDLE ftHandle)
{
	FT_STATUS ftStatus = FT_OK;
	BYTE byInputBuffer[1024];
	DWORD dwNumBytesToRead = 0;
	DWORD dwNumBytesRead = 0;


	// -----------------------------------------------------------
	// At this point, the MPSSE is ready for commands
	// -----------------------------------------------------------

	// Reset USB device
	ftStatus = FT_ResetDevice(ftHandle);
	if (ftStatus != FT_OK) {
		CMD_LOG("configurePortAsMPSSE FT_ResetDevice failed! Error in initializing the MPSSE 0x%08x\n", ftStatus);
		return ftStatus;
	}

	// Purge USB receive buffer first by reading out all old data from receive buffer
	// Get the number of bytes in the receive buffer
	ftStatus = FT_GetQueueStatus(ftHandle, &dwNumBytesToRead);
	if (ftStatus != FT_OK) {
		CMD_LOG("configurePortAsMPSSE FT_GetQueueStatus failed! Error in initializing the MPSSE 0x%08x\n", ftStatus);
		return ftStatus;
	}
	// Read out the data from receive buffer
	if (dwNumBytesToRead > 0) {
		FT_Read(ftHandle, &byInputBuffer, dwNumBytesToRead, &dwNumBytesRead);
	}

	// Set USB request transfer sizes to 64K
	ftStatus = FT_SetUSBParameters(ftHandle, 65536, 65535);
	if (ftStatus != FT_OK) {
		CMD_LOG("configurePortAsMPSSE FT_SetUSBParameters failed! Error in initializing the MPSSE 0x%08x\n", ftStatus);
		return ftStatus;
	}

	// Disable event and error characters
	ftStatus = FT_SetChars(ftHandle, false, 0, false, 0);
	if (ftStatus != FT_OK) {
		CMD_LOG("configurePortAsMPSSE FT_SetChars failed! Error in initializing the MPSSE 0x%08x\n", ftStatus);
		return ftStatus;
	}

	// Set the read and write timeouts in milliseconds
	ftStatus = FT_SetTimeouts(ftHandle, 0, 5000);
	if (ftStatus != FT_OK) {
		CMD_LOG("configurePortAsMPSSE FT_SetChars failed! Error in initializing the MPSSE 0x%08x\n", ftStatus);
		return ftStatus;
	}

	// Set the latency timer (default is 16mS)
	ftStatus = FT_SetLatencyTimer(ftHandle, 16);
	if (ftStatus != FT_OK) {
		CMD_LOG("configurePortAsMPSSE FT_SetLatencyTimer failed! Error in initializing the MPSSE 0x%08x\n", ftStatus);
		return ftStatus;
	}

	// Reset controller
	ftStatus = FT_SetBitMode(ftHandle, 0x0, 0x00);
	if (ftStatus != FT_OK) {
		CMD_LOG("configurePortAsMPSSE FT_SetBitMode failed! Error in initializing the MPSSE 0x%08x\n", ftStatus);
		return ftStatus;
	}

	// Enable MPSSE mode
	ftStatus = FT_SetBitMode(ftHandle, 0x0, 0x02);
	if (ftStatus != FT_OK) {
		CMD_LOG("configurePortAsMPSSE FT_SetBitMode2 failed! Error in initializing the MPSSE 0x%08x\n", ftStatus);
		return ftStatus;
	}

	Sleep(100); // Wait for all the USB stuff to complete and work

	return ftStatus;
}

// -----------------------------------------------------------
// Synchronize the MPSSE by sending a bogus opcode (0xAA),
// The MPSSE will respond with "Bad Command" (0xFA) followed by
// the bogus opcode itself.
// -----------------------------------------------------------
static FT_STATUS sendBadCommand(FT_HANDLE ftHandle)
{
	FT_STATUS ftStatus = FT_OK;
	BYTE byOutputBuffer[1024] = { 0 }; // Buffer to hold MPSSE commands and data to be sent
	BYTE byInputBuffer[1024] = { 0 }; // Buffer to hold data read
	DWORD dwNumBytesToSend = 0;
	DWORD dwNumBytesSent = 0;
	DWORD dwNumBytesToRead = 0;
	DWORD dwNumBytesRead = 0;


	// Add bogus command ‘xAA’ to the queue
	byOutputBuffer[dwNumBytesToSend++] = 0xAA;//'\xAA';

	// Send off the BAD commands
	ftStatus = FT_Write(ftHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
	if (ftStatus != FT_OK) {
		CMD_LOG("sendBadCommand FT_Write failed! ftStatus=0x%08x\n", ftStatus);
		return ftStatus;
	}

	dwNumBytesToSend = 0; // Reset output buffer pointer

	do
	{
		ftStatus = FT_GetQueueStatus(ftHandle, &dwNumBytesToRead);
		// Get the number of bytes in the device input buffer
	} while ((dwNumBytesToRead == 0) && (ftStatus == FT_OK)); //or Timeout


	// Read out the data from input buffer
	ftStatus = FT_Read(ftHandle, &byInputBuffer, dwNumBytesToRead, &dwNumBytesRead);

	//Check if Bad command and echo command received
	bool bCommandEchod = false;
	for (DWORD dwCount = 0; dwCount < dwNumBytesRead - 1; dwCount++) {
		if ((byInputBuffer[dwCount] == 0xFA) && (byInputBuffer[dwCount + 1] == 0xAA)) {
			bCommandEchod = true;
			break;
		}
	}
	if (bCommandEchod == false) {
		CMD_LOG("sendBadCommand FT_Read failed! Bad command and echo command NOT received...\n");
		ftStatus = FT_OTHER_ERROR;
	}

	return ftStatus;
}

static FT_STATUS configureMPSSEasJTAG(FT_HANDLE ftHandle)
{
	FT_STATUS ftStatus = FT_OK;
	BYTE byOutputBuffer[1024] = { 0 }; // Buffer to hold MPSSE commands and data to be sent
	DWORD dwNumBytesToSend = 0;
	DWORD dwNumBytesSent = 0;


	//
	// Set up the Hi-Speed specific commands
	//
	// Use 60MHz master clock (disable divide by 5)
	byOutputBuffer[dwNumBytesToSend++] = 0x8A;
	// Turn off adaptive clocking (may be needed for ARM)
	byOutputBuffer[dwNumBytesToSend++] = 0x97;
	// Disable three-phase clocking
	byOutputBuffer[dwNumBytesToSend++] = 0x8D;
	ftStatus = FT_Write(ftHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
	if (ftStatus != FT_OK) {
		CMD_LOG("configureMPSSEasJTAG FT_Write failed! Set up the Hi-Speed specifics! ftStatus=0x%08x\n", ftStatus);
		return ftStatus;
	}

	dwNumBytesToSend = 0;
	dwNumBytesSent = 0;


	//
	// Set initial states of the MPSSE interface - low byte, both pin directions and output values
	//
	// Pin name Signal Direction Config Initial State Config
	// ADBUS0 TCK output 1 low 0
	// ADBUS1 TDI output 1 low 0
	// ADBUS2 TDO input 0 0
	// ADBUS3 TMS output 1 high 1
	// ADBUS4 GPIOL0 input 0 0
	// ADBUS5 GPIOL1 input 0 0
	// ADBUS6 GPIOL2 input 0 0
	// ADBUS7 GPIOL3 input 0 0
	// Set data bits low-byte of MPSSE port
	byOutputBuffer[dwNumBytesToSend++] = 0x80;
	// Initial state config above
	byOutputBuffer[dwNumBytesToSend++] = 0x08;
	// Direction config above
	byOutputBuffer[dwNumBytesToSend++] = 0x0B;
	// Send off the low GPIO config commands
	ftStatus = FT_Write(ftHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
	if (ftStatus != FT_OK) {
		CMD_LOG("configureMPSSEasJTAG FT_Write failed! Send off the low GPIO config commands! ftStatus=0x%08x\n", ftStatus);
		return ftStatus;
	}

	dwNumBytesToSend = 0;
	dwNumBytesSent = 0;


	//
	// Set initial states of the MPSSE interface - high byte, both pin directions and output values
	//
	// Pin name Signal Direction Config Initial State Config
	// ACBUS0 GPIOH0 input 0 0
	// ACBUS1 GPIOH1 input 0 0
	// ACBUS2 GPIOH2 input 0 0
	// ACBUS3 GPIOH3 input 0 0
	// ACBUS4 GPIOH4 input 0 0
	// ACBUS5 GPIOH5 input 0 0
	// ACBUS6 GPIOH6 input 0 0
	// ACBUS7 GPIOH7 input 0 0

	// Set data bits low-byte of MPSSE port
	byOutputBuffer[dwNumBytesToSend++] = 0x82;
	// Initial state config above
	byOutputBuffer[dwNumBytesToSend++] = 0x00;
	// Direction config above
	byOutputBuffer[dwNumBytesToSend++] = 0x00;

	// Send off the high GPIO config commands
	ftStatus = FT_Write(ftHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
	if (ftStatus != FT_OK) {
		CMD_LOG("configureMPSSEasJTAG FT_Write failed! Send off the high GPIO config commands! ftStatus=0x%08x\n", ftStatus);
		return ftStatus;
	}

	dwNumBytesToSend = 0;
	dwNumBytesSent = 0;


	//
	// Set TCK frequency TCK = 60MHz /((1 + [(1 +0xValueH*256) OR 0xValueL])*2)
	//
	DWORD dwClockDivisor = 0x05DB; // Value of clock divisor, SCL Frequency = 60/((1+0x05DB)*2) (MHz) = 20khz
	//Command to set clock divisor
	byOutputBuffer[dwNumBytesToSend++] = '\x86';
	//Set 0xValueL of clock divisor
	byOutputBuffer[dwNumBytesToSend++] = dwClockDivisor & 0xFF;
	//Set 0xValueH of clock divisor
	byOutputBuffer[dwNumBytesToSend++] = (dwClockDivisor >> 8) & 0xFF;
	// Send off the clock divisor commands
	ftStatus = FT_Write(ftHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
	if (ftStatus != FT_OK) {
		CMD_LOG("configureMPSSEasJTAG FT_Write failed! Send off the clock divisor commands! ftStatus=0x%08x\n", ftStatus);
		return ftStatus;
	}

	dwNumBytesToSend = 0;
	dwNumBytesSent = 0;


	//
	// Disable internal loop-back
	//
	// Disable loopback
	byOutputBuffer[dwNumBytesToSend++] = 0x85;
	// Send off the loopback command
	ftStatus = FT_Write(ftHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
	if (ftStatus != FT_OK) {
		CMD_LOG("configureMPSSEasJTAG FT_Write failed!  Send off the disable loopback commands! ftStatus=0x%08x\n", ftStatus);
		return ftStatus;
	}

	return ftStatus;
}

static FT_STATUS readIR(FT_HANDLE ftHandle, BYTE* byteIRdefault, BYTE* byteBypass)
{
	FT_STATUS ftStatus = FT_OK;
	BYTE byOutputBuffer[1024] = { 0 };
	BYTE byInputBuffer[1024] = { 0 };
	DWORD dwNumBytesToSend = 0;
	DWORD dwNumBytesSent = 0;
	DWORD dwNumBytesToRead = 0;
	DWORD dwNumBytesRead = 0;


	//
	// Navigage TMS through Test-Logic-Reset -> Run-Test-Idle -> Select-DR-Scan -> Select-IR-Scan
	// TMS=1 TMS=0 TMS=1 TMS=1
	//
	// Don't read data in Test-Logic-Reset, Run-Test-Idle, Select-DR-Scan, Select-IR-Scan
	byOutputBuffer[dwNumBytesToSend++] = 0x4B;
	// Number of clock pulses = Length + 1 (6 clocks here)
	byOutputBuffer[dwNumBytesToSend++] = 0x05;
	// Data is shifted LSB first, so the TMS pattern is 101100
	byOutputBuffer[dwNumBytesToSend++] = 0x0D;
	// Send off the TMS command
	ftStatus = FT_Write(ftHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
	if (ftStatus != FT_OK) {
		CMD_LOG("readIR FT_Write failed! Navigage TMS through TLR -> RTI -> SDR -> SIR! ftStatus=0x%08x\n", ftStatus);
		return ftStatus;
	}

	dwNumBytesToSend = 0;
	dwNumBytesSent = 0;


	//
	// TMS is currently low.
	// State machine is in Shift-IR, so now use the TDI/TDO command to shift 1's out TDI/DO while reading TDO/DI
	// Although 8 bits need shifted in, only 7 are clocked here. The 8th will be in conjunciton with a TMS command, coming next
	//
	// Clock data out throuth states Capture-IR, Shift-IR and Exit-IR, read back result
	byOutputBuffer[dwNumBytesToSend++] = 0x3B;
	// Number of clock pulses = Length + 1 (7 clocks here)
	byOutputBuffer[dwNumBytesToSend++] = 0x06;
	// Shift out 1111111 (ignore last bit)
	byOutputBuffer[dwNumBytesToSend++] = 0xFF;
	// Send off the TMS command
	ftStatus = FT_Write(ftHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
	if (ftStatus != FT_OK) {
		CMD_LOG("readIR FT_Write failed! shift 1's out TDI/DO while reading TDO/DI! ftStatus=0x%08x\n", ftStatus);
		return ftStatus;
	}

	dwNumBytesToSend = 0;
	dwNumBytesSent = 0;

	// Here is the TMS command for one clock. Data is also shifted in.
	// Clock out TMS, Read one bit.
	byOutputBuffer[dwNumBytesToSend++] = 0x6B;
	// Number of clock pulses = Length + 0 (1 clock here)
	byOutputBuffer[dwNumBytesToSend++] = 0x00;
	// Data is shifted LSB first, so TMS becomes 1. Also, bit 7 is shifted into TDI/DO, also a 1
	// The 1 in bit 1 will leave TMS high for the next commands.
	byOutputBuffer[dwNumBytesToSend++] = 0x83;
	// Send off the TMS command
	ftStatus = FT_Write(ftHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
	if (ftStatus != FT_OK) {
		CMD_LOG("readIR FT_Write failed! Clock out TMS, Read one bit! ftStatus=0x%08x\n", ftStatus);
		return ftStatus;
	}

	dwNumBytesToSend = 0;
	dwNumBytesSent = 0;


	//
	// Navigage TMS from Exit-IR through Update-IR -> Select-DR-Scan -> Capture-DR
	// TMS=1 TMS=1 TMS=0
	//
	// Don't read data in Update-IR -> Select-DR-Scan -> Capture-DR
	byOutputBuffer[dwNumBytesToSend++] = 0x4B;
	// Number of clock pulses = Length + 1 (4 clocks here)
	byOutputBuffer[dwNumBytesToSend++] = 0x03;
	// Data is shifted LSB first, so the TMS pattern is 110
	byOutputBuffer[dwNumBytesToSend++] = 0x83;
	// Send off the TMS command
	ftStatus = FT_Write(ftHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
	if (ftStatus != FT_OK) {
		CMD_LOG("readIR FT_Write failed! Navigage TMS from EIR through UIR -> SDR -> CDR! ftStatus=0x%08x\n", ftStatus);
		return ftStatus;
	}

	dwNumBytesToSend = 0;
	dwNumBytesSent = 0;


	//
	// TMS is currently low.
	// State machine is in Shift-DR, so now use the TDI/TDO command to shift 101 out TDI/DO while reading TDO/DI
	// Although 3 bits need shifted in, only 2 are clocked here. The 3rd will be in conjunciton with a TMS command, coming next
	//
	// Clock data out throuth states Shift-DR and Exit-DR.
	byOutputBuffer[dwNumBytesToSend++] = 0x3B;
	// Number of clock pulses = Length + 1 (2 clocks here)
	byOutputBuffer[dwNumBytesToSend++] = 0x01;
	// Shift out 101 (ignore last bit)
	byOutputBuffer[dwNumBytesToSend++] = 0x01;
	// Send off the TMS command
	ftStatus = FT_Write(ftHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
	if (ftStatus != FT_OK) {
		CMD_LOG("readIR FT_Write failed! Use the TDI/TDO command to shift 101 out TDI/DO while reading TDO/DI! ftStatus=0x%08x\n", ftStatus);
		return ftStatus;
	}

	dwNumBytesToSend = 0;
	dwNumBytesSent = 0;

	// Here is the TMS command for one clock. Data is also shifted in.
	// Clock out TMS, Read one bit.
	byOutputBuffer[dwNumBytesToSend++] = 0x6B;
	// Number of clock pulses = Length + 0 (1 clock here)
	byOutputBuffer[dwNumBytesToSend++] = 0x00;
	// Data is shifted LSB first, so TMS becomes 1. Also, bit 7 is shifted into TDI/DO, also a 1
	// The 1 in bit 1 will leave TMS high for the next commands.
	byOutputBuffer[dwNumBytesToSend++] = 0x83;
	// Send off the TMS command
	ftStatus = FT_Write(ftHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
	if (ftStatus != FT_OK) {
		CMD_LOG("readIR FT_Write failed! Clock out TMS, Read one bit! ftStatus=0x%08x\n", ftStatus);
		return ftStatus;
	}

	dwNumBytesToSend = 0;
	dwNumBytesSent = 0;


	//
	// Navigage TMS through Update-DR -> Select-DR-Scan -> Select-IR-Scan -> Test Logic Reset
	// TMS=1 TMS=1 TMS=1 TMS=1
	//
	// Don't read data in Update-DR -> Select-DR-Scan -> Select-IR-Scan -> Test Logic Reset
	byOutputBuffer[dwNumBytesToSend++] = 0x4B;
	// Number of clock pulses = Length + 1 (4 clocks here)
	byOutputBuffer[dwNumBytesToSend++] = 0x03;
	// Data is shifted LSB first, so the TMS pattern is 101100
	byOutputBuffer[dwNumBytesToSend++] = 0xFF;
	// Send off the TMS command
	ftStatus = FT_Write(ftHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
	if (ftStatus != FT_OK) {
		CMD_LOG("readIR FT_Write failed! Return to TLR from UDR! ftStatus=0x%08x\n", ftStatus);
		return ftStatus;
	}

	dwNumBytesToSend = 0;
	dwNumBytesSent = 0;


	//
	// Read ID and bypass register
	//
	// Get the number of bytes in the device input buffer or Timeout
	do
	{
		ftStatus = FT_GetQueueStatus(ftHandle, &dwNumBytesToRead);
	} while ((dwNumBytesToRead == 0) && (ftStatus == FT_OK));

	// Read out the data from input buffer
	ftStatus = FT_Read(ftHandle, &byInputBuffer, dwNumBytesToRead, &dwNumBytesRead);
	if (ftStatus != FT_OK) {
		CMD_LOG("readIR FT_Read failed! Read out the data from input buffer! ftStatus=0x%08x\n", ftStatus);
		return ftStatus;
	}

	CMD_LOG("  FT_Read BytesRead=%d\n", dwNumBytesRead);
	for (DWORD i = 0; i < dwNumBytesRead; i++) {
		CMD_LOG("  FT_Read Buffer[%d]=0x%02x\n", i, byInputBuffer[i]);
	}
	*byteIRdefault = byInputBuffer[dwNumBytesRead - 3];
	*byteBypass = (byInputBuffer[dwNumBytesRead - 1] >> 5);


	// Generate a clock while in Test-Logic-Reset
	// This will not do anything with the TAP in the Test-Logic-Reset state,
	// but will demonstrate generation of clocks without any data transfer
	// Generate clock pulses
	byOutputBuffer[dwNumBytesToSend++] = 0x8F;
	// (0x0002 + 1) * 8 = 24 clocks
	byOutputBuffer[dwNumBytesToSend++] = 0x02;
	byOutputBuffer[dwNumBytesToSend++] = 0x00;
	// Send off the clock commands
	ftStatus = FT_Write(ftHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
	if (ftStatus != FT_OK) {
		CMD_LOG("readIR FT_Write failed! Generate a clock while in Test-Logic-Reset! ftStatus=0x%08x\n", ftStatus);
		return ftStatus;
	}

	return ftStatus;
}

bool FT4232_D2XX_JTAG_TISN74BCT8244A() 
{
	FT_STATUS ftStatus = 0;
	FT_HANDLE ftHandle = NULL;
	DWORD dwNumDevices = 0;
	BYTE byteIRdefault = 0;
	BYTE byteBypass = 0;
	bool bFound = false;


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
	CMD_LOG("\n");

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

		if (DeviceNameIsSet()) {
			if (!DeviceNameCompare(devInfo.Description)) {
				continue;
			}
			else {
				CMD_LOG("  Specified device is found!\n");
				bFound = true;
			}
		}

		CMD_LOG("\n");
		CMD_LOG("Assume device has the MPSSE and open it...\n");
		ftStatus = FT_Open(i, &ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("Open Failed with error 0x%08x\n", ftStatus);
			return false;
		}

		CMD_LOG("Configuring port for MPSSE use...\n");
		ftStatus = configurePortAsMPSSE(ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("Configure port failed with error 0x%08x\n", ftStatus);
			goto exit;
		}

		CMD_LOG("Sending a bad command...\n");
		ftStatus = sendBadCommand(ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("Sending a bad command failed with error 0x%08x\n", ftStatus);
			goto exit;
		}

		CMD_LOG("Configuring MPSSE settings for JTAG...\n");
		ftStatus = configureMPSSEasJTAG(ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("Configuring MPSSE settings for JTAG failed with error 0x%08x\n", ftStatus);
			goto exit;
		}

		CMD_LOG("Reading IR default value and BYPASS register...\n");
		ftStatus = readIR(ftHandle, &byteIRdefault, &byteBypass);
		if (ftStatus != FT_OK) {
			CMD_LOG("Reading IR default value and BYPASS register failed with error 0x%08x\n", ftStatus);
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

		FT_Close(ftHandle);
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
		FT_Close(ftHandle);
		ftHandle = NULL;
	}
	return (ftStatus == FT_OK);
}


