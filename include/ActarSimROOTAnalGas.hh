/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez Pol
//*-- Date: 04/2008
//*-- Last Update: 14/06/16 by Hector Alvarez Pol
// --------------------------------------------------------------
// Description:
//   The ACTAR gas detector part of the ROOT Analysis
//
// --------------------------------------------------------------
// Comments:
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

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
class TBranch;
class TFile;
class TClonesArray;
class TProfile; // for hbeamEnergyAtRange, to get the energy loss as a function of the path length

class ActarSimData;
class ActarSimTrack;
class ActarSimSimpleTrack;

class ActarSimROOTAnalGas{
private:

  char* dirName;

  ActarSimData* theData; //Data
  ActarSimTrack* theTracks; //Data tracks

  ActarSimSimpleTrack** simpleTrack; //the two simple data track
  TClonesArray*  simpleTrackCA;

  TFile* simFile;               //Local pointer to simFile
  TTree* eventTree; //Tree
  TTree* tracksTree; //Tree

  TBranch* dataBranch; //Local branch
  TBranch* simpleTrackBranch; //Local branch
  TBranch* trackBranch; //Local branch

  TH1D *hStepSumLengthOnGas1;   // Step Length
  TH1D *hStepSumLengthOnGas2;   // Step Length

  //energy loss on the gas
  TH1D *hTotELossOnGas1; // Energy Loss of primary 1 on Gas
  TH1D *hTotELossOnGas2; // Energy Loss of primary 2 on Gas

  TH2D *htrackInPads;
  TH2D *htrack1InPads;
  TH2D *htrack2InPads;

  TH1D *hEdepInGas;// Energy deposit in Gas over the distance

  ///// The accumulated energy loss and track length of each step, dypang 080225
  TProfile *hbeamEnergyAtRange;
  ///// end of dypang part 080225

  TH2D *htrackFromBeam;

  TH3D *htrack;

  //Control of minimum simpleTrack stride length
  G4double minStrideLength;

  G4PrimaryParticle* primary;//Storing the primary for accesing during UserStep
  G4int primNbOfParticles;
  G4double primEnergy;
  G4double primTheta;
  G4double primPhi;

  G4int theRunID; //To keep some numbers on the Tree
  G4int theEventID; //To keep some numbers on the Tree

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

  TBranch* GetDataBranch(){return dataBranch;}
  void SetDataBranch(TBranch* aBranch) {dataBranch= aBranch;}
  TBranch* GetTrackBranch(){return trackBranch;}
  void SetTrackBranch(TBranch* aBranch) {trackBranch= aBranch;}
  TBranch* GetSimpleTrackBranch(){return simpleTrackBranch;}
  void SetSimpleTrackBranch(TBranch* aBranch) {simpleTrackBranch= aBranch;}

  TClonesArray* getSimpleTrackCA(void){return simpleTrackCA;}
  void SetSimpleTrackCA(TClonesArray* CA) {simpleTrackCA = CA;}

  // G4VUserPrimaryGeneratorAction
  void GeneratePrimaries(const G4Event*);

  // G4UserRunAction
  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction(const G4Run*);

  // G4UserEventAction
  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);

  // G4UserSteppingAction
  void UserSteppingAction(const G4Step*);

};
#endif
