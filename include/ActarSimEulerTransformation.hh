// - AUTHOR: M.S.  Golovkov 02/2008
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimEulerTransformation_h
#define ActarSimEulerTransformation_h 1

#include "G4ThreeVector.hh"
#include "globals.hh"

class ActarSimEulerTransformation {
private:
  G4double ThetaInBeamSystem;          ///<
  G4double PhiInBeamSystem;            ///<
  G4double BeamDirectionAtVertexTheta; ///<
  G4double BeamDirectionAtVertexPhi;   ///<

  G4double ThetaInLabSystem;           ///<
  G4double PhiInLabSystem;             ///<

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
