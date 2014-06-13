/////////////////////////////////////////////////////////////////
//*-- AUTHOR (translation to C++): Hector Alvarez-Pol  hapol@fpddux.usc.es
//*-- AUTHOR:  Wolfgang Mittig
//*-- Date: 11/2005
//*-- Last Update: 02/11/05
// --------------------------------------------------------------
// Description:
//   PROGRAM CINE CINEMATIQUE RELATIVISTIQUE
//
// --------------------------------------------------------------
// Comments:
//   - 02/11/05 translation from the original FORTRAN 
//      to Geant4-like C++
// 
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#ifndef ActarSimCinePrimGenerator_h
#define ActarSimCinePrimGenerator_h 1

#include "G4ThreeVector.hh"
#include "globals.hh"

class ActarSimCinePrimGenerator {
private:

  G4double S1; //mass of the incident particle
  G4double S2; //mass of the target
  G4double S3; //mass of the scattered particle
  G4double S4; //mass of the recoil

  G4double QM; // Reaction Q

  G4double TH; // Theta Lab angle

  G4double EI; // LAB energy (total Lab energy in MeV)
  G4double EN; // Target excitation energy (positive)
  G4double ENI; // Projectile excitation energy (positive)

  G4double* ANGAV; // First solution vector
  G4double* ANGAR; // Second solution vector

public:
  ActarSimCinePrimGenerator();
  ~ActarSimCinePrimGenerator();

  void SetS1(G4double value){S1 = value;}
  void SetS2(G4double value){S2 = value;}
  void SetS3(G4double value){S3 = value;}
  void SetS4(G4double value){S4 = value;}
  void SetQM(G4double value){QM = value;}
  void SetEI(G4double value){EI = value;}
  void SetEN(G4double value){EN = value;}
  void SetENI(G4double value){ENI = value;}
  void SetANGAV(G4int place, G4double value){ANGAV[place] = value;}
  void SetANGAR(G4int place, G4double value){ANGAR[place] = value;}
  void SetIncidentMass(G4double value){S1 = value;}
  void SetTargetMass(G4double value){S2 = value;}
  void SetScatteredMass(G4double value){S3 = value;}
  void SetRecoilMass(G4double value){S4 = value;}
  void SetReactionQ(G4double value){QM = value;}
  void SetLabEnergy(G4double value){EI = value;}
  void SetTargetExcitationEnergy(G4double value){EN = value;}
  void SetProjectileExcitationEnergy(G4double value){ENI = value;}
  void SetThetaLabAngle(G4double value){TH = value;}

  G4double GetS1(void){return S1;}
  G4double GetS2(void){return S2;}
  G4double GetS3(void){return S3;}
  G4double GetS4(void){return S4;}
  G4double GetQM(void){return QM;}
  G4double GetEI(void){return EI;}
  G4double GetEN(void){return EN;}
  G4double GetENI(void){return ENI;}
  G4double GetANGAV(G4int place){return ANGAV[place];}
  G4double GetANGAR(G4int place){return ANGAR[place];}
  G4double GetThetaLabAngle(void){return TH;}

  void RelativisticKinematics();  

  void Dump();  

  void printResults(G4int sel);  

};

#endif
