/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez Pol
//*-- Date: 04/2008
//*-- Last Update: 07/01/15
// --------------------------------------------------------------
// Description:
//   SD for the Silicons
//
// --------------------------------------------------------------
// Comments:
//
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#ifndef ActarSimSilSD_h
#define ActarSimSilSD_h 1

#include "G4VSensitiveDetector.hh"
#include "ActarSimSilGeantHit.hh"

class G4Step;
class G4HCofThisEvent;

class ActarSimSilSD : public G4VSensitiveDetector {

private:
  ActarSimSilGeantHitsCollection* hitsCollection; //Geant step-like hits collect.

public:
  ActarSimSilSD(G4String);
  ~ActarSimSilSD();
  
  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*,G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);
};
#endif

