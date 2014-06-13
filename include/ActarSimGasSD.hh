/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez Pol    hapol@fpddux.usc.es
//*-- Date: 04/2006
//*-- Last Update: 13/06/06 by Hector Alvarez
// --------------------------------------------------------------
// Description:
//   SD for the gas volume in the detector
//
// --------------------------------------------------------------
// Comments:
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#ifndef ActarSimGasSD_h
#define ActarSimGasSD_h 1

#include "G4VSensitiveDetector.hh"
#include "ActarSimGasGeantHit.hh"

class G4Step;
class G4HCofThisEvent;

class ActarSimGasSD : public G4VSensitiveDetector {

private:
  ActarSimGasGeantHitsCollection* hitsCollection; //Geant step-like hits collect.

public:
  ActarSimGasSD(G4String);
  ~ActarSimGasSD();
  
  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*,G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);
};
#endif

