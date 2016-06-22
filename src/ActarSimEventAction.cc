// - AUTHOR: Hector Alvarez-Pol 11/2004
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/
//////////////////////////////////////////////////////////////////
/// \class ActarSimEventAction
/// Actions to be performed before or after each event
/////////////////////////////////////////////////////////////////

#include "ActarSimEventAction.hh"
#include "ActarSimEventActionMessenger.hh"

#include "ActarSimROOTAnalysis.hh"

#include "G4Event.hh"
#include "G4TrajectoryContainer.hh"
#include "G4VTrajectory.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4UnitsTable.hh"
#include "G4ThreeVector.hh"

#include "G4RunManager.hh"

#include "Randomize.hh"
#include <iomanip>

//////////////////////////////////////////////////////////////////
/// Constructor
ActarSimEventAction::ActarSimEventAction()
  :drawFlag("all"), printModulo(1) {
  eventMessenger = new ActarSimEventActionMessenger(this);
}

//////////////////////////////////////////////////////////////////
/// Destructor
ActarSimEventAction::~ActarSimEventAction() {
  delete eventMessenger;
}

//////////////////////////////////////////////////////////////////
/// Actions performed at the beginning of each event
void ActarSimEventAction::BeginOfEventAction(const G4Event* evt){
  G4int evtNb = evt->GetEventID();

  const G4int verboseLevel = G4RunManager::GetRunManager()->GetVerboseLevel();
  if(verboseLevel>0){
    if (evtNb%printModulo == 0) {
      G4cout << "##################################################################"
	     << G4endl
	     << "########    ActarSimEventAction::BeginOfEventAction()   ##########"
	     << G4endl
	     << "########           Begin of event: " << evtNb << "        ########"<<  G4endl;
      CLHEP::HepRandom::showEngineStatus();
      G4cout << "##################################################################"
	     << G4endl;
    }
  }

  // Histogramming and other ROOT related actions
  if (gActarSimROOTAnalysis) gActarSimROOTAnalysis->BeginOfEventAction(evt);
}

//////////////////////////////////////////////////////////////////
/// Actions performed at the end of each event (some information
/// of the trajectories display is included)
void ActarSimEventAction::EndOfEventAction(const G4Event* evt) {
  G4int evtNb = evt->GetEventID();

  if (evtNb%printModulo == 0){
    G4cout << "##################################################################"
	   << G4endl
	   << "#########    ActarSimEventAction::EndOfEventAction()   #########"
	   << G4endl
	   << "#### End of event: " << evtNb << G4endl;
    G4cout << "##################################################################"
	   << G4endl;
  }

  // Histogramming
  if(gActarSimROOTAnalysis)
    gActarSimROOTAnalysis->EndOfEventAction(evt);

  // extract the trajectories and draw them

  // You can get a default drawing without this code by using, e.g.,
  // /vis/scene/add/trajectories 1000
  // The code here adds sophistication under control of drawFlag.

  // See comments in G4VTrajectory::DrawTrajectory for the
  // interpretation of the argument, 1000.

  G4VVisManager* pVisManager = G4VVisManager::GetConcreteInstance();
  if (pVisManager) {
    G4TrajectoryContainer* trajectoryContainer = evt->GetTrajectoryContainer();
    G4int n_trajectories = 0;
    if (trajectoryContainer) n_trajectories = trajectoryContainer->entries();

    for (G4int i=0; i<n_trajectories; i++) {
      G4VTrajectory* trj = (G4Trajectory*)((*(evt->GetTrajectoryContainer()))[i]);
      if (drawFlag == "all") trj->DrawTrajectory();
      else if ((drawFlag == "charged")&&(trj->GetCharge() != 0.))
	trj->DrawTrajectory();
      else if ((drawFlag == "neutral")&&(trj->GetCharge() == 0.))
	trj->DrawTrajectory();
    }
  }
}
