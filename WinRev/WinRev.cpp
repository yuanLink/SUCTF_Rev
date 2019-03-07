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

// Global Event Container
EventHandle::AEventContainer* container = 0;
ProcessInterace* procInterface = 0;
Protector::ProtectorContext* protContext = 0;
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
	if (!protContext->ProcessProtector()) {
		MyDbgPrint("The Process Protector Failed!");
#ifdef _RELEASE
		exit(-1)
#endif
	}
	return bRet;

}
int main()
{
	GlobalInit();
	// BeginCheck();
	CheckGlobalFlagsClearInProcess();
    return 0;
}

