#include"RevProtect.h"
// #include"ThreadHeader.h"

extern EventHandle::AEventContainer* container;
wchar_t checkSign[][33] = {
	L"438078d884693cdb2dbc12a84d381899",
	L"6de452781ffd3e77696e0564d27dbdfd",
	L"611db7cd21044e969b0b28008d1ef565",
	L"2cffaa33fdbfe6dea9df8aabc71b9989",
	L"c898eaf62c0cbcc089939366f516e09f",
	L"5303e7f2944c9081f864a2e11a8a0aef",
};

bool Protector::ProtectorContext::InitProtector() {
	bool bRet = false;
	HMODULE hDll = GetModuleHandle(L"ntdll.dll");
	if (hDll == NULL) {
		printf("[ERROR] Get Handle failed!\n");
		return bRet;
	}
	pfnNtCurrentTEB = (PFNNtCurrentTEB)GetProcAddress(hDll, "NtCurrentTeb");
	pfnNtQueryInformationFile = (PFNNtQueryInformationFile)GetProcAddress(hDll, "NtQueryInformationFile");
	if (!pfnNtCurrentTEB || !pfnNtQueryInformationFile) {
		MyDbgPrint("Could not get all the function ptr!\n");
		return bRet;
	}
	bRet = true;
	return bRet;
}

bool Protector::ProtectorContext::ThreadProtector() {
	bool bRet = FALSE;
	// PTEB teb = NULL;
#ifdef _DEBUG
	HMODULE hDll = GetModuleHandle(L"ntdll.dll");
	if (hDll == NULL) {
		printf("[ERROR] Get Handle failed!\n");
		return bRet;
	}
	/*pfnNtCurrentTEB = (PFNNtCurrentTEB)GetProcAddress(hDll, "NtCurrentTeb");*/
	if (pfnNtCurrentTEB == NULL) {
#if defined (_M_AMD64)
		teb = (struct _TEB *)__readgsqword(FIELD_OFFSET(NT_TIB, Self));
	}
#else
		printf("[ERROR] Get NtCurrentTeb failed!\n");
		return bRet;
	}
#endif
	WCHAR wcPath[MAX_PATH] = { 0 };
	
	// GetSystemDirectory(wcPath, MAX_PATH);
	// std::string wszPath = wcPath;
	// wszPath += "\\gdi32.dll";
	std::wstring wszPath = L"gdi32.dll";
	// WCHAR testPath[] = L"gdi32.dll";
	HANDLE hFile = CreateFile(wszPath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, OPEN_EXISTING, 0, 0);
	CloseHandle(hFile);
	HMODULE hAntiDbg = LoadLibrary(wszPath.c_str());
	
	// here, we stop the event deliver
	__asm {
		pushad
		mov eax,dword ptr fs:[0x18]
		mov byte ptr[eax+0xFCA],0x80
		popad
	}
	/*if (teb == NULL) {
		teb = (PTEB)pfnNtCurrentTEB();
	}
	USHORT usTmpSupportMsg = teb->SuppressDebugMsg;*/
	// teb->SuppressDebugMsg = 0x80;
	// and now we can try to detect
	FreeLibrary(hAntiDbg);
	hFile = NULL;
	hFile = CreateFile(wszPath.c_str(), GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	if (hFile != INVALID_HANDLE_VALUE) {
		puts("TODO:Using Event to notify check is passed");
	}
	else {
		puts("YOU ARE DEBUG!!!");
	}
#endif

}
// 2. check the process that has not hash number(like ida.exe ida64.exe so on)
bool Protector::ProtectorContext::ProcessProtector() {
	bool bRet = false;
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
	// TODO:here we send one part of encode dll object to here
	EventHandle::AEventPublisher::publish(container, procEvent, NULL);	
	return bRet;
}
// 3. normal: check the IsDebuggerPresent()
bool Protector::ProtectorContext::DebuggerProtector() {
	bool bDebug = true;
	do {
		if (IsDebuggerPresent()) {
			break;
		}
		if (CheckRemoteDebuggerPresent(GetCurrentProcess(), (PBOOL)&bDebug)) {
			break;
		}
		if (pfnNtQueryInformationFile) {
			int debugPort = 0;
		}
	} while (false);
	if (bDebug) {
#ifdef _DEBUG
		MyDbgPrint("Deteched!\n");
#else
		exit(-1);
#endif
	}
}

bool ProcessInterace::InitProcessCheckHandler() {
	proCheckHandler = new Protector::ProcessCheckHandler(PROCESS_START, Protector::PROCESS_CHECK_PASS);
	int dwRet = EventHandle::AEventSubscriber::subscribe(container, proCheckHandler);
	
	return dwRet >= 0;
}