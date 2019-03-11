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
#define CHECK_PASS L"ThreadCheckPass"
#define SHARE_MEMORY L"Share_Memory"
#include<stdio.h>
#include<Windows.h>
#include<string>
#endif COMMON_H