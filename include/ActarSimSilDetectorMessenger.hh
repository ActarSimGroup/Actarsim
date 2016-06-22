// - AUTHOR: Hector Alvarez-Pol 04/2008
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimSilDetectorMessenger_h
#define ActarSimSilDetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class ActarSimSilDetectorConstruction;
class ActarSimPrimaryGeneratorAction;

class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWith3VectorAndUnit;

class ActarSimSilDetectorMessenger: public G4UImessenger {
private:
  ActarSimSilDetectorConstruction* ActarSimSilDetector; ///< Pointer to main detector class

  G4UIdirectory*             detDir;                    ///< Directory in messenger structure
  G4UIcmdWithoutParameter*   printCmd;            ///< Prints geometry
  G4UIcmdWithAnInteger*      sideCoverageCmd;     ///< Selects the silicon coverage (default 1)
  G4UIcmdWithADoubleAndUnit* xBoxHalfLengthCmd;   ///< Sets the x half length of the silicon detectors box
  G4UIcmdWithADoubleAndUnit* yBoxHalfLengthCmd;   ///< Sets the y half length of the silicon detectors box
  G4UIcmdWithADoubleAndUnit* zBoxHalfLengthCmd;   ///< Sets the z half length of the silicon detectors box

public:
  ActarSimSilDetectorMessenger(ActarSimSilDetectorConstruction* );
  ~ActarSimSilDetectorMessenger();

  void SetNewValue(G4UIcommand*, G4String);
  //G4String GetCurrentValue(G4UIcommand*);
};
#endif
