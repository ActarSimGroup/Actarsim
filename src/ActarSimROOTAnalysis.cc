// - AUTHOR: Hector Alvarez-Pol 03/2005
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/
//////////////////////////////////////////////////////////////////
/// \class ActarSimROOTAnalysis
/// ROOT-based analysis functionality
/////////////////////////////////////////////////////////////////

#include "ActarSimROOTAnalysis.hh"

#include "ActarSimROOTAnalGas.hh"
#include "ActarSimROOTAnalSci.hh"
#include "ActarSimROOTAnalSil.hh"
#include "ActarSimROOTAnalSilRing.hh"
#include "ActarSimROOTAnalSciRing.hh"
#include "ActarSimROOTAnalPla.hh"

#include "ActarSimDetectorConstruction.hh"
#include "ActarSimAnalysisMessenger.hh"

#include "ActarSimPrimaryGeneratorAction.hh"
#include "ActarSimPrimaryInfo.hh"

#include "ActarSimData.hh"
#include "ActarSimTrack.hh"
#include "ActarSimSimpleTrack.hh"

#include "ActarSimBeamInfo.hh"

#include "G4ios.hh"

#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Event.hh"
#include "G4Run.hh"
#include "G4Track.hh"
#include "G4ClassificationOfNewTrack.hh"
#include "G4TrackStatus.hh"
#include "G4Step.hh"
#include "G4Types.hh"
//#include "G4VVisManager.hh"

#include "G4Trajectory.hh"

#include <time.h>

#include "TROOT.h"
#include "TApplication.h"
#include "TSystem.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TTree.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TClonesArray.h"

//global pointer to the ROOT analysis manager
ActarSimROOTAnalysis *gActarSimROOTAnalysis = (ActarSimROOTAnalysis *)0;

//////////////////////////////////////////////////////////////////
/// Constructor
ActarSimROOTAnalysis::ActarSimROOTAnalysis():
  storeTracksFlag("off"), storeTrackHistosFlag("off"),
  storeEventsFlag("off"), storeSimpleTracksFlag("on"),
  storeHistogramsFlag("off"), beamInteractionFlag("off") {

  LastDoItTime = (time_t)0;
  if(gSystem) gSystem->ProcessEvents();

  if(gActarSimROOTAnalysis)
    delete gActarSimROOTAnalysis;
  gActarSimROOTAnalysis = this;

  //create a messenger for this class
  analMessenger = new ActarSimAnalysisMessenger(this);

  newDirName = new char[255];

  simFile = 0;
  eventTree=0;
  tracksTree=0;
  primaryInfoCA = 0;
  beamInfoCA = 0;
  theDataCA = 0;

  //null initialization to check that is null before their instantiation
  gasAnal = 0;
  silAnal = 0;
  silRingAnal = 0;
  sciAnal = 0;
  sciRingAnal = 0;
  plaAnal = 0;

  gasAnalIncludedFlag = 0;
  silAnalIncludedFlag = 0;
  silRingAnalIncludedFlag = 0;
  sciAnalIncludedFlag = 0;
  sciRingAnalIncludedFlag = 0;
  plaAnalIncludedFlag = 0;

  theData = new ActarSimData();
  pBeamInfo = new ActarSimBeamInfo();

  //TODO -> Implement SimpleHit versions in Sil and Sci
  //TODO -> containing only simulation data (no hardware info)

  OnceAWhileDoIt();
}

//////////////////////////////////////////////////////////////////
/// Constructor. Save and close Files.
ActarSimROOTAnalysis::~ActarSimROOTAnalysis() {
  delete analMessenger;

  simFile->Write();
  simFile->Close();

  if (gActarSimROOTAnalysis == this)
    gActarSimROOTAnalysis = (ActarSimROOTAnalysis *)0;

  delete theData;

  if (gSystem) gSystem->ProcessEvents();
}

//////////////////////////////////////////////////////////////////
/// Initialization of the detector analysis after the
/// class constructor, to allow the selection valid detectors
void ActarSimROOTAnalysis::InitAnalysisForExistingDetectors() {
  //TFile for storing the info
  if(!simFile){
    //simFile = new TFile("simFile.root","RECREATE");
    //simFile = new TFile("root_files/simFile.root","RECREATE");
    simFile = new TFile("root_files/sim_files/simFile.root","RECREATE");
    simFile->cd();

    eventTree = new TTree("The_ACTAR_Event_Tree","Event_Tree");
    tracksTree = new TTree("The_ACTAR_Tracks_Tree","Tracks_Tree");

    primaryInfoCA = new TClonesArray("ActarSimPrimaryInfo",2);
    eventTree->Branch("primaryInfo",&primaryInfoCA);
    beamInfoCA = new TClonesArray("ActarSimBeamInfo",1);
    eventTree->Branch("beamInfo",&beamInfoCA);
    theDataCA = new TClonesArray("ActarSimData",1);
    eventTree->Branch("theData",&theDataCA);
  }

  //Create the detector analysis only if the flag is set and is not already
  //set before (then, should be null as they are defined in the constructor)
  if(gasAnalIncludedFlag && !gasAnal)
    gasAnal = new ActarSimROOTAnalGas();

  if(silAnalIncludedFlag && !silAnal)
    silAnal = new ActarSimROOTAnalSil();

  if(silRingAnalIncludedFlag && !silRingAnal)
    silRingAnal = new ActarSimROOTAnalSilRing();

  if(sciAnalIncludedFlag && !sciAnal)
    sciAnal = new ActarSimROOTAnalSci();

  if(sciRingAnalIncludedFlag && !sciRingAnal)
    sciRingAnal = new ActarSimROOTAnalSciRing();

  if(plaAnalIncludedFlag && !plaAnal)
    plaAnal = new ActarSimROOTAnalPla();

  //OTHER DETECTORS ANALYSIS SHOULD BE INCLUDED HERE
  OnceAWhileDoIt();
}

//////////////////////////////////////////////////////////////////
/// Things to do while contructing...
void ActarSimROOTAnalysis::Construct(const G4VPhysicalVolume *theWorldVolume) {
  if (theWorldVolume) {;} /* keep the compiler "quiet" */
  if (gSystem) gSystem->ProcessEvents();

  OnceAWhileDoIt();
}

//////////////////////////////////////////////////////////////////
///  Actions to perform in the analysis during the particle construction
void ActarSimROOTAnalysis::ConstructParticle(){
  if (gSystem) gSystem->ProcessEvents();

  OnceAWhileDoIt();
}

//////////////////////////////////////////////////////////////////
/// Actions to perform in the analysis during the processes construction
void ActarSimROOTAnalysis::ConstructProcess(){
  if (gSystem) gSystem->ProcessEvents();

  OnceAWhileDoIt();
}

//////////////////////////////////////////////////////////////////
/// Actions to perform in the analysis during the cut setting
void ActarSimROOTAnalysis::SetCuts() {
  if (gSystem) gSystem->ProcessEvents();

  OnceAWhileDoIt();
}

//////////////////////////////////////////////////////////////////
/// Defining any beam related histogram or information in the output file
void ActarSimROOTAnalysis::GenerateBeam(const G4Event *anEvent){
  SetTheEventID(anEvent->GetEventID());

  //Clear the ClonesArray before filling it
  pBeamInfo->SetEventID(theEventID);
  pBeamInfo->SetRunID(theRunID);
  //There is only one clone of beamInfo..
  new((*beamInfoCA)[0])ActarSimBeamInfo(*pBeamInfo);
  //do not delete pBeamInfo, as the same pointer is used for all events
}

//////////////////////////////////////////////////////////////////
/// DEPRECATED!!!!! DO NOT USE
/// TODO Change from this to a GeneratePrimaries(anEvent, beamInfo).
/// DEPRECATED!!!!! DO NOT USE
void ActarSimROOTAnalysis::GeneratePrimaries(const G4Event *anEvent,
					     G4double Theta1,
					     G4double Theta2,
					     G4double Energy1,
					     G4double Energy2) {
  //DEPRECATED!!!!! DO NOT USE
  if (gSystem) gSystem->ProcessEvents();
  SetTheEventID(anEvent->GetEventID());

  //TODO->Remove this assymetry!!! There should be only one GeneratePrimaries
  //and all information should come in objects, nor arguments!!!!
  Double_t aTheta1 = (Theta1 / deg);   // in [deg]
  Double_t aTheta2 = (Theta2 / deg);   // in [deg]
  Double_t aEnergy1 = (Energy1 / MeV); // in [MeV]
  Double_t aEnergy2 = (Energy2 / MeV); // in [MeV]

  G4int nbOfPrimaryVertex = anEvent->GetNumberOfPrimaryVertex();
  G4int* nbOfPrimaries = new G4int[nbOfPrimaryVertex];
  G4int totalPrimaries = 0;

  for(G4int nbVertexes = 0 ; nbVertexes < nbOfPrimaryVertex ; nbVertexes++){
    nbOfPrimaries[nbVertexes] = anEvent->GetPrimaryVertex(nbVertexes)->GetNumberOfParticle();
    totalPrimaries += nbOfPrimaries[nbVertexes];
  }

  G4int countingPrimaries = 0;

  //Clear the ClonesArray before filling it
  primaryInfoCA->Clear();

  //Let us create and fill as many ActarSimPrimaryInfo as primaries in the event
  thePrimaryInfo = new ActarSimPrimaryInfo*[totalPrimaries];

  for(G4int i=0;i<nbOfPrimaryVertex;i++) {
    for(G4int j=0;j<nbOfPrimaries[i];j++) {
      thePrimaryInfo[countingPrimaries] =
	new ActarSimPrimaryInfo(anEvent->GetPrimaryVertex(i)->GetPrimary(j));
      thePrimaryInfo[countingPrimaries]->SetNbPrimariesInEvent(totalPrimaries);
      //do really need this datamember
      thePrimaryInfo[countingPrimaries]->SetRunID(GetTheRunID());
      thePrimaryInfo[countingPrimaries]->SetEventID(GetTheEventID());
      thePrimaryInfo[countingPrimaries]->SetVertexPosition(anEvent->GetPrimaryVertex(i)->GetX0() / mm,
							   anEvent->GetPrimaryVertex(i)->GetY0() / mm,
							   anEvent->GetPrimaryVertex(i)->GetZ0() / mm );
      //thePrimaryInfo[i]->print();
      countingPrimaries++;
    }
  }
  //G4cout<<"ActarSimROOTAnalysis----> GeneratePrimaries() "<<totalPrimaries<<G4endl;

  //at the end, fill the ClonesArray
  for (G4int i=0;i<totalPrimaries;i++)
    new((*primaryInfoCA)[i])ActarSimPrimaryInfo(*thePrimaryInfo[i]);

  //G4cout<<" in LOOP ActarSimROOTAnalysis----> GeneratePrimaries() "<<G4endl;

  for (G4int i=0;i<totalPrimaries;i++) delete thePrimaryInfo[i];
  delete thePrimaryInfo;

  delete nbOfPrimaries;

  if (hScatteredIonKinematic) hScatteredIonKinematic->Fill(aTheta1,aEnergy1);
  if (hRecoilIonKinematic) hRecoilIonKinematic->Fill(aTheta2,aEnergy2);

  if(gasAnal) gasAnal->GeneratePrimaries(anEvent);
  if(silAnal) silAnal->GeneratePrimaries(anEvent);
  if(silRingAnal) silRingAnal->GeneratePrimaries(anEvent);
  if(sciAnal) sciAnal->GeneratePrimaries(anEvent);
  if(sciRingAnal) sciRingAnal->GeneratePrimaries(anEvent);
  if(plaAnal) plaAnal->GeneratePrimaries(anEvent);

  OnceAWhileDoIt();
}

//////////////////////////////////////////////////////////////////
/// Actions to perform in the analysis during the cut setting
void ActarSimROOTAnalysis::GeneratePrimaries(const G4Event *anEvent, ActarSimBeamInfo *beamInfo) {

  if (gSystem) gSystem->ProcessEvents();
  SetTheEventID(anEvent->GetEventID());

  Double_t aTheta1 = beamInfo->GetThetaEntrance() / deg;   // in [deg]
  Double_t aTheta2 = beamInfo->GetThetaVertex() / deg;   // in [deg]
  Double_t aEnergy1 = beamInfo->GetEnergyEntrance() / MeV; // in [MeV]
  Double_t aEnergy2 = beamInfo->GetEnergyVertex() / MeV; // in [MeV]


  G4int nbOfPrimaryVertex = anEvent->GetNumberOfPrimaryVertex();
  G4int* nbOfPrimaries = new G4int[nbOfPrimaryVertex];
  G4int totalPrimaries = 0;

  for(G4int nbVertexes = 0 ; nbVertexes < nbOfPrimaryVertex ; nbVertexes++){
    nbOfPrimaries[nbVertexes] = anEvent->GetPrimaryVertex(nbVertexes)->GetNumberOfParticle();
    totalPrimaries += nbOfPrimaries[nbVertexes];
  }

  G4int countingPrimaries = 0;

  //Clear the ClonesArray before filling it
  primaryInfoCA->Clear();

  //Let us create and fill as many ActarSimPrimaryInfo as primaries in the event
  thePrimaryInfo = new ActarSimPrimaryInfo*[totalPrimaries];

  for(G4int i=0;i<nbOfPrimaryVertex;i++) {
    for(G4int j=0;j<nbOfPrimaries[i];j++) {
      thePrimaryInfo[countingPrimaries] =
	new ActarSimPrimaryInfo(anEvent->GetPrimaryVertex(i)->GetPrimary(j));
      thePrimaryInfo[countingPrimaries]->SetNbPrimariesInEvent(totalPrimaries);
      //do really need this datamember
      thePrimaryInfo[countingPrimaries]->SetRunID(GetTheRunID());
      thePrimaryInfo[countingPrimaries]->SetEventID(GetTheEventID());
      thePrimaryInfo[countingPrimaries]->SetVertexPosition(anEvent->GetPrimaryVertex(i)->GetX0() / mm,
							   anEvent->GetPrimaryVertex(i)->GetY0() / mm,
							   anEvent->GetPrimaryVertex(i)->GetZ0() / mm );
      //thePrimaryInfo[i]->print();
      countingPrimaries++;
    }
  }

  //at the end, fill the ClonesArray
  //G4cout<<"ActarSimROOTAnalysis----> GeneratePrimaries() "<<totalPrimaries<<G4endl;

  for (G4int i=0;i<totalPrimaries;i++){
    //G4cout<<"PrimaryInfoCA[i]"<<primaryInfoCA<<G4endl;
    new((*primaryInfoCA)[i])ActarSimPrimaryInfo(*thePrimaryInfo[i]);
    //G4cout<<" in LOOP ActarSimROOTAnalysis----> GeneratePrimaries() "<<i<<G4endl;
  }
  for (G4int i=0;i<totalPrimaries;i++) delete thePrimaryInfo[i];
  delete thePrimaryInfo;
  delete nbOfPrimaries;

  if (hScatteredIonKinematic) hScatteredIonKinematic->Fill(aTheta1,aEnergy1);
  if (hRecoilIonKinematic) hRecoilIonKinematic->Fill(aTheta2,aEnergy2);

  if(gasAnal) gasAnal->GeneratePrimaries(anEvent);
  if(silAnal) silAnal->GeneratePrimaries(anEvent);
  if(silRingAnal) silRingAnal->GeneratePrimaries(anEvent);
  if(sciAnal) sciAnal->GeneratePrimaries(anEvent);
  if(sciRingAnal) sciRingAnal->GeneratePrimaries(anEvent);
  if(plaAnal) plaAnal->GeneratePrimaries(anEvent);

  OnceAWhileDoIt();
}

//////////////////////////////////////////////////////////////////
/// Actions to perform in the analysis at the beginning of the run
void ActarSimROOTAnalysis::BeginOfRunAction(const G4Run *aRun) {
  if (gSystem) gSystem->ProcessEvents();

  //Storing the runID
  SetTheRunID(aRun->GetRunID());

  //going to the file!!!
  G4cout << "##################################################################" << G4endl
	 << "########  ActarSimROOTAnalysis::BeginOfRunAction()    ############" << G4endl;
  G4cout << "########  New Run With Number " << aRun->GetRunID() << " Detected!!  ######" << G4endl;
  G4cout << "####  A new directory will be opened in the output ROOT file  ####" << G4endl;
  G4cout << "################################################################## " << G4endl;

  //static Char_t newDirName[255];
  sprintf(newDirName,"%s%i","Histos",aRun->GetRunID());
  simFile->mkdir(newDirName,newDirName);
  simFile->cd(newDirName);

  if(storeHistogramsFlag=="on"){
    // Step Sum Length
    // histogram for the Cine Kinematic Results for the Scattered Ion
    //if(reactionFromCineFlag == "on")
    hScatteredIonKinematic =
      (TH2F *)gROOT->FindObject("hScatteredIonKinematic");
    if (hScatteredIonKinematic) hScatteredIonKinematic->Reset();
    else {
      hScatteredIonKinematic = new TH2F("hScatteredIonKinematic",
					"Cine Kinematic for the Scattered Ion",
					2, 0., 180, 2, 0., 100.);
      if (hScatteredIonKinematic) hScatteredIonKinematic->SetXTitle("Angle (deg)");
      if (hScatteredIonKinematic) hScatteredIonKinematic->SetYTitle("Energy (MeV)");
      if (hScatteredIonKinematic) hScatteredIonKinematic->SetMarkerColor(1);
      if (hScatteredIonKinematic) hScatteredIonKinematic->SetMarkerStyle(21);
      if (hScatteredIonKinematic) hScatteredIonKinematic->SetMarkerSize(0.8);
    }

    // histogram for the Cine Kinematic Results for the Recoil Ion
    hRecoilIonKinematic =
      (TH2F *)gROOT->FindObject("hRecoilIonKinematic");
    if (hRecoilIonKinematic) hRecoilIonKinematic->Reset();
    else {
      hRecoilIonKinematic = new TH2F("hRecoilIonKinematic",
				     "Cine Kinematic for the Recoil Ion",
				     2, 0., 10, 2, 0., 1000.);
      if (hRecoilIonKinematic) hRecoilIonKinematic->SetXTitle("Angle (deg)");
      if (hRecoilIonKinematic) hRecoilIonKinematic->SetYTitle("Energy (MeV)");
      if (hRecoilIonKinematic) hRecoilIonKinematic->SetMarkerColor(1);
      if (hRecoilIonKinematic) hRecoilIonKinematic->SetMarkerStyle(21);
      if (hRecoilIonKinematic) hRecoilIonKinematic->SetMarkerSize(0.8);
    }
  }

  if(storeHistogramsFlag=="on"){
    // Primary
    hPrimTheta  =  (TH1D *)gROOT->FindObject("hPrimTheta");
    if(hPrimTheta) hPrimTheta->Reset();
    else {
      hPrimTheta = new TH1D("hPrimTheta",
			    "Primary Theta angle",
			    1000, -0.01, 3.15); //
      hPrimTheta->SetXTitle("Theta [rad]");
    }

    // Primary
    hPrimPhi  =  (TH1D *)gROOT->FindObject("hPrimPhi");
    if(hPrimPhi) hPrimPhi->Reset();
    else {
      hPrimPhi = new TH1D("hPrimPhi",
			  "Primary Phi angle",
			  1000, -0.01, 6.29); //
      hPrimPhi->SetXTitle("Phi [rad]");
    }

    // Primary
    hPrimEnergy  =  (TH1D *)gROOT->FindObject("hPrimEnergy");
    if(hPrimEnergy) hPrimEnergy->Reset();
    else {
      hPrimEnergy = new TH1D("hPrimEnergy",
			     "Primary Energy (first particle)",
			     10000, -0.01, 301); //
      hPrimEnergy->SetXTitle("Energy [MeV]");
    }

    // Primary Energy vs Theta angle
    hPrimEnergyVsTheta =
      (TH2D *)gROOT->FindObject("hPrimEnergyVsTheta");
    if(hPrimEnergyVsTheta) hPrimEnergyVsTheta->Reset();
    else {
      hPrimEnergyVsTheta = new TH2D("hPrimEnergyVsTheta",
				    "Primary Energy vs Theta angle",
				    100, -0.01, 3.15, 100, 0, 301); //mover a 301
      hPrimEnergyVsTheta->SetYTitle("E [MeV]");
      hPrimEnergyVsTheta->SetXTitle("Theta [rad]");
    }
  }

  //calling the actions defined for each detector
  if(gasAnal) gasAnal->BeginOfRunAction(aRun);
  if(silAnal) silAnal->BeginOfRunAction(aRun);
  if(silRingAnal) silRingAnal->BeginOfRunAction(aRun);
  if(sciAnal) sciAnal->BeginOfRunAction(aRun);
  if(sciRingAnal) sciRingAnal->BeginOfRunAction(aRun);
  if(plaAnal) plaAnal->BeginOfRunAction(aRun);

  simFile->cd();

  OnceAWhileDoIt(true); // do it now
}

//////////////////////////////////////////////////////////////////
/// Actions to perform in the analysis at the end of the run
void ActarSimROOTAnalysis::EndOfRunAction(const G4Run *aRun) {
  if(gasAnal) gasAnal->EndOfRunAction(aRun);

  if (aRun) {;} /* keep the compiler "quiet" */
  if (gSystem) gSystem->ProcessEvents();

  G4cout << "##################################################################" << G4endl
	 << "########  ActarSimROOTAnalysis::EndOfRunAction()    ############" << G4endl;
  G4cout << "########  Run With Number " << aRun->GetRunID() << " finished!  ######" << G4endl;
  G4cout << "################################################################## " << G4endl;

  OnceAWhileDoIt(true); // do it now
}

//////////////////////////////////////////////////////////////////
/// Actions to perform in the analysis at the beginning of the event
void ActarSimROOTAnalysis::BeginOfEventAction(const G4Event *anEvent){
  SetTheEventID(anEvent->GetEventID());

  if (gSystem) gSystem->ProcessEvents();

  //calling the actions defined for each detector
  if(gasAnal) gasAnal->BeginOfEventAction(anEvent);
  if(silAnal) silAnal->BeginOfEventAction(anEvent);
  if(silRingAnal) silRingAnal->BeginOfEventAction(anEvent);
  if(sciAnal) sciAnal->BeginOfEventAction(anEvent);
  if(sciAnal) sciAnal->BeginOfEventAction(anEvent);
  if(plaAnal) plaAnal->BeginOfEventAction(anEvent);

  OnceAWhileDoIt();
}

//////////////////////////////////////////////////////////////////
/// Actions to perform in the analysis at the end of the event
void ActarSimROOTAnalysis::EndOfEventAction(const G4Event *anEvent) {
  if (gSystem) gSystem->ProcessEvents();

  G4PrimaryVertex* myPVertex1 = anEvent->GetPrimaryVertex(0);
  G4PrimaryVertex* myPVertex2 = 0;
  if(anEvent->GetNumberOfPrimaryVertex()>1)
    myPVertex2 = anEvent->GetPrimaryVertex(1);

  // G4cout << "The number of Primaries in the first vertex is "
  //     <<  myPVertex->GetNumberOfParticle() << G4endl;

  G4PrimaryParticle* myPrim1 = myPVertex1->GetPrimary();
  G4PrimaryParticle* myPrim2 = 0;
  if(myPVertex2) myPrim2 = myPVertex2->GetPrimary();
  G4ThreeVector momentumPrim1 = myPrim1->GetMomentum();
  G4ThreeVector momentumPrim2;
  if(myPrim2) momentumPrim2 = myPrim2->GetMomentum();
  G4double massPrim1 = myPrim1->GetMass();
  G4double massPrim2=0.0;
  if(myPrim2) massPrim2 = myPrim2->GetMass();

  //KRANE(A.5) T = E - mc2;
  G4double energyPrim1 = sqrt(momentumPrim1.mag2()+massPrim1*massPrim1) - massPrim1;
  G4double energyPrim2 = sqrt(momentumPrim2.mag2()+massPrim2*massPrim2) - massPrim2;


  if(storeHistogramsFlag=="on"){ // added flag dypang 080301
    //Primary histograms
    if (hPrimTheta)
      hPrimTheta->Fill(momentumPrim1.theta());
    if (hPrimPhi)
      hPrimPhi->Fill(momentumPrim1.phi());
    if (hPrimEnergy)
      hPrimEnergy->Fill(energyPrim1);
    if (hPrimEnergyVsTheta)
      hPrimEnergyVsTheta->Fill(momentumPrim1.theta(),energyPrim1);
  }

  if(storeEventsFlag=="on"){
    theData->SetThetaPrim1(momentumPrim1.theta());
    theData->SetThetaPrim2(momentumPrim2.theta());
    theData->SetPhiPrim1(momentumPrim1.phi());
    theData->SetPhiPrim2(momentumPrim2.phi());
    theData->SetEnergyPrim1(energyPrim1);
    theData->SetEnergyPrim2(energyPrim2);
  }

  //calling the actions defined for each detector
  //DPL jun2012 only silicon hits stored in ROOT file
  //G4int hitsCollectionID =G4SDManager::GetSDMpointer()->GetCollectionID("SilCollection");
  //G4HCofThisEvent* HCofEvent = anEvent->GetHCofThisEvent();
  //ActarSimSilGeantHitsCollection* hitsCollection =
  // (ActarSimSilGeantHitsCollection*) HCofEvent->GetHC(hitsCollectionID);
  //Number of ActarSimSilGeantHit (or steps) in the hitsCollection
  //G4int NbHits = hitsCollection->entries();
  //G4cout<<"Hits in the silicon "<< NbHits <<G4endl;
  //if(NbHits){
  //G4cout<<"ActarSimROOTAnalysis----> EndOfEventAction() "<<gasAnal<<G4endl;
  if(gasAnal) gasAnal->EndOfEventAction(anEvent);
  if(silAnal) silAnal->EndOfEventAction(anEvent);
  if(silRingAnal) silRingAnal->EndOfEventAction(anEvent);
  if(sciAnal) sciAnal->EndOfEventAction(anEvent);
  if(sciRingAnal) sciRingAnal->EndOfEventAction(anEvent);
  if(plaAnal) plaAnal->EndOfEventAction(anEvent);

  if(pBeamInfo->GetStatus()==0){  // pBeamInfo==0 at end of the event in the "fragments event" (pBeamInfo==2 in beam events )
    //There is only one clone of theData..
    new((*theDataCA)[0])ActarSimData(*theData);
    //do not delete theData, as the same pointer is used for all events
  }

  eventTree->Fill();
  primaryInfoCA->Clear(); //needed to avoid duplication of the CA contents in even/odd events
  beamInfoCA->Clear(); //needed to avoid duplication of the CA contents in even/odd events
  theDataCA->Clear(); //needed to avoid duplication of the CA contents in even/odd events
  //}
  OnceAWhileDoIt();

}

//////////////////////////////////////////////////////////////////
/// Actions to perform in the analysis when classifying new tracks
void ActarSimROOTAnalysis::ClassifyNewTrack(const G4Track *aTrack,
					    G4ClassificationOfNewTrack *classification_ptr) {
  if (aTrack){;} /* keep the compiler "quiet" */
  if (classification_ptr){;} /* keep the compiler "quiet" */
  // G4ClassificationOfNewTrack &classification = (*classification_ptr);

  if (gSystem) gSystem->ProcessEvents();

  OnceAWhileDoIt();
}

//////////////////////////////////////////////////////////////////
///  Actions to perform in the analysis before the user tracking
void ActarSimROOTAnalysis::PreUserTrackingAction(const G4Track *aTrack){

  if (aTrack){;} /* keep the compiler "quiet" */
  if (gSystem) gSystem->ProcessEvents();

  OnceAWhileDoIt();
}

//////////////////////////////////////////////////////////////////
///  Actions to perform in the analysis after the user tracking
void ActarSimROOTAnalysis::PostUserTrackingAction(const G4Track *aTrack,
						  G4TrackStatus *status_ptr) {
  // G4TrackStatus &status = (*status_ptr);
  if (aTrack){;} /* keep the compiler "quiet" */
  if (status_ptr){;} /* keep the compiler "quiet" */
  if (gSystem) gSystem->ProcessEvents();

  OnceAWhileDoIt();
}


//////////////////////////////////////////////////////////////////
///  Actions to perform in the analysis after each step
void ActarSimROOTAnalysis::UserSteppingAction(const G4Step *aStep){
  //calling the actions defined for each detector
  if(gasAnal) gasAnal->UserSteppingAction(aStep);
  if(silAnal) silAnal->UserSteppingAction(aStep);
  if(silRingAnal) silRingAnal->UserSteppingAction(aStep);
  if(sciAnal) sciAnal->UserSteppingAction(aStep);
  if(sciRingAnal) sciRingAnal->UserSteppingAction(aStep);
  if(plaAnal) plaAnal->UserSteppingAction(aStep);

  // Processing the beam, in case of beamInteractionFlag on
  // If a beam ion is being tracked with status 1 (ion beam being tracked) and if the present
  // step corresponds to the primary (the ion itself!), checks if the z position of vertex,
  // [calculated in ActarSimPrimaryGeneratorAction and stored in the ActarSimBeamInfo]
  // is reached. If so, store the ion position and use it for vertex generation and abort the event.

  if(beamInteractionFlag=="on" && pBeamInfo->GetStatus() == 1){
    G4double zVertex = pBeamInfo->GetZVertex();
    if(aStep->GetTrack()->GetParentID()==0){
      if(aStep->GetPreStepPoint()->GetPosition().z() < zVertex &&
	 aStep->GetPostStepPoint()->GetPosition().z() > zVertex){
        const G4int verboseLevel = G4RunManager::GetRunManager()->GetVerboseLevel();
        if(verboseLevel>0){
          G4cout << G4endl
		 << " *************************************************** " << G4endl
		 << " * ActarSimROOTAnalysis::UserSteppingAction() " << G4endl
		 << " * beamInteractionFlag=on, beam.Status=1, primary particle (ion beam) " << G4endl
		 << " * zVertex at " << aStep->GetPreStepPoint()->GetPosition().z() << G4endl
		 << " * aborting the present event and moving to "<< G4endl;
          G4cout << " *************************************************** "<< G4endl;
        }
	pBeamInfo->SetXVertex(aStep->GetPreStepPoint()->GetPosition().x()/mm);
	pBeamInfo->SetYVertex(aStep->GetPreStepPoint()->GetPosition().y()/mm);
	pBeamInfo->SetZVertex(aStep->GetPreStepPoint()->GetPosition().z()/mm);
	pBeamInfo->SetEnergyVertex(aStep->GetTrack()->GetKineticEnergy()/MeV);
	pBeamInfo->SetTimeVertex(aStep->GetTrack()->GetGlobalTime()/ns);

        // beam direction calculated by beam position at entrance and at vertex, needed for Euler transformation
        G4ThreeVector beamDirection(pBeamInfo->GetXVertex()-pBeamInfo->GetXEntrance(),
                                    pBeamInfo->GetYVertex()-pBeamInfo->GetYEntrance(),
                                    pBeamInfo->GetZVertex()-pBeamInfo->GetZEntrance());

        pBeamInfo->SetThetaVertex(beamDirection.theta());
        pBeamInfo->SetPhiVertex(beamDirection.phi());

        pBeamInfo->SetStatus(2);
        G4RunManager::GetRunManager()->AbortEvent();
      }
    }
  }
  OnceAWhileDoIt();
}

//////////////////////////////////////////////////////////////////
///  Recursive controller
void ActarSimROOTAnalysis::OnceAWhileDoIt(const G4bool DoItNow) {
  time_t Now = time(0); // get the current time (measured in seconds)
  if ( (!DoItNow) && (LastDoItTime > (Now - 10)) ) return; // every 10 seconds
  LastDoItTime = Now;

  if (gSystem) gSystem->ProcessEvents();
}

//////////////////////////////////////////////////////////////////
/// Setter of the minimum stride length in the gas
void ActarSimROOTAnalysis::SetMinStrideLength(Double_t value){
  if(gasAnal)
    gasAnal->SetMinStrideLength(value);
}
