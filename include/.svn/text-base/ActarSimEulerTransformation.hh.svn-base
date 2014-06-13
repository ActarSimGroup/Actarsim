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

#ifndef ActarSimEulerTransformation_h
#define ActarSimEulerTransformation_h 1

#include "G4ThreeVector.hh"
#include "globals.hh"

class ActarSimEulerTransformation {
private:

// inputs: teta0,phi0, beamdirection=>(alpha,beta,gamma)
// outputs: theta_1,phi_1

  G4double ThetaInBeamSystem;
  G4double PhiInBeamSystem;
  G4double BeamDirectionAtVertexTheta;
  G4double BeamDirectionAtVertexPhi;

  G4double ThetaInLabSystem;
  G4double PhiInLabSystem;

public:
  ActarSimEulerTransformation();
  ~ActarSimEulerTransformation();

  void SetThetaInBeamSystem(G4double value){ThetaInBeamSystem = value;}
  void SetPhiInBeamSystem(G4double value){PhiInBeamSystem = value;}
  void SetBeamDirectionAtVertexTheta(G4double value){BeamDirectionAtVertexTheta = value;}
  void SetBeamDirectionAtVertexPhi(G4double value){BeamDirectionAtVertexPhi = value;}

  G4double GetThetaInBeamSystem(void){return ThetaInBeamSystem;}
  G4double GetPhiInBeamSystem(void){return PhiInBeamSystem;}
  G4double GetThetaInLabSystem(void){return ThetaInLabSystem;}
  G4double GetPhiInLabSystem(void){return PhiInLabSystem;}
  G4double GetBeamDirectionAtVertexTheta(void){return BeamDirectionAtVertexTheta;}
  G4double GetBeamDirectionAtVertexPhi(void){return BeamDirectionAtVertexPhi;}

  void DoTheEulerTransformationBeam2Lab();

  void Dump();

  void PrintResults();

};
#endif
