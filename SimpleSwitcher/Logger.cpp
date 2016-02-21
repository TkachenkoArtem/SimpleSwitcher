#include "stdafx.h"

TLogLevel c_loglevel = LOG_LEVEL_0;

void SetLogLevel(TLogLevel logLevel)
{
	c_loglevel = logLevel;
}

TLogLevel GetLogLevel()
{
#ifdef _DEBUG
	return LOG_LEVEL_2;
#endif
	return c_loglevel;

}

