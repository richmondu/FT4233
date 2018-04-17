#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include "TestLogger.h"
#include "TestOutput.h"



typedef bool(*TEST_FUNCTION)();

typedef struct _TEST_CASE
{
	TEST_FUNCTION	m_fxn;
	std::string		m_dsc;

} TEST_CASE;

extern std::string TEST_CONFIG_LOG_FILE_NAME;

extern std::vector<TEST_CASE> TEST_CONFIG_TEST_CASES;
extern std::vector<TEST_CASE> TEST_CONFIG_TEST_CASES_2;
extern std::vector<TEST_CASE> TEST_CONFIG_TEST_CASES_3;
extern std::vector<TEST_CASE> TEST_CONFIG_TEST_CASES_4;
extern std::vector<TEST_CASE> TEST_CONFIG_TEST_CASES_5;

extern bool TEST_CONFIG_STOPWHENFAIL;

extern bool TEST_CONFIG_STRESSTEST;

extern bool TEST_CONFIG_TERMINATE_ALL_TESTS;

extern std::string TEST_CONFIG_DEVICE_NAME;
extern std::string TEST_CONFIG_DEVICE_NAME_2;

extern int TEST_CONFIG_TEST_DEFAULT_LOOPBACK_SIZE;

// From TESTCASES.cpp
bool DeviceNameIsSet();
bool DeviceNameIsSetEx();
bool DeviceNameCompare(const char* pcDeviceName);
bool DeviceNameCompareEx(const char* pcDeviceName);
void DeviceNameNotFoundAbort();
