// - AUTHOR: Hector Alvarez-Pol 04/2008
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimSciRingHit_h
#define ActarSimSciRingHit_h 1

#include "TROOT.h"  //for including Rtypes.h

#include "TObject.h"

class ActarSimSciRingHit : public TObject{
private:
  UInt_t type;           ///< Scintillator type
  UInt_t copy;           ///< Scintillator copy

  Double_t time;         ///< Pulse time (w.r.t. vertex emission)
  Double_t energy;       ///< Total energy deposited

  UInt_t eventID;        ///< Event ID
  UInt_t runID;          ///< Run ID
  UInt_t trackID;        ///< Track ID

  UInt_t particleID;       ///< Particle ID
  Double_t particleCharge; ///< Particle charge
  Double_t particleMass;   ///< Particle charge

public:
  ActarSimSciRingHit();
  ~ActarSimSciRingHit();

  void SetType(UInt_t ty){type = ty;}
  void SetCopy(UInt_t co){copy = co;}

  void SetTime(Double_t ti){time = ti;}
  void SetEnergy(Double_t ed){energy = ed;}

  void SetEventID(UInt_t ev){eventID = ev;}
  void SetRunID(UInt_t run){runID = run;}
  void SetTrackID(UInt_t track){trackID = track;}

  void SetParticleID(UInt_t pdgID){particleID = pdgID;}
  void SetParticleCharge(Double_t pdgCharge){particleCharge = pdgCharge;}
  void SetParticleMass(Double_t pdgMass){particleMass = pdgMass;}

  Int_t GetType(){return type;}
  Int_t GetCopy(){return copy;}

  Double_t GetEnergy(){return energy;}
  Double_t GetTime(){return time;}

  UInt_t GetEventID(){return eventID;}
  UInt_t GetRunID(){return runID;}
  UInt_t GetTrackID(){return trackID;}

  UInt_t GetParticleID(){return particleID;}
  Double_t GetParticleCharge(){return particleCharge;}
  Double_t GetParticleMass(){return particleMass;}

  void print(void);

  ClassDef(ActarSimSciRingHit,1) //ROOT CINT
};
#endif
