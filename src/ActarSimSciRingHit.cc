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

#include "ActarSimSciRingHit.hh"
#include "G4ios.hh"
#include "globals.hh"

ClassImp(ActarSimSciRingHit)

ActarSimSciRingHit::ActarSimSciRingHit(){
  //
  // Constructor with initialization to zero
  //

  type = 0;
  copy = 0;

  time = 0.;
  energy = 0.;

  eventID = 0;
  runID = 0;
	trackID=0;

  particleID=0;
  particleCharge=0.;
  particleMass=0.;

}

ActarSimSciRingHit::~ActarSimSciRingHit(){
  //
  // Destructor, nothing to do
  //
}


void ActarSimSciRingHit::print(void){
  //
  // Printing data information
  //
    /*
    G4cout << "-------------------------------------------" << G4endl;
    G4cout << "------- ActarSimSciRingHit::print() ---------" << G4endl;
    G4cout << " type: " << type
    << ", copy: " << copy
    << G4endl;
    G4cout << " energy: " << energy / MeV << " MeV"
    << ", time: " << time / ns << " ns"
    << G4endl;
    G4cout << "-------------------------------------------" << G4endl;
  */
}


