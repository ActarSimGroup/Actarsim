// - AUTHOR: Hector Alvarez-Pol 04/2008
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/
//////////////////////////////////////////////////////////////////
/// \class ActarSimSilRingHit
/// A Silicon Ring hit
/////////////////////////////////////////////////////////////////

#include "ActarSimSilRingHit.hh"
#include "G4ios.hh"
#include "globals.hh"

ClassImp(ActarSimSilRingHit)

//////////////////////////////////////////////////////////////////
/// Constructor with initialization to zero
ActarSimSilRingHit::ActarSimSilRingHit(){
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

//////////////////////////////////////////////////////////////////
/// Destructor, nothing to do
ActarSimSilRingHit::~ActarSimSilRingHit(){
}

//////////////////////////////////////////////////////////////////
///  Printing data information. Not implemented yet.
void ActarSimSilRingHit::print(void){
}
