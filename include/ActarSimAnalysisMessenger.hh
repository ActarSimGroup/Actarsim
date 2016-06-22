// - AUTHOR: Hector Alvarez-Pol 01/2006
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

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
private:
  ActarSimROOTAnalysis* analExample;                 ///< Pointer to main analysis class
  G4UIdirectory*        analDir;                     ///< Directory in messenger structure
  G4UIcmdWithAString*   storeTracksCmd;              ///< Store the tracks in the output Tree
  G4UIcmdWithAString*   storeTrackHistosCmd;         ///< Store the tracks in Histograms
  G4UIcmdWithADoubleAndUnit* setMinStrideLengthCmd;  ///< Sets the minimum value for the stride length
  G4UIcmdWithAString*   storeEventsCmd;              ///< Store the events in the output Tree
  G4UIcmdWithAString*   storeSimpleTracksCmd;        ///< Store the simple tracks in the output Tree
  G4UIcmdWithAString*   storeHistosCmd;              ///< Store histograms in the output Tree

public:
  ActarSimAnalysisMessenger(ActarSimROOTAnalysis*);
  ~ActarSimAnalysisMessenger();

  void SetNewValue(G4UIcommand*, G4String);
};
#endif
