// - AUTHOR: Hector Alvarez-Pol 04/2008
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimSciSD_h
#define ActarSimSciSD_h 1

#include "G4VSensitiveDetector.hh"
#include "ActarSimSciGeantHit.hh"

class G4Step;
class G4HCofThisEvent;

class ActarSimSciSD : public G4VSensitiveDetector {
private:
  ActarSimSciGeantHitsCollection* hitsCollection; ///< Geant step-like hits collect.

public:
  ActarSimSciSD(G4String);
  ~ActarSimSciSD();

  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*,G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);
};
#endif
