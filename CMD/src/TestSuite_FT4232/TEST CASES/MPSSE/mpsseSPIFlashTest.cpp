#include "libMPSSE_spi.h"
#include "../D2XX/ftdiIds.h"
#include "../../TEST SUITE/TestSuite.h"
#include "../../TEST SUITE/TestLogger.h"
#pragma comment(lib, "libMPSSE.lib")
#include <stdio.h>
#include <time.h>
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

		CMD_LOG("  Channel configured successfully: ClkRate=%07d LatencyTmr=%03d Options=%02d\n\n",
			config.ClockRate, config.LatencyTimer, config.configOptions);


		SPIFlash* pSpiFlash = new SPIFlash(ftHandle); 
		{
			uint32 ulJedecID = 0;
			uint16 ulManID = 0;
			uint64 ullUniqueID = 0;

			ulJedecID = pSpiFlash->getJEDECID();
			CMD_LOG("JEDEC ID=0x%08x\n", ulJedecID);
			CMD_LOG("  ManufacturerID=0x%02x\n", (ulJedecID >> 16) & 0xFF);
			CMD_LOG("  MemoryTypeID=0x%02x\n", (ulJedecID >> 8) & 0xFF);
			CMD_LOG("  CapacityID=0x%02x\n", (ulJedecID >> 0) & 0xFF);
			CMD_LOG("\n");
			if (ulJedecID != 0x00ef4018) {
				CMD_LOG("Invalid. Expected JEDEC ID is 0x00ef4018\n");
				ftStatus = FT_OTHER_ERROR;
				goto exit2;
			}

			ulManID = pSpiFlash->getManID();
			CMD_LOG("MANUFACTURER ID=0x%08x\n", ulManID);
			CMD_LOG("  ManufacturerID=0x%02x\n", (ulManID >> 8) & 0xFF);
			CMD_LOG("  DeviceID=0x%02x\n", (ulManID >> 0) & 0xFF);
			CMD_LOG("\n");
			if (ulManID != 0x0000ef17) {
				CMD_LOG("Invalid. Expected MANUFACTURER ID is 0x0000ef17\n");
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

		CMD_LOG("  Channel configured successfully: ClkRate=%07d LatencyTmr=%03d Options=%02d\n\n",
			config.ClockRate, config.LatencyTimer, config.configOptions);


		SPIFlash* pSpiFlash = new SPIFlash(ftHandle);
		{
			uint32 ulJedecID = 0;
			uint16 ulManID = 0;
			uint64 ullUniqueID = 0;


			CMD_LOG("Before power restart...\n\n");

			ulJedecID = pSpiFlash->getJEDECID();
			CMD_LOG("JEDEC ID=0x%08x\n", ulJedecID);
			CMD_LOG("  ManufacturerID=0x%02x\n", (ulJedecID >> 16) & 0xFFFF);
			CMD_LOG("  MemoryTypeID=0x%02x\n", (ulJedecID >> 8) & 0xFF);
			CMD_LOG("  CapacityID=0x%02x\n", (ulJedecID >> 0) & 0xFF);
			CMD_LOG("\n");

			ulManID = pSpiFlash->getManID();
			CMD_LOG("MANUFACTURER ID=0x%08x\n", ulManID);
			CMD_LOG("  ManufacturerID=0x%02x\n", (ulManID >> 8) & 0xFF);
			CMD_LOG("  DeviceID=0x%02x\n", (ulManID >> 0) & 0xFF);
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
			CMD_LOG("  ManufacturerID=0x%02x\n", (ulJedecID >> 16) & 0xFFFF);
			CMD_LOG("  MemoryTypeID=0x%02x\n", (ulJedecID >> 8) & 0xFF);
			CMD_LOG("  CapacityID=0x%02x\n", (ulJedecID >> 0) & 0xFF);
			CMD_LOG("\n");

			ulManID = pSpiFlash->getManID();
			CMD_LOG("MANUFACTURER ID=0x%08x\n", ulManID);
			CMD_LOG("  ManufacturerID=0x%02x\n", (ulManID >> 8) & 0xFF);
			CMD_LOG("  DeviceID=0x%02x\n", (ulManID >> 0) & 0xFF);
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
			CMD_LOG("  ManufacturerID=0x%02x\n", (ulJedecID >> 16) & 0xFFFF);
			CMD_LOG("  MemoryTypeID=0x%02x\n", (ulJedecID >> 8) & 0xFF);
			CMD_LOG("  CapacityID=0x%02x\n", (ulJedecID >> 0) & 0xFF);
			CMD_LOG("\n");

			ulManID = pSpiFlash->getManID();
			CMD_LOG("MANUFACTURER ID=0x%08x\n", ulManID);
			CMD_LOG("  ManufacturerID=0x%02x\n", (ulManID >> 8) & 0xFF);
			CMD_LOG("  DeviceID=0x%02x\n", (ulManID >> 0) & 0xFF);
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
			CMD_LOG("  ManufacturerID=0x%02x\n", (ulJedecID >> 16) & 0xFFFF);
			CMD_LOG("  MemoryTypeID=0x%02x\n", (ulJedecID >> 8) & 0xFF);
			CMD_LOG("  CapacityID=0x%02x\n", (ulJedecID >> 0) & 0xFF);
			CMD_LOG("\n");

			ulManID = pSpiFlash->getManID();
			CMD_LOG("MANUFACTURER ID=0x%08x\n", ulManID);
			CMD_LOG("  ManufacturerID=0x%02x\n", (ulManID >> 8) & 0xFF);
			CMD_LOG("  DeviceID=0x%02x\n", (ulManID >> 0) & 0xFF);
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

	srand(time(NULL));
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
#define BASICIO_SIZE 256
#define BASICIO_ADDRESS 0
			uint8* pucBuffer = new uint8[BASICIO_SIZE];
			uint8* pucBuffer2 = new uint8[BASICIO_SIZE];

#if 1
			// Sector has to be erase before writing to the chip
			int sectorId = BASICIO_ADDRESS / 4096;
			int sectorCount = ((BASICIO_ADDRESS + BASICIO_SIZE) / 4096) - sectorId + 1;
			for (int zz=0; zz<sectorCount; zz++) {
				CMD_LOG("Erasing sector %d...\n\n", sectorId + zz);
				bRet = pSpiFlash->eraseSector(zz*4096);
				if (!bRet) {
					CMD_LOG("SPI EraseSector failed!\n");
					ftStatus = FT_OTHER_ERROR;
					delete pSpiFlash;
					pSpiFlash = NULL;
					goto exit;
				}
			}
#endif

			CMD_LOG("Writing %d bytes...\n\n", BASICIO_SIZE);
			for (int xy = 0; xy < BASICIO_SIZE; xy++) {
				pucBuffer[xy] = rand();
				CMD_LOG("%02x ", pucBuffer[xy]);
				if ((xy % 16) == 15) {
					CMD_LOG("\n");
				}
			}
			CMD_LOG("\n\n");

			bRet = pSpiFlash->write(BASICIO_ADDRESS, pucBuffer, BASICIO_SIZE, false);
			if (!bRet) {
				CMD_LOG("SPI Write failed!\n");
				ftStatus = FT_OTHER_ERROR;
				delete pSpiFlash;
				pSpiFlash = NULL;
				goto exit;
			}

			CMD_LOG("Reading %d bytes...\n\n", BASICIO_SIZE);
			memset(pucBuffer2, 0, BASICIO_SIZE);
			bRet = pSpiFlash->read(BASICIO_ADDRESS, pucBuffer2, BASICIO_SIZE, false);
			if (!bRet) {
				CMD_LOG("SPI Read failed!\n");
				ftStatus = FT_OTHER_ERROR;
				delete pSpiFlash;
				pSpiFlash = NULL;
				goto exit;
			}
			if (memcmp(pucBuffer, pucBuffer2, BASICIO_SIZE) != 0) {
				CMD_LOG("  Data mismatch!\n");
				for (int x = 0; x < BASICIO_SIZE; x++) {
					CMD_LOG("%02X ", pucBuffer2[x]);
					if ((x % 16) == 15) {
						CMD_LOG("\n");
					}
				}
				CMD_LOG("\n\n");
				ftStatus = FT_OTHER_ERROR;
				delete pSpiFlash;
				pSpiFlash = NULL;
				goto exit;
			}
			else {
				CMD_LOG("  Data matches!\n\n");
			}

			CMD_LOG("Fast reading %d bytes...\n\n", BASICIO_SIZE);
			memset(pucBuffer2, 0, BASICIO_SIZE);
			bRet = pSpiFlash->read(BASICIO_ADDRESS, pucBuffer2, BASICIO_SIZE, true);
			if (!bRet) {
				CMD_LOG("SPI Read failed!\n");
				ftStatus = FT_OTHER_ERROR;
				delete pSpiFlash;
				pSpiFlash = NULL;
				goto exit;
			}
			if (memcmp(pucBuffer, pucBuffer2, BASICIO_SIZE) != 0) {
				CMD_LOG("  Data mismatch!\n");
				for (int x = 0; x < BASICIO_SIZE; x++) {
					CMD_LOG("%02X ", pucBuffer2[x]);
					if ((x % 16) == 15) {
						CMD_LOG("\n");
					}
				}
				CMD_LOG("\n\n");
				ftStatus = FT_OTHER_ERROR;
				delete pSpiFlash;
				pSpiFlash = NULL;
				goto exit;
			}
			else {
				CMD_LOG("  Data matches!\n");
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
#define SIMPLEIO_SIZE 4096
#define SIMPLEIO_ADDRESS 0
			uint8* pucBuffer = new uint8[SIMPLEIO_SIZE];
			uint8* pucBuffer2 = new uint8[SIMPLEIO_SIZE];

#if 1
			// Sector has to be erase before writing to the chip
			int sectorId = SIMPLEIO_ADDRESS / 4096;
			int sectorCount = ((SIMPLEIO_ADDRESS + SIMPLEIO_SIZE) / 4096) - sectorId + 1;
			for (int zz = 0; zz<sectorCount; zz++) {
				CMD_LOG("Erasing sector %d...\n\n", sectorId + zz);
				bRet = pSpiFlash->eraseSector(zz * 4096);
				if (!bRet) {
					CMD_LOG("SPI EraseSector failed!\n");
					ftStatus = FT_OTHER_ERROR;
					delete pSpiFlash;
					pSpiFlash = NULL;
					goto exit;
				}
			}
#endif

			CMD_LOG("Writing %d bytes...\n\n", SIMPLEIO_SIZE);
			for (int xy = 0; xy < SIMPLEIO_SIZE; xy++) {
				pucBuffer[xy] = rand();
				CMD_LOG("%02x ", pucBuffer[xy]);
				if ((xy % 16) == 15) {
					CMD_LOG("\n");
				}
			}
			CMD_LOG("\n\n");

			bRet = pSpiFlash->write(SIMPLEIO_ADDRESS, pucBuffer, SIMPLEIO_SIZE, false);
			if (!bRet) {
				CMD_LOG("SPI Write failed!\n");
				ftStatus = FT_OTHER_ERROR;
				delete pSpiFlash;
				pSpiFlash = NULL;
				goto exit;
			}

			CMD_LOG("Reading %d bytes...\n\n", SIMPLEIO_SIZE);
			memset(pucBuffer2, 0, SIMPLEIO_SIZE);
			bRet = pSpiFlash->read(SIMPLEIO_ADDRESS, pucBuffer2, SIMPLEIO_SIZE, false);
			if (!bRet) {
				CMD_LOG("SPI Read failed!\n");
				ftStatus = FT_OTHER_ERROR;
				delete pSpiFlash;
				pSpiFlash = NULL;
				goto exit;
			}
			if (memcmp(pucBuffer, pucBuffer2, SIMPLEIO_SIZE) != 0) {
				CMD_LOG("  Data mismatch!\n");
				for (int x = 0; x < SIMPLEIO_SIZE; x++) {
					CMD_LOG("%02X ", pucBuffer2[x]);
					if ((x % 16) == 15) {
						CMD_LOG("\n");
					}
				}
				CMD_LOG("\n\n");
				ftStatus = FT_OTHER_ERROR;
				delete pSpiFlash;
				pSpiFlash = NULL;
				goto exit;
			}
			else {
				CMD_LOG("  Data matches!\n\n");
			}

			CMD_LOG("Fast reading %d bytes...\n\n", SIMPLEIO_SIZE);
			memset(pucBuffer2, 0, SIMPLEIO_SIZE);
			bRet = pSpiFlash->read(SIMPLEIO_ADDRESS, pucBuffer2, SIMPLEIO_SIZE, true);
			if (!bRet) {
				CMD_LOG("SPI Read failed!\n");
				ftStatus = FT_OTHER_ERROR;
				delete pSpiFlash;
				pSpiFlash = NULL;
				goto exit;
			}
			if (memcmp(pucBuffer, pucBuffer2, SIMPLEIO_SIZE) != 0) {
				CMD_LOG("  Data mismatch!\n");
				for (int x = 0; x < SIMPLEIO_SIZE; x++) {
					CMD_LOG("%02X ", pucBuffer2[x]);
					if ((x % 16) == 15) {
						CMD_LOG("\n");
					}
				}
				CMD_LOG("\n\n");
				ftStatus = FT_OTHER_ERROR;
				delete pSpiFlash;
				pSpiFlash = NULL;
				goto exit;
			}
			else {
				CMD_LOG("  Data matches!\n");
			}

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

bool FT4232_MPSSE_SPIFlash_W25Q128JV_MultiSectorIO()
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
#define MULTISECTORIO_SIZE 32768
#define MULTISECTORIO_ADDRESS 1234
			uint8* pucBuffer = new uint8[MULTISECTORIO_SIZE];
			uint8* pucBuffer2 = new uint8[MULTISECTORIO_SIZE];

#if 1
			// Sector has to be erase before writing to the chip
			int sectorId = MULTISECTORIO_ADDRESS / 4096;
			int sectorCount = ((MULTISECTORIO_ADDRESS + MULTISECTORIO_SIZE) / 4096) - sectorId + 1;
			for (int zz = 0; zz<sectorCount; zz++) {
				CMD_LOG("Erasing sector %d...\n\n", sectorId + zz);
				bRet = pSpiFlash->eraseSector(zz * 4096);
				if (!bRet) {
					CMD_LOG("SPI EraseSector failed!\n");
					ftStatus = FT_OTHER_ERROR;
					delete pSpiFlash;
					pSpiFlash = NULL;
					goto exit;
				}
			}
#endif

			CMD_LOG("Writing %d bytes...\n\n", MULTISECTORIO_SIZE);
			for (int xy = 0; xy < MULTISECTORIO_SIZE; xy++) {
				pucBuffer[xy] = rand();
				CMD_LOG("%02x ", pucBuffer[xy]);
				if ((xy % 16) == 15) {
					CMD_LOG("\n");
				}
			}
			CMD_LOG("\n\n");

			bRet = pSpiFlash->write(MULTISECTORIO_ADDRESS, pucBuffer, MULTISECTORIO_SIZE, false);
			if (!bRet) {
				CMD_LOG("SPI Write failed!\n");
				ftStatus = FT_OTHER_ERROR;
				delete pSpiFlash;
				pSpiFlash = NULL;
				goto exit;
			}

			CMD_LOG("Reading %d bytes...\n\n", MULTISECTORIO_SIZE);
			memset(pucBuffer2, 0, MULTISECTORIO_SIZE);
			bRet = pSpiFlash->read(MULTISECTORIO_ADDRESS, pucBuffer2, MULTISECTORIO_SIZE, false);
			if (!bRet) {
				CMD_LOG("SPI Read failed!\n");
				ftStatus = FT_OTHER_ERROR;
				delete pSpiFlash;
				pSpiFlash = NULL;
				goto exit;
			}
			if (memcmp(pucBuffer, pucBuffer2, MULTISECTORIO_SIZE) != 0) {
				CMD_LOG("  Data mismatch!\n");
				for (int x = 0; x < MULTISECTORIO_SIZE; x++) {
					CMD_LOG("%02X ", pucBuffer2[x]);
					if ((x % 16) == 15) {
						CMD_LOG("\n");
					}
				}
				CMD_LOG("\n\n");
				ftStatus = FT_OTHER_ERROR;
				delete pSpiFlash;
				pSpiFlash = NULL;
				goto exit;
			}
			else {
				CMD_LOG("  Data matches!\n\n");
			}

			CMD_LOG("Fast reading %d bytes...\n\n", MULTISECTORIO_SIZE);
			memset(pucBuffer2, 0, MULTISECTORIO_SIZE);
			bRet = pSpiFlash->read(MULTISECTORIO_ADDRESS, pucBuffer2, MULTISECTORIO_SIZE, true);
			if (!bRet) {
				CMD_LOG("SPI Read failed!\n");
				ftStatus = FT_OTHER_ERROR;
				delete pSpiFlash;
				pSpiFlash = NULL;
				goto exit;
			}
			if (memcmp(pucBuffer, pucBuffer2, MULTISECTORIO_SIZE) != 0) {
				CMD_LOG("  Data mismatch!\n");
				for (int x = 0; x < MULTISECTORIO_SIZE; x++) {
					CMD_LOG("%02X ", pucBuffer2[x]);
					if ((x % 16) == 15) {
						CMD_LOG("\n");
					}
				}
				CMD_LOG("\n\n");
				ftStatus = FT_OTHER_ERROR;
				delete pSpiFlash;
				pSpiFlash = NULL;
				goto exit;
			}
			else {
				CMD_LOG("  Data matches!\n");
			}

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
				{
					ftHandle = NULL;
					ftStatus = SPI_OpenChannel(i, &ftHandle);
					if (ftStatus != FT_OK) {
						CMD_LOG("SPI_OpenChannel failed! Cannot open MPSSE channel\n");
						goto exit;
					}

					ChannelConfig config;
					config.ClockRate = (I2C_CLOCKRATE)ClockRate[j];
					config.LatencyTimer = (uint8)Latency[k];
					config.configOptions = 0;
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
#define CLOCKEDIO_SIZE 256
#define CLOCKEDIO_ADDRESS 4000
						uint8* pucBuffer = new uint8[CLOCKEDIO_SIZE];
						uint8* pucBuffer2 = new uint8[CLOCKEDIO_SIZE];

#if 1
						// Sector has to be erase before writing to the chip
						int sectorId = CLOCKEDIO_ADDRESS / 4096;
						int sectorCount = ((CLOCKEDIO_ADDRESS + CLOCKEDIO_SIZE) / 4096) - sectorId + 1;
						for (int zz = 0; zz < sectorCount; zz++) {
							CMD_LOG("Erasing sector %d...\n\n", sectorId + zz);
							bRet = pSpiFlash->eraseSector(zz * 4096);
							if (!bRet) {
								CMD_LOG("SPI EraseSector failed!\n");
								ftStatus = FT_OTHER_ERROR;
								delete pSpiFlash;
								pSpiFlash = NULL;
								goto exit;
							}
						}
#endif

						CMD_LOG("Writing %d bytes...\n\n", CLOCKEDIO_SIZE);
						for (int xy = 0; xy < CLOCKEDIO_SIZE; xy++) {
							pucBuffer[xy] = rand();
							CMD_LOG("%02x ", pucBuffer[xy]);
							if ((xy % 16) == 15) {
								CMD_LOG("\n");
							}
						}
						CMD_LOG("\n\n");

						bRet = pSpiFlash->write(CLOCKEDIO_ADDRESS, pucBuffer, CLOCKEDIO_SIZE, false);
						if (!bRet) {
							CMD_LOG("SPI Write failed!\n");
							ftStatus = FT_OTHER_ERROR;
							delete pSpiFlash;
							pSpiFlash = NULL;
							goto exit;
						}

						CMD_LOG("Reading %d bytes...\n\n", CLOCKEDIO_SIZE);
						memset(pucBuffer2, 0, CLOCKEDIO_SIZE);
						bRet = pSpiFlash->read(CLOCKEDIO_ADDRESS, pucBuffer2, CLOCKEDIO_SIZE, false);
						if (!bRet) {
							CMD_LOG("SPI Read failed!\n");
							ftStatus = FT_OTHER_ERROR;
							delete pSpiFlash;
							pSpiFlash = NULL;
							goto exit;
						}
						if (memcmp(pucBuffer, pucBuffer2, CLOCKEDIO_SIZE) != 0) {
							CMD_LOG("  Data mismatch!\n");
							for (int x = 0; x < CLOCKEDIO_SIZE; x++) {
								CMD_LOG("%02X ", pucBuffer2[x]);
								if ((x % 16) == 15) {
									CMD_LOG("\n");
								}
							}
							CMD_LOG("\n\n");
							ftStatus = FT_OTHER_ERROR;
							delete pSpiFlash;
							pSpiFlash = NULL;
							goto exit;
						}
						else {
							CMD_LOG("  Data matches!\n\n");
						}

						CMD_LOG("Fast reading %d bytes...\n\n", CLOCKEDIO_SIZE);
						memset(pucBuffer2, 0, CLOCKEDIO_SIZE);
						bRet = pSpiFlash->read(CLOCKEDIO_ADDRESS, pucBuffer2, CLOCKEDIO_SIZE, true);
						if (!bRet) {
							CMD_LOG("SPI Read failed!\n");
							ftStatus = FT_OTHER_ERROR;
							delete pSpiFlash;
							pSpiFlash = NULL;
							goto exit;
						}
						if (memcmp(pucBuffer, pucBuffer2, CLOCKEDIO_SIZE) != 0) {
							CMD_LOG("  Data mismatch!\n");
							for (int x = 0; x < CLOCKEDIO_SIZE; x++) {
								CMD_LOG("%02X ", pucBuffer2[x]);
								if ((x % 16) == 15) {
									CMD_LOG("\n");
								}
							}
							CMD_LOG("\n\n");
							ftStatus = FT_OTHER_ERROR;
							delete pSpiFlash;
							pSpiFlash = NULL;
							goto exit;
						}
						else {
							CMD_LOG("  Data matches!\n");
						}

						delete[] pucBuffer;
						delete[] pucBuffer2;
						delete pSpiFlash;
						pSpiFlash = NULL;
					}

					ftStatus = SPI_CloseChannel(ftHandle);
					if (ftStatus != FT_OK) {
						CMD_LOG("SPI_CloseChannel failed!\n");
						goto exit;
					}
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
