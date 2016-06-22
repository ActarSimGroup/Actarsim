// - AUTHOR: Hector Alvarez-Pol 04/2008
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/
//////////////////////////////////////////////////////////////////
/// \class ActarSimSciRingGeantHit
/// A Geant Hit in the Scintillator volume. It represents
/// the information of each step with energy deposited.
/////////////////////////////////////////////////////////////////

#include "ActarSimSciRingGeantHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4Allocator<ActarSimSciRingGeantHit> ActarSimSciRingGeantHitAllocator;

//////////////////////////////////////////////////////////////////
/// Constructor. Makes nothing.
ActarSimSciRingGeantHit::ActarSimSciRingGeantHit() {
}

//////////////////////////////////////////////////////////////////
/// Destructor. Makes nothing.
ActarSimSciRingGeantHit::~ActarSimSciRingGeantHit() {
}

//////////////////////////////////////////////////////////////////
///  Copy constructor
ActarSimSciRingGeantHit::ActarSimSciRingGeantHit(const ActarSimSciRingGeantHit& right) : G4VHit() {
  edep = right.edep;
  pos = right.pos;
  prePos = right.prePos;
  localPos = right.localPos;
  localPrePos = right.localPrePos;
  detName = right.detName;
  postDetName = right.postDetName;
  preDetName = right.preDetName;
  detID = right.detID;
  toF = right.toF;
  trackID = right.trackID;
  parentID = right.parentID;
  particleID = right.particleID;
  particleCharge = right.particleCharge;
  particleMass = right.particleMass;
}

//////////////////////////////////////////////////////////////////
///  Operator =
const ActarSimSciRingGeantHit& ActarSimSciRingGeantHit::operator=(const ActarSimSciRingGeantHit& right){
  edep = right.edep;
  pos = right.pos;
  prePos = right.prePos;
  localPos = right.localPos;
  localPrePos = right.localPrePos;
  detName = right.detName;
  postDetName = right.postDetName;
  preDetName = right.preDetName;
  detID = right.detID;
  toF = right.toF;
  trackID = right.trackID;
  parentID = right.parentID;
  particleID = right.particleID;
  particleCharge = right.particleCharge;
  particleMass = right.particleMass;

  return *this;
}

//////////////////////////////////////////////////////////////////
///  Operator ==
G4int ActarSimSciRingGeantHit::operator==(const ActarSimSciRingGeantHit& right) const{
  return (this==&right) ? 1 : 0;
}

//////////////////////////////////////////////////////////////////
///  Draws the Hit. A clear red point on the Hit position
void ActarSimSciRingGeantHit::Draw(){
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager) {
    G4Circle circle(pos);
    circle.SetScreenSize(4);
    circle.SetFillStyle(G4Circle::filled);
    G4Colour colour(1.,0.,0.);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);
    pVVisManager->Draw(circle);
  }
}

//////////////////////////////////////////////////////////////////
///  Prints full information about the calGeantHit
void ActarSimSciRingGeantHit::Print(){
  G4cout << "##################################################################"
	 << G4endl
	 << "###############     ActarSimSciRingGeantHit::Print()    ###################" << G4endl
	 << "trackID: " << trackID
	 << ", parentID: " << parentID
	 << ", particleID: " << particleID
	 << ", particleCharge: " << particleCharge << G4endl;
  G4cout << "detID: " << detID
	 << ", detName: " << detName
	 << ", postDetName: " << postDetName
	 << ", preDetName: " << preDetName
	 << G4endl;
  G4cout << "edep: " << edep  / MeV << " MeV"
	 << ", pos: " << pos << " mm" << G4endl
	 << ", prePos: " << prePos << " mm" << G4endl;
  G4cout << "toF: " << toF  / ns << " ns" << ", localPos: " << localPos << " mm"
	 << ", localPrePos: " << localPrePos << " mm" << G4endl;
  G4cout << "##################################################################"
	 << G4endl;
}
