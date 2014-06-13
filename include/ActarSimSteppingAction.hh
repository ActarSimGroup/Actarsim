/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol hapol@fpddux.usc.es
//*-- Date: 11/2004
//*-- Last Update: 28/10/05
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
//
/////////////////////////////////////////////////////////////////
#ifndef ActarSimSteppingAction_h
#define ActarSimSteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class ActarSimDetectorConstruction;
class ActarSimEventAction;

class ActarSimSteppingAction : public G4UserSteppingAction
{
  public:
    ActarSimSteppingAction(ActarSimDetectorConstruction*, ActarSimEventAction*);
   ~ActarSimSteppingAction();

    void UserSteppingAction(const G4Step*);

  private:
    ActarSimDetectorConstruction* detector;
    ActarSimEventAction*          eventaction;
};

#endif
