// - AUTHOR: Hector Alvarez-Pol 11/2004
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/
//////////////////////////////////////////////////////////////////
/// \class ActarSimEventActionMessenger
/// Messenger for the event actions
/////////////////////////////////////////////////////////////////

#include "ActarSimEventActionMessenger.hh"

#include "ActarSimEventAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "globals.hh"

//////////////////////////////////////////////////////////////////
/// Constructor
/// command included in this AnalysisMessenger:
/// - /ActarSim/event/drawTracks
/// - /ActarSim/event/printModulo
ActarSimEventActionMessenger::ActarSimEventActionMessenger(ActarSimEventAction* EvAct)
  :eventAction(EvAct) {
  eventDir = new G4UIdirectory("/ActarSim/event/");
  eventDir->SetGuidance("event control");

  DrawCmd = new G4UIcmdWithAString("/ActarSim/event/drawTracks",this);
  DrawCmd->SetGuidance("Draw the tracks in the event");
  DrawCmd->SetGuidance("  Choice : none, charged, neutral, all(default)");
  DrawCmd->SetParameterName("choice",true);
  DrawCmd->SetDefaultValue("all");
  DrawCmd->SetCandidates("none charged neutral all");
  DrawCmd->AvailableForStates(G4State_Idle);

  PrintCmd = new G4UIcmdWithAnInteger("/ActarSim/event/printModulo",this);
  PrintCmd->SetGuidance("Print events modulo n");
  PrintCmd->SetParameterName("EventNb",false);
  PrintCmd->SetRange("EventNb>0");
  PrintCmd->AvailableForStates(G4State_Idle);
}

//////////////////////////////////////////////////////////////////
/// Destructor
ActarSimEventActionMessenger::~ActarSimEventActionMessenger() {
  delete DrawCmd;
  delete PrintCmd;
  delete eventDir;
}

//////////////////////////////////////////////////////////////////
/// Setting the values using the ActarSimEventAction interface
void ActarSimEventActionMessenger::SetNewValue(G4UIcommand* command,G4String newValue) {
  //
  // Setting the values from the interfaces
  //
  if(command == DrawCmd)
    eventAction->SetDrawFlag(newValue);

  if(command == PrintCmd)
    eventAction->SetPrintModulo(PrintCmd->GetNewIntValue(newValue));
}
