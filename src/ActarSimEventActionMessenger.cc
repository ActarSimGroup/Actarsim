/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol
//*-- Date: 11/2004
//*-- Last Update: 28/10/05
// --------------------------------------------------------------
// Description:
//   Messenger for the event actions
//
// --------------------------------------------------------------
// Comments:
//   - 03/12/04: based on example/novice/N03 structure
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include "ActarSimEventActionMessenger.hh"

#include "ActarSimEventAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "globals.hh"


ActarSimEventActionMessenger::ActarSimEventActionMessenger(ActarSimEventAction* EvAct)
:eventAction(EvAct) {
  //
  // Constructor
  //
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


ActarSimEventActionMessenger::~ActarSimEventActionMessenger() {
  //
  // Destructor
  //
  delete DrawCmd;
  delete PrintCmd;
  delete eventDir;
}


void ActarSimEventActionMessenger::SetNewValue(
                                        G4UIcommand* command,G4String newValue) {
  //
  // Setting the values from the interfaces
  //
  if(command == DrawCmd)
    {eventAction->SetDrawFlag(newValue);}

  if(command == PrintCmd)
    {eventAction->SetPrintModulo(PrintCmd->GetNewIntValue(newValue));}

}

