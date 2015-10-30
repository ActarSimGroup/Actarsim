/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez Pol
//*-- Date: 04/2008
//*-- Last Update: 07/01/15 by Hector Alvarez Pol
// --------------------------------------------------------------
// Description:
//   The ACTAR Scintillator detector part of the ROOT Analysis
//
// --------------------------------------------------------------
// Comments:
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#ifndef ActarSimROOTAnalSciRing_h
#define ActarSimROOTAnalSciRing_h 1

#include "ActarSimROOTAnalysis.hh"

class ActarSimSciRingHit;
class ActarSimSciRingGeantHit;

class G4VPhysicalVolume;
class G4Event;
class G4Run;
class G4Track;
class G4Step;

#include "G4ClassificationOfNewTrack.hh"
#include "G4TrackStatus.hh"
#include "G4Types.hh"
#include "G4PrimaryParticle.hh"

class TH1D;
class TH2D;
class TTree;
class TBranch;
class TFile;
class TClonesArray;

class ActarSimROOTAnalSciRing{
private:

  char* dirName;
 
  TFile* simFile;               //Local pointer to simFile
  TTree* eventTree; //Tree

  TBranch* sciRingHitsBranch; //Local branch

  ActarSimSciRingHit** theSciRingHit; //Data
  TClonesArray* sciRingHitCA;

  //G4PrimaryParticle* primary;//Storing the primary for accesing during UserStep NOT USED

  G4int theRunID; //To keep some numbers on the Tree
  G4int theEventID; //To keep some numbers on the Tree

public:

  ActarSimROOTAnalSciRing();
  ~ActarSimROOTAnalSciRing();

  G4int GetTheEventID(){return theEventID;}
  void SetTheEventID(G4int id){theEventID = id;}
  G4int GetTheRunID(){return theRunID;}
  void SetTheRunID(G4int id){theRunID = id;}

  TBranch* GetSciRingHitsBranch(){return sciRingHitsBranch;}
  void SetSciRingHitsBranch(TBranch* aBranch) {sciRingHitsBranch= aBranch;}

  TClonesArray* getSciRingHitsCA(void){return sciRingHitCA;}
  void SetSciRingHitsCA(TClonesArray* CA) {sciRingHitCA = CA;}

  void FillingHits(const G4Event *anEvent);
  void AddCalCrystalHit(ActarSimSciRingHit*,ActarSimSciRingGeantHit*,G4int);

  // G4VUserPrimaryGeneratorAction
  void GeneratePrimaries(const G4Event*);

  // G4UserRunAction
  void BeginOfRunAction(const G4Run*);
  //void EndOfRunAction(const G4Run*);

  // G4UserEventAction
  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);

  // G4UserSteppingAction
  void UserSteppingAction(const G4Step*);

};
#endif
