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
class ActarSimSciSD;
class ParisTrackerSD;
class ActarSimGasDetectorConstruction;
class ActarSimSilDetectorConstruction;
class ActarSimSciDetectorConstruction;
class ActarSimParisDetectorConstruction;

class ActarSimDetectorConstruction : public G4VUserDetectorConstruction {  
private:

  ActarSimGasSD* gasSD;
  ActarSimSilSD* silSD;
  ActarSimSciSD* sciSD;
  ParisTrackerSD* parisSD;
  // Volumes
  G4Box* solidWorld;

  // Logical volumes
  G4LogicalVolume* worldLog;      //pointer to logic world
  G4LogicalVolume* AlplateLog;    //pointer to logic aluminium plate
  G4LogicalVolume* SlitLog;       //pointer to logic slit in Al plate
  
  // Physical volumes
  G4VPhysicalVolume* worldPhys;   //pointer to physical world
  G4VPhysicalVolume* AlplatePhys; //pointer to physical Al plate
  G4VPhysicalVolume* SlitPhys;    //pointer to physical Al plate

  //Assembly of slits
  G4AssemblyVolume* SlitMask;
  
  // Materials
  G4Material* mediumMaterial;
  G4Material* defaultMaterial;

  ActarSimUniformEMField* emField; //pointer to the uniform em. field

  //electric and magnetic field
  G4ThreeVector eField;
  G4ThreeVector mField;

  //Position of the slit
  G4ThreeVector  slitPos;
  G4ThreeVector platePos;
  //G4RotationMatrix slitRot;
  //Control of the geometry of the experiment
  G4String gasGeoIncludedFlag;
  G4String silGeoIncludedFlag;
  G4String sciGeoIncludedFlag;
  G4String parisGeoIncludedFlag;
  G4String plateIncludedFlag;
  G4String slitOrientation;

  //Detectors
  ActarSimGasDetectorConstruction* gasDet;  //target
  ActarSimSilDetectorConstruction* silDet;     //recoil si
  ActarSimSciDetectorConstruction* sciDet;     //calorimeter
  ActarSimParisDetectorConstruction* parisDet;     //PARIS

  ActarSimDetectorMessenger* detectorMessenger;  //pointer to the Messenger

  void DefineMaterials();
  G4VPhysicalVolume* ConstructActar();

public:
  
  ActarSimDetectorConstruction();
  ~ActarSimDetectorConstruction();
  
  ActarSimGasSD* GetGasSD(void){return gasSD;}
  ActarSimSilSD* GetSilSD(void){return silSD;}
  ActarSimSciSD* GetSciSD(void){return sciSD;}
  ParisTrackerSD*  GetParisSD(void){return parisSD;}

  G4VPhysicalVolume* Construct();
  
  void SetMediumMaterial(G4String);
  void SetDefaultMaterial(G4String);

  void UpdateGeometry();
  void UpdateEMField();

  G4Material* GetMediumMaterial() {return mediumMaterial;};
  G4Material* GetDefaultMaterial() {return defaultMaterial;};
  
  //const G4VPhysicalVolume* GetWorldPhys()   {return worldPhys;};
  //const G4VPhysicalVolume* GetGasPhys() {return gasPhys;};
  //const G4LogicalVolume* GetWorldLog()     {return worldLog;};

  G4LogicalVolume* GetWorldLogicalVolume(){return worldLog;}
  G4VPhysicalVolume* GetWorldPhysicalVolume(){return worldPhys;}
  //G4Material* GetWorldMaterial() {return worldMaterial;}

  ActarSimGasDetectorConstruction* GetGasDetector() {return gasDet;}
  ActarSimSilDetectorConstruction* GetSilDetector() {return silDet;}
  ActarSimSciDetectorConstruction* GetSciDetector() {return sciDet;}

  void SetGasGeoIncludedFlag(G4String val){gasGeoIncludedFlag=val;}
  void SetSilGeoIncludedFlag(G4String val){silGeoIncludedFlag=val;}
  void SetSciGeoIncludedFlag(G4String val){sciGeoIncludedFlag=val;}
  void SetParisGeoIncludedFlag(G4String val){parisGeoIncludedFlag=val;}
  void SetPlateIncludedFlag(G4String val){plateIncludedFlag=val;}
  void SetSlitOrientation(G4String val){slitOrientation=val;}


  void SetEleField(G4ThreeVector eVector);
  void SetMagField(G4ThreeVector mVector);

  void SetSlitPosition(G4ThreeVector position);
  void SetPlatePosition(G4ThreeVector position);
  void PrintDetectorParameters();
};

#endif

