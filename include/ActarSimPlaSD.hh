// - AUTHOR: Hector Alvarez-Pol 04/2008
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimPlaSD_h
#define ActarSimPlaSD_h 1

#include "G4VSensitiveDetector.hh"
#include "ActarSimPlaGeantHit.hh"

class G4Step;
class G4HCofThisEvent;

class ActarSimPlaSD : public G4VSensitiveDetector {
private:
  ActarSimPlaGeantHitsCollection* hitsCollection; ///< Geant step-like hits collect.

public:
  ActarSimPlaSD(G4String);
  ~ActarSimPlaSD();

  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*,G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);
};
#endif
