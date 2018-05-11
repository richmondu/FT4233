#include "SPIFlash.h"



SPIFlash::SPIFlash(FT_HANDLE ftHandle) {
	m_ftHandle = ftHandle;
}

uint32 SPIFlash::getJEDECID(void) {
	uint32 ulTransferred = 0;
	uint32 ulResult = 0;
	uint8 ucResult = 0;


	// Write the JEDEC ID
	CHIP_DESELECT();
	m_ftStatus = WRITE_INSTRUCTION(JEDECID, 0, 0);
	if (!m_ftStatus) {
		CMD_LOG("getJEDECID: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
		CHIP_SELECT();
		return 0;
	}

	// Read Manufacturer ID
	m_ftStatus = SPI_Read(m_ftHandle, &ucResult, 1, &ulTransferred, 0);
	if (!m_ftStatus) {
		CMD_LOG("getJEDECID: SPI_Read failed! m_ftStatus=%08x\n", m_ftStatus);
		CHIP_SELECT();
		return 0;
	}
	ulResult = (ucResult & 0xFF);

	// Read Memory Type ID
	m_ftStatus = SPI_Read(m_ftHandle, &ucResult, 1, &ulTransferred, 0);
	if (!m_ftStatus) {
		CMD_LOG("getJEDECID: SPI_Read failed! m_ftStatus=%08x\n", m_ftStatus);
		CHIP_SELECT();
		return 0;
	}
	ulResult = (ulResult << 8) | (ucResult & 0xFF);

	// Read Capacity ID
	m_ftStatus = SPI_Read(m_ftHandle, &ucResult, 1, &ulTransferred, 0);
	if (!m_ftStatus) {
		CMD_LOG("getJEDECID: SPI_Read failed! m_ftStatus=%08x\n", m_ftStatus);
		CHIP_SELECT();
		return 0;
	}
	ulResult = (ulResult << 8) | (ucResult & 0xFF);

	CHIP_SELECT();
	return ulResult;
}

uint16 SPIFlash::getManID(void) {
	uint32 ulTransferred = 0;
	uint16 uwResult = 0;
	uint8 ucResult = 0;


	// Write the MAN ID
	CHIP_DESELECT();
	m_ftStatus = WRITE_INSTRUCTION(MANID, 1, 0);
	if (!m_ftStatus) {
		CMD_LOG("getManID: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
		CHIP_SELECT();
		return 0;
	}

	// Read Manufacturer ID
	m_ftStatus = SPI_Read(m_ftHandle, &ucResult, 1, &ulTransferred, 0);
	if (!m_ftStatus) {
		CMD_LOG("getManID: SPI_Read failed! m_ftStatus=%08x\n", m_ftStatus);
		CHIP_SELECT();
		return 0;
	}
	uwResult = (ucResult & 0xFF);

	// Read Device ID
	m_ftStatus = SPI_Read(m_ftHandle, &ucResult, 1, &ulTransferred, 0);
	if (!m_ftStatus) {
		CMD_LOG("getManID: SPI_Read failed! m_ftStatus=%08x\n", m_ftStatus);
		CHIP_SELECT();
		return 0;
	}
	uwResult = (uwResult << 8) | (ucResult & 0xFF);

	CHIP_SELECT();
	return uwResult;
}

uint64 SPIFlash::getUniqueID(void) {
	uint32 ulTransferred = 0;
	uint64 ullResult = 0;
	uint8 ucResult = 0;


	// Write the Unique ID
	CHIP_DESELECT();
	m_ftStatus = WRITE_INSTRUCTION(UNIQUEID, 4, 0);
	if (!m_ftStatus) {
		CMD_LOG("getUniqueID: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
		CHIP_SELECT();
		return 0;
	}

	// Read Unique ID
	m_ftStatus = SPI_Read(m_ftHandle, (uint8*)&ullResult, sizeof(ullResult), &ulTransferred, 0);
	if (!m_ftStatus) {
		CMD_LOG("getUniqueID: SPI_Read failed! m_ftStatus=%08x\n", m_ftStatus);
		CHIP_SELECT();
		return 0;
	}

	CHIP_SELECT();
	return ullResult;
}

//Wakes chip from low power state.
bool SPIFlash::powerUp(void) {

	// Write the RELEASE ID
	CHIP_DESELECT();
	m_ftStatus = WRITE_INSTRUCTION(RELEASE, 0, 0);
	if (!m_ftStatus) {
		CMD_LOG("powerUp: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
		CHIP_SELECT();
		return false;
	}

	Sleep(1);

	// Write the WRITEENABLE ID
	m_ftStatus = WRITE_INSTRUCTION(WRITEENABLE, 0, 0);
	if (!m_ftStatus) {
		CMD_LOG("powerUp: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
		CHIP_SELECT();
		return false;
	}
	if (!(READ_STAT(READSTAT1) & WRTEN)) {
		printf("powerUp: SPI_Write failed! WRTEN not set\n");
		CHIP_SELECT();
		return false;
	}

	// Write the WRITEENABLE ID
	m_ftStatus = WRITE_INSTRUCTION(WRITEDISABLE, 0, 0);
	if (!m_ftStatus) {
		CMD_LOG("powerUp: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
		CHIP_SELECT();
		return false;
	}

	bChipPowerDown = false;
	CHIP_SELECT();

	return true;
}

//Puts device in low power state. Good for battery powered operations.
//In powerDown() the chip will only respond to powerUp()
bool SPIFlash::powerDown(void) {

	// Write the POWERDOWN ID
	CHIP_DESELECT();
	m_ftStatus = WRITE_INSTRUCTION(POWERDOWN, 0, 0);
	if (!m_ftStatus) {
		CMD_LOG("powerDown: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
		CHIP_SELECT();
		return false;
	}

	bChipPowerDown = true;
	Sleep(1);

	// Write the WRITEENABLE ID
	m_ftStatus = WRITE_INSTRUCTION(WRITEENABLE, 0, 0);
	if (!m_ftStatus) {
		CMD_LOG("powerDown: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
		CHIP_SELECT();
		return false;
	}
	if (!(READ_STAT(READSTAT1) & WRTEN)) {
		CMD_LOG("powerDown: SPI_Write failed! WRTEN not set\n");
		CHIP_SELECT();
		return false;
	}

	CHIP_SELECT();
	return true;
}

//Suspends current Block Erase/Sector Erase/Page Program. Does not suspend chipErase().
//Page Program, Write Status Register, Erase instructions are not allowed.
//Erase suspend is only allowed during Block/Sector erase.
//Program suspend is only allowed during Page/Quad Page Program
bool SPIFlash::suspendProg(void) {

	if (IS_POWERDOWN() || IS_NOTBUSY()) {
		CMD_LOG("suspendProg: IS_POWERDOWN() || IS_NOTBUSY()!\n");
		return false;
	}
	if (!IS_NOSUSPEND()) {
		CMD_LOG("suspendProg: !IS_NOSUSPEND()!\n");
		return true;
	}

	// Write the SUSPEND ID
	CHIP_DESELECT();
	m_ftStatus = WRITE_INSTRUCTION(SUSPEND, 0, 0);
	if (!m_ftStatus) {
		CMD_LOG("suspendProg: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
		CHIP_SELECT();
		return false;
	}

	Sleep(1);

	if (!IS_NOTBUSY() || IS_NOSUSPEND()) {
		CMD_LOG("suspendProg: !IS_NOTBUSY() || IS_NOSUSPEND()!\n");
		CHIP_SELECT();
		return false;
	}

	CHIP_SELECT();
	return true;
}

//Resumes previously suspended Block Erase/Sector Erase/Page Program.
bool SPIFlash::resumeProg(void) {

	if (IS_POWERDOWN() || !IS_NOTBUSY() || IS_NOSUSPEND()) {
		CMD_LOG("resumeProg: IS_POWERDOWN() || !IS_NOTBUSY() || IS_NOSUSPEND()!\n");
		return false;
	}

	// Write the RESUME ID
	CHIP_DESELECT();
	m_ftStatus = WRITE_INSTRUCTION(RESUME, 0, 0);
	if (!m_ftStatus) {
		CMD_LOG("resumeProg: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
		CHIP_SELECT();
		return false;
	}

	Sleep(1);

	if (IS_NOTBUSY() || !IS_NOSUSPEND()) {
		CMD_LOG("resumeProg: IS_NOTBUSY() || !IS_NOSUSPEND()!\n");
		CHIP_SELECT();
		return false;
	}

	CHIP_SELECT();
	return true;
}

//Erases whole chip. Think twice before using.
bool SPIFlash::eraseChip(void) {

	if (IS_POWERDOWN() || !IS_NOTBUSY()) {
		return false;
	}

	// Write the WRITEENABLE ID
	CHIP_DESELECT();
	m_ftStatus = WRITE_INSTRUCTION(WRITEENABLE, 0, 0);
	if (!m_ftStatus) {
		CMD_LOG("eraseChip: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
		CHIP_SELECT();
		return false;
	}
	if (!(READ_STAT(READSTAT1) & WRTEN)) {
		CMD_LOG("eraseChip: SPI_Write failed! WRTEN not set\n");
		CHIP_SELECT();
		return false;
	}

	// Write the CHIPERASE ID
	m_ftStatus = WRITE_INSTRUCTION(CHIPERASE, 0, 0);
	if (!m_ftStatus) {
		CMD_LOG("eraseChip: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
		CHIP_SELECT();
		return false;
	}

	while (READ_STAT(READSTAT1) & BUSY) {
		CMD_LOG("eraseChip: Chip erase is ongoing...\n");
		Sleep(1);
	}

	CHIP_SELECT();
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
	CHIP_DESELECT();
	m_ftStatus = WRITE_INSTRUCTION(WRITEENABLE, 0, 0);
	if (!m_ftStatus) {
		CMD_LOG("eraseSector: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
		CHIP_SELECT();
		return false;
	}
	if (!(READ_STAT(READSTAT1) & WRTEN)) {
		CMD_LOG("eraseSector: SPI_Write failed! WRTEN not set\n");
		CHIP_SELECT();
		return false;
	}

	// Write the SECTORERASE ID
	m_ftStatus = WRITE_INSTRUCTION(SECTORERASE, 0, 0);
	if (!m_ftStatus) {
		CMD_LOG("eraseSector: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
		CHIP_SELECT();
		return false;
	}

	Sleep(1000);

	if (!IS_NOTBUSY()) {
		CMD_LOG("eraseSector: IS_NOTBUSY failed!\n");
		CHIP_SELECT();
		return false;
	}

	CHIP_SELECT();
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
	CHIP_DESELECT();
	m_ftStatus = WRITE_INSTRUCTION(WRITEENABLE, 0, 0);
	if (!m_ftStatus) {
		CMD_LOG("eraseBlock32K: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
		CHIP_SELECT();
		return false;
	}

	Sleep(1000);

	if (!IS_NOTBUSY()) {
		CMD_LOG("eraseBlock32K: IS_NOTBUSY failed!\n");
		CHIP_SELECT();
		return false;
	}

	// Write the BLOCK32ERASE ID
	m_ftStatus = WRITE_INSTRUCTION(BLOCK32ERASE, 0, 0);
	if (!m_ftStatus) {
		CMD_LOG("eraseBlock32K: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
		CHIP_SELECT();
		return false;
	}

	CHIP_SELECT();
	return true;
}

bool SPIFlash::eraseBlock64K() {

	if (IS_POWERDOWN() || !IS_NOTBUSY()) {
		return false;
	}

	//if (addr + 65536 >= _chip.capacity) {
	//	return false;
	//}

	CHIP_DESELECT();
	m_ftStatus = WRITE_INSTRUCTION(BLOCK64ERASE, 0, 0);
	if (!m_ftStatus) {
		CMD_LOG("eraseBlock64K: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
		CHIP_SELECT();
		return false;
	}

	Sleep(1200);

	if (!IS_NOTBUSY()) {
		CMD_LOG("eraseBlock64K: IS_NOTBUSY failed!\n");
		CHIP_SELECT();
		return false;
	}

	// Write the BLOCK64ERASE ID
	m_ftStatus = WRITE_INSTRUCTION(BLOCK64ERASE, 0, 0);
	if (!m_ftStatus) {
		CMD_LOG("eraseBlock64K: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
		CHIP_SELECT();
		return false;
	}

	CHIP_SELECT();
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
	CHIP_DESELECT();
	m_ftStatus = WRITE_INSTRUCTION(WRITEENABLE, 0, 0);
	if (!m_ftStatus) {
		CMD_LOG("write: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
		CHIP_SELECT();
		return false;
	}
	if (!(READ_STAT(READSTAT1) & WRTEN)) {
		CMD_LOG("write: SPI_Write failed! WRTEN not set\n");
		CHIP_SELECT();
		return false;
	}


	uint16_t maxBytes = SPI_PAGESIZE - (ulAddr % SPI_PAGESIZE);  // Force the first set of bytes to stay within the first page

	if (ulBufferSize <= maxBytes) {
		//CHIP_SELECT();

		// Write the PAGEPROG ID
		m_ftStatus = WRITE_INSTRUCTION(PAGEPROG, 0, 0);
		if (!m_ftStatus) {
			CMD_LOG("write: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
			CHIP_SELECT();
			return false;
		}

		// Write address
		uint32 ulTransferred = 0;
		m_ftStatus = SPI_Write(m_ftHandle, (uint8*)&ulAddr, 4, &ulTransferred, 0);
		if (!m_ftStatus) {
			CMD_LOG("write: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
			CHIP_SELECT();
			return 0;
		}

		for (uint16_t i = 0; i < ulBufferSize; ++i) {
			ulTransferred = 0;
			m_ftStatus = SPI_Write(m_ftHandle, &pucBuffer[i], 1, &ulTransferred, 0);
			if (!m_ftStatus) {
				CMD_LOG("write: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
				CHIP_SELECT();
				return 0;
			}
		}
		CHIP_SELECT();
	}
	else {
		uint32_t length = ulBufferSize;
		uint16_t writeBufSz;
		uint16_t data_offset = 0;
		uint32 ulTempAddr = ulAddr;

		do {
			writeBufSz = (length <= maxBytes) ? length : maxBytes;

			CHIP_DESELECT();

			// Write the PAGEPROG ID
			m_ftStatus = WRITE_INSTRUCTION(PAGEPROG, 0, 0);
			if (!m_ftStatus) {
				CMD_LOG("write: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
				CHIP_SELECT();
				return false;
			}

			// Write address
			uint32 ulTransferred = 0;
			m_ftStatus = SPI_Write(m_ftHandle, (uint8*)&ulTempAddr, 4, &ulTransferred, 0);
			if (!m_ftStatus) {
				CMD_LOG("write: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
				CHIP_SELECT();
				return 0;
			}

			for (uint16_t i = 0; i < writeBufSz; ++i) {
				ulTransferred = 0;
				m_ftStatus = SPI_Write(m_ftHandle, &pucBuffer[i], 1, &ulTransferred, 0);
				if (!m_ftStatus) {
					CMD_LOG("write: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
					CHIP_SELECT();
					return 0;
				}
			}
			//CHIP_SELECT();

			ulTempAddr += writeBufSz;
			data_offset += writeBufSz;
			length -= writeBufSz;
			maxBytes = 256;   // Now we can do up to 256 bytes per loop

			if (!IS_NOTBUSY()) {
				CHIP_SELECT();
				return false;
			}

			// Write the WRITEENABLE ID
			m_ftStatus = WRITE_INSTRUCTION(WRITEENABLE, 0, SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
			if (!m_ftStatus) {
				CMD_LOG("write: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
				CHIP_SELECT();
				return false;
			}
			if (!(READ_STAT(READSTAT1) & WRTEN)) {
				CMD_LOG("write: SPI_Write failed! WRTEN not set\n");
				CHIP_SELECT();
				return false;
			}
			CHIP_SELECT();

		} while (length > 0);
	}

	if (!bErrorCheck) {
		CHIP_SELECT();
		return true;
	}
	else {
		if (!IS_NOTBUSY()) {
			CHIP_SELECT();
			return false;
		}

		CHIP_DESELECT();

		// Write the READDATA ID
		m_ftStatus = WRITE_INSTRUCTION(READDATA, 0, 0);
		if (!m_ftStatus) {
			CMD_LOG("write: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
			CHIP_SELECT();
			return false;
		}

		// Write address
		uint32 ulTransferred = 0;
		m_ftStatus = SPI_Write(m_ftHandle, (uint8*)&ulAddr, 4, &ulTransferred, 0);
		if (!m_ftStatus) {
			CMD_LOG("write: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
			CHIP_SELECT();
			return 0;
		}

		for (uint16_t j = 0; j < ulBufferSize; j++) {
			uint8 ucByte = 0;
			ulTransferred = 0;
			m_ftStatus = SPI_Read(m_ftHandle, &ucByte, 1, &ulTransferred, 0);
			if (!m_ftStatus) {
				CMD_LOG("write: SPI_Read failed! m_ftStatus=%08x\n", m_ftStatus);
				CHIP_SELECT();
				return 0;
			}
			if (ucByte != pucBuffer[j]) {
				CMD_LOG("write: SPI_Read failed! ucByte(0x%02x) != pucBuffer[j](0x%02x)\n", ucByte, pucBuffer[j]);
				CHIP_SELECT();
				return false;
			}
		}

		CHIP_SELECT();
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
	CHIP_DESELECT();
	m_ftStatus = WRITE_INSTRUCTION(bFastRead ? FASTREAD : READDATA, 0, 0);
	if (!m_ftStatus) {
		CMD_LOG("read: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
		CHIP_SELECT();
		return false;
	}

	uint32 ulTotal = 0;
	do {
		uint32 ulTransferred = 0;
		m_ftStatus = SPI_Read(m_ftHandle, pucBuffer+ulTotal, ulBufferSize-ulTotal, &ulTransferred, 0);
		if (!m_ftStatus) {
			CMD_LOG("read: SPI_Read failed!  m_ftStatus=%08x\n", m_ftStatus);
			CHIP_SELECT();
			return 0;
		}
		ulTotal += ulTransferred;
	} while (ulTotal < ulBufferSize);
	CHIP_SELECT();

	
	return true;
}





void SPIFlash::CHIP_SELECT(void) {
	SPI_ToggleCS(m_ftHandle, TRUE);
}

void SPIFlash::CHIP_DESELECT(void) {
	SPI_ToggleCS(m_ftHandle, FALSE);
}

FT_STATUS SPIFlash::WRITE_INSTRUCTION(uint8 ucCommand, uint32 ulDummyCount, uint32 ulOptions) {
	uint32 ulTransferred = 0;


	m_ftStatus = SPI_Write(m_ftHandle, &ucCommand, 1, &ulTransferred, ulOptions);
	if (!m_ftStatus) {
		CMD_LOG("WRITE_INSTRUCTION: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
		return 0;
	}

	if (ulDummyCount) {
		m_ftStatus = SPI_Write(m_ftHandle, 0, ulDummyCount, &ulTransferred, 0);
		if (!m_ftStatus) {
			CMD_LOG("WRITE_INSTRUCTION: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
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
		CMD_LOG("READ_STAT: SPI_Write failed! m_ftStatus=%08x\n", m_ftStatus);
		return 0;
	}

	// Read Stat
	m_ftStatus = SPI_Read(m_ftHandle, &ucResult, 1, &ulTransferred, SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (!m_ftStatus) {
		CMD_LOG("READ_STAT: SPI_Read failed! m_ftStatus=%08x\n", m_ftStatus);
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
		CMD_LOG("IS_NOTBUSY: Is busy!\n");
		Sleep(1);
	} while (true);
	return true;
}

bool SPIFlash::IS_NOSUSPEND(void) {
	uint8 ucStat = READ_STAT(READSTAT2);
	if (ucStat & SUS) {
		CMD_LOG("IS_NOSUSPEND: Is suspended!\n");
		return false;
	}
	return true;
}
