// - AUTHOR: Hector Alvarez-Pol 04/2006
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/
//////////////////////////////////////////////////////////////////
/// \class ActarSimGasGeantHit
/// A Geant Hit in the calorimeter volume. It represents the
/// information of each step with energy deposited in the
/// calorimeter volume.
/////////////////////////////////////////////////////////////////

#include "ActarSimGasGeantHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4Allocator<ActarSimGasGeantHit> ActarSimGasGeantHitAllocator;

//////////////////////////////////////////////////////////////////
/// Constructor
ActarSimGasGeantHit::ActarSimGasGeantHit() {
}

//////////////////////////////////////////////////////////////////
/// Destructor
ActarSimGasGeantHit::~ActarSimGasGeantHit() {
}

//////////////////////////////////////////////////////////////////
/// Copy constructor
ActarSimGasGeantHit::ActarSimGasGeantHit(const ActarSimGasGeantHit& right) : G4VHit() {
  trackID = right.trackID;
  parentID = right.parentID;
  edep = right.edep;
  particleCharge = right.particleCharge;
  particleMass = right.particleMass;
  particleID = right.particleID;
  prePos = right.prePos;
  postPos = right.postPos;
  detName = right.detName;
  detID = right.detID;
  preToF = right.preToF;
  postToF = right.postToF;
  stepLength = right.stepLength;
  stepEnergy = right.stepEnergy;
}

//////////////////////////////////////////////////////////////////
/// Operator =
const ActarSimGasGeantHit& ActarSimGasGeantHit::operator=(const ActarSimGasGeantHit& right){
  trackID = right.trackID;
  parentID = right.parentID;
  edep = right.edep;
  particleCharge = right.particleCharge;
  particleMass = right.particleMass;
  particleID = right.particleID;
  prePos = right.prePos;
  postPos = right.postPos;
  detName = right.detName;
  detID = right.detID;
  preToF = right.preToF;
  postToF = right.postToF;
  stepLength = right.stepLength;
  stepEnergy = right.stepEnergy;

  return *this;
}

//////////////////////////////////////////////////////////////////
/// Operator ==
G4int ActarSimGasGeantHit::operator==(const ActarSimGasGeantHit& right) const{
  return (this==&right) ? 1 : 0;
}

//////////////////////////////////////////////////////////////////
/// Draws the Hit. A clear red point on the Hit position
void ActarSimGasGeantHit::Draw(){
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager) {
    G4Circle circle(prePos);
    circle.SetScreenSize(4);
    circle.SetFillStyle(G4Circle::filled);
    G4Colour colour(1.,0.,0.);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);
    pVVisManager->Draw(circle);
  }
}

//////////////////////////////////////////////////////////////////
/// Prints full information about the calGeantHit
void ActarSimGasGeantHit::Print(){
  G4cout << "##################################################################"
	 << G4endl
	 << "############     ActarSimGasGeantHit::Print()     ################" << G4endl
	 << "trackID: " << trackID
	 << "parentID: " << parentID
	 << ", detID: " << detID
	 << ", detName: " << detName << G4endl;
  G4cout << "edep: " << edep  / MeV << " MeV"
	 << ", stepEnergy: " << stepEnergy
	 << ", prePos: " << prePos
	 << ", postPos: " << postPos
	 << ", stepLength: " << stepLength  / mm << " mm"
	 << ", preToF: " << preToF  / ns << " ns"
	 << ", posToF: " << postToF  / ns << " ns" 	 << G4endl;
  G4cout << "##################################################################"
	 << G4endl;
}
