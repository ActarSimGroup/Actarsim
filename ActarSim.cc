/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol (hapol@fpddux.usc.es)
//             Beatriz Fernandez (bfd@ns.ph.liv.ac.uk)
//             Esther Estevez (hachebarra79@yahoo.es)
//             Elisangela Benjamim (ebenjami@usc.es)
//*-- Date: 03/2006
//*-- Last Update: 28/10/05
//*-- Copyright: GENP (Univ. Santiago de Compostela)
//
// --------------------------------------------------------------
// Comments:
//    ACTARSIM is an application for the simulation of the
//    ACTive TArget detector. 
// 
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include <time.h>

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "G4UIQt.hh"

#ifdef G4UI_USE_ROOT
#include "G4UIRoot.hh"
#endif

#ifdef G4UI_USE_XM
#include "G4UIXm.hh"
#endif

#include "Randomize.hh"

#ifdef G4VIS_USE
#include "ActarSimVisManager.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include "ActarSimDetectorConstruction.hh"
#include "ActarSimPhysicsList.hh"
#include "ActarSimPrimaryGeneratorAction.hh"
#include "ActarSimRunAction.hh"
#include "ActarSimEventAction.hh"
#include "ActarSimSteppingAction.hh"
#include "ActarSimSteppingVerbose.hh"
#include "ActarSimROOTAnalysis.hh"

int main(int argc,char** argv)
{
  // choose the Random engine
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  G4long seed=time(0); //returns time in seconds as an integer
  CLHEP::HepRandom::setTheSeed(seed);//changes the seed of the random engine


  //my Verbose output class
  G4VSteppingVerbose::SetInstance(new ActarSimSteppingVerbose);
  
  // Construct the default run manager
  G4RunManager* runManager = new G4RunManager;
  
  // set mandatory initialization classes
  ActarSimDetectorConstruction* detector = new ActarSimDetectorConstruction;
  runManager->SetUserInitialization(detector);
  runManager->SetUserInitialization(new ActarSimPhysicsList);
  
  // histogramming
  //ActarSimROOTAnalysis *analysis = new ActarSimROOTAnalysis(detector);
  ActarSimROOTAnalysis *analysis = new ActarSimROOTAnalysis();

  G4UIsession* session=0;

  if (argc==1){   // Define UI session for interactive mode.
#if defined (G4UI_USE_ROOT)
    // G4URoot is a ROOT based GUI.
    session = new G4UIRoot(argc,argv);
#elif defined (G4UI_USE_XM)
    // G4UIXm is an Xm based GUI.
    session = new G4UIXm(argc,argv);
#elif defined(G4UI_USE_TCSH)
    session = new G4UIterminal(new G4UItcsh);  
#else
    session = new G4UIterminal();
#endif
#ifdef G4UI_USE
    session = new G4UIQt(argc,argv);
#endif
  }
#ifdef G4VIS_USE
  // visualization manager
  G4VisManager* visManager = new ActarSimVisManager;
  visManager->Initialize();
#endif
  
  // set mandatory user action class
  runManager->SetUserAction(new ActarSimPrimaryGeneratorAction);
  runManager->SetUserAction(new ActarSimRunAction);
  ActarSimEventAction* eventaction = new ActarSimEventAction;
  runManager->SetUserAction(eventaction);
  runManager->SetUserAction(new ActarSimSteppingAction(detector,eventaction));
  
  // Initialize G4 kernel -->Make it manually to allow the definition of
  //    commands in PreInit state (for instance to define the PhysicsList)
  //runManager->Initialize();

  // get the pointer to the UI manager and set verbosities
  G4UImanager* UI = G4UImanager::GetUIpointer();

  if (session) {
    // G4UIterminal is a (dumb) terminal.      
    //UI->ApplyCommand("/control/execute vis2.mac");    
    UI->ApplyCommand("/control/execute gui.mac");
    session->SessionStart();
    if ( (G4UImanager::GetUIpointer()) &&
	 (session ==(G4UImanager::GetUIpointer())->GetSession()) )
      delete session;
  }
  else {
    //This part should be used for batch
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UI->ApplyCommand(command+fileName);
  }

  // job termination 
  delete analysis;
#ifdef G4VIS_USE
  delete visManager;
#endif
  G4cout << "RunManager deleting... "<< G4endl;
  delete runManager;
  
return 0;
}
  


