#include "SPIFlash.h"



SPIFlash::SPIFlash(FT_HANDLE ftHandle) {
	m_ftHandle = ftHandle;
}

uint32 SPIFlash::getJEDECID(void) {
	uint32 ulTransferred = 0;
	uint32 ulResult = 0;
	uint8 ucResult = 0;


	// Write the JEDEC ID
	m_ftStatus = WRITE_INSTRUCTION(JEDECID, 0, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);
	if (!m_ftStatus) {
		printf("getJEDECID: SPI_Write failed!\n");
		CHIP_DESELECT();
		return 0;
	}

	// Read Manufacturer ID
	m_ftStatus = SPI_Read(m_ftHandle, &ucResult, 1, &ulTransferred, 0);
	if (!m_ftStatus) {
		printf("getJEDECID: SPI_Read failed!\n");
		CHIP_DESELECT();
		return 0;
	}
	ulResult = (ucResult & 0xFF);

	// Read Memory Type ID
	m_ftStatus = SPI_Read(m_ftHandle, &ucResult, 1, &ulTransferred, 0);
	if (!m_ftStatus) {
		printf("getJEDECID: SPI_Read failed!\n");
		CHIP_DESELECT();
		return 0;
	}
	ulResult = (ulResult << 8) | (ucResult & 0xFF);

	// Read Capacity ID
	m_ftStatus = SPI_Read(m_ftHandle, &ucResult, 1, &ulTransferred, SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (!m_ftStatus) {
		printf("getJEDECID: SPI_Read failed!\n");
		CHIP_DESELECT();
		return 0;
	}
	ulResult = (ulResult << 8) | (ucResult & 0xFF);

	CHIP_DESELECT();
	return ulResult;
}

uint16 SPIFlash::getManID(void) {
	uint32 ulTransferred = 0;
	uint16 uwResult = 0;
	uint8 ucResult = 0;


	// Write the MAN ID
	m_ftStatus = WRITE_INSTRUCTION(MANID, 1, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);
	if (!m_ftStatus) {
		printf("getManID: SPI_Write failed!\n");
		CHIP_DESELECT();
		return 0;
	}

	// Read Manufacturer ID
	m_ftStatus = SPI_Read(m_ftHandle, &ucResult, 1, &ulTransferred, 0);
	if (!m_ftStatus) {
		printf("getManID: SPI_Read failed!\n");
		CHIP_DESELECT();
		return 0;
	}
	uwResult = (ucResult & 0xFF);

	// Read Device ID
	m_ftStatus = SPI_Read(m_ftHandle, &ucResult, 1, &ulTransferred, SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (!m_ftStatus) {
		printf("getManID: SPI_Read failed!\n");
		CHIP_DESELECT();
		return 0;
	}
	uwResult = (uwResult << 8) | (ucResult & 0xFF);

	CHIP_DESELECT();
	return uwResult;
}

uint64 SPIFlash::getUniqueID(void) {
	uint32 ulTransferred = 0;
	uint64 ullResult = 0;
	uint8 ucResult = 0;


	// Write the Unique ID
	m_ftStatus = WRITE_INSTRUCTION(UNIQUEID, 4, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);
	if (!m_ftStatus) {
		printf("getUniqueID: SPI_Write failed!\n");
		CHIP_DESELECT();
		return 0;
	}

	// Read Unique ID
	m_ftStatus = SPI_Read(m_ftHandle, (uint8*)&ullResult, sizeof(ullResult), &ulTransferred, SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (!m_ftStatus) {
		printf("getUniqueID: SPI_Read failed!\n");
		CHIP_DESELECT();
		return 0;
	}

	CHIP_DESELECT();
	return ullResult;
}

//Wakes chip from low power state.
bool SPIFlash::powerUp(void) {

	// Write the RELEASE ID
	m_ftStatus = WRITE_INSTRUCTION(RELEASE, 0, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (!m_ftStatus) {
		printf("powerUp: SPI_Write failed!\n");
		CHIP_DESELECT();
		return false;
	}

	Sleep(1);

	// Write the WRITEENABLE ID
	m_ftStatus = WRITE_INSTRUCTION(WRITEENABLE, 0, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (!m_ftStatus) {
		printf("powerUp: SPI_Write failed!\n");
		CHIP_DESELECT();
		return false;
	}
	if (!(READ_STAT(READSTAT1) & WRTEN)) {
		printf("powerDown: SPI_Write failed! WRTEN not set\n");
		return false;
	}

	// Write the WRITEENABLE ID
	m_ftStatus = WRITE_INSTRUCTION(WRITEDISABLE, 0, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (!m_ftStatus) {
		printf("powerUp: SPI_Write failed!\n");
		CHIP_DESELECT();
		return false;
	}

	bChipPowerDown = false;

	CHIP_DESELECT();
	return true;
}

//Puts device in low power state. Good for battery powered operations.
//In powerDown() the chip will only respond to powerUp()
bool SPIFlash::powerDown(void) {

	// Write the POWERDOWN ID
	m_ftStatus = WRITE_INSTRUCTION(POWERDOWN, 0, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (!m_ftStatus) {
		printf("powerDown: SPI_Write failed!\n");
		CHIP_DESELECT();
		return false;
	}

	bChipPowerDown = true;
	Sleep(1);

	// Write the WRITEENABLE ID
	m_ftStatus = WRITE_INSTRUCTION(WRITEENABLE, 0, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (!m_ftStatus) {
		printf("powerDown: SPI_Write failed!\n");
		CHIP_DESELECT();
		return false;
	}
	if (!(READ_STAT(READSTAT1) & WRTEN)) {
		printf("powerDown: SPI_Write failed! WRTEN not set\n");
		return false;
	}

	CHIP_DESELECT();
	return true;
}

//Suspends current Block Erase/Sector Erase/Page Program. Does not suspend chipErase().
//Page Program, Write Status Register, Erase instructions are not allowed.
//Erase suspend is only allowed during Block/Sector erase.
//Program suspend is only allowed during Page/Quad Page Program
bool SPIFlash::suspendProg(void) {

	if (IS_POWERDOWN() || IS_NOTBUSY()) {
		return false;
	}
	if (!IS_NOSUSPEND()) {
		return true;
	}

	// Write the SUSPEND ID
	m_ftStatus = WRITE_INSTRUCTION(SUSPEND, 0, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (!m_ftStatus) {
		printf("powerDown: SPI_Write failed!\n");
		CHIP_DESELECT();
		return false;
	}

	Sleep(1);

	if (!IS_NOTBUSY() || IS_NOSUSPEND()) {
		return false;
	}

	return true;
}

//Resumes previously suspended Block Erase/Sector Erase/Page Program.
bool SPIFlash::resumeProg(void) {

	if (IS_POWERDOWN() || !IS_NOTBUSY() || IS_NOSUSPEND()) {
		return false;
	}

	// Write the RESUME ID
	m_ftStatus = WRITE_INSTRUCTION(RESUME, 0, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (!m_ftStatus) {
		printf("powerDown: SPI_Write failed!\n");
		CHIP_DESELECT();
		return false;
	}

	Sleep(1);

	if (IS_NOTBUSY() || !IS_NOSUSPEND()) {
		return false;
	}

	return true;
}

//Erases whole chip. Think twice before using.
bool SPIFlash::eraseChip(void) {

	if (IS_POWERDOWN() || !IS_NOTBUSY()) {
		return false;
	}

	// Write the WRITEENABLE ID
	m_ftStatus = WRITE_INSTRUCTION(WRITEENABLE, 0, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (!m_ftStatus) {
		printf("eraseChip: SPI_Write failed!\n");
		CHIP_DESELECT();
		return false;
	}
	if (!(READ_STAT(READSTAT1) & WRTEN)) {
		printf("eraseChip: SPI_Write failed! WRTEN not set\n");
		return false;
	}

	// Write the CHIPERASE ID
	m_ftStatus = WRITE_INSTRUCTION(CHIPERASE, 0, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (!m_ftStatus) {
		printf("eraseChip: SPI_Write failed!\n");
		CHIP_DESELECT();
		return false;
	}

	while (READ_STAT(READSTAT1) & BUSY) {
		printf("eraseChip: Chip erase is ongoing...\n");
		Sleep(1);
	}

	return true;
}

bool SPIFlash::eraseSector(uint32 ulAddr) {

	if (IS_POWERDOWN() || !IS_NOTBUSY()) {
		return false;
	}

	//if (addr + 4096 >= _chip.capacity) {
	//	return false;
	//}

	// Write the WRITEENABLE ID
	m_ftStatus = WRITE_INSTRUCTION(WRITEENABLE, 0, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (!m_ftStatus) {
		printf("eraseChip: SPI_Write failed!\n");
		CHIP_DESELECT();
		return false;
	}
	if (!(READ_STAT(READSTAT1) & WRTEN)) {
		printf("eraseChip: SPI_Write failed! WRTEN not set\n");
		return false;
	}

	// Write the SECTORERASE ID
	m_ftStatus = WRITE_INSTRUCTION(SECTORERASE, 0, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (!m_ftStatus) {
		printf("eraseChip: SPI_Write failed!\n");
		CHIP_DESELECT();
		return false;
	}

	Sleep(1000);

	if (!IS_NOTBUSY()) {
		printf("eraseChip: IS_NOTBUSY failed!\n");
		return false;
	}

	return true;
}

bool SPIFlash::eraseBlock32K() {

	if (IS_POWERDOWN() || !IS_NOTBUSY()) {
		return false;
	}

	//if (addr + 32768 >= _chip.capacity) {
	//	return false;
	//}

	// Write the WRITEENABLE ID
	m_ftStatus = WRITE_INSTRUCTION(WRITEENABLE, 0, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (!m_ftStatus) {
		printf("eraseBlock32K: SPI_Write failed!\n");
		CHIP_DESELECT();
		return false;
	}

	Sleep(1000);

	if (!IS_NOTBUSY()) {
		printf("eraseBlock32K: IS_NOTBUSY failed!\n");
		return false;
	}

	// Write the BLOCK32ERASE ID
	m_ftStatus = WRITE_INSTRUCTION(BLOCK32ERASE, 0, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (!m_ftStatus) {
		printf("eraseBlock32K: SPI_Write failed!\n");
		CHIP_DESELECT();
		return false;
	}

	return true;
}

bool SPIFlash::eraseBlock64K() {

	if (IS_POWERDOWN() || !IS_NOTBUSY()) {
		return false;
	}

	//if (addr + 65536 >= _chip.capacity) {
	//	return false;
	//}

	m_ftStatus = WRITE_INSTRUCTION(BLOCK64ERASE, 0, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (!m_ftStatus) {
		printf("eraseBlock64K: SPI_Write failed!\n");
		CHIP_DESELECT();
		return false;
	}

	Sleep(1200);

	if (!IS_NOTBUSY()) {
		printf("eraseBlock64K: IS_NOTBUSY failed!\n");
		return false;
	}

	// Write the BLOCK64ERASE ID
	m_ftStatus = WRITE_INSTRUCTION(BLOCK64ERASE, 0, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (!m_ftStatus) {
		printf("eraseBlock64K: SPI_Write failed!\n");
		CHIP_DESELECT();
		return false;
	}

	return true;
}

// Writes an array of bytes starting from a specific location in a page.
//  Takes four arguments -
//    1. _addr --> Any address - from 0 to capacity
//    2. data_buffer --> The pointer to the array of bytes be written to a particular location on a page
//    3. bufferSize --> Size of the array of bytes - in number of bytes
//    4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::write(uint32 ulAddr, uint8 *pucBuffer, uint32 ulBufferSize, bool bErrorCheck)
{
	if (IS_POWERDOWN() || !IS_NOTBUSY()) {
		return false;
	}

	//if (addr + 32768 >= _chip.capacity) {
	//	return false;
	//}

	// Write the WRITEENABLE ID
	m_ftStatus = WRITE_INSTRUCTION(WRITEENABLE, 0, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (!m_ftStatus) {
		printf("write: SPI_Write failed!\n");
		CHIP_DESELECT();
		return false;
	}
	if (!(READ_STAT(READSTAT1) & WRTEN)) {
		printf("write: SPI_Write failed! WRTEN not set\n");
		return false;
	}


	uint16_t maxBytes = SPI_PAGESIZE - (ulAddr % SPI_PAGESIZE);  // Force the first set of bytes to stay within the first page

	if (ulBufferSize <= maxBytes) {
		//CHIP_SELECT();

		// Write the PAGEPROG ID
		m_ftStatus = WRITE_INSTRUCTION(PAGEPROG, 0, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);
		if (!m_ftStatus) {
			printf("write: SPI_Write failed!\n");
			CHIP_DESELECT();
			return false;
		}

		// Write address
		uint32 ulTransferred = 0;
		m_ftStatus = SPI_Write(m_ftHandle, (uint8*)&ulAddr, 4, &ulTransferred, 0);
		if (!m_ftStatus) {
			printf("write: SPI_Write failed!\n");
			CHIP_DESELECT();
			return 0;
		}

		for (uint16_t i = 0; i < ulBufferSize; ++i) {
			ulTransferred = 0;
			m_ftStatus = SPI_Write(m_ftHandle, &pucBuffer[i], 1, &ulTransferred, 0);
			if (!m_ftStatus) {
				printf("write: SPI_Write failed!\n");
				CHIP_DESELECT();
				return 0;
			}
		}

		CHIP_DESELECT();
	}
	else {
		uint32_t length = ulBufferSize;
		uint16_t writeBufSz;
		uint16_t data_offset = 0;
		uint32 ulTempAddr = ulAddr;

		do {
			writeBufSz = (length <= maxBytes) ? length : maxBytes;

			//CHIP_SELECT();

			// Write the PAGEPROG ID
			m_ftStatus = WRITE_INSTRUCTION(PAGEPROG, 0, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);
			if (!m_ftStatus) {
				printf("write: SPI_Write failed!\n");
				CHIP_DESELECT();
				return false;
			}

			// Write address
			uint32 ulTransferred = 0;
			m_ftStatus = SPI_Write(m_ftHandle, (uint8*)&ulTempAddr, 4, &ulTransferred, 0);
			if (!m_ftStatus) {
				printf("write: SPI_Write failed!\n");
				CHIP_DESELECT();
				return 0;
			}

			for (uint16_t i = 0; i < writeBufSz; ++i) {
				ulTransferred = 0;
				m_ftStatus = SPI_Write(m_ftHandle, &pucBuffer[i], 1, &ulTransferred, 0);
				if (!m_ftStatus) {
					printf("write: SPI_Write failed!\n");
					CHIP_DESELECT();
					return 0;
				}
			}
			CHIP_DESELECT();

			ulTempAddr += writeBufSz;
			data_offset += writeBufSz;
			length -= writeBufSz;
			maxBytes = 256;   // Now we can do up to 256 bytes per loop

			if (!IS_NOTBUSY()) {
				return false;
			}

			// Write the WRITEENABLE ID
			m_ftStatus = WRITE_INSTRUCTION(WRITEENABLE, 0, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
			if (!m_ftStatus) {
				printf("eraseChip: SPI_Write failed!\n");
				CHIP_DESELECT();
				return false;
			}
			if (!(READ_STAT(READSTAT1) & WRTEN)) {
				printf("eraseChip: SPI_Write failed! WRTEN not set\n");
				return false;
			}

		} while (length > 0);
	}

	if (!bErrorCheck) {
		CHIP_DESELECT();
		return true;
	}
	else {
		if (!IS_NOTBUSY()) {
			return false;
		}

		//CHIP_SELECT();

		// Write the READDATA ID
		m_ftStatus = WRITE_INSTRUCTION(READDATA, 0, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);
		if (!m_ftStatus) {
			printf("write: SPI_Write failed!\n");
			CHIP_DESELECT();
			return false;
		}

		// Write address
		uint32 ulTransferred = 0;
		m_ftStatus = SPI_Write(m_ftHandle, (uint8*)&ulAddr, 4, &ulTransferred, 0);
		if (!m_ftStatus) {
			printf("write: SPI_Write failed!\n");
			CHIP_DESELECT();
			return 0;
		}

		for (uint16_t j = 0; j < ulBufferSize; j++) {
			uint8 ucByte = 0;
			ulTransferred = 0;
			m_ftStatus = SPI_Read(m_ftHandle, &ucByte, 1, &ulTransferred, 0);
			if (!m_ftStatus) {
				printf("write: SPI_Read failed! m_ftStatus=0x%02x\n", m_ftStatus);
				CHIP_DESELECT();
				return 0;
			}
			if (ucByte != pucBuffer[j]) {
				printf("write: SPI_Read failed! ucByte(0x%02x) != pucBuffer[j](0x%02x)\n", ucByte, pucBuffer[j]);
				return false;
			}
		}

		CHIP_DESELECT();
		return true;
	}

	return true;
}

// Reads an array of bytes starting from a specific location in a page.
//  Takes four arguments
//    1. _addr --> Any address from 0 to capacity
//    2. data_buffer --> The array of bytes to be read from the flash memory - starting at the address indicated
//    3. bufferSize --> The size of the buffer - in number of bytes.
//    4. fastRead --> defaults to false - executes _beginFastRead() if set to true
bool SPIFlash::read(uint32 ulAddr, uint8 *pucBuffer, uint32 ulBufferSize, bool bFastRead)
{
	if (IS_POWERDOWN() || !IS_NOTBUSY()) {
		return false;
	}

	// Write FASTREAD/READDATA ID
	m_ftStatus = WRITE_INSTRUCTION(bFastRead ? FASTREAD : READDATA, 0, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);
	if (!m_ftStatus) {
		printf("write: SPI_Write failed!\n");
		CHIP_DESELECT();
		return false;
	}

	uint32 ulTotal = 0;
	do {
		uint32 ulTransferred = 0;
		m_ftStatus = SPI_Read(m_ftHandle, pucBuffer+ulTotal, ulBufferSize-ulTotal, &ulTransferred, 0);
		if (!m_ftStatus) {
			printf("write: SPI_Read failed! m_ftStatus=0x%02x\n", m_ftStatus);
			CHIP_DESELECT();
			return 0;
		}
		ulTotal += ulTransferred;
	} while (ulTotal < ulBufferSize);

	CHIP_DESELECT();
	return true;
}





void SPIFlash::CHIP_SELECT(void) {
	m_ftStatus = SPI_ToggleCS(m_ftHandle, TRUE);
	if (!m_ftStatus) {
		printf("CHIP_SELECT: SPI_ToggleCS failed!\n");
	}
}

void SPIFlash::CHIP_DESELECT(void) {
	m_ftStatus = SPI_ToggleCS(m_ftHandle, FALSE);
	if (!m_ftStatus) {
		printf("CHIP_DESELECT: SPI_ToggleCS failed!\n");
	}
}

FT_STATUS SPIFlash::WRITE_INSTRUCTION(uint8 ucCommand, uint32 ulDummyCount, uint32 ulOptions) {
	uint32 ulTransferred = 0;


	m_ftStatus = SPI_Write(m_ftHandle, &ucCommand, 1, &ulTransferred, ulOptions);
	if (!m_ftStatus) {
		printf("WRITE_INSTRUCTION: SPI_Write failed!\n");
		return 0;
	}

	if (ulDummyCount) {
		m_ftStatus = SPI_Write(m_ftHandle, 0, ulDummyCount, &ulTransferred, 0);
		if (!m_ftStatus) {
			printf("WRITE_INSTRUCTION: SPI_Write failed!\n");
			return 0;
		}
	}

	return m_ftStatus;
}

uint8 SPIFlash::READ_STAT(uint8 ucStatMode) {

	uint8 ucResult = 0;
	uint32 ulTransferred = 0;

	// Write the POWERDOWN ID
	m_ftStatus = WRITE_INSTRUCTION(ucStatMode, 0, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);
	if (!m_ftStatus) {
		printf("READ_STAT: SPI_Write failed!\n");
		CHIP_DESELECT();
		return 0;
	}

	// Read Stat
	m_ftStatus = SPI_Read(m_ftHandle, &ucResult, 1, &ulTransferred, SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (!m_ftStatus) {
		printf("READ_STAT: SPI_Read failed!\n");
		CHIP_DESELECT();
		return 0;
	}

	return ucResult;
}

bool SPIFlash::IS_POWERDOWN(void) {
	return bChipPowerDown;
}

bool SPIFlash::IS_NOTBUSY(void) {
	do {
		uint8 ucStat = READ_STAT(READSTAT1);
		if (!(ucStat & BUSY)) {
			break;
		}
		printf("IS_NOTBUSY: Is busy!\n");
		Sleep(1);
	} while (true);
	return true;
}

bool SPIFlash::IS_NOSUSPEND(void) {
	uint8 ucStat = READ_STAT(READSTAT2);
	if (ucStat & SUS) {
		return false;
	}
	return true;
}
