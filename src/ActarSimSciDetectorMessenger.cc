 ///////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez Pol 
//*-- Date: 04/2008
//*-- Last Update: 07/01/15
// --------------------------------------------------------------
// Description:
//   Messenger of the Scintillator detector
//
// --------------------------------------------------------------
// Comments:
//
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include "ActarSimSciDetectorMessenger.hh"

#include "G4RunManager.hh"

#include "ActarSimSciDetectorConstruction.hh"
#include "ActarSimPrimaryGeneratorAction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"



ActarSimSciDetectorMessenger::
ActarSimSciDetectorMessenger(ActarSimSciDetectorConstruction* ActarSimSciDet)
  :ActarSimSciDetector(ActarSimSciDet) { 
  //
  // Constructor with fully functionality
  //
  
  detDir = new G4UIdirectory("/ActarSim/det/sci/");
  detDir->SetGuidance("scintillator detector control");
 
  printCmd = new G4UIcmdWithoutParameter("/ActarSim/det/sci/print",this);
  printCmd->SetGuidance("Prints geometry.");
  printCmd->AvailableForStates(G4State_Idle);

  sideCoverageCmd = new G4UIcmdWithAnInteger("/ActarSim/det/sci/sideCoverage",this);
  sideCoverageCmd->SetGuidance("Selects the scintillator coverage (default 1).");
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

  xBoxHalfLengthCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/sci/xBoxHalfLength",this);
  xBoxHalfLengthCmd->SetGuidance("Sets the x half length of the scintillator detectors box");
  xBoxHalfLengthCmd->SetParameterName("xBoxSci",false);
  xBoxHalfLengthCmd->SetRange("xBoxSci>=0.");
  xBoxHalfLengthCmd->SetUnitCategory("Length");
  xBoxHalfLengthCmd->SetDefaultValue(0.5);
  xBoxHalfLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  yBoxHalfLengthCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/sci/yBoxHalfLength",this);
  yBoxHalfLengthCmd->SetGuidance("Sets the y half length of the scintillator detectors box");
  yBoxHalfLengthCmd->SetParameterName("yBoxSci",false);
  yBoxHalfLengthCmd->SetRange("yBoxSci>=0.");
  yBoxHalfLengthCmd->SetUnitCategory("Length");
  yBoxHalfLengthCmd->SetDefaultValue(0.5);
  yBoxHalfLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  zBoxHalfLengthCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/sci/zBoxHalfLength",this);
  zBoxHalfLengthCmd->SetGuidance("Sets the z half length of the scintillator detectors box");
  zBoxHalfLengthCmd->SetParameterName("zBoxSci",false);
  zBoxHalfLengthCmd->SetRange("zBoxSci>=0.");
  zBoxHalfLengthCmd->SetUnitCategory("Length");
  zBoxHalfLengthCmd->SetDefaultValue(0.5);
  zBoxHalfLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}


ActarSimSciDetectorMessenger::~ActarSimSciDetectorMessenger() {
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


void ActarSimSciDetectorMessenger::SetNewValue(G4UIcommand* command,
					       G4String newValue) {
  //
  // Setting the new values and connecting to actions
  //

  if( command == printCmd )
    ActarSimSciDetector->PrintDetectorParameters(); 

  if( command == sideCoverageCmd)
    ActarSimSciDetector->SetSideCoverage(sideCoverageCmd->GetNewIntValue(newValue));

  if( command == xBoxHalfLengthCmd)
    ActarSimSciDetector->SetXBoxSciHalfLength(xBoxHalfLengthCmd->GetNewDoubleValue(newValue));

  if( command == yBoxHalfLengthCmd)
    ActarSimSciDetector->SetYBoxSciHalfLength(yBoxHalfLengthCmd->GetNewDoubleValue(newValue));

  if( command == zBoxHalfLengthCmd)
    ActarSimSciDetector->SetZBoxSciHalfLength(zBoxHalfLengthCmd->GetNewDoubleValue(newValue));

}

