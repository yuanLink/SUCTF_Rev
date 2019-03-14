// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include"../WinRev/Common.h"

void DecMessage(char key[], char answer[], char buffer[]) {
	// TODO: use DES to decrypt this message
	// and final it will get a key with length 18
}
bool AttachProcess() {
	char input[18] = { 0 };
	puts("Emmm? you now the answer?");
	scanf("%17s", input);
	// here we write the buffer to share memory, and wen send the 
	// really event to the main thread to make main thread
	// we wait main thread to
	HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, TRUE, DLL_INPUT);
	if (hEvent == NULL) {
		printf("[Error] Create Event ERROR with ERROR Code %x\n", GetLastError());
		return false;
	}
	// set the event to main thread
	WaitForSingleObject(hEvent, -1);
	CloseHandle(hEvent);
	HANDLE hSharemem = OpenFileMapping(FILE_MAP_ALL_ACCESS, NULL, SHARE_MEMORY);
	if (hSharemem == NULL) {
		printf("Create File failed with ERROR Code %x!\n", GetLastError());
		return false;
	}
	void* buffer = MapViewOfFile(hSharemem, FILE_MAP_READ, 0, 0, g_dwSize);
	if (buffer == NULL) {
		printf("Map failed with ERROR Code %x\n", GetLastError());
		CloseHandle(hSharemem);
		return false;
	}
	CloseHandle(hSharemem);
	char* ans_buffer = (char*)malloc(g_dwSize);
	memset(ans_buffer, '\0', g_dwSize);
	memcpy(ans_buffer, buffer, g_dwSize);
	// now finish, we wait for main thread finish 
	// here we use DES to decrypt the solution
#ifdef _DEBUG
	printf("ans_buffer is %s", ans_buffer);
#endif
	char key[] = "Ak13aK3y";
	char answer[18] = { 0 };
	DecMessage(key, answer, ans_buffer);
	if (!strcmp(answer, input)) {
		printf("Get finally answer!\n");
	}
	else {
		printf("wow... game start!\n");
	}

	// here we will finish all the process
#ifdef _DEBUG
	return true;
#else
	exit(0);
#endif
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	bool bRet = false;
    switch (ul_reason_for_call)
    {
	case DLL_PROCESS_ATTACH: {
		bRet = AttachProcess();
	}
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return bRet;
}

