#ifndef AKIRA_EVENT_H_
#define AKIRA_EVENT_H_

#include<cstdio>
#include<vector>
#include<map>

/*
 *		AEvent
 *
 * A interface Event that to 
 */

typedef int EVENTTYPE;

#ifdef _DEBUG
#define MyDbgPrint(Fmt,...)				\
		puts("[EventHandle]");			\
		printf(Fmt, __VA_ARGS__);		\
		puts("");
#else
define MyDbgPrint(Fmt,...)				\
		puts("[Release]");				\
		printf(Fmt, __VA_ARGS__);		\
		puts("");
#endif

namespace EventHandle{
	class AEventContainer;
	class AEventSubscriber;
	class AEventPublisher;
	class AEvent{
	public:
		AEvent(int eventUUID, EVENTTYPE type){
			eventUUID = -1;
			typeId = type;
		}
		EVENTTYPE GetAEventType(){
			return typeId;
		};
		int GetAEventID() {
			return eventUUID;
		}
		void SetAEventType(EVENTTYPE type){
			typeId = type;
		}
	private:
		// record the event UUID
		int eventUUID;
		EVENTTYPE typeId;
	};

	class AEventHandler{
	public:
		/*
		 *		 OnEventTrigger
		 *	
		 * When this event happen, it will trigger
		 * 
		 */
		AEventHandler(){
			typeId = -1;
			eventHandlerId = -1;
		}

		AEventHandler(int eventhandler_id, EVENTTYPE typeId){
			this->typeId = typeId;
			this->eventHandlerId = eventhandler_id;
		}
		virtual bool OnEventTrigger(void *Context);
		/*
		 *		 OnEventCondition
		 *	
		 * To filter whether this event has happened
		 * This function can check all context in this game, so
		 * it can judge whether this event will happen
		 * 
		 */
		// bool OnCondition(void* Context);
		EVENTTYPE GetAEventTypeID(){
			return typeId;
		}
		int GetAEventHandlerID(){
			return eventHandlerId;
		}
	private:
		EVENTTYPE typeId;
		int eventHandlerId;

	};

	class AEventSubscriber{
	public:
		/*		subscriber
		 *
		 * This function will subscribe the AEvent into the queue,
		 * and it will make the AEvent in queue.
		 * The AEvent must be subscriber before anyone to use it
		 *
		 * @param AEventContainer is a container which contain all the AEvent in one queue,
		 * and it will trigger when call @notify.
		 * @param AEventHandler is an event that include "uuid for event" and "event trigger"
		 * @return a container uuid, which can help to remove specified event
		 */
		static int subscribe(AEventContainer*, AEventHandler*);
		/*		unsubscriber
		 *
		 * This function will unsubscribe the AEvent into the queue,
		 *
		 * @param AEventContainer is a container which contain all the AEvent in one queue,
		 * @param AEventHandler is an event that include "uuid for event" and "event trigger"
		 * @return a container uuid, which can help to remove specified event
		 */
		static int unsubscribe(AEventContainer*, AEventHandler);
	};
	
	class AEventPublisher {
	public:
		/*		publisher
		 *
		 * This function will publish the eventh to the queue
		 *
		 * @param AEventContainer is a container which contain all the AEvent in one queue,
		 * @param AEventHandler is an event that include "uuid for event" and "event trigger"
		 * @param Context is argument for trigger
		 * @return the number of the event trigger
		 */
		static int publish(AEventContainer*, AEvent, void* Context);
	};

	class AEventContainer {
	public:
		AEventContainer(){
			eventUid = 0;
		}
		// add event to the map
		// return the conainer uuid for this AEvent to help
		// to remove event
		int AddEventHandler(AEventHandler* aEventPtr);

		// remove event from the map
		// return true if remove successfuly
		bool RemoveEventHandler(AEventHandler eventHandler);

		// get event
		std::vector<AEventHandler*> *GetAEventHandler(AEvent);
	private:
		/*
		 *		 EventMap
		 *	
		 * Using this map to record Event Type and event queue.And this queue
		 * is used to record all event that publisher publish and subscriber to subscribe
		 * 
		 */
		unsigned long eventUid;
		std::map<EVENTTYPE, std::vector<AEventHandler*>*> mapEventHandler;

	};
}
#endif AKIRA_EVENT_H_