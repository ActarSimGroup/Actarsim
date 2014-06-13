 ///////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez Pol    hapolyo@usc.es
//*-- Date: 04/2008
//*-- Last Update: 
// --------------------------------------------------------------
// Description:
//   Messenger of the Silicon detector
//
// --------------------------------------------------------------
// Comments:
//
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include "ActarSimParisDetectorMessenger.hh"

#include "G4RunManager.hh"

#include "ActarSimParisDetectorConstruction.hh"
#include "ActarSimPrimaryGeneratorAction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"



ActarSimParisDetectorMessenger::
ActarSimParisDetectorMessenger(ActarSimParisDetectorConstruction* ActarSimParisDet)
  :ActarSimParisDetector(ActarSimParisDet) { 
  //
  // Constructor with fully functionality
  //

  detDir = new G4UIdirectory("/ActarSim/det/paris/");
  detDir->SetGuidance("Paris detector control");

  printCmd = new G4UIcmdWithoutParameter("/ActarSim/det/paris/print",this);
  printCmd->SetGuidance("Prints geometry.");
  printCmd->AvailableForStates(G4State_Idle);

  sideCoverageCmd = new G4UIcmdWithAnInteger("/ActarSim/det/paris/sideCoverage",this);
  sideCoverageCmd->SetGuidance("Selects the silicon coverage (default 1).");
  sideCoverageCmd->SetGuidance(" 6 bits to indicate which sci wall is present (1) or absent (0). The order is: ");
  sideCoverageCmd->SetGuidance(" bit1 (lsb) beam output wall  ");
  sideCoverageCmd->SetGuidance(" bit2 lower (gravity based) wall ");
  sideCoverageCmd->SetGuidance(" bit3 upper (gravity based) wall ");
  sideCoverageCmd->SetGuidance(" bit4 left (from beam point of view) wall ");
  sideCoverageCmd->SetGuidance(" bit5 right (from beam point of view) wall ");
  sideCoverageCmd->SetGuidance(" bit6 (msb) beam entrance wall ");
  sideCoverageCmd->SetGuidance(" Convert the final binary to a decimal number (betwee 0 and 63) and set the coverage! ");
  sideCoverageCmd->SetParameterName("type",false);
  sideCoverageCmd->AvailableForStates(G4State_Idle);

  xBoxHalfLengthCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/paris/xBoxHalfLength",this);
  xBoxHalfLengthCmd->SetGuidance("Sets the x half length of the silicon detectors box");
  xBoxHalfLengthCmd->SetParameterName("xBoxParis",false);
  xBoxHalfLengthCmd->SetRange("xBoxParis>=0.");
  xBoxHalfLengthCmd->SetUnitCategory("Length");
  xBoxHalfLengthCmd->SetDefaultValue(0.5);
  xBoxHalfLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  yBoxHalfLengthCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/paris/yBoxHalfLength",this);
  yBoxHalfLengthCmd->SetGuidance("Sets the y half length of the silicon detectors box");
  yBoxHalfLengthCmd->SetParameterName("yBoxParis",false);
  yBoxHalfLengthCmd->SetRange("yBoxParis>=0.");
  yBoxHalfLengthCmd->SetUnitCategory("Length");
  yBoxHalfLengthCmd->SetDefaultValue(0.5);
  yBoxHalfLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  zBoxHalfLengthCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/paris/zBoxHalfLength",this);
  zBoxHalfLengthCmd->SetGuidance("Sets the z half length of the silicon detectors box");
  zBoxHalfLengthCmd->SetParameterName("zBoxParis",false);
  zBoxHalfLengthCmd->SetRange("zBoxParis>=0.");
  zBoxHalfLengthCmd->SetUnitCategory("Length");
  zBoxHalfLengthCmd->SetDefaultValue(0.5);
  zBoxHalfLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}


ActarSimParisDetectorMessenger::~ActarSimParisDetectorMessenger() {
  //
  // Destructor
  //
  delete detDir;
  delete printCmd;
  delete sideCoverageCmd;
  delete xBoxHalfLengthCmd;
  delete yBoxHalfLengthCmd;
  delete zBoxHalfLengthCmd;
}


void ActarSimParisDetectorMessenger::SetNewValue(G4UIcommand* command,
					    G4String newValue) {
  //
  // Setting the new values and connecting to actions
  //

  if( command == printCmd )
    ActarSimParisDetector->PrintDetectorParameters(); 

  if( command == sideCoverageCmd)
    ActarSimParisDetector->SetSideCoverage(sideCoverageCmd->GetNewIntValue(newValue));

  if( command == xBoxHalfLengthCmd)
    ActarSimParisDetector->SetXBoxParisHalfLength(xBoxHalfLengthCmd->GetNewDoubleValue(newValue));

  if( command == yBoxHalfLengthCmd)
    ActarSimParisDetector->SetYBoxParisHalfLength(yBoxHalfLengthCmd->GetNewDoubleValue(newValue));

  if( command == zBoxHalfLengthCmd)
    ActarSimParisDetector->SetZBoxParisHalfLength(zBoxHalfLengthCmd->GetNewDoubleValue(newValue));
}

