/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol hapolyo@usc.es
//*-- Date: 11/2004
//*-- Last Update: 29/11/05
// --------------------------------------------------------------
// Description:
//   Messenger for the detector construction
//
// --------------------------------------------------------------
// Comments:
//   - 03/12/04 Created based on example/novice/N03 structure
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

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

class ActarSimDetectorMessenger: public G4UImessenger
{
private:
  ActarSimDetectorConstruction* ActarSimDetector;
  
  G4UIdirectory*             ActarSimDir;
  G4UIdirectory*             detDir;
  //G4UIcmdWith3VectorAndUnit* worldSizeCmd;

  G4UIcmdWithAString* MaikoGeoIncludedFlagCmd;
  G4UIcmdWithAString* gasGeoIncludedFlagCmd;
  // G4UIcmdWithADoubleAndUnit* zGasBoxPositionCmd;
  G4UIcmdWithAString* silGeoIncludedFlagCmd;
  G4UIcmdWithAString* sciGeoIncludedFlagCmd;
 
  G4UIcmdWithAString*        mediumMaterCmd;   

  G4UIcmdWith3Vector*        eleFieldCmd; //problems defining elec field units
  G4UIcmdWith3VectorAndUnit* magFieldCmd;
  G4UIcmdWithoutParameter*   updateCmd;
  G4UIcmdWithoutParameter*   printCmd;
  
public:
  ActarSimDetectorMessenger(ActarSimDetectorConstruction* );
  ~ActarSimDetectorMessenger();
  
  void SetNewValue(G4UIcommand*, G4String);
  //G4String GetCurrentValue(G4UIcommand*);
};

#endif

