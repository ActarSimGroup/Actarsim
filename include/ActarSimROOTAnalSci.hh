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

#ifndef ActarSimROOTAnalSci_h
#define ActarSimROOTAnalSci_h 1

#include "ActarSimROOTAnalysis.hh"

class ActarSimSciHit;
class ActarSimSciGeantHit;

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

class ActarSimROOTAnalSci{
private:

  char* dirName;
 
  TFile* simFile;               //Local pointer to simFile
  TTree* eventTree; //Tree

  TBranch* sciHitsBranch; //Local branch

  ActarSimSciHit** theSciHit; //Data
  TClonesArray* sciHitCA;

  G4PrimaryParticle* primary;//Storing the primary for accesing during UserStep

  G4int theRunID; //To keep some numbers on the Tree
  G4int theEventID; //To keep some numbers on the Tree

public:

  ActarSimROOTAnalSci();
  ~ActarSimROOTAnalSci();

  G4int GetTheEventID(){return theEventID;}
  void SetTheEventID(G4int id){theEventID = id;}
  G4int GetTheRunID(){return theRunID;}
  void SetTheRunID(G4int id){theRunID = id;}
  
  TBranch* GetSciHitsBranch(){return sciHitsBranch;}
  void SetSciHitsBranch(TBranch* aBranch) {sciHitsBranch= aBranch;}
  
  TClonesArray* getSciHitsCA(void){return sciHitCA;} 
  void SetSciHitsCA(TClonesArray* CA) {sciHitCA = CA;}

  void FillingHits(const G4Event *anEvent);
  void AddCalCrystalHit(ActarSimSciHit*,ActarSimSciGeantHit*,G4int);

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

