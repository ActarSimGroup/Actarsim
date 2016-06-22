// - AUTHOR: Hector Alvarez-Pol 04/2008
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

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

class ActarSimROOTAnalPla {
private:
  char* dirName;

  TFile* simFile;              ///< Local pointer to simFile
  TTree* eventTree;            ///< Local pointer to the event tree

  TBranch* plaHitsBranch;      ///< Local branch for plastics

  ActarSimPlaHit** thePlaHit;  ///< Pointer to the hits in the plastic
  TClonesArray* plaHitCA;      ///< ClonesArray of the hits in the plastic

  //G4PrimaryParticle* primary;//Storing the primary for accesing during UserStep NOT USED

  G4int theRunID;             ///< Run ID
  G4int theEventID;           ///< Event ID

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

  void GeneratePrimaries(const G4Event*);

  void BeginOfRunAction(const G4Run*);
  //void EndOfRunAction(const G4Run*);

  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);

  void UserSteppingAction(const G4Step*);
};
#endif
