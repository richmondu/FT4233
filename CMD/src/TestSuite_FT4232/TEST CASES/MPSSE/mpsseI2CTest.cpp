#include "libMPSSE_i2c.h"
#include "../D2XX/ftdiIds.h"
#include "../../TEST SUITE/TestSuite.h"
#include "../../TEST SUITE/TestLogger.h"
#pragma comment(lib, "libMPSSE.lib")
#include <stdio.h>



bool FT4232_MPSSE_I2C_Enumerate()
{
	FT_STATUS ftStatus = FT_OK;
	uint32 ulNumChannels = 0;
	bool bFound = false;


	Init_libMPSSE();

	ftStatus = I2C_GetNumChannels(&ulNumChannels);
	if (ftStatus != FT_OK) {
		CMD_LOG("I2C_GetNumChannels failed!\n");
		goto exit;
	}
	if (!ulNumChannels) {
		CMD_LOG("I2C_GetNumChannels failed! No channels found! ulNumChannels=%d\n", ulNumChannels);
		ftStatus = FT_DEVICE_NOT_FOUND;
		goto exit;
	}

	if (ulNumChannels != 2 && ulNumChannels != 1) {
		CMD_LOG("I2C_GetNumChannels warning! A FT_4232 should have 2 MPSSE capable channels enumerated! But UMFTPD2A is an exception.\n");
		ftStatus = FT_OTHER_ERROR;
		goto exit;
	}

	for (uint32 i = 0; i < ulNumChannels; i++) {
		FT_DEVICE_LIST_INFO_NODE devInfo;
		ftStatus = I2C_GetChannelInfo(i, &devInfo);
		if (ftStatus != FT_OK) {
			CMD_LOG("I2C_GetChannelInfo failed!\n");
			ftStatus = FT_OTHER_ERROR;
			goto exit;
		}
		CMD_LOG("Dev %d:\n", i);
		CMD_LOG("  ID=0x%08x (%s)\n", devInfo.ID, FT_GetVidPidString(devInfo.ID));
		CMD_LOG("  LocId=0x%08x\n", devInfo.LocId);
		CMD_LOG("  Type=0x%08x\n", devInfo.Type);
		CMD_LOG("  Flags=0x%08x\n", devInfo.Flags);
		CMD_LOG("  SerialNumber=%s\n", devInfo.SerialNumber);
		CMD_LOG("  Description=%s\n", devInfo.Description);

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
			Cleanup_libMPSSE();
			return false;
		}
	}


exit:
	Cleanup_libMPSSE();
	return (ftStatus == FT_OK);
}

bool FT4232_MPSSE_I2C_Open()
{
	FT_STATUS ftStatus = FT_OK;
	uint32 ulNumChannels = 0;


	Init_libMPSSE();

	ftStatus = I2C_GetNumChannels(&ulNumChannels);
	if (ftStatus != FT_OK) {
		CMD_LOG("I2C_GetNumChannels failed!\n");
		goto exit;
	}
	if (!ulNumChannels) {
		CMD_LOG("I2C_GetNumChannels failed! No channels found! ulNumChannels=%d\n", ulNumChannels);
		ftStatus = FT_DEVICE_NOT_FOUND;
		goto exit;
	}

	if (ulNumChannels != 2 && ulNumChannels != 1) {
		CMD_LOG("I2C_GetNumChannels warning! A FT_4232 should have 2 MPSSE capable channels enumerated! But UMFTPD2A is an exception.\n");
		ftStatus = FT_OTHER_ERROR;
		goto exit;
	}

	for (uint32 i = 0; i < ulNumChannels; i++) {
		FT_DEVICE_LIST_INFO_NODE devInfo;
		ftStatus = I2C_GetChannelInfo(i, &devInfo);
		if (ftStatus != FT_OK) {
			CMD_LOG("I2C_GetChannelInfo failed!\n");
			ftStatus = FT_OTHER_ERROR;
			goto exit;
		}

		if (DeviceNameIsSet()) {
			if (!DeviceNameCompare(devInfo.Description)) {
				continue;
			}
		}

		CMD_LOG("Dev %d:\n", i);
		CMD_LOG("  ID=0x%08x (%s)\n", devInfo.ID, FT_GetVidPidString(devInfo.ID));
		CMD_LOG("  Description=%s\n", devInfo.Description);

		FT_HANDLE ftHandle = NULL;
		ftStatus = I2C_OpenChannel(i, &ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("I2C_OpenChannel failed! Cannot open MPSSE channel\n");
			goto exit;
		}
		CMD_LOG("  Channel opened successfully.\n");
		ftStatus = I2C_CloseChannel(ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("I2C_CloseChannel failed!\n");
			goto exit;
		}
	}


exit:
	Cleanup_libMPSSE();
	return (ftStatus == FT_OK);
}

bool FT4232_MPSSE_I2C_Configure()
{
	FT_STATUS ftStatus = FT_OK;
	uint32 ulNumChannels = 0;


	Init_libMPSSE();

	ftStatus = I2C_GetNumChannels(&ulNumChannels);
	if (ftStatus != FT_OK) {
		CMD_LOG("I2C_GetNumChannels failed!\n");
		goto exit;
	}
	if (!ulNumChannels) {
		CMD_LOG("I2C_GetNumChannels failed! No channels found! ulNumChannels=%d\n", ulNumChannels);
		ftStatus = FT_DEVICE_NOT_FOUND;
		goto exit;
	}

	if (ulNumChannels != 2 && ulNumChannels != 1) {
		CMD_LOG("I2C_GetNumChannels warning! A FT_4232 should have 2 MPSSE capable channels enumerated! But UMFTPD2A is an exception.\n");
		ftStatus = FT_OTHER_ERROR;
		goto exit;
	}

	for (uint32 i = 0; i < ulNumChannels; i++) {
		FT_DEVICE_LIST_INFO_NODE devInfo;
		ftStatus = I2C_GetChannelInfo(i, &devInfo);
		if (ftStatus != FT_OK) {
			CMD_LOG("I2C_GetChannelInfo failed!\n");
			ftStatus = FT_OTHER_ERROR;
			goto exit;
		}

		if (DeviceNameIsSet()) {
			if (!DeviceNameCompare(devInfo.Description)) {
				continue;
			}
		}

		CMD_LOG("Dev %d:\n", i);
		CMD_LOG("  ID=0x%08x (%s)\n", devInfo.ID, FT_GetVidPidString(devInfo.ID));
		CMD_LOG("  Description=%s\n", devInfo.Description);

		FT_HANDLE ftHandle = NULL;
		ftStatus = I2C_OpenChannel(i, &ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("I2C_OpenChannel failed! Cannot open MPSSE channel\n");
			goto exit;
		}

		I2C_CLOCKRATE ClockRate[] = { 
			I2C_CLOCK_STANDARD_MODE , 
			I2C_CLOCK_FAST_MODE, 
			I2C_CLOCK_FAST_MODE_PLUS, 
			I2C_CLOCK_HIGH_SPEED_MODE 
		};

		uint8 Latency[] = {1, 4, 16, 64, 128, 255};

		for (int i = 0; i < sizeof(ClockRate)/sizeof(ClockRate[0]); i++) {
			for (int j = 0; j < sizeof(Latency)/sizeof(Latency[0]); j++) {
				for (int k = 0; k < 4; k++) {
					ChannelConfig config;
					config.ClockRate = (I2C_CLOCKRATE)ClockRate[i];
					config.LatencyTimer = (uint8)Latency[j];
					config.Options = (uint32)k;
					CMD_LOG("  Channel configured successfully: ClkRate=%07d LatencyTmr=%03d Options=%d\n",
						config.ClockRate, config.LatencyTimer, config.Options);
					ftStatus = I2C_InitChannel(ftHandle, &config);
					if (ftStatus != FT_OK) {
						CMD_LOG("I2C_InitChannel failed! Cannot configure MPSSE channel ClockRate=%d LatencyTimer=%d Options=%d\n", 
							config.ClockRate, config.LatencyTimer, config.Options);
						goto exit;
					}
				}
			}
		}

		ftStatus = I2C_CloseChannel(ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("I2C_CloseChannel failed!\n");
			goto exit;
		}
	}

exit:
	Cleanup_libMPSSE();
	return (ftStatus == FT_OK);
}

static FT_STATUS write_byte_EEPROM24LC024H(FT_HANDLE ftHandle, uint8 slaveAddress, uint8 registerAddress, uint8 data)
{
	FT_STATUS ftStatus;
	uint8 buffer[32] = { 0 };
	uint32 bytesToTransfer = 0;
	uint32 bytesTransfered;
	bool writeComplete = 0;
	uint32 retry = 0;

	bytesToTransfer = 0;
	bytesTransfered = 0;
	buffer[bytesToTransfer++] = registerAddress; /* Byte addressed inside EEPROM */
	buffer[bytesToTransfer++] = data;
	ftStatus = I2C_DeviceWrite(ftHandle, slaveAddress, bytesToTransfer, buffer, &bytesTransfered, 
		I2C_TRANSFER_OPTIONS_START_BIT | I2C_TRANSFER_OPTIONS_STOP_BIT);

	/* poll to check completition */
	while ((writeComplete == 0) && (retry<10))
	{
		bytesToTransfer = 0;
		bytesTransfered = 0;
		buffer[bytesToTransfer++] = registerAddress; /* Addressed inside EEPROM  */
		ftStatus = I2C_DeviceWrite(ftHandle, slaveAddress, bytesToTransfer, buffer, &bytesTransfered,
			I2C_TRANSFER_OPTIONS_START_BIT | I2C_TRANSFER_OPTIONS_BREAK_ON_NACK);
		if ((FT_OK == ftStatus) && (bytesToTransfer == bytesTransfered))
		{
			writeComplete = 1;
			//CMD_LOG("  ... Write done\n");
		}
		retry++;
		//CMD_LOG("Retry=%d\n",retry);
	}
	return ftStatus;
}

static FT_STATUS read_byte_EEPROM24LC024H(FT_HANDLE ftHandle, uint8 slaveAddress, uint8 registerAddress, uint8 *data)
{
	FT_STATUS ftStatus;
	uint8 buffer[32] = { 0 };
	uint32 bytesToTransfer = 0;
	uint32 bytesTransfered;

	bytesToTransfer = 0;
	bytesTransfered = 0;
	buffer[bytesToTransfer++] = registerAddress; /*Byte addressed inside EEPROM */
	ftStatus = I2C_DeviceWrite(ftHandle, slaveAddress, bytesToTransfer, buffer, &bytesTransfered, I2C_TRANSFER_OPTIONS_START_BIT);
	if (ftStatus != FT_OK || bytesToTransfer!= bytesTransfered) {
		CMD_LOG("I2C_DeviceWrite failed!\n");
		return ftStatus;
	}

	bytesToTransfer = 1;
	bytesTransfered = 0;
	ftStatus = I2C_DeviceRead(ftHandle, slaveAddress, bytesToTransfer, buffer, &bytesTransfered, I2C_TRANSFER_OPTIONS_START_BIT);
	if (ftStatus != FT_OK || bytesToTransfer != bytesTransfered) {
		CMD_LOG("I2C_DeviceRead failed!\n");
		return ftStatus;
	}
	*data = buffer[0];
	return ftStatus;
}

// FT4232_MPSSE_I2C_IO_EEPROM24LC024H test uses Microchip's 24LC024H EEPROM 
//   This is the same EEPROM used in MPSSE I2C Example at 
//   http://www.ftdichip.com/Support/SoftwareExamples/MPSSE/LibMPSSE-I2C/LibMPSSE-I2C_source.zip
//   24LC024H Datasheet can be downloaded at
//   http://ww1.microchip.com/downloads/en/devicedoc/22102a.pdf
// Must connect EEPROM 24LC024H to UMFTPD2A: SCK, MISO, MOSI, GND, VCC
bool FT4232_MPSSE_I2C_IO_EEPROM24LC024H()
{
	FT_STATUS ftStatus = FT_OK;
	uint32 ulNumChannels = 0;


	Init_libMPSSE();

	ftStatus = I2C_GetNumChannels(&ulNumChannels);
	if (ftStatus != FT_OK) {
		CMD_LOG("I2C_GetNumChannels failed!\n");
		goto exit;
	}
	if (!ulNumChannels) {
		CMD_LOG("I2C_GetNumChannels failed! No channels found! ulNumChannels=%d\n", ulNumChannels);
		ftStatus = FT_DEVICE_NOT_FOUND;
		goto exit;
	}

	if (ulNumChannels != 2 && ulNumChannels != 1) {
		CMD_LOG("I2C_GetNumChannels warning! A FT_4232 should have 2 MPSSE capable channels enumerated! But UMFTPD2A is an exception.\n");
		ftStatus = FT_OTHER_ERROR;
		goto exit;
	}

	for (uint32 i = 0; i < ulNumChannels; i++) {
		FT_DEVICE_LIST_INFO_NODE devInfo;
		ftStatus = I2C_GetChannelInfo(i, &devInfo);
		if (ftStatus != FT_OK) {
			CMD_LOG("I2C_GetChannelInfo failed!\n");
			ftStatus = FT_OTHER_ERROR;
			goto exit;
		}

		if (DeviceNameIsSet()) {
			if (!DeviceNameCompare(devInfo.Description)) {
				continue;
			}
		}

		CMD_LOG("Dev %d:\n", i);
		CMD_LOG("  ID=0x%08x (%s)\n", devInfo.ID, FT_GetVidPidString(devInfo.ID));
		CMD_LOG("  Description=%s\n", devInfo.Description);

		FT_HANDLE ftHandle = NULL;
		ftStatus = I2C_OpenChannel(i, &ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("I2C_OpenChannel failed! Cannot open MPSSE channel\n");
			goto exit;
		}

		ChannelConfig config;
		config.ClockRate = I2C_CLOCK_STANDARD_MODE;
		config.LatencyTimer = 255;
		config.Options = 0;
		ftStatus = I2C_InitChannel(ftHandle, &config);
		if (ftStatus != FT_OK) {
			CMD_LOG("I2C_InitChannel failed! Cannot configure MPSSE channel ClockRate=%d LatencyTimer=%d Options=%d\n",
				config.ClockRate, config.LatencyTimer, config.Options);
			I2C_CloseChannel(ftHandle);
			goto exit;
		}

		uint8 addressWrite = 0x57;
		uint8 addressRead = 0x57;
		uint8 addressStart = 0;
		uint8 addressEnd = 5;

		for (uint8 offset = addressStart; offset < addressEnd; offset++)
		{
			CMD_LOG("  writing address = %d data = %d\n", offset, 0xAA + offset);
			ftStatus = write_byte_EEPROM24LC024H(ftHandle, addressWrite, offset, 0xAA + offset);
			if (ftStatus != FT_OK) {
				CMD_LOG("write_byte failed!\n");
				I2C_CloseChannel(ftHandle);
				goto exit;
			}

			uint8 data = 0;
			ftStatus = read_byte_EEPROM24LC024H(ftHandle, addressRead, offset, &data);
			CMD_LOG("  reading address = %d data = %d\n", offset, data);
			if (ftStatus != FT_OK) {
				CMD_LOG("write_byte failed!\n");
				I2C_CloseChannel(ftHandle);
				goto exit;
			}

			if (data != 0xAA + offset) {
				CMD_LOG("ERROR: data mismatch! Please check sck, miso, mosi connection as well as vcc and gnd\n");
				I2C_CloseChannel(ftHandle);
				goto exit;
			}
		}
		CMD_LOG("  Channel write/read successfully\n");

		ftStatus = I2C_CloseChannel(ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("I2C_CloseChannel failed!\n");
			goto exit;
		}
	}

exit:
	Cleanup_libMPSSE();
	return (ftStatus == FT_OK);
}

bool FT4232_MPSSE_I2C_GPIO()
{
	FT_STATUS ftStatus = FT_OK;
	uint32 ulNumChannels = 0;


	Init_libMPSSE();

	ftStatus = I2C_GetNumChannels(&ulNumChannels);
	if (ftStatus != FT_OK) {
		CMD_LOG("I2C_GetNumChannels failed!\n");
		goto exit;
	}
	if (!ulNumChannels) {
		CMD_LOG("I2C_GetNumChannels failed! No channels found! ulNumChannels=%d\n", ulNumChannels);
		ftStatus = FT_DEVICE_NOT_FOUND;
		goto exit;
	}

	if (ulNumChannels != 2 && ulNumChannels != 1) {
		CMD_LOG("I2C_GetNumChannels warning! A FT_4232 should have 2 MPSSE capable channels enumerated! But UMFTPD2A is an exception.\n");
		ftStatus = FT_OTHER_ERROR;
		goto exit;
	}

	for (uint32 i = 0; i < ulNumChannels; i++) {
		FT_DEVICE_LIST_INFO_NODE devInfo;
		ftStatus = I2C_GetChannelInfo(i, &devInfo);
		if (ftStatus != FT_OK) {
			CMD_LOG("I2C_GetChannelInfo failed!\n");
			ftStatus = FT_OTHER_ERROR;
			goto exit;
		}

		if (DeviceNameIsSet()) {
			if (!DeviceNameCompare(devInfo.Description)) {
				continue;
			}
		}

		CMD_LOG("Dev %d:\n", i);
		CMD_LOG("  ID=0x%08x (%s)\n", devInfo.ID, FT_GetVidPidString(devInfo.ID));
		CMD_LOG("  Description=%s\n", devInfo.Description);

		FT_HANDLE ftHandle = NULL;
		ftStatus = I2C_OpenChannel(i, &ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("I2C_OpenChannel failed! Cannot open MPSSE channel\n");
			goto exit;
		}

		ChannelConfig config;
		config.ClockRate = I2C_CLOCK_HIGH_SPEED_MODE;
		config.LatencyTimer = 1;
		config.Options = 0;
		ftStatus = I2C_InitChannel(ftHandle, &config);
		if (ftStatus != FT_OK) {
			CMD_LOG("I2C_InitChannel failed! Cannot configure MPSSE channel ClockRate=%d LatencyTimer=%d Options=%d\n",
				config.ClockRate, config.LatencyTimer, config.Options);
			I2C_CloseChannel(ftHandle);
			goto exit;
		}

		uint8 gpio_value[] = { 0, 0xFF };
		uint8 gpio_dir[] = { 0, 0xFF };

		for (int i = 0; i < sizeof(gpio_value) / sizeof(gpio_value[0]); i++) {
			for (int j = 0; j < sizeof(gpio_dir) / sizeof(gpio_dir[0]); j++) {
				uint8 value = gpio_value[i];
				uint8 dir = gpio_dir[j];

				ftStatus = FT_WriteGPIO(ftHandle, dir, value);
				if (ftStatus != FT_OK) {
					CMD_LOG("FT_WriteGPIO failed! Cannot read gpio\n");
					I2C_CloseChannel(ftHandle);
					goto exit;
				}
				CMD_LOG("  FT_WriteGPIO value=%02x dir=%02x\n", value, dir);

				value = 0;
				ftStatus = FT_ReadGPIO(ftHandle, &value);
				if (ftStatus != FT_OK) {
					CMD_LOG("FT_ReadGPIO failed! Cannot read gpio\n");
					I2C_CloseChannel(ftHandle);
					goto exit;
				}
				CMD_LOG("  FT_ReadGPIO  value=%02x\n", value);
			}
		}

		ftStatus = I2C_CloseChannel(ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("I2C_CloseChannel failed!\n");
			goto exit;
		}
	}

exit:
	Cleanup_libMPSSE();
	return (ftStatus == FT_OK);
}
