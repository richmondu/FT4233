#include <windows.h>
#include <setupapi.h>
#include <stdio.h>
#include <initguid.h>
#include <usbiodef.h>
#include "../../TEST SUITE/TestLogger.h"
#pragma comment(lib, "setupapi.lib")



static void print_property
(
	__in HDEVINFO        hDevInfo,
	__in SP_DEVINFO_DATA DeviceInfoData,
	__in PCWSTR          Label,
	__in DWORD           Property
)
{
	DWORD  DataT;
	LPTSTR buffer = NULL;
	DWORD  buffersize = 0;

	while (!SetupDiGetDeviceRegistryProperty(
		hDevInfo, &DeviceInfoData, Property, &DataT, (PBYTE)buffer, buffersize, &buffersize)) {
		if (ERROR_INSUFFICIENT_BUFFER == GetLastError()) {
			if (buffer) {
				LocalFree(buffer);
			}
			buffer = (LPTSTR)LocalAlloc(LPTR, buffersize * 2);
		}
		else {
			break;
		}
	}
	if (buffer) {
		FILE_LOG("  %S\n", buffer);
		LocalFree(buffer);
	}
}

static int enumerateGuid(LPGUID guid)
{
	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA DeviceInfoData;
	DWORD i;

	hDevInfo = SetupDiGetClassDevs(guid, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (INVALID_HANDLE_VALUE == hDevInfo) {
		return 1;
	}

	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++) {
		LPTSTR buffer = NULL;
		DWORD  buffersize = 0;
		while (!SetupDiGetDeviceInstanceId(
			hDevInfo, &DeviceInfoData, buffer, buffersize, &buffersize)) {
			if (buffer) {
				LocalFree(buffer);
			}
			if (ERROR_INSUFFICIENT_BUFFER == GetLastError()) {
				buffer = (LPTSTR)LocalAlloc(LPTR, buffersize * 2);
			}
			else {
				FILE_LOG("error: could not get device instance id (0x%x)\n", GetLastError());
				break;
			}
		}
		if (buffer) {
			print_property(hDevInfo, DeviceInfoData, L"", SPDRP_DEVICEDESC);
			print_property(hDevInfo, DeviceInfoData, L"", SPDRP_HARDWAREID);
			print_property(hDevInfo, DeviceInfoData, L"", SPDRP_MFG);
			print_property(hDevInfo, DeviceInfoData, L"", SPDRP_LOCATION_INFORMATION);

			FILE_LOG("\n");
		}
	}
	if (NO_ERROR != GetLastError() && ERROR_NO_MORE_ITEMS != GetLastError()) {
		return 1;
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);
	return 0;
}

void DisplayEnumeratedDevices()
{
	FILE_LOG("USB Host Controllers:\n");
	FILE_LOG("\n");
	enumerateGuid((LPGUID)&GUID_DEVINTERFACE_USB_HOST_CONTROLLER);
	FILE_LOG("USB Hubs:\n");
	FILE_LOG("\n");
	enumerateGuid((LPGUID)&GUID_DEVINTERFACE_USB_HUB);
	FILE_LOG("USB Devices:\n");
	FILE_LOG("\n");
	enumerateGuid((LPGUID)&GUID_DEVINTERFACE_USB_DEVICE);
	FILE_LOG("\n");
}
