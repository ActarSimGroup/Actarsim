/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez Pol 
//*-- Date: 04/2008
//*-- Last Update: 07/01/15 by Hector Alvarez Pol
// --------------------------------------------------------------
// Description:
//   The ACTAR Silicon detector part of the ROOT Analysis
//
// --------------------------------------------------------------
// Comments:
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#ifndef ActarSimROOTAnalSilRing_h
#define ActarSimROOTAnalSilRing_h 1

#include "ActarSimROOTAnalysis.hh"

#include "ActarSimSilRingHit.hh"
#include "ActarSimSilRingGeantHit.hh"

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

class ActarSimROOTAnalSilRing{
private:

  char* dirName;

  ActarSimSilRingHit** theSilRingHit;
  TClonesArray* silRingHitCA;

  TFile* simFile;               //Local pointer to simFile
  TTree* eventTree; //Tree

  TBranch* silRingHitsBranch; //Local branch

  G4int theRunID; //To keep some numbers on the Tree
  G4int theEventID; //To keep some numbers on the Tree

public:

  ActarSimROOTAnalSilRing();
  ~ActarSimROOTAnalSilRing();

  G4int GetTheEventID(){return theEventID;}
  void SetTheEventID(G4int id){theEventID = id;}
  G4int GetTheRunID(){return theRunID;}
  void SetTheRunID(G4int id){theRunID = id;}

  TBranch* GetSilRingHitsBranch(){return silRingHitsBranch;}
  void SetSilRingHitsBranch(TBranch* aBranch) {silRingHitsBranch= aBranch;}

  TClonesArray* getSilRingHitsCA(void){return silRingHitCA;}
  void SetSilRingHitsCA(TClonesArray* CA) {silRingHitCA = CA;}

  // G4VUserPrimaryGeneratorAction
  void GeneratePrimaries(const G4Event*);

  // G4UserRunAction
  void BeginOfRunAction(const G4Run*);
  //void EndOfRunAction(const G4Run*);

  // G4UserEventAction
  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);

  void FillingHits(const G4Event*);

  void AddSilRingHit(ActarSimSilRingHit* cHit,
		 ActarSimSilRingGeantHit* gHit,
		 G4int mode);

  // G4UserSteppingAction
  void UserSteppingAction(const G4Step*);

};
#endif

