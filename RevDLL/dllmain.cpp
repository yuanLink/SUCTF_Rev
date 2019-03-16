// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include"../WinRev/Common.h"
// #include"../WinRev/aes.h"

void DecMessage(char key[], char answer[], char buffer[]) {
	// TODO: use DES to decrypt this message
	struct AES_ctx ctx;
	AES_init_ctx(&ctx, (const uint8_t*)key);
	memcpy(answer, buffer, g_dwBufferSize);
	AES_ECB_decrypt(&ctx, (uint8_t*)answer);
	// and final it will get a key with length 18
#ifdef _DEBUG
	printf("decryption content is %s", answer);
#endif
}
bool AttachProcess() {
	char input[g_dwBufferSize] = { 0 };
	puts("Emmm? you now the answer?");
	scanf("%21s", input);
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
	void* buffer = MapViewOfFile(hSharemem, FILE_MAP_READ, 0, 0, g_dwMemSize);
	if (buffer == NULL) {
		printf("Map failed with ERROR Code %x\n", GetLastError());
		CloseHandle(hSharemem);
		return false;
	}
	CloseHandle(hSharemem);
	char* ans_buffer = (char*)malloc(g_dwMemSize);
	memset(ans_buffer, '\0', g_dwMemSize);
	memcpy(ans_buffer, buffer, g_dwMemSize);
	// now finish, we wait for main thread finish 
	// here we use DES to decrypt the solution
	char key[] = "Ak13aK3y";
	char answer[g_dwBufferSize+1] = { 0 };
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

