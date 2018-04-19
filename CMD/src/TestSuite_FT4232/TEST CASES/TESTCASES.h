#pragma once

// From ftd2xxTest.cpp
bool FT4232_D2XX_Info();
bool FT4232_D2XX_Enumerate();
bool FT4232_D2XX_Open();
bool FT4232_D2XX_ResetPort();
bool FT4232_D2XX_CyclePort();
bool FT4232_D2XX_ResetDevice();
bool FT4232_D2XX_UART();
bool FT4232_D2XX_GPIO();
bool FT4232_D2XX_SPIUnmanaged();
bool FT4232_D2XX_SetSync245();
bool FT4232_D2XX_UnsetSync245();

// From mpsseI2CTest.cpp
bool FT4232_MPSSE_I2C_Enumerate();
bool FT4232_MPSSE_I2C_Open();
bool FT4232_MPSSE_I2C_Configure();
bool FT4232_MPSSE_I2C_IO_EEPROM24LC024H_BasicIO();
bool FT4232_MPSSE_I2C_IO_EEPROM24LC024H_ClockedIO();
bool FT4232_MPSSE_I2C_IO_EEPROM24LC024H_StressIO();
//bool FT4232_MPSSE_I2C_GPIO();

// From mpsseSPITest.cpp
bool FT4232_MPSSE_SPI_Enumerate();
bool FT4232_MPSSE_SPI_Open();
bool FT4232_MPSSE_SPI_Configure();
bool FT4232_MPSSE_SPI_IO_EEPROM93LC56B_BasicIO();
bool FT4232_MPSSE_SPI_IO_EEPROM93LC56B_ClockedIO();
bool FT4232_MPSSE_SPI_IO_EEPROM93LC56B_StressIO();
//bool FT4232_MPSSE_SPI_GPIO();

// From ftd2xxLoopbackTest.cpp
bool FT4232_LOOPBACK_2Threads2Devices();

// From ftd2xxJtagTest.cpp
bool FT4232_D2XX_JTAG_TISN74BCT8244A();

// From mpsseJtagTest.cpp
bool FT4232_MPSSE_JTAG_TISN74BCT8244A();
bool FT4232_MPSSE_JTAG_TISN74BCT8244A_2();

// From mpsseSPIFlashTest.cpp
bool FT4232_MPSSE_SPIFlash_W25Q128JV_Enumerate();
bool FT4232_MPSSE_SPIFlash_W25Q128JV_Open();
bool FT4232_MPSSE_SPIFlash_W25Q128JV_ReadIDs();
bool FT4232_MPSSE_SPIFlash_W25Q128JV_Power();
bool FT4232_MPSSE_SPIFlash_W25Q128JV_Suspend();
bool FT4232_MPSSE_SPIFlash_W25Q128JV_BasicIO();
bool FT4232_MPSSE_SPIFlash_W25Q128JV_SimpleIO();
bool FT4232_MPSSE_SPIFlash_W25Q128JV_ClockedIO();
bool FT4232_MPSSE_SPIFlash_W25Q128JV_Erase();
