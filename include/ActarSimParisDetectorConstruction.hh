/////////////////////////////////////////////////////////////////
//*-- AUTHOR : David Perez-Loureiro    loureiro@ganil.fr
//*-- Date: 02/2013
//*-- Last Update: 06/02/13 by D. Perez-Loureiro
// --------------------------------------------------------------
// Description:
//   Silicon detector description
//
// --------------------------------------------------------------
// Comments:
//
//   - 06/02/12 Adding PARIS clusters to ACTARSim
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#ifndef ActarSimParisDetectorConstruction_h
#define ActarSimParisDetectorConstruction_h 1

class G4LogicalVolume;
class G4VPhysicalVolume;
class ActarSimParisDetectorMessenger;
class ActarSimDetectorConstruction;

#include "G4VUserDetectorConstruction.hh"
//#include "ActarSimDetectorConstruction.hh"
#include "ActarSimParisGeometry.hh"
#include "ActarSimPhysicsList.hh"
#include "ActarSimPrimaryGeneratorAction.hh"
//#include "parisFRTrackerHit.hh"
//#include "parisFRTrackerSD.hh"

#include "G4Material.hh"
#include "G4MaterialTable.hh"

#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4Polyhedra.hh"
#include "G4Polycone.hh"
#include "G4Orb.hh"
#include "G4Torus.hh"
#include "G4Trd.hh"
#include "G4Trap.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"

#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

#include "G4UImanager.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "globals.hh"


class ActarSimParisDetectorConstruction
{
private:
  
  
  //Pointers
  ActarSimParisDetectorMessenger* parisMessenger;   //pointer to the Messenger
  ActarSimDetectorConstruction* detConstruction;//pointer to the global detector
  
  G4VPhysicalVolume* ConstructParis(G4LogicalVolume*);

  G4int sideCoverage;   // 6 bits to indicate which sil wall is present (1) or absent (0)
			// order is:
			// bit1 (lsb) beam output wall 
			// bit2 lower (gravity based) wall
			// bit3 upper (gravity based) wall
			// bit4 left (from beam point of view) wall
			// bit5 right (from beam point of view) wall
			// bit6 (msb) beam entrance wall
  
  G4double xBoxParisHalfLength;   // Remember: x is horizontal
  G4double yBoxParisHalfLength;   // Remember: y is vertical
  G4double zBoxParisHalfLength;   // Remember: z is along beam

public:
  
  ActarSimParisDetectorConstruction(ActarSimDetectorConstruction*);
  ~ActarSimParisDetectorConstruction();
  
  G4VPhysicalVolume* Construct(G4LogicalVolume*);

  void SetSideCoverage(G4int cov){sideCoverage = cov;}
  void SetXBoxParisHalfLength(G4double xBox){xBoxParisHalfLength=xBox;}
  void SetYBoxParisHalfLength(G4double yBox){yBoxParisHalfLength=yBox;}
  void SetZBoxParisHalfLength(G4double zBox){zBoxParisHalfLength=zBox;}

  G4int GetSideCoverage(){return sideCoverage;}
  G4double GetXBoxParisHalfLength(){return xBoxParisHalfLength;}
  G4double GetYBoxParisHalfLength(){return yBoxParisHalfLength;}
  G4double GetZBoxParisHalfLength(){return zBoxParisHalfLength;}

  void UpdateGeometry();
  void PrintDetectorParameters();

};

#endif


