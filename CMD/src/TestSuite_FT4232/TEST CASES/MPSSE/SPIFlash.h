#pragma once
#include "libMPSSE_spi.h"
#include "../D2XX/ftd2xx.h"
#include "../D2XX/ftdiIds.h"
#include "../../TEST SUITE/TestSuite.h"
#include "../../TEST SUITE/TestLogger.h"
#pragma comment(lib, "libMPSSE.lib")
#include <stdio.h>



class SPIFlash
{
public:

	const uint8 MANID = 0x90;
	const uint8 PAGEPROG = 0x02;
	const uint8 READDATA = 0x03;
	const uint8 FASTREAD = 0x0B;
	const uint8 WRITEDISABLE = 0x04;
	const uint8 READSTAT1 = 0x05;
	const uint8 READSTAT2 = 0x35;
	const uint8 READSTAT3 = 0x15;
	const uint8 WRITESTATEN = 0x50;
	const uint8 WRITESTAT1 = 0x01;
	const uint8 WRITESTAT2 = 0x31;
	const uint8 WRITESTAT3 = 0x11;
	const uint8 WRITEENABLE = 0x06;
	const uint8 ADDR4BYTE_EN = 0xB7;
	const uint8 ADDR4BYTE_DIS = 0xE9;
	const uint8 SECTORERASE = 0x20;
	const uint8 BLOCK32ERASE = 0x52;
	const uint8 BLOCK64ERASE = 0xD8;
	const uint8 CHIPERASE = 0x60;
	const uint8 SUSPEND = 0x75;
	const uint8 ID = 0x90;
	const uint8 RESUME = 0x7A;
	const uint8 JEDECID = 0x9F;
	const uint8 POWERDOWN = 0xB9;
	const uint8 RELEASE = 0xAB;
	const uint8 READSFDP = 0x5A;
	const uint8 UNIQUEID = 0x4B;

	const uint8 BUSY = 0x01;
	const uint8 WRTEN = 0x02;
	const uint8 SUS = 0x80;

	const uint16 SPI_PAGESIZE = 0x100;


public:

	SPIFlash(FT_HANDLE ftHandle);

	uint32 getJEDECID(void);
	uint16 getManID(void);
	uint64 getUniqueID(void);

	bool powerUp(void);
	bool powerDown(void);
	bool suspendProg(void);
	bool resumeProg(void);

	bool eraseChip(void);
	bool eraseSector(uint32 ulAddr);
	bool eraseBlock32K();
	bool eraseBlock64K();

	bool write(uint32 ulAddr, uint8 *pucBuffer, uint32 ulBufferSize, bool bErrorCheck = true);
	bool read(uint32 ulAddr, uint8 *pucBuffer, uint32 ulBufferSize, bool bFastRead = false);


private:

	FT_HANDLE m_ftHandle;
	FT_STATUS m_ftStatus;
	bool bChipPowerDown = false;


	void CHIP_SELECT(void);
	void CHIP_DESELECT(void);
	FT_STATUS WRITE_INSTRUCTION(uint8 ucCommand, uint32 ulDummyCount, uint32 ulOptions);
	uint8 READ_STAT(uint8 ucStatMode);
	bool IS_POWERDOWN();
	bool IS_NOTBUSY();
	bool IS_NOSUSPEND();
	FT_STATUS WRITE_ADDRESS(uint32 ulAddr);
};






