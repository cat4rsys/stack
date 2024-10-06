#include <cstdio>

#define DO_LOG_DUMP(logFile, stk, line, file) {                                               \
	logDump(logFile, stk, line, __func__, file);                          \
}

void logInit(FILE * logFile);
void logClose(FILE * logFile);
void logDump(FILE * logFile, Stack * stk, int line, const char * func, const char * file); //
