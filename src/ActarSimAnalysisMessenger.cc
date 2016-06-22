// - AUTHOR: Hector Alvarez-Pol 11/2004
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/
//////////////////////////////////////////////////////////////////
/// \class ActarSimAnalysisMessenger
/// Messenger for the analysis interaction
/////////////////////////////////////////////////////////////////

#include "ActarSimAnalysisMessenger.hh"

#include "ActarSimROOTAnalysis.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "globals.hh"

//////////////////////////////////////////////////////////////////
/// Constructor
/// command included in this AnalysisMessenger:
/// - /ActarSim/analControl/storeTracks
/// - /ActarSim/analControl/storeTrackHistos
/// - /ActarSim/analControl/storeEvents
/// - /ActarSim/analControl/storeSimpleTracks
/// - /ActarSim/analControl/storeHistograms
/// - /ActarSim/analControl/setMinStrideLength
ActarSimAnalysisMessenger::ActarSimAnalysisMessenger(ActarSimROOTAnalysis* analEx)
  :analExample(analEx) {
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

  storeEventsCmd = new G4UIcmdWithAString("/ActarSim/analControl/storeEvents",this);
  storeEventsCmd->SetGuidance("Store the events in the output Tree");
  storeEventsCmd->SetGuidance("  Choice : on, off(default)");
  storeEventsCmd->SetParameterName("choice",true);
  storeEventsCmd->SetDefaultValue("off");
  storeEventsCmd->SetCandidates("on off");
  storeEventsCmd->AvailableForStates(G4State_Idle);

  storeSimpleTracksCmd = new G4UIcmdWithAString("/ActarSim/analControl/storeSimpleTracks",this);
  storeSimpleTracksCmd->SetGuidance("Store the simple tracks in the output Tree");
  storeSimpleTracksCmd->SetGuidance("  Choice : on, off(default)");
  storeSimpleTracksCmd->SetParameterName("choice",true);
  storeSimpleTracksCmd->SetDefaultValue("off");
  storeSimpleTracksCmd->SetCandidates("on off");
  storeSimpleTracksCmd->AvailableForStates(G4State_Idle);

  storeHistosCmd = new G4UIcmdWithAString("/ActarSim/analControl/storeHistograms",this);
  storeHistosCmd->SetGuidance("Store histograms in the output Tree");
  storeHistosCmd->SetGuidance("  Choice : on, off(default)");
  storeHistosCmd->SetParameterName("choice",true);
  storeHistosCmd->SetDefaultValue("off");
  storeHistosCmd->SetCandidates("on off");
  storeHistosCmd->AvailableForStates(G4State_Idle);

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

//////////////////////////////////////////////////////////////////
/// Destructor
ActarSimAnalysisMessenger::~ActarSimAnalysisMessenger() {
  delete storeTracksCmd;
  delete storeTrackHistosCmd;
  delete setMinStrideLengthCmd;
  delete storeEventsCmd;
  delete storeSimpleTracksCmd;
  delete storeHistosCmd;
  delete analDir;
}

//////////////////////////////////////////////////////////////////
/// Setting the values using the ActarSimROOTAnalysis interface
void ActarSimAnalysisMessenger::SetNewValue(G4UIcommand* command,
					    G4String newValue) {

  if(command == storeTracksCmd)
    analExample->SetStoreTracksFlag(newValue);

  if(command == storeTrackHistosCmd)
    analExample->SetStoreTrackHistosFlag(newValue);

  if(command == storeEventsCmd)
    analExample->SetStoreEventsFlag(newValue);

  if(command == storeSimpleTracksCmd)
    analExample->SetStoreSimpleTracksFlag(newValue);

  if(command == storeHistosCmd)
    analExample->SetStoreHistogramsFlag(newValue);

  if(command == setMinStrideLengthCmd)
    analExample->SetMinStrideLength(setMinStrideLengthCmd->GetNewDoubleValue(newValue));
}
