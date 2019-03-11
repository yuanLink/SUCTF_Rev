#pragma once
// #include<Windows.h>
#include"Common.h"
#include"AEvent.h"
#include"md5.h"
#include<tlhelp32.h>
#ifndef REV_PROTECT_H
#define REV_PROTECT_H
/*
#if defined(_M_IX86)
return (PTEB)__readfsdword(0x18);
#elif defined (_M_AMD64)
return (struct _TEB *)__readgsqword(FIELD_OFFSET(NT_TIB, Self));
*/


namespace Protector {

#define PROTECT_EVENT(id)		(0x100000 | id)

const int PROCESS_CHECK_PASS = PROTECT_EVENT(0x1);
const int DEBUGGER_CHECK_PASS = PROTECT_EVENT(0x2);
	/*!
	 * Define some export function name
	 */
	typedef void* (*PFNNtCurrentTEB)();
	typedef NTSTATUS (*PFNNtQueryInformationProcess)(
		IN HANDLE           ProcessHandle,
		IN int				ProcessInformationClass,
		OUT PVOID           ProcessInformation,
		IN ULONG            ProcessInformationLength,
		OUT PULONG          ReturnLength
	);
	typedef enum THREADINFOCLASS {

		ThreadBasicInformation, // 0
		ThreadTimes, // 1
		ThreadPriority, // 2 
		ThreadBasePriority, // 3 
		ThreadAffinityMask, // 4 
		ThreadImpersonationToken, // 5 
		ThreadDescriptorTableEntry, // 6
		ThreadEnableAlignmentFaultFixup, // 7 
		ThreadEventPair, // 8 
		ThreadQuerySetWin32StartAddress, // 9
		ThreadZeroTlsCell, // 10 
		ThreadPerformanceCount, // 11
		ThreadAmILastThread, // 12 
		ThreadIdealProcessor, // 13 
		ThreadPriorityBoost, // 14
		ThreadSetTlsArrayAddress, // 15 
		ThreadIsIoPending, // 16
		ThreadHideFromDebugger // 17 
	} THREADINFOCLASS;
	typedef NTSTATUS(WINAPI *PFNZwQueryInformationThread)(
		HANDLE ThreadHandle,
		THREADINFOCLASS  ThreadInformationClass,
		PVOID ThreadInformation,
		ULONG ThreadInformationLength);
	class ProtectorContext {
	public:
		ProtectorContext() {
			pfnNtCurrentTEB = nullptr;
			pfnNtQueryInformationProcess = nullptr;
		}
		/* check the protector initialize */
		bool InitProtector();
		// listen to the event
		bool ListenCheckEvent();

		// 1. check thread the SameTebFlags &0x40(if == 1, it is anti-debug)
		bool ThreadProtector();
		// 2. check the process that has not hash number(like ida.exe ida64.exe so on)
		bool ProcessProtector();
		// 3. normal: check the IsDebuggerPresent()
		bool DebuggerProtector();
		// 4. TODO:Exception to check debug
	private:
		PFNNtCurrentTEB pfnNtCurrentTEB;
		PFNNtQueryInformationProcess pfnNtQueryInformationProcess;
		PFNZwQueryInformationThread pfnZwQueryInnformationThread;
		void* GetPEB() {
#ifdef _WIN64
			UINT64 peb = __readgsqword(0x60);
#else
			UINT32 peb = __readfsdword(0x30);
#endif
			return (void*)peb;
		}

		// Get PEB for WOW64 Process
//		PVOID GetPEB64()
//		{
//			PVOID pPeb = 0;
//#ifndef _WIN64
//			// 1. There are two copies of PEB - PEB64 and PEB32 in WOW64 process
//			// 2. PEB64 follows after PEB32
//			// 3. This is true for versions lower than Windows 8, else __readfsdword returns address of real PEB64
//			if (IsWin8OrHigher())
//			{
//				BOOL isWow64 = FALSE;
//				typedef BOOL(WINAPI *pfnIsWow64Process)(HANDLE hProcess, PBOOL isWow64);
//				pfnIsWow64Process fnIsWow64Process = (pfnIsWow64Process)
//					GetProcAddress(GetModuleHandleA("Kernel32.dll"), "IsWow64Process");
//				if (fnIsWow64Process(GetCurrentProcess(), &isWow64))
//				{
//					if (isWow64)
//					{
//						pPeb = (PVOID)__readfsdword(0x0C * sizeof(PVOID));
//						pPeb = (PVOID)((PBYTE)pPeb + 0x1000);
//					}
//				}
//			}
//#endif
//			return pPeb;
//		}
		bool DebuggerCheckWithPEB();
		const int FLG_HEAP_ENABLE_TAIL_CHECK = 0x10;
		const int FLG_HEAP_ENABLE_FREE_CHECK = 0x20;
		const int FLG_HEAP_VALIDATE_PARAMETERS = 0x40;
		const int NT_GLOBAL_FLAG_DEBUGGED = FLG_HEAP_VALIDATE_PARAMETERS | FLG_HEAP_ENABLE_FREE_CHECK | FLG_HEAP_ENABLE_TAIL_CHECK;
	};

	class ProcessCheckHandler :public EventHandle::AEventHandler {
	public:
		ProcessCheckHandler(int eventhandler_id, EVENTTYPE typeId) :AEventHandler(eventhandler_id, typeId) {
			// memset(obj_part2, '\0', 120);
		}
		// this handler will try to add a new object to 
		bool OnEventTrigger(void* Context) {
			MyDbgPrint(obj_part2);
			// TODO:we add a des here to decryption the obj
			// and write the object to the context
			// context may be the dll obj or a struct, TBD
			return true;
		}
	private:
		char obj_part2[120] = "Now just a test";
					
	};

}

class ProcessInterace {
public:
	enum HandlerType {
		PROCESS_START,
		THREAD_QUERY,
		LOAD_LIBRARY
	};
	bool InitProcessCheckHandler();
private:
	// all handler put here
	Protector::ProcessCheckHandler* proCheckHandler;
};

#endif REV_PROTECT_H