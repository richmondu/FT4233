#ifndef FTDI_IDS_H
#define FTDI_IDS_H



#define FT2232_DEVINFO_ID	0x04036010
#define FT4232_DEVINFO_ID	0x04036011
#define FT232_DEVINFO_ID	0x04036014

#define FT2232_DEVINFO_STR	"FT2232H Dual USB-UART/FIFO IC"
#define FT4232_DEVINFO_STR	"FT4232H Quad HS USB-UART/FIFO IC"
#define FT232_DEVINFO_STR	"FT232H Single HS USB-UART/FIFO IC"

const char* FT_GetVidPidString(unsigned int ulVidPid);
bool FT_FindVidPid(unsigned int ulVidPid);


#endif // FTDI_IDS_H

