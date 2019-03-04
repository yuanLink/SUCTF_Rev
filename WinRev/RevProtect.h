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
	class ProtectorContext {
	public:
		ProtectorContext() {
			pfnNtCurrentTEB = nullptr;
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