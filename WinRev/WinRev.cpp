// WinRev.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include"RevProtect.h"
#include"Common.h"

extern void UnitTestForAEvent();

// Global Event Container
EventHandle::AEventContainer* container = 0;
Protector::ProtectorContext* protContext = 0;
bool TestForAntiDbg() {
	Protector::ProtectorContext prot;
	prot.ThreadProtector();
	return true;
}

bool GlobalInit() {
	bool bRet = false;
	if (container == NULL)
		container = new EventHandle::AEventContainer();
	
	if (!ProcessInterace::InitProcessCheckHandler()) {
		MyDbgPrint("Init failed");			
		return bRet;
	}
	return false;
}

// check step 1
bool BeginCheck() {
	bool bRet = false;
	protContext = new Protector::ProtectorContext();
	if (protContext->ProcessProtector()) {
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
	BeginCheck();
    return 0;
}

