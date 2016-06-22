// - AUTHOR: Hector Alvarez-Pol 01/2006
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

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

class ActarSimROOTAnalSci {
private:
  char* dirName;

  TFile* simFile;             ///< Local pointer to simFile
  TTree* eventTree;           ///< Local pointer to the event tree

  TBranch* sciHitsBranch;     ///< Local branch for scintillators

  ActarSimSciHit** theSciHit; ///< Pointer to the hits in the scintillators
  TClonesArray* sciHitCA;     ///< ClonesArray of the hits in the scintillators

  //G4PrimaryParticle* primary;//Storing the primary for accesing during UserStep NOT USED

  G4int theRunID;            ///< Run ID
  G4int theEventID;          ///< Event ID

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

  void GeneratePrimaries(const G4Event*);

  void BeginOfRunAction(const G4Run*);
  //void EndOfRunAction(const G4Run*);

  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);

  void UserSteppingAction(const G4Step*);
};
#endif
