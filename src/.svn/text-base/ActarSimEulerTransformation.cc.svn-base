/////////////////////////////////////////////////////////////////
// Euler transformation, converted from Fortran subroutine
// author: M.S. Golovkov
// date: 02/18/08
// --------------------------------------------------------------
// Description:
//   Euler transformation between beam system and lab system
// --------------------------------------------------------------
// Comments:
//   - 02/18/08 translation from the original FORTRAN
//      to Geant4-like C++
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include "ActarSimEulerTransformation.hh"

#include "globals.hh"
#include "math.h"

using namespace std;

ActarSimEulerTransformation::ActarSimEulerTransformation(){
  //
  // Constructor
  //

  // ***  Transformation  coordinate system (Euler angles) :  ***
  // DEFINITIONS:
  //  --from beam to lab system
  //     alpha = phi
  //     beta  = theta
  //     gamma = 0
  // *** inputs: theta_beam, phi_beam, alpha, beta, gamma
  // *** calculate: theta_lab, phi_lab (from beam to lab)
  //
  //  --or from lab to beam system
  //     alpha = pi
  //     beta  = theta_beam
  //     gamma = pi - phi_beam

  ThetaInBeamSystem= 0.0;
  PhiInBeamSystem  = 0.0;
  BeamDirectionAtVertexTheta=0.0;
  BeamDirectionAtVertexPhi=0.0;
  ThetaInLabSystem = 0.0;
  PhiInLabSystem   = 0.0;
}

ActarSimEulerTransformation::~ActarSimEulerTransformation(){
  // destructor
}


void ActarSimEulerTransformation::DoTheEulerTransformationBeam2Lab(){

  // ***  Transformation  coordinate system (Euler angles) :  ***
  // DEFINITIONS:
  //  --from beam to lab system
  //     alpha = phi
  //     beta  = theta
  //     gamma = 0
  // *** inputs: theta_beam, phi_beam, alpha, beta, gamma
  // *** calculate: theta_lab, phi_lab (from beam to lab)
  //
  //  --or from lab to beam system
  //     alpha = pi
  //     beta  = theta_beam
  //     gamma = pi - phi_beam

  // BE CAREFUL of the units! Here the angles are not in degrees!

  const G4double PI=3.14159265358979323846;
  //         const G4double rad=180.0/PI;

  G4double Theta0 = ThetaInBeamSystem;
  G4double Phi0   = PhiInBeamSystem;

  G4double alpha = BeamDirectionAtVertexPhi;      // alpha=phi
  G4double beta  = BeamDirectionAtVertexTheta;    // beta =theta
  G4double gamma = 0.0;

  G4double px = sin(Theta0)*cos(Phi0);
  G4double py = sin(Theta0)*sin(Phi0);
  G4double pz = cos(Theta0);

  G4double sa = sin(alpha);
  G4double ca = cos(alpha);
  G4double sb = sin(beta);
  G4double cb = cos(beta);
  G4double sg = sin(gamma);
  G4double cg = cos(gamma);

  G4double px_1 = px*(ca*cb*cg-sa*sg)+py*(-ca*cb*sg-sa*cg)+pz*(ca*sb);
  G4double py_1 = px*(sa*cb*cg+ca*sg)+py*(-sa*cb*sg+ca*cg)+pz*(sa*sb);
  G4double pz_1 = px*(-sb*cg) + py*(sb*sg) + pz*(cb);

  G4double pxy_1 = sqrt(px_1*px_1 + py_1*py_1);
  G4double sithe1 = pxy_1;
  G4double cothe1 = pz_1;

  G4double theta_1 = atan2(sithe1,cothe1);

  ThetaInLabSystem = theta_1;

  G4double phi_1=0.0;

  if(sithe1 != 0.0){
    G4double siphi1 = py_1/pxy_1;
    G4double cophi1 = px_1/pxy_1;
    phi_1 = atan2(siphi1,cophi1);

    if(phi_1 < 0.0){
      phi_1 = 2.*PI + phi_1;
    }
  }

  if(sithe1 == 0.0){
      phi_1 = 0.0;
  }

  PhiInLabSystem = phi_1;
}

void ActarSimEulerTransformation::Dump(){

}

void ActarSimEulerTransformation::PrintResults(){
  G4cout << "Beam direction in the Lab frame:" << G4endl
         << "  theta=" << BeamDirectionAtVertexTheta << ", phi=" << BeamDirectionAtVertexPhi << G4endl;
  G4cout << "Particle direction in the beam system:" << G4endl
         << "  theta=" << ThetaInBeamSystem << ", phi=" << PhiInBeamSystem << G4endl;
  G4cout << "Particle direction in the Lab system:" << G4endl
         << "  theta=" << ThetaInLabSystem << ", phi=" << PhiInLabSystem << G4endl;
}

