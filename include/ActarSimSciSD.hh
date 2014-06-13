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

#ifndef ActarSimSciSD_h
#define ActarSimSciSD_h 1

#include "G4VSensitiveDetector.hh"
#include "ActarSimSciGeantHit.hh"

class G4Step;
class G4HCofThisEvent;

class ActarSimSciSD : public G4VSensitiveDetector {

private:
  ActarSimSciGeantHitsCollection* hitsCollection; //Geant step-like hits collect.

public:
  ActarSimSciSD(G4String);
  ~ActarSimSciSD();
  
  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*,G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);
};
#endif

