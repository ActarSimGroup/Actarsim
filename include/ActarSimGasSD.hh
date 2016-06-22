// - AUTHOR: Hector Alvarez-Pol 04/2006
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimGasSD_h
#define ActarSimGasSD_h 1

#include "G4VSensitiveDetector.hh"
#include "ActarSimGasGeantHit.hh"

class G4Step;
class G4HCofThisEvent;

class ActarSimGasSD : public G4VSensitiveDetector {
private:
  ActarSimGasGeantHitsCollection* hitsCollection; ///< Geant step-like hits collect.

public:
  ActarSimGasSD(G4String);
  ~ActarSimGasSD();

  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*,G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);
};
#endif
