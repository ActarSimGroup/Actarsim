/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol hapol@fpddux.usc.es
//*-- Date: 11/2004
//*-- Last Update: 05/05/08
// --------------------------------------------------------------
// Description:
//   Detector construction and complementary definitions
//
// --------------------------------------------------------------
// Comments:
//
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
  G4LogicalVolume* chamberLog;      //pointer to logic world
  G4LogicalVolume* AlplateLog;    //pointer to logic aluminium plate

  // Physical volumes
  G4VPhysicalVolume* worldPhys;   //pointer to physical world
  G4VPhysicalVolume* chamberPhys;   //pointer to physical world
  G4VPhysicalVolume* AlplatePhys; //pointer to physical Al plate

  //Assembly of slits
  G4AssemblyVolume* SlitMask;
  
  // Materials
  G4Material* mediumMaterial;
  G4Material* defaultMaterial;
  G4Material* chamberMaterial;
  G4Material* windowMaterial;

  ActarSimUniformEMField* emField; //pointer to the uniform em. field

  //electric and magnetic field
  G4ThreeVector eField;
  G4ThreeVector mField;

  //Chamber Y,Z length
  G4double chamberSizeY;
  G4double chamberSizeZ;

  //Position of the GasBox
  G4double zGasBoxPosition;

  //Control of the geometry of the experiment
  G4String MaikoGeoIncludedFlag;
  G4String gasGeoIncludedFlag;
  G4String silGeoIncludedFlag;
  G4String sciGeoIncludedFlag;

  //Detectors
  ActarSimGasDetectorConstruction* gasDet;  //target
  ActarSimSilDetectorConstruction* silDet;     //recoil si
  ActarSimSilRingDetectorConstruction* silRingDet;
  ActarSimSciDetectorConstruction* sciDet;     //calorimeter
  ActarSimSciRingDetectorConstruction* sciRingDet;
  ActarSimPlaDetectorConstruction* plaDet;     //Hodoscope

  ActarSimDetectorMessenger* detectorMessenger;  //pointer to the Messenger

  void DefineMaterials();
  G4VPhysicalVolume* ConstructActar();

public:
  
  ActarSimDetectorConstruction();
  ~ActarSimDetectorConstruction();
  
  ActarSimGasSD* GetGasSD(void){return gasSD;}
  ActarSimSilSD* GetSilSD(void){return silSD;}
  ActarSimSilRingSD* GetSilRingSD(void){return silRingSD;}
  ActarSimSciSD* GetSciSD(void){return sciSD;}
  ActarSimSciRingSD* GetSciRingSD(void){return sciRingSD;}
  ActarSimPlaSD* GetPlaSD(void){return plaSD;}

  G4VPhysicalVolume* Construct();
  
  void SetMediumMaterial(G4String);
  void SetDefaultMaterial(G4String);
  void SetChamberMaterial(G4String);
  void SetWindowMaterial (G4String);

  void SetZGasBoxPosition(G4double val){zGasBoxPosition = val;} 

  void UpdateGeometry();
  void UpdateEMField();

  G4Material* GetMediumMaterial() {return mediumMaterial;};
  G4Material* GetDefaultMaterial() {return defaultMaterial;};
  G4Material* GetChamberMaterial() {return chamberMaterial;};
 
  //const G4VPhysicalVolume* GetWorldPhys()   {return worldPhys;};
  //const G4VPhysicalVolume* GetGasPhys() {return gasPhys;};
  //const G4LogicalVolume* GetWorldLog()     {return worldLog;};

  G4LogicalVolume* GetWorldLogicalVolume(){return worldLog;}
  G4VPhysicalVolume* GetWorldPhysicalVolume(){return worldPhys;}
  //G4Material* GetWorldMaterial() {return worldMaterial;}
  G4LogicalVolume* GetChamberLogicalVolume(){return chamberLog;}
  G4VPhysicalVolume* GetChamberPhysicalVolume(){return chamberPhys;}

  G4double GetChamberZLength(void){return chamberSizeZ;} 
  G4double GetChamberYLength(void){return chamberSizeY;} 

  G4double GetZGasBoxPosition(void){return zGasBoxPosition;} 

  ActarSimGasDetectorConstruction* GetGasDetector() {return gasDet;}
  ActarSimSilDetectorConstruction* GetSilDetector() {return silDet;}
  ActarSimSilRingDetectorConstruction* GetSilRingDetector() {return silRingDet;}
  ActarSimSciDetectorConstruction* GetSciDetector() {return sciDet;}
  ActarSimSciRingDetectorConstruction* GetSciRingDetector() {return sciRingDet;}
  ActarSimPlaDetectorConstruction* GetPlaDetector() {return plaDet;}

  void SetMaikoGeoIncludedFlag(G4String val){MaikoGeoIncludedFlag=val;}
  void SetGasGeoIncludedFlag(G4String val){gasGeoIncludedFlag=val;}
  void SetSilGeoIncludedFlag(G4String val){silGeoIncludedFlag=val;}
  void SetSciGeoIncludedFlag(G4String val){sciGeoIncludedFlag=val;}

  void SetEleField(G4ThreeVector eVector);
  void SetMagField(G4ThreeVector mVector);

  void PrintDetectorParameters();
};

#endif

