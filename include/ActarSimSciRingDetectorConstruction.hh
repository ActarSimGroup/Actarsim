// - AUTHOR: Hector Alvarez-Pol 04/2008
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

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
  G4Material* sciBulkMaterial;    ///< Pointer to the scintillator material

  ActarSimDetectorConstruction* detConstruction; ///< Pointer to the global detector

  /// 6 bits to indicate which sci wall is present (1) or absent (0)
  /// order is:
  /// - bit1 (lsb) beam output wall
  /// - bit2 lower (gravity based) wall
  /// - bit3 upper (gravity based) wall
  /// - bit4 left (from beam point of view) wall
  /// - bit5 right (from beam point of view) wall
  /// - bit6 (msb) beam entrance wall
  G4int sideCoverage;

  G4double xBoxSciHalfLength;   ///< Scintillator box half length along X (X is horizontal)
  G4double yBoxSciHalfLength;   ///< Scintillator box half length along Y (Y is vertical)
  G4double zBoxSciHalfLength;   ///< Scintillator box half length along Z (Z is along beam)

  G4VPhysicalVolume* ConstructSci(G4LogicalVolume*);

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
