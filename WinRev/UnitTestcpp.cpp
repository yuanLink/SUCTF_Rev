#include"AEvent.h"
#include<iostream>
#include<cstdlib>

//#define ADD_EVENT			0x00000001
//#define SUB_EVENT			0x00000002
//#define PRINT_EVENT			0x10000000

typedef struct AddObj {
	int num;
}AddObj;

class ADDEventHandler : public EventHandle::AEventHandler {

public:
	ADDEventHandler(int eventhandler_id, EVENTTYPE typeId):AEventHandler(eventhandler_id, typeId) {
	}
	// UnitTest: here the context is the structure
	// to the object(add object)
	bool OnEventTrigger(void *Context) {
		AddObj* addObj = (AddObj*)Context;
		addObj->num += 1;
		return true;
	}
};

class ADD2EventHandler : public EventHandle::AEventHandler {
public:
	ADD2EventHandler(int eventhandler_id, EVENTTYPE typeId) :AEventHandler(eventhandler_id, typeId) {
	}
	// UnitTest: here the context is the structure
	// to the object(add object)
	bool OnEventTrigger(void *Context) {
		AddObj* addObj = (AddObj*)Context;
		addObj->num += 2;
		return true;
	}
};

class MUL3EventHandler : public EventHandle::AEventHandler {

public:
	MUL3EventHandler(int eventhandler_id, EVENTTYPE typeId) :AEventHandler(eventhandler_id, typeId) {
	}
	// UnitTest: here the context is the structure
	// to the object(add object)
	bool OnEventTrigger(void *Context) {
		AddObj* addObj = (AddObj*)Context;
		addObj->num *= 3;
		return true;
	}
};

int dwGlobalEvent = 0;
int dwGlobalEventHandler = 0;
enum UnitTestEvent {
	ADD_EVENT,
	SUB_EVENT,
	MUL_EVENT,
	PRINT_EVENT,
};

// ========= Unit Test for Event =========

/////	1.Test For Event normal work
void UnitTestForOneAEvent(){

	AddObj addObj;
	addObj.num = 0;
	// prepare one event, one handler and one container
	EventHandle::AEvent addOne(++dwGlobalEvent, ADD_EVENT);
	ADDEventHandler *addHandler = new ADDEventHandler(dwGlobalEventHandler, (int)ADD_EVENT);
	
	EventHandle::AEventContainer* container = new EventHandle::AEventContainer();
	// subscribe this handler, use container to save it
	EventHandle::AEventSubscriber::subscribe(container, addHandler);
	// now publish the event, check the event has happend
	EventHandle::AEventPublisher::publish(container, addOne, &addObj);
	
	// finish event, so we unsubscribe the addHandler
	EventHandle::AEventSubscriber::unsubscribe(container, *addHandler);
	// check the event
	EventHandle::AEventPublisher::publish(container, addOne, &addObj);

}

///   2. Test For Two Event 
bool UnitTestForTwoAEvent() {
	puts("============ UnitTestForTwoAEvent ===============");
	bool bRet = false;
	AddObj addObj;
	addObj.num = 0;
	// prepare one event, one handler and one container
	EventHandle::AEvent addOne(++dwGlobalEvent, ADD_EVENT);
	ADDEventHandler *addHandler = new ADDEventHandler(dwGlobalEventHandler, (int)ADD_EVENT);
	ADD2EventHandler *addHandler2 = new ADD2EventHandler(dwGlobalEventHandler, (int)ADD_EVENT);

	EventHandle::AEventContainer* container = new EventHandle::AEventContainer();
	// subscribe this handler, use container to save it
	EventHandle::AEventSubscriber::subscribe(container, addHandler);
	EventHandle::AEventSubscriber::subscribe(container, addHandler2);
	// now publish the event, check the event has happend
	EventHandle::AEventPublisher::publish(container, addOne, &addObj);
	if (addObj.num == 3) {
		puts("[UnitTest] Pass");
	}
	else {
		printf("[UnitTest] Failed! addObj num is %x", addObj.num);
		return bRet;
	}
	// unsubscribe one handler, but contain another one
	EventHandle::AEventSubscriber::unsubscribe(container, *addHandler);
	// check the event
	EventHandle::AEventPublisher::publish(container, addOne, &addObj);
	if (addObj.num == 5) {
		puts("[UnitTest] Pass");
	}
	else {
		printf("[UnitTest] Failed! addObj num is %x", addObj.num);
		return bRet;
	}
	// unsubscribe one handler, but contain another one
	EventHandle::AEventSubscriber::unsubscribe(container, *addHandler2);
	// check the event
	EventHandle::AEventPublisher::publish(container, addOne, &addObj);
	if (addObj.num == 5) {
		puts("[UnitTest] Pass");
		bRet = true;
	}
	else {
		printf("[UnitTest] Failed! addObj num is %x", addObj.num);
		return bRet;
	}

	puts("============ UnitTestForTwoAEvent Finish ===============");
	return bRet;
}

///   3. Test For Two Different Event 
bool UnitTestForTwoDiffAEvent() {

	puts("============ UnitTestForTwoDiffAEvent ===============");
	bool bRet = false;
	AddObj addObj;
	addObj.num = 0;
	// prepare one event, one handler and one container
	EventHandle::AEvent addOne(++dwGlobalEvent, ADD_EVENT);
	EventHandle::AEvent mulOne(++dwGlobalEvent, MUL_EVENT);
	ADDEventHandler *addHandler = new ADDEventHandler(dwGlobalEventHandler++, (int)ADD_EVENT);
	MUL3EventHandler *mulHandler = new MUL3EventHandler(dwGlobalEventHandler++, (int)MUL_EVENT);

	EventHandle::AEventContainer* container = new EventHandle::AEventContainer();
	// subscribe this handler, use container to save it
	EventHandle::AEventSubscriber::subscribe(container, addHandler);
	EventHandle::AEventSubscriber::subscribe(container, mulHandler);
	// now publish the event, check the event has happend
	EventHandle::AEventPublisher::publish(container, addOne, &addObj);
	if (addObj.num == 1) {
		puts("[UnitTest] Pass");
	}
	else {
		printf("[UnitTest] Failed! addObj num is %x", addObj.num);
		return bRet;
	}
	// unsubscribe one handler, but contain another one
	EventHandle::AEventSubscriber::unsubscribe(container, *addHandler);
	// check the event
	EventHandle::AEventPublisher::publish(container, addOne, &addObj);
	if (addObj.num == 1) {
		puts("[UnitTest] Pass");
	}
	else {
		printf("[UnitTest] Failed! addObj num is %x", addObj.num);
		return bRet;
	}
	// check the event
	EventHandle::AEventPublisher::publish(container, mulOne, &addObj);
	if (addObj.num == 3) {
		puts("[UnitTest] Pass");
		bRet = true;
	}
	else {
		printf("[UnitTest] Failed! addObj num is %x", addObj.num);
		return bRet;
	}

	puts("============ UnitTestForTwoDiffAEvent Finish ===============");
	return bRet;
}

void UnitTestForAEvent() {
	if (UnitTestForTwoAEvent()&& UnitTestForTwoDiffAEvent()) {
		printf("[UnitTest] Pass\n");
	}
}