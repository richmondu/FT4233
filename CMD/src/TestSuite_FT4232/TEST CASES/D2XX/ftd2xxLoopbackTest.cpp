#include "ftd2xx.h"
#include "ftdiIds.h"
#include "../../TEST SUITE/TestSuite.h"
#include "../../TEST SUITE/TestLogger.h"
#pragma comment(lib, "ftd2xx.lib")
#include <stdio.h>
#include <conio.h>



// Can modify these macros
#define DISABLE_DATA_COMPARE 1	// Temporarily disabled until setup is not available
#define DISPLAY_BUFFER 0		// Enable to display buffer

// Do not modify these macros
#define NUM_THREADS 2
#define THREAD_WRITER 0
#define THREAD_READER 1



static int getInput(DWORD dwNumDevices, const char* pcPromptMsg)
{
	CMD_LOG("%s\n", pcPromptMsg);

	int cInput = getc(stdin);
	int iDevIndex = -1;

	while (cInput != (int)'\n' || (iDevIndex < 0 || iDevIndex >= (int)dwNumDevices))
	{
		iDevIndex = cInput - '0';
		cInput = getc(stdin);
		if (cInput == '\n' && (iDevIndex < 0 || iDevIndex >= (int)dwNumDevices)) {
			CMD_LOG("%s\n", pcPromptMsg);
		}
	}

	return iDevIndex;
}

static bool getInputWithTimeout(int iTimeoutMs)
{
	HANDLE stdinHandle = GetStdHandle(STD_INPUT_HANDLE);
	switch (WaitForSingleObject(stdinHandle, iTimeoutMs)) {
		case(WAIT_OBJECT_0): {
			if (_kbhit()) {
				_getch();
				return true;
			}
			else {
				// clear events
				INPUT_RECORD r[512];
				DWORD read;
				ReadConsoleInput(stdinHandle, r, 512, &read);
				break;
			}
		}
		case(WAIT_TIMEOUT):
		case(WAIT_FAILED):
		case(WAIT_ABANDONED):
		default: {
			break;
		}
	}

	return false;
}
static bool openDevices(int iDevIndex1, int iDevIndex2, FT_HANDLE* pftHandle1, FT_HANDLE* pftHandle2)
{
	FT_STATUS ftStatus;

	ftStatus = FT_Open(iDevIndex1, pftHandle1);
	if (ftStatus != FT_OK) {
		CMD_LOG("FT_Open failed! Cannot open device by Index1=%d", iDevIndex1);
		return false;
	}

	if (iDevIndex2 != iDevIndex1) {
		ftStatus = FT_Open(iDevIndex2, pftHandle2);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_Open failed! Cannot open device by Index2=%d", iDevIndex2);
			return false;
		}
	}
	else {
		*pftHandle2 = *pftHandle1;
	}

	return true;
}

static void closeDevices(FT_HANDLE ftHandle1, FT_HANDLE ftHandle2)
{
	FT_STATUS ftStatus;

	if (ftHandle1 == ftHandle2) {
		ftStatus = FT_Close(ftHandle1);
	}
	else {
		ftStatus = FT_Close(ftHandle1);
		ftStatus = FT_Close(ftHandle2);
	}
}

static bool displayDevices(DWORD dwNumDevices)
{
	FT_STATUS ftStatus;
	int iFound = 0;


	for (DWORD i = 0; i<dwNumDevices; ++i) {
		FT_DEVICE_LIST_INFO_NODE devInfo;
		memset(&devInfo, 0, sizeof(devInfo));

		ftStatus = FT_GetDeviceInfoDetail(i, &devInfo.Flags, &devInfo.Type,
			&devInfo.ID, &devInfo.LocId,
			devInfo.SerialNumber,
			devInfo.Description,
			&devInfo.ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_GetDeviceInfoDetail failed!\n");
			return false;
		}

		CMD_LOG("Dev %d:\n", i);
		CMD_LOG("  ID=0x%08x (%s)\n", devInfo.ID, FT_GetVidPidString(devInfo.ID));
		CMD_LOG("  LocId=0x%08x\n", devInfo.LocId);
		CMD_LOG("  Type=0x%08x\n", devInfo.Type);
		CMD_LOG("  Flags=0x%08x\n", devInfo.Flags);
		CMD_LOG("  SerialNumber=%s\n", devInfo.SerialNumber);
		CMD_LOG("  Description=%s\n", devInfo.Description);

		if (DeviceNameIsSetEx()) {
			if (DeviceNameCompareEx(devInfo.Description)) {
				iFound++;
				CMD_LOG("  Specified device is found!\n");
			}
		}
	}

	CMD_LOG("\n");
	return true;
}

typedef enum {
	LoopbackWrite = 0,
	LoopbackRead,
	LoopbackCompare,
} LoopbackStatus;

typedef struct _ThreadParams 
{
	FT_HANDLE ftHandle;
	HANDLE* phMutex;
	bool* pbStop;
	bool* pbReadError;
	bool* pbWriteError;
	LoopbackStatus* pbStatus;
	int iBufSize;
	char* pcBuffer;

} ThreadParams;

DWORD WINAPI ThreadWriter(LPVOID lpParam) 
{
	ThreadParams* pThread = (ThreadParams*)lpParam;
	FT_STATUS ftStatus;
	DWORD dwCount = 0;
	DWORD dwWaitResult = 0;
	DWORD dwIteration = 0;
	CMD_LOG("ThreadWriter started..\n");


	ftStatus = FT_SetTimeouts(pThread->ftHandle, 0, 1000);
	if (ftStatus != FT_OK) {
		CMD_LOG("ThreadWriter FT_SetTimeouts failed!\n");
		return 0;
	}

	while (!(*pThread->pbStop)) {
		dwWaitResult = WaitForSingleObject(*pThread->phMutex, 1000);
		if (*pThread->pbStop) {
			CMD_LOG("ThreadWriter aborted!\n");
			return 0;
		}
		switch (dwWaitResult) {
			case WAIT_OBJECT_0: {
				CMD_LOG("ThreadWriter mutex get... [%d]\n", dwIteration++);
				try {
					if (*pThread->pbStatus != LoopbackWrite) {
						CMD_LOG("ThreadWriter waiting for turn! %d\n", *pThread->pbStatus);
						CMD_LOG("ThreadWriter mutex released...\n");
						ReleaseMutex(*pThread->phMutex);
						Sleep(1000);
						break;
					}

					DWORD dwBytesTransferred = 0;

					ftStatus = FT_Write(pThread->ftHandle, pThread->pcBuffer, pThread->iBufSize, &dwBytesTransferred);
					if (ftStatus != FT_OK) {
						CMD_LOG("ThreadWriter FT_Write failed! ftStatus = 0x%08x\n", ftStatus);
						*pThread->pbWriteError = true;
						ReleaseMutex(*pThread->phMutex);
						goto exit;
					}
					else {
						CMD_LOG("ThreadWriter FT_Write %d bytes\n", dwBytesTransferred);
#if DISABLE_DATA_COMPARE
						Sleep(250);
#else // DISABLE_DATA_COMPARE
						if (dwBytesTransferred != pThread->iBufSize) {
							CMD_LOG("ThreadWriter FT_Write failed! dwBytesTransferred(%d) != pThread->iBufSize(%d)\n", dwBytesTransferred, pThread->iBufSize);
						}
#endif // DISABLE_DATA_COMPARE

						*pThread->pbStatus = LoopbackRead;
					}
				}
				catch (...) {
					CMD_LOG("ThreadWriter mutex released...\n");
					ReleaseMutex(*pThread->phMutex);
					goto exit;
				}
				CMD_LOG("ThreadWriter mutex released...\n");
				ReleaseMutex(*pThread->phMutex);
				break;
			}
			case WAIT_ABANDONED: {
				CMD_LOG("ThreadWriter aborted!\n");
				return 0;
			}
		}
	}

exit:
	CMD_LOG("ThreadWriter completed!\n");
	return 0;
}

DWORD WINAPI ThreadReader(LPVOID lpParam)
{
	ThreadParams* pThread = (ThreadParams*)lpParam;
	FT_STATUS ftStatus;
	DWORD dwCount = 0;
	DWORD dwWaitResult = 0;
	DWORD dwIteration = 0;
	DWORD dwTotalBytes = 0;
	CMD_LOG("ThreadReader started...\n");


	char* pcBuffer = (char*)malloc(pThread->iBufSize);
	if (!pcBuffer) {
		CMD_LOG("ThreadReader malloc failed!\n");
		return 0;
	}

	ftStatus = FT_SetTimeouts(pThread->ftHandle, 1000, 0);
	if (ftStatus != FT_OK) {
		CMD_LOG("ThreadReader FT_SetTimeouts failed!\n");
		goto exit;
	}

	while (!(*pThread->pbStop)) {
		dwWaitResult = WaitForSingleObject(*pThread->phMutex, 1000);
		if (*pThread->pbStop) {
			CMD_LOG("ThreadReader aborted!\n");
			goto exit;
		}
		switch (dwWaitResult) {
			case WAIT_OBJECT_0: {
				CMD_LOG("ThreadReader mutex get... [%d]\n", dwIteration++);
				try {
					if (*pThread->pbStatus != LoopbackRead) {
						CMD_LOG("ThreadReader waiting for turn! %d\n", *pThread->pbStatus);
						CMD_LOG("ThreadReader mutex released...\n");
						ReleaseMutex(*pThread->phMutex);
						Sleep(1000);
						break;
					}

					DWORD dwBytesTransferred = 0;
					ftStatus = FT_GetQueueStatus(pThread->ftHandle, &dwBytesTransferred);
					if (ftStatus != FT_OK) {
						CMD_LOG("ThreadReader FT_GetQueueStatus failed! ftStatus = 0x%08x\n", ftStatus);
						*pThread->pbReadError = true;
						ReleaseMutex(*pThread->phMutex);
						goto exit;
					}
					CMD_LOG("ThreadReader FT_GetQueueStatus %d available bytes\n", dwBytesTransferred);
					if (!dwBytesTransferred) {
						ReleaseMutex(*pThread->phMutex);
						Sleep(500);
						continue;
					}

					DWORD dwBytesToRead = pThread->iBufSize - dwTotalBytes;
					if (dwBytesToRead > dwBytesTransferred) {
						dwBytesToRead = dwBytesTransferred;
					}

					ftStatus = FT_Read(pThread->ftHandle, pcBuffer + dwTotalBytes, dwBytesToRead, &dwBytesTransferred);
					if (ftStatus != FT_OK) {
						CMD_LOG("ThreadReader FT_Read failed! ftStatus = 0x%08x\n", ftStatus);
						*pThread->pbReadError = true;
						ReleaseMutex(*pThread->phMutex);
						goto exit;
					}
					else {
						CMD_LOG("ThreadReader FT_Read %d bytes %d total bytes\n", dwBytesTransferred, dwTotalBytes);
						dwTotalBytes += dwBytesTransferred;
						if ((int)dwTotalBytes < pThread->iBufSize) {
							continue;
						}

						*pThread->pbStatus = LoopbackCompare;

#if DISABLE_DATA_COMPARE
						Sleep(250);
#else // DISABLE_DATA_COMPARE
						if (dwBytesTransferred != pThread->iBufSize) {
							CMD_LOG("ThreadReader FT_Read failed! dwBytesTransferred(%d) != pThread->iBufSize(%d)\n", dwBytesTransferred, pThread->iBufSize);
						}

						if (memcmp(pcBuffer, pThread->pcBuffer, pThread->iBufSize) != 0) {
							CMD_LOG("ThreadReader memcmp failed! Data mismatch!\n", dwBytesTransferred, pThread->iBufSize);
							*pThread->pbReadError = true;
							ReleaseMutex(*pThread->phMutex);
							goto exit;
						}
#endif // DISABLE_DATA_COMPARE

						*pThread->pbStatus = LoopbackWrite;
					}
				}
				catch (...) {
					CMD_LOG("ThreadReader mutex released...\n");
					ReleaseMutex(*pThread->phMutex);
					goto exit;
				}
				CMD_LOG("ThreadReader mutex released...\n");
				ReleaseMutex(*pThread->phMutex);
				break;
			}
			case WAIT_ABANDONED: {
				CMD_LOG("ThreadReader aborted!\n");
				goto exit;
			}
		}
	}

exit:
	if (pcBuffer) {
		free(pcBuffer);
		pcBuffer = NULL;
	}

	CMD_LOG("ThreadReader completed!\n");
	return 0;
}

static char* constructBuffer(int size)
{
	char* buf = (char*)malloc(size);
	if (!buf) {
		return NULL;
	}
	memset(buf, 0, size);

	int counter = 64; // counter every 64 bytes
	for (int i=0, j=0; i < size; i++) {
		buf[i] = (i % 256);
		if ((i % counter) == 0) {
			if (i + 3 <= size) {
				*((int*)(buf + i)) = j;
				j++;
				i += 3;
			}
		}
	}
	CMD_LOG("LoopbackSize=%d\n", size);

#if DISPLAY_BUFFER
	for (int i = 0; i < size; i++) {
		CMD_LOG("%02x ", (unsigned char)buf[i]);
		if ((i % 16) == 15) {
			CMD_LOG("\n");
		}
	}
#endif // DISPLAY_BUFFER

	return buf;
}

static void releaseBuffer(char* buf)
{
	if (buf) {
		free(buf);
		buf = NULL;
	}
}

static bool DoLoopback(int iDevIndex, int iDevIndex2)
{
	DWORD dwWaitResult = 0;
	bool bRet = false;
	HANDLE hThreads[NUM_THREADS] = { NULL };
	LPTHREAD_START_ROUTINE fxnThreads[NUM_THREADS] = { ThreadWriter, ThreadReader };
	ThreadParams hParams[NUM_THREADS] = { 0 };

	FT_HANDLE ftHandle[NUM_THREADS] = { NULL, NULL };
	HANDLE hMutex = NULL;
	bool bStop = false;
	LoopbackStatus bStatus = LoopbackWrite;
	bool bWriteError = false;
	bool bReadError = false;
	int iBufSize = TEST_CONFIG_TEST_DEFAULT_LOOPBACK_SIZE;
	char* pcBuffer = NULL;


	bRet = openDevices(iDevIndex, iDevIndex2, &ftHandle[THREAD_WRITER], &ftHandle[THREAD_READER]);
	if (!bRet) {
		CMD_LOG("Cannot open the loopback devices!\n");
		return false;
	}

	hMutex = CreateMutex(NULL, FALSE, NULL);
	if (!hMutex) {
		CMD_LOG("Cannot create mutex for the loopback devices!\n");
		goto exit;
	}

	pcBuffer = constructBuffer(iBufSize);
	if (!pcBuffer) {
		CMD_LOG("Cannot create buffer for the loopback devices!\n");
		goto exit;
	}

	for (int i = 0; i < NUM_THREADS; i++) {
		hParams[i].ftHandle = ftHandle[i];
		hParams[i].phMutex = &hMutex;
		hParams[i].pbStop = &bStop;
		hParams[i].pbStatus = &bStatus;
		hParams[i].pbWriteError = &bWriteError;
		hParams[i].pbReadError = &bReadError;
		hParams[i].iBufSize = iBufSize;
		hParams[i].pcBuffer = pcBuffer;
		hThreads[i] = CreateThread(NULL, 0, fxnThreads[i], &hParams[i], 0, NULL);
		if (hThreads[i] == NULL) {
			CMD_LOG("Cannot create thread[%d]!\n", i);
			goto exit;
		}
		Sleep(1);
	}

	CMD_LOG("  Press any key to stop the threads.\n");
	while (true) {
		dwWaitResult = WaitForMultipleObjects(NUM_THREADS, hThreads, TRUE, 1000);
		switch (dwWaitResult) {
			case WAIT_TIMEOUT: {
				if (!bWriteError && !bReadError) {
					CMD_LOG("  Press any key to stop the threads.\n");
				}
				break;
			}
			case WAIT_OBJECT_0: {
				CMD_LOG("  The threads have aborted!\n");
				goto exit;
			}
			case WAIT_ABANDONED: {
				CMD_LOG("WaitForMultipleObjects WAIT_ABANDONED\n");
				goto exit;
			}
		}
		if (bWriteError) {
			CMD_LOG("  Writer thread failed. Sending abort signal now!\n");
			bStop = true;
		}
		if (bReadError) {
			CMD_LOG("  Reader thread failed. Sending abort signal now!\n");
			bStop = true;
		}
		if (getInputWithTimeout(500)) {
			CMD_LOG("  A key was pressed. Sending abort signal now!\n");
			bStop = true;
		}
	}

exit:

	if (hMutex != NULL) {
		CloseHandle(hMutex);
		hMutex = NULL;
	}

	for (int i = 0; i<NUM_THREADS; i++) {
		if (hThreads[i] != NULL) {
			CloseHandle(hThreads[i]);
			hThreads[i] = NULL;
		}
	}

	closeDevices(ftHandle[THREAD_WRITER], ftHandle[THREAD_READER]);

	releaseBuffer(pcBuffer);

	return (!bWriteError && !bReadError);
}

static int getDeviceIndex(DWORD dwNumDevices, std::string szDeviceName)
{
	FT_STATUS ftStatus;

	for (DWORD i = 0; i < dwNumDevices; ++i) {
		FT_DEVICE_LIST_INFO_NODE devInfo;
		memset(&devInfo, 0, sizeof(devInfo));

		ftStatus = FT_GetDeviceInfoDetail(i, &devInfo.Flags, &devInfo.Type,
			&devInfo.ID, &devInfo.LocId,
			devInfo.SerialNumber,
			devInfo.Description,
			&devInfo.ftHandle);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_GetDeviceInfoDetail failed!\n");
			return -1;
		}

		std::string szDescription = devInfo.Description;
		std::transform(szDescription.begin(), szDescription.end(), szDescription.begin(), ::tolower);
		if (szDeviceName == szDescription) {
			return i;
		}
	}

	return -1;
}

bool FT4232_LOOPBACK_2Threads2Devices() {

	bool bRet = false;
	FT_STATUS ftStatus = 0;
	DWORD dwNumDevices = 0;

	do {
		ftStatus = FT_ListDevices(&dwNumDevices, NULL, FT_LIST_NUMBER_ONLY);
		if (ftStatus != FT_OK) {
			CMD_LOG("FT_ListDevices failed after cycle port!\n");
			return false;
		}
		if (dwNumDevices == 0) {
			CMD_LOG("No devices detected!\n", dwNumDevices);
			CMD_LOG("Please connect the 2 loopback devices and press any key\n");
			system("pause");
			CMD_LOG("\n");
		}
	} while (dwNumDevices == 0);

	ftStatus = FT_CreateDeviceInfoList(&dwNumDevices);
	if (ftStatus != FT_OK) {
		CMD_LOG("FT_CreateDeviceInfoList failed! No devices found!\n");
		return false;
	}

	CMD_LOG("DeviceCount=%d\n", dwNumDevices);
	
	bRet = displayDevices(dwNumDevices);
	if (!bRet) {
		CMD_LOG("Cannot enumerate the devices connected!\n");
		return false;
	}

	int iDevIndex = -1;
	int iDevIndex2 = -1;

	if (DeviceNameIsSetEx()) {
		CMD_LOG("Specified devices:\n");
		CMD_LOG("  %s\n", TEST_CONFIG_DEVICE_NAME.c_str());
		CMD_LOG("  %s\n", TEST_CONFIG_DEVICE_NAME_2.c_str());
		CMD_LOG("\n");

		iDevIndex = getDeviceIndex(dwNumDevices, TEST_CONFIG_DEVICE_NAME);
		iDevIndex2 = getDeviceIndex(dwNumDevices, TEST_CONFIG_DEVICE_NAME_2);
		if (iDevIndex == -1 || iDevIndex2 == -1) {
			CMD_LOG("Warning: One or both of the specified devices are not found!\n");
			CMD_LOG("\n");
		}
		else if (iDevIndex == iDevIndex2) {
			CMD_LOG("Warning: Selected devices are the same!\n");
			CMD_LOG("\n");
		}
	}
	else {
		CMD_LOG("Warning: One or both devices are not specified in program arguments!\n");
		CMD_LOG("\n");
	}
	if (iDevIndex == -1 || iDevIndex2 == -1 || iDevIndex == iDevIndex2) {
		iDevIndex = getInput(dwNumDevices, "Please enter the index of the 1st loopback device: ");
		iDevIndex2 = getInput(dwNumDevices, "Please enter the index of the 2nd loopback device: ");
	}

	CMD_LOG("\n");
	CMD_LOG("Selected devices:\n");
	CMD_LOG("  1st loopback device=%d\n", iDevIndex);
	CMD_LOG("  2nd loopback device=%d\n", iDevIndex2);
	CMD_LOG("\n");

	return DoLoopback(iDevIndex, iDevIndex2);
}
