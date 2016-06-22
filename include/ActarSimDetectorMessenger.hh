// - AUTHOR: Hector Alvarez-Pol 11/2004
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimDetectorMessenger_h
#define ActarSimDetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class ActarSimDetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADouble;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWith3Vector;
class G4UIcmdWithoutParameter;

class ActarSimDetectorMessenger: public G4UImessenger {
private:
  ActarSimDetectorConstruction* ActarSimDetector;     ///< Pointer to detector class

  G4UIdirectory*             ActarSimDir;             ///< Directory in messenger structure
  G4UIdirectory*             detDir;                  ///< Directory in messenger structure
  //G4UIcmdWith3VectorAndUnit* worldSizeCmd;

  G4UIcmdWithAString* MaikoGeoIncludedFlagCmd;        ///< Includes the Maiko geometry in the simulation (default off).
  G4UIcmdWithAString* ACTARTPCDEMOGeoIncludedFlagCmd; ///< Includes the ACTARTPC Demonstrator geometry in the simulation (default off).
  G4UIcmdWithAString* ACTARTPCGeoIncludedFlagCmd;     ///< Includes the ACTARTPC geometry in the simulation (default off).
  G4UIcmdWithAString* SpecMATGeoIncludedFlagCmd;      ///< Includes the SpecMAT geometry in the simulation (default off).
  G4UIcmdWithAString* OthersGeoIncludedFlagCmd;       ///< Includes Other geometries in the simulation (default off).
  G4UIcmdWithADoubleAndUnit* chamberSizeXCmd;         ///< Select the half-length X dimension of the Gas Chamber.
  G4UIcmdWithADoubleAndUnit* chamberSizeYCmd;         ///< Select the half-length Y dimension of the Gas Chamber.
  G4UIcmdWithADoubleAndUnit* chamberSizeZCmd;         ///< Select the half-length Z dimension of the Gas Chamber.
  G4UIcmdWithADoubleAndUnit* chamberCenterXCmd;       ///< Select the X offset of the Chamber center.
  G4UIcmdWithADoubleAndUnit* chamberCenterYCmd;       ///< Select the Y offset of the Chamber center.
  G4UIcmdWithADoubleAndUnit* chamberCenterZCmd;       ///< Select the Z offset of the Chamber center.
  G4UIcmdWithADoubleAndUnit* worldSizeXCmd;           ///< Select the half-length X dimension of the World.
  G4UIcmdWithADoubleAndUnit* worldSizeYCmd;           ///< Select the half-length Y dimension of the World.
  G4UIcmdWithADoubleAndUnit* worldSizeZCmd;           ///< Select the half-length Z dimension of the World.
  G4UIcmdWithAString* gasGeoIncludedFlagCmd;          ///< Includes the geometry of the gas volume in the simulation (default on).
  G4UIcmdWithAString* silGeoIncludedFlagCmd;          ///< Includes the geometry of the silicons in the simulation (default off).
  G4UIcmdWithAString* sciGeoIncludedFlagCmd;          ///< Includes the geometry of the scintillator in the simulation (default off).

  G4UIcmdWithAString*        mediumMaterialCmd;       ///< Select Material outside the Chamber.
  G4UIcmdWithAString*        chamberMaterialCmd;      ///< Select Material in the Chamber (but not in the gas!).

  G4UIcmdWith3Vector*        eleFieldCmd;             ///< Define electric field.
  G4UIcmdWith3VectorAndUnit* magFieldCmd;             ///< Define magnetic field.
  G4UIcmdWithoutParameter*   updateCmd;               ///< Update geometry.
  G4UIcmdWithoutParameter*   printCmd;                ///< Prints geometry.

public:
  ActarSimDetectorMessenger(ActarSimDetectorConstruction* );
  ~ActarSimDetectorMessenger();

  void SetNewValue(G4UIcommand*, G4String);
  //G4String GetCurrentValue(G4UIcommand*);
};
#endif
