/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez Pol
//*-- Date: 05/2005
//*-- Last Update: 07/01/15 by Hector Alvarez Pol
// --------------------------------------------------------------
// Description:
//   The ACTAR Silicon detectorpart of the ROOT Analysis
//
// --------------------------------------------------------------
// Comments:
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include "ActarSimROOTAnalSil.hh"

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

//ROOT INCLUDES
#include "TROOT.h"
#include "TApplication.h"
#include "TSystem.h"
#include "TH1.h"
#include "TH2.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"
#include "TClonesArray.h"

ActarSimROOTAnalSil::ActarSimROOTAnalSil() {
  //
  // Constructor
  //

  //The simulation file
  simFile = ((ActarSimROOTAnalysis*)gActarSimROOTAnalysis)->GetSimFile();
  simFile->cd();

  //The tree
  eventTree = ((ActarSimROOTAnalysis*)gActarSimROOTAnalysis)->GetEventTree();

  //The clones array of Sci Hits
  //TODO-> Repeat here the trick SimpleHit/HitSim/Hit that is existing in ActarSimAnalSil.cc
  silHitCA = new TClonesArray("ActarSimSilHit",2);

  silHitsBranch = eventTree->Branch("silHits",&silHitCA);
  silHitsBranch->SetAutoDelete(kTRUE);

}


ActarSimROOTAnalSil::~ActarSimROOTAnalSil() {
  //
  // Destructor
  //

}


void ActarSimROOTAnalSil::GeneratePrimaries(const G4Event *anEvent){
  //
  // Actions to perform in the silicon anal when generating the primaries
  //

  if(anEvent){;} /* keep the compiler "quiet" */
}


void ActarSimROOTAnalSil::BeginOfRunAction(const G4Run *aRun) {
  //
  // Actions to perform in the silicon anal at the begining of the run
  //
  if (aRun){;} /* keep the compiler "quiet" */

  //Storing the runID
  SetTheRunID(aRun->GetRunID());

  char newDirName[255];
  sprintf(newDirName,"%s%i","Histos",aRun->GetRunID());
  simFile->cd(newDirName);

  dirName = new char[255];

  sprintf(dirName,"%s","sil");
  gDirectory->mkdir(dirName,dirName);
  gDirectory->cd(dirName);

  simFile->cd();
}



void ActarSimROOTAnalSil::BeginOfEventAction(const G4Event *anEvent) {
  //
  // Actions to perform in the silicon anal at the begining of the event
  //

  SetTheEventID(anEvent->GetEventID());
}


void ActarSimROOTAnalSil::EndOfEventAction(const G4Event *anEvent) {
  //
  // Actions to perform in the silicon anal at the beginning of the run
  // Defining the ActarSimSilHit from the ActarSimSilGeantHits
  //

  FillingHits(anEvent);
}


void ActarSimROOTAnalSil::FillingHits(const G4Event *anEvent) {
  //
  // Defining the ActarSimSilHits from the ActarSimSilGeantHits
  //
  // A simple algorithm checking the number of primaries
  // reaching the Sil and calculating their mean parameters
  // taking into account their energy deposition on the silicons
  // NOTE that only primaries can produce Hits following this scheme
  //
  //Hit Container ID for ActarSimSilGeantHit
  G4int hitsCollectionID =
    G4SDManager::GetSDMpointer()->GetCollectionID("SilCollection");

  G4HCofThisEvent* HCofEvent = anEvent->GetHCofThisEvent();

  ActarSimSilGeantHitsCollection* hitsCollection =
    (ActarSimSilGeantHitsCollection*) HCofEvent->GetHC(hitsCollectionID);

  //Number of ActarSimSilGeantHit (or steps) in the hitsCollection
  G4int NbHits = hitsCollection->entries();

  G4int indepHits = 0; //number of Hits
  G4int* trackIDtable; //stores the trackID of primary particles for each (valid) GeantHit
  trackIDtable = new G4int[NbHits];
  G4int* detIDtable;   //stores the detIDs for each (valid) GeantHit
  detIDtable = new G4int[NbHits];
  G4int* IDtable;         //stores the order in previous array for each (valid) GeantHit
  IDtable = new G4int[NbHits];

  for (G4int i=0;i<NbHits;i++) {
    if((*hitsCollection)[i]->GetParentID()==0) { //step from primary
      if(indepHits==0) { //only for the first Hit
	trackIDtable[indepHits] = (*hitsCollection)[i]->GetTrackID();
	detIDtable[indepHits] = (*hitsCollection)[i]->GetDetID();
	IDtable[i] = indepHits;
	indepHits++;
      }
      else { // this part is never reached. Maybe because there is always only one indepHits that has parentID equals to 0.
	for(G4int j=0; j<indepHits;j++) {
	  if( (*hitsCollection)[i]->GetTrackID() == trackIDtable[j] &&
	      (*hitsCollection)[i]->GetDetID() == detIDtable[j]) { //checking trackID and detID
	    IDtable[i] = j;
	    break; //not a new Hit
	  }
	  if(j==indepHits-1){ //we got the last hit and there was no match!
	    trackIDtable[indepHits] = (*hitsCollection)[i]->GetTrackID();
	    detIDtable[indepHits] = (*hitsCollection)[i]->GetDetID();
	    IDtable[i] = indepHits;
	    indepHits++;
	  }
	}
      }
    }
  }

  //Let us create as many ActarSimSilHit as independent primary particles
  theSilHit = new ActarSimSilHit*[indepHits];
  for (G4int i=0;i<indepHits;i++)
    theSilHit[i] = new ActarSimSilHit();

  //Clear the ClonesArray before filling it
  silHitCA->Clear();

  //a variable to check if the Hit was already created
  G4int* existing;
  existing = new G4int[indepHits];
  for(G4int i=0;i<indepHits;i++) existing[i] = 0;

  for(G4int i=0;i<NbHits;i++) {
    if( (*hitsCollection)[i]->GetParentID()==0 ) { //step from primary
      //the IDtable[i] contains the order in the indepHits list
      if( existing[IDtable[i]]==0) { //if the indepHits does not exist
	AddSilHit(theSilHit[IDtable[i]],(*hitsCollection)[i],0);
	existing[IDtable[i]] = 1;
      }
      else
	AddSilHit(theSilHit[IDtable[i]],(*hitsCollection)[i],1);
    }
  }

  //at the end, fill the ClonesArray
  for (G4int i=0;i<indepHits;i++){
    //G4cout<<"ActarSimROOTAnalSil----------->FillingHits() "<<silHitCA<<G4endl;
    new((*silHitCA)[i])ActarSimSilHit(*theSilHit[i]);
    //G4cout<<"ActarSimROOTAnalSil----------->FillingHits() "<<silHitCA<<G4endl;
  }
  delete [] trackIDtable;
  delete [] IDtable;
  delete [] existing;
  delete [] detIDtable;
  for (G4int i=0;i<indepHits;i++) delete theSilHit[i];
  delete [] theSilHit;
}

void ActarSimROOTAnalSil::AddSilHit(ActarSimSilHit* cHit,
			       ActarSimSilGeantHit* gHit,
			       G4int mode) {
  //
  // Function to move the information from the ActarSimSilGeantHit (a step hit)
  // to ActarSimSilHit (a simple primary representation in the silicon)
  // Two modes are possible:
  // mode == 0 : creation; the ActarSimSilHit is void and is
  //             filled by the data from the ActarSimSilGeantHit
  // mode == 1 : addition; the ActarSimSilHit was already created
  //             by other ActarSimSilGeantHit and some data members are updated

  if(mode == 0) { //creation
    cHit->SetDetectorID(gHit->GetDetID());

    cHit->SetXPos(gHit->GetLocalPrePos().x()/mm);
    cHit->SetYPos(gHit->GetLocalPrePos().y()/mm);
    cHit->SetZPos(gHit->GetLocalPrePos().z()/mm);

    cHit->SetTime(gHit->GetToF()/ns);
    cHit->SetEnergy(gHit->GetEdep()/MeV);
    cHit->SetEBeforeSil(gHit->GetEBeforeSil()/MeV);
    cHit->SetEAfterSil(gHit->GetEAfterSil()/MeV);

    cHit->SetTrackID(gHit->GetTrackID());
    cHit->SetEventID(GetTheEventID());
    cHit->SetRunID(GetTheRunID());

    cHit->SetParticleID(gHit->GetParticleID());
    cHit->SetParticleCharge(gHit->GetParticleCharge());
    cHit->SetParticleMass(gHit->GetParticleMass());

    cHit->SetStepsContributing(1);

  }

  else if(mode==1){ //addition
    cHit->SetEnergy(cHit->GetEnergy() + gHit->GetEdep());
    //taking the smaller outgoing energy of the geantHits
    if(cHit->GetEAfterSil()>gHit->GetEAfterSil()) cHit->SetEAfterSil(gHit->GetEAfterSil()/MeV);
    //taking the larger incoming energy of the geantHits
    if(cHit->GetEBeforeSil()<gHit->GetEBeforeSil()) cHit->SetEBeforeSil(gHit->GetEBeforeSil()/MeV);

    cHit->SetStepsContributing(cHit->GetStepsContributing()+1);
    // The mean value of a distribution {x_i} can also be computed iteratively
    // if the values x_i are drawn one-by-one. After a new value x, the new mean is:
    // mean(t) = mean(t-1) + (1/t)(x-mean(t-1))
    cHit->SetXPos(cHit->GetXPos() +
		  (gHit->GetLocalPrePos().x()-cHit->GetXPos())/((G4double)cHit->GetStepsContributing()));
    cHit->SetYPos(cHit->GetYPos() +
		  (gHit->GetLocalPrePos().y()-cHit->GetYPos())/((G4double)cHit->GetStepsContributing()));
    cHit->SetZPos(cHit->GetZPos() +
		  (gHit->GetLocalPrePos().z()-cHit->GetZPos())/((G4double)cHit->GetStepsContributing()));

    //taking the shorter time of the geantHits
    if(cHit->GetTime()>gHit->GetToF()) cHit->SetTime(gHit->GetToF()/ns);
  }
}


void ActarSimROOTAnalSil::UserSteppingAction(const G4Step *aStep){
  //
  // Actions to perform in the ACTAR gas detector analysis after each step
  //

  if(aStep){;} /* keep the compiler "quiet" */

}
