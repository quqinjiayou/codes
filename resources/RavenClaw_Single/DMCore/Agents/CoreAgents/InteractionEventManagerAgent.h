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
// INTERACTIONEVENTMANAGERAGENT.H - handles interaction events such as
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

#pragma once
#ifndef __INTERACTIONEVENTMANAGERAGENT_H__
#define __INTERACTIONEVENTMANAGERAGENT_H__

#include <windows.h>			// needed for handle to newInputEvent

#include "DMCore/Events/InteractionEvent.h"
#include "Utils/Utils.h"
#include "DMCore/Agents/Agent.h"

#define IET_DIALOG_STATE_CHANGE	"dialog_state_change"
#define IET_USER_UTT_START	"user_utterance_start"
#define IET_USER_UTT_END	"user_utterance_end"
#define IET_PARTIAL_USER_UTT "partial_user_utterance"
#define IET_SYSTEM_UTT_START	"system_utterance_start"
#define IET_SYSTEM_UTT_END	"system_utterance_end"
#define IET_SYSTEM_UTT_CANCELED	"system_utterance_canceled"
#define IET_FLOOR_OWNER_CHANGES "floor_owner_changes"
#define IET_SESSION "session"
#define IET_GUI "gui"

//-----------------------------------------------------------------------------
// CInteractionEventManagerAgent Class - 
//   This class is an agent which handles events from the Interaction Manager
//-----------------------------------------------------------------------------
class CInteractionEventManagerAgent: public CAgent {

private:
	//---------------------------------------------------------------------
	// Private members
	//---------------------------------------------------------------------
	//

    // handle for a new interaction event signal
	HANDLE hNewInteractionEvent;                     
	
	// queue of current (unprocessed) events
	queue <CInteractionEvent*, list<CInteractionEvent*> > qpieEventQueue;

	// history of past events
	vector <CInteractionEvent*> vpieEventHistory;

	// pointer to most recently processed event
	CInteractionEvent *pieLastEvent;

	// pointer to most recently processed user input
	CInteractionEvent *pieLastInput;

public:

	//---------------------------------------------------------------------
	// Constructor and destructor
	//---------------------------------------------------------------------
	//
	// Default constructor
	CInteractionEventManagerAgent(string sAName, 
					   string sAConfiguration = "",
					   string sAType = "CAgent:CInteractionEventManagerAgent");

	// Virtual destructor
	virtual ~CInteractionEventManagerAgent();

	// static function for dynamic agent creation
	static CAgent* AgentFactory(string sAName, string sAConfiguration);

	//---------------------------------------------------------------------
	// CAgent Class overwritten methods 
	//---------------------------------------------------------------------
	//
	virtual void Reset();	// overwritten Reset

	virtual void Initialize();	// overwritten Initialize

public:

	//---------------------------------------------------------------------
	// InteractionEventManagerAgent class specific public methods
	//---------------------------------------------------------------------

	// Indicates if the queue contains at least one event
	bool HasEvent();

	// Dequeues one event from the unprocessed event queue
	CInteractionEvent *GetNextEvent();

	// Returns a pointer to the last event/user input processed
	CInteractionEvent *GetLastEvent();
	CInteractionEvent *GetLastInput();

	void SetLastEvent(CInteractionEvent * e) {
		pieLastEvent = e;
		pieLastInput = e;

	}

	// Checks if the last event matches a given expectation
	bool LastEventMatches(string sGrammarExpectation);

	// Checks if the current input matches a given expectation
	bool LastInputMatches(string sGrammarExpectation);

    // Checks if the current event is a complete or partial event
    bool LastEventIsComplete();

    // Returns the confidence score on the current input
    float GetLastEventConfidence();

	// Returns the string value corresponding to a given expectation from 
	// the current input 
	string GetValueForExpectation(string sGrammarExpectation);

	// Waits for an interaction event to arrive from the Interaction Manager
	void WaitForEvent();

	// Used by the Galaxy Bridge to signal that a new event has arrived
	void SignalInteractionEventArrived();

};

#endif // __INTERACTIONEVENTMANAGERAGENT_H__