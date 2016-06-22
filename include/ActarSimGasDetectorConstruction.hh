// - AUTHOR: Hector Alvarez-Pol 04/2008
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimGasDetectorConstruction_h
#define ActarSimGasDetectorConstruction_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class ActarSimGasDetectorMessenger;
class ActarSimDetectorConstruction;

class ActarSimGasDetectorConstruction {
private:
  G4Material* gasMaterial;            ///< Pointer to the gas material
  G4Material* beamShieldMaterial;     ///< Pointer to the beam shield material

  G4int NumberOfGasMix;               ///< Number of gases in the gas mix (maximum 10)
  G4String gasMixMaterial[10];        ///< List of gas materials
  G4double gasMixRatio[10];           ///< List of gas ratios in the mix

  G4double gasPressure;               ///< Gas pressure
  G4double gasTemperature;            ///< Gas temperature

  G4double gasBoxSizeX;               ///< Gas Box X half-lengths
  G4double gasBoxSizeY;               ///< Gas Box Y half-lengths
  G4double gasBoxSizeZ;               ///< Gas Box Z half-lengths

  G4double gasBoxCenterX;             ///< Gas Box X center
  G4double gasBoxCenterY;             ///< Gas Box Y center
  G4double gasBoxCenterZ;             ///< Gas Box Z center

  G4double radiusGasTub;             ///< Radius of the Gas Tube
  G4double lengthGasTub;             ///< Length of the Gas Tube

  G4LogicalVolume* wireFoilLog;      ///< Pointer to the wire foil logical volume
  G4VPhysicalVolume* wireFoilPhys;   ///< Pointer to the wire foil physical volume

  G4double innerRadiusBeamShieldTub; ///< Inner radius of the beam shielding tube
  G4double outerRadiusBeamShieldTub; ///< Outer radius of the beam shielding tube
  G4double lengthBeamShieldTub;      ///< Length of the beam shielding tube

  G4String detectorGeometry;        ///< Type of the detector geometry (box or tube)
  G4String beamShieldGeometry;      ///< Type of the detector geometry (tube)

  ActarSimGasDetectorMessenger* gasMessenger;    ///< Pointer to the Messenger
  //ActarSimDetectorMessenger* detectorMessenger;   //pointer to the Messenger
  ActarSimDetectorConstruction* detConstruction; ///< Pointer to the global detector construction

  G4VPhysicalVolume* ConstructGas(G4LogicalVolume*);

public:
  ActarSimGasDetectorConstruction(ActarSimDetectorConstruction*);
  ~ActarSimGasDetectorConstruction();

  G4VPhysicalVolume* Construct(G4LogicalVolume*);

  //void DefineGas ();
  void SetGasMaterial (G4String);
  void SetGasMixture(G4int val){NumberOfGasMix = val;}
  void SetGasMixMaterial(G4int GasNum, G4String GasMat, G4double GasRatio) {
    gasMixMaterial[GasNum-1]=GasMat;
    gasMixRatio[GasNum-1]=GasRatio;
  }

  void SetGasPressure(G4double val){gasPressure = val;}
  void SetGasTemperature(G4double val){gasTemperature = val;}

  void SetBeamShieldMaterial(G4String);
  void SetDetectorGeometry(G4String);
  void SetBeamShieldGeometry(G4String);

  void SetGasBoxSizeX(G4double val){gasBoxSizeX = val;}
  void SetGasBoxSizeY(G4double val){gasBoxSizeY = val;}
  void SetGasBoxSizeZ(G4double val){gasBoxSizeZ = val;}

  void SetGasBoxCenterX(G4double val){gasBoxCenterX = val;}
  void SetGasBoxCenterY(G4double val){gasBoxCenterY = val;}
  void SetGasBoxCenterZ(G4double val){gasBoxCenterZ = val;}

  void SetRadiusGasTub(G4double val){radiusGasTub = val;}
  void SetLengthGasTub(G4double val){lengthGasTub = val;}
  void SetInnerRadiusBeamShieldTub(G4double val){innerRadiusBeamShieldTub = val;}
  void SetOuterRadiusBeamShieldTub(G4double val){outerRadiusBeamShieldTub = val;}
  void SetLengthBeamShieldTub(G4double val){lengthBeamShieldTub = val;}

  G4Material* GetGasMaterial() {return gasMaterial;}
  G4double GetGasPressure(void){return gasPressure;}
  G4double GetGasTemperature(void){return gasTemperature;}

  G4String GetDetectorGeometry(){return detectorGeometry;}
  G4String GetBeamShieldGeometry(){return beamShieldGeometry;}

  G4double GetGasBoxSizeX(void){return gasBoxSizeX;}
  G4double GetGasBoxSizeY(void){return gasBoxSizeY;}
  G4double GetGasBoxSizeZ(void){return gasBoxSizeZ;}

  G4double GetGasBoxCenterX(void){return gasBoxCenterX;}
  G4double GetGasBoxCenterY(void){return gasBoxCenterY;}
  G4double GetGasBoxCenterZ(void){return gasBoxCenterZ;}

  G4double GetRadiusGasTub(void){return radiusGasTub;}
  G4double GetLengthGasTub(void){return lengthGasTub;}
  G4double GetInnerRadiusBeamShieldTub(void){return innerRadiusBeamShieldTub;}
  G4double GetOuterRadiusBeamShieldTub(void){return outerRadiusBeamShieldTub;}
  G4double GetLengthBeamShieldTub(void){return lengthBeamShieldTub;}

  void UpdateGeometry();
  void PrintDetectorParameters();
};
#endif
