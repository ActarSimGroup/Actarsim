// - AUTHOR: Hector Alvarez-Pol 04/2008
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/
//////////////////////////////////////////////////////////////////
/// \class ActarSimSciHit
/// A Scintillator hit
/////////////////////////////////////////////////////////////////

#include "ActarSimSciHit.hh"
#include "G4ios.hh"
#include "globals.hh"

ClassImp(ActarSimSciHit)

//////////////////////////////////////////////////////////////////
/// Constructor with initialization to zero
ActarSimSciHit::ActarSimSciHit(){
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

//////////////////////////////////////////////////////////////////
/// Destructor, nothing to do
ActarSimSciHit::~ActarSimSciHit(){
}

//////////////////////////////////////////////////////////////////
/// Printing data information (commented out for the moment)
void ActarSimSciHit::print(void){
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
