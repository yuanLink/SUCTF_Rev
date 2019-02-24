#pragma once
// #include<Windows.h>
#include"Common.h"
#ifndef REV_PROTECT_H
#define REV_PROTECT_H
/*
#if defined(_M_IX86)
return (PTEB)__readfsdword(0x18);
#elif defined (_M_AMD64)
return (struct _TEB *)__readgsqword(FIELD_OFFSET(NT_TIB, Self));
*/

namespace Protector {
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
}
#endif REV_PROTECt_H