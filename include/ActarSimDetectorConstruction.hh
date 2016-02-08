/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol
//*-- Date: 11/2004
//*-- Last Update: 26/11/15
// --------------------------------------------------------------
// Description:
//   Detector construction and complementary definitions
//
// --------------------------------------------------------------
// Comments:
//
//   - 26/11/15 Recovering old functionality and correcting
//   - 17/04/08 Modularizing the detectors construction
//   - 04/04/06 Multigeometry with the possibility of updating.
//              Gas as a sensitive detector...
//   - 24/05/05 Created based on calGamma simulation
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#ifndef ActarSimDetectorConstruction_h
#define ActarSimDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"

#include "ActarSimUniformEMField.hh"
#include "globals.hh"

class G4Box;
class G4Tubs;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4AssemblyVolume;
class ActarSimDetectorMessenger;

class ActarSimGasSD;
class ActarSimSilSD;
class ActarSimSilRingSD;
class ActarSimSciSD;
class ActarSimSciRingSD;
class ActarSimPlaSD;
class ActarSimGasDetectorConstruction;
class ActarSimSilDetectorConstruction;
class ActarSimSilRingDetectorConstruction;
class ActarSimSciDetectorConstruction;
class ActarSimSciRingDetectorConstruction;
class ActarSimPlaDetectorConstruction;

class ActarSimDetectorConstruction : public G4VUserDetectorConstruction {
private:

  ActarSimGasSD* gasSD;
  ActarSimSilSD* silSD;
  ActarSimSilRingSD* silRingSD;
  ActarSimSciSD* sciSD;
  ActarSimSciRingSD* sciRingSD;
  ActarSimPlaSD* plaSD;

  // Volumes
  G4Box* solidWorld;

  // Logical volumes
  G4LogicalVolume* worldLog;      //pointer to logic world
  G4LogicalVolume* chamberLog;    //pointer to logic chamber
  G4LogicalVolume* AlplateLog;    //pointer to logic aluminium plate
  G4LogicalVolume* DiamondLog;    //pointer to logic Diamond detector
  G4LogicalVolume* SupportLog;    //pointer to logic CageField support

  // Physical volumes
  G4VPhysicalVolume* worldPhys;   //pointer to physical world
  G4VPhysicalVolume* chamberPhys; //pointer to physical chamber
  G4VPhysicalVolume* AlplatePhys; //pointer to physical Al plate
  G4VPhysicalVolume* DiamondPhys; //pointer to physical Diamond detector
  G4VPhysicalVolume* SupportPhys; //pointer to physical CageField support

  //Assembly of slits
  //G4AssemblyVolume* SlitMask;  //NOT USED IN THIS FILE

  // Materials
  G4Material* mediumMaterial;
  G4Material* defaultMaterial;
  G4Material* chamberMaterial;
  G4Material* windowMaterial;

  ActarSimUniformEMField* emField; //pointer to the uniform em. field

  //electric and magnetic field
  G4ThreeVector eField;
  G4ThreeVector mField;

  //World X,Y,Z half-lengths (always centered at (0,0,0))
  G4double worldSizeX;
  G4double worldSizeY;
  G4double worldSizeZ;

  //Chamber X,Y,Z half-lengths
  G4double chamberSizeX;
  G4double chamberSizeY;
  G4double chamberSizeZ;

  //Chamber X,Y,Z Center
  G4double chamberCenterX;
  G4double chamberCenterY;
  G4double chamberCenterZ;

  //Control of the geometry of the experiment
  G4String MaikoGeoIncludedFlag;
  G4String ACTARTPCDEMOGeoIncludedFlag;
  G4String ACTARTPCGeoIncludedFlag;
  G4String gasGeoIncludedFlag;
  G4String silGeoIncludedFlag;
  G4String sciGeoIncludedFlag;
  
  G4int    AT_Project;                              //Main switch to select the type of detector

  //Detectors
  ActarSimGasDetectorConstruction* gasDet;          //target
  ActarSimSilDetectorConstruction* silDet;          //recoil si
  ActarSimSilRingDetectorConstruction* silRingDet;  //silRing for MAIKO
  ActarSimSciDetectorConstruction* sciDet;          //calorimeter
  ActarSimSciRingDetectorConstruction* sciRingDet;  //sciRing for MAIKO
  ActarSimPlaDetectorConstruction* plaDet;          //Hodoscope

  ActarSimDetectorMessenger* detectorMessenger;  //pointer to the Messenger

  void DefineMaterials();
  G4VPhysicalVolume* ConstructActarTPC();
  G4VPhysicalVolume* ConstructSpecMAT();

public:

  ActarSimDetectorConstruction();
  ~ActarSimDetectorConstruction();

  G4VPhysicalVolume* Construct();

  void SetWorldSizeX(G4double val){worldSizeX = val;}
  void SetWorldSizeY(G4double val){worldSizeY = val;}
  void SetWorldSizeZ(G4double val){worldSizeZ = val;}

  void SetChamberSizeX(G4double val){chamberSizeX = val;}
  void SetChamberSizeY(G4double val){chamberSizeY = val;}
  void SetChamberSizeZ(G4double val){chamberSizeZ = val;}

  void SetChamberCenterX(G4double val){chamberCenterX = val;}
  void SetChamberCenterY(G4double val){chamberCenterY = val;}
  void SetChamberCenterZ(G4double val){chamberCenterZ = val;}

  void SetAT_Project(G4int val){AT_Project=val;}

  void SetMediumMaterial(G4String);
  void SetDefaultMaterial(G4String);
  void SetChamberMaterial(G4String);
  void SetWindowMaterial (G4String);
  void SetUpdateChamberMaterial(G4Material*);

  void SetMaikoGeoIncludedFlag(G4String val){MaikoGeoIncludedFlag=val;}
  void SetACTARTPCDEMOGeoIncludedFlag(G4String val){ACTARTPCDEMOGeoIncludedFlag=val;}
  void SetACTARTPCGeoIncludedFlag(G4String val){ACTARTPCGeoIncludedFlag=val;}
  void SetGasGeoIncludedFlag(G4String val){gasGeoIncludedFlag=val;}
  void SetSilGeoIncludedFlag(G4String val){silGeoIncludedFlag=val;}
  void SetSciGeoIncludedFlag(G4String val){sciGeoIncludedFlag=val;}

  ActarSimGasSD* GetGasSD(void){return gasSD;}
  ActarSimSilSD* GetSilSD(void){return silSD;}
  ActarSimSilRingSD* GetSilRingSD(void){return silRingSD;}
  ActarSimSciSD* GetSciSD(void){return sciSD;}
  ActarSimSciRingSD* GetSciRingSD(void){return sciRingSD;}
  ActarSimPlaSD* GetPlaSD(void){return plaSD;}

  ActarSimDetectorMessenger* GetDetectorMessenger(){return detectorMessenger;};

  G4LogicalVolume* GetWorldLogicalVolume(){return worldLog;}
  G4VPhysicalVolume* GetWorldPhysicalVolume(){return worldPhys;}
  G4LogicalVolume* GetChamberLogicalVolume(){return chamberLog;}
  G4VPhysicalVolume* GetChamberPhysicalVolume(){return chamberPhys;}

  G4double GetWorldSizeX(void){return worldSizeX;}
  G4double GetWorldSizeY(void){return worldSizeY;}
  G4double GetWorldSizeZ(void){return worldSizeZ;}

  G4double GetChamberSizeX(void){return chamberSizeX;}
  G4double GetChamberSizeY(void){return chamberSizeY;}
  G4double GetChamberSizeZ(void){return chamberSizeZ;}

  G4double GetChamberCenterX(void){return chamberCenterX;}
  G4double GetChamberCenterY(void){return chamberCenterY;}
  G4double GetChamberCenterZ(void){return chamberCenterZ;}

  G4int GetAT_Project(){return AT_Project;}
  
  G4Material* GetMediumMaterial() {return mediumMaterial;};
  G4Material* GetDefaultMaterial() {return defaultMaterial;};
  G4Material* GetChamberMaterial() {return chamberMaterial;};

  ActarSimGasDetectorConstruction* GetGasDetector() {return gasDet;}
  ActarSimSilDetectorConstruction* GetSilDetector() {return silDet;}
  ActarSimSilRingDetectorConstruction* GetSilRingDetector() {return silRingDet;}
  ActarSimSciDetectorConstruction* GetSciDetector() {return sciDet;}
  ActarSimSciRingDetectorConstruction* GetSciRingDetector() {return sciRingDet;}
  ActarSimPlaDetectorConstruction* GetPlaDetector() {return plaDet;}

  G4String GetMaikoGeoIncludedFlag(void){return MaikoGeoIncludedFlag;}
  G4String GetACTARTPCDEMOGeoIncludedFlag(void){return ACTARTPCDEMOGeoIncludedFlag;}
  G4String GetACTARTPCGeoIncludedFlag(void){return ACTARTPCGeoIncludedFlag;}

  void UpdateGeometry();
  void UpdateEMField();

  void SetEleField(G4ThreeVector eVector);
  void SetMagField(G4ThreeVector mVector);

  void PrintDetectorParameters();
};

#endif
