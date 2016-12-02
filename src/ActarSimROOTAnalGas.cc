// - AUTHOR: Hector Alvarez-Pol 05/2005
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/
//////////////////////////////////////////////////////////////////
/// \class ActarSimROOTAnalGas
/// The gas detector part of the ROOT Analysis
/////////////////////////////////////////////////////////////////

#include "ActarSimROOTAnalGas.hh"

#include "G4ios.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4Event.hh"
#include "G4Run.hh"
#include "G4Track.hh"
#include "G4ClassificationOfNewTrack.hh"
#include "G4TrackStatus.hh"
#include "G4Step.hh"
#include "G4Types.hh"

//#include "G4PhysicalConstants.hh"
//#include "G4SystemOfUnits.hh"

#include "ActarSimTrack.hh"
#include "ActarSimSimpleTrack.hh"
#include "ActarSimData.hh"

//ROOT INCLUDES
#include "TROOT.h"
//#include "TApplication.h"
//#include "TSystem.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
//#include "TPad.h"
//#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"
#include "TClonesArray.h"
#include "TProfile.h"

#include "Randomize.hh"

//////////////////////////////////////////////////////////////////
/// Default constructor... Simply inits
ActarSimROOTAnalGas::ActarSimROOTAnalGas(){
  init();
}

//////////////////////////////////////////////////////////////////
/// Makes most of the work of a constructor...
void ActarSimROOTAnalGas::init(){
  G4cout << "##################################################################" << G4endl
	 << "###########    ActarSimROOTAnalGas::init()    ####################" << G4endl;

  //The simulation file
  simFile = ((ActarSimROOTAnalysis*) gActarSimROOTAnalysis)->GetSimFile();
  simFile->cd();

  //histograms of example
  hStepSumLengthOnGas1 = (TH1D *)0;
  hStepSumLengthOnGas2 = (TH1D *)0;

  htrackInPads = (TH2D *)0;
  htrack1InPads = (TH2D *)0;
  htrack2InPads = (TH2D *)0;
  htrackFromBeam = (TH2D *)0;
  htrack = (TH3D *)0;

  hEdepInGas = (TH1D *)0;

  // The accumulated energy loss and track length of each step
  hbeamEnergyAtRange=(TProfile *)0;

  //energy loss on the Gas
  hTotELossOnGas1 = (TH1D *)0;      //Energy Loss
  hTotELossOnGas2 = (TH1D *)0;      //Energy Loss

  //The tree
  eventTree =
    ((ActarSimROOTAnalysis*)gActarSimROOTAnalysis)->GetEventTree();
  tracksTree =
    ((ActarSimROOTAnalysis*)gActarSimROOTAnalysis)->GetTracksTree();

  //The clones array of Crystal Hits
  simpleTrackCA = new TClonesArray("ActarSimSimpleTrack",100);

  //theData should be initiated in ActarSimROOTAnalysis, as is going to be
  //used at several levels, while tracks are only relevant to gas...
  theData =
    ((ActarSimROOTAnalysis*)gActarSimROOTAnalysis)->GetTheData();

  theTracks = new ActarSimTrack();

  //Let us create 2 simpleTrack's for the primaries...
  simpleTrack = new ActarSimSimpleTrack*[2];
  for (G4int i=0;i<2;i++)
    simpleTrack[i] = new ActarSimSimpleTrack();

  //eventTree->Branch("theData","ActarSimData",&theData,128000,99);
  tracksTree->Branch("trackData","ActarSimTrack",&theTracks,128000,99);
  //Now, simple track as a TClonesArray
  eventTree->Branch("simpleTrackData",&simpleTrackCA);

  //minStrideLength = 0.1 * mm; //default value for the minimum stride length
  minStrideLength = 1.0 * CLHEP::mm; //default value for the minimum stride length
}

//////////////////////////////////////////////////////////////////
/// Destructor. Makes nothing
ActarSimROOTAnalGas::~ActarSimROOTAnalGas() {
}

//////////////////////////////////////////////////////////////////
/// Actions to perform in the analysis when generating the primaries
void ActarSimROOTAnalGas::GeneratePrimaries(const G4Event *anEvent){
  /// NOT VALID !!! ONLY GAMMAS ALLOWED, MODIFY FOR OTHER PARTICLES!!!
  //Filling the primary accessing on other functions
  //(in particular during UserSteppingAction()
  G4PrimaryVertex* myPVertex = anEvent->GetPrimaryVertex();
  //Modify this code in future for allowing several primary
  //particles and select on the gammas
  primNbOfParticles = myPVertex->GetNumberOfParticle();
  primary = myPVertex->GetPrimary();
  G4ThreeVector momentumPrim = primary->GetMomentum();
  primTheta = momentumPrim.theta();
  primPhi = momentumPrim.phi();
  // P^2 = E^2 - M^2 = (T + M)^2 - M^2
  // E = Ekin + M
  primEnergy = momentumPrim.mag(); //in case the mass is not zero, NOT VALID
}

//////////////////////////////////////////////////////////////////
/// Actions to perform in the analysis at the begining of the run
void ActarSimROOTAnalGas::BeginOfRunAction(const G4Run *aRun) {
  //Storing the runID
  SetTheRunID(aRun->GetRunID());

  char newDirName[255];
  sprintf(newDirName,"%s%i","Histos",aRun->GetRunID());
  simFile->cd(newDirName);

  dirName = new char[255];

  sprintf(dirName,"%s","gas");
  gDirectory->mkdir(dirName,dirName);
  gDirectory->cd(dirName);

  // Step Sum Length
  if(((ActarSimROOTAnalysis*) gActarSimROOTAnalysis)->GetStoreHistogramsFlag()== "on") {
    hStepSumLengthOnGas1 = (TH1D *)gROOT->FindObject("hStepSumLengthOnGas1");
    if (hStepSumLengthOnGas1) hStepSumLengthOnGas1->Reset();
    else {
      hStepSumLengthOnGas1 = new TH1D("hStepSumLengthOnGas1","Step Sum Length On Gas for first primary", 1000, 0.0, 1000.0);// in [cm]
      if (hStepSumLengthOnGas1) hStepSumLengthOnGas1->SetXTitle("[mm]");
    }
    // Step Sum Length
    hStepSumLengthOnGas2 = (TH1D *)gROOT->FindObject("hStepSumLengthOnGas2");
    if (hStepSumLengthOnGas2) hStepSumLengthOnGas2->Reset();
    else {
      hStepSumLengthOnGas2 = new TH1D("hStepSumLengthOnGas2","Step Sum Length On Gas for second primary", 1000, 0.0, 1000.0);// in [cm]
      if (hStepSumLengthOnGas2) hStepSumLengthOnGas2->SetXTitle("[mm]");
    }
  }
  if(((ActarSimROOTAnalysis*) gActarSimROOTAnalysis)->GetStoreTrackHistosFlag()== "on") {
    htrackInPads =
      (TH2D *)gROOT->FindObject("htrackInPads");
    if(htrackInPads) htrackInPads->Reset();
    else {
      htrackInPads = new TH2D("htrackInPads",
			      "All tracks in the XZ Pads Plane",
			      250, -500, 500, 250, -500, 500);
      htrackInPads->SetYTitle("Z [mm]");
      htrackInPads->SetXTitle("X [mm]");
    }

    //
    htrack1InPads =
      (TH2D *)gROOT->FindObject("htrack1InPads");
    if(htrack1InPads) htrack1InPads->Reset();
    else {
      htrack1InPads = new TH2D("htrack1InPads",
			       "track 1 In the XZ Pads Plane",
			       250, -500, 500, 250, -500, 500);
      htrack1InPads->SetYTitle("Z [mm]");
      htrack1InPads->SetXTitle("X [mm]");
    }

    // The accumulated energy loss and track length of each step
    hbeamEnergyAtRange =
      (TProfile *)gROOT->FindObject("hbeamEnergyAtRange");
    if(hbeamEnergyAtRange) hbeamEnergyAtRange->Reset();
    else {
      hbeamEnergyAtRange = new TProfile("hbeamEnergyAtRange",
					"Accumulated beam energy loss and trajectory length",
					500, 0, 500);
      hbeamEnergyAtRange->SetYTitle("Accumulated energy loss [MeV]");
      hbeamEnergyAtRange->SetXTitle("trajectory length [mm]");
    }

    //
    htrack2InPads =
      (TH2D *)gROOT->FindObject("htrack2InPads");
    if(htrack2InPads) htrack2InPads->Reset();
    else {
      htrack2InPads = new TH2D("htrack2InPads",
			       "track 2 In the XZ Pads Plane",
			       250, -500, 500, 250, -500, 500);
      htrack2InPads->SetYTitle("Z [mm]");
      htrack2InPads->SetXTitle("X [mm]");
    }
    //
    htrackFromBeam =
      (TH2D *)gROOT->FindObject("htrackFromBeam");
    if(htrackFromBeam) htrackFromBeam->Reset();
    else {
      htrackFromBeam = new TH2D("htrackFromBeam",
				"All tracks from a beam view ",
				250, -500, 500, 250, -500, 500);
      htrackInPads->SetYTitle("Y [mm]");
      htrackInPads->SetXTitle("X [mm]");
    }

    //
    htrack =
      (TH3D *)gROOT->FindObject("htrack");
    if(htrack) htrack->Reset();
    else {
      htrack = new TH3D("htrack",
			"All tracks from a beam view ",
			100, -500, 500, 100, -500, 500, 100, -500, 500);
      htrack->SetZTitle("Z [mm]");
      htrack->SetYTitle("Y [mm]");
      htrack->SetXTitle("X [mm]");
    }

    hEdepInGas =
      (TH1D *)gROOT->FindObject("hEdepInGas");
    if(hEdepInGas) hEdepInGas->Reset();
    else {
      hEdepInGas = new TH1D("hEdepInGas",
			    "Edep along Gas chamber (normalized!)",
			    300, 0, 300);
      hEdepInGas->SetXTitle("Z [mm]");
    }

    //Now, I will introduce the information of interest!
    // Total Energy Loss on Gas1
    hTotELossOnGas1  = (TH1D *)gROOT->FindObject("hTotELossOnGas1");
    if(hTotELossOnGas1) hTotELossOnGas1->Reset();
    else {
      hTotELossOnGas1 = new TH1D("hTotELossOnGas1",
                                 "Total Energy Loss inside the Gas for primary 1",
                                 1000, 0.0, 301.0); // in [MeV]
      hTotELossOnGas1->SetXTitle("[MeV]");
    }

    // Total Energy Loss on the gas
    hTotELossOnGas2  =
      (TH1D *)gROOT->FindObject("hTotELossOnGas2");
    if(hTotELossOnGas2) hTotELossOnGas2->Reset();
    else {
      hTotELossOnGas2 = new TH1D("hTotELossOnGas2",
				 "Total Energy Loss inside the Gas for primary 2",
				 1000, 0.0, 301.0); // in [MeV]
      hTotELossOnGas2->SetXTitle("[MeV]");
    }
  }

  simFile->cd();
}

//////////////////////////////////////////////////////////////////
/// Actions to perform in the analysis at the end of the run
void ActarSimROOTAnalGas::EndOfRunAction(const G4Run *aRun) {
  G4int nbofEvents = aRun->GetNumberOfEvent();

  if(((ActarSimROOTAnalysis*) gActarSimROOTAnalysis)->GetStoreTrackHistosFlag() == "on") {
    //G4double binWidth = hEdepInGas->GetBinWidth();
    hEdepInGas->Scale(1./nbofEvents);
    //G4cout << "Number of events: "<< nbofEvents << G4endl;
  }
}

//////////////////////////////////////////////////////////////////
/// Actions to perform in the analysis at the begining of the event
void ActarSimROOTAnalGas::BeginOfEventAction(const G4Event *anEvent) {
  SetTheEventID(anEvent->GetEventID());
}

//////////////////////////////////////////////////////////////////
/// Actions to perform in the analysis at the end of the event
void ActarSimROOTAnalGas::EndOfEventAction(const G4Event *anEvent) {
  Double_t aEnergyInGas1 =0;// (EnerGas1 / MeV); // in [MeV]
  Double_t aEnergyInGas2 =0;// (EnerGas2 / MeV); // in [MeV]
  Double_t aTLInGas1 =0;// (TLGas1 / mm); // in [mm]
  Double_t aTLInGas2 =0;// (TLGas2 / mm); // in [mm]

  if(((ActarSimROOTAnalysis*) gActarSimROOTAnalysis)->GetStoreSimpleTracksFlag()=="on"){
    //moving here the recollection of the steps info into strides
    //which was previously made in the UserSteppingAction() function
    //Now we will use the GeantHits to recover the steps from the gas

    //Hit Container ID for ActarSimGasGeantHit
    G4int hitsCollectionID =
      G4SDManager::GetSDMpointer()->GetCollectionID("gasCollection");

    G4HCofThisEvent* HCofEvent = anEvent->GetHCofThisEvent();

    ActarSimGasGeantHitsCollection* hitsCollection =
      (ActarSimGasGeantHitsCollection*) HCofEvent->GetHC(hitsCollectionID);

    //Number of ActarSimGasGeantHit (or steps) in the hitsCollection
    G4int NbHits = hitsCollection->entries();
    G4int NbStrides = 0;
    G4int strideOrdinal[2];
    strideOrdinal[0]=0;
    strideOrdinal[1]=0;

    simpleTrackCA->Clear();

    //  G4cout << "Information on the collection..." << G4endl
    // << "Number of GasGeantHits in the collection: " <<  NbHits
    // << G4endl;

    //  for (G4int i=0;i<NbHits;i++) {
    //if((*hitsCollection)[i]->GetStepLength()>1)
    //  G4cout << (*hitsCollection)[i]->GetPrePos().x() <<", "
    //     << (*hitsCollection)[i]->GetPrePos().y() <<", "
    //     << (*hitsCollection)[i]->GetPrePos().z() <<"      "
    //     << (*hitsCollection)[i]->GetPostPos().x() <<", "
    //     << (*hitsCollection)[i]->GetPostPos().y() <<", "
    //     << (*hitsCollection)[i]->GetPostPos().z() <<"      "
    //     << (*hitsCollection)[i]->GetEdep() <<"      "
    //     << (*hitsCollection)[i]->GetStepLength() <<"      " << G4endl;
    //}

    //init values
    for (G4int i=0;i<NbHits;i++) {
      //G4cout << (*hitsCollection)[i]->GetDetName() << G4endl;
      for(G4int j=0;j<2;j++) { //that is, for 2 primaries
        if((*hitsCollection)[i]->GetTrackID()==(j+1) &&
	   (*hitsCollection)[i]->GetParentID()==0){ //this step comes from a primary
	  //New algorithm to reduce the number of GEANT4 steps to a few strides...
          if(simpleTrack[j]->GetNumberSteps() == 0) {
            //the first step in the stride!
            simpleTrack[j]->SetXPre((*hitsCollection)[i]->GetPrePos().x());
            simpleTrack[j]->SetYPre((*hitsCollection)[i]->GetPrePos().y());
	    simpleTrack[j]->SetZPre((*hitsCollection)[i]->GetPrePos().z());
            simpleTrack[j]->SetXPost((*hitsCollection)[i]->GetPostPos().x());
            simpleTrack[j]->SetYPost((*hitsCollection)[i]->GetPostPos().y());
            simpleTrack[j]->SetZPost((*hitsCollection)[i]->GetPostPos().z());
            simpleTrack[j]->SetEnergyStride  ((*hitsCollection)[i]->GetEdep());
            simpleTrack[j]->SetParticleCharge((*hitsCollection)[i]->GetParticleCharge());
            simpleTrack[j]->SetParticleMass((*hitsCollection)[i]->GetParticleMass());
            simpleTrack[j]->SetParticleID((*hitsCollection)[i]->GetParticleID());
            simpleTrack[j]->SetStrideLength((*hitsCollection)[i]->GetStepLength());
	    //G4cout << "Step lenth: " << (*hitsCollection)[i]->GetStepLength() << G4endl;
	    simpleTrack[j]->SetParticleEnergy((*hitsCollection)[i]->GetStepEnergy());
            simpleTrack[j]->SetTimePre((*hitsCollection)[i]->GetPreToF());
            simpleTrack[j]->SetTimePost((*hitsCollection)[i]->GetPostToF());
            simpleTrack[j]->SetNumberSteps(1);
            simpleTrack[j]->SetTrackID((*hitsCollection)[i]->GetTrackID());
            simpleTrack[j]->SetParentTrackID((*hitsCollection)[i]->GetParentID());
            simpleTrack[j]->SetEventID(GetTheEventID());
            simpleTrack[j]->SetRunID(GetTheRunID());
            simpleTrack[j]->SetStrideOrdinal(strideOrdinal[j]);

	    //G4cout << "First step in stride (type" << j << ")" << G4endl
	    //<< "preTime " << (*hitsCollection)[i]->GetPreToF()
	    //<< " postTime" << (*hitsCollection)[i]->GetPostToF()
	    //<< "initPos: " <<  (*hitsCollection)[i]->GetPrePos()
	    //<< "  finalPos: " <<  (*hitsCollection)[i]->GetPostPos()<< G4endl;
          }
          else {
            simpleTrack[j]->SetXPost((*hitsCollection)[i]->GetPostPos().x());
            simpleTrack[j]->SetYPost((*hitsCollection)[i]->GetPostPos().y());
            simpleTrack[j]->SetZPost((*hitsCollection)[i]->GetPostPos().z());
            simpleTrack[j]->SetTimePost((*hitsCollection)[i]->GetPostToF());
            simpleTrack[j]->SetEnergyStride(simpleTrack[j]->GetEnergyStride() +
					    (*hitsCollection)[i]->GetEdep());
	    simpleTrack[j]->SetStrideLength(simpleTrack[j]->GetStrideLength() +
					    (*hitsCollection)[i]->GetStepLength());
	    simpleTrack[j]->SetNumberSteps(simpleTrack[j]->GetNumberSteps()+1);

	    //G4cout << "next step of type "  << j << G4endl
            // << " postTime" << (*hitsCollection)[i]->GetPostToF()
            // << "length up to now: " << simpleTrack[j]->GetStrideLength()<< G4endl;
          }
          if(simpleTrack[j]->GetStrideLength() > minStrideLength || (*hitsCollection)[i]->GetParticleCharge()<=2 ){
            //the sum of steps is larger that the given parameter... the stride goes to the Tree
            //G4cout << "...larger than minStrideLength and stored (type "
            // << j << ")  "  <<  NbStrides << "  "<<  strideOrdinal[j] << "  "
            //<< simpleTrack[j]->GetTimePre()<< "  "<<simpleTrack[j]->GetTimePost()<< G4endl;
            //G4cout<<"ActarSimROOTAnalGas------->EndOfEventAction() "<<simpleTrack[j]<<G4endl;
            new((*simpleTrackCA)[NbStrides])ActarSimSimpleTrack(*simpleTrack[j]);
            //G4cout<<"ActarSimROOTAnalGas------->EndOfEventAction()"<<simpleTrackCA<<G4endl;
            NbStrides++;
            strideOrdinal[j]++;
            simpleTrack[j]->Reset();
          }
          //David Perez Loureiro 28-10-2011-----------------------------------------//
          if(j==0){
            aEnergyInGas1 += (*hitsCollection)[i]->GetEdep();
            aTLInGas1 +=(*hitsCollection)[i]->GetStepLength();
          }
          else {
            aEnergyInGas2 += (*hitsCollection)[i]->GetEdep();
            aTLInGas2 +=(*hitsCollection)[i]->GetStepLength();
          }
        }
      }//end of loop in primaries
    }//end of loop on hits

    for(G4int j=0;j<2;j++) {
      if(simpleTrack[j]->GetNumberSteps() > 0){
      	if(simpleTrack[j]->GetStrideLength() > minStrideLength)
          G4cout << "ERROR in ActarSimRootAnalysis::EndOfEventAction: "
		 << "Something does not match !? Consult an expert :-)" << G4endl;
	//even if the sum of steps is not larger that the given parameter...
	//the stride goes to the Tree (last steps of the track...)
	//G4cout<<"End of loop  (type "<< j  << ")  "   <<  NbStrides << "  "
	//<<  strideOrdinal[j] << "  "<< simpleTrack[j]->GetTimePre()
	//<< "  "<<simpleTrack[j]->GetTimePost()<< G4endl;
	//<<" with length " << simpleTrack[j]->GetStrideLength() << G4endl;
	new((*simpleTrackCA)[NbStrides])ActarSimSimpleTrack(*simpleTrack[j]);
        NbStrides++;
        strideOrdinal[j]++;
        simpleTrack[j]->Reset();
      }
    }
  }
  if(((ActarSimROOTAnalysis*) gActarSimROOTAnalysis)->GetStoreEventsFlag()=="on"){
    theData->SetEnergyOnGasPrim1(aEnergyInGas1);
    theData->SetEnergyOnGasPrim2(aEnergyInGas2);
    theData->SetStepSumLengthOnGasPrim1(aTLInGas1);
    theData->SetStepSumLengthOnGasPrim2(aTLInGas2);
    theData->SetEventID(anEvent->GetEventID());
    theData->SetRunID(GetTheRunID());
  }

  if(((ActarSimROOTAnalysis*) gActarSimROOTAnalysis)->GetStoreTrackHistosFlag()=="on"){
    if (hStepSumLengthOnGas1) hStepSumLengthOnGas1->Fill(aTLInGas1);
    if (hStepSumLengthOnGas2) hStepSumLengthOnGas2->Fill(aTLInGas2);
    if (hTotELossOnGas1) hTotELossOnGas1->Fill(aEnergyInGas1);
    if (hTotELossOnGas2) hTotELossOnGas2->Fill(aEnergyInGas2);
  }
  /*
  //checking the number of electrons created on the ion path...
  G4TrajectoryContainer* myTraCon = anEvent->GetTrajectoryContainer();
  G4int numTracks = 0;
  if(myTraCon) numTracks = myTraCon->entries();//entries of the container
  G4cout << " INFO  numTracks=" << numTracks << G4endl;
  for(G4int i=0;i<numTracks;i++) {
    G4Trajectory* traje =(G4Trajectory*)((*(anEvent->GetTrajectoryContainer()))[i]);
    //if(traje->GetParticleName() == "e-")
    G4cout << " INFO --  TrackID: " << traje->GetTrackID()
	   << "  ParentID: " <<  traje->GetParentID()
	   << "  ParticleName: " <<  traje->GetParticleName() << G4endl
	   << "  Charge: " <<  traje->GetCharge()
	   << "  PDGEncoding: " <<  traje->GetPDGEncoding() << G4endl
	   << "  InitialMomentum: " <<  traje->GetInitialMomentum().x()
	     << " , " <<  traje->GetInitialMomentum().y()
	   << " , " <<  traje->GetInitialMomentum().z() << G4endl;
    // << "  Energy: " << traje->GetInitialMomentum().x()
    //	 << "  : " <<  traje->Get()
    //	 << "  : " <<  traje->Get()
  }
  */
}

//////////////////////////////////////////////////////////////////
/// Actions to perform in the ACTAR gas detector analysis after each step
void ActarSimROOTAnalGas::UserSteppingAction(const G4Step *aStep){
  G4Track* myTrack = aStep->GetTrack();
  G4ThreeVector prePoint = aStep->GetPreStepPoint()->GetPosition();
  G4ThreeVector postPoint = aStep->GetPostStepPoint()->GetPosition();

  G4double z1 = prePoint.z();
  G4double z2 = postPoint.z();
  G4double z  = z1 + G4UniformRand()*(z2-z1);// + 0.5*(fDetector->GetAbsorSizeX());

  G4double edep = aStep->GetTotalEnergyDeposit();
  if (edep <= 0.) return;

  if(((ActarSimROOTAnalysis*) gActarSimROOTAnalysis)->GetStoreTrackHistosFlag() == "on") {
    if(hEdepInGas && z2<300) hEdepInGas->Fill(z,edep);
    if(htrack) htrack->Fill(postPoint.x(),
			    postPoint.y(),
			    postPoint.z());
    if(htrackFromBeam) htrackFromBeam->Fill(postPoint.x(),
					    postPoint.y(),
					    aStep->GetTotalEnergyDeposit());
    if(htrackInPads) htrackInPads->Fill(postPoint.x(),
					postPoint.z(),
					aStep->GetTotalEnergyDeposit());
    if(myTrack->GetTrackID()==1 && myTrack->GetParentID()==0)
      if(htrack1InPads) htrack1InPads->Fill(postPoint.x(),
					    postPoint.z(),
					    aStep->GetTotalEnergyDeposit());
    if(myTrack->GetTrackID()==2 && myTrack->GetParentID()==0)
      if(htrack2InPads) htrack2InPads->Fill(postPoint.x(),
					    postPoint.z(),
					    aStep->GetTotalEnergyDeposit());
  }

  if(((ActarSimROOTAnalysis*) gActarSimROOTAnalysis)->GetStoreTracksFlag() == "on") {
    theTracks->SetXCoord(postPoint.x());
    theTracks->SetYCoord(postPoint.y());
    theTracks->SetZCoord(postPoint.z());
    theTracks->SetXPreCoord(prePoint.x());
    theTracks->SetYPreCoord(prePoint.y());
    theTracks->SetZPreCoord(prePoint.z());
    theTracks->SetEnergyStep(aStep->GetTotalEnergyDeposit());
    theTracks->SetTrackID(myTrack->GetTrackID());
    theTracks->SetParentTrackID(myTrack->GetParentID());
    theTracks->SetEventID(GetTheEventID());
    theTracks->SetRunID(GetTheRunID());
    tracksTree->Fill();
  }
}
