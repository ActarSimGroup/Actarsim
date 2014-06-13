/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez Pol    hapolyo@usc.es
//*-- Date: 04/2008
//*-- Last Update: 
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

#ifndef ActarSimSciDetectorMessenger_h
#define ActarSimSciDetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class ActarSimSciDetectorConstruction;
class ActarSimPrimaryGeneratorAction;

class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWith3VectorAndUnit;


class ActarSimSciDetectorMessenger: public G4UImessenger {

private:
  ActarSimSciDetectorConstruction* ActarSimSciDetector;
  
  G4UIdirectory*             detDir;
  G4UIcmdWithoutParameter*   printCmd;
  G4UIcmdWithAnInteger*      sideCoverageCmd;
  G4UIcmdWithADoubleAndUnit* xBoxHalfLengthCmd;
  G4UIcmdWithADoubleAndUnit* yBoxHalfLengthCmd;
  G4UIcmdWithADoubleAndUnit* zBoxHalfLengthCmd;

public:
  ActarSimSciDetectorMessenger(ActarSimSciDetectorConstruction* );
  ~ActarSimSciDetectorMessenger();
  
  void SetNewValue(G4UIcommand*, G4String);
  //G4String GetCurrentValue(G4UIcommand*);
};
#endif

