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
		addObj.num = 0;
	}
	// UnitTest: here the context is the structure
	// to the object(add object)
	bool OnEventTrigger(void *Context) {
		addObj.num += 1;
		printf("[OnTrigger] addObj is %x!\n", addObj.num);
		return true;
	}
private:
	AddObj addObj;
};
int dwGlobalEvent = 0;
int dwGlobalEventHandler = 0;
enum UnitTestEvent {
	ADD_EVENT,
	SUB_EVENT,
	PRINT_EVENT,
};

// ========= Unit Test for Event =========

/////	1.Test For Event normal work
void UnitTestFoAEvent(){

	// prepare one event, one handler and one container
	EventHandle::AEvent addOne(++dwGlobalEvent, ADD_EVENT);
	ADDEventHandler *addHandler = new ADDEventHandler((int)ADD_EVENT, dwGlobalEventHandler);
	
	EventHandle::AEventContainer* container = new EventHandle::AEventContainer();
	// subscribe this handler, use container to save it
	EventHandle::AEventSubscriber::subscribe(container, addHandler);
	// now publish the event, check the event has happend
	EventHandle::AEventPublisher::publish(container, addOne);
	
}