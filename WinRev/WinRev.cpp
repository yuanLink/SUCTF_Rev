// WinRev.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include"RevProtect.h"
#include"Common.h"
//http://waleedassar.blogspot.com
//http://www.twitter.com/waleedassar
//Upon receiving LOAD_DLL_DEBUG_EVENT debug events, debuggers save the value of
//the "hFile" member of LOAD_DLL_DEBUG_INFO somewhere so that it can use it to read info and 
//then close it upon receiving the corresponding UNLOAD_DLL_DEBUG_INFO.

//Windbg and IDA Pro debuggers cache this handle and do not close it until the corresponding
//UNLOAD_DLL_DEBUG_EVENT is received. 
//So, to detect these debuggers, we set TEB.SuppressDebugMsg just after LoadLibrary
//has been called. This way the debugger will never receive UNLOAD_DLL_DEBUG_EVENT then any try to 
//acquire exclusive access to the Dll file after FreeLibrary will fail.
//Executable can be found at:
//http://code.google.com/p/ollytlscatch/downloads/detail?name=SuppressDebugMsg.exe

// #include "stdafx.h"
// #include <Windows.h>
//#include "stdio.h"
//#define IDD_MFPLAYBACK_DIALOG           102
//#define IDM_EXIT                        105
//#define IDC_MFPLAYBACK                  109
//#define IDD_OPENURL                     129
//#define IDC_EDIT_URL                    1000
//#define ID_FILE_OPENFILE                32771
//#define ID_FILE_OPENURL                 32772
//#define IDC_STATIC                      -1
//#define IDR_WALIED2						2000

//void main()
//{
//	HRSRC h = FindResource(0, MAKEINTRESOURCE(IDR_WALIED2), "WALIED");
//	if (h)
//	{
//		HGLOBAL hG = LoadResource(0, h);
//		if (hG)
//		{
//			void* pDll = LockResource(hG);
//			if (pDll)
//			{
//				char path[MAX_PATH] = { 0 };
//				GetCurrentDirectory(MAX_PATH, path);
//				unsigned long len = strlen(path);
//				if (path[len - 1] != '\\') path[len] = '\\';
//				strcat(path, "walied.dll");
//				HANDLE hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, OPEN_EXISTING, 0, 0);
//				if (hFile == INVALID_HANDLE_VALUE)
//				{
//					hFile = CreateFile(path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, CREATE_ALWAYS, 0, 0);
//				}
//				if (hFile == INVALID_HANDLE_VALUE) ExitProcess(0);
//				else
//				{
//					unsigned long writ;
//					WriteFile(hFile, pDll, SizeofResource(0, h), &writ, 0);
//					CloseHandle(hFile);
//					HMODULE Base = LoadLibrary(path);
//					__asm
//					{
//						pushad
//						mov eax, dword ptr fs : [0x18]
//						mov byte ptr[eax + 0xFCA], 0x80//This is the trick.
//						popad
//					}
//					FreeLibrary(Base);
//					hFile = 0;
//					hFile = CreateFile(path, GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
//					if (hFile != INVALID_HANDLE_VALUE)
//					{
//						MessageBox(0, "Expected behavior", "waliedassar", 0);
//						CloseHandle(hFile);
//					}
//					else
//					{
//						MessageBox(0, "Debugger detected", "waliedassar", 0);
//					}
//					DeleteFile(path);
//				}
//			}
//		}
//	}
//}
bool TestForAntiDbg() {
	Protector::ProtectorContext prot;
	prot.ThreadProtector();
	return true;
}
int main()
{
	TestForAntiDbg();
    return 0;
}

