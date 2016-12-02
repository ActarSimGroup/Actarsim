// - AUTHOR: Hector Alvarez-Pol 04/2008
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/
//////////////////////////////////////////////////////////////////
/// \class ActarSimSilRingGeantHit
///  A Geant Hit in the Silicon volume. It represents
///  the information of each step with energy deposited.
/////////////////////////////////////////////////////////////////

#include "ActarSimSilRingGeantHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

G4Allocator<ActarSimSilRingGeantHit> ActarSimSilRingGeantHitAllocator;

//////////////////////////////////////////////////////////////////
/// Constructor. Nothing to do.
ActarSimSilRingGeantHit::ActarSimSilRingGeantHit() {
}

//////////////////////////////////////////////////////////////////
/// Destructor. Nothing to do.
ActarSimSilRingGeantHit::~ActarSimSilRingGeantHit() {
}

//////////////////////////////////////////////////////////////////
///  Copy constructor
ActarSimSilRingGeantHit::ActarSimSilRingGeantHit(const ActarSimSilRingGeantHit& right) : G4VHit() {
  edep = right.edep;
  eBeforeSil = right.eBeforeSil;
  eAfterSil = right.eAfterSil;
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
/// Operator =
const ActarSimSilRingGeantHit& ActarSimSilRingGeantHit::operator=(const ActarSimSilRingGeantHit& right){
  edep = right.edep;
  eBeforeSil = right.eBeforeSil;
  eAfterSil = right.eAfterSil;
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
/// Operator ==
G4int ActarSimSilRingGeantHit::operator==(const ActarSimSilRingGeantHit& right) const{
  return (this==&right) ? 1 : 0;
}

//////////////////////////////////////////////////////////////////
/// Draws the Hit. A clear red point on the Hit position
void ActarSimSilRingGeantHit::Draw(){
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
void ActarSimSilRingGeantHit::Print(){
  G4cout << "##################################################################"
	 << G4endl
	 << "###############     ActarSimSilRingGeantHit::Print()    ###################" << G4endl
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
