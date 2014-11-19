/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez Pol    hapolyo@usc.es
//*-- Date: 04/2008
//*-- Last Update: 
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

#ifndef ActarSimSilRingSD_h
#define ActarSimSilRingSD_h 1

#include "G4VSensitiveDetector.hh"
#include "ActarSimSilRingGeantHit.hh"

class G4Step;
class G4HCofThisEvent;

class ActarSimSilRingSD : public G4VSensitiveDetector {

private:
  ActarSimSilRingGeantHitsCollection* hitsCollection; //Geant step-like hits collect.

public:
  ActarSimSilRingSD(G4String);
  ~ActarSimSilRingSD();
  
  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*,G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);
};
#endif

