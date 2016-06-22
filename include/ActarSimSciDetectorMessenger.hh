// - AUTHOR: Hector Alvarez-Pol 04/2008
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimSciDetectorMessenger_h
#define ActarSimSciDetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class ActarSimSciDetectorConstruction;
class ActarSimPrimaryGeneratorAction;

class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWith3VectorAndUnit;

class ActarSimSciDetectorMessenger: public G4UImessenger {
private:
  ActarSimSciDetectorConstruction* ActarSimSciDetector; ///< Pointer to main sci detector class

  G4UIdirectory*             detDir;                    ///< Directory in messenger structure
  G4UIcmdWithoutParameter*   printCmd;                  ///< Prints geometry
  G4UIcmdWithAnInteger*      sideCoverageCmd;           ///< Selects the scintillator coverage (default 1)
  G4UIcmdWithADoubleAndUnit* xBoxHalfLengthCmd;         ///< Sets the x half length of the sci detectors box
  G4UIcmdWithADoubleAndUnit* yBoxHalfLengthCmd;         ///< Sets the y half length of the sci detectors box
  G4UIcmdWithADoubleAndUnit* zBoxHalfLengthCmd;         ///< Sets the z half length of the sci detectors box

public:
  ActarSimSciDetectorMessenger(ActarSimSciDetectorConstruction* );
  ~ActarSimSciDetectorMessenger();

  void SetNewValue(G4UIcommand*, G4String);
  //G4String GetCurrentValue(G4UIcommand*);
};
#endif
