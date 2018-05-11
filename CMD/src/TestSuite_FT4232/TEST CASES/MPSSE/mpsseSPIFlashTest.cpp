#include "libMPSSE_spi.h"
#include "../D2XX/ftdiIds.h"
#include "../../TEST SUITE/TestSuite.h"
#include "../../TEST SUITE/TestLogger.h"
#pragma comment(lib, "libMPSSE.lib")
#include <stdio.h>
#include "SPIFlash.h"



bool FT4232_MPSSE_SPIFlash_W25Q128JV_Enumerate()
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
			ftStatus = FT_OTHER_ERROR;
		}
	}


exit:
	Cleanup_libMPSSE();
	return (ftStatus == FT_OK);
}

bool FT4232_MPSSE_SPIFlash_W25Q128JV_Open()
{
	FT_STATUS ftStatus = FT_OK;
	uint32 ulNumChannels = 0;
	boolean bFound = false;


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
		CMD_LOG("  Description=%s\n", devInfo.Description);

		if (DeviceNameIsSet()) {
			if (!DeviceNameCompare(devInfo.Description)) {
				continue;
			}
			bFound = true;
			CMD_LOG("  Specified device is found!\n");
		}

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

		if (bFound) {
			break;
		}
	}

	if (DeviceNameIsSet()) {
		if (!bFound) {
			CMD_LOG("Specified device is NOT found!\n");
			ftStatus = FT_OTHER_ERROR;
		}
	}

exit:
	Cleanup_libMPSSE();
	return (ftStatus == FT_OK);
}

bool FT4232_MPSSE_SPIFlash_W25Q128JV_ReadIDs()
{
	FT_STATUS ftStatus = FT_OK;
	uint32 ulNumChannels = 0;
	boolean bFound = false;
	FT_HANDLE ftHandle = NULL;


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
		CMD_LOG("  Description=%s\n", devInfo.Description);

		if (DeviceNameIsSet()) {
			if (!DeviceNameCompare(devInfo.Description)) {
				continue;
			}
			bFound = true;
			CMD_LOG("  Specified device is found!\n");
		}

		ftHandle = NULL;
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
		config.reserved = 0;

		ftStatus = SPI_InitChannel(ftHandle, &config);
		if (ftStatus != FT_OK) {
			CMD_LOG("SPI_InitChannel failed! Cannot configure MPSSE channel ClockRate=%d LatencyTimer=%d Options=%02d\n",
				config.ClockRate, config.LatencyTimer, config.configOptions);
			goto exit;
		}

		CMD_LOG("  Channel configured successfully: ClkRate=%07d LatencyTmr=%03d Options=%02d\n",
			config.ClockRate, config.LatencyTimer, config.configOptions);


		SPIFlash* pSpiFlash = new SPIFlash(ftHandle); 
		{
			uint32 ulJedecID = 0;
			uint16 ulManID = 0;
			uint64 ullUniqueID = 0;

			ulJedecID = pSpiFlash->getJEDECID();
			CMD_LOG("JEDEC ID=0x%08x\n", ulJedecID);
			CMD_LOG("  ManufacturerID=0x%02x\n", (ulJedecID >> 16) && 0xFF);
			CMD_LOG("  MemoryTypeID=0x%02x\n", (ulJedecID >> 8) && 0xFF);
			CMD_LOG("  CapacityID=0x%02x\n", (ulJedecID >> 0) && 0xFF);
			CMD_LOG("\n");
			if (ulJedecID == 0) {
				ftStatus = FT_OTHER_ERROR;
				goto exit2;
			}

			ulManID = pSpiFlash->getManID();
			CMD_LOG("MANUFACTURER ID=0x%08x\n", ulManID);
			CMD_LOG("  ManufacturerID=0x%02x\n", (ulManID >> 8) && 0xFF);
			CMD_LOG("  DeviceID=0x%02x\n", (ulManID >> 0) && 0xFF);
			CMD_LOG("\n");
			if (ulManID == 0) {
				ftStatus = FT_OTHER_ERROR;
				goto exit2;
			}

			ullUniqueID = pSpiFlash->getUniqueID();
			CMD_LOG("UNIQUE ID=0x%0llx\n", ullUniqueID);
			CMD_LOG("\n");
			if (ullUniqueID == 0) {
				ftStatus = FT_OTHER_ERROR;
				goto exit2;
			}

exit2:
			delete pSpiFlash;
			pSpiFlash = NULL;
		}

		SPI_CloseChannel(ftHandle);
		ftHandle = NULL;

		if (bFound) {
			break;
		}
	}

	if (DeviceNameIsSet()) {
		if (!bFound) {
			CMD_LOG("Specified device is NOT found!\n");
			ftStatus = FT_OTHER_ERROR;
		}
	}

exit:
	if (ftHandle != NULL) {
		SPI_CloseChannel(ftHandle);
		ftHandle = NULL;
	}
	Cleanup_libMPSSE();
	return (ftStatus == FT_OK);
}

bool FT4232_MPSSE_SPIFlash_W25Q128JV_Power()
{
	FT_STATUS ftStatus = FT_OK;
	uint32 ulNumChannels = 0;
	bool bRet = false;
	boolean bFound = false;
	FT_HANDLE ftHandle = NULL;


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
		CMD_LOG("  Description=%s\n", devInfo.Description);

		if (DeviceNameIsSet()) {
			if (!DeviceNameCompare(devInfo.Description)) {
				continue;
			}
			bFound = true;
			CMD_LOG("  Specified device is found!\n");
		}

		ftHandle = NULL;
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
		config.reserved = 0;

		ftStatus = SPI_InitChannel(ftHandle, &config);
		if (ftStatus != FT_OK) {
			CMD_LOG("SPI_InitChannel failed! Cannot configure MPSSE channel ClockRate=%d LatencyTimer=%d Options=%02d\n",
				config.ClockRate, config.LatencyTimer, config.configOptions);
			goto exit;
		}

		CMD_LOG("  Channel configured successfully: ClkRate=%07d LatencyTmr=%03d Options=%02d\n",
			config.ClockRate, config.LatencyTimer, config.configOptions);


		SPIFlash* pSpiFlash = new SPIFlash(ftHandle);
		{
			uint32 ulJedecID = 0;
			uint16 ulManID = 0;
			uint64 ullUniqueID = 0;


			CMD_LOG("Before power restart...\n\n");

			ulJedecID = pSpiFlash->getJEDECID();
			CMD_LOG("JEDEC ID=0x%08x\n", ulJedecID);
			CMD_LOG("  ManufacturerID=0x%02x\n", (ulJedecID >> 16) && 0xFF);
			CMD_LOG("  MemoryTypeID=0x%02x\n", (ulJedecID >> 8) && 0xFF);
			CMD_LOG("  CapacityID=0x%02x\n", (ulJedecID >> 0) && 0xFF);
			CMD_LOG("\n");

			ulManID = pSpiFlash->getManID();
			CMD_LOG("MANUFACTURER ID=0x%08x\n", ulManID);
			CMD_LOG("  ManufacturerID=0x%02x\n", (ulManID >> 8) && 0xFF);
			CMD_LOG("  DeviceID=0x%02x\n", (ulManID >> 0) && 0xFF);
			CMD_LOG("\n");

			ullUniqueID = pSpiFlash->getUniqueID();
			CMD_LOG("UNIQUE ID=0x%0llx\n", ullUniqueID);
			CMD_LOG("\n");

			if (ulJedecID == 0 || ulManID == 0 || ullUniqueID == 0) {
				ftStatus = FT_OTHER_ERROR;
			}

			bRet = pSpiFlash->powerDown();
			CMD_LOG("Power down: %s!\n", bRet ? "successful" : "failed");
			bRet = pSpiFlash->powerUp();
			CMD_LOG("Power up: %s!\n", bRet ? "successful" : "failed");


			CMD_LOG("After power restart...\n\n");

			ulJedecID = pSpiFlash->getJEDECID();
			CMD_LOG("JEDEC ID=0x%08x\n", ulJedecID);
			CMD_LOG("  ManufacturerID=0x%02x\n", (ulJedecID >> 16) && 0xFF);
			CMD_LOG("  MemoryTypeID=0x%02x\n", (ulJedecID >> 8) && 0xFF);
			CMD_LOG("  CapacityID=0x%02x\n", (ulJedecID >> 0) && 0xFF);
			CMD_LOG("\n");

			ulManID = pSpiFlash->getManID();
			CMD_LOG("MANUFACTURER ID=0x%08x\n", ulManID);
			CMD_LOG("  ManufacturerID=0x%02x\n", (ulManID >> 8) && 0xFF);
			CMD_LOG("  DeviceID=0x%02x\n", (ulManID >> 0) && 0xFF);
			CMD_LOG("\n");

			ullUniqueID = pSpiFlash->getUniqueID();
			CMD_LOG("UNIQUE ID=0x%0llx\n", ullUniqueID);
			CMD_LOG("\n");

			if (ulJedecID == 0 || ulManID == 0 || ullUniqueID == 0) {
				ftStatus = FT_OTHER_ERROR;
			}

			delete pSpiFlash;
			pSpiFlash = NULL;
		}

		SPI_CloseChannel(ftHandle);
		ftHandle = NULL;

		if (bFound) {
			break;
		}
	}

	if (DeviceNameIsSet()) {
		if (!bFound) {
			CMD_LOG("Specified device is NOT found!\n");
			ftStatus = FT_OTHER_ERROR;
		}
	}

exit:
	if (ftHandle != NULL) {
		SPI_CloseChannel(ftHandle);
		ftHandle = NULL;
	}
	Cleanup_libMPSSE();
	return (ftStatus == FT_OK);
}

bool FT4232_MPSSE_SPIFlash_W25Q128JV_Suspend()
{
	FT_STATUS ftStatus = FT_OK;
	uint32 ulNumChannels = 0;
	bool bRet = false;
	boolean bFound = false;
	FT_HANDLE ftHandle = NULL;


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
		CMD_LOG("  Description=%s\n", devInfo.Description);

		if (DeviceNameIsSet()) {
			if (!DeviceNameCompare(devInfo.Description)) {
				continue;
			}
			bFound = true;
			CMD_LOG("  Specified device is found!\n");
		}

		ftHandle = NULL;
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
		config.reserved = 0;

		ftStatus = SPI_InitChannel(ftHandle, &config);
		if (ftStatus != FT_OK) {
			CMD_LOG("SPI_InitChannel failed! Cannot configure MPSSE channel ClockRate=%d LatencyTimer=%d Options=%02d\n",
				config.ClockRate, config.LatencyTimer, config.configOptions);
			goto exit;
		}

		CMD_LOG("  Channel configured successfully: ClkRate=%07d LatencyTmr=%03d Options=%02d\n",
			config.ClockRate, config.LatencyTimer, config.configOptions);


		SPIFlash* pSpiFlash = new SPIFlash(ftHandle);
		{
			uint32 ulJedecID = 0;
			uint16 ulManID = 0;
			uint64 ullUniqueID = 0;


			CMD_LOG("Before suspend...\n\n");

			ulJedecID = pSpiFlash->getJEDECID();
			CMD_LOG("JEDEC ID=0x%08x\n", ulJedecID);
			CMD_LOG("  ManufacturerID=0x%02x\n", (ulJedecID >> 16) && 0xFF);
			CMD_LOG("  MemoryTypeID=0x%02x\n", (ulJedecID >> 8) && 0xFF);
			CMD_LOG("  CapacityID=0x%02x\n", (ulJedecID >> 0) && 0xFF);
			CMD_LOG("\n");

			ulManID = pSpiFlash->getManID();
			CMD_LOG("MANUFACTURER ID=0x%08x\n", ulManID);
			CMD_LOG("  ManufacturerID=0x%02x\n", (ulManID >> 8) && 0xFF);
			CMD_LOG("  DeviceID=0x%02x\n", (ulManID >> 0) && 0xFF);
			CMD_LOG("\n");

			ullUniqueID = pSpiFlash->getUniqueID();
			CMD_LOG("UNIQUE ID=0x%0llx\n", ullUniqueID);
			CMD_LOG("\n");

			if (ulJedecID == 0 || ulManID == 0 || ullUniqueID == 0) {
				ftStatus = FT_OTHER_ERROR;
			}

			bRet = pSpiFlash->suspendProg();
			CMD_LOG("Suspend: %s!\n", bRet ? "successful" : "failed");
			bRet = pSpiFlash->resumeProg();
			CMD_LOG("Resume: %s!\n", bRet ? "successful" : "failed");


			CMD_LOG("After suspend...\n\n");

			ulJedecID = pSpiFlash->getJEDECID();
			CMD_LOG("JEDEC ID=0x%08x\n", ulJedecID);
			CMD_LOG("  ManufacturerID=0x%02x\n", (ulJedecID >> 16) && 0xFF);
			CMD_LOG("  MemoryTypeID=0x%02x\n", (ulJedecID >> 8) && 0xFF);
			CMD_LOG("  CapacityID=0x%02x\n", (ulJedecID >> 0) && 0xFF);
			CMD_LOG("\n");

			ulManID = pSpiFlash->getManID();
			CMD_LOG("MANUFACTURER ID=0x%08x\n", ulManID);
			CMD_LOG("  ManufacturerID=0x%02x\n", (ulManID >> 8) && 0xFF);
			CMD_LOG("  DeviceID=0x%02x\n", (ulManID >> 0) && 0xFF);
			CMD_LOG("\n");

			ullUniqueID = pSpiFlash->getUniqueID();
			CMD_LOG("UNIQUE ID=0x%0llx\n", ullUniqueID);
			CMD_LOG("\n");

			if (ulJedecID == 0 || ulManID == 0 || ullUniqueID == 0) {
				ftStatus = FT_OTHER_ERROR;
			}

			delete pSpiFlash;
			pSpiFlash = NULL;
		}

		SPI_CloseChannel(ftHandle);
		ftHandle = NULL;

		if (bFound) {
			break;
		}
	}

	if (DeviceNameIsSet()) {
		if (!bFound) {
			CMD_LOG("Specified device is NOT found!\n");
			ftStatus = FT_OTHER_ERROR;
		}
	}

exit:
	if (ftHandle != NULL) {
		SPI_CloseChannel(ftHandle);
		ftHandle = NULL;
	}
	Cleanup_libMPSSE();
	return (ftStatus == FT_OK);
}

bool FT4232_MPSSE_SPIFlash_W25Q128JV_BasicIO()
{
	FT_STATUS ftStatus = FT_OK;
	uint32 ulNumChannels = 0;
	bool bRet = false;
	boolean bFound = false;
	FT_HANDLE ftHandle = NULL;


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
		CMD_LOG("  Description=%s\n", devInfo.Description);

		if (DeviceNameIsSet()) {
			if (!DeviceNameCompare(devInfo.Description)) {
				continue;
			}
			bFound = true;
			CMD_LOG("  Specified device is found!\n");
		}

		ftHandle = NULL;
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
		config.reserved = 0;

		ftStatus = SPI_InitChannel(ftHandle, &config);
		if (ftStatus != FT_OK) {
			CMD_LOG("SPI_InitChannel failed! Cannot configure MPSSE channel ClockRate=%d LatencyTimer=%d Options=%02d\n",
				config.ClockRate, config.LatencyTimer, config.configOptions);
			goto exit;
		}

		CMD_LOG("  Channel configured successfully: ClkRate=%07d LatencyTmr=%03d Options=%02d\n",
			config.ClockRate, config.LatencyTimer, config.configOptions);


		SPIFlash* pSpiFlash = new SPIFlash(ftHandle);
		{
			uint8* pucBuffer = new uint8[16];
			uint8* pucBuffer2 = new uint8[16];


			CMD_LOG("Writing 16 bytes...\n\n");
			memcpy(pucBuffer, "SPI,HelloWorld!", 16);
			bRet = pSpiFlash->write(0, pucBuffer, 16, false);
			if (!bRet) {
				CMD_LOG("SPI Write failed!\n");
				ftStatus = FT_OTHER_ERROR;
				delete pSpiFlash;
				pSpiFlash = NULL;
				goto exit;
			}

			CMD_LOG("Reading 16 bytes...\n\n");
			memset(pucBuffer2, 0, 16);
			bRet = pSpiFlash->read(0, pucBuffer2, 16, false);
			if (!bRet) {
				CMD_LOG("SPI Read failed!\n");
				ftStatus = FT_OTHER_ERROR;
				delete pSpiFlash;
				pSpiFlash = NULL;
				goto exit;
			}
			if (memcmp(pucBuffer, pucBuffer2, 16) != 0) {
				CMD_LOG("Data mismatch!\n");
				for (int x = 0; x < 16; x++) {
					CMD_LOG("%02X ", pucBuffer[x]);
				}
				CMD_LOG("\n");
				for (int x = 0; x < 16; x++) {
					CMD_LOG("%02X ", pucBuffer2[x]);
				}
				CMD_LOG("\n\n");
				ftStatus = FT_OTHER_ERROR;
				delete pSpiFlash;
				pSpiFlash = NULL;
				goto exit;
			}
			CMD_LOG("Data matches!\n");

			CMD_LOG("Fast reading 16 bytes...\n\n");
			memset(pucBuffer2, 0, 16);
			bRet = pSpiFlash->read(0, pucBuffer2, 16, true);
			if (!bRet) {
				CMD_LOG("SPI Read failed!\n");
				ftStatus = FT_OTHER_ERROR;
				delete pSpiFlash;
				pSpiFlash = NULL;
				goto exit;
			}
			if (memcmp(pucBuffer, pucBuffer2, 16) != 0) {
				CMD_LOG("Data mismatch!\n");
				ftStatus = FT_OTHER_ERROR;
				delete pSpiFlash;
				pSpiFlash = NULL;
				goto exit;
			}
			CMD_LOG("Data matches!\n");

			delete[] pucBuffer;
			delete[] pucBuffer2;
			delete pSpiFlash;
			pSpiFlash = NULL;
		}


		SPI_CloseChannel(ftHandle);
		ftHandle = NULL;

		if (bFound) {
			break;
		}
	}

	if (DeviceNameIsSet()) {
		if (!bFound) {
			CMD_LOG("Specified device is NOT found!\n");
			ftStatus = FT_OTHER_ERROR;
		}
	}

exit:
	if (ftHandle != NULL) {
		SPI_CloseChannel(ftHandle);
		ftHandle = NULL;
	}
	Cleanup_libMPSSE();
	return (ftStatus == FT_OK);
}

bool FT4232_MPSSE_SPIFlash_W25Q128JV_SimpleIO()
{
	FT_STATUS ftStatus = FT_OK;
	uint32 ulNumChannels = 0;
	bool bRet = false;
	boolean bFound = false;
	FT_HANDLE ftHandle = NULL;


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
		CMD_LOG("  Description=%s\n", devInfo.Description);

		if (DeviceNameIsSet()) {
			if (!DeviceNameCompare(devInfo.Description)) {
				continue;
			}
			bFound = true;
			CMD_LOG("  Specified device is found!\n");
		}

		ftHandle = NULL;
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
		config.reserved = 0;

		ftStatus = SPI_InitChannel(ftHandle, &config);
		if (ftStatus != FT_OK) {
			CMD_LOG("SPI_InitChannel failed! Cannot configure MPSSE channel ClockRate=%d LatencyTimer=%d Options=%02d\n",
				config.ClockRate, config.LatencyTimer, config.configOptions);
			goto exit;
		}

		CMD_LOG("  Channel configured successfully: ClkRate=%07d LatencyTmr=%03d Options=%02d\n",
			config.ClockRate, config.LatencyTimer, config.configOptions);

		srand(NULL);
		SPIFlash* pSpiFlash = new SPIFlash(ftHandle);
		{
			#define BUFSIZE 65536
			uint8* pucBuffer = new uint8[BUFSIZE];
			uint8* pucBuffer2 = new uint8[BUFSIZE];

			for (int offset=0; offset<BUFSIZE; offset+=16384) {
				memset(pucBuffer + offset, 0x55, 4096);
				memset(pucBuffer + offset + 4096, 0xAA, 4096);
				for (int x=8192; x<12288; x++) {
					pucBuffer[offset + x] = (x % 256);
				}
				for (int x=12288; x<16384; x++) {
					pucBuffer[offset + x] = rand() % 256;
				}
			}

			CMD_LOG("Writing %d bytes...\n", BUFSIZE);
			bRet = pSpiFlash->write(0, pucBuffer, BUFSIZE, false);
			if (!bRet) {
				CMD_LOG("SPI Write failed!\n");
				ftStatus = FT_OTHER_ERROR;
				delete pSpiFlash;
				pSpiFlash = NULL;
				goto exit;
			}

			CMD_LOG("Reading %d bytes...\n", BUFSIZE);
			memset(pucBuffer2, 0, BUFSIZE);
			bRet = pSpiFlash->read(0, pucBuffer2, BUFSIZE, false);
			if (!bRet) {
				CMD_LOG("SPI Read failed!\n");
				ftStatus = FT_OTHER_ERROR;
				delete pSpiFlash;
				pSpiFlash = NULL;
				goto exit;
			}
			if (memcmp(pucBuffer, pucBuffer2, BUFSIZE) != 0) {
				CMD_LOG("Data mismatch!\n");
				ftStatus = FT_OTHER_ERROR;
				delete pSpiFlash;
				pSpiFlash = NULL;
				goto exit;
			}
			CMD_LOG("Data matches!\n");


			delete[] pucBuffer;
			delete[] pucBuffer2;
			delete pSpiFlash;
			pSpiFlash = NULL;
		}


		SPI_CloseChannel(ftHandle);

		if (bFound) {
			break;
		}
	}

	if (DeviceNameIsSet()) {
		if (!bFound) {
			CMD_LOG("Specified device is NOT found!\n");
			ftStatus = FT_OTHER_ERROR;
		}
	}

exit:
	if (ftHandle != NULL) {
		SPI_CloseChannel(ftHandle);
		ftHandle = NULL;
	}
	Cleanup_libMPSSE();
	return (ftStatus == FT_OK);
}

bool FT4232_MPSSE_SPIFlash_W25Q128JV_ClockedIO()
{
	FT_STATUS ftStatus = FT_OK;
	uint32 ulNumChannels = 0;
	bool bRet = false;
	boolean bFound = false;
	FT_HANDLE ftHandle = NULL;


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
		CMD_LOG("  Description=%s\n", devInfo.Description);

		if (DeviceNameIsSet()) {
			if (!DeviceNameCompare(devInfo.Description)) {
				continue;
			}
			bFound = true;
			CMD_LOG("  Specified device is found!\n");
		}

		I2C_CLOCKRATE ClockRate[] = {
			I2C_CLOCK_STANDARD_MODE ,
			I2C_CLOCK_FAST_MODE,
			I2C_CLOCK_FAST_MODE_PLUS,
			I2C_CLOCK_HIGH_SPEED_MODE
		};

		uint8 Latency[] = { 1, 64, 255 };

		srand(NULL);
		for (int j = 0; j < sizeof(ClockRate) / sizeof(ClockRate[0]); j++) {
			for (int k = 0; k < sizeof(Latency) / sizeof(Latency[0]); k++) {
				for (int l = 0; l < 64; l++) {

					// do not test everything
					if ((rand() % 8) != 0) {
						continue;
					}

					ftHandle = NULL;
					ftStatus = SPI_OpenChannel(i, &ftHandle);
					if (ftStatus != FT_OK) {
						CMD_LOG("SPI_OpenChannel failed! Cannot open MPSSE channel\n");
						goto exit;
					}

					ChannelConfig config;
					config.ClockRate = (I2C_CLOCKRATE)ClockRate[j];
					config.LatencyTimer = (uint8)Latency[k];
					config.configOptions = (uint32)l;
					config.Pin = 0;
					config.reserved = 0;

					CMD_LOG("  Channel configured successfully: ClkRate=%07d LatencyTmr=%03d Options=%02d\n",
						config.ClockRate, config.LatencyTimer, config.configOptions);

					ftStatus = SPI_InitChannel(ftHandle, &config);
					if (ftStatus != FT_OK) {
						CMD_LOG("SPI_InitChannel failed! Cannot configure MPSSE channel ClockRate=%d LatencyTimer=%d Options=%02d\n",
							config.ClockRate, config.LatencyTimer, config.configOptions);
						goto exit;
					}

					SPIFlash* pSpiFlash = new SPIFlash(ftHandle);
					{
#define BUFSIZE 65536
						uint8* pucBuffer = new uint8[BUFSIZE];
						uint8* pucBuffer2 = new uint8[BUFSIZE];

						for (int offset = 0; offset < BUFSIZE; offset += 16384) {
							memset(pucBuffer + offset, 0x55, 4096);
							memset(pucBuffer + offset + 4096, 0xAA, 4096);
							for (int x = 8192; x < 12288; x++) {
								pucBuffer[offset + x] = (x % 256);
							}
							for (int x = 12288; x < 16384; x++) {
								pucBuffer[offset + x] = rand() % 256;
							}
						}

						CMD_LOG("Writing %d bytes...\n", BUFSIZE);
						bRet = pSpiFlash->write(0, pucBuffer, BUFSIZE, false);
						if (!bRet) {
							CMD_LOG("SPI Write failed!\n");
							ftStatus = FT_OTHER_ERROR;
							delete pSpiFlash;
							pSpiFlash = NULL;
							goto exit;
						}

						CMD_LOG("Reading %d bytes...\n", BUFSIZE);
						memset(pucBuffer2, 0, BUFSIZE);
						bRet = pSpiFlash->read(0, pucBuffer2, BUFSIZE, false);
						if (!bRet) {
							CMD_LOG("SPI Read failed!\n");
							ftStatus = FT_OTHER_ERROR;
							delete pSpiFlash;
							pSpiFlash = NULL;
							goto exit;
						}
						if (memcmp(pucBuffer, pucBuffer2, BUFSIZE) != 0) {
							CMD_LOG("Data mismatch!\n");
							ftStatus = FT_OTHER_ERROR;
							delete pSpiFlash;
							pSpiFlash = NULL;
							goto exit;
						}
						CMD_LOG("Data matches!\n");


						delete[] pucBuffer;
						delete[] pucBuffer2;
						delete pSpiFlash;
						pSpiFlash = NULL;
					}

					SPI_CloseChannel(ftHandle);
					ftHandle = NULL;
				}
			}
		}

		if (bFound) {
			break;
		}
	}

	if (DeviceNameIsSet()) {
		if (!bFound) {
			CMD_LOG("Specified device is NOT found!\n");
			ftStatus = FT_OTHER_ERROR;
		}
	}

exit:
	if (ftHandle != NULL) {
		SPI_CloseChannel(ftHandle);
		ftHandle = NULL;
	}
	Cleanup_libMPSSE();
	return (ftStatus == FT_OK);
}

bool FT4232_MPSSE_SPIFlash_W25Q128JV_Erase()
{
	FT_STATUS ftStatus = FT_OK;
	uint32 ulNumChannels = 0;
	bool bRet = false;
	boolean bFound = false;
	FT_HANDLE ftHandle = NULL;


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
		CMD_LOG("  Description=%s\n", devInfo.Description);

		if (DeviceNameIsSet()) {
			if (!DeviceNameCompare(devInfo.Description)) {
				continue;
			}
			bFound = true;
			CMD_LOG("  Specified device is found!\n");
		}

		ftHandle = NULL;
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
		config.reserved = 0;

		ftStatus = SPI_InitChannel(ftHandle, &config);
		if (ftStatus != FT_OK) {
			CMD_LOG("SPI_InitChannel failed! Cannot configure MPSSE channel ClockRate=%d LatencyTimer=%d Options=%02d\n",
				config.ClockRate, config.LatencyTimer, config.configOptions);
			goto exit;
		}

		CMD_LOG("  Channel configured successfully: ClkRate=%07d LatencyTmr=%03d Options=%02d\n",
			config.ClockRate, config.LatencyTimer, config.configOptions);

		srand(NULL);
		SPIFlash* pSpiFlash = new SPIFlash(ftHandle);
		{
#define BUFERASE 65536
			uint8* pucBuffer = new uint8[BUFERASE];
			uint8* pucBuffer2 = new uint8[BUFERASE];

			for (int offset = 0; offset<BUFERASE; offset += 16384) {
				memset(pucBuffer + offset, 0x55, 4096);
				memset(pucBuffer + offset + 4096, 0xAA, 4096);
				for (int x = 8192; x<12288; x++) {
					pucBuffer[offset + x] = (x % 256);
				}
				for (int x = 12288; x<16384; x++) {
					pucBuffer[offset + x] = rand() % 256;
				}
			}

			CMD_LOG("Writing %d bytes...\n", BUFERASE);
			bRet = pSpiFlash->write(0, pucBuffer, BUFERASE, false);
			if (!bRet) {
				CMD_LOG("SPI Write failed!\n");
				ftStatus = FT_OTHER_ERROR;
				delete pSpiFlash;
				pSpiFlash = NULL;
				goto exit;
			}

			CMD_LOG("Erasing %d bytes...\n", BUFERASE);
			bRet = pSpiFlash->eraseChip();
			if (!bRet) {
				CMD_LOG("SPI Erase chip failed!\n");
				ftStatus = FT_OTHER_ERROR;
				delete pSpiFlash;
				pSpiFlash = NULL;
				goto exit;
			}

			delete[] pucBuffer;
			delete[] pucBuffer2;
			delete pSpiFlash;
			pSpiFlash = NULL;
		}


		SPI_CloseChannel(ftHandle);
		ftHandle = NULL;

		if (bFound) {
			break;
		}
	}

	if (DeviceNameIsSet()) {
		if (!bFound) {
			CMD_LOG("Specified device is NOT found!\n");
			ftStatus = FT_OTHER_ERROR;
		}
	}

exit:
	if (ftHandle != NULL) {
		SPI_CloseChannel(ftHandle);
		ftHandle = NULL;
	}
	Cleanup_libMPSSE();
	return (ftStatus == FT_OK);
}
