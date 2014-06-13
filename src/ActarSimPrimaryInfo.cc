/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol     hapol@fpddux.usc.es
//*-- Date: 04/2008
//*-- Last Update: 
// --------------------------------------------------------------
// Description:
//   The information from the primaries generated in the reaction
//   vertex. Information to be accessed in the ROOT file per event
//
// --------------------------------------------------------------
// Comments:
//
//
/////////////////////////////////////////////////////////////////

#include "ActarSimPrimaryInfo.hh"

#include "G4PrimaryParticle.hh"
#include "G4ThreeVector.hh"

ClassImp(ActarSimPrimaryInfo)
  
ActarSimPrimaryInfo::ActarSimPrimaryInfo() {
  //
  // Init values
  //
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

ActarSimPrimaryInfo::ActarSimPrimaryInfo(G4PrimaryParticle* prim) {
  //
  // Values from a primary
  //
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


ActarSimPrimaryInfo::~ActarSimPrimaryInfo(){
  //
  // Destructor. Makes nothing
  //
}



void ActarSimPrimaryInfo::print(void){
  //
  // Prints info
  //
  /*    G4cout << "-------------------------------------------" << G4endl;
  G4cout << "------- ActarSimPrimaryInfo::print() ---------" << G4endl;
  G4cout << " PDGcode: " << PDGcode
         << ", trackID: " << trackID
         << ", mass: " << mass
         << ", charge: " << charge / eplus << " eplus"
	 << G4endl;
  G4cout << " Px: " << Px
         << " Py: " << Py
         << " Pz: " << Pz
         << ", properTime: " << properTime / ns << " ns"
         << G4endl;
  G4cout << " polX: " << polX
         << " polY: " << polY
         << " polZ: " << polZ
         << ", Weight0: " << Weight0
         << G4endl;
  G4cout << " x0: " << x0 / mm << " mm"
         << " y0: " << y0 / mm << " mm"
         << " z0: " << z0 / mm << " mm"<< G4endl;
  G4cout << " nbPrimariesInEvent: " << nbPrimariesInEvent
         << ", kineticEnergy: " << kineticEnergy
         << ", theta: " << theta / rad << " rad"
         << ", phi: " << phi / rad << " rad"<< G4endl;
  G4cout << "-------------------------------------------" << G4endl;
  */
}


void ActarSimPrimaryInfo::SetVertexPosition(Double_t x, Double_t y, Double_t z) {
  //
  // Sets the position of the vertex (origin of the primary particle)
  //
  x0 = x;
  y0 = y;
  z0 = z;
}


void ActarSimPrimaryInfo::SetMomentum(Double_t px, Double_t py, Double_t pz) {
  //
  // Sets the momentum
  //
  Px = px;
  Py = py;
  Pz = pz;
  G4ThreeVector mom(Px,Py,Pz);
  theta = mom.theta();        
  phi = mom.phi();          

}

void ActarSimPrimaryInfo::Set4Momentum(Double_t px, Double_t py, Double_t pz, Double_t E){
  //
  // Sets the 4momentum
  //
  Px = px;
  Py = py;
  Pz = pz;
  Double_t mas2 = E*E - px*px - py*py - pz*pz;
  if(mas2>=0.)
    { mass = std::sqrt(mas2); }
  else
    { mass = -1.0; }
}

void ActarSimPrimaryInfo::SetPolarization(Double_t px,Double_t py,Double_t pz) {
  //
  // Sets the polarization
  //
  polX = px;
  polY = py;
  polZ = pz;
}
