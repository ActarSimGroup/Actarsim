// - AUTHOR: Hector Alvarez-Pol 04/2008
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/
//////////////////////////////////////////////////////////////////
/// \class ActarSimPrimaryInfo
/// The information from the primaries generated in the reaction
/// vertex. Information to be accessed in the ROOT file per event
/////////////////////////////////////////////////////////////////

#include "ActarSimPrimaryInfo.hh"

#include "G4PrimaryParticle.hh"
#include "G4ThreeVector.hh"

ClassImp(ActarSimPrimaryInfo)

//////////////////////////////////////////////////////////////////
/// Constructor
ActarSimPrimaryInfo::ActarSimPrimaryInfo() {
  nbPrimariesInEvent = 0;
  kineticEnergy = 0;
  theta = 0;
  phi =0;
  PDGcode = 0;
  Px = 0;
  Py = 0;
  Pz = 0;
  trackID = 0;
  mass = 0;
  charge = 0;
  polX = 0;
  polY = 0;
  polZ = 0;
  Weight0 = 0;
  properTime = 0;
  x0 = 0.;
  y0 = 0.;
  z0 = 0.;
  eventID = 0;
  runID = 0;
}

//////////////////////////////////////////////////////////////////
/// Constructor from a primary
ActarSimPrimaryInfo::ActarSimPrimaryInfo(G4PrimaryParticle* prim) {
  theta = prim->GetMomentum().theta() / rad;
  phi = prim->GetMomentum().phi() / rad;

  PDGcode = prim->GetPDGcode();
  Px = prim->GetPx();
  Py = prim->GetPy();
  Pz = prim->GetPz();
  trackID = prim->GetTrackID();
  mass = prim->GetMass();
  charge = prim->GetCharge() / eplus;
  polX = prim->GetPolX();
  polY = prim->GetPolY();
  polZ = prim->GetPolZ();
  Weight0 = prim->GetWeight();
  properTime = prim->GetProperTime() / ns;

  //calculating the kinetic energy
  Double_t energysquared = (prim->GetMass()*prim->GetMass()) +
    (prim->GetPx()*prim->GetPx()+
     prim->GetPy()*prim->GetPy()+
     prim->GetPz()*prim->GetPz());
  kineticEnergy = sqrt(energysquared) - prim->GetMass();
}

//////////////////////////////////////////////////////////////////
/// Destructor. Makes nothing
ActarSimPrimaryInfo::~ActarSimPrimaryInfo(){
}

//////////////////////////////////////////////////////////////////
/// Prints info
void ActarSimPrimaryInfo::print(void){
  G4cout << "-------------------------------------------" << G4endl;
  G4cout << "------- ActarSimPrimaryInfo::print() ---------" << G4endl;
  G4cout << " eventID: " << eventID <<  ", runID: " << runID	 << G4endl;
  G4cout << " PDGcode: " << PDGcode
         << ", trackID: " << trackID
         << ", mass: " << mass
         << ", charge: " << charge / eplus << " eplus" << G4endl;
  G4cout << " Px: " << Px
         << " Py: " << Py
         << " Pz: " << Pz
         << ", properTime: " << properTime / ns << " ns" << G4endl;
  G4cout << " polX: " << polX
         << " polY: " << polY
         << " polZ: " << polZ
         << ", Weight0: " << Weight0 << G4endl;
  G4cout << " x0: " << x0 / mm << " mm"
         << " y0: " << y0 / mm << " mm"
         << " z0: " << z0 / mm << " mm" << G4endl;
  G4cout << " nbPrimariesInEvent: " << nbPrimariesInEvent
         << ", kineticEnergy: " << kineticEnergy
         << ", theta: " << theta / rad << " rad"
         << ", phi: " << phi / rad << " rad" << G4endl;
  G4cout << "-------------------------------------------" << G4endl;
}

//////////////////////////////////////////////////////////////////
/// Sets the position of the vertex (origin of the primary particle)
void ActarSimPrimaryInfo::SetVertexPosition(Double_t x, Double_t y, Double_t z) {
  x0 = x;
  y0 = y;
  z0 = z;
}

//////////////////////////////////////////////////////////////////
/// Sets the momentum
void ActarSimPrimaryInfo::SetMomentum(Double_t px, Double_t py, Double_t pz) {
  Px = px;
  Py = py;
  Pz = pz;
  G4ThreeVector mom(Px,Py,Pz);
  theta = mom.theta();
  phi = mom.phi();
}

//////////////////////////////////////////////////////////////////
/// Sets the 4momentum
void ActarSimPrimaryInfo::Set4Momentum(Double_t px, Double_t py, Double_t pz, Double_t E){
  Px = px;
  Py = py;
  Pz = pz;
  Double_t mas2 = E*E - px*px - py*py - pz*pz;
  if(mas2>=0.) {
    mass = std::sqrt(mas2);
  }
  else {
    mass = -1.0;
  }
}

//////////////////////////////////////////////////////////////////
/// Sets the polarization
void ActarSimPrimaryInfo::SetPolarization(Double_t px,Double_t py,Double_t pz) {
  polX = px;
  polY = py;
  polZ = pz;
}
