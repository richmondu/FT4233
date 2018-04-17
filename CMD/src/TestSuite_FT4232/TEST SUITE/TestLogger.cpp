#include "TestLogger.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <share.h>
#include <time.h>
#include <cstdarg>
#include <iostream>
using namespace std;



CLogger *g_pLogger = NULL;

CLogger::CLogger(const char* a_pcFileName, bool a_bAppend) :
	m_pLogFile(NULL)
{
#ifdef LOG_TO_FILE
	if (a_pcFileName) {
		char* pcFileExtension = strrchr((char*)a_pcFileName, '.');
		int iFileNameLen = (int)(pcFileExtension - a_pcFileName);

		memset(m_pcFileName, 0, sizeof(m_pcFileName));
		strncpy_s(m_pcFileName, a_pcFileName, iFileNameLen);
		time_t now;
		struct tm st;
		time(&now);
		localtime_s(&st, &now);
		sprintf_s(m_pcFileName, "%s_%d%02d%02d%02d%02d%02d%s",
			m_pcFileName,
			1900 + st.tm_year, 1 + st.tm_mon, st.tm_mday, st.tm_hour, st.tm_min, st.tm_sec,
			pcFileExtension);

		if (a_bAppend)
			m_pLogFile = _fsopen(m_pcFileName, "a+", _SH_DENYWR);
		else
			m_pLogFile = _fsopen(m_pcFileName, "w+", _SH_DENYWR);

		if (m_pLogFile) {
			fseek(m_pLogFile, 0, SEEK_END);
			long lSize = ftell(m_pLogFile);
		}
	}
#endif // LOG_TO_FILE
}

CLogger::~CLogger()
{
	if (m_pLogFile) {
		fclose(m_pLogFile);
		m_pLogFile = NULL;
	}
}

void CLogger::Log(bool a_bNoTime, bool a_bConsole, const char* a_pcFormat, ...)
{
	memset(m_aucLineBuffer, 0, sizeof(m_aucLineBuffer));

	va_list argptr;
	va_start(argptr, a_pcFormat);
	vsprintf_s(m_aucLineBuffer, a_pcFormat, argptr);
	va_end(argptr);

	// Display to console
	if (a_bConsole) {
		std::cout << m_aucLineBuffer << std::flush;
	}

#ifdef LOG_TO_FILE
	// Everything is logged in the log file
	if (m_pLogFile) {
		if (a_bNoTime) {
			fprintf(m_pLogFile, m_aucLineBuffer);
		}
		else {
			time_t now;
			struct tm st;
			time(&now);
			localtime_s(&st, &now);
			fprintf(m_pLogFile, "[%d-%02d-%02d %02d:%02d:%02d] %s",
				1900 + st.tm_year, 1 + st.tm_mon, st.tm_mday, st.tm_hour, st.tm_min, st.tm_sec,
				m_aucLineBuffer);
		}

		fflush(m_pLogFile);
	}
#endif // LOG_TO_FILE
}
