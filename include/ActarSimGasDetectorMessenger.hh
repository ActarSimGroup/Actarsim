/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez Pol
//*-- Date: 04/2008
//*-- Last Update: 15/12/14
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

class ActarSimDetectorConstruction;
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
  ActarSimDetectorConstruction* ActarSimDetector;
  ActarSimGasDetectorConstruction* ActarSimGasDetector;

  G4UIcmdWithAnInteger*      gasMixtureFlagCmd;
  G4UIcommand*               gasMixtureCmd;

  G4UIdirectory*             detDir;
  G4UIdirectory*             detDirMix;

  G4UIcmdWithAString*        gasMaterCmd;
  G4UIcmdWithADoubleAndUnit* gasPresCmd;
  G4UIcmdWithADoubleAndUnit* gasTempCmd;

  G4UIcmdWithAString*        beamShieldMaterCmd;

  G4UIcmdWithoutParameter*   printCmd;

  G4UIcmdWithAString*        detectorGeometryCmd;
  G4UIcmdWithAString*        setBeamShieldCmd;
  G4UIcmdWithADoubleAndUnit* gasBoxSizeXCmd;
  G4UIcmdWithADoubleAndUnit* gasBoxSizeYCmd;
  G4UIcmdWithADoubleAndUnit* gasBoxSizeZCmd;
  G4UIcmdWithADoubleAndUnit* gasBoxCenterXCmd;
  G4UIcmdWithADoubleAndUnit* gasBoxCenterYCmd;
  G4UIcmdWithADoubleAndUnit* gasBoxCenterZCmd;
  G4UIcmdWithADoubleAndUnit* radiusGasTubCmd;
  G4UIcmdWithADoubleAndUnit* lengthGasTubCmd;
  G4UIcmdWithADoubleAndUnit* innerRadiusBeamShieldTubCmd;
  G4UIcmdWithADoubleAndUnit* outerRadiusBeamShieldTubCmd;
  G4UIcmdWithADoubleAndUnit* lengthBeamShieldTubCmd;

  //For the gas mixture
  G4int fGasNumber;
  G4String fGasMaterial;
  G4double fGasRatio;

private:
  void GasMixtureCommand(G4String newValues);

public:
  ActarSimGasDetectorMessenger(ActarSimDetectorConstruction* ,ActarSimGasDetectorConstruction* );
  ~ActarSimGasDetectorMessenger();

  void SetNewValue(G4UIcommand*, G4String);
  //G4String GetCurrentValue(G4UIcommand*);
};
#endif
