/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol
//*-- Date: 11/2004
//*-- Last Update: 10/01/16
// --------------------------------------------------------------
// Description:
//   Messenger for the analysis interaction
//
// --------------------------------------------------------------
// Comments:
//
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include "ActarSimAnalysisMessenger.hh"

#include "ActarSimROOTAnalysis.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "globals.hh"


ActarSimAnalysisMessenger::ActarSimAnalysisMessenger(ActarSimROOTAnalysis* analEx)
:analExample(analEx) {
  //
  // Constructor
  //
  analDir = new G4UIdirectory("/ActarSim/analControl/");
  analDir->SetGuidance("Analysis control");

  storeTracksCmd = new G4UIcmdWithAString("/ActarSim/analControl/storeTracks",this);
  storeTracksCmd->SetGuidance("Store the tracks in the output Tree");
  storeTracksCmd->SetGuidance("  Choice : on, off(default)");
  storeTracksCmd->SetParameterName("choice",true);
  storeTracksCmd->SetDefaultValue("off");
  storeTracksCmd->SetCandidates("on off");
  storeTracksCmd->AvailableForStates(G4State_Idle);

  storeTrackHistosCmd = new G4UIcmdWithAString("/ActarSim/analControl/storeTrackHistos",this);
  storeTrackHistosCmd->SetGuidance("Store the tracks in Histograms");
  storeTrackHistosCmd->SetGuidance("  Choice : on, off(default)");
  storeTrackHistosCmd->SetParameterName("choice",true);
  storeTrackHistosCmd->SetDefaultValue("off");
  storeTrackHistosCmd->SetCandidates("on off");
  storeTrackHistosCmd->AvailableForStates(G4State_Idle);

  storeEventsCmd = new G4UIcmdWithAString("/ActarSim/analControl/storeEvents",this); // new dypang 080301
  storeEventsCmd->SetGuidance("Store the events in the output Tree");
  storeEventsCmd->SetGuidance("  Choice : on, off(default)");
  storeEventsCmd->SetParameterName("choice",true);
  storeEventsCmd->SetDefaultValue("off");
  storeEventsCmd->SetCandidates("on off");
  storeEventsCmd->AvailableForStates(G4State_Idle);

  storeSimpleTracksCmd = new G4UIcmdWithAString("/ActarSim/analControl/storeSimpleTracks",this); // new 080301
  storeSimpleTracksCmd->SetGuidance("Store the simple tracks in the output Tree");
  storeSimpleTracksCmd->SetGuidance("  Choice : on, off(default)");
  storeSimpleTracksCmd->SetParameterName("choice",true);
  storeSimpleTracksCmd->SetDefaultValue("off");
  storeSimpleTracksCmd->SetCandidates("on off");
  storeSimpleTracksCmd->AvailableForStates(G4State_Idle);

  storeHistosCmd = new G4UIcmdWithAString("/ActarSim/analControl/storeHistograms",this); // new dypang 080301
  storeHistosCmd->SetGuidance("Store histograms in the output Tree");
  storeHistosCmd->SetGuidance("  Choice : on, off(default)");
  storeHistosCmd->SetParameterName("choice",true);
  storeHistosCmd->SetDefaultValue("off");
  storeHistosCmd->SetCandidates("on off");
  storeHistosCmd->AvailableForStates(G4State_Idle);

  //DPL 28Nov2012
  setMinStrideLengthCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/analControl/setMinStrideLength",this);
  setMinStrideLengthCmd->SetGuidance("Sets the minimum value for the stride length.");
  setMinStrideLengthCmd->SetGuidance("Specifies the minimum length of the strides in the simpleTracks approximation.");
  setMinStrideLengthCmd->SetGuidance("  Choice : on, off(default)");
  setMinStrideLengthCmd->SetParameterName("length",false);
  setMinStrideLengthCmd->SetRange("length>=0.");
  setMinStrideLengthCmd->SetUnitCategory("Length");
  setMinStrideLengthCmd->SetDefaultValue(1.);
  setMinStrideLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}


ActarSimAnalysisMessenger::~ActarSimAnalysisMessenger() {
  //
  // Destructor
  //
  delete storeTracksCmd;
  delete storeTrackHistosCmd;
  delete setMinStrideLengthCmd;
  delete storeEventsCmd;       // new dypang 080301
  delete storeSimpleTracksCmd; // new dypang 080301
  delete storeHistosCmd;       // new dypang 080301
  delete analDir;
}


void ActarSimAnalysisMessenger::SetNewValue(G4UIcommand* command,
					    G4String newValue) {
  //
  // Setting the values from the interfaces
  //
  if(command == storeTracksCmd)
    {analExample->SetStoreTracksFlag(newValue);}

  if(command == storeTrackHistosCmd)
    {analExample->SetStoreTrackHistosFlag(newValue);}

  if(command == storeEventsCmd) // new dypang 080301
    {analExample->SetStoreEventsFlag(newValue);}

  if(command == storeSimpleTracksCmd) // new dypang 080301
    {analExample->SetStoreSimpleTracksFlag(newValue);}

  if(command == storeHistosCmd) // new dypang 080301
    {analExample->SetStoreHistogramsFlag(newValue);}

  //DPL 28Nov2012
  if(command == setMinStrideLengthCmd)
    {analExample->SetMinStrideLength(setMinStrideLengthCmd->GetNewDoubleValue(newValue));}

}
