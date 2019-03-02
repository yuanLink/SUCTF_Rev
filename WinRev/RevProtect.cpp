#include"RevProtect.h"
#include"ThreadHeader.h"
// 1. check thread the SameTebFlags &0x40(if == 1, it is anti-debug)
bool Protector::ProtectorContext::ThreadProtector() {
	bool bRet = FALSE;
	PTEB teb = NULL;
#ifdef _DEBUG
	HMODULE hDll = GetModuleHandle(L"ntdll.dll");
	if (hDll == NULL) {
		printf("[ERROR] Get Handle failed!\n");
		return bRet;
	}
	pfnNtCurrentTEB = (PFNNtCurrentTEB)GetProcAddress(hDll, "NtCurrentTeb");
	if (pfnNtCurrentTEB == NULL) {
#if defined (_M_AMD64)
		teb = (struct _TEB *)__readgsqword(FIELD_OFFSET(NT_TIB, Self));
	}
#else
		printf("[ERROR] Get NtCurrentTeb failed!\n");
		return bRet;
	}
#endif
	WCHAR wcPath[MAX_PATH] = { 0 };
	
	// GetSystemDirectory(wcPath, MAX_PATH);
	// std::string wszPath = wcPath;
	// wszPath += "\\gdi32.dll";
	std::wstring wszPath = L"gdi32.dll";
	// WCHAR testPath[] = L"gdi32.dll";
	HANDLE hFile = CreateFile(wszPath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, OPEN_EXISTING, 0, 0);
	CloseHandle(hFile);
	HMODULE hAntiDbg = LoadLibrary(wszPath.c_str());
	
	// here, we stop the event deliver
	__asm {
		pushad
		mov eax,dword ptr fs:[0x18]
		mov byte ptr[eax+0xFCA],0x80
		popad
	}
	/*if (teb == NULL) {
		teb = (PTEB)pfnNtCurrentTEB();
	}
	USHORT usTmpSupportMsg = teb->SuppressDebugMsg;*/
	// teb->SuppressDebugMsg = 0x80;
	// and now we can try to detect
	FreeLibrary(hAntiDbg);
	hFile = NULL;
	hFile = CreateFile(wszPath.c_str(), GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	if (hFile != INVALID_HANDLE_VALUE) {
		puts("TODO:Using Event to notify check is passed");
	}
	else {
		puts("YOU ARE DEBUG!!!");
	}
#endif

}
// 2. check the process that has not hash number(like ida.exe ida64.exe so on)
bool ProcessProtector();
// 3. normal: check the IsDebuggerPresent()
bool DebuggerProtector();