// WinRev.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include"RevProtect.h"
#include"Common.h"

extern void UnitTestFoAEvent();
bool TestForAntiDbg() {
	Protector::ProtectorContext prot;
	prot.ThreadProtector();
	return true;
}
int main()
{
	UnitTestFoAEvent();
    return 0;
}

