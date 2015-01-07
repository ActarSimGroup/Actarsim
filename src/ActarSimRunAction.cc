/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol
//*-- Date: 11/2004
//*-- Last Update: 07/01/15
// --------------------------------------------------------------
// Description:
//   Controls the actions to perform before and after a run.
//
// --------------------------------------------------------------
// Comments:
//   - 30/11/04 Created based on example/novice/N03 structure
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include "ActarSimRunAction.hh"

#include "ActarSimROOTAnalysis.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"

ActarSimRunAction::ActarSimRunAction(){
  //
  // Constructor. Makes nothing
  //
}


ActarSimRunAction::~ActarSimRunAction(){
  //
  // Destructor
  //
}


void ActarSimRunAction::BeginOfRunAction(const G4Run* aRun) {
  //
  // Actions to perform at the beginning og the run
  //
  
  const G4int verboseLevel = G4RunManager::GetRunManager()->GetVerboseLevel();
  if(verboseLevel>2){
    G4cout << "##################################################################"
	  << G4endl
	  << "###########   ActarSimRunAction::BeginOfRunAction()  ##############"
	  << G4endl
	  << "###    Run " << aRun->GetRunID() << " start." << G4endl;
    G4cout << "##################################################################"
	 << G4endl;
  }

  //inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(true);

  // Histogramming
  if (gActarSimROOTAnalysis) gActarSimROOTAnalysis->BeginOfRunAction(aRun);
}


void ActarSimRunAction::EndOfRunAction(const G4Run* aRun) {
  //
  // Actions to perform at the end of the run
  //
  // Histogramming
  if(gActarSimROOTAnalysis) gActarSimROOTAnalysis->EndOfRunAction(aRun);
}

