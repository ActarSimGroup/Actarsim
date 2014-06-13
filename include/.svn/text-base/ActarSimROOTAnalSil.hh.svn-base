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

#ifndef ActarSimROOTAnalSil_h
#define ActarSimROOTAnalSil_h 1

#include "ActarSimROOTAnalysis.hh"

#include "ActarSimSilHit.hh"
#include "ActarSimSilGeantHit.hh"

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

class ActarSimROOTAnalSil{
private:

  char* dirName;

  ActarSimSilHit** theSilHit;
  TClonesArray* silHitCA;

  TFile* simFile;               //Local pointer to simFile
  TTree* eventTree; //Tree

  TBranch* silHitsBranch; //Local branch

  G4int theRunID; //To keep some numbers on the Tree
  G4int theEventID; //To keep some numbers on the Tree

public:

  ActarSimROOTAnalSil();
  ~ActarSimROOTAnalSil();

  G4int GetTheEventID(){return theEventID;}
  void SetTheEventID(G4int id){theEventID = id;}
  G4int GetTheRunID(){return theRunID;}
  void SetTheRunID(G4int id){theRunID = id;}

  TBranch* GetSilHitsBranch(){return silHitsBranch;}
  void SetSilHitsBranch(TBranch* aBranch) {silHitsBranch= aBranch;}

  TClonesArray* getSilHitsCA(void){return silHitCA;}
  void SetSilHitsCA(TClonesArray* CA) {silHitCA = CA;}

  // G4VUserPrimaryGeneratorAction
  void GeneratePrimaries(const G4Event*);

  // G4UserRunAction
  void BeginOfRunAction(const G4Run*);
  //void EndOfRunAction(const G4Run*);

  // G4UserEventAction
  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);

  void FillingHits(const G4Event*);

  void AddSilHit(ActarSimSilHit* cHit,
		 ActarSimSilGeantHit* gHit,
		 G4int mode);

  // G4UserSteppingAction
  void UserSteppingAction(const G4Step*);

};
#endif

