#include"stdafx.h"
#include"RevProtect.h"
#include"Common.h"
#include"LoadDLL.h"
#include"DLLHeader.h"
#include"md5.h"
#include <process.h>
#include"GlobalString.h"

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
		TerminateProcess(GetCurrentProcess(), 0xBABEFACE);
	}
	GetRealMsg(szNTQueryProcess, NTQUERYINFORMATIONPROCESS_MSG);
	GetRealMsg(szZwQueryThread, ZWQUERYINFORMATIONTHREAD_MSG);
	GetRealMsg(szNtQueueApcThread, NTQUEUEAPCTHREAD_MSG);
	GetRealMsg(szNtdll, NTDLL_MSG);
	/*puts((const char*)szNTQueryProcess);
	puts((const char*)szZwQueryThread);
	puts((const char*)szNtQueueApcThread);
	puts((const char*)szNtdll);*/
}

//extern void UnitTestForAEvent();
//extern void UnitTestForDLL();
extern void CheckGlobalFlagsClearInProcess();
extern void CheckGlobalFlagsClearInFile();

// Global Event Container
EventHandle::AEventContainer* container = 0;
ProcessInterace* procInterface = 0;
Protector::ProtectorContext* protContext = 0;
HANDLE g_ReadyLibrary[3];
PVOID FinalLoadLibrary();
//void EncMessage(char key[], char answer[], char buffer[]) {
//	struct AES_ctx ctx;
//	AES_init_ctx(&ctx, (const uint8_t*)key);
//	memcpy(answer, buffer, g_dwBufferSize);
//	AES_ECB_encrypt(&ctx, (uint8_t*)answer);
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
	g_ReadyLibrary[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
	g_ReadyLibrary[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
	g_ReadyLibrary[2] = CreateEvent(NULL, FALSE, FALSE, NULL);

	protContext = new Protector::ProtectorContext();
	protContext->InitProtector();
	return false;
}

bool CheckPasswd(char* passwd) {
	const int dwPWLength = sizeof(szCheckPasswd);
	char checkpw[dwPWLength+1] = { 0 };
	memcpy(checkpw, szCheckPasswd, sizeof(szCheckPasswd));
	GetRealMsg(checkpw, CHECK_PASSWD_MSG)
	char tmp[19] = { 0 };
	for (int i = 0; i < sizeof(checkpw) - 1; i++) {
		tmp[i] = passwd[i] ^ i;
	}
	int maps[] = { 1, 5, 4, 2, 3, 0 };
	for (int i = 0; i < sizeof(checkpw) - 1; i++) {
		int index = maps[i % 6] + (i / 6) * 6;
		if (checkpw[i] != tmp[index]) {
			// MyDbgPrint("Error compare at %x", i);
			return false;
		}
	}
	// MyDbgPrint("Finish print!");
	return true;
}
// Akira_aut0_ch3ss_!
// flag{Ak1rAWin!}
bool FakeChecking() {
	bool bRet = false;
	char passwd[19] = { 0 };
	char szWelTmp[sizeof(szWelcome) + 1] = { 0 };
	memcpy(szWelTmp, szWelcome, sizeof(szWelcome));
	PrintRealMsg(szWelcome, WELCOME_MSG)
	scanf_s("%18s", passwd,19);
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
unsigned int __stdcall BeginCheck(void* arg_list) {
	static bool bPassCheck = false;
	bool bRet = false;
	while (true) {
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
			protContext->QueueAPCFunc(*(HANDLE*)arg_list, FinalLoadLibrary);
		}
		SleepEx(3, TRUE);
	}
	return bRet;

}

PVOID FinalLoadLibrary() {
	bool bRet = true;
	DWORD dwEventRet = WaitForMultipleObjects(3, g_ReadyLibrary, TRUE,5000);
	if (dwEventRet == WAIT_TIMEOUT) {
		PrintRealMsg(szTimeout, TIME_OUT_MSG);
		// printf("Time out!\n");
		exit(-1);
	}
	PVOID buffer = NULL;
	HANDLE hShareMem = NULL;
	char* bufFile = NULL;
	LOAD_DLL_INFO *hDLL = NULL;
	HANDLE hEvent = NULL;
	do {
		hShareMem = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, g_dwMemSize, SHARE_MEMORY);
		if (hShareMem == NULL) {
			// printf("[UnitTestForLoadDLL] create file mapping failed with %x!\n", GetLastError());
			bRet = false;
			exit(-1);
			// break;
		}
		buffer = MapViewOfFile(hShareMem, FILE_MAP_ALL_ACCESS, 0, 0, g_dwMemSize);
		if (buffer == NULL) {
			// printf("[UnitTestForLoadDLL] map file failed with %x\n", GetLastError());
			bRet = false;
			exit(-1);
			// break;
		}
		memset(buffer, '\0', g_dwMemSize);

		char test_buffer[] = { 0x94,0xbf,0x7a,0xc,0xa4,0x35,0x50,0xd1,0xc2,0x15,0xec,0xef,0x9d,0x9a,0xaa,0x56 };

		memcpy(buffer, test_buffer, g_dwBufferSize);
		hEvent = CreateEvent(NULL, FALSE, TRUE, DLL_INPUT);
		if (hEvent == INVALID_HANDLE_VALUE) {
			// printf("[UnitTestForLoadDLL] event failed with %x\n", GetLastError());
			exit(-1);
			// bRet = false;
			break;
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
		bufFile = new char[dwDLLSize];
		// (char*)malloc(dwDLLSize);
		ZeroMemory(bufFile, dwDLLSize);
		// ReadFile(hFile, bufFile, dwDLLSize, NULL, NULL);
		// check the magic number
		memcpy(bufFile, DLL_Content, g_dwDLLSize);
		hDLL = new LOAD_DLL_INFO;
		// printf("try to cakk!\n");
		if (bufFile[0] == 'M' && bufFile[1] == 'Z') {
			// try to read file from memory
			
			DWORD res = LoadDLLFromMemory(bufFile, dwDLLSize, NULL, hDLL);
			if (res != ELoadDLLResult_OK) {
				// printf("[UnitTestForLoadDLL] Load DLL From memory failed!\n");
				bRet = false;
				break;
			}
			// HANDLE hDLL = LoadLibrary(L"..\\x64\\Debug\\RevDLL.dll");
		}
	} while (false);
	if(hDLL != NULL)
		delete hDLL;
	if(bufFile != NULL)
		delete bufFile;
	if(hEvent != NULL && hEvent != INVALID_HANDLE_VALUE)
		CloseHandle(hEvent);
	if(hShareMem != NULL && hShareMem != INVALID_HANDLE_VALUE)
	CloseHandle(hShareMem);
	return &bRet;
}

void CheckSpecialFile() {
	// now we test for the Alertable File Stream
	WIN32_FIND_DATA ffd;
	WIN32_FIND_STREAM_DATA ffsd;
	const int STREAM_SIZE = 0x40;
	char szStreamBuffer[STREAM_SIZE] = { 0 };
	puts("Now we check for the sign ...");
	wchar_t szPathBuf[MAX_PATH] = { 0 };
	wchar_t szModulePath[MAX_PATH] = { 0 };
	int bRet = 0;
	//_wgetcwd(szPathBuf, MAX_PATH * sizeof(wchar_t));
	//printf("Now execute path %ls\n", szPathBuf);
	GetModuleFileName(NULL, szModulePath, sizeof(szModulePath));
	printf("Now module path %ls\n", szModulePath);
	wcscat_s(szModulePath, MAX_PATH * sizeof(wchar_t), L":signature");
	// bRet = GetCurrentDirectory(MAX_PATH, szPathBuf);
	/*if (bRet == 0) {
		puts("require failed");
		return;
	}*/
	wcscat_s(szPathBuf, MAX_PATH * sizeof(wchar_t), L"\\*");
	HANDLE hExeFile = CreateFile(szModulePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE == hExeFile) {
		puts("Could not find signature file!");
		return;
	}
	bool bRead = ReadFile(hExeFile, szStreamBuffer, STREAM_SIZE, NULL, NULL);
	if (bRead) {
		// Overwatch
		// fcaeeb6e34b4303e99b91206bd325f2b
		uint8_t sign[16] = { 0 };
		md5((uint8_t*)szStreamBuffer, strlen(szStreamBuffer), sign);
		uint8_t check_sign[] = { 252, 174, 235, 110, 52, 180, 48, 62, 153, 185, 18, 6, 189, 50, 95, 43 };
		printf("stream is: %s with length is %d\n", szStreamBuffer, strlen(szStreamBuffer));
		for (int i = 0; i < sizeof(check_sign) / sizeof(uint8_t); i++) {
			if (check_sign[i] != sign[i]) {
				printf("error happend at [%x]%x!=%x\n", i,check_sign[i], sign[i]);
				return;
			}
		}
		puts("Pass test!");
		return;
	}
	//HANDLE hFind = FindFirstFile(szPathBuf, &ffd);

	//if (INVALID_HANDLE_VALUE == hFind)
	//{
	//	puts("Find file failed!");
	//	return;
	//}

	//do
	//{
	//	wchar_t szFilePath[MAX_PATH] = { 0 };
	//	wcsncpy(szFilePath, szPathBuf, wcslen(szPathBuf) - 1);
	//	wcscat(szFilePath, ffd.cFileName);
	//	printf("file name %ls\n", szFilePath);
	//	HANDLE hFindStream = FindFirstStreamW(szFilePath, FindStreamInfoStandard, &ffsd, NULL);
	//	if (hFindStream == INVALID_HANDLE_VALUE) {
	//		printf("not find stream!\n");
	//		continue;
	//	}
	//	if (ffsd.StreamSize.QuadPart != 0) {
	//		/*HANDLE hFileStream = CreateFile(szFilePath, )*/
	//		bool bRead = ReadFile(hFindStream, szSteeamBuffer, STREAM_SIZE, NULL, NULL);
	//		printf("the read szStreamBuffer is %ls", szSteeamBuffer);
	//	}
	//} while (FindNextFile(hFind, &ffd));
	// FindFirstStream
}
int main()
{
	//char key[] = "Ak1i3aS3cre7K3y";
	//unsigned char answer[44] = { 0 };
	//unsigned char buffer[] = "flag{Ak1rAWin!}";
	//EncMessage(key, (char*)answer, (char*)buffer);
	//for (int i = 0; i < sizeof(answer); i++) {
	//	printf("0x%x,",answer[i]);
	//}
	CheckSpecialFile();
	//GlobalInit();
	//HANDLE hCurThread = GetCurrentThread();
	//// Test for multithread
	//HANDLE hThread = (HANDLE)_beginthreadex(NULL, NULL,
	//	BeginCheck,
	//	&hCurThread, NULL, NULL);
	//
	//if (!FakeChecking()) {
	//	PrintRealMsg(szWrongPasswd, WRONG_PASSWD_MSG);
	//	// TerminateProcess(GetCurrentProcess, 0);
	//	exit(-1);
	//}
	//SleepEx(5000, TRUE);
    return 0;
}

