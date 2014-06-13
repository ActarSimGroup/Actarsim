/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez Pol     hapolyo@usc.es
//*-- Date: 04/2008
//*-- Last Update:
// --------------------------------------------------------------
// Description:
//   A Scintillator hit
//
// --------------------------------------------------------------
// Comments:
//
//
/////////////////////////////////////////////////////////////////

#include "ActarSimSciHit.hh"
#include "G4ios.hh"
#include "globals.hh"

ClassImp(ActarSimSciHit)

ActarSimSciHit::ActarSimSciHit(){
  //
  // Constructor with initialization to zero
  //

  type = 0;
  copy = 0;

  time = 0.;
  energy = 0.;

  eventID = 0;
  runID = 0;

  particleID=0;
  particleCharge=0.;
  particleMass=0.;

}

ActarSimSciHit::~ActarSimSciHit(){
  //
  // Destructor, nothing to do
  //
}


void ActarSimSciHit::print(void){
  //
  // Printing data information
  //
    /*
    G4cout << "-------------------------------------------" << G4endl;
    G4cout << "------- ActarSimSciHit::print() ---------" << G4endl;
    G4cout << " type: " << type
    << ", copy: " << copy
    << G4endl;
    G4cout << " energy: " << energy / MeV << " MeV"
    << ", time: " << time / ns << " ns"
    << G4endl;
    G4cout << "-------------------------------------------" << G4endl;
  */
}


