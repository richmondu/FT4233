#pragma once
#include <cstdio>



#define LOG_TO_FILE                 
#define CMD_LOG(format, ...)        do{ if(g_pLogger) g_pLogger->Log(false, true, format, ## __VA_ARGS__); }while(0)
#define FILE_LOG(format, ...)       do{ if(g_pLogger) g_pLogger->Log(false, false, format, ## __VA_ARGS__); }while(0)
#define CMD_LOG_NOTIME(format, ...) do{ if(g_pLogger) g_pLogger->Log(true, true, format, ## __VA_ARGS__); }while(0)

class CLogger;
extern CLogger *g_pLogger;
#define LOGGER_INIT(x,y) {g_pLogger = new CLogger((x),(y));}
#define LOGGER_FREE() {if (g_pLogger) {delete g_pLogger; g_pLogger = NULL;}}
#define LOGGER_FILE() {g_pLogger->GetLogFile()}

class CLogger
{
public:
	CLogger(const char* a_pcFileName, bool a_bAppend);
	~CLogger();
	void Log(bool a_bNoTime, bool a_bConsole, const char* a_pcFormat, ...);
	const char* GetLogFile() { return m_pcFileName; }

private:
	char m_aucLineBuffer[1024];
	char m_pcFileName[128];
	FILE* m_pLogFile;
};
