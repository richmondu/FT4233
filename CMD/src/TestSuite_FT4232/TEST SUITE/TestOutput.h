#pragma once
#include "TestSuite.h"
#include <vector>
using namespace std;

void DisplayTestStart(const char* a_pcDescription);
void DisplayTestStop(bool a_bStatus);
void DisplayTestSummary(std::vector<std::string> a_szTestCases, std::vector<bool> a_bTestResults);
void DisplayTestLogFile(const char* a_pcFileName);
void DisplayTestHostInfo();
