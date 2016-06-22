// - AUTHOR: Hector Alvarez-Pol 04/2008
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

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

  ActarSimSilRingHit** theSilRingHit;  ///< Pointer to the hits in the silicon ring
  TClonesArray* silRingHitCA;          ///< ClonesArray of the hits in the silicon ring

  TFile* simFile;                      ///< Local pointer to simFile
  TTree* eventTree;                    ///< Local pointer to the event tree

  TBranch* silRingHitsBranch;          ///< Local branch for the silicon hits

  G4int theRunID;                      ///< Run ID
  G4int theEventID;                    ///< Event ID

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

  void GeneratePrimaries(const G4Event*);

  void BeginOfRunAction(const G4Run*);
  //void EndOfRunAction(const G4Run*);

  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);

  void FillingHits(const G4Event*);

  void AddSilRingHit(ActarSimSilRingHit* cHit,
		 ActarSimSilRingGeantHit* gHit,
		 G4int mode);

  void UserSteppingAction(const G4Step*);
};
#endif
