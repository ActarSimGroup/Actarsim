/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol
//*-- Date: 11/2004
//*-- Last Update: 1/12/14
// --------------------------------------------------------------
// Description:
//   Actions to be performed before or after each event
//
// --------------------------------------------------------------
// Comments:
//
//   - 30/11/04 Created based on example/novice/N03 structure
//   - 1/12/14 Cleaning old data structure
// --------------------------------------------------------------
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

ActarSimEventAction::ActarSimEventAction()
  :drawFlag("all"), printModulo(1) {
  //
  // Constructor
  //
  eventMessenger = new ActarSimEventActionMessenger(this);
}


ActarSimEventAction::~ActarSimEventAction() {
  //
  // Destructor
  //
  delete eventMessenger;
}


void ActarSimEventAction::BeginOfEventAction(const G4Event* evt){
  //
  // At the beginning...
  //
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


void ActarSimEventAction::EndOfEventAction(const G4Event* evt) {
  //
  //  After the end of the event...
  //
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

  //DPLoureiro Drawing track updated for GEANT 4.9 i_mode supressed. No more warnings
  G4VVisManager* pVisManager = G4VVisManager::GetConcreteInstance();
  if (pVisManager)
    {
      G4TrajectoryContainer* trajectoryContainer = evt->GetTrajectoryContainer();
      G4int n_trajectories = 0;
      if (trajectoryContainer) n_trajectories = trajectoryContainer->entries();
      
      for (G4int i=0; i<n_trajectories; i++)
        { G4VTrajectory* trj = (G4Trajectory*)((*(evt->GetTrajectoryContainer()))[i]);
	  if (drawFlag == "all") trj->DrawTrajectory();
	  else if ((drawFlag == "charged")&&(trj->GetCharge() != 0.))
	    trj->DrawTrajectory();
	  else if ((drawFlag == "neutral")&&(trj->GetCharge() == 0.))
	    trj->DrawTrajectory();
        }
    }
  
}

