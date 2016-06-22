// - AUTHOR: Hector Alvarez-Pol 04/2008
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimSilRingSD_h
#define ActarSimSilRingSD_h 1

#include "G4VSensitiveDetector.hh"
#include "ActarSimSilRingGeantHit.hh"

class G4Step;
class G4HCofThisEvent;

class ActarSimSilRingSD : public G4VSensitiveDetector {
private:
  ActarSimSilRingGeantHitsCollection* hitsCollection; ///< Geant step-like hits collect.

public:
  ActarSimSilRingSD(G4String);
  ~ActarSimSilRingSD();

  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*,G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);
};
#endif
