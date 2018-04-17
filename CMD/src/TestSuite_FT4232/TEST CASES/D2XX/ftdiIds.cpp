#include "ftdiIds.h"
#include <string.h>



const char* FT_GetVidPidString(unsigned int ulVidPid) 
{
	switch (ulVidPid) {
	case FT2232_DEVINFO_ID:
		return FT2232_DEVINFO_STR;
	case FT4232_DEVINFO_ID:
		return FT4232_DEVINFO_STR;
	case FT232_DEVINFO_ID:
		return FT232_DEVINFO_STR;
	}
	return "Unknown";
}

bool FT_FindVidPid(unsigned int ulVidPid)
{
	const char* str = FT_GetVidPidString(ulVidPid);
	return (strcmp(str, "Unknown") != 0);
}
