// This is a small example program that demonstrates the loading/usage of 2 DLLs:
// One of them is written in C while the other in C++.
#include "LoadDLL.h"
#include "TestDLL.h"
#include "TestDLL_CPP.h"
#include <stdio.h>
#include <crtdbg.h>


// Loading the DLL with WinAPI LoadLibrary allows us to debug the DLL.
// With 'manual DLL loading' the debugger doesn't recognize the DLL code
// and can't associate it with the debug info. It's better to use the
// 'manual DLL loader' only in case of release and during development
// compile/test with 'manual DLL loading' only to test whether you are
// using a feature that isn't allowed.
#if USE_LOADLIBRARY


typedef HMODULE MODULE_HANDLE;

MODULE_HANDLE LoadModule(const char* dll_path)
{
	return LoadLibraryA(dll_path);
}

bool UnloadModule(MODULE_HANDLE handle)
{
	return FALSE != FreeLibrary(handle);
}

void* GetModuleFunction(MODULE_HANDLE handle, const char* func_name)
{
	return (void*)GetProcAddress(handle, func_name);
}


#else


typedef LOAD_DLL_INFO* MODULE_HANDLE;

MODULE_HANDLE LoadModule(const char* dll_path)
{
	LOAD_DLL_INFO* p = new LOAD_DLL_INFO;
	DWORD res = LoadDLLFromFileName(dll_path, 0, p);
	if (res != ELoadDLLResult_OK)
	{
		delete p;
		return NULL;
	}
	return p;
}

bool UnloadModule(MODULE_HANDLE handle)
{
	bool res = FALSE != UnloadDLL(handle);
	delete handle;
	return res;
}

void* GetModuleFunction(MODULE_HANDLE handle, const char* func_name)
{
	return (void*)myGetProcAddress_LoadDLLInfo(handle, func_name);
}

#endif



void Test_DLL()
{
	printf("Testing the DLL written in C\n");
	printf("----------------------------\n");
	MODULE_HANDLE hDll = LoadModule("TestDLL.dll");
	if (!hDll)
	{
		printf("LoadDLL() failed!\n");
		return;
	}

	typedef const DLLInterface*(*PGetDLLInterface)();
	PGetDLLInterface GetDLLInterface = (PGetDLLInterface)GetModuleFunction(hDll, "GetDLLInterface");
	if (!GetDLLInterface)
	{
		printf("GetDLLFunction() failed!\n");
		return;
	}

	const DLLInterface* dll_interface = GetDLLInterface();
	int sum = dll_interface->AddNumbers(5, 6);
	printf("AddNumbers(5, 6): %d\n", sum);

	dll_interface->MyMessageBox("Calling MyMessageBox() from the user of the DLL.");

	if (!UnloadModule(hDll))
		printf("UnloadDLL() failed!\n");

	printf("\n");
}


void Test_DLL_CPP()
{
	printf("Testing the DLL written in C++\n");
	printf("------------------------------\n");

	MODULE_HANDLE hDLL = LoadModule("TestDLL_CPP.dll");
	if (!hDLL)
	{
		printf("LoadDLL() failed!\n");
		return;
	}

	typedef I_DLLInterface*(*PGetDLLInterface)();
	PGetDLLInterface GetDLLInterface = (PGetDLLInterface)GetModuleFunction(hDLL, "GetDLLInterface");
	if (!GetDLLInterface)
	{
		printf("GetDLLFunction() failed!\n");
		return;
	}

	I_DLLInterface* dll_interface = GetDLLInterface();
	int sum = dll_interface->AddNumbers(5, 6);
	printf("AddNumbers(5, 6): %d\n", sum);

	dll_interface->MyMessageBox("Calling MyMessageBox() from the user of the DLL_CPP.");

	if (!UnloadModule(hDLL))
		printf("UnloadDLL() failed!\n");

	printf("\n");
}

bool SetupCurrentDir()
{
	wchar_t buf[MAX_PATH];
	DWORD len = GetModuleFileNameW(NULL, buf, MAX_PATH);
	if (!len || len>=MAX_PATH)
		return false;
	wchar_t* p = buf + len;
	while (p>buf && p[-1]!=L'\\' && p[-1]!='/')
		--p;
	while (p>buf && (p[-1]==L'\\' || p[-1]=='/'))
		--p;
	*p = 0;
	if (!SetCurrentDirectoryW(buf))
		return false;
	return true;
}

int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
	if (!SetupCurrentDir())
	{
		printf("Error setting up the current directory for execution.\n");
		return 1;
	}
	Test_DLL();
	Test_DLL_CPP();
	printf("Finished testing.\nPress ENTER to exit.\n");
	getchar();
	return 0;
}
