/////////////////////////////////////////////////////////////////
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

#ifndef ActarSimParisDetectorMessenger_h
#define ActarSimParisDetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class ActarSimParisDetectorConstruction;
class ActarSimPrimaryGeneratorAction;

class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWith3VectorAndUnit;


class ActarSimParisDetectorMessenger: public G4UImessenger {

private:
  ActarSimParisDetectorConstruction* ActarSimParisDetector;

  G4UIdirectory*             detDir;
  G4UIcmdWithoutParameter*   printCmd;
  G4UIcmdWithAnInteger*      sideCoverageCmd;
  G4UIcmdWithADoubleAndUnit* xBoxHalfLengthCmd;
  G4UIcmdWithADoubleAndUnit* yBoxHalfLengthCmd;
  G4UIcmdWithADoubleAndUnit* zBoxHalfLengthCmd;

public:
  ActarSimParisDetectorMessenger(ActarSimParisDetectorConstruction* );
  ~ActarSimParisDetectorMessenger();

  void SetNewValue(G4UIcommand*, G4String);
  //G4String GetCurrentValue(G4UIcommand*);
};
#endif

