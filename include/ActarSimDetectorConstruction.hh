// - AUTHOR: Hector Alvarez-Pol 11/2004
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

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
  ActarSimGasSD* gasSD;          ///< Pointer to gas sensitive detector
  ActarSimSilSD* silSD;          ///< Pointer to silicon sensitive detector
  ActarSimSilRingSD* silRingSD;  ///< Pointer to silicon ring sensitive detector
  ActarSimSciSD* sciSD;          ///< Pointer to scintillator sensitive detector
  ActarSimSciRingSD* sciRingSD;  ///< Pointer to scintillator ring sensitive detector
  ActarSimPlaSD* plaSD;          ///< Pointer to plastic sensitive detector

  G4Box* solidWorld;

  G4LogicalVolume* worldLog;      ///< Pointer to logic world
  G4LogicalVolume* chamberLog;    ///< Pointer to logic chamber
  G4LogicalVolume* AlplateLog;    ///< Pointer to logic aluminium plate
  G4LogicalVolume* DiamondLog;    ///< Pointer to logic Diamond detector
  G4LogicalVolume* SupportLog;    ///< Pointer to logic CageField support

  G4VPhysicalVolume* worldPhys;   ///< Pointer to physical world
  G4VPhysicalVolume* chamberPhys; ///< Pointer to physical chamber
  G4VPhysicalVolume* AlplatePhys; ///< Pointer to physical Al plate
  G4VPhysicalVolume* DiamondPhys; ///< Pointer to physical Diamond detector
  G4VPhysicalVolume* SupportPhys; ///< Pointer to physical CageField support

  //Assembly of slits
  //G4AssemblyVolume* SlitMask;  //NOT USED IN THIS FILE

  G4Material* mediumMaterial;    ///< Pointer to the medium material
  G4Material* defaultMaterial;   ///< Pointer to the default material
  G4Material* chamberMaterial;   ///< Pointer to the chamber material
  G4Material* windowMaterial;    ///< Pointer to the window material

  ActarSimUniformEMField* emField; ///< Pointer to the uniform em. field

  G4ThreeVector eField;           ///< Electric field vector
  G4ThreeVector mField;           ///< Magnetic field vector

  G4double worldSizeX;    ///< World X half-lengths (always centered at (0,0,0))
  G4double worldSizeY;    ///< World Y half-lengths (always centered at (0,0,0))
  G4double worldSizeZ;    ///< World Z half-lengths (always centered at (0,0,0))

  G4double chamberSizeX;  ///< Chamber X half-lengths
  G4double chamberSizeY;  ///< Chamber Y half-lengths
  G4double chamberSizeZ;  ///< Chamber Z half-lengths

  G4double chamberCenterX;  ///< Chamber X Center
  G4double chamberCenterY;  ///< Chamber Y Center
  G4double chamberCenterZ;  ///< Chamber Z Center

  G4String MaikoGeoIncludedFlag;        ///< Control variable for including MAIKO
  G4String ACTARTPCDEMOGeoIncludedFlag; ///< Control variable for including ACTARTPCDEMO
  G4String ACTARTPCGeoIncludedFlag;     ///< Control variable for including ACTARTPC
  G4String gasGeoIncludedFlag;          ///< Control variable for including a gas volume
  G4String silGeoIncludedFlag;          ///< Control variable for including the silicons
  G4String sciGeoIncludedFlag;          ///< Control variable for including the scintillators
  G4String SpecMATGeoIncludedFlag;      ///< Control variable for including SpecMAT
  G4String OthersGeoIncludedFlag;       ///< Control variable for including other geometries

  ActarSimGasDetectorConstruction* gasDet;          ///< Pointer to target constructor
  ActarSimSilDetectorConstruction* silDet;          ///< Pointer to recoil silicon constructor
  ActarSimSilRingDetectorConstruction* silRingDet;  ///< Pointer to silRing for MAIKO constructor
  ActarSimSciDetectorConstruction* sciDet;          ///< Pointer to scintillator constructor
  ActarSimSciRingDetectorConstruction* sciRingDet;  ///< Pointer to sciRing for MAIKO constructor
  ActarSimPlaDetectorConstruction* plaDet;          ///< Pointer to Hodoscope constructor

  ActarSimDetectorMessenger* detectorMessenger;  ///< Pointer to the Messenger

  void DefineMaterials();
  G4VPhysicalVolume* ConstructEmptyWorld();
  G4VPhysicalVolume* ConstructActarTPC();
  G4VPhysicalVolume* ConstructActarTPCDEMO();
  G4VPhysicalVolume* ConstructSpecMAT();
  G4VPhysicalVolume* ConstructMAIKO();
  G4VPhysicalVolume* ConstructOthers();

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

  void SetMediumMaterial(G4String);
  void SetDefaultMaterial(G4String);
  void SetChamberMaterial(G4String);
  void SetWindowMaterial (G4String);
  void SetUpdateChamberMaterial(G4Material*);

  void SetMaikoGeoIncludedFlag(G4String val){MaikoGeoIncludedFlag=val;}
  void SetACTARTPCDEMOGeoIncludedFlag(G4String val){ACTARTPCDEMOGeoIncludedFlag=val;}
  void SetACTARTPCGeoIncludedFlag(G4String val){ACTARTPCGeoIncludedFlag=val;}
  void SetSpecMATGeoIncludedFlag(G4String val){SpecMATGeoIncludedFlag=val;}
  void SetOthersGeoIncludedFlag(G4String val){OthersGeoIncludedFlag=val;}

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
  G4String GetSpecMATGeoIncludedFlag(void){return SpecMATGeoIncludedFlag;}
  G4String GetOthersGeoIncludedFlag(void){return OthersGeoIncludedFlag;}

  void UpdateGeometry();
  void UpdateEMField();

  void SetEleField(G4ThreeVector eVector);
  void SetMagField(G4ThreeVector mVector);

  void PrintDetectorParameters();
};
#endif
