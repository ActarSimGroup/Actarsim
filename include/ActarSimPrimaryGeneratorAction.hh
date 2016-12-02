// - AUTHOR: Hector Alvarez-Pol 11/2004
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimPrimaryGeneratorAction_h
#define ActarSimPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4Ions.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleMomentum.hh"
#include "globals.hh"

#include "ActarSimEventGenerator.hh"

class G4Event;
class ActarSimPrimaryGeneratorMessenger;
class ActarSimDetectorConstruction;
class ActarSimGasDetectorConstruction;

class ActarSimPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
private:
  G4ParticleGun* particleGun;                       ///< Pointer to G4particleGun object initialized in constructor
  G4ParticleTable* particleTable;                   ///< Pointer to the G4ParticleTable
  G4IonTable* ionTable;                             ///< Pointer to the G4IonTable
  ActarSimPrimaryGeneratorMessenger* gunMessenger;  ///< Pointer to messenger
  //ActarSimEventGenerator *pReadEvGen;

  ActarSimGasDetectorConstruction* gasDetector;    ///< Pointer to gas detector constructor, to get some geometrical info

  G4Ions* incidentIon;          ///< Pointer to incident ion
  G4Ions* targetIon;            ///< Pointer to target ion
  G4Ions* scatteredIon;         ///< Pointer to scattered ion
  G4Ions* recoilIon;            ///< Pointer to recoil ion
  G4double incidentIonCharge;   ///< Charge of incident ion
  G4double targetIonCharge;     ///< Charge of target ion
  G4double scatteredIonCharge;  ///< Charge of scattered ion
  G4double recoilIonCharge;     ///< Charge of recoil ion
  G4double reactionQ;           ///< Reaction Q
  G4double labEnergy;           ///< Laboratory energy
  G4double incidentEnergy;      ///< Total incident ion energy
  G4double thetaLabAngle;       ///< Polar angle in the laboratory system

  G4String randomVertexZPositionFlag; ///< Flag to control the (random) Z position of the vertex
  G4double randomVertexZPositionMin;  ///< Minimum value for the (random) Z position of the vertex
  G4double randomVertexZPositionMax;  ///< Maximum value for the (random) Z position of the vertex
  G4double vertexZPosition;           ///< Value for the (random) Z position of the vertex

  //beam parameters
  G4double emittance;            ///< Beam emittance
  G4double beamRadiusAtEntrance; ///< Beam radius at the entrance point

  G4ThreeVector beamPosition;               ///< Beam position at the entrance
  G4ParticleMomentum beamMomentumDirection; ///< Beam (momentum) direction
  G4bool  beamDirectionFlag;                ///< Flag for a beam direction defined by angles (0) or vector (1)

  G4double lengthParameter;       ///< Parameter coming from the geometry selection
  G4double  randomThetaMin;       ///< Minimum random theta angle in CINE
  G4double  randomThetaMax;       ///< Maximum random theta angle in CINE

  G4double  randomPhiMin;         ///< Minimum random phi angle in CINE
  G4double  randomPhiMax;         ///< Maximum random phi angle in CINE

  G4String  beamInteractionFlag;          ///< Flag for beam interaction mode
  G4String  realisticBeamFlag;            ///< Flag for realistic beam interaction
  G4String  reactionFromEvGenFlag;        ///< Flag for a reaction taken from the tabulated Ev Generator
  G4String  reactionFromCrossSectionFlag; ///< Flag for a reaction taken from the Ev Generator+CINE
  G4String  reactionFromFileFlag;         ///< Flag for a reaction taken from a file
  G4String  reactionFromCineFlag;         ///< Flag for a reaction calculated using Cine
  G4String  randomThetaFlag;              ///< Flag for a random theta angle in CINE
  G4String  randomPhiFlag;                ///< Flag for a random phi angle in CINE
  G4String  alphaSourceFlag;              ///< Flag for a alpha source emitter
  G4String  reactionFile;                 ///< File definition for a reaction

  G4String  reactionFromKineFlag;  ///< Flag for using KINE
  G4double  thetaCMAngle;          ///< Center of mass polar angle
  G4double  userThetaAngle;        ///< User theta angle
  G4double  userPhiAngle;          ///< User phi angle
  G4double  massOfProjectile;      ///< Mass of the projectile ion
  G4double  massOfTarget;          ///< Mass of target ion
  G4double  massOfScattered;       ///< Mass of the scattered ion
  G4double  massOfRecoiled;        ///< Mass of the recoil ion
  G4double  exEnergyOfProjectile;  ///< Energy of the projectile ion
  G4double  exEnergyOfTarget;      ///< Energy of the target ion
  G4double  exEnergyOfScattered;   ///< Energy of the scattered ion
  G4double  exEnergyOfRecoiled;    ///< Energy of the recoil ion
  G4ThreeVector vertexPosition;    ///< Position of the vertex
  G4String  randomPhiAngleFlag;    ///< Flag for a random phi angle

public:
  ActarSimPrimaryGeneratorAction();
  ~ActarSimPrimaryGeneratorAction();

  void GeneratePrimaries(G4Event* anEvent);

  void SetReactionFromCineFlag(G4String val) { reactionFromCineFlag = val;}
  void SetIncidentIon(G4Ions* aIonDef) { incidentIon = aIonDef;}
  void SetTargetIon(G4Ions* aIonDef) { targetIon = aIonDef;}
  void SetScatteredIon(G4Ions* aIonDef) { scatteredIon = aIonDef;}
  void SetRecoilIon(G4Ions* aIonDef) { recoilIon = aIonDef;}
  void SetIncidentIonCharge(G4double aCharge) { incidentIonCharge = aCharge; }
  void SetTargetIonCharge(G4double aCharge) { targetIonCharge = aCharge; }
  void SetScatteredIonCharge(G4double aCharge) { scatteredIonCharge = aCharge; }
  void SetRecoilIonCharge(G4double aCharge) { recoilIonCharge = aCharge; }
  //void SetIncidentIonExcEnergy(G4double aExcEnergy) { incidentIonExcEnergy = aExcEnergy; }
  //void SetTargetIonExcEnergy(G4double aExcEnergy) { targetIonExcEnergy = aExcEnergy; }
  //void SetScatteredIonExcEnergy(G4double aExcEnergy) { scatteredIonExcEnergy = aExcEnergy; }
  //void SetRecoilIonExcEnergy(G4double aExcEnergy) { recoilIonExcEnergy = aExcEnergy; }

  // corresponding Kine part
  void SetReactionFromKineFlag(G4String val) { reactionFromKineFlag = val;}

  void SetThetaCMAngle(G4double val){thetaCMAngle=val; beamDirectionFlag=0;}
  void SetUserThetaAngle(G4double val){userThetaAngle=val; beamDirectionFlag=0;}
  void SetUserPhiAngle(G4double val){userPhiAngle=val; beamDirectionFlag=0;}

  G4double GetThetaCMAngle(){return thetaCMAngle;}
  G4double GetUserThetaAngle(){return userThetaAngle;}
  G4double GetUserPhiAngle(){return userPhiAngle;}

  void SetMassOfProjectile(G4double val){massOfProjectile=val;}
  void SetMassOfTarget(G4double val){massOfTarget=val;}
  void SetMassOfScattered(G4double val){massOfScattered=val;}
  void SetMassOfRecoiled(G4double val){massOfRecoiled=val;}

  G4double GetMassOfProjectile(){return massOfProjectile;}
  G4double GetMassOfTarget(){return massOfTarget;}
  G4double GetMassOfScattered(){return massOfScattered;}
  G4double GetMassOfRecoiled(){return massOfRecoiled;}

  void SetExEnergyOfProjectile(G4double val){exEnergyOfProjectile=val;}
  void SetExEnergyOfTarget(G4double val)    {exEnergyOfTarget=val;}
  void SetExEnergyOfScattered(G4double val) {exEnergyOfScattered=val;}
  void SetExEnergyOfRecoiled(G4double val)  {exEnergyOfRecoiled=val;}

  G4double GetExEnergyOfProjectile(){return exEnergyOfProjectile;}
  G4double GetExEnergyOfTarget()    {return exEnergyOfTarget;}
  G4double GetExEnergyOfScattered() {return exEnergyOfScattered;}
  G4double GetExEnergyOfRecoiled()  {return exEnergyOfRecoiled;}

  void     SetRandomVertexZPositionFlag(G4String val) {randomVertexZPositionFlag=val;}
  G4String GetRandomVertexZPositionFlag() {return randomVertexZPositionFlag;}
  void SetVertexPosition(G4ThreeVector apos) {vertexPosition=apos;}
  G4ThreeVector GetVertexPosition(){return vertexPosition;}

  void SetRandomPhiAngleFlag(G4String val) { randomPhiAngleFlag = val;}
  // end of corresponding Kine part

  void SetReactionQ(G4double val) { reactionQ = val;}

  void SetLabEnergy(G4double val) { labEnergy = val;}

  void SetIncidentEnergy(G4double val) { incidentEnergy = val;}

  void SetThetaLabAngle(G4double val) { thetaLabAngle = val;}
  void SetVertexZPosition(G4double val) { vertexZPosition = val;} // vertex Z position, dypang 080704

  void SetRandomThetaVal(G4double min, G4double max) {
    randomThetaMin = min;
    randomThetaMax = max;
  }

  void SetRandomPhiVal(G4double min, G4double max) {
    randomPhiMin = min;
    randomPhiMax = max;
  }

  void SetRandomVertexZPositionVal(G4double min, G4double max) {
    randomVertexZPositionMin = min;
    randomVertexZPositionMax = max;
  }

  void SetBeamInteractionFlag(G4String val) { beamInteractionFlag = val;}

  void SetRealisticBeamFlag(G4String val) { realisticBeamFlag = val;}
  void SetReactionFromFileFlag(G4String val) { reactionFromFileFlag = val;}

  void SetReactionFromEvGenFlag(G4String val) { reactionFromEvGenFlag = val;}
  void SetReactionFromCrossSectionFlag(G4String val) { reactionFromCrossSectionFlag = val;}

  void SetRandomThetaFlag(G4String val) { randomThetaFlag = val;}
  void SetRandomPhiFlag(G4String val) { randomPhiFlag = val;}
  void SetAlphaSourceFlag(G4String val) { alphaSourceFlag = val;}
  void SetReactionFile(G4String val) { reactionFile = val;}

  //virtual void SetInitialValues();

  void SetEmittance(G4double val){emittance = val;}
  void SetBeamRadiusAtEntrance(G4double val){beamRadiusAtEntrance = val;}
  G4double GetEmittance(){return emittance;}
  G4double GetBeamRadiusAtEntrance(){return beamRadiusAtEntrance;}

  void SetParticleDefinition(G4ParticleDefinition * aParticleDefinition)
    { particleGun->SetParticleDefinition(aParticleDefinition);}
  inline void SetParticleMomentum(G4ParticleMomentum aMomentum)
    { particleGun->SetParticleMomentum(aMomentum);}
  inline void SetParticleMomentumDirection(G4ParticleMomentum aMomentumDirection)
    { particleGun->SetParticleMomentumDirection(aMomentumDirection); }
  inline void SetParticleEnergy(G4double aKineticEnergy)
    { particleGun->SetParticleEnergy(aKineticEnergy); incidentEnergy = aKineticEnergy;}
  inline void SetParticleCharge(G4double aCharge)
    { particleGun->SetParticleCharge(aCharge); }
  inline void SetParticlePolarization(G4ThreeVector aVal)
    { particleGun->SetParticlePolarization(aVal); }
  inline void SetParticlePosition(G4ThreeVector aPos)
    { particleGun->SetParticlePosition(aPos); }
  inline void SetParticleTime(G4double aTime)
    { particleGun->SetParticleTime(aTime); }
  inline void SetNumberOfParticles(G4int i)
    { particleGun->SetNumberOfParticles(i); }

  void SetBeamMomentumDirection(G4ParticleMomentum aMomentumDirection)
    { beamMomentumDirection=aMomentumDirection; beamDirectionFlag=1;}
  void SetBeamPosition(G4ThreeVector aPos)
    { beamPosition=aPos;}

  inline G4ParticleDefinition* GetParticleDefinition()
    { return particleGun->GetParticleDefinition(); }
  inline G4ParticleMomentum GetParticleMomentumDirection()
    { return particleGun->GetParticleMomentumDirection(); }
  inline G4double GetParticleEnergy()
    { return particleGun->GetParticleEnergy(); }
  inline G4double GetParticleCharge()
    { return particleGun->GetParticleCharge(); }
  inline G4ThreeVector GetParticlePolarization()
    { return particleGun->GetParticlePolarization(); }
  inline G4ThreeVector GetParticlePosition()
    { return particleGun->GetParticlePosition(); }
  inline G4double GetParticleTime()
    { return particleGun->GetParticleTime(); }
  inline G4int GetNumberOfParticles()
    { return particleGun->GetNumberOfParticles(); }

  G4double GetRandomThetaMin() {return randomThetaMin;}
  G4double GetRandomThetaMax() {return randomThetaMax;}

  G4Ions* GetIncidentIon(){return incidentIon;}
  G4Ions* GetTargetIon(){return targetIon;}
  G4Ions* GetScatteredIon(){return scatteredIon;}
  G4Ions* GetRecoilIon(){return recoilIon;}
  G4double GetIncidentIonCharge(){return incidentIonCharge;}
  G4double GetTargetIonCharge(){return targetIonCharge;}
  G4double GetScatteredIonCharge(){return scatteredIonCharge;}
  G4double GetRecoilIonCharge(){return recoilIonCharge;}
  //G4double GetIncidentIonExcEnergy(){return incidentIonExcEnergy;}
  //G4double GetTargetIonExcEnergy(){return targetIonExcEnergy;}
  //G4double GetScatteredIonExcEnergy(){return scatteredIonExcEnergy;}
  //G4double GetRecoilIonExcEnergy(){return recoilIonExcEnergy;}
  G4double GetReactionQ(){return reactionQ;}
  G4double GetLabEnergy(){return labEnergy;}
  G4double GetIncidentEnergy(){return incidentEnergy;}
  G4double GetThetaLabAngle(){return thetaLabAngle;}
  G4double GetVertexZPosition(){return vertexZPosition;}
};
#endif
