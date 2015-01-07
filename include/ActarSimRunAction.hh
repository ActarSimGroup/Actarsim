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

#ifndef ActarSimRunAction_h
#define ActarSimRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

class ActarSimRunAction : public G4UserRunAction
{
  public:
    ActarSimRunAction();
   ~ActarSimRunAction();

  public:
    void BeginOfRunAction(const G4Run*);
    void EndOfRunAction(const G4Run*);

};


#endif

