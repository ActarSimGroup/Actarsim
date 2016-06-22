// - AUTHOR: Hector Alvarez-Pol 04/2008
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/
//////////////////////////////////////////////////////////////////
/// \class ActarSimPlaHit
/// A plastic (scintillator) hit.
/// [Y. Ayyad] We just add the same information as Silicon
/// to identify the scintillator bar.
/////////////////////////////////////////////////////////////////

#include "ActarSimPlaHit.hh"
#include "G4ios.hh"
#include "globals.hh"

ClassImp(ActarSimPlaHit)

//////////////////////////////////////////////////////////////////
/// Constructor with initialization to zero
ActarSimPlaHit::ActarSimPlaHit(){
  detectorID = 0;
  xpos=0;
  ypos=0;
  zpos=0;
  time = 0.;
  energy = 0.;
  eBeforePla = 0.;
  eAfterPla = 0.;
  eventID = 0;
  runID = 0;
  trackID=0;
  particleID=0;
  particleCharge=0.;
  particleMass=0.;
  stepsContributing = 0;
}

//////////////////////////////////////////////////////////////////
/// Destructor, nothing to do
ActarSimPlaHit::~ActarSimPlaHit(){
}

//////////////////////////////////////////////////////////////////
/// Printing data information (NOT IMPLEMENTED!)
void ActarSimPlaHit::print(void){
}
