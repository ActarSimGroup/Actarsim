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

#ifndef ActarSimEventAction_h
#define ActarSimEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

class ActarSimEventActionMessenger;

class ActarSimEventAction : public G4UserEventAction {

private:

  G4String  drawFlag;
  G4int     printModulo;

  ActarSimEventActionMessenger*  eventMessenger;

public:
  ActarSimEventAction();
  ~ActarSimEventAction();

  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);

  void SetDrawFlag   (G4String val)  {drawFlag = val;};
  void SetPrintModulo(G4int    val)  {printModulo = val;};

};

#endif


