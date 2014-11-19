////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez    hapolyo@usc.es
//*-- Date: 04/2008
//*-- Last Update: 17/05/08 by Hector Alvarez
// --------------------------------------------------------------
// Description:
//   Gas volume detector description
//
// --------------------------------------------------------------
// Comments:
//
//   - 17/04/08 Modularizing the ACTAR geometry
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

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

  // Materials
  G4Material* gasMaterial;
  G4Material* beamShieldMaterial;

  // GasBox Size Parameters
  G4double xGasBox;
  G4double yGasBox;
  G4double zGasBox;

  // GasTub Size Parameters
  G4double radiusGasTub;
  G4double lengthGasTub;

  // beamShieldTub Size Parameters
  G4double innerRadiusBeamShieldTub;
  G4double outerRadiusBeamShieldTub;
  G4double lengthBeamShieldTub;

  //Select type of volume
  G4String detectorGeometry;
  G4String beamShieldGeometry;

  //
  G4String luciteBoxIncluded;
  ActarSimGasDetectorMessenger* gasMessenger;   //pointer to the Messenger
  ActarSimDetectorConstruction* detConstruction;//pointer to the global detector
  
  G4VPhysicalVolume* ConstructGas(G4LogicalVolume*);

public:
  
  ActarSimGasDetectorConstruction(ActarSimDetectorConstruction*);
  ~ActarSimGasDetectorConstruction();
  
  G4VPhysicalVolume* Construct(G4LogicalVolume*);

  void SetGasMaterial (G4String);
  void SetBeamShieldMaterial(G4String);
  void SetDetectorGeometry(G4String);
  void SetBeamShieldGeometry(G4String);
  void SetLuciteBox(G4String);

  G4Material* GetGasMaterial() {return gasMaterial;}

  void SetXGasBox(G4double val){xGasBox = val;} 
  void SetYGasBox(G4double val){yGasBox = val;} 
  void SetZGasBox(G4double val){zGasBox = val;} 
  void SetRadiusGasTub(G4double val){radiusGasTub = val;} 
  void SetLengthGasTub(G4double val){lengthGasTub = val;} 
  void SetInnerRadiusBeamShieldTub(G4double val){innerRadiusBeamShieldTub = val;} 
  void SetOuterRadiusBeamShieldTub(G4double val){outerRadiusBeamShieldTub = val;} 
  void SetLengthBeamShieldTub(G4double val){lengthBeamShieldTub = val;} 

  G4String GetDetectorGeometry(){return detectorGeometry;}
  G4String GetBeamShieldGeometry(){return beamShieldGeometry;}

  G4double GetXGasBox(void){return xGasBox;} 
  G4double GetYGasBox(void){return yGasBox;} 
  G4double GetZGasBox(void){return zGasBox;} 
  G4double GetRadiusGasTub(void){return radiusGasTub;} 
  G4double GetLengthGasTub(void){return lengthGasTub;} 
  G4double GetInnerRadiusBeamShieldTub(void){return innerRadiusBeamShieldTub;} 
  G4double GetOuterRadiusBeamShieldTub(void){return outerRadiusBeamShieldTub;} 
  G4double GetLengthBeamShieldTub(void){return lengthBeamShieldTub;} 


  void UpdateGeometry();
  void PrintDetectorParameters();

};
#endif

