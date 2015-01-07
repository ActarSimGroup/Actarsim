/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol
//*-- Date: 01/2006
//*-- Last Update: 1/12/14 by Hector Alvarez
// --------------------------------------------------------------
// Description:
//   The information from the beam parameters used for the reaction
//   vertex. Information to be accessed in the ROOT file per event
//
// --------------------------------------------------------------
// Comments:
//
//     - 07/05/07 Data structure for beam parameters
//
/////////////////////////////////////////////////////////////////

#include "ActarSimBeamInfo.hh"
#include "G4ThreeVector.hh"

ClassImp(ActarSimBeamInfo)

ActarSimBeamInfo::ActarSimBeamInfo() {
  //
  // Init values
  //
  energyEntrance = 0.;     // Energy at the entrance
  energyVertex = 0.;       // Energy at the vertex
  thetaEntrance = 0.;      // theta emission angle
  phiEntrance = 0.;         // phi emission angle
  xEntrance = 0. ;          // beam entrance position
  yEntrance = 0.;
  zEntrance = 0.;
  xVertex = 0.;            // reaction vertex position
  yVertex = 0.;
  zVertex = 0.;
  timeVertex = 0.;        // time at vertex formation
  status = 0;
}


ActarSimBeamInfo::~ActarSimBeamInfo(){
  //
  // Destructor. Makes nothing
  //
}


void ActarSimBeamInfo::print(void){
  //
  // Prints info
  //
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
  G4cout << " Status : " << status << G4endl;
  G4cout << "-------------------------------------------" << G4endl;
}


void ActarSimBeamInfo::SetPositionEntrance(Double_t x, Double_t y, Double_t z) {
  //
  // Sets the position at the entrance of the chamber
  //
  xEntrance = x;
  yEntrance = y;
  zEntrance = z;
}


void ActarSimBeamInfo::SetPositionVertex(Double_t x, Double_t y, Double_t z) {
  //
  // Sets the position of the reaction vertex
  //
  xVertex = x;
  yVertex = y;
  zVertex = z;
}
//   void SetPositionEntrance(Double_t ,Double_t ,Double_t );
//   void SetAnglesEntrance(Double_t ,Double_t );
//   void SetPositionVertex(Double_t ,Double_t ,Double_t );
//   void SetAnglesVertex(Double_t ,Double_t );

void ActarSimBeamInfo::SetAnglesEntrance(Double_t angleTheta, Double_t anglePhi){
  //
  // Sets the 4momentum
  //
  thetaEntrance = angleTheta;
  phiEntrance = anglePhi;
}

void ActarSimBeamInfo::SetAnglesVertex(Double_t angleTheta, Double_t anglePhi){
  //
  // Sets the 4momentum
  //
  thetaEntrance = angleTheta;
  phiEntrance = anglePhi;
}





