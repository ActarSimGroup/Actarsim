// - AUTHOR: Hector Alvarez-Pol 04/2008
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

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

  ActarSimSilHit** theSilHit;          ///< Pointer to the hits in the silicons
  TClonesArray* silHitCA;              ///< ClonesArray of the hits in the silicons

  TFile* simFile;                      ///< Local pointer to simFile
  TTree* eventTree;                    ///< Local pointer to the event tree

  TBranch* silHitsBranch;              ///< Local branch for the silicon hits

  G4int theRunID;                     ///< Run ID
  G4int theEventID;                   ///< Event ID

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

  void GeneratePrimaries(const G4Event*);

  void BeginOfRunAction(const G4Run*);
  //void EndOfRunAction(const G4Run*);

  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);

  void FillingHits(const G4Event*);

  void AddSilHit(ActarSimSilHit* cHit,
		 ActarSimSilGeantHit* gHit,
		 G4int mode);

  void UserSteppingAction(const G4Step*);
};
#endif
