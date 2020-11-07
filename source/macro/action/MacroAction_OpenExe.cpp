
// by simon yeung, 01/11/2020
// all rights reserved

#include "MacroAction_OpenExe.h"
#include "win.h"
#include <memory>
#include <assert.h>

#define MAX_EXE_LEN	(1024)	// have a smaller max size for stack allocation

MacroAction_OpenExe::MacroAction_OpenExe(const char* exePath)
	: MacroAction()
{
	size_t len= strlen(exePath);
	size_t sz = len + 1;
	assert(sz < MAX_EXE_LEN);
	m_exePath= (char*)malloc(sz);
	memcpy(m_exePath, exePath, sz);
}

MacroAction_OpenExe::~MacroAction_OpenExe()
{
	free(m_exePath);
}

bool	MacroAction_OpenExe::start()
{
	char cmdLine[MAX_EXE_LEN];
	strcpy_s(cmdLine, sizeof(cmdLine), m_exePath);
	
	STARTUPINFOA startUpInfo;
	memset(&startUpInfo, 0, sizeof(startUpInfo));
	startUpInfo.cb= sizeof(startUpInfo);
	PROCESS_INFORMATION processInfo;
	memset(&processInfo, 0, sizeof(processInfo));
	if (CreateProcessA(nullptr, cmdLine, nullptr, nullptr, true, 0, nullptr, nullptr, &startUpInfo, &processInfo))
	{
		//WaitForSingleObject(processInfo.hProcess, INFINITE);
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
	}
	return true;
}
