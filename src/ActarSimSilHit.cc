/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez Pol
//*-- Date: 04/2008
//*-- Last Update: 07/01/15
// --------------------------------------------------------------
// Description:
//   A Silicon hit
//
// --------------------------------------------------------------
// Comments:
//
//
/////////////////////////////////////////////////////////////////

#include "ActarSimSilHit.hh"
#include "G4ios.hh"
#include "globals.hh"

ClassImp(ActarSimSilHit)

ActarSimSilHit::ActarSimSilHit(){
  //
  // Constructor with initialization to zero
  //

  detectorID = 0;

  xpos=0;
  ypos=0;
  zpos=0;

  time = 0.;
  energy = 0.;

  trackID = 0;
  eventID = 0;
  runID = 0;

  particleID = 0;
  particleCharge = 0;
  particleMass=0;

  stepsContributing = 0;

}

ActarSimSilHit::~ActarSimSilHit(){
  //
  // Destructor, nothing to do
  //
}


void ActarSimSilHit::print(void){
  //
  // Printing data information
  //


}
