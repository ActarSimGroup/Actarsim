// - AUTHOR: Hector Alvarez-Pol 11/2004
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/
//////////////////////////////////////////////////////////////////
/// \class ActarSimSteppingAction
/// Controls the actions to perform during the steps. Note
/// that unneeded code lines at this point will affect performance.
/////////////////////////////////////////////////////////////////

#include "ActarSimSteppingAction.hh"

#include "ActarSimROOTAnalysis.hh"

#include "ActarSimDetectorConstruction.hh"
#include "ActarSimEventAction.hh"

#include "G4Track.hh"

//////////////////////////////////////////////////////////////////
/// Constructor
ActarSimSteppingAction::ActarSimSteppingAction(ActarSimDetectorConstruction* det,
					       ActarSimEventAction* evt)
  :detector(det), eventaction(evt){
  if(detector){	; }
  if(eventaction){ ; }
}

//////////////////////////////////////////////////////////////////
/// Destructor
ActarSimSteppingAction::~ActarSimSteppingAction() {
}

//////////////////////////////////////////////////////////////////
/// G4 actions to perform every step. All actions moved to
/// ROOT analysis classes for easier ROOT output
void ActarSimSteppingAction::UserSteppingAction(const G4Step* aStep) {
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
