// WinRev.cpp: 定义控制台应用程序的入口点。
//

#include"stdafx.h"
#include"RevProtect.h"
#include"Common.h"

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
extern void CheckGlobalFlagsClearInProcess();
extern void CheckGlobalFlagsClearInFile();

// Global Event Container
EventHandle::AEventContainer* container = 0;
ProcessInterace* procInterface = 0;
Protector::ProtectorContext* protContext = 0;
extern "C" UINT_PTR __security_cookie;  /* /GS security cookie */

									 /*
									 * The following two names are automatically created by the linker for any
									 * image that has the safe exception table present.
									 */

extern "C" PVOID __safe_se_handler_table[]; /* base of safe handler entry table */
extern "C" BYTE  __safe_se_handler_count;  /* absolute symbol whose address is
									   the count of table entries */
typedef struct {
	DWORD       Size;
	DWORD       TimeDateStamp;
	WORD        MajorVersion;
	WORD        MinorVersion;
	DWORD       GlobalFlagsClear;
	DWORD       GlobalFlagsSet;
	DWORD       CriticalSectionDefaultTimeout;
	ULONGLONG       DeCommitFreeBlockThreshold;
	ULONGLONG       DeCommitTotalFreeThreshold;
	ULONGLONG       LockPrefixTable;            // VA
	ULONGLONG       MaximumAllocationSize;
	ULONGLONG       VirtualMemoryThreshold;
	ULONGLONG       ProcessAffinityMask;
	DWORD       ProcessHeapFlags;
	WORD        CSDVersion;
	WORD        Reserved1;
	DWORD       EditList;                   // VA
	DWORD_PTR   *SecurityCookie;
	PVOID       *SEHandlerTable;
	DWORD       SEHandlerCount;
} IMAGE_LOAD_CONFIG_DIRECTORY64_2;
typedef struct {
	DWORD       Size;
	DWORD       TimeDateStamp;
	WORD        MajorVersion;
	WORD        MinorVersion;
	DWORD       GlobalFlagsClear;
	DWORD       GlobalFlagsSet;
	DWORD       CriticalSectionDefaultTimeout;
	DWORD       DeCommitFreeBlockThreshold;
	DWORD       DeCommitTotalFreeThreshold;
	DWORD       LockPrefixTable;            // VA
	DWORD       MaximumAllocationSize;
	DWORD       VirtualMemoryThreshold;
	DWORD       ProcessAffinityMask;
	DWORD       ProcessHeapFlags;
	WORD        CSDVersion;
	WORD        Reserved1;
	DWORD       EditList;                   // VA
	DWORD_PTR   *SecurityCookie;
	PVOID       *SEHandlerTable;
	DWORD       SEHandlerCount;
} IMAGE_LOAD_CONFIG_DIRECTORY32_2;

const IMAGE_LOAD_CONFIG_DIRECTORY64_2 _load_config_used = {
	sizeof(IMAGE_LOAD_CONFIG_DIRECTORY64_2),
	0,
	0,
	0,
	1,
	1,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
	/*&__security_cookie,
	__safe_se_handler_table,
	(DWORD)(DWORD_PTR)&__safe_se_handler_count*/
};
//extern "C" 
//const IMAGE_LOAD_CONFIG_DIRECTORY _load_config_used = { sizeof(IMAGE_LOAD_CONFIG_DIRECTORY) };

void DecryptPartOne(char* passwd) {
	// TODO:use passwd to decode this content
	MyDbgPrint("[DecryptPartOne] Decrypt with key %s", passwd);
	return;
}
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
	return false;
}

bool CheckPasswd(char* passwd) {
	// TODO:use more difficult encode(ECB)
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
		DecryptPartOne(passwd);
		EventHandle::AEvent passCheck(2, Protector::PASSWORD_CHECK_PASS);
		EventHandle::AEventPublisher::publish(container, passCheck, NULL);
	}
	return false;
}
// check step 1
bool BeginCheck() {
	bool bRet = false;
	protContext = new Protector::ProtectorContext();
	protContext->InitProtector();
	if (!protContext->ProcessProtector()) {
		MyDbgPrint("The Process Protector Failed!");
#ifdef _RELEASE
		exit(-1)
#endif
	}
	protContext->DebuggerProtector();
	delete protContext;
	return bRet;

}
int main()
{
	GlobalInit();
	BeginCheck();
	FakeChecking();
	char tmp[] = "Akira_aut0_ch3ss_!";
	// CheckPasswd(tmp);
	// CheckGlobalFlagsClearInProcess();
	// CheckGlobalFlagsClearInFile();
    return 0;
}

