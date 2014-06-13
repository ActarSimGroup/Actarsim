//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//

#include "ParisTrackerSD.hh"
#include "ParisSingleHit.hh"

#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4VProcess.hh"
#include "G4ios.hh"

ParisTrackerSD::ParisTrackerSD(G4String name): G4VSensitiveDetector(name)
{
	G4String HCname = "ParisCollection"; collectionName.insert(HCname); 
	
}
ParisTrackerSD::~ParisTrackerSD(){;}

void ParisTrackerSD::Initialize(G4HCofThisEvent* HCE)
{
//	G4cout << " In ParisTrackerSD::Initialize " << G4endl;
	static int HCID = -1;
	trackerCollection = new ParisSingleHitsCollection(SensitiveDetectorName,collectionName[0]); 
	
	if ( HCID < 0 ) 
		HCID = GetCollectionID(0);
	HCE->AddHitsCollection(HCID,trackerCollection);
	//G4cout << " Out ParisTrackerSD::Initialize " << G4endl;
}

G4bool ParisTrackerSD::ProcessHits(G4Step* aStep, G4TouchableHistory *touch)
{
	G4String tmp;
	G4int temp_code;
	
//	G4cout << " In ParisTrackerSD::ProcessHits" << G4endl;
	// nothing to be stored if no energy 
				
	G4double edep = aStep->GetTotalEnergyDeposit();  if ( edep == 0. ) return false;

	// a new hit is created
	ParisSingleHit *newHit = new ParisSingleHit();
	
	// set hit properties  
	newHit->SetTrackID(aStep->GetTrack()->GetTrackID());
	newHit->SetParentID(aStep->GetTrack()->GetParentID());	
	newHit->SetPrimaryID(aStep->GetTrack()->GetParentID());
			
	newHit->SetEdep( edep );
	newHit->SetPos(  aStep->GetPostStepPoint()->GetPosition() );
	newHit->SetToF ( aStep->GetPostStepPoint()->GetGlobalTime() );	
	
	newHit->SetDetName  (aStep->GetTrack()->GetVolume()->GetName());
	//	newHit->SetDetID    (aStep->GetTrack()->GetVolume()->GetCopyNo());	
	
//	newHit->SetMotherDetName(touch->GetVolume()->GetName());
   	newHit->SetMotherID(aStep->GetPreStepPoint()->GetTouchableHandle()->GetReplicaNumber(2)); 
   	newHit->SetDetID(aStep->GetPreStepPoint()->GetTouchableHandle()->GetReplicaNumber(1)); 
	
	tmp = aStep->GetTrack()->GetDefinition()->GetParticleName();
	newHit->SetParticleName( tmp );

	temp_code = aStep->GetTrack()->GetDefinition()->GetPDGEncoding ();
	newHit->SetPDGcode(temp_code);
	
	tmp = aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();	
	newHit->SetProcessName( tmp );
		
	// add this hit to the collection
	trackerCollection->insert( newHit );

//	G4cout << " Out ParisTrackerSD::ProcessHits" << G4endl;
	return true;
}

void ParisTrackerSD::EndOfEvent(G4HCofThisEvent*)
{
  G4int NbHits = trackerCollection->entries();
  if (verboseLevel>0) {
    G4cout << "Tracker Hits Collection: in this event they are " << NbHits
	   << " (GEANT-like) hits in PARIS: " << G4endl;
    for (G4int i=0;i<NbHits;i++) (*trackerCollection)[i]->Print();
    }
}

void ParisTrackerSD::clear()
{
} 

void ParisTrackerSD::DrawAll()
{
} 

void ParisTrackerSD::PrintAll()
{
} 
