#include "TestLogger.h"
#include "TestSuite.h"
#include <time.h>
#include <string.h>
#include <vector>
#include <windows.h>
#include <VersionHelpers.h>
using namespace std;



static time_t g_begin;

static void DisplayTime(const char* label , struct tm* stTime)
{
	if (stTime) {
		CMD_LOG("%s,%d-%02d-%02d %02d:%02d:%02d\n",
			label ? label : "",
			1900 + stTime->tm_year, 1 + stTime->tm_mon,
			stTime->tm_mday, stTime->tm_hour,
			stTime->tm_min, stTime->tm_sec);
	}
}

void DisplayTestStart(const char* a_pcDescription)
{
	struct tm stStartTime;
	time(&g_begin);
	localtime_s(&stStartTime, &g_begin);
	CMD_LOG("------------------------------------------------------------------------\n");
	CMD_LOG("%s\n", a_pcDescription ? a_pcDescription : "");
	CMD_LOG("------------------------------------------------------------------------\n");
	DisplayTime("Start time", &stStartTime);
	CMD_LOG("\n");
}

void DisplayTestStop(bool a_bStatus)
{
	time_t now;
	struct tm stStopTime;
	time(&now);
	localtime_s(&stStopTime, &now);
	CMD_LOG("\n");
	DisplayTime("Stop time", &stStopTime);

	double ullDiffTimeSeconds = difftime(now, g_begin);
	if (ullDiffTimeSeconds < 60) {
		CMD_LOG("Duration,%d second(s)\n", 
			(int)ullDiffTimeSeconds
		);
	}
	else if (ullDiffTimeSeconds < 3600) {
		CMD_LOG("Duration,%d minute(s) and %d second(s)\n",
			((int)(ullDiffTimeSeconds) / 60),
			((int)(ullDiffTimeSeconds) % 60)
		);
	}
	else {
		CMD_LOG("Duration,%d hour(s) and %d second(s)\n",
			((int)(ullDiffTimeSeconds) / 3600),
			((int)(ullDiffTimeSeconds) % 3600)
		);
	}

	CMD_LOG("Status,%s\n", a_bStatus ? "SUCCESS" : "FAILED");
	CMD_LOG("\n");
	CMD_LOG("\n");
}

void DisplayTestSummary(std::vector<std::string> a_szTestCases, std::vector<bool> a_bTestResults)
{
	CMD_LOG("------------------------------------------------------------------------\n");
	CMD_LOG("Summary\n");
	CMD_LOG("------------------------------------------------------------------------\n");

	int iSuccessCount = 0;
	int iFailCount = 0;
	int idx = 0;
	for (auto & i : a_szTestCases) {
		CMD_LOG("  %s %s\n", a_bTestResults[idx] ? "SUCCESS" : "FAIL   ", i.c_str());
		if (a_bTestResults[idx]) {
			iSuccessCount++;
		}
		else {
			iFailCount++;
		}
		idx++;
	}

	CMD_LOG("Summary Log Counts,[ Fails (%d); Success (%d) ]\n",
		iFailCount, iSuccessCount);
	CMD_LOG("\n");
}

void DisplayTestLogFile(const char* a_pcFileName)
{
	if (a_pcFileName) {
		CMD_LOG("%s\n", a_pcFileName);
		CMD_LOG("\n");
		CMD_LOG("\n");
	}
}

void DisplayTestHostInfo()
{
#define BUF_SIZE 64
	char szComputerName[BUF_SIZE] = { 0 };
	char szUserName[BUF_SIZE] = { 0 };
	char szOSVersion[BUF_SIZE] = { 0 };
	DWORD dwSize = 0;
	BOOL bRet = 0;
	SYSTEMTIME stLocalTime = { 0 };
	OSVERSIONINFO osVersion = { 0 };


	// Get local time
	::GetLocalTime(&stLocalTime);

	// Get computer name
	dwSize = BUF_SIZE;
	bRet = ::GetComputerNameA(szComputerName, &dwSize);
	if (!bRet) {
		return;
	}
	if (dwSize >= BUF_SIZE) {
		return;
	}

	// Get user name
	dwSize = BUF_SIZE;
	bRet = ::GetUserNameA(szUserName, &dwSize);
	if (!bRet) {
		return;
	}
	if (dwSize >= BUF_SIZE) {
		return;
	}

	// Get OS version
	if (IsWindows10OrGreater()) {
		strcpy_s(szOSVersion, "Windows 10 or Server");
	}
	else if (IsWindows8Point1OrGreater()) {
		strcpy_s(szOSVersion, "Windows 8.1");
	}
	else if (IsWindows8OrGreater()) {
		strcpy_s(szOSVersion, "Windows 8");
	}
	else if (IsWindows7SP1OrGreater()) {
		strcpy_s(szOSVersion, "Windows 7 SP1");
	}
	else if (IsWindows7OrGreater()) {
		strcpy_s(szOSVersion, "Windows 7");
	}
	else if (IsWindowsVistaSP2OrGreater()) {
		strcpy_s(szOSVersion, "Windows Vista SP2");
	}
	else if (IsWindowsVistaSP1OrGreater()) {
		strcpy_s(szOSVersion, "Windows Vista SP1");
	}
	else if (IsWindowsVistaOrGreater()) {
		strcpy_s(szOSVersion, "Windows Vista");
	}
	else if (IsWindowsXPSP3OrGreater()) {
		strcpy_s(szOSVersion, "Windows XP SP3");
	}
	else if (IsWindowsXPSP2OrGreater()) {
		strcpy_s(szOSVersion, "Windows XP SP2");
	}
	else if (IsWindowsXPSP1OrGreater()) {
		strcpy_s(szOSVersion, "Windows XP SP1");
	}
	else if (IsWindowsXPOrGreater()) {
		strcpy_s(szOSVersion, "Windows XP");
	}
	else {
		strcpy_s(szOSVersion, "Windows 10");
	}

	CMD_LOG("------------------------------------------------------------------------\n");
	CMD_LOG("TEST SUITE,%s\n", "FT4233");
	CMD_LOG("REVISION DATE,%s %s\n", __DATE__, __TIME__);
	CMD_LOG("WORKSTATION,%s\n", szComputerName);
	CMD_LOG("OSVERSION,%s\n", szOSVersion);
	CMD_LOG("OPERATOR,%s\n", szUserName);
	CMD_LOG("DATE,%d-%02d-%02d\n", stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay);
	CMD_LOG("TIME,%02d:%02d:%02d\n", stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond);
	CMD_LOG("------------------------------------------------------------------------\n");

	CMD_LOG("\n");
	CMD_LOG("\n");
}