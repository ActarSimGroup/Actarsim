/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol  hapol@fpddux.usc.es
//*-- Date: 11/2004
//*-- Last Update: 01/12/05
// --------------------------------------------------------------
// Description:
//   Messenger for the analysis interaction
//
// --------------------------------------------------------------
// Comments:
// 
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#ifndef ActarSimAnalysisMessenger_h
#define ActarSimAnalysisMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class ActarSimROOTAnalysis;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAnInteger;

class ActarSimAnalysisMessenger: public G4UImessenger {
  
public:
  ActarSimAnalysisMessenger(ActarSimROOTAnalysis*);
  ~ActarSimAnalysisMessenger();
  
  void SetNewValue(G4UIcommand*, G4String);
  
private:
  ActarSimROOTAnalysis*  analExample;
  G4UIdirectory*        analDir;   
  G4UIcmdWithAString*   storeTracksCmd;
  G4UIcmdWithAString*   storeTrackHistosCmd;
  G4UIcmdWithADoubleAndUnit* setMinStrideLengthCmd;
  G4UIcmdWithAString*   storeEventsCmd;           // added for The_ACTAR_Event_Tree, dypang 080301
  G4UIcmdWithAString*   storeSimpleTracksCmd;     // added for The_ACTAR_Simple_Tracks_Tree, dypang 080301
  G4UIcmdWithAString*   storeHistosCmd;           // added for Histograms, dypang 080301
};

#endif
