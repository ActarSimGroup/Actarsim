// - AUTHOR: Hector Alvarez-Pol 01/2006
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/
//////////////////////////////////////////////////////////////////
/// \class ActarSimBeamInfo
/// The information from the beam parameters at the entrance of
/// the gas chamber and at the reaction vertex. All data members
/// are accesible in the branch beamInfo of The_Event_Tree,
/// except status which is for internal use.
//////////////////////////////////////////////////////////////////

#include "ActarSimBeamInfo.hh"
#include "G4ThreeVector.hh"

ClassImp(ActarSimBeamInfo)

//////////////////////////////////////////////////////////////////
/// Constructor: initialize all variables
ActarSimBeamInfo::ActarSimBeamInfo() {
  energyEntrance = 0.;     // Energy at the entrance
  energyVertex = 0.;       // Energy at the vertex
  thetaEntrance = 0.;      // theta emission angle
  phiEntrance = 0.;        // phi emission angle
  thetaVertex = 0.;        // theta vertex angle
  phiVertex = 0.;          // phi vertex angle
  xEntrance = 0. ;         // beam entrance position
  yEntrance = 0.;
  zEntrance = 0.;
  xVertex = 0.;            // reaction vertex position
  yVertex = 0.;
  zVertex = 0.;
  timeVertex = 0.;         // time at vertex formation
  mass = 0.0;              // mass
  charge = 0.0;            // charge
  eventID = 0;             // eventID
  runID = 0;               // runID
  status = 0;              // status
}

//////////////////////////////////////////////////////////////////
/// Destructor (makes nothing)
ActarSimBeamInfo::~ActarSimBeamInfo(){
}

//////////////////////////////////////////////////////////////////
/// Prints the complete state of the class members
void ActarSimBeamInfo::print(void){
  G4cout << "-------------------------------------------" << G4endl;
  G4cout << "------- ActarSimBeamInfo::print() ---------" << G4endl;
  G4cout << "-------------------------------------------" << G4endl;
  G4cout << " Energy at the chamber entrance:  " << energyEntrance/MeV << " MeV" << G4endl;
  G4cout << " Energy at the reaction vertex:  " << energyVertex/MeV << " MeV" << G4endl;
  G4cout << " Parameters (emmitance) at the chamber entrance:  "  << G4endl;
  G4cout << " xEntrance: " << xEntrance / mm << " mm"
         << " yEntrance: " << yEntrance / mm << " mm"
	 << " zEntrance: " << zEntrance / mm << " mm"<< G4endl;
  G4cout << " thetaEntrance: " << thetaEntrance / rad << " rad"
	 << " phiEntrance: " << phiEntrance / rad << " rad"<< G4endl;
  G4cout << " Position at the reaction vertex:  "  << G4endl;
  G4cout << " xVertex: " << xVertex / mm << " mm"
         << " yVertex: " << yVertex / mm << " mm"
	 << " zVertex: " << zVertex / mm << " mm"<< G4endl;
  G4cout << " thetaVertex: " << thetaVertex / rad << " rad"
	 << " phiVertex: " << phiVertex / rad << " rad"<< G4endl;
  G4cout << " Time at the reaction vertex:  " << timeVertex/ns << " ns" << G4endl;
  G4cout << " Mass: " << mass
         << ", charge: " << charge / eplus << " eplus" << G4endl;
  G4cout << " Status : " << status << G4endl;
  G4cout << "-------------------------------------------" << G4endl;
}

//////////////////////////////////////////////////////////////////
/// Sets the position at entrance of the chamber
void ActarSimBeamInfo::SetPositionEntrance(Double_t x, Double_t y, Double_t z) {
  xEntrance = x;
  yEntrance = y;
  zEntrance = z;
}

//////////////////////////////////////////////////////////////////
/// Sets the position of the reaction vertex
void ActarSimBeamInfo::SetPositionVertex(Double_t x, Double_t y, Double_t z) {
  xVertex = x;
  yVertex = y;
  zVertex = z;
}

//////////////////////////////////////////////////////////////////
/// Sets the angles at entrance
void ActarSimBeamInfo::SetAnglesEntrance(Double_t angleTheta, Double_t anglePhi){
  thetaEntrance = angleTheta;
  phiEntrance = anglePhi;
}

//////////////////////////////////////////////////////////////////
/// Sets the angles at vertex
void ActarSimBeamInfo::SetAnglesVertex(Double_t angleTheta, Double_t anglePhi){
  thetaEntrance = angleTheta;
  phiEntrance = anglePhi;
}
