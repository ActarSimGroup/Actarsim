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

#ifndef ActarSimPlaDetectorConstruction_h
#define ActarSimPlaDetectorConstruction_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
//class ActarSimPlaDetectorMessenger;
class ActarSimDetectorConstruction;

class ActarSimPlaDetectorConstruction {  
private:
  
  // Materials
  G4Material* plaBulkMaterial;
 
//  ActarSimPlaDetectorMessenger* plaMessenger;   //pointer to the Messenger
  ActarSimDetectorConstruction* detConstruction;//pointer to the global detector
  
  G4VPhysicalVolume* ConstructPla(G4LogicalVolume*);

  G4int sideCoverage;   // 6 bits to indicate which sci wall is present (1) or absent (0)
			// order is:
			// bit1 (lsb) beam output wall 
			// bit2 lower (gravity based) wall
			// bit3 upper (gravity based) wall
			// bit4 left (from beam point of view) wall
			// bit5 right (from beam point of view) wall
			// bit6 (msb) beam entrance wall

                       //all the following are half length of a box!!
  G4double xBoxPlaHalfLength;   // Remember: x is horizontal
  G4double yBoxPlaHalfLength;   // Remember: y is vertical
  G4double zBoxPlaHalfLength;   // Remember: z is along beam

public:
  
  ActarSimPlaDetectorConstruction(ActarSimDetectorConstruction*);
  ~ActarSimPlaDetectorConstruction();
  
  G4VPhysicalVolume* Construct(G4LogicalVolume*);

  void SetPlaBulkMaterial (G4String);
  void SetSideCoverage(G4int cov){sideCoverage = cov;}
  void SetXBoxPlaHalfLength(G4double xBox){xBoxPlaHalfLength=xBox;}
  void SetYBoxPlaHalfLength(G4double yBox){yBoxPlaHalfLength=yBox;}
  void SetZBoxPlaHalfLength(G4double zBox){zBoxPlaHalfLength=zBox;}

  G4Material* GetPlaBulkMaterial() {return plaBulkMaterial;}

  G4int GetSideCoverage(){return sideCoverage;}
  G4double GetXBoxPlaHalfLength(){return xBoxPlaHalfLength;}
  G4double GetYBoxPlaHalfLength(){return yBoxPlaHalfLength;}
  G4double GetZBoxPlaHalfLength(){return zBoxPlaHalfLength;}

  void UpdateGeometry();
  void PrintDetectorParameters();

};
#endif

