// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include"../WinRev/Common.h"

void AttachProcess() {
	char input[18] = { 0 };
	puts("Emmm? you now the answer?");
	scanf("%17s", input);
	// here we write the buffer to share memory, and wen send the 
	// really event to the main thread to make main thread
	const int dwSize = 0x8000;
	HANDLE hSharemem = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, NULL, dwSize, SHARE_MEMORY);
	if (hSharemem == NULL) {
		printf("Create File failed!\n");
		return;
	}
	void* buffer = MapViewOfFile(hSharemem, FILE_MAP_WRITE, 0, 0, dwSize);
	if (buffer == NULL) {
		printf("Map failed\n");
		CloseHandle(hSharemem);
	}
	memcpy(buffer, input, sizeof(input));
	// end 
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
	case DLL_PROCESS_ATTACH: {

	}
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

