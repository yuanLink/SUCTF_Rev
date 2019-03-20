#include"RevProtect.h"
#include<process.h>
// #include"ThreadHeader.h"

extern EventHandle::AEventContainer* container;
wchar_t checkSign[][33] = {
	L"438078d884693cdb2dbc12a84d381899",
	L"6de452781ffd3e77696e0564d27dbdfd",//windbg.exe
	L"611db7cd21044e969b0b28008d1ef565",//??
	L"2cffaa33fdbfe6dea9df8aabc71b9989",//ida64.exe
	L"c898eaf62c0cbcc089939366f516e09f",
	L"5303e7f2944c9081f864a2e11a8a0aef",
	L"679542632bddb4cf47ed0a61f1f83ee4"// dbgx_shell.exe(windbg_preview)
};

PIMAGE_NT_HEADERS GetImageNtHeaders(PBYTE pImageBase)
{
	PIMAGE_DOS_HEADER pImageDosHeader = (PIMAGE_DOS_HEADER)pImageBase;
	return (PIMAGE_NT_HEADERS)(pImageBase + pImageDosHeader->e_lfanew);
}
PIMAGE_SECTION_HEADER FindRDataSection(PBYTE pImageBase)
{
	static const std::string rdata = ".rdata";
	PIMAGE_NT_HEADERS pImageNtHeaders = GetImageNtHeaders(pImageBase);
	PIMAGE_SECTION_HEADER pImageSectionHeader = IMAGE_FIRST_SECTION(pImageNtHeaders);
	int n = 0;
	for (; n < pImageNtHeaders->FileHeader.NumberOfSections; ++n)
	{
		if (rdata == (char*)pImageSectionHeader[n].Name)
		{
			break;
		}
	}
	return &pImageSectionHeader[n];
}
void CheckGlobalFlagsClearInProcess()
{
	PBYTE pImageBase = (PBYTE)GetModuleHandle(NULL);
	PIMAGE_NT_HEADERS pImageNtHeaders = GetImageNtHeaders(pImageBase);
	PIMAGE_LOAD_CONFIG_DIRECTORY pImageLoadConfigDirectory = (PIMAGE_LOAD_CONFIG_DIRECTORY)(pImageBase
		+ pImageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].VirtualAddress);
	if (pImageLoadConfigDirectory->GlobalFlagsClear != 0)
	{
		MyDbgPrint("Stop debugging program\n");
		// exit(-1);
	}
	else {
		printf("pAAASS\n");
	}
}
void CheckGlobalFlagsClearInFile()
{
	HANDLE hExecutable = INVALID_HANDLE_VALUE;
	HANDLE hExecutableMapping = NULL;
	PBYTE pMappedImageBase = NULL;
	__try
	{
		PBYTE pImageBase = (PBYTE)GetModuleHandle(NULL);
		PIMAGE_SECTION_HEADER pImageSectionHeader = FindRDataSection(pImageBase);
		TCHAR pszExecutablePath[MAX_PATH];
		DWORD dwPathLength = GetModuleFileName(NULL, pszExecutablePath, MAX_PATH);
		if (0 == dwPathLength) __leave;
		hExecutable = CreateFile(pszExecutablePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (INVALID_HANDLE_VALUE == hExecutable) __leave;
		hExecutableMapping = CreateFileMapping(hExecutable, NULL, PAGE_READONLY, 0, 0, NULL);
		if (NULL == hExecutableMapping) __leave;
		pMappedImageBase = (PBYTE)MapViewOfFile(hExecutableMapping, FILE_MAP_READ, 0, 0,
			pImageSectionHeader->PointerToRawData + pImageSectionHeader->SizeOfRawData);
		if (NULL == pMappedImageBase) __leave;
		PIMAGE_NT_HEADERS pImageNtHeaders = GetImageNtHeaders(pMappedImageBase);
		PIMAGE_LOAD_CONFIG_DIRECTORY pImageLoadConfigDirectory = (PIMAGE_LOAD_CONFIG_DIRECTORY)(pMappedImageBase
			+ (pImageSectionHeader->PointerToRawData
				+ (pImageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].VirtualAddress - pImageSectionHeader->VirtualAddress)));
		if (pImageLoadConfigDirectory->GlobalFlagsClear != 0)
		{
			// std::cout << "Stop debugging program!" << std::endl;
			MyDbgPrint("CheckGlobalFlag find you!\n");
			exit(-1);
		}
	}
	__finally
	{
		if (NULL != pMappedImageBase)
			UnmapViewOfFile(pMappedImageBase);
		if (NULL != hExecutableMapping)
			CloseHandle(hExecutableMapping);
		if (INVALID_HANDLE_VALUE != hExecutable)
			CloseHandle(hExecutable);
	}
}

bool DecryptThirdPart() {
	return true;
}
bool Protector::ProtectorContext::InitProtector() {
	bool bRet = false;
	HMODULE hDll = GetModuleHandle(L"ntdll.dll");
	if (hDll == NULL) {
		printf("[ERROR] Get Handle failed!\n");
		return bRet;
	}
	// pfnNtCurrentTEB = (PFNNtCurrentTEB)GetProcAddress(hDll, "NtCurrentTeb");
	pfnNtQueryInformationProcess = (PFNNtQueryInformationProcess)GetProcAddress(hDll, "NtQueryInformationProcess");
	pfnZwQueryInformationThread = (PFNZwQueryInformationThread)GetProcAddress(hDll, "ZwQueryInformationThread");
	pfnNtQueueApcThread = (NTSTATUS(NTAPI *)(HANDLE, PVOID, PVOID, PVOID, ULONG)) GetProcAddress(hDll, "NtQueueApcThread");
	if (// !pfnNtCurrentTEB || 
		!pfnNtQueryInformationProcess ||
		!pfnZwQueryInformationThread ||
		!pfnNtQueueApcThread) {
		MyDbgPrint("Could not get all the function ptr!\n");
		return bRet;
	}
	bRet = true;
	return bRet;
}

bool Protector::ProtectorContext::ThreadProtector() {
	bool bRet = FALSE;
	return bRet;
}
// 2. check the process that has not hash number(like ida.exe ida64.exe so on)
// this will work when the thread is created
bool Protector::ProtectorContext::ProcessProtector() {
	bool bRet = false;
	static bool bEvent = false;
	EventHandle::AEvent procEvent(1, Protector::PROCESS_CHECK_PASS);
	PROCESSENTRY32 procEntry = { 0 };
	procEntry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hProcSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcSnapshot == INVALID_HANDLE_VALUE) {
		MyDbgPrint("Create Snap shot failed");
		return bRet;
	}
	BOOL bProcess = Process32First(hProcSnapshot, &procEntry);
	if (bProcess == FALSE) {
		MyDbgPrint("Could not get process");
		return bRet;
	}
	while (bProcess) {
		MyDbgPrint("ProcessID:[%x]:%ls", procEntry.th32ProcessID, procEntry.szExeFile);
		uint8_t bSign[33] = { 0 };
		wchar_t chSign[33] = { 0 };
		md5((const uint8_t*)procEntry.szExeFile, wcslen(procEntry.szExeFile), bSign);
		for (int i = 0; i < 16; i++) {
			swprintf(chSign + i * 2, 3, L"%02x", bSign[i]);
		}
#ifdef _DEBUG
		printf("%ls\n", chSign);
#endif 
		for (int i = 0; i < sizeof(checkSign) / sizeof(char**); i++) {
			if (!wcscmp(chSign, checkSign[i])) {
				printf("=====[+]====== Get It =====[+]======\n");
#ifdef _RELEASE
				exit(-1);
#endif
				break;
			}
		}
		// TOOD: add md5 compare here
		bProcess = Process32Next(hProcSnapshot, &procEntry);
	}
	bRet = true;
	// pass process check, we pubish this event
	int dwMagic = 0x6a;
	if (bEvent == false) {
		EventHandle::AEventPublisher::publish(container, procEvent, &dwMagic);
		bEvent = true;
	}
	return bRet;
}

bool Protector::ProtectorContext::DebuggerCheckWithPEB() {
	void* peb = GetPEB();

	PVOID pPeb = GetPEB();
	// PVOID pPeb64 = GetPEB64();
	DWORD offsetNtGlobalFlag = 0;
#ifdef _WIN64
	offsetNtGlobalFlag = 0xBC;
#else
	offsetNtGlobalFlag = 0x68;
#endif
	DWORD NtGlobalFlag = *(PDWORD)((PBYTE)pPeb + offsetNtGlobalFlag);
	if (NtGlobalFlag & NT_GLOBAL_FLAG_DEBUGGED)
	{
		MyDbgPrint("PEB Debugging program!\n");
		// exit(-1);
	}
	return true;
}
// 3. normal: check the IsDebuggerPresent()
bool Protector::ProtectorContext::DebuggerProtector() {
	BOOL bDebug = true;
	do {
		if (pfnZwQueryInformationThread) {
			pfnZwQueryInformationThread(GetCurrentThread(), ThreadHideFromDebugger, NULL, 0);
		}
		if (IsDebuggerPresent()) {
			// break;
		}
		if (CheckRemoteDebuggerPresent(GetCurrentProcess(), (PBOOL)&bDebug)) {
			// break;
		}
		// make sure this function can work
		if (pfnNtQueryInformationProcess) {
			int debugPort = 0;
			pfnNtQueryInformationProcess(GetCurrentProcess(), 7, &debugPort, sizeof(debugPort), NULL);
			bDebug = debugPort != 0;
			HANDLE hDbgObj = NULL;
			pfnNtQueryInformationProcess(GetCurrentProcess(), 0x1e, &hDbgObj, sizeof(hDbgObj), NULL);
			bDebug = hDbgObj != NULL;
		}
		// TODO:add more debug check method
		 bDebug = DebuggerCheckWithPEB();
	} while (false);
	if (bDebug) {
#ifdef _DEBUG
		MyDbgPrint("Deteched!\n");
#else
		exit(-1);
#endif
	}
	return bDebug;
}

bool Protector::ProtectorContext::QueueAPCFunc(APCInsertFunc func) {
	if (pfnNtQueueApcThread) {
		HANDLE hThread = GetCurrentThread();
		pfnNtQueueApcThread(hThread, func, NULL, NULL, NULL);
	}
	return true;
}

bool ProcessInterace::InitProcessCheckHandler() {
	proCheckHandler = new Protector::ProcessCheckHandler(PROCESS_START, Protector::PROCESS_CHECK_PASS);
	passCheckHandler = new Protector::PasswordCheckHandler(PASSWORD_CHECK, Protector::PASSWORD_CHECK_PASS);
	dbgCheckHandler = new Protector::DebuggerCheckHandler(DEBUGER_CHECK, Protector::DEBUGGER_CHECK_PASS);
	int dwRet = EventHandle::AEventSubscriber::subscribe(container, proCheckHandler);
	dwRet = EventHandle::AEventSubscriber::subscribe(container, passCheckHandler);
	dwRet = EventHandle::AEventSubscriber::subscribe(container, dbgCheckHandler);
	
	return dwRet >= 0;
}