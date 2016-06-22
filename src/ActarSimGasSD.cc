// - AUTHOR: Hector Alvarez-Pol 04/2006
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/
//////////////////////////////////////////////////////////////////
/// \class ActarSimGasSD
/// SD for the gas volume in the detector
/////////////////////////////////////////////////////////////////

#include "ActarSimGasSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4UnitsTable.hh"

#include "G4VPhysicalVolume.hh"
#include "G4TouchableHistory.hh"
#include "G4VTouchable.hh"

//////////////////////////////////////////////////////////////////
/// Constructor
ActarSimGasSD::ActarSimGasSD(G4String name):G4VSensitiveDetector(name){
  G4String HCname;
  collectionName.insert(HCname="gasCollection");
}

//////////////////////////////////////////////////////////////////
/// Destructor
ActarSimGasSD::~ActarSimGasSD(){
}

//////////////////////////////////////////////////////////////////
/// Initializing the ActarSimCalGeantHitsCollection object
/// Invoked automatically at the beggining of each event
void ActarSimGasSD::Initialize(G4HCofThisEvent* HCE){

  hitsCollection = new ActarSimGasGeantHitsCollection
    (SensitiveDetectorName,collectionName[0]);
  static G4int HCID = -1;
  if(HCID<0)
    HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);

  HCE->AddHitsCollection( HCID, hitsCollection );
}

//////////////////////////////////////////////////////////////////
/// Filling the ActarSimCalGeantHit information with the step info.
/// Invoked by G4SteppingManager for each step
G4bool ActarSimGasSD::ProcessHits(G4Step* aStep,G4TouchableHistory*){

  //G4double edep = aStep->GetTotalEnergyDeposit()/MeV;
  G4double edep = -aStep->GetDeltaEnergy()/MeV;

  if(edep==0.) return false;

  ActarSimGasGeantHit* newHit = new ActarSimGasGeantHit();

  newHit->SetTrackID(aStep->GetTrack()->GetTrackID());
  newHit->SetParentID(aStep->GetTrack()->GetParentID());
  //newHit->SetStep(aStep);

  newHit->SetEdep(edep);
  newHit->SetParticleCharge(aStep->GetTrack()->GetDefinition()->GetPDGCharge());
  newHit->SetParticleMass(aStep->GetTrack()->GetDefinition()->GetPDGMass());
  newHit->SetParticleID(aStep->GetTrack()->GetDefinition()->GetPDGEncoding());
  newHit->SetPrePos(aStep->GetPreStepPoint()->GetPosition()/mm);
  newHit->SetPostPos(aStep->GetPostStepPoint()->GetPosition()/mm);
  newHit->SetPreToF(aStep->GetPreStepPoint()->GetGlobalTime()/ns);
  newHit->SetPostToF(aStep->GetPostStepPoint()->GetGlobalTime()/ns);
  newHit->SetStepLength(aStep->GetStepLength()/mm);
  newHit->SetStepEnergy(aStep->GetTrack()->GetKineticEnergy()/MeV);

  newHit->SetDetName(aStep->GetTrack()->GetVolume()->GetName());
  newHit->SetDetID(aStep->GetTrack()->GetVolume()->GetCopyNo());

  //newHit->Print();

  hitsCollection->insert(newHit);

  // newHit cannot be deleted here !
  // It should be deleted after the end of the event

  return true;
}

//////////////////////////////////////////////////////////////////
/// Just prints and draws the event hits (class ActarSimGasGeantHit).
/// The recollection of the hits energy deposition in a crystal
/// is done in the ActarSimAnalysis::EndOfEventAction()
void ActarSimGasSD::EndOfEvent(G4HCofThisEvent*){

  G4int NbHits = hitsCollection->entries();
  if (verboseLevel>0) {
    G4cout << "Hits Collection: in this event they are " << NbHits
	   << " (GEANT-like) hits in the gas volume: " << G4endl;
    for (G4int i=0;i<NbHits;i++) (*hitsCollection)[i]->Print();
  }
  //for (G4int i=0;i<NbHits;i++) (*hitsCollection)[i]->Draw();
}
