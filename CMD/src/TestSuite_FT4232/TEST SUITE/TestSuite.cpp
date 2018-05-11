#include "TestSuite.h"
#include "../Test Cases/USB/USB.h"



static void runTest(
	std::vector<TEST_CASE> bConfigtestCases, 
	bool bConfigFailThenStop,
	bool bConfigStressTest,
	std::string szConfigFileName)
{
	std::vector<std::string> szTestCases;
	std::vector<bool> bTestResults;


	LOGGER_INIT(szConfigFileName.c_str(), false);

	DisplayTestHostInfo();
	DisplayEnumeratedDevices();

	do {
		int idx = 0;
		for (auto & i : bConfigtestCases) {
			if (i.m_fxn == NULL) {
				break;
			}

			DisplayTestStart(i.m_dsc.c_str());
			szTestCases.push_back(i.m_dsc);
			bool bRet = i.m_fxn();
			bTestResults.push_back(bRet);

			DisplayTestStop(bRet);

			if (!bRet) {
				if (bConfigFailThenStop) {
					goto exit;
				}
			}

			if (TEST_CONFIG_TERMINATE_ALL_TESTS) {
				goto exit;
			}

			idx++;
		}
	} while (bConfigStressTest);


exit:

	DisplayTestSummary(szTestCases, bTestResults);
	DisplayTestLogFile(LOGGER_FILE());
	LOGGER_FREE();
}

static const std::vector<std::string> g_szUsageCommands = 
	{ "mpsse", "loopback", "fastserial", "jtag", "spiflash", "spieeprom", "i2ceeprom" };

static void showUsage(const char* pcFilename)
{
	printf("------------------------------------------------------------------------\n");
	printf("USAGE\n");
	printf("------------------------------------------------------------------------\n");

	const char* ptr = strrchr((char*)pcFilename, '\\');
	if (ptr) {
		ptr++;
	}
	else {
		ptr = pcFilename;
	}

	printf("%s -m <MODE> -l <FILENAME>\n", ptr);
	printf("\n");

	printf("Options:\n");
	printf("  -h --help    \tShow this screen\n");
	printf("  -m --mode    \t0=MPSSE 1=loopback 2=fastserial 3=JTAG\n");
	printf("               \t4=SPIflash 5=SPIeeprom 6=I2Ceeprom\n");
	printf("  -e --enum    \tEnumerate FTDI devices/channels/ports connected\n");
	printf("  -d --device  \tPerform test on specified device/channel/port only\n");
	printf("  -b --bufsize \tSet size of buffer for loopback test\n");
	printf("  -s --stress  \tPerform the test cases repeatedly\n");
	printf("  -f --failstop\tExit program when a test fails\n");
	printf("  -l --logfile \tCustomize file name of the log file\n");
	printf("\n");

	printf("Examples:\n");
	printf("  %s --enum\n", ptr);
	for (unsigned int j = 0; j < g_szUsageCommands.size(); j++) {
		if (j == 0) {
			printf("  %s --mode %d\n", ptr, j);
			printf("  %s --mode %d --failstop 0\n", ptr, j);
			printf("  %s --mode %d --stress 1\n", ptr, j);
			printf("  %s --mode %d --stress 1 --failstop 0\n", ptr, j);
		}
		else if (j == 1) {
			printf("  %s --mode %d -d \"UMFTPD2A A\" -d \"UMFTPD2A B\" --bufsize 1024\n", ptr, j);
			printf("  %s --mode %d --device \"UMFTPD2A A\" --device \"UMFTPD2A B\"\n", ptr, j);
		}
		else if (j == 2) {
			printf("  %s --mode %d --logfile TestSuite_FT4232.log\n", ptr, j);
		}
		else if (j >= 3 && j <= 6) {
			printf("  %s --mode %d --device \"UMFTPD2A A\"\n", ptr, j);
		}
	}
	for (unsigned int j = 0; j < g_szUsageCommands.size(); j++) {
		if (j == 0) {
			printf("  %s -m %s\n", ptr, g_szUsageCommands[j].c_str());
			printf("  %s -m %s -f 0\n", ptr, g_szUsageCommands[j].c_str());
			printf("  %s -m %s -s 1 \n", ptr, g_szUsageCommands[j].c_str());
			printf("  %s -m %s -s 1 -f 0\n", ptr, g_szUsageCommands[j].c_str());
		}
		else if (j == 1) {
			printf("  %s -m %s -d \"UMFTPD2A A\" -d \"UMFTPD2A B\" -b 1024\n", ptr, g_szUsageCommands[j].c_str());
			printf("  %s -m %s -d \"UMFTPD2A A\" -d \"UMFTPD2A B\"\n", ptr, g_szUsageCommands[j].c_str());
		}
		else if (j == 2) {
			printf("  %s -m %s -l TestSuite_FT4232.log\n", ptr, g_szUsageCommands[j].c_str());
		}
		else if (j >= 3 && j <= 6) {
			printf("  %s -m %s -d \"UMFTPD2A A\"\n", ptr, g_szUsageCommands[j].c_str());
		}
	}
	printf("\n");

	printf("\n");
}

int main(int argc, char* argv[])
{
	std::vector<TEST_CASE> testCases = TEST_CONFIG_TEST_CASES_4;// TEST_CONFIG_TEST_CASES_1;
	bool bFailThenStop = TEST_CONFIG_STOPWHENFAIL;
	bool bStressTest = TEST_CONFIG_STRESSTEST;
	std::string szConfigFileName = TEST_CONFIG_LOG_FILE_NAME;
	

	if (argc == 1) {
		showUsage(argv[0]);
		runTest(testCases, bFailThenStop, bStressTest, szConfigFileName);
		system("pause");
	}
	else {
		std::vector<std::string> args(argv + 1, argv + argc);
		for (std::vector<std::string>::iterator i = args.begin(); i != args.end(); ++i) {
			std::string option = *i;
			std::transform(option.begin(), option.end(), option.begin(), ::tolower);
			if (option == "-h" || option == "--help") {
				showUsage(argv[0]);
				goto exit;
			}
			else if (option == "-m" || option == "--mode") {
				std::string val = *++i;
				std::transform(val.begin(), val.end(), val.begin(), ::tolower);
				if (val == "0" || val == g_szUsageCommands[0]) {
					testCases = TEST_CONFIG_TEST_CASES;
				}
				else if (val == "1" || val == g_szUsageCommands[1]) {
					testCases = TEST_CONFIG_TEST_CASES_2;
				}
				else if (val == "2" || val == g_szUsageCommands[2]) {
					testCases = TEST_CONFIG_TEST_CASES_3;
				}
				else if (val == "3" || val == g_szUsageCommands[3]) {
					testCases = TEST_CONFIG_TEST_CASES_4;
				}
				else if (val == "4" || val == g_szUsageCommands[4]) {
					testCases = TEST_CONFIG_TEST_CASES_5;
				}
				else if (val == "5" || val == g_szUsageCommands[5]) {
					testCases = TEST_CONFIG_TEST_CASES_6;
				}
				else if (val == "6" || val == g_szUsageCommands[6]) {
					testCases = TEST_CONFIG_TEST_CASES_7;
				}
			}
			else if (option == "-f" || option == "--failstop") {
				std::string val = *++i;
				if (val == "0") {
					bFailThenStop = false;
				}
				else if (val == "1") {
					bFailThenStop = true;
				}
			}
			else if (option == "-s" || option == "--stress") {
				std::string val = *++i;
				if (val == "0") {
					bStressTest = false;
				}
				else if (val == "1") {
					bStressTest = true;
				}
			}
			else if (option == "-l" || option == "--logfile") {
				szConfigFileName = *++i;
			}
			else if (option == "-e" || option == "--enum") {
				testCases = TEST_CONFIG_TEST_CASES_1;
			}
			else if (option == "-d" || option == "--device") {
				std::string szDeviceName = *++i;
				std::transform(szDeviceName.begin(), szDeviceName.end(), szDeviceName.begin(), ::tolower);
				printf("Device name: [%s][%d]\n", szDeviceName.c_str(), szDeviceName.length());
				if (TEST_CONFIG_DEVICE_NAME == "") {
					TEST_CONFIG_DEVICE_NAME = szDeviceName;
				}
				else if (TEST_CONFIG_DEVICE_NAME_2 == "") {
					TEST_CONFIG_DEVICE_NAME_2 = szDeviceName;
				}
			}
			else if (option == "-b" || option == "--bufsize") {
				int lBufSize = 0;
				try {
					lBufSize = std::stoi(*++i);
					if (lBufSize > 0) {
						TEST_CONFIG_TEST_DEFAULT_LOOPBACK_SIZE = lBufSize;
					}
				}
				catch (...) {
				}
			}
			else {
				showUsage(argv[0]);
				goto exit;
			}
		}
		runTest(testCases, bFailThenStop, bStressTest, szConfigFileName);
	}

exit:
	return 0;
}
