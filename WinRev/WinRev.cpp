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
extern void UnitTestForDLL();
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

bool FakeCompare() {
	return false;
}
int main()
{
	// GlobalInit();
	// BeginCheck();
	UnitTestForDLL();
	// CheckGlobalFlagsClearInProcess();
	// CheckGlobalFlagsClearInFile();
    return 0;
}

