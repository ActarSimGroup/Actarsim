/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez Pol     hapol@fpddux.usc.es
//*-- Date: 04/2008
//*-- Last Update: 14/04/08 by Hector Alvarez Pol
// --------------------------------------------------------------
// Description:
//   The ACTAR Silicon detector part of the ROOT Analysis
//
// --------------------------------------------------------------
// Comments:
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#ifndef ActarSimROOTAnalParis_h
#define ActarSimROOTAnalParis_h 1

#include "ActarSimROOTAnalysis.hh"

#include "ActarSimParisHit.hh"
#include "ParisSingleHit.hh"

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

class ActarSimROOTAnalParis{
private:

  char* dirName;

  ActarSimParisHit** theParisHit;
  TClonesArray* parisHitCA;

  TFile* simFile;               //Local pointer to simFile
  TTree* eventTree; //Tree

  TBranch* parisHitsBranch; //Local branch

  G4int theRunID; //To keep some numbers on the Tree
  G4int theEventID; //To keep some numbers on the Tree

public:

  ActarSimROOTAnalParis();
  ~ActarSimROOTAnalParis();

  G4int GetTheEventID(){return theEventID;}
  void SetTheEventID(G4int id){theEventID = id;}
  G4int GetTheRunID(){return theRunID;}
  void SetTheRunID(G4int id){theRunID = id;}

  TBranch* GetParisHitsBranch(){return parisHitsBranch;}
  void SetParisHitsBranch(TBranch* aBranch) {parisHitsBranch= aBranch;}

  TClonesArray* GetParisHitsCA(void){return parisHitCA;}
  void SetParisHitsCA(TClonesArray* CA) {parisHitCA = CA;}

  // G4VUserPrimaryGeneratorAction
  void GeneratePrimaries(const G4Event*);

  // G4UserRunAction
  void BeginOfRunAction(const G4Run*);
  //void EndOfRunAction(const G4Run*);

  // G4UserEventAction

  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);

  void FillingHits(const G4Event*);

  void AddParisHit(ActarSimParisHit* cHit,
		 ParisSingleHit* gHit,
		 G4int mode);

  // G4UserSteppingAction
  void UserSteppingAction(const G4Step*);

};
#endif

