//=============================================================================
//
//   Copyright (c) 2000-2004, Carnegie Mellon University.  
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
// DMCORE.CPP - implements the dialog core thread
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
//   [2003-05-13] (dbohus): changed so that configuration parameters are in a 
//                           hash, which gets also logged
//   [2003-02-14] (dbohus): added pGroundingManager agent
//   [2002-05-25] (dbohus): deemed preliminary stable version 0.5
//   [2002-04-13] (dbohus): moved all functionality out into the DMCoreAgent
//   [2002-01-26] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

#include <SVN_Data.h>
#include <fstream>
#include "DMCore/Core.h"
#include "DialogTask/DialogTask.h"
#include "DMInterfaces/Dialog.h"
#include <json/json.h>
//-----------------------------------------------------------------------------
// Definitions for the dialog core agents
//-----------------------------------------------------------------------------
CDMCoreAgent			*pDMCore;
COutputManagerAgent		*pOutputManager;
CInteractionEventManagerAgent	*pInteractionEventManager;
CTrafficManagerAgent	*pTrafficManager;
CStateManagerAgent		*pStateManager;
CDTTManagerAgent		*pDTTManager;
CGroundingManagerAgent  *pGroundingManager;

//-----------------------------------------------------------------------------
// Dialog core initialization
//-----------------------------------------------------------------------------
void InitializeDialogCore(TRavenClawConfigParams rcpParams) {

	// Log the core initialization parameters
	Log(CORETHREAD_STREAM, "Initializing Core ...");
    Log(CORETHREAD_STREAM, "Core initialization parameters dumped below:\n%s",
        rcpParams.ToString().c_str());

	// Initialize the random number gererator
	_timeb theTime = GetTime();
	srand((unsigned)(theTime.millitm + theTime.time*1000));

	// Create a new Dialog Management Core Agent, and register it
	AgentsRegistry.Clear();
	AgentsRegistry.RegisterAgentType("CDMCoreAgent", 
									  CDMCoreAgent::AgentFactory);
	pDMCore = (CDMCoreAgent*)AgentsRegistry.CreateAgent("CDMCoreAgent", 
														"DMCoreAgent");
	if(!pDMCore) 
		FatalError("Could not create DMCore agent.");
	pDMCore->Initialize();
	pDMCore->Register();
    // set the default core configuration 
    pDMCore->SetDefaultTimeoutPeriod(
        atoi(rcpParams.Get(RCP_DEFAULT_TIMEOUT).c_str()));
    pDMCore->SetDefaultNonunderstandingThreshold(
        (float)atof(
        rcpParams.Get(RCP_DEFAULT_NONUNDERSTANDING_THRESHOLD).c_str()));

	// create all the other dialog core agents
	Log(CORETHREAD_STREAM, "Creating auxiliary core dialog core agents ...");

	// create the interaction event manager
	AgentsRegistry.RegisterAgentType("CInteractionEventManagerAgent", 
									 CInteractionEventManagerAgent::AgentFactory);
	pInteractionEventManager = (CInteractionEventManagerAgent *)
					AgentsRegistry.CreateAgent("CInteractionEventManagerAgent", 
											   "InteractionEventManagerAgent");
	if(!pInteractionEventManager) 
		FatalError("Could not create InteractionEventManager agent.");
	pInteractionEventManager->Initialize();
	pInteractionEventManager->Register();

	// create the output manager
	AgentsRegistry.RegisterAgentType("COutputManagerAgent", 
									 COutputManagerAgent::AgentFactory);
	pOutputManager = (COutputManagerAgent *)
					AgentsRegistry.CreateAgent("COutputManagerAgent", 
											   "OutputManagerAgent");
	if(!pOutputManager) 
		FatalError("Could not create OutputManager agent.");
	pOutputManager->Initialize();
	pOutputManager->Register();

	// create the galaxy stub
	AgentsRegistry.RegisterAgentType("CTrafficManagerAgent", 
									 CTrafficManagerAgent::AgentFactory);
	pTrafficManager = (CTrafficManagerAgent *)
					AgentsRegistry.CreateAgent("CTrafficManagerAgent", 
											   "TrafficManagerAgent");
	if(!pTrafficManager) 
		FatalError("Could not create TrafficManager agent.");
	pTrafficManager->Initialize();
	pTrafficManager->Register();

	// create the state manager
	AgentsRegistry.RegisterAgentType("CStateManagerAgent", 
									 CStateManagerAgent::AgentFactory);
	pStateManager = (CStateManagerAgent *)
					AgentsRegistry.CreateAgent("CStateManagerAgent", 
											   "StateManagerAgent");
	if(!pStateManager) 
		FatalError("Could not create StateManager agent.");
	pStateManager->Initialize();
	pStateManager->Register();
    // set the state broadcast address
	pStateManager->LoadDialogStateNames(rcpParams.Get(RCP_DIALOG_STATES_FILE));

	// create the dialog task tree manager
	AgentsRegistry.RegisterAgentType("CDTTManagerAgent", 
									 CDTTManagerAgent::AgentFactory);
	pDTTManager = (CDTTManagerAgent *)
					AgentsRegistry.CreateAgent("CDTTManagerAgent", 
											   "DTTManagerAgent");
	if(!pDTTManager) 
		FatalError("Could not create DTTManager agent.");
	pDTTManager->Initialize();
	pDTTManager->Register();

	// create the grounding manager
	AgentsRegistry.RegisterAgentType("CGroundingManagerAgent", 
									 CGroundingManagerAgent::AgentFactory);
	pGroundingManager = (CGroundingManagerAgent *)
					      AgentsRegistry.CreateAgent("CGroundingManagerAgent", 
											         "GroundingManagerAgent");
	if(!pGroundingManager) 
		FatalError("Could not create GroundingManager agent.");
	pGroundingManager->Initialize();
	pGroundingManager->Register();    
    // set the configuration
    pGroundingManager->SetConfiguration(
        rcpParams.Get(RCP_GROUNDING_MANAGER_CONFIGURATION));
    // and load the models specifications from the grounding policies file
	if (rcpParams.Get(RCP_GROUNDING_POLICIES) != "") 
		pGroundingManager->LoadPoliciesFromString(rcpParams.Get(RCP_GROUNDING_POLICIES));
	else
		pGroundingManager->LoadPoliciesFromFile(rcpParams.Get(RCP_GROUNDING_POLICIES_FILE));

    Log(CORETHREAD_STREAM, "Auxiliary core dialog management agents "\
						   "created successfully.");

	Log(CORETHREAD_STREAM, "Core initialization completed successfully.");
}

//-----------------------------------------------------------------------------
// Dialog core termination
//-----------------------------------------------------------------------------
void TerminateDialogCore() {
	Log(CORETHREAD_STREAM, "Terminating Core ...");

	// destroy the core dialog management agent
	delete pDMCore;
	pDMCore = NULL;
	// and all the other core agents
	delete pDTTManager;
	pDTTManager = NULL;
	delete pTrafficManager;
	pTrafficManager = NULL;
	delete pOutputManager;
	pOutputManager = NULL;
	delete pInteractionEventManager;
	pInteractionEventManager = NULL;
	delete pStateManager;
	pStateManager = NULL;
    delete pGroundingManager;
	pGroundingManager = NULL;

	// and finally clear up the registry
	AgentsRegistry.Clear();

    // and log that the core terminated successfully
	Log(CORETHREAD_STREAM, "Core terminated successfully.");
}

static const string INPUT_FRAME_SEP_BEGIN = ">>>>>>>>>>>>>>>>>>>>";
static const string INPUT_FRAME_SEP_END = "<<<<<<<<<<<<<<<<<<<<";
bool LoadDialogInput(const char* inputFile, list<DialogInput> listDialogInput) {
	//TODO: fill this function
	ifstream ifs(inputFile);
	if (!ifs) {
		return false;
	}
	Json::Value root;
	Json::Reader reader;
	bool ret = reader.parse(ifs,root,false);
	if (!ret) {
		return false;
	}

	Log(DMCORE_STREAM, "Json:\n%s\n", root.toStyledString().c_str());

	if (!root.isArray()) {
		DialogInput input;
		input.inputJson == root;
		listDialogInput.push_back(input);

	} else {
		for (int i = 0; i < root.size(); ++i) {
			Json::Value jsonElement = root[i];
			DialogInput input;
			input.inputJson = jsonElement;
			listDialogInput.push_back(input);

		}
	}

	ifs.close();
	return true;

}

void ExecuteOnInputFile(const char* inputFileName, const char* outFileName) {
	list<DialogInput> listDialogInput;
	LoadDialogInput(inputFileName, listDialogInput);

	FILE* outFile = fopen(outFileName, "a");
	if (NULL == outFile) {
		return;
	}
	for (list<DialogInput>::iterator it = listDialogInput.begin(); listDialogInput.end() != it; it++ ) {
		DialogInput dlgInput = *it;
		DialogResponse response = pDMCore->Proc(dlgInput);

		fprintf(outFile, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
		fprintf(outFile, response.ToString().c_str());
	}
	fclose(outFile);

}

//-----------------------------------------------------------------------------
// THE DIALOG CORE THREAD FUNCTION
//-----------------------------------------------------------------------------
#define STRING_DM_INPUT_FILE "G:/PROJECTS/java/example-systems/MyFlight/data/input/dm_input.txt"
#define STRING_DM_OUT_FILE "G:/PROJECTS/java/example-systems/MyFlight/data/output/dm_output.txt"
DWORD WINAPI DialogCoreThread(LPVOID pParams) {

	TRavenClawConfigParams rcpParams = *(TRavenClawConfigParams *)pParams;
	InitializeLogging(rcpParams.Get(RCP_LOG_DIR), 
		rcpParams.Get(RCP_LOG_PREFIX), DEFAULT_LOG_FILENAME, 
		rcpParams.Get(RCP_LOGGED_STREAMS), rcpParams.Get(RCP_DISPLAYED_STREAMS),
		rcpParams.Get(RCP_EXIT_ON_FATAL_ERROR));

	// Print out Olympus version stuff
	Log(CORETHREAD_STREAM, "Olympus Branch: %s\nOlympus Revision: %s", 
		OLYMPUS_SVN_BRANCH, OLYMPUS_SVN_REVISION);

	// Initialize the core: create all the core agents
	InitializeDialogCore(rcpParams);	

	// Call the dialog task initialize function 
	DialogTaskOnBeginSession();

	// Do the dialog dance :)
	//pDMCore->Execute();
	ExecuteOnInputFile(STRING_DM_INPUT_FILE, STRING_DM_OUT_FILE);

	// Terminate the dialog core
	TerminateDialogCore();

	// Finally, send a message to signal that this session of the 
	// Dialog Core is over
	PostThreadMessage(g_idDMInterfaceThread, WM_DIALOGFINISHED, 0, 0);
	return 0;
}

/*

int main() {
	ExecuteOnInputFile(STRING_DM_INPUT_FILE, STRING_DM_OUT_FILE);

	return 0;


}

*/