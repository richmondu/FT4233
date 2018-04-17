#include "libMPSSE_spi.h"
#include "../D2XX/ftdiIds.h"
#include "../../TEST SUITE/TestSuite.h"
#include "../../TEST SUITE/TestLogger.h"
#pragma comment(lib, "libMPSSE.lib")
#include <stdio.h>



bool FT4232_MPSSE_SPI_Enumerate()
{
	FT_STATUS ftStatus = FT_OK;
	uint32 ulNumChannels = 0;
	bool bFound = false;


	Init_libMPSSE();

	ftStatus = SPI_GetNumChannels(&ulNumChannels);
	if (ftStatus != FT_OK) {
		CMD_LOG("SPI_GetNumChannels failed!\n");
		goto exit;
	}
	if (!ulNumChannels) {
		CMD_LOG("SPI_GetNumChannels failed! No channels found! ulNumChannels=%d\n", ulNumChannels);
		ftStatus = FT_DEVICE_NOT_FOUND;
		goto exit;
	}

	if (ulNumChannels != 2 && ulNumChannels != 1) {
		CMD_LOG("SPI_GetNumChannels warning! A FT_4232 should have 2 MPSSE capable channels enumerated! But UMFTPD2A is an exception.\n");
		ftStatus = FT_OTHER_ERROR;
		goto exit;
	}

	for (uint32 i = 0; i < ulNumChannels; i++) {
		FT_DEVICE_LIST_INFO_NODE devInfo;
		ftStatus = SPI_GetChannelInfo(i, &devInfo);
		if (ftStatus != FT_OK) {
			CMD_LOG("SPI_GetChannelInfo failed!\n");
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

bool FT4232_MPSSE_SPI_Open()
{
	FT_STATUS ftStatus = FT_OK;
	uint32 ulNumChannels = 0;


	Init_libMPSSE();

	ftStatus = SPI_GetNumChannels(&ulNumChannels);
	if (ftStatus != FT_OK) {
		CMD_LOG("SPI_GetNumChannels failed!\n");
		goto exit;
	}
	if (!ulNumChannels) {
		CMD_LOG("SPI_GetNumChannels failed! No channels found! ulNumChannels=%d\n", ulNumChannels);
		ftStatus = FT_DEVICE_NOT_FOUND;
		goto exit;
	}

	if (ulNumChannels != 2 && ulNumChannels != 1) {
		CMD_LOG("SPI_GetNumChannels warning! A FT_4232 should have 2 MPSSE capable channels enumerated! But UMFTPD2A is an exception.\n");
		ftStatus = FT_OTHER_ERROR;
		goto exit;
	}

	for (uint32 i = 0; i < ulNumChannels; i++) {
		FT_DEVICE_LIST_INFO_NODE devInfo;
		ftStatus = SPI_GetChannelInfo(i, &devInfo);
		if (ftStatus != FT_OK) {
			CMD_LOG("SPI_GetChannelInfo failed!\n");
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
		ftStatus = SPI_OpenChannel(i, &ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("SPI_OpenChannel failed! Cannot open MPSSE channel\n");
			goto exit;
		}
		CMD_LOG("  Channel opened successfully.\n");
		ftStatus = SPI_CloseChannel(ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("SPI_CloseChannel failed!\n");
			goto exit;
		}
	}

exit:
	Cleanup_libMPSSE();
	return (ftStatus == FT_OK);
}

bool FT4232_MPSSE_SPI_Configure()
{
	FT_STATUS ftStatus = FT_OK;
	uint32 ulNumChannels = 0;


	Init_libMPSSE();

	ftStatus = SPI_GetNumChannels(&ulNumChannels);
	if (ftStatus != FT_OK) {
		CMD_LOG("SPI_GetNumChannels failed!\n");
		goto exit;
	}
	if (!ulNumChannels) {
		CMD_LOG("SPI_GetNumChannels failed! No channels found! ulNumChannels=%d\n", ulNumChannels);
		ftStatus = FT_DEVICE_NOT_FOUND;
		goto exit;
	}

	if (ulNumChannels != 2 && ulNumChannels != 1) {
		CMD_LOG("SPI_GetNumChannels warning! A FT_4232 should have 2 MPSSE capable channels enumerated! But UMFTPD2A is an exception.\n");
		ftStatus = FT_OTHER_ERROR;
		goto exit;
	}

	for (uint32 i = 0; i < ulNumChannels; i++) {
		FT_DEVICE_LIST_INFO_NODE devInfo;
		ftStatus = SPI_GetChannelInfo(i, &devInfo);
		if (ftStatus != FT_OK) {
			CMD_LOG("SPI_GetChannelInfo failed!\n");
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
		ftStatus = SPI_OpenChannel(i, &ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("SPI_OpenChannel failed! Cannot open MPSSE channel\n");
			goto exit;
		}

		I2C_CLOCKRATE ClockRate[] = {
			I2C_CLOCK_STANDARD_MODE ,
			I2C_CLOCK_FAST_MODE,
			I2C_CLOCK_FAST_MODE_PLUS,
			I2C_CLOCK_HIGH_SPEED_MODE
		};

		uint8 Latency[] = { 1, 64, 255 };

		srand(NULL);
		for (int i = 0; i < sizeof(ClockRate) / sizeof(ClockRate[0]); i++) {
			for (int j = 0; j < sizeof(Latency) / sizeof(Latency[0]); j++) {
				for (int k = 0; k < 64; k++) {

					// do not test everything
					if ((rand() % 8) != 0) {
						continue;
					}

					ChannelConfig config;
					config.ClockRate = (I2C_CLOCKRATE)ClockRate[i];
					config.LatencyTimer = (uint8)Latency[j];
					config.configOptions = (uint32)k;
					config.Pin = 0;
					config.reserved = 0;

					CMD_LOG("  Channel configured successfully: ClkRate=%07d LatencyTmr=%03d Options=%02d\n",
						config.ClockRate, config.LatencyTimer, config.configOptions);

					ftStatus = SPI_InitChannel(ftHandle, &config);
					if (ftStatus != FT_OK) {
						CMD_LOG("SPI_InitChannel failed! Cannot configure MPSSE channel ClockRate=%d LatencyTimer=%d Options=%02d\n",
							config.ClockRate, config.LatencyTimer, config.configOptions);
						SPI_CloseChannel(ftHandle);
						goto exit;
					}
				}
			}
		}

		ftStatus = SPI_CloseChannel(ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("SPI_CloseChannel failed!\n");
			goto exit;
		}
	}

exit:
	Cleanup_libMPSSE();
	return (ftStatus == FT_OK);
}

static FT_STATUS read_byte(FT_HANDLE ftHandle, uint8 slaveAddress, uint8 address, uint16 *data)
{
	uint8 buffer[32] = { 0 };
	uint32 sizeToTransfer = 0;
	uint32 sizeTransfered;
	uint8 writeComplete = 0;
	uint32 retry = 0;
	FT_STATUS status;

	/* CS_High + Write command + Address */
	sizeToTransfer = 1;
	sizeTransfered = 0;
	buffer[0] = 0xC0;/* Write command (3bits)*/
	buffer[0] = buffer[0] | ((address >> 3) & 0x0F);/*5 most significant add bits*/
	status = SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);
	if (status != FT_OK) {
		CMD_LOG("SPI_Write failed!\n");
		return status;
	}

	/*Write partial address bits */
	sizeToTransfer = 4;
	sizeTransfered = 0;
	buffer[0] = (address & 0x07) << 5; /* least significant 3 address bits */
	status = SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BITS);
	if (status != FT_OK) {
		CMD_LOG("SPI_Write failed!\n");
		return status;
	}

	/*Read 2 bytes*/
	sizeToTransfer = 2;
	sizeTransfered = 0;
	status = SPI_Read(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (status != FT_OK) {
		CMD_LOG("SPI_Read failed!\n");
		return status;
	}

	*data = (uint16)(buffer[1] << 8);
	*data = (*data & 0xFF00) | (0x00FF & (uint16)buffer[0]);

	return status;
}

static FT_STATUS write_byte(FT_HANDLE ftHandle, uint8 slaveAddress, uint8 address, uint16 data)
{
	uint8 buffer[32] = { 0 };
	uint32 sizeToTransfer = 0;
	uint32 sizeTransfered = 0;
	uint8 writeComplete = 0;
	uint32 retry = 0;
	FT_STATUS status;

	/* Write command EWEN(with CS_High -> CS_Low) */
	sizeToTransfer = 11;
	sizeTransfered = 0;
	buffer[0] = 0x9F;/* SPI_EWEN -> binary 10011xxxxxx (11bits) */
	buffer[1] = 0xFF;
	status = SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BITS |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (status != FT_OK) {
		CMD_LOG("SPI_Write failed!\n");
		return status;
	}

	/* CS_High + Write command + Address */
	sizeToTransfer = 1;
	sizeTransfered = 0;
	buffer[0] = 0xA0;/* Write command (3bits) */
	buffer[0] = buffer[0] | ((address >> 3) & 0x0F);/*5 most significant add bits*/
	status = SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);
	if (status != FT_OK) {
		CMD_LOG("SPI_Write failed!\n");
		return status;
	}

	/*Write 3 least sig address bits */
	sizeToTransfer = 3;
	sizeTransfered = 0;
	buffer[0] = (address & 0x07) << 5; /* least significant 3 address bits */
	status = SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BITS);
	if (status != FT_OK) {
		CMD_LOG("SPI_Write failed!\n");
		return status;
	}

	/* Write 2 byte data + CS_Low */
	sizeToTransfer = 2;
	sizeTransfered = 0;
	buffer[0] = (uint8)(data & 0xFF);
	buffer[1] = (uint8)((data & 0xFF00) >> 8);
	status = SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (status != FT_OK) {
		CMD_LOG("SPI_Write failed!\n");
		return status;
	}

	/* Wait until D0 is high */
#if 1
	/* Strobe Chip Select */
	sizeToTransfer = 0;
	sizeTransfered = 0;
	status = SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BITS |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);
	if (status != FT_OK) {
		CMD_LOG("SPI_Write failed!\n");
		return status;
	}
#ifndef __linux__
	Sleep(10);
#endif
	sizeToTransfer = 0;
	sizeTransfered = 0;
	status = SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BITS |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (status != FT_OK) {
		CMD_LOG("SPI_Write failed!\n");
		return status;
	}
#else
	retry = 0;
	state = FALSE;
	SPI_IsBusy(ftHandle, &state);
	while ((FALSE == state) && (retry<SPI_WRITE_COMPLETION_RETRY))
	{
		CMD_LOG("SPI device is busy(%u)\n", (unsigned)retry);
		SPI_IsBusy(ftHandle, &state);
		retry++;
	}
#endif
	/* Write command EWEN(with CS_High -> CS_Low) */
	sizeToTransfer = 11;
	sizeTransfered = 0;
	buffer[0] = 0x8F;/* SPI_EWEN -> binary 10011xxxxxx (11bits) */
	buffer[1] = 0xFF;
	status = SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BITS |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (status != FT_OK) {
		CMD_LOG("SPI_Write failed!\n");
		return status;
	}
	return status;
}

// FT4232_MPSSE_SPI_IO_EEPROM93LC56B test uses Microchip's 93LC56B EEPROM 
//   This is the same EEPROM used in MPSSE SPI Example at 
//   http://www.ftdichip.com/Support/SoftwareExamples/MPSSE/LibMPSSE-SPI/LibMPSSE-SPI_source.zip
//   93LC56B Datasheet can be downloaded at
//   http://ww1.microchip.com/downloads/en/DeviceDoc/21794F.pdf
// Must connect EEPROM 24LC024H to UMFTPD2A: SCK, MISO, MOSI, GND, VCC
bool FT4232_MPSSE_SPI_IO_EEPROM93LC56B()
{
	FT_STATUS ftStatus = FT_OK;
	uint32 ulNumChannels = 0;


	Init_libMPSSE();

	ftStatus = SPI_GetNumChannels(&ulNumChannels);
	if (ftStatus != FT_OK) {
		CMD_LOG("SPI_GetNumChannels failed!\n");
		goto exit;
	}
	if (!ulNumChannels) {
		CMD_LOG("SPI_GetNumChannels failed! No channels found! ulNumChannels=%d\n", ulNumChannels);
		ftStatus = FT_DEVICE_NOT_FOUND;
		goto exit;
	}

	if (ulNumChannels != 2 && ulNumChannels != 1) {
		CMD_LOG("SPI_GetNumChannels warning! A FT_4232 should have 2 MPSSE capable channels enumerated! But UMFTPD2A is an exception.\n");
		ftStatus = FT_OTHER_ERROR;
		goto exit;
	}

	for (uint32 i = 0; i < ulNumChannels; i++) {
		FT_DEVICE_LIST_INFO_NODE devInfo;
		ftStatus = SPI_GetChannelInfo(i, &devInfo);
		if (ftStatus != FT_OK) {
			CMD_LOG("SPI_GetChannelInfo failed!\n");
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
		ftStatus = SPI_OpenChannel(i, &ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("SPI_OpenChannel failed! Cannot open MPSSE channel\n");
			goto exit;
		}

		ChannelConfig config;
		config.ClockRate = I2C_CLOCK_HIGH_SPEED_MODE;
		config.LatencyTimer = 255;
		config.configOptions = SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3;
		config.Pin = 0;

		ftStatus = SPI_InitChannel(ftHandle, &config);
		if (ftStatus != FT_OK) {
			CMD_LOG("SPI_InitChannel failed! Cannot configure MPSSE channel ClockRate=%d LatencyTimer=%d Options=%02d\n",
				config.ClockRate, config.LatencyTimer, config.configOptions);
			SPI_CloseChannel(ftHandle);
			goto exit;
		}

		uint8 addressSlave = 0;
		uint8 addressStart = 0;
		uint8 addressEnd = 5;

		for (uint8 offset = addressStart; offset < addressEnd; offset++) {

			CMD_LOG("writing address = %d data = %d\n", offset, 0xAA + offset);
			ftStatus = write_byte(ftHandle, addressSlave, offset, 0xAA + offset);
			if (ftStatus != FT_OK) {
				CMD_LOG("write_byte failed!\n");
				SPI_CloseChannel(ftHandle);
				goto exit;
			}

			uint16 data = 0;
			ftStatus = read_byte(ftHandle, addressSlave, offset, &data);
			CMD_LOG("reading address = %d data = %d\n", offset, data);
			if (ftStatus != FT_OK) {
				CMD_LOG("write_byte failed!\n");
				SPI_CloseChannel(ftHandle);
				goto exit;
			}

			if (data != 0xAA + offset) {
				CMD_LOG("ERROR: data mismatch! Please check sk, cs, di, do connection as well as vcc and gnd\n");
				SPI_CloseChannel(ftHandle);
				goto exit;
			}
		}

		ftStatus = SPI_CloseChannel(ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("SPI_CloseChannel failed!\n");
			goto exit;
		}
	}

exit:
	Cleanup_libMPSSE();
	return (ftStatus == FT_OK);
}

bool FT4232_MPSSE_SPI_GPIO()
{
	FT_STATUS ftStatus = FT_OK;
	uint32 ulNumChannels = 0;


	Init_libMPSSE();

	ftStatus = SPI_GetNumChannels(&ulNumChannels);
	if (ftStatus != FT_OK) {
		CMD_LOG("SPI_GetNumChannels failed!\n");
		goto exit;
	}
	if (!ulNumChannels) {
		CMD_LOG("SPI_GetNumChannels failed! No channels found! ulNumChannels=%d\n", ulNumChannels);
		ftStatus = FT_DEVICE_NOT_FOUND;
		goto exit;
	}

	if (ulNumChannels != 2 && ulNumChannels != 1) {
		CMD_LOG("SPI_GetNumChannels warning! A FT_4232 should have 2 MPSSE capable channels enumerated! But UMFTPD2A is an exception.\n");
		ftStatus = FT_OTHER_ERROR;
		goto exit;
	}

	for (uint32 i = 0; i < ulNumChannels; i++) {
		FT_DEVICE_LIST_INFO_NODE devInfo;
		ftStatus = SPI_GetChannelInfo(i, &devInfo);
		if (ftStatus != FT_OK) {
			CMD_LOG("SPI_GetChannelInfo failed!\n");
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
		ftStatus = SPI_OpenChannel(i, &ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("SPI_OpenChannel failed! Cannot open MPSSE channel\n");
			goto exit;
		}

		ChannelConfig config;
		config.ClockRate = I2C_CLOCK_HIGH_SPEED_MODE;
		config.LatencyTimer = 1;
		config.configOptions = 0;
		ftStatus = SPI_InitChannel(ftHandle, &config);
		if (ftStatus != FT_OK) {
			CMD_LOG("SPI_InitChannel failed! Cannot configure MPSSE channel ClockRate=%d LatencyTimer=%d Options=%d\n",
				config.ClockRate, config.LatencyTimer, config.configOptions);
			SPI_CloseChannel(ftHandle);
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
					SPI_CloseChannel(ftHandle);
					goto exit;
				}
				CMD_LOG("  FT_WriteGPIO value=%02x dir=%02x\n", value, dir);

				value = 0;
				ftStatus = FT_ReadGPIO(ftHandle, &value);
				if (ftStatus != FT_OK) {
					CMD_LOG("FT_ReadGPIO failed! Cannot read gpio\n");
					SPI_CloseChannel(ftHandle);
					goto exit;
				}
				CMD_LOG("  FT_ReadGPIO  value=%02x\n", value);
			}
		}

		ftStatus = SPI_CloseChannel(ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("SPI_CloseChannel failed!\n");
			goto exit;
		}
	}

exit:
	Cleanup_libMPSSE();
	return (ftStatus == FT_OK);
}

