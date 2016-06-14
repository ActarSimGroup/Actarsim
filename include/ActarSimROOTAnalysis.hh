/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol
//*-- Date: 03/2005
//*-- Last Update: 14/06/16 by hapol
// --------------------------------------------------------------
// Description:
//   ROOT-based analysis functionality
//
// --------------------------------------------------------------
// Comments:
//   - 14/04/08 Changing the structure for individual
//              detector analysis, similar to R3BSim
//   - 16/03/05 Created based on Ica4 example structure
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#ifndef ActarSimROOTAnalysis_h
#define ActarSimROOTAnalysis_h 1

class G4VPhysicalVolume;
class G4Event;
class G4Run;
class G4Track;
class G4Step;

#include "G4ClassificationOfNewTrack.hh"
#include "G4TrackStatus.hh"
#include "G4Types.hh"
#include "G4PrimaryParticle.hh"

#include <time.h>

#include "TClonesArray.h"

class TH1D;
class TH2D;
class TH3D;
class TTree;
class TBranch;
class TFile;
class TClonesArray;
class TH2F;

class ActarSimAnalysisMessenger;

class ActarSimROOTAnalGas;
class ActarSimROOTAnalSil;
class ActarSimROOTAnalSilRing;
class ActarSimROOTAnalSci;
class ActarSimROOTAnalSciRing;
class ActarSimROOTAnalPla;

class ActarSimDetectorConstruction;
class ActarSimPrimaryGeneratorAction;
class ActarSimPrimaryInfo;
class ActarSimBeamInfo;

class ActarSimData;
class ActarSimTrack;
class ActarSimSimpleTrack;

class ActarSimROOTAnalysis;
extern ActarSimROOTAnalysis *gActarSimROOTAnalysis; // global

class ActarSimROOTAnalysis {

private:

  time_t LastDoItTime; // used in OnceAWhileDoIt method

  TFile* simFile;     // The ROOT File
  char* newDirName;

  TTree* eventTree; //Tree
  TTree* tracksTree; //Tree

  ActarSimROOTAnalGas* gasAnal;     // detector specific
  ActarSimROOTAnalSil* silAnal;
  ActarSimROOTAnalSilRing* silRingAnal;
  ActarSimROOTAnalSci* sciAnal;
  ActarSimROOTAnalSciRing* sciRingAnal;
  ActarSimROOTAnalPla* plaAnal;

  ActarSimBeamInfo* pBeamInfo;

  //G4PrimaryParticle* primary;      //Storing the primary for accesing during UserStep //NOT USED
  ActarSimAnalysisMessenger* analMessenger; // pointer to messenger

  TBranch* primaryInfoBranch; //Local primaries branch
  TBranch* beamInfoBranch;    //Beam Info branch

  //TClonesArray*  simpleTrackCA; //NOT USED

  ActarSimData* theData; //Data
  //ActarSimTrack* theTracks; //Data tracks
  //ActarSimSimpleTrack** simpleTrack; //the two simple data track NOT USED

  //G4double primTheta; //NOT USED
  //G4double primPhi; //NOT USED

  //primary physics (momentum and mass are given by the pointer)
  TH1D *hPrimTheta;              // Primary Theta angle
  TH1D *hPrimPhi;                // Primary Phi angle
  TH1D *hPrimEnergy;             // Primary energy
  TH2D *hPrimEnergyVsTheta;      // Primary Energy vs Theta angle

  //histograms for the Cine Kinematic Results
  TH2F *hScatteredIonKinematic;
  TH2F *hRecoilIonKinematic;

  ActarSimPrimaryInfo** thePrimaryInfo; //Primary particles data
  TClonesArray* primaryInfoCA;

  G4int theRunID; //To keep some numbers on the Tree
  G4int theEventID; //To keep some numbers on the Tree

  //Flags for control of gas tracks...
  G4String  storeTracksFlag;
  G4String  storeTrackHistosFlag;
  G4String  storeEventsFlag;
  G4String  storeSimpleTracksFlag;
  G4String  storeHistogramsFlag;
  G4String  beamInteractionFlag;  //flag to turn "on"/"off" the beam interaction analysis

  G4int gasAnalIncludedFlag; //flag to turn on(1)/off(0) the calorim. analysis
  G4int silAnalIncludedFlag; //flag to turn on(1)/off(0) the tracker analysis
  G4int silRingAnalIncludedFlag;
  G4int sciAnalIncludedFlag; //flag to turn on(1)/off(0) the DCH analysis
  G4int sciRingAnalIncludedFlag;
  G4int plaAnalIncludedFlag;

public:

  ActarSimROOTAnalysis();
  ~ActarSimROOTAnalysis();

  TFile* GetSimFile(){return simFile;}
  void SetSimFile(TFile* file) {simFile = file;}

  TTree* GetEventTree(){return eventTree;}
  void SetEventTree(TTree* tree) {eventTree = tree;}
  TTree* GetTracksTree(){return tracksTree;}
  void SetTracksTree(TTree* tree) {tracksTree = tree;}
  ActarSimData* GetTheData(){return theData;}
  void SetTheData(ActarSimData* data){theData = data;}
  //ActarSimTrack* GetTheTracks(){return theTracks;}
  //void SetTheTracks(ActarSimTrack* tr){theTracks = tr;}

  ActarSimBeamInfo* GetBeamInfo(){return pBeamInfo;}

  G4int GetTheEventID(){return theEventID;}
  void SetTheEventID(G4int id){theEventID = id;}
  G4int GetTheRunID(){return theRunID;}
  void SetTheRunID(G4int id){theRunID = id;}

  void SetGasAnalOn(){gasAnalIncludedFlag=1;}
  void SetSilAnalOn(){silAnalIncludedFlag=1;}
  void SetSilRingAnalOn(){silRingAnalIncludedFlag=1;}
  void SetSciAnalOn(){sciAnalIncludedFlag=1;}
  void SetSciRingAnalOn(){sciRingAnalIncludedFlag=1;}
  void SetPlaAnalOn(){plaAnalIncludedFlag=1;}
  void SetGasAnalOff(){gasAnalIncludedFlag=0;}
  void SetSilAnalOff(){silAnalIncludedFlag=0;}
  void SetSciAnalOff(){sciAnalIncludedFlag=0;}
  void SetSciRingAnalOff(){sciRingAnalIncludedFlag=0;}
  void SetPlaAnalOff(){plaAnalIncludedFlag=0;}

  //Messenger actions
  void SetStoreTracksFlag(G4String val) {storeTracksFlag = val;};
  void SetStoreTrackHistosFlag(G4String val) {storeTrackHistosFlag = val;};
  void SetStoreEventsFlag(G4String val) {storeEventsFlag = val;};
  void SetStoreSimpleTracksFlag(G4String val) {storeSimpleTracksFlag=val;};
  void SetStoreHistogramsFlag(G4String val) {storeHistogramsFlag=val;};
  void SetBeamInteractionFlag(G4String val){beamInteractionFlag=val;}

  G4String GetStoreTracksFlag() {return storeTracksFlag;}
  G4String GetStoreTrackHistosFlag() {return storeTrackHistosFlag;}
  G4String GetStoreEventsFlag() {return storeEventsFlag;}
  G4String GetStoreSimpleTracksFlag() {return storeSimpleTracksFlag;}
  G4String GetStoreHistogramsFlag() {return storeHistogramsFlag;}
  G4String GetBeamInteractionFlag(){return beamInteractionFlag;}

  G4int GetGasAnalStatus(){return gasAnalIncludedFlag;}
  G4int GetSilAnalStatus(){return silAnalIncludedFlag;}
  G4int GetSilRingAnalStatus(){return silRingAnalIncludedFlag;}
  G4int GetSciAnalStatus(){return sciAnalIncludedFlag;}
  G4int GetSciRingAnalStatus(){return sciRingAnalIncludedFlag;}
  G4int GetPlaAnalStatus(){return plaAnalIncludedFlag;}

  void InitAnalysisForExistingDetectors();

  //DPL 29NOV2012
  void SetMinStrideLength(Double_t value);

 // G4VUserDetectorConstruction
  void Construct(const G4VPhysicalVolume*);

  // G4VUserPhysicsList
  void ConstructParticle();
  void ConstructProcess();
  void SetCuts();

  // G4VUserPrimaryGeneratorAction
  //TODO->Solve this assymetry!
  //void GeneratePrimaries(const G4Event*);
  void GeneratePrimaries(const G4Event*,G4double,G4double,G4double,G4double);
  void GeneratePrimaries(const G4Event *anEvent, ActarSimBeamInfo *beamInfo);
  void GenerateBeam(const G4Event*);

  // G4UserRunAction
  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction(const G4Run*);

  // G4UserEventAction
  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);

  // G4UserStackingAction
  void ClassifyNewTrack(const G4Track*, G4ClassificationOfNewTrack*);
  void NewStage();
  void PrepareNewEvent();

  // G4UserTrackingAction
  void PreUserTrackingAction(const G4Track*);
  void PostUserTrackingAction(const G4Track*, G4TrackStatus*);

  // G4UserSteppingAction
  void UserSteppingAction(const G4Step*); // original

  // once a while do "something"
  void OnceAWhileDoIt(const G4bool DoItNow = false);

};

#endif
