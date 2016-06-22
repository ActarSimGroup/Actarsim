// - AUTHOR: Hector Alvarez-Pol 03/2005
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

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
extern ActarSimROOTAnalysis *gActarSimROOTAnalysis; ///< Global pointer to this soliton

class ActarSimROOTAnalysis {
private:
  time_t LastDoItTime;   ///< Used in OnceAWhileDoIt method for recursivity
  TFile* simFile;        ///< The ROOT file
  char* newDirName;      ///< Directory name within ROOT file

  TTree* eventTree;      ///< Events tree
  TTree* tracksTree;     ///< Tracks tree

  ActarSimROOTAnalGas* gasAnal;         ///< Pointer to detector specific (gas chamber) analysis class
  ActarSimROOTAnalSil* silAnal;         ///< Pointer to detector specific (silicon) analysis class
  ActarSimROOTAnalSilRing* silRingAnal; ///< Pointer to detector specific (silicon ring) analysis class
  ActarSimROOTAnalSci* sciAnal;         ///< Pointer to detector specific (scintillator) analysis class
  ActarSimROOTAnalSciRing* sciRingAnal; ///< Pointer to detector specific (scintillator ring) analysis class
  ActarSimROOTAnalPla* plaAnal;         ///< Pointer to detector specific (plastic) analysis class

  ActarSimBeamInfo* pBeamInfo;          ///< Pointer to beam information object

  ActarSimAnalysisMessenger* analMessenger;  ///< Pointer to the corresponding messenger

  ActarSimData* theData;             ///< Pointer to data object

  TH1D *hPrimTheta;              ///< Histogram of primary Theta angle
  TH1D *hPrimPhi;                ///< Histogram of primary Phi angle
  TH1D *hPrimEnergy;             ///< Histogram of primary energy
  TH2D *hPrimEnergyVsTheta;      ///< Histogram of primary Energy vs Theta angle

  TH2F *hScatteredIonKinematic;  ///< Histogram with Cine Kinematic results for scattered ions
  TH2F *hRecoilIonKinematic;     ///< Histogram with Cine Kinematic results for recoil ions

  ActarSimPrimaryInfo** thePrimaryInfo; ///< Primary particles data
  TClonesArray* primaryInfoCA;          ///< ClonesArray of primaries info objects
  TClonesArray* beamInfoCA;             ///< ClonesArray of the beam info objects
  TClonesArray* theDataCA;              ///< ClonesArray of the data objects

  G4int theRunID;      ///< Particle Run ID
  G4int theEventID;    ///< Particle Event ID

  G4String  storeTracksFlag;       ///< Flag to turn "on"/"off" the storage of complete tracks
  G4String  storeTrackHistosFlag;  ///< Flag to turn "on"/"off" the storage of histograms related to the tracks
  G4String  storeEventsFlag;       ///< Flag to turn "on"/"off" the storage fo events
  G4String  storeSimpleTracksFlag; ///< Flag to turn "on"/"off" the storage of simple tracks
  G4String  storeHistogramsFlag;   ///< Flag to turn "on"/"off" the storage of general histograms
  G4String  beamInteractionFlag;   ///< Flag to turn "on"/"off" the beam interaction analysis

  G4int gasAnalIncludedFlag;     ///< Flag to turn on(1)/off(0) the gas chamber analysis
  G4int silAnalIncludedFlag;     ///< Flag to turn on(1)/off(0) the silicon analysis
  G4int silRingAnalIncludedFlag; ///<Flag to turn on(1)/off(0) the silicon ring analysis
  G4int sciAnalIncludedFlag;     ///< Flag to turn on(1)/off(0) the scintillator analysis
  G4int sciRingAnalIncludedFlag; ///< Flag to turn on(1)/off(0) the scintillator ring analysis
  G4int plaAnalIncludedFlag;     ///< Flag to turn on(1)/off(0) the plastic analysis

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
  void SetMinStrideLength(Double_t value);

  void Construct(const G4VPhysicalVolume*);

  void ConstructParticle();
  void ConstructProcess();
  void SetCuts();

  //TODO->Solve this assymetry!
  //void GeneratePrimaries(const G4Event*);
  void GeneratePrimaries(const G4Event*,G4double,G4double,G4double,G4double);
  void GeneratePrimaries(const G4Event *anEvent, ActarSimBeamInfo *beamInfo);
  void GenerateBeam(const G4Event*);

  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction(const G4Run*);

  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);

  void ClassifyNewTrack(const G4Track*, G4ClassificationOfNewTrack*);

  void PreUserTrackingAction(const G4Track*);
  void PostUserTrackingAction(const G4Track*, G4TrackStatus*);

  void UserSteppingAction(const G4Step*); // original

  void OnceAWhileDoIt(const G4bool DoItNow = false);
};
#endif
