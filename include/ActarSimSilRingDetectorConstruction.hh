/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez
//*-- Date: 04/2008
//*-- Last Update: 07/01/15 by Hector Alvarez
// --------------------------------------------------------------
// Description:
//   Silicon detector description
//
// --------------------------------------------------------------
// Comments:
//
//   - 17/04/08 Modularizing the ACTAR geometry
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#ifndef ActarSimSilRingDetectorConstruction_h
#define ActarSimSilRingDetectorConstruction_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class ActarSimSilRingDetectorMessenger;
class ActarSimDetectorConstruction;

class ActarSimSilRingDetectorConstruction {
private:

  // Materials
  G4Material* silBulkMaterial;

  //ActarSimSilRingDetectorMessenger* silMessenger;   //pointer to the Messenger NOT USED
  ActarSimDetectorConstruction* detConstruction;//pointer to the global detector

  G4VPhysicalVolume* ConstructSil(G4LogicalVolume*);

  G4int sideCoverage;   // 6 bits to indicate which sil wall is present (1) or absent (0)
			// order is:
			// bit1 (lsb) beam output wall
			// bit2 lower (gravity based) wall
			// bit3 upper (gravity based) wall
			// bit4 left (from beam point of view) wall
			// bit5 right (from beam point of view) wall
			// bit6 (msb) beam entrance wall

                       //all the following are half length of a box!!
  G4double xBoxSilHalfLength;   // Remember: x is horizontal
  G4double yBoxSilHalfLength;   // Remember: y is vertical
  G4double zBoxSilHalfLength;   // Remember: z is along beam


public:

  ActarSimSilRingDetectorConstruction(ActarSimDetectorConstruction*);
  ~ActarSimSilRingDetectorConstruction();

  G4VPhysicalVolume* Construct(G4LogicalVolume*);

  void SetSilBulkMaterial (G4String);
  void SetSideCoverage(G4int cov){sideCoverage = cov;}
  void SetXBoxSilHalfLength(G4double xBox){xBoxSilHalfLength=xBox;}
  void SetYBoxSilHalfLength(G4double yBox){yBoxSilHalfLength=yBox;}
  void SetZBoxSilHalfLength(G4double zBox){zBoxSilHalfLength=zBox;}

  G4Material* GetSilBulkMaterial() {return silBulkMaterial;}

  G4int GetSideCoverage(){return sideCoverage;}
  G4double GetXBoxSilHalfLength(){return xBoxSilHalfLength;}
  G4double GetYBoxSilHalfLength(){return yBoxSilHalfLength;}
  G4double GetZBoxSilHalfLength(){return zBoxSilHalfLength;}

  void UpdateGeometry();
  void PrintDetectorParameters();

};
#endif
