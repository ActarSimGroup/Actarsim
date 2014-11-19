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

#include "ActarSimPlaHit.hh"
#include "G4ios.hh"
#include "globals.hh"

ClassImp(ActarSimPlaHit)

ActarSimPlaHit::ActarSimPlaHit(){
  //
  // Constructor with initialization to zero
  //
	
	detectorID = 0;
	
	detCenterCoordinateX=0.; // center of the present silicon, dypang 090130
	detCenterCoordinateY=0.; // center of the present silicon, dypang 090130
	detCenterCoordinateZ=0.; // center of the present silicon, dypang 090130
	
	xpos=0;
	ypos=0;
	zpos=0;

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

ActarSimPlaHit::~ActarSimPlaHit(){
  //
  // Destructor, nothing to do
  //
}


void ActarSimPlaHit::print(void){
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


