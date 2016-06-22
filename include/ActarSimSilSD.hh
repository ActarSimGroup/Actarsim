// - AUTHOR: Hector Alvarez-Pol 04/2008
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimSilSD_h
#define ActarSimSilSD_h 1

#include "G4VSensitiveDetector.hh"
#include "ActarSimSilGeantHit.hh"

class G4Step;
class G4HCofThisEvent;

class ActarSimSilSD : public G4VSensitiveDetector {
private:
  ActarSimSilGeantHitsCollection* hitsCollection;  ///< Geant step-like hits collect.

public:
  ActarSimSilSD(G4String);
  ~ActarSimSilSD();

  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*,G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);
};
#endif
