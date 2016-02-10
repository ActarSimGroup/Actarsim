/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol
//*-- Date: 11/2004
//*-- Last Update: 23/12/14
// --------------------------------------------------------------
// Description:
//   Controls the actions to perform during the steps. Note
// that unneeded code lines at this point will affect performance.
//
// --------------------------------------------------------------
// Comments:
//   - 30/11/04 Created based on example/novice/N03 structure
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include "ActarSimSteppingAction.hh"

#include "ActarSimROOTAnalysis.hh"

#include "ActarSimDetectorConstruction.hh"
#include "ActarSimEventAction.hh"

#include "G4Track.hh"

ActarSimSteppingAction::ActarSimSteppingAction(ActarSimDetectorConstruction* det,
					       ActarSimEventAction* evt)
  :detector(det), eventaction(evt){
  //
  //
  //
	if(detector){
	;}
	if(eventaction){
	;}
}


ActarSimSteppingAction::~ActarSimSteppingAction() {
  //
  //
  //
}


void ActarSimSteppingAction::UserSteppingAction(const G4Step* aStep) {
  //
  //G4 actions to perform every step. All actions moved to
  // ROOT analysis classes for easier ROOT output
  //

  const G4Track* track = aStep->GetTrack();
  G4VPhysicalVolume* volume = track->GetVolume();
  if(volume){;} // to quiet the compiler

  // collect energy and track length step by step
  G4double edep = aStep->GetTotalEnergyDeposit();
  if(edep){;} // to quiet the compiler

  G4double stepl = 0.;
  if(stepl){;} // to quiet the compiler
  if (track->GetDefinition()->GetPDGCharge() != 0. && track->GetParentID()==0)
    stepl = aStep->GetStepLength();
  else stepl = 0;

  // Histogramming
  if (gActarSimROOTAnalysis)
    gActarSimROOTAnalysis->UserSteppingAction(aStep); // original
 //     gActarSimROOTAnalysis->UserSteppingAction(aStep,eventaction); // dypang 080225

  // save the random number seed of this event, under condition
  // if(condition) G4RunManager::GetRunManager()->rndmSaveThisEvent();
}
