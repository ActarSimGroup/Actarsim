// - AUTHOR: Hector Alvarez-Pol 04/2008
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimROOTAnalGas_h
#define ActarSimROOTAnalGas_h 1

#include "ActarSimROOTAnalysis.hh"

class G4VPhysicalVolume;
class G4Event;
class G4Run;
class G4Track;
class G4Step;

#include "G4ClassificationOfNewTrack.hh"
#include "G4TrackStatus.hh"
#include "G4Types.hh"
#include "G4PrimaryParticle.hh"
#include "ActarSimGasGeantHit.hh"

class TH1D;
class TH2D;
class TH3D;
class TTree;
//class TBranch;
class TFile;
class TClonesArray;
class TProfile; // for hbeamEnergyAtRange, to get the energy loss as a function of the path length

class ActarSimData;
class ActarSimTrack;
class ActarSimSimpleTrack;

class ActarSimROOTAnalGas {
private:
  char* dirName;

  ActarSimData* theData;    ///< Pointer to data
  ActarSimTrack* theTracks; ///< Pointer to tracks

  ActarSimSimpleTrack** simpleTrack; ///< Pointer to simple tracks
  TClonesArray*  simpleTrackCA;      ///< ClonesArray for simple tracks

  TFile* simFile;               ///< Local pointer to simFile
  TTree* eventTree;             ///< Local pointer to the event tree
  TTree* tracksTree;            ///< Local pointer to the tracks tree

  TH1D *hStepSumLengthOnGas1;   ///< Histogram: step Length on gas for first primary
  TH1D *hStepSumLengthOnGas2;   ///< Histogram: step Length on gas for second primary

  TH1D *hTotELossOnGas1;    ///< Histogram: energy Loss on gas for first primary
  TH1D *hTotELossOnGas2;    ///< Histogram: energy Loss on gas for second primary

  TH2D *htrackInPads;       ///< Histogram: all tracks projected in pads
  TH2D *htrack1InPads;      ///< Histogram: first primary track projected in pads
  TH2D *htrack2InPads;      ///< Histogram: second primary track projected in pads

  TH1D *hEdepInGas;         ///< Histogram: energy deposit in Gas over the distance

  TProfile *hbeamEnergyAtRange; ///< Histogram profile: ccumulated energy loss and track length of each step

  TH2D *htrackFromBeam;     ///< Histogram: tracks from beam direction

  TH3D *htrack;             ///< Histogram: 3D track

  G4double minStrideLength; ///< Control of minimum simpleTrack stride length

  G4PrimaryParticle* primary;  ///< Storing the primary for accesing during UserStep
  G4int primNbOfParticles;     ///< Number of primaries
  G4double primEnergy;         ///< Energy of primary
  G4double primTheta;          ///< Theta of primary
  G4double primPhi;            ///< Phi of primary

  G4int theRunID;      ///< Run ID
  G4int theEventID;    ///< Event ID

public:
  ActarSimROOTAnalGas();
  ~ActarSimROOTAnalGas();

  void init();

  TTree* GetEventTree(){return eventTree;}
  void SetEventTree(TTree* tree) {eventTree = tree;}
  TTree* GetTracksTree(){return tracksTree;}
  void SetTracksTree(TTree* tree) {tracksTree = tree;}

  G4int GetTheEventID(){return theEventID;}
  void SetTheEventID(G4int id){theEventID = id;}
  G4int GetTheRunID(){return theRunID;}
  void SetTheRunID(G4int id){theRunID = id;}

  G4int GetPrimNbOfParticles(){return primNbOfParticles;}
  G4double GetPrimEnergy(){return primEnergy;}
  G4double GetPrimTheta(){return primTheta;}
  G4double GetPrimPhi(){return primPhi;}

  void SetPrimNbOfParticles(G4int nb){primNbOfParticles = nb;}
  void SetPrimEnergy(G4double pe){primEnergy = pe;}
  void SetPrimTheta(G4double pt){primTheta = pt;}
  void SetPrimPhi(G4double pp){primPhi = pp;}

  void SetMinStrideLength(G4double val) {minStrideLength = val;};

  TClonesArray* getSimpleTrackCA(void){return simpleTrackCA;}
  void SetSimpleTrackCA(TClonesArray* CA) {simpleTrackCA = CA;}

  void GeneratePrimaries(const G4Event*);

  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction(const G4Run*);

  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);

  void UserSteppingAction(const G4Step*);
};
#endif
