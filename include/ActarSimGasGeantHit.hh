// - AUTHOR: Hector Alvarez-Pol 04/2006
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimGasGeantHit_h
#define ActarSimGasGeantHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4Step.hh"

class ActarSimGasGeantHit : public G4VHit {
private:
  G4int         trackID;          ///< ID for each track
  G4int         parentID;         ///< ID for the parent track
  //G4Step        *bstep;
  G4double      edep;             ///< Energy deposited
  G4double      particleCharge;   ///< Charge of the particle
  G4double      particleMass;     ///< Mass of the particle
  G4int         particleID;       ///< Particle ID according to the GDP-coding
  G4ThreeVector postPos;          ///< Position after step
  G4ThreeVector prePos;           ///< Position before step
  G4String      detName;          ///< Detector name where energy is deposited
  G4int         detID;            ///< Detector ID
  G4double      preToF;           ///< Time before step
  G4double      postToF;          ///< Time after step
  G4double      stepLength;       ///< Length of the step
  G4double      stepEnergy;       ///< Particle energy before step

public:
  ActarSimGasGeantHit();
  ~ActarSimGasGeantHit();
  ActarSimGasGeantHit(const ActarSimGasGeantHit&);
  const ActarSimGasGeantHit& operator=(const ActarSimGasGeantHit&);
  G4int operator==(const ActarSimGasGeantHit&) const;

  inline void* operator new(size_t);
  inline void  operator delete(void*);

  void Draw();
  void Print();
  void PrinttoFile();

  void SetTrackID(G4int track){ trackID = track; }
  void SetParentID(G4int track){ parentID = track; }
  //void SetStep(G4Step* astep){bstep=astep;}
  void SetEdep(G4double de){ edep = de; }
  void SetParticleCharge(G4double pc){particleCharge=pc;}
  void SetParticleMass(G4double pm){particleMass=pm;}
  void SetParticleID(G4int pi){particleID=pi;}
  void SetPrePos(G4ThreeVector xyz){ prePos = xyz; }
  void SetPostPos(G4ThreeVector xyz){ postPos = xyz; }
  void SetPreToF(G4double Time){ preToF = Time; }
  void SetPostToF(G4double Time){ postToF = Time; }
  void SetDetName(G4String Name){ detName = Name; }
  void SetDetID(G4int id){ detID = id; }
  void SetStepLength(G4double len){ stepLength = len; }
  void SetStepEnergy(G4double sten){ stepEnergy = sten; }

  G4int         GetTrackID(){ return trackID; }
  G4int         GetParentID(){ return parentID; }
  //G4Step        *GetStep(){return bstep;}
  G4double      GetEdep(){ return edep; }
  G4double      GetParticleCharge(){return particleCharge;}
  G4double      GetParticleMass(){return particleMass;}
  G4int         GetParticleID(){return particleID;}
  G4ThreeVector GetPrePos(){ return prePos; }
  G4ThreeVector GetPostPos(){ return postPos; }
  G4String      GetDetName(){ return detName; }
  G4int         GetDetID(){ return detID; }
  G4double      GetPreToF(){ return preToF; }
  G4double      GetPostToF(){ return postToF; }
  G4double      GetStepLength(){ return stepLength; }
  G4double      GetStepEnergy(){ return stepEnergy; }
};

typedef G4THitsCollection<ActarSimGasGeantHit> ActarSimGasGeantHitsCollection;

extern G4Allocator<ActarSimGasGeantHit> ActarSimGasGeantHitAllocator;

inline void* ActarSimGasGeantHit::operator new(size_t) {
  void *aHit;
  aHit = (void *) ActarSimGasGeantHitAllocator.MallocSingle();
  return aHit;
}

inline void ActarSimGasGeantHit::operator delete(void *aHit) {
  ActarSimGasGeantHitAllocator.FreeSingle((ActarSimGasGeantHit*) aHit);
}
#endif
