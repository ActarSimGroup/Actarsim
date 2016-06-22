// - AUTHOR: Hector Alvarez-Pol 04/2008
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimSilDetectorConstruction_h
#define ActarSimSilDetectorConstruction_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class ActarSimSilDetectorMessenger;
class ActarSimDetectorConstruction;

class ActarSimSilDetectorConstruction {
private:
  G4Material* silBulkMaterial; ///< Pointer to the material the Silicon is made of

  ActarSimSilDetectorMessenger* silMessenger;   ///< Pointer to the Messenger
  ActarSimDetectorConstruction* detConstruction;///< Pointer to the global detector

  /// 6 bits to indicate which Silicon wall is present (1) or absent (0)
  /// order is:
  /// - bit1 (lsb) beam output wall
  /// - bit2 lower (gravity based) wall
  /// - bit3 upper (gravity based) wall
  /// - bit4 left (from beam point of view) wall
  /// - bit5 right (from beam point of view) wall
  /// - bit6 (msb) beam entrance wall
  G4int sideCoverage;

  G4double xBoxSilHalfLength;  ///< Silicon box half length along X (X is horizontal)
  G4double yBoxSilHalfLength;  ///< Silicon box half length along Y (Y is vertical)
  G4double zBoxSilHalfLength;  ///< Silicon box half length along Z (Z is along beam)

  G4VPhysicalVolume* ConstructSil(G4LogicalVolume*);
  G4VPhysicalVolume* DSSD_Al_LayerPhys;
  //G4VPhysicalVolume* MAYA_Al_LayerPhys;

public:
  ActarSimSilDetectorConstruction(ActarSimDetectorConstruction*);
  ~ActarSimSilDetectorConstruction();

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
