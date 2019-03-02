#include"AEvent.h"

bool EventHandle::AEventHandler::OnEventTrigger(void *Context){
	return true;
}

// ============== Eventh Container ========================
int EventHandle::AEventContainer::AddEventHandler(AEventHandler* aEventPtr){
	if(mapEventHandler.find(aEventPtr->GetAEventTypeID()) == mapEventHandler.end()){
		// now we add new Event
		MyDbgPrint("add new EventType");
		mapEventHandler[aEventPtr->GetAEventTypeID()] = new std::vector<AEventHandler*>();
	}
	mapEventHandler[aEventPtr->GetAEventTypeID()]->push_back(aEventPtr);
	return eventUid++;
}

bool EventHandle::AEventContainer::RemoveEventHandler(AEventHandler eventHandler){
	// MyDbgPrint("[TODO] RemoveEvent Hanlder was not finished");
	std::vector<EventHandle::AEventHandler*> * eventQueue = NULL;
	AEventHandler* ptrEventHandler = NULL;
	eventQueue = this->mapEventHandler[eventHandler.GetAEventTypeID()];
	if(eventQueue == NULL){
		MyDbgPrint("the event we want to get is empty");
	}
	std::vector<EventHandle::AEventHandler*>::iterator it;
	for(it = eventQueue->begin(); it != eventQueue->end(); it++){
		if((*it)->GetAEventHandlerID() == eventHandler.GetAEventHandlerID()){
			ptrEventHandler = *it;
			eventQueue->erase(it);
			MyDbgPrint("event handler %x has been erase", ptrEventHandler->GetAEventHandlerID());
			delete ptrEventHandler;
			break;
		}
	}

	MyDbgPrint("we could not find the eventHandler");
	return false;
}

std::vector<EventHandle::AEventHandler*> *EventHandle::AEventContainer::GetAEventHandler(AEvent aevent){
	std::vector<EventHandle::AEventHandler*> * eventQueue = NULL;
	eventQueue = this->mapEventHandler[aevent.GetAEventType()];
	if(eventQueue == NULL){
		MyDbgPrint("the event we want to get is empty");
	}
	return eventQueue;
	
}
// ============== Eventh Subscriber =====================
int EventHandle::AEventSubscriber::subscribe(AEventContainer* container, AEventHandler* ptrEventHandler){
	
	if(container == NULL){
		MyDbgPrint("It's an empty container..\n");
		return -1;
	}
	// AEventHandler* ptrEventHandler = new AEventHandler(eventHandler);
	return container->AddEventHandler(ptrEventHandler);
}

int EventHandle::AEventSubscriber::unsubscribe(AEventContainer* container, AEventHandler eventHandler) {
	// TODO:Finish this api
	return -1;
}
// ============== Eventh Publisher =====================
int EventHandle::AEventPublisher::publish(AEventContainer* container, AEvent aevent){

	if(container == NULL){
		MyDbgPrint("It's an empty container in publish..\n");
		return -1;
	}
	int dwTriggerNum = 0;
	std::vector<AEventHandler*>* tmp = container->GetAEventHandler(aevent);
	if(tmp != NULL){
		for(int i = 0; i < tmp->size(); i++){
			AEventHandler* eachEventHandler = (*tmp)[i];
			if(eachEventHandler == NULL){
				MyDbgPrint("event handler %x is NULL", eachEventHandler->GetAEventHandlerID());
				return -1;
			}
			eachEventHandler->OnEventTrigger(NULL);
		}
	}
}