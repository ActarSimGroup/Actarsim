/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez Pol
//*-- Date: 05/2008
//*-- Last Update: 23/12/14 by Hector Alvarez Pol
// --------------------------------------------------------------
// Description:
//   The Plastic Scintillator detector part of the ROOT Analysis
//
// --------------------------------------------------------------
// Comments:
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include "ActarSimROOTAnalPla.hh"
#include "ActarSimPlaHit.hh"
#include "ActarSimPlaGeantHit.hh"

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

//for calculating the optical photon wavelenght for a given enegy
static const G4double LambdaE = twopi * 1.973269602e-16 * m * GeV;

ActarSimROOTAnalPla::ActarSimROOTAnalPla() {
  //
  // Constructor
  //

  //The simulation file
  simFile = ((ActarSimROOTAnalysis*)gActarSimROOTAnalysis)->GetSimFile();
  simFile->cd();

  //The tree
  eventTree = ((ActarSimROOTAnalysis*)gActarSimROOTAnalysis)->GetEventTree();

  //TODO->Implement SimHits/Hits duality as R3B...

  //The clones array of Sci Hits
  plaHitCA = new TClonesArray("ActarSimPlaHit",2);

  plaHitsBranch = eventTree->Branch("plaHits",&plaHitCA);

}


ActarSimROOTAnalPla::~ActarSimROOTAnalPla() {
  //
  // Destructor
  //

}


void ActarSimROOTAnalPla::GeneratePrimaries(const G4Event *anEvent){
  //
  // Actions to perform in the scintillator anal when generating the primaries
  //
  if(anEvent){;} // to quiet the compiler

}


void ActarSimROOTAnalPla::BeginOfRunAction(const G4Run *aRun) {
  //
  // Actions to perform in the scintillator anal at the begining of the run
  //
  if (aRun){;} /* keep the compiler "quiet" */

  //Storing the runID
  SetTheRunID(aRun->GetRunID());

  char newDirName[255];
  sprintf(newDirName,"%s%i","Histos",aRun->GetRunID());
  simFile->cd(newDirName);

  dirName = new char[255];

  sprintf(dirName,"%s","pla");
  gDirectory->mkdir(dirName,dirName);
  gDirectory->cd(dirName);

  simFile->cd();
}



void ActarSimROOTAnalPla::BeginOfEventAction(const G4Event *anEvent) {
  //
  // Actions to perform in the scintillator anal at the begining of the event
  //

  SetTheEventID(anEvent->GetEventID());
}


void ActarSimROOTAnalPla::EndOfEventAction(const G4Event *anEvent) {
  //
  // Actions to perform in the scintillator anal at the beginning of the run
  //

  FillingHits(anEvent);
}


void ActarSimROOTAnalPla::UserSteppingAction(const G4Step *aStep){
  //
  // Actions to perform in the scintillator detector analysis after each step
  //
  if(aStep){;} // to quiet the compiler

}


void ActarSimROOTAnalPla::FillingHits(const G4Event *anEvent) {
  //
  // Defining the ActarSimSciHits from the ActarSimSciGeantHits
  //

  //Hit Container ID for ActarSimSciGeantHit
  G4int hitsCollectionID =
    G4SDManager::GetSDMpointer()->GetCollectionID("PlaCollection");

  G4HCofThisEvent* HCofEvent = anEvent->GetHCofThisEvent();

  ActarSimPlaGeantHitsCollection* hitsCollection =
    (ActarSimPlaGeantHitsCollection*) HCofEvent->GetHC(hitsCollectionID);

  //Number of R3BCalGeantHit (or steps) in the hitsCollection
  G4int NbHits = hitsCollection->entries();
  //G4int NbHitsWithSomeEnergy = NbHits;
  //G4cout << " NbHits: " << NbHits << G4endl;
	
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
	thePlaHit = new ActarSimPlaHit*[indepHits];
	for (G4int i=0;i<indepHits;i++)
		thePlaHit[i] = new ActarSimPlaHit();
	
	//Clear the ClonesArray before filling it
	plaHitCA->Clear();
	
	//a variable to check if the Hit was already created
	G4int* existing;
	existing = new G4int[indepHits];
	for(G4int i=0;i<indepHits;i++) existing[i] = 0;
	
	for(G4int i=0;i<NbHits;i++) {
		if( (*hitsCollection)[i]->GetParentID()==0 ) { //step from primary
			//the IDtable[i] contains the order in the indepHits list
			if( existing[IDtable[i]]==0) { //if the indepHits does not exist
				AddCalPlaHit(thePlaHit[IDtable[i]],(*hitsCollection)[i],0);
				existing[IDtable[i]] = 1;
			}
			else
				AddCalPlaHit(thePlaHit[IDtable[i]],(*hitsCollection)[i],1);
		}
	}
	
	//at the end, fill the ClonesArray
	for (G4int i=0;i<indepHits;i++)
		new((*plaHitCA)[i])ActarSimPlaHit(*thePlaHit[i]);
	
	delete [] trackIDtable;
	delete [] IDtable;
	delete [] existing;
	delete [] detIDtable;
	for (G4int i=0;i<indepHits;i++) delete thePlaHit[i];
	delete [] thePlaHit;
}


	
	//CsI-like hit pattern
/*
	//We accept edep=0 GeantHits, we have to remove them
  //from the total number of GeantHits accepted for creating a real CrystalHit
  for (G4int i=0;i<NbHits;i++)
    if((*hitsCollection)[i]->GetEdep()==0.)
      NbHitsWithSomeEnergy--;
  //G4cout << " NbHitsWithSomeEnergy: " << NbHitsWithSomeEnergy << G4endl;

  G4int NbCrystalsWithHit=NbHitsWithSomeEnergy;
  G4String* nameWithSomeEnergy;
  G4int* detIDWithSomeEnergy;
  detIDWithSomeEnergy = new G4int[NbHitsWithSomeEnergy];
  nameWithSomeEnergy = new G4String[NbHitsWithSomeEnergy];

  //keep the crystal identifiers of the GeantHits with some energy
  G4int hitsWithEnergyCounter=0;
  for (G4int i=0;i<NbHits;i++) {
    if((*hitsCollection)[i]->GetEdep()>0.){
      nameWithSomeEnergy[hitsWithEnergyCounter] = (*hitsCollection)[i]->GetDetName();
      detIDWithSomeEnergy[hitsWithEnergyCounter] = (*hitsCollection)[i]->GetDetID();
      //G4cout << "with some energy:   name:" << nameWithSomeEnergy[hitsWithEnergyCounter]
      //     << " detID:"<<detIDWithSomeEnergy[hitsWithEnergyCounter] << G4endl;
      hitsWithEnergyCounter++;
    }
  }

  //if(hitsWithEnergyCounter != NbHitsWithSomeEnergy) {
  //  G4cout << "ERROR in ActarSimROOTAnalSci::FillingHits(): hitsWithEnergyCounter!=NbHitsWithSomeEnergy" << G4endl;
  //  G4cout << " hitsWithEnergyCounter = " << hitsWithEnergyCounter << " while NbHitsWithSomeEnergy = " << NbHitsWithSomeEnergy << G4endl;
  //}

  //We ask for the number of crystals with signal and
  //this loop calculates them from the number of ActarSimSciGeantHits with energy
  for (G4int i=0;i<NbHitsWithSomeEnergy;i++) {
    for(G4int j=0;j<i;j++){
      if(nameWithSomeEnergy[i] == nameWithSomeEnergy[j] && detIDWithSomeEnergy[i] == detIDWithSomeEnergy[j]){
	NbCrystalsWithHit--;
	break;  //break stops the second for() sentence as soon as one repetition is found
      }
    }
  }
  //G4cout << " NbCrystalsWithHit: " << NbCrystalsWithHit << G4endl;

  G4String* nameWithHit;
  G4int* detIDWithHit;
  detIDWithHit = new G4int[NbCrystalsWithHit];
  nameWithHit = new G4String[NbCrystalsWithHit];
  hitsWithEnergyCounter=0;

  //keep the crystal identifiers of the final crystalHits
  G4int hitsCounter=0;
  for (G4int i=0;i<NbHits;i++) {
    if ((*hitsCollection)[i]->GetEdep()>0.) {
      if(hitsCounter==0) { //the first geantHit with energy always creates a newHit
	nameWithHit[hitsCounter] = (*hitsCollection)[i]->GetDetName();
	detIDWithHit[hitsCounter] = (*hitsCollection)[i]->GetDetID();
	//G4cout << "with hit:   name:" << nameWithHit[hitsCounter]
	//       << " detID:"<<detIDWithHit[hitsCounter] << G4endl;
	hitsCounter++;
      }
      else {
	for (G4int j=0;j<hitsCounter;j++) {
	  if ( (*hitsCollection)[i]->GetDetName() == nameWithHit[j] &&
	       (*hitsCollection)[i]->GetDetID() == detIDWithHit[j] ) {
	    break;  //break stops the second for() sentence as soon as one repetition is found
	  }
	  if(j==hitsCounter-1){ //when the previous if was never true, there is no repetition
	    nameWithHit[hitsCounter] = (*hitsCollection)[i]->GetDetName();
	    detIDWithHit[hitsCounter] = (*hitsCollection)[i]->GetDetID();
	    //G4cout << "with hit:   name:" << nameWithHit[hitsCounter]
	    //   << " detID:"<<detIDWithHit[hitsCounter] << G4endl;
	    hitsCounter++;
	  }
	}
      }
    }
  }
  //DELETE ME AFTER THE PRELIMINARY TESTS!
  //if(hitsCounter != NbCrystalsWithHit) {
  //  G4cout << "ERROR in R3BROOTAnalCal::FillingHits(): hitsCounter!=NbCrystalsWithHit" << G4endl;
  //  G4cout << " hitsCounter = " << hitsCounter << " while NbCrystalsWithHit = " << NbCrystalsWithHit << G4endl;
  //}

  //Let us create as many R3BCalCrystalHit as NbCrystalsWithHit
  //TODO->Recover here the simhit/hit duality if needed!!
  //if(((ActarSimROOTAnalysis*)gActarSimROOTAnalysis)->GetUseCrystalHitSim()==0){
    thePlaHit = new ActarSimPlaHit*[NbCrystalsWithHit];
    for (G4int i=0;i<NbCrystalsWithHit;i++)
      thePlaHit[i] = new ActarSimPlaHit();
  //}
  //else{  //In case that the simulated hits were used!
  //  theSciHit = new ActarSimSciHit*[NbCrystalsWithHit];
  //  for (G4int i=0;i<NbCrystalsWithHit;i++)
  //    theSciHit[i] = new ActarSimSciHitSim();
  //}

  //Clear the ClonesArray before filling it
  plaHitCA->Clear();

  G4bool counted = 0;
  hitsCounter = 0; //now this variable is going to count the added GeantHits
  G4String* name;
  G4int* detID;
  detID = new G4int[NbCrystalsWithHit];
  name = new G4String[NbCrystalsWithHit];

  G4bool shouldThisBeStored = 0;
  //Filling the ActarSimSciHit from the R3BCalGeantHit (or step)
  for (G4int i=0;i<NbHits;i++) {
    //do not accept GeantHits with edep=0
    //if there is no other GeantHit with edep>0 in the same crystal!
    shouldThisBeStored=0;
    counted =0;
    //G4cout << "ADDING THE HITS. GeantHit with name:" << (*hitsCollection)[i]->GetDetName()
    //   << " detID:"<< (*hitsCollection)[i]->GetDetID() << " edep:"<< (*hitsCollection)[i]->GetEdep() <<" under consideration"<< G4endl;
    for (G4int j=0;j<NbCrystalsWithHit;j++) {
      if( (*hitsCollection)[i]->GetDetName() == nameWithHit[j] &&
	  (*hitsCollection)[i]->GetDetID() == detIDWithHit[j] ) {
	shouldThisBeStored=1;
	break;  //break stops the for() sentence as soon as one "energetic" partner is found
      }
    }
    if(!shouldThisBeStored) continue; //so, continue with next, if there is no "energetic partner"
    if(hitsCounter==0){ //only for the first geantHit accepted for storage
      name[hitsCounter] = (*hitsCollection)[i]->GetDetName();
      detID[hitsCounter] = (*hitsCollection)[i]->GetDetID();
      AddCalPlaHit(thePlaHit[hitsCounter],(*hitsCollection)[i],0);
      //G4cout << "ADD hit:   name:" << name[hitsCounter]
      //     << " detID:"<<detID[hitsCounter]  << " with code 0 (initial)" << G4endl;
      hitsCounter++;
    }
    else {   // from the second in advance, compare if a R3BCalCrystalHit
             // in the same volume already exists
      for (G4int j=0;j<hitsCounter;j++) {
	if( (*hitsCollection)[i]->GetDetName() == name[j] &&
	    (*hitsCollection)[i]->GetDetID() == detID[j]     ){ //identical Hit already present
	  AddCalPlaHit(thePlaHit[j],(*hitsCollection)[i],1);
	  //G4cout << "ADD hit:   name:" << name[j]
	  // << " detID:"<< detID[j]  << " with code 1" << G4endl;
	  counted = 1;
	  break;   // stops the for() loop; the info is written in the first identical Hit
	}
      }
      if(counted==0) {	//No identical Hit present.
	name[hitsCounter] = (*hitsCollection)[i]->GetDetName();
	detID[hitsCounter] = (*hitsCollection)[i]->GetDetID();
	AddCalPlaHit(thePlaHit[hitsCounter],(*hitsCollection)[i],0);
	//G4cout << "ADD hit:   name:" << name[hitsCounter]
	//     << " detID:"<<detID[hitsCounter]  << " with code 0" << G4endl;
	hitsCounter++;
      }
    }
  }

  //BORRAME
  //G4cout << G4endl <<"************************  END OF DEBUGGING *****************************************" << G4endl;
  //G4cout <<"************************************************************************************" << G4endl;

  //at the end, fill the ClonesArray
  //TODO-> Recover here the simhit/hit duality if needed!!
  //if(((ActarSimROOTAnalysis*)gActarSimROOTAnalysis)->GetUseCrystalHitSim()==0){
    for (G4int i=0;i<NbCrystalsWithHit;i++)
      new((*plaHitCA)[i])ActarSimPlaHit(*thePlaHit[i]);
  //}
  //else{  //In case that the simulated hits were used!
  //  ActarSimSciHitSim* testSim;
  //  for (G4int i=0;i<NbCrystalsWithHit;i++) {
  //    testSim =  (ActarSimSciHitSim*) (theSciHit[i]);
  //    new((*sciHitCA)[i])ActarSimSciHitSim(*testSim);
  //  }
  //}

  // all branches at the same time!
  //G4cout << " #@BITCOUNT "<< crystalHitsBranch->Fill() << G4endl;
  //G4cout << " #@BITCOUNT "<< theR3BTree->Fill() << G4endl;

  delete [] detIDWithSomeEnergy;
  delete [] nameWithSomeEnergy;
  delete [] detIDWithHit;
  delete [] nameWithHit;
  delete [] detID;
  delete [] name;
  for (G4int i=0;i<NbCrystalsWithHit;i++) delete thePlaHit[i];
  delete [] thePlaHit;
 
 }
 
 */ 
	//End of CsI Like hit
 



void ActarSimROOTAnalPla::AddCalPlaHit(ActarSimPlaHit* cHit,
				      ActarSimPlaGeantHit* gHit,
				      G4int mode) {
  //
  // Function to move the information from the ActarSimSciGeantHit (a step hit)
  // to ActarSimSciHit (an event hit) for the Darmstadt-Heidelberg Crystall Ball.
  // Two modes are possible:
  // mode == 0 : creation; the ActarSimSciHit is void and is
  //             filled by the data from the ActarSimSciGeantHit
  // mode == 1 : addition; the ActarSimSciHit was already created
  //             by other ActarSimSciGeantHit and some data members are updated

  if(mode == 0) { //creation
    if( gHit->GetDetName() == "plaPhys" )   ; //cHit->SetType(1);
    else G4cout << "ERROR in R3BROOTAnalCal::AddCalCrystalHit()." << G4endl
                << "Unknown Detector Name: "<< gHit->GetDetName() << G4endl << G4endl;
	  
	  cHit->SetDetectorID(gHit->GetDetID());
	  
	  //cHit->SetDetCenterCoordinateX(gHit->GetDetCenterCoordinate().x()/mm); // center of the present silicon, dypang 090130
	  //cHit->SetDetCenterCoordinateY(gHit->GetDetCenterCoordinate().y()/mm); // center of the present silicon, dypang 090130
	  //cHit->SetDetCenterCoordinateZ(gHit->GetDetCenterCoordinate().z()/mm); // center of the present silicon, dypang 090130
	  
	  cHit->SetXPos(gHit->GetLocalPrePos().x()/mm);
	  cHit->SetYPos(gHit->GetLocalPrePos().y()/mm);
	  cHit->SetZPos(gHit->GetLocalPrePos().z()/mm);
	  
	  cHit->SetTime(gHit->GetToF()/ns);
	  cHit->SetEnergy(gHit->GetEdep()/MeV);
	  cHit->SetEBeforePla(gHit->GetEBeforePla()/MeV);
	  cHit->SetEAfterPla(gHit->GetEAfterPla()/MeV);
	  
	  cHit->SetTrackID(gHit->GetTrackID());
	  cHit->SetEventID(GetTheEventID());
	  cHit->SetRunID(GetTheRunID());
	  
	  cHit->SetParticleID(gHit->GetParticleID());
	  cHit->SetParticleCharge(gHit->GetParticleCharge());
	  cHit->SetParticleMass(gHit->GetParticleMass());
	  
	  cHit->SetStepsContributing(1);

	  
	   //CsI Like
   /* cHit->SetCopy(gHit->GetDetID());

    cHit->SetEnergy(gHit->GetEdep()/ MeV);
    cHit->SetTime(gHit->GetToF() / ns);

	cHit->SetTrackID(gHit->GetTrackID());  
    cHit->SetEventID(GetTheEventID());
    cHit->SetRunID(GetTheRunID());

    cHit->SetParticleID(gHit->GetParticleID());
    cHit->SetParticleCharge(gHit->GetParticleCharge());
    cHit->SetParticleMass(gHit->GetParticleMass());*/ //CsI-like

  //TODO-> Recover here the simhit/hit duality if needed!!
/*
    if(((ActarSimROOTAnalysis*)gActarSimROOTAnalysis)->GetUseCrystalHitSim()!=0){
      if( fabs(gHit->GetLocalPos().z())> 120 )
	((ActarSimSciHitSim*)cHit)->SetEnergyPerZone(24,gHit->GetEdep()/ MeV);
      else {
	G4int bin = (G4int)((gHit->GetLocalPos().z()/10) + 12);
	((ActarSimSciHitSim*)cHit)->SetEnergyPerZone( bin,
							gHit->GetEdep()/ MeV);
	//G4cout << G4endl << "Initial posZ:" << gHit->GetLocalPos().z() << "  bin " << bin
	//       << " E:" <<  ((ActarSimSciHitSim*)cHit)->GetEnergyPerZone( bin ) << G4endl << G4endl;
      }
      ((ActarSimSciHitSim*)cHit)->SetNbOfSteps(1);
      ((ActarSimSciHitSim*)cHit)->SetTimeFirstStep(gHit->GetToF() / ns);
      ((ActarSimSciHitSim*)cHit)->SetTimeLastStep(gHit->GetToF() / ns);
      ((ActarSimSciHitSim*)cHit)->SetNbOfPrimaries(GetPrimNbOfParticles());
      ((ActarSimSciHitSim*)cHit)->SetEnergyPrimary(GetPrimEnergy() / MeV);
      ((ActarSimSciHitSim*)cHit)->SetThetaPrimary(GetPrimTheta() / rad);
      ((ActarSimSciHitSim*)cHit)->SetPhiPrimary(GetPrimPhi() / rad);
      if(gHit->GetProcessName()=="phot") ((ActarSimSciHitSim*)cHit)->SetFirstInteractionType(1);
      else if(gHit->GetProcessName()=="compt") ((ActarSimSciHitSim*)cHit)->SetFirstInteractionType(2);
      else if(gHit->GetProcessName()=="conv") ((ActarSimSciHitSim*)cHit)->SetFirstInteractionType(3);
      else if(gHit->GetProcessName()=="msc") ((ActarSimSciHitSim*)cHit)->SetFirstInteractionType(4);
      else if(gHit->GetProcessName()=="eBrem") ((ActarSimSciHitSim*)cHit)->SetFirstInteractionType(5);
      else if(gHit->GetProcessName()=="Transportation") ((ActarSimSciHitSim*)cHit)->SetFirstInteractionType(6);
      else ((ActarSimSciHitSim*)cHit)->SetFirstInteractionType(0);
      ((ActarSimSciHitSim*)cHit)->SetFirstInteractionX(gHit->GetLocalPos().x());
      ((ActarSimSciHitSim*)cHit)->SetFirstInteractionY(gHit->GetLocalPos().y());
      ((ActarSimSciHitSim*)cHit)->SetFirstInteractionZ(gHit->GetLocalPos().z());
    }
*/
  }
  else if(mode==1){ //addition
    cHit->SetEnergy(cHit->GetEnergy() + gHit->GetEdep()/ MeV);
	  
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
	  
	  
	  
	  
    if(gHit->GetToF()<cHit->GetTime()) cHit->SetTime(gHit->GetToF()/ ns);

    //TODO-> Recover here the simhit/hit duality if needed!!
/*
    if(((ActarSimROOTAnalysis*)gActarSimROOTAnalysis)->GetUseCrystalHitSim()!=0){
      if( fabs(gHit->GetLocalPos().z())> 120 )
	((R3BCalCrystalHitSim*)cHit)->SetEnergyPerZone(24,
						       ((R3BCalCrystalHitSim*)cHit)->GetEnergyPerZone(24)+gHit->GetEdep()/ MeV);
      else {
	G4int bin = (G4int)((gHit->GetLocalPos().z()/10) + 12);
	((R3BCalCrystalHitSim*)cHit)->SetEnergyPerZone( bin,
							((R3BCalCrystalHitSim*)cHit)->GetEnergyPerZone(bin)+gHit->GetEdep()/ MeV);
	//G4cout << G4endl << "Adding posZ:" << gHit->GetLocalPos().z() << "  bin " << bin << " E:"
	//       << ((R3BCalCrystalHitSim*)cHit)->GetEnergyPerZone( bin ) << G4endl << G4endl;
      }
      ((R3BCalCrystalHitSim*)cHit)->SetNbOfSteps(((R3BCalCrystalHitSim*)cHit)->GetNbOfSteps()+1);
      if(gHit->GetToF()<((R3BCalCrystalHitSim*)cHit)->GetTime()){
	((R3BCalCrystalHitSim*)cHit)->SetTime(gHit->GetToF()/ ns);
	((R3BCalCrystalHitSim*)cHit)->SetTimeFirstStep(gHit->GetToF() / ns);
	if(gHit->GetProcessName()=="phot") ((R3BCalCrystalHitSim*)cHit)->SetFirstInteractionType(1);
	else if(gHit->GetProcessName()=="compt") ((R3BCalCrystalHitSim*)cHit)->SetFirstInteractionType(2);
	else if(gHit->GetProcessName()=="conv") ((R3BCalCrystalHitSim*)cHit)->SetFirstInteractionType(3);
	else if(gHit->GetProcessName()=="msc") ((R3BCalCrystalHitSim*)cHit)->SetFirstInteractionType(4);
	else if(gHit->GetProcessName()=="eBrem") ((R3BCalCrystalHitSim*)cHit)->SetFirstInteractionType(5);
	else if(gHit->GetProcessName()=="Transportation") ((R3BCalCrystalHitSim*)cHit)->SetFirstInteractionType(6);
	else ((R3BCalCrystalHitSim*)cHit)->SetFirstInteractionType(0);
	((R3BCalCrystalHitSim*)cHit)->SetFirstInteractionX(gHit->GetLocalPos().x());
	((R3BCalCrystalHitSim*)cHit)->SetFirstInteractionY(gHit->GetLocalPos().y());
	((R3BCalCrystalHitSim*)cHit)->SetFirstInteractionZ(gHit->GetLocalPos().z());
      }
      else if(gHit->GetToF()>((R3BCalCrystalHitSim*)cHit)->GetTimeLastStep())
	((R3BCalCrystalHitSim*)cHit)->SetTimeLastStep(gHit->GetToF());
    }
*/
  }
}
