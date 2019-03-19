#include"stdafx.h"
#include"RevProtect.h"
#include"Common.h"
#include"LoadDLL.h"
#include"DLLHeader.h"
#include <process.h>

void NTAPI __stdcall TLS_CALLBACK1(PVOID DllHandle, DWORD dwReason, PVOID Reserved);

#ifdef _M_IX86  
#pragma comment (linker, "/INCLUDE:__tls_used")  
#pragma comment (linker, "/INCLUDE:__tls_callback")  
#else  
#pragma comment (linker, "/INCLUDE:_tls_used")  
#pragma comment (linker, "/INCLUDE:_tls_callback")  
#endif  
EXTERN_C
#ifdef _M_X64  
#pragma const_seg (".CRT$XLB")  
const
#else																																		
#pragma data_seg (".CRT$XLB")  
#endif  

PIMAGE_TLS_CALLBACK _tls_callback[] = { TLS_CALLBACK1,0 };
#pragma data_seg ()  
#pragma const_seg ()  

VOID NTAPI TLS_CALLBACK1(PVOID DllHandle, DWORD Reason, PVOID Reserved)
{
	if (IsDebuggerPresent())
	{
		MessageBoxA(NULL, "Stop debugging program!", "Error", MB_OK | MB_ICONERROR);
		// TerminateProcess(GetCurrentProcess(), 0xBABEFACE);
	}
}

extern void UnitTestForAEvent();
extern void UnitTestForDLL();
extern void CheckGlobalFlagsClearInProcess();
extern void CheckGlobalFlagsClearInFile();

// Global Event Container
EventHandle::AEventContainer* container = 0;
ProcessInterace* procInterface = 0;
Protector::ProtectorContext* protContext = 0;

PVOID FinalLoadLibrary();
//void DecryptPartOne(char* passwd) {
//	// TODO:use passwd to decode this content
//	int length = strlen(passwd);
//	MyDbgPrint("[DecryptPartOne] Decrypt with key %s", passwd);
//	for (int i = 0; i < g_dwOneOffset; i++) {
//		DLL_Content[i] ^= passwd[i % length];
//	}
//	return;
//}
bool TestForAntiDbg() {
	Protector::ProtectorContext prot;
	prot.ThreadProtector();
	return true;
}

bool GlobalInit() {
	bool bRet = false;
	if (container == NULL) {
		container = new EventHandle::AEventContainer();
	}

	if (procInterface == NULL) {
		procInterface = new ProcessInterace();
	}
	if (!procInterface->InitProcessCheckHandler()) {
		MyDbgPrint("Init failed");
		return bRet;
	}
	protContext = new Protector::ProtectorContext();
	protContext->InitProtector();
	return false;
}

bool CheckPasswd(char* passwd) {
	char checkpw[] = "jZekqArhU|9g>0O}|d";
	char tmp[19] = { 0 };
	for (int i = 0; i < strlen(checkpw); i++) {
		tmp[i] = passwd[i] ^ i;
	}
	int maps[] = { 1, 5, 4, 2, 3, 0 };
	for (int i = 0; i < strlen(checkpw); i++) {
		int index = maps[i % 6] + (i / 6) * 6;
		if (checkpw[i] != tmp[index]) {
			MyDbgPrint("Error compare at %x", i);
			return false;
		}
	}
	MyDbgPrint("Finish print!");
	return true;
}
// Akira_aut0_ch3ss_!
bool FakeChecking() {
	bool bRet = false;
	char passwd[19] = { 0 };
	puts("[+]======================[+]");
	puts("[+] Akira's Homework 2nd [+]");
	puts("[+]======================[+]");
	puts("[=] My passwords is:");
	scanf("%18s", passwd);
	if (CheckPasswd(passwd)) {
		// here we publish new event
		// we use akira key to decode the message and 
		// add this into object
		EventHandle::AEvent passCheck(2, Protector::PASSWORD_CHECK_PASS);
		EventHandle::AEventPublisher::publish(container, passCheck, passwd);
		bRet = true;
	}
	return bRet;
}
// check step 1
// ================ TODO: Add this function to multi-thread ====================
unsigned int __stdcall BeginCheck(void*) {
	static bool bPassCheck = false;
	bool bRet = false;
	if (!protContext->ProcessProtector()) {
		MyDbgPrint("The Process Protector Failed!");
#ifdef _RELEASE
		exit(-1)
#endif
	}
	// start new threads
	protContext->DebuggerProtector();
	// delete protContext;
	if (!bPassCheck) {
		EventHandle::AEvent procEvent(3, Protector::DEBUGGER_CHECK_PASS);
		EventHandle::AEventPublisher::publish(container, procEvent, NULL);
		protContext->QueueAPCFunc(FinalLoadLibrary);
	}
	SleepEx(3, TRUE);
	return bRet;

}

PVOID FinalLoadLibrary() {
	bool bRet = true;
	HANDLE hShareMem = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, g_dwMemSize, SHARE_MEMORY);
	if (hShareMem == NULL) {
		printf("[UnitTestForLoadDLL] create file mapping failed with %x!\n", GetLastError());
		bRet = false;
	}
	PVOID buffer = MapViewOfFile(hShareMem, FILE_MAP_ALL_ACCESS, 0, 0, g_dwMemSize);
	if (buffer == NULL) {
		printf("[UnitTestForLoadDLL] map file failed with %x\n", GetLastError());
		bRet = false;
	}
	memset(buffer, '\0', g_dwMemSize);

	char test_buffer[] = { 0x7c,0x45,0x38,0x38,0x59,0x4f,0x56,0x8f,0xb6,0x9d,0xaa,0x7b,0x9c,0xb6,0x11,0x36,0x6f,0x6d,0x77,0x6f,0x72,0x6b,0x21,0x7d };

	memcpy(buffer, test_buffer, g_dwBufferSize);
	HANDLE hEvent = CreateEvent(NULL, FALSE, TRUE, DLL_INPUT);
	if (hEvent == INVALID_HANDLE_VALUE) {
		printf("[UnitTestForLoadDLL] event failed with %x\n", GetLastError());
		bRet = false;
	}
	//HANDLE hFile = CreateFile(L"..\\x64\\Debug\\RevDLL.dll", GENERIC_READ,
	//	FILE_SHARE_READ, NULL, OPEN_EXISTING,
	//	FILE_ATTRIBUTE_NORMAL, NULL);
	//if (hFile == INVALID_HANDLE_VALUE) {
	//	printf("[UnitTestForLoadDLL] Create file error!\n");
	//	return false;
	//}
	int dwDLLSize = 0;
	// dwDLLSize = GetFileSize(hFile, NULL);
	dwDLLSize = g_dwDLLSize;
	char* bufFile = new char[dwDLLSize];
		// (char*)malloc(dwDLLSize);
	ZeroMemory(bufFile, dwDLLSize);
	// ReadFile(hFile, bufFile, dwDLLSize, NULL, NULL);
	// check the magic number
	memcpy(bufFile, DLL_Content, g_dwDLLSize);
	LOAD_DLL_INFO *hDLL = new LOAD_DLL_INFO;
	if (bufFile[0] == 'M' && bufFile[1] == 'Z') {
		// try to read file from memory
		DWORD res = LoadDLLFromMemory(bufFile, dwDLLSize, NULL, hDLL);
		if (res != ELoadDLLResult_OK) {
			printf("[UnitTestForLoadDLL] Load DLL From memory failed!\n");
			bRet = false;
		}
		// HANDLE hDLL = LoadLibrary(L"..\\x64\\Debug\\RevDLL.dll");
	}
	delete hDLL;
	delete bufFile;
	CloseHandle(hEvent);
	CloseHandle(hShareMem);
	return &bRet;
}
int main()
{
	GlobalInit();
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, NULL,
		BeginCheck,
		NULL, NULL, NULL);
	FakeChecking();
	BeginCheck(NULL);
	// UnitTestForDLL();
	
	// FinalLoadLibrary();
	// FakeChecking();
    return 0;
}

