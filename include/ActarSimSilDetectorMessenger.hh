/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez Pol
//*-- Date: 04/2008
//*-- Last Update: 07/01/15
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

#ifndef ActarSimSilDetectorMessenger_h
#define ActarSimSilDetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class ActarSimSilDetectorConstruction;
class ActarSimPrimaryGeneratorAction;

class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWith3VectorAndUnit;


class ActarSimSilDetectorMessenger: public G4UImessenger {

private:
  ActarSimSilDetectorConstruction* ActarSimSilDetector;

  G4UIdirectory*             detDir;
  G4UIcmdWithoutParameter*   printCmd;
  G4UIcmdWithAnInteger*      sideCoverageCmd;
  G4UIcmdWithADoubleAndUnit* xBoxHalfLengthCmd;
  G4UIcmdWithADoubleAndUnit* yBoxHalfLengthCmd;
  G4UIcmdWithADoubleAndUnit* zBoxHalfLengthCmd;

public:
  ActarSimSilDetectorMessenger(ActarSimSilDetectorConstruction* );
  ~ActarSimSilDetectorMessenger();

  void SetNewValue(G4UIcommand*, G4String);
  //G4String GetCurrentValue(G4UIcommand*);
};
#endif

