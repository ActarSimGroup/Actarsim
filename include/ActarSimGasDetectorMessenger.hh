/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez Pol    hapolyo@usc.es
//*-- Date: 04/2008
//*-- Last Update: 05/05/08
// --------------------------------------------------------------
// Description:
//   Messenger of the GAS ACTAR detector
//
// --------------------------------------------------------------
// Comments:
//
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#ifndef ActarSimGasDetectorMessenger_h
#define ActarSimGasDetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class ActarSimGasDetectorConstruction;
class ActarSimPrimaryGeneratorAction;

class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADouble;
class G4UIcmdWithoutParameter;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWith3Vector;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithoutParameter;


class ActarSimGasDetectorMessenger: public G4UImessenger {

private:
  ActarSimGasDetectorConstruction* ActarSimGasDetector;
    
  G4UIdirectory*             detDir;

  G4UIcmdWithAString*        gasMaterCmd;
  G4UIcmdWithAString*        beamShieldMaterCmd;

  G4UIcmdWithoutParameter*   printCmd;

  G4UIcmdWithAString*        detectorGeometryCmd;
  G4UIcmdWithAString*        luciteBoxCmd;
  G4UIcmdWithAString*        setBeamShieldCmd;
  G4UIcmdWithADoubleAndUnit* xGasBoxCmd;
  G4UIcmdWithADoubleAndUnit* yGasBoxCmd;
  G4UIcmdWithADoubleAndUnit* zGasBoxCmd;
  G4UIcmdWithADoubleAndUnit* radiusGasTubCmd;
  G4UIcmdWithADoubleAndUnit* lengthGasTubCmd;
  G4UIcmdWithADoubleAndUnit* innerRadiusBeamShieldTubCmd;
  G4UIcmdWithADoubleAndUnit* outerRadiusBeamShieldTubCmd;
  G4UIcmdWithADoubleAndUnit* lengthBeamShieldTubCmd;

public:
  ActarSimGasDetectorMessenger(ActarSimGasDetectorConstruction* );
  ~ActarSimGasDetectorMessenger();
  
  void SetNewValue(G4UIcommand*, G4String);
  //G4String GetCurrentValue(G4UIcommand*);
};
#endif

