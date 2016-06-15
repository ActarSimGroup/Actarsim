/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez Pol
//*-- Date: 04/2008
//*-- Last Update: 07/01/2015
// --------------------------------------------------------------
// Description:
//   A Silicon hit
//
// --------------------------------------------------------------
// Comments:
//
//
/////////////////////////////////////////////////////////////////

#include "ActarSimSilRingHit.hh"
#include "G4ios.hh"
#include "globals.hh"

ClassImp(ActarSimSilRingHit)

ActarSimSilRingHit::ActarSimSilRingHit(){
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

ActarSimSilRingHit::~ActarSimSilRingHit(){
  //
  // Destructor, nothing to do
  //
}


void ActarSimSilRingHit::print(void){
  //
  // Printing data information
  //


}
