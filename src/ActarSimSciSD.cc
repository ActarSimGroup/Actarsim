/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez Pol    hapolyo@usc.es
//*-- Date: 04/2008
//*-- Last Update:
// --------------------------------------------------------------
// Description:
//   SD for the Scintillators
//
// --------------------------------------------------------------
// Comments:
//
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include "ActarSimSciSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4UnitsTable.hh"

#include "G4VProcess.hh"
#include "G4VPhysicalVolume.hh"
#include "G4TouchableHistory.hh"
#include "G4VTouchable.hh"

ActarSimSciSD::ActarSimSciSD(G4String name)
  :G4VSensitiveDetector(name){
  //
  // Constructor,
  // just naming the Hit collection
  //
  G4String HCname;
  collectionName.insert(HCname="SciCollection");
}


ActarSimSciSD::~ActarSimSciSD(){
  //
  // Destructor, nothing to do
  //
}


void ActarSimSciSD::Initialize(G4HCofThisEvent* HCE){
  //
  // Initializing the ActarSimSciGeantHitsCollection object
  // Invoked automatically at the beggining of each event
  //
  hitsCollection = new ActarSimSciGeantHitsCollection
    (SensitiveDetectorName,collectionName[0]);
  static G4int HCID = -1;
  if(HCID<0)
    HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);

  HCE->AddHitsCollection( HCID, hitsCollection );
}


G4bool ActarSimSciSD::ProcessHits(G4Step* aStep,G4TouchableHistory*){
  //
  // Filling the ActarSimSciGeantHit information with the step info
  // Invoked by G4SteppingManager for each step
  //
  G4double edep = aStep->GetTotalEnergyDeposit();

  if(edep==0.) return false;

  ActarSimSciGeantHit* newHit = new ActarSimSciGeantHit();

  newHit->SetEdep(edep);

  newHit->SetPos(aStep->GetPostStepPoint()->GetPosition());
  //The elements should be taken of the PostStep, the real parameters of the
  //interaction. The PreStep keeps record of where the particle comes from
  newHit->SetPrePos(aStep->GetPreStepPoint()->GetPosition());

  //for the local translation, x = R-1 (x_m - T)
  G4ThreeVector theLocalPos =
    (aStep->GetPostStepPoint()->GetPhysicalVolume()->GetObjectRotationValue().inverse()) *
    (aStep->GetPostStepPoint()->GetPosition()-aStep->GetPostStepPoint()->GetPhysicalVolume()->GetObjectTranslation());
  newHit->SetLocalPos(theLocalPos);

  theLocalPos = //just using the same variable...
    (aStep->GetPreStepPoint()->GetPhysicalVolume()->GetObjectRotationValue().inverse())*
    (aStep->GetPreStepPoint()->GetPosition()-aStep->GetPreStepPoint()->GetPhysicalVolume()->GetObjectTranslation());
  newHit->SetLocalPrePos(theLocalPos);

  newHit->SetDetName(aStep->GetTrack()->GetVolume()->GetName());
  newHit->SetPreDetName(aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName());
  newHit->SetPostDetName(aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName());
  newHit->SetDetID(aStep->GetTrack()->GetVolume()->GetCopyNo());

  newHit->SetToF(aStep->GetPostStepPoint()->GetGlobalTime());

  newHit->SetTrackID(aStep->GetTrack()->GetTrackID());
  newHit->SetParentID(aStep->GetTrack()->GetParentID());
  newHit->SetParticleID(aStep->GetTrack()->GetDefinition()->GetPDGEncoding());
  newHit->SetParticleCharge(aStep->GetTrack()->GetDefinition()->GetPDGCharge());
  newHit->SetParticleMass(aStep->GetTrack()->GetDefinition()->GetPDGMass());

  hitsCollection->insert(newHit);

  // newHit cannot be deleted here !
  // It should be deleted after the end of the event

  return true;
}


void ActarSimSciSD::EndOfEvent(G4HCofThisEvent*){
  //
  // Just prints and draws the event hits (class ActarSimSciGeantHit)
  // The recollection of the hits energy deposition in the plastic
  // is done in the ActarSimROOTAnalysis::EndOfEventAction()
  //
  G4int NbHits = hitsCollection->entries();
  if (verboseLevel>0) {
    G4cout << "Hits Collection: in this event they are " << NbHits
	   << " (GEANT-like) hits in the Sci: " << G4endl;
    for (G4int i=0;i<NbHits;i++) (*hitsCollection)[i]->Print();
  }
  //for (G4int i=0;i<NbHits;i++) (*hitsCollection)[i]->Draw();
}
