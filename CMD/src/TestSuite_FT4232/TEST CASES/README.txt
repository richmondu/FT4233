Instructions:

1. Do not update any of the files in TEST SUITE

2. Update macro values in TESTCASES.cpp to customize the tests covered

   #define SAMPLES_TESTS			0
   #define D2XX_TESTS				1
   #define MPSSE_I2C_TESTS			1
   #define MPSSE_I2C_EEPROM24LC024H_TESTS	0 // enable only if you have I2C EEPROM 24LC024H
   #define MPSSE_SPI_TESTS			1
   #define MPSSE_SPI_EEPROM93LC56B_TESTS	0 // enable only if you have SPI EEPROM 93LC56B

3. When adding new test cases, declare the new test functions in TESTCASES.cpp and TESTCASES.h.
   The new test functions can be defined in new files or existing files: 
   mpsseI2CTest.cpp, mpsseSPITest.cpp and ftd2xxTest.cpp


Notes:

1. Developed using free MS Visual Studio Community version vs_community__259855441.1521517987
   https://www.visualstudio.com/free-developer-offers/
2. This uses the updated MPSSE library with the following modifications:
   a. Use the latest D2XX driver CDM v2.12.28 WHQL Certified
   b. Fix location ID issue for FT4232H 
      (D2XX reports locID of 0 for all 4 channels causing MPSSE library to not detect Channel A and B as MPSSE capable)
3. This has been tested using UMFTPD2A (FT4232H) and C232HM-DDHSL-0 (FT232H)
   Please update ftdiIds.cpp and ftdiIds.h to support additional FTDI devices.
4. For I2C and SPI testing, EEPROM 24LC024H and EEPROM 93LC56B are needed.
   These are the EEPROMS used for MPSSE examples in the website.
   To enable these tests, enable the following in TESTCASES.cpp
   MPSSE_I2C_EEPROM24LC024H_TESTS
   MPSSE_SPI_EEPROM93LC56B_TESTS
5. Test logs can be found at /TestSuite_FT4232/ directory with the following file format:
   TestSuite_FT4232_2018MMDDHHmmSS.log
6. Summary of the tests executed is displayed at the end of the test.