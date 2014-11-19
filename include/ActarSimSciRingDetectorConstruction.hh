/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez    hapolyo@usc.es
//*-- Date: 04/2008
//*-- Last Update: 17/05/08 by Hector Alvarez
// --------------------------------------------------------------
// Description:
//   Scintillator detector description
//
// --------------------------------------------------------------
// Comments:
//
//   - 17/04/08 Modularizing the ACTAR geometry
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#ifndef ActarSimSciRingDetectorConstruction_h
#define ActarSimSciRingDetectorConstruction_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
//class ActarSimSciDetectorMessenger;
class ActarSimDetectorConstruction;

class ActarSimSciRingDetectorConstruction {  
private:
  
  // Materials
  G4Material* sciBulkMaterial;
 
  // ActarSimSciDetectorMessenger* sciMessenger;   //pointer to the Messenger
  ActarSimDetectorConstruction* detConstruction;//pointer to the global detector
  
  G4VPhysicalVolume* ConstructSci(G4LogicalVolume*);

  G4int sideCoverage;   // 6 bits to indicate which sci wall is present (1) or absent (0)
			// order is:
			// bit1 (lsb) beam output wall 
			// bit2 lower (gravity based) wall
			// bit3 upper (gravity based) wall
			// bit4 left (from beam point of view) wall
			// bit5 right (from beam point of view) wall
			// bit6 (msb) beam entrance wall

                       //all the following are half length of a box!!
  G4double xBoxSciHalfLength;   // Remember: x is horizontal
  G4double yBoxSciHalfLength;   // Remember: y is vertical
  G4double zBoxSciHalfLength;   // Remember: z is along beam

public:
  
  ActarSimSciRingDetectorConstruction(ActarSimDetectorConstruction*);
  ~ActarSimSciRingDetectorConstruction();
  
  G4VPhysicalVolume* Construct(G4LogicalVolume*);

  void SetSciBulkMaterial (G4String);
  void SetSideCoverage(G4int cov){sideCoverage = cov;}
  void SetXBoxSciHalfLength(G4double xBox){xBoxSciHalfLength=xBox;}
  void SetYBoxSciHalfLength(G4double yBox){yBoxSciHalfLength=yBox;}
  void SetZBoxSciHalfLength(G4double zBox){zBoxSciHalfLength=zBox;}

  G4Material* GetSciBulkMaterial() {return sciBulkMaterial;}

  G4int GetSideCoverage(){return sideCoverage;}
  G4double GetXBoxSciHalfLength(){return xBoxSciHalfLength;}
  G4double GetYBoxSciHalfLength(){return yBoxSciHalfLength;}
  G4double GetZBoxSciHalfLength(){return zBoxSciHalfLength;}

  void UpdateGeometry();
  void PrintDetectorParameters();

};
#endif

