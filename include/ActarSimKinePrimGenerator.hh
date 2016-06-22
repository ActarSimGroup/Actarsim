// - AUTHOR: M.S. Golovkov/Pang Danyang 02/2008
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimKinePrimGenerator_h
#define ActarSimKinePrimGenerator_h 1

#include "G4ThreeVector.hh"
#include "globals.hh"

class ActarSimKinePrimGenerator {
private:
  G4double m1;  ///< Mass0 of the incident particle
  G4double m2;  ///< Mass0 of the target
  G4double m3;  ///< Mass0 of the scattered particle
  G4double m4;  ///< Mass0 of the recoil
  G4double ex1; ///< Excitation energy of the incident particle
  G4double ex2; ///< Excitation energy of the target
  G4double ex3; ///< Excitation energy of the scattered particle
  G4double ex4; ///< Excitation energy of the recoil

  G4double tb;  ///< Incident energy (total Lab energy in MeV)

  G4double thetacmsInput; ///< Theta CM angle of the scattered particle, in degrees

  G4double* ANGAs;  ///< Lab angle and energy of scattered particle
  G4double* ANGAr;  ///< Lab angle and energy of recoiled particle
  G4bool    NoSolution; ///< Flag

public:
  ActarSimKinePrimGenerator();
  ~ActarSimKinePrimGenerator();

  void SetMassOfProjectile(G4double value){m1 = value;}
  void SetMassOfTarget(G4double value){m2 = value;}
  void SetMassOfScattered(G4double value){m3 = value;}
  void SetMassOfRecoiled(G4double value){m4 = value;}
  void SetExEnergyOfProjectile(G4double value){ex1 = value;}
  void SetExEnergyOfTarget(G4double value){ex2 = value;}
  void SetExEnergyOfScattered(G4double value){ex3 = value;}
  void SetExEnergyOfRecoiled(G4double value){ex4 = value;}

  void SetLabEnergy(G4double value){tb = value;}

  void SetThetaCMAngle(G4double value){thetacmsInput = value;}

  void SetANGAs(G4int place, G4double value){ANGAs[place] = value;}
  void SetANGAr(G4int place, G4double value){ANGAr[place] = value;}

  void SetNoSolution(G4bool value){NoSolution=value;}

  G4double GetMassOfProjectile(void){return m1;}
  G4double GetMassOfTarget(void){return m2;}
  G4double GetMassOfScattered(void){return m3;}
  G4double GetMassOfRecoiled(void){return m4;}
  G4double GetExEnergyOfProjectile(void){return ex1;}
  G4double GetExEnergyOfTarget(void){return ex2;}
  G4double GetExEnergyOfScattered(void){return ex3;}
  G4double GetExEnergyOfRecoiled(void){return ex4;}

  G4double GetLabEnergy(void){return tb;}

  G4double GetThetaCMAngle(void){return thetacmsInput;}

  G4double GetANGAs(G4int place){return ANGAs[place];}
  G4double GetANGAr(G4int place){return ANGAr[place];}

  G4bool GetNoSolution(void){return NoSolution;}

  void KineKinematics();

  void Dump();

  void PrintResults();
};
#endif
