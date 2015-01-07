/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez Pol
//*-- Date: 04/2008
//*-- Last Update: 16/12/14 by Hector Alvarez
// --------------------------------------------------------------
// Description:
//   SD for the plastic scintillators
//
// --------------------------------------------------------------
// Comments:
//
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#ifndef ActarSimPlaSD_h
#define ActarSimPlaSD_h 1

#include "G4VSensitiveDetector.hh"
#include "ActarSimPlaGeantHit.hh"

class G4Step;
class G4HCofThisEvent;

class ActarSimPlaSD : public G4VSensitiveDetector {

private:
  ActarSimPlaGeantHitsCollection* hitsCollection; //Geant step-like hits collect.

public:
  ActarSimPlaSD(G4String);
  ~ActarSimPlaSD();
  
  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*,G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);
};
#endif

