// - AUTHOR: Hector Alvarez-Pol 11/2004
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/
//////////////////////////////////////////////////////////////////
/// \class ActarSimRunAction
/// Controls the actions to perform before and after a run.
/////////////////////////////////////////////////////////////////

#include "ActarSimRunAction.hh"

#include "ActarSimROOTAnalysis.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"

//////////////////////////////////////////////////////////////////
/// Constructor. Makes nothing
ActarSimRunAction::ActarSimRunAction(){
}

//////////////////////////////////////////////////////////////////
/// Destructor. Makes nothing
ActarSimRunAction::~ActarSimRunAction(){
}

//////////////////////////////////////////////////////////////////
/// Actions to perform at the beginning og the run
void ActarSimRunAction::BeginOfRunAction(const G4Run* aRun) {

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

//////////////////////////////////////////////////////////////////
/// Actions to perform at the end of the run
void ActarSimRunAction::EndOfRunAction(const G4Run* aRun) {
  // Histogramming
  if(gActarSimROOTAnalysis) gActarSimROOTAnalysis->EndOfRunAction(aRun);
}
