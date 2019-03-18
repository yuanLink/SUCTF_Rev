#pragma once
#ifndef COMMON_H
#define COMMON_H
/*
 * Define some common MACRO
 */

/* This event will resume only when 
 * 1. pass the thread check
 * 2. pass the simple check
 * And this event will used to trigger to APC initialize ,which will try to initialize
 * the real object
 */

// ======= TODO: this global object we use MACRO or function to encrypt, and we decrypt
// them when we really need =============
#define CHECK_PASS L"ThreadCheckPass"
#define SHARE_MEMORY L"ShareMemory"
#define DLL_INPUT L"DLLInput"
const int g_dwMemSize = 0x8000;
const int g_dwBufferSize = 24;
#include<stdio.h>
#include<Windows.h>
#include<string>
#include"aes.h"
#endif COMMON_H