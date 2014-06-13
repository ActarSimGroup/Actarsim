/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez Pol     hapolyo@usc.es
//*-- Date: 04/2008
//*-- Last Update:
// --------------------------------------------------------------
// Description:
//   A Parisicon hit
//
// --------------------------------------------------------------
// Comments:
//
//
/////////////////////////////////////////////////////////////////

#include "ActarSimParisHit.hh"
#include "G4ios.hh"
#include "globals.hh"

ClassImp(ActarSimParisHit)

ActarSimParisHit::ActarSimParisHit(){
  //
  // Constructor with initialization to zero
  //

  detectorID = 0;
  motherID=0;

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
  process_name="";
}

ActarSimParisHit::~ActarSimParisHit(){
  //
  // Destructor, nothing to do
  //
}


void ActarSimParisHit::Print(void){
  //
  // Printing data information
  //


}


