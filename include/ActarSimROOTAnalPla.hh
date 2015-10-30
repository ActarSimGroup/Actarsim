/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez Pol
//*-- Date: 04/2008
//*-- Last Update: 23/12/14 by Hector Alvarez Pol
// --------------------------------------------------------------
// Description:
//   The Plastic Scintillator detector part of the ROOT Analysis
//
// --------------------------------------------------------------
// Comments:
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#ifndef ActarSimROOTAnalPla_h
#define ActarSimROOTAnalPla_h 1

#include "ActarSimROOTAnalysis.hh"

class ActarSimPlaHit;
class ActarSimPlaGeantHit;

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

class ActarSimROOTAnalPla{
private:

  char* dirName;

  TFile* simFile;               //Local pointer to simFile
  TTree* eventTree; //Tree

  TBranch* plaHitsBranch; //Local branch

  ActarSimPlaHit** thePlaHit; //Data
  TClonesArray* plaHitCA;

  //G4PrimaryParticle* primary;//Storing the primary for accesing during UserStep NOT USED

  G4int theRunID; //To keep some numbers on the Tree
  G4int theEventID; //To keep some numbers on the Tree

public:

  ActarSimROOTAnalPla();
  ~ActarSimROOTAnalPla();

  G4int GetTheEventID(){return theEventID;}
  void SetTheEventID(G4int id){theEventID = id;}
  G4int GetTheRunID(){return theRunID;}
  void SetTheRunID(G4int id){theRunID = id;}

  TBranch* GetPlaHitsBranch(){return plaHitsBranch;}
  void SetPlaHitsBranch(TBranch* aBranch) {plaHitsBranch= aBranch;}

  TClonesArray* getPlaHitsCA(void){return plaHitCA;}
  void SetPlaHitsCA(TClonesArray* CA) {plaHitCA = CA;}

  void FillingHits(const G4Event *anEvent);
  void AddCalPlaHit(ActarSimPlaHit*,ActarSimPlaGeantHit*,G4int);

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
