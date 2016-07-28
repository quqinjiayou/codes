//=============================================================================
//
//   Copyright (c) 2000-2005, Carnegie Mellon University.  
//   All rights reserved.
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
//
//   1. Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer. 
//
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in
//      the documentation and/or other materials provided with the
//      distribution.
//
//   This work was supported in part by funding from the Defense Advanced 
//   Research Projects Agency and the National Science Foundation of the 
//   United States of America, and the CMU Sphinx Speech Consortium.
//
//   THIS SOFTWARE IS PROVIDED BY CARNEGIE MELLON UNIVERSITY ``AS IS'' AND 
//   ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
//   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//   PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY
//   NOR ITS EMPLOYEES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
//   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//=============================================================================

//-----------------------------------------------------------------------------
// 
// INTERACTIONEVENTMANAGERAGENT.CPP - handles interaction events such as
//								prompt delivery notification, new user inputs,
//								barge-in...
// 
// ----------------------------------------------------------------------------
// 
// BEFORE MAKING CHANGES TO THIS CODE, please read the appropriate 
// documentation, available in the Documentation folder. 
//
// ANY SIGNIFICANT CHANGES made should be reflected back in the documentation
// file(s)
//
// ANY CHANGES made (even small bug fixes, should be reflected in the history
// below, in reverse chronological order
// 
// HISTORY --------------------------------------------------------------------
//
//   [2005-11-07] (antoine): added LastEventIsComplete to handle partial and
//                           complete events
//   [2005-06-21] (antoine): started this
//
//-----------------------------------------------------------------------------

#include "DMInterfaces/DMInterface.h"
#include "DMCore/Core.h"

#include "InteractionEventManagerAgent.h"

#include "DMCore/Events/GalaxyInteractionEvent.h"

//---------------------------------------------------------------------
// Constructor and destructor
//---------------------------------------------------------------------
//
// A: Default constructor
CInteractionEventManagerAgent::CInteractionEventManagerAgent(string sAName,
									   string sAConfiguration,  
									   string sAType) : 
	CAgent(sAName, sAConfiguration, sAType) {

	// create a "NewInput" event to handle communication across the threads
	hNewInteractionEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(hNewInteractionEvent != NULL);
}
					   
// A: destructor
CInteractionEventManagerAgent::~CInteractionEventManagerAgent() {
	CloseHandle(hNewInteractionEvent);
}

// A: static function for dynamic agent creation
CAgent* CInteractionEventManagerAgent::AgentFactory(string sAName, 
													string sAConfiguration) {
	return new CInteractionEventManagerAgent(sAName, sAConfiguration);
}

//---------------------------------------------------------------------
// CAgent Class overwritten methods 
//---------------------------------------------------------------------
//
void CInteractionEventManagerAgent::Reset() {
}

void CInteractionEventManagerAgent::Initialize() {
	pieLastEvent = NULL;
	pieLastInput = NULL;
}

//---------------------------------------------------------------------
// InteractionEventManagerAgent class specific public methods
//---------------------------------------------------------------------

// A: Indicates if the queue has at least one event
bool CInteractionEventManagerAgent::HasEvent() {
	return !qpieEventQueue.empty();
}

// A: Dequeues one event from the queue
CInteractionEvent *CInteractionEventManagerAgent::GetNextEvent() {
	if (!qpieEventQueue.empty()) {
	
		// pops the event from the event queue
		CInteractionEvent *pieNext = qpieEventQueue.front();
		qpieEventQueue.pop();

		// updates pointer to last event processed
		pieLastEvent = pieNext;
		// also if the event was an input
		if (pieNext->GetType() == IET_USER_UTT_END) {
			pieLastInput = pieNext;
		}

		// pushes the event in the history
		vpieEventHistory.push_back(pieNext);

		return pieNext;
	}
	else {
		return NULL;
	}
}

// A: Returns a pointer to the last event processed
CInteractionEvent *CInteractionEventManagerAgent::GetLastEvent() {
	return pieLastEvent;
}

// A: Returns a pointer to the last user input processed
CInteractionEvent *CInteractionEventManagerAgent::GetLastInput() {
	return pieLastInput;
}
	
// A: Check if the last event matches a certain grammar expectation
bool CInteractionEventManagerAgent::LastEventMatches(string sGrammarExpectation) {
	// delegate it to the InteractionEvent class
	return pieLastEvent->Matches(sGrammarExpectation);
}

// A: Check if the last user input matches a certain grammar expectation
bool CInteractionEventManagerAgent::LastInputMatches(string sGrammarExpectation) {
	// delegate it to the InteractionEvent class
	return pieLastInput->Matches(sGrammarExpectation);
}

// A: Check if the last event is a complete or a partial event
bool CInteractionEventManagerAgent::LastEventIsComplete() {
	return pieLastEvent->IsComplete();
}

// A: Returns the confidence score for the last event
float CInteractionEventManagerAgent::GetLastEventConfidence() {
    // delegate it to the Input class
    return pieLastEvent->GetConfidence();
}

// D: Returns the string value of a grammar concept
string CInteractionEventManagerAgent::GetValueForExpectation(string sGrammarExpectation) {
	// delegate it to the Input class
	return pieLastEvent->GetValueForExpectation(sGrammarExpectation);
}

// A: Waits for an interaction event to arrive from the Interaction Manager
void CInteractionEventManagerAgent::WaitForEvent() {

	if (qpieEventQueue.empty()) {
		// retrieve the current thread id
		DWORD dwThreadId = GetCurrentThreadId();

		// send a message to the galaxy interface to wait for input
		PostThreadMessage(g_idDMInterfaceThread, WM_WAITINTERACTIONEVENT, 0, dwThreadId);
		
		// log that we started waiting for an input
		Log(INPUTMANAGER_STREAM, "Waiting for interaction event ...");

		// and then wait for the utterance to appear
		WaitForSingleObject(hNewInteractionEvent, INFINITE);

		// process the new event
		CInteractionEvent *pieEvent = NULL;

#ifdef GALAXY
		// identify the type of event
		string sType = (string)Gal_GetString((Gal_Frame)gfLastEvent, ":event_type");
		Gal_Frame gfEventFrame = Gal_CopyFrame((Gal_Frame)gfLastEvent);

		// create the appropriate event object
		pieEvent = new CGalaxyInteractionEvent(gfEventFrame);

		if (sType == IET_USER_UTT_END) {
			Log(INPUTMANAGER_STREAM, "New user input [User:%s]", 
				pieEvent->GetStringProperty("[uttid]").c_str());
		}
#endif

		// push the event at the end of the event queue
		qpieEventQueue.push(pieEvent);

		// log it
		Log(INPUTMANAGER_STREAM, "New interaction event (%s) arrived "
			"(dumped below)\n%s", 
			sType.c_str(), pieEvent->ToString().c_str()); 

	}

}

// A: Used by the Galaxy Bridge to signal that a new event has arrived
void CInteractionEventManagerAgent::SignalInteractionEventArrived() {
	// signal that the input has appeared
	SetEvent(hNewInteractionEvent);
}

