// - AUTHOR: Hector Alvarez-Pol 04/2008
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

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
  ActarSimDetectorConstruction* ActarSimDetector;         ///< Pointer to main detector geometry class
  ActarSimGasDetectorConstruction* ActarSimGasDetector;   ///< Pointer to main gas detector geometry class

  G4UIcmdWithAnInteger*      gasMixtureFlagCmd;           ///< Set a Gas Mixture with N number of elements (default 0).
  G4UIcommand*               gasMixtureCmd;               ///< Set a Gas Mixture (for the Gas box and the Chamber).

  G4UIdirectory*             detDir;                      ///< Directory
  G4UIdirectory*             detDirMix;                   ///< Directory for gas mix

  G4UIcmdWithAString*        gasMaterCmd;                 ///< Select Material of the Gas (for the Gas box and the Chamber).
  G4UIcmdWithADoubleAndUnit* gasPresCmd;                  ///< Select the Gas Pressure (for the Gas box and the Chamber).
  G4UIcmdWithADoubleAndUnit* gasTempCmd;                  ///< Select the Gas Temperature (for the Gas box and the Chamber).

  G4UIcmdWithAString*        beamShieldMaterCmd;          ///< Select Material of the beam shield.

  G4UIcmdWithoutParameter*   printCmd;                    ///< Prints geometry.

  G4UIcmdWithAString*        detectorGeometryCmd;         ///< Select the geometry of the detector.
  G4UIcmdWithAString*        setBeamShieldCmd;            ///< Sets a beam shield and selects the geometry.
  G4UIcmdWithADoubleAndUnit* gasBoxSizeXCmd;              ///< Select the half-length X dimension of the Gas Box.
  G4UIcmdWithADoubleAndUnit* gasBoxSizeYCmd;              ///< Select the half-length Y dimension of the Gas Box.
  G4UIcmdWithADoubleAndUnit* gasBoxSizeZCmd;              ///< Select the half-length Z dimension of the Gas Box.
  G4UIcmdWithADoubleAndUnit* gasBoxCenterXCmd;            ///< Select the X offset of the Gas Box center.
  G4UIcmdWithADoubleAndUnit* gasBoxCenterYCmd;            ///< Select the Y offset of the Gas Box center.
  G4UIcmdWithADoubleAndUnit* gasBoxCenterZCmd;            ///< Select the Z offset of the Gas Box center.
  G4UIcmdWithADoubleAndUnit* radiusGasTubCmd;             ///< Select the external radius of the Gas Tube.
  G4UIcmdWithADoubleAndUnit* lengthGasTubCmd;             ///< Select the half-length of the Gas Tube.
  G4UIcmdWithADoubleAndUnit* innerRadiusBeamShieldTubCmd; ///< Select the internal radius of the Gas Tube.
  G4UIcmdWithADoubleAndUnit* outerRadiusBeamShieldTubCmd; ///< Select the external radius of the Gas Tube.
  G4UIcmdWithADoubleAndUnit* lengthBeamShieldTubCmd;      ///< Select the half-length of the Gas Tube.

  //For the gas mixture
  G4int fGasNumber;                                       ///< Gas mixture parameters: number
  G4String fGasMaterial;                                  ///< Gas mixture parameters: material
  G4double fGasRatio;                                     ///< Gas mixture parameters: ratio

  void GasMixtureCommand(G4String newValues);

public:
  ActarSimGasDetectorMessenger(ActarSimDetectorConstruction* ,ActarSimGasDetectorConstruction* );
  ~ActarSimGasDetectorMessenger();

  void SetNewValue(G4UIcommand*, G4String);
  //G4String GetCurrentValue(G4UIcommand*);
};
#endif
