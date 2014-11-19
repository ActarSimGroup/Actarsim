/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez Pol    hapolyo@usc.es
//*-- Date: 04/2008
//*-- Last Update: 
// --------------------------------------------------------------
// Description:
//   SD for the Scintillators
//
// --------------------------------------------------------------
// Comments:
//
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#ifndef ActarSimSciRingSD_h
#define ActarSimSciRingSD_h 1

#include "G4VSensitiveDetector.hh"
#include "ActarSimSciRingGeantHit.hh"

class G4Step;
class G4HCofThisEvent;

class ActarSimSciRingSD : public G4VSensitiveDetector {

private:
  ActarSimSciRingGeantHitsCollection* hitsCollection; //Geant step-like hits collect.

public:
  ActarSimSciRingSD(G4String);
  ~ActarSimSciRingSD();
  
  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*,G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);
};
#endif

