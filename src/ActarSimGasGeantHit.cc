/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez Pol     hapol@fpddux.usc.es
//*-- Date: 04/2006
//*-- Last Update: 04/04/06 by Hector Alvarez Pol
// --------------------------------------------------------------
// Description:
//   A Geant Hit in the calorimeter volume. It represents the
//   information of each step with energy deposited in the
//   calorimeter volume.
//
// --------------------------------------------------------------
// Comments:
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include "ActarSimGasGeantHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4Allocator<ActarSimGasGeantHit> ActarSimGasGeantHitAllocator;


ActarSimGasGeantHit::ActarSimGasGeantHit() {
  //
  // Constructor
  //
}


ActarSimGasGeantHit::~ActarSimGasGeantHit() {
  //
  // Destructor
  //
}


ActarSimGasGeantHit::ActarSimGasGeantHit(const ActarSimGasGeantHit& right) : G4VHit() {
  //
  // Copy constructor
  //
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
}


const ActarSimGasGeantHit& ActarSimGasGeantHit::operator=(const ActarSimGasGeantHit& right){
  //
  // Operator =
  //
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

  return *this;
}


G4int ActarSimGasGeantHit::operator==(const ActarSimGasGeantHit& right) const{
  //
  // Operator ==
  //
  return (this==&right) ? 1 : 0;
}


void ActarSimGasGeantHit::Draw(){
  //
  // Draws the Hit. A clear red point on the Hit position
  //
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager)
  {
    G4Circle circle(prePos);
    circle.SetScreenSize(4);
    circle.SetFillStyle(G4Circle::filled);
    G4Colour colour(1.,0.,0.);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);
    pVVisManager->Draw(circle);
  }
}


void ActarSimGasGeantHit::Print(){
  //
  // Prints full information about the calGeantHit
  //
  G4cout << "##################################################################"
	 << G4endl
	 << "############     ActarSimGasGeantHit::Print()     ################" << G4endl
	 << "trackID: " << trackID
	 << "parentID: " << parentID
	 << ", detID: " << detID
	 << ", detName: " << detName << G4endl;
  G4cout << "edep: " << edep  / MeV << " MeV"
	 << ", prePos: " << prePos
	 << ", postPos: " << postPos
	 << ", stepLength: " << stepLength  / mm << " mm"
	 << ", preToF: " << preToF  / ns << " ns"
	 << ", posToF: " << postToF  / ns << " ns" 	 << G4endl;
  G4cout << "##################################################################"
	 << G4endl;

}




