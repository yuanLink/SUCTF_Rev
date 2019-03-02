#include"AkiraEvent.h"
#include<iostream>
#include<cstdlib>

#define ADD_EVENT			0x00000001
#define SUB_EVENT			0x00000002
#define PRINT_EVENT			0x10000000

int dwGlobalEvent = 0;
void UnitTestFoAEvent(){

	EventHandle::AEvent *AddOne = new EventHandle::AEvent(++dwGlobalEvent);
}