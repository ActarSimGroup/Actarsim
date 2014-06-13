/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol
//*-- Date: 11/2004
//*-- Last Update: 28/10/05
// --------------------------------------------------------------
// Description:
//   Actions to be performed before or after each event
//
// --------------------------------------------------------------
// Comments:
//
//   - 30/11/04 Created based on example/novice/N03 structure
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
  //G4double  energy1Gas, energy2Gas;
  //G4double  trackL1Gas, trackL2Gas;
  //G4ThreeVector lastPrim2Position;

  G4String  drawFlag;
  G4int     printModulo;

  ActarSimEventActionMessenger*  eventMessenger;

public:
  ActarSimEventAction();
  ~ActarSimEventAction();

  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);

//TODO->Remove completely after moving all functionality to ROOTAnalGas
  //void AddGas1(G4double de, G4double dl) {energy1Gas += de;
  //                                       trackL1Gas += dl;
  //	  };
  //void AddGas2(G4double de, G4double dl) {energy2Gas += de;
  //                                          trackL2Gas += dl;};

  void SetDrawFlag   (G4String val)  {drawFlag = val;};
  void SetPrintModulo(G4int    val)  {printModulo = val;};

/////  added by dypang for getting the accululated energy loss and track length of each step. dypang 080225
  //G4double GetEnergy1Gas(){return energy1Gas;};
  //G4double GetEnergy2Gas(){return energy2Gas;};
  //G4double GetTrackL1Gas(){return trackL1Gas;};
  //G4double GetTrackL2Gas(){return trackL2Gas;};
  //void SetLastPrim2Position(G4ThreeVector aPos) {lastPrim2Position=aPos;}; // 080302
///// end of dypang part. 080225

};

#endif


