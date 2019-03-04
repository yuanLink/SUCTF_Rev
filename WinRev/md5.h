#pragma once
#ifndef MD5_H
#define MD5_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
void __cdecl md5(const uint8_t *initial_msg, size_t initial_len, uint8_t *digest);

#endif // MD5_H