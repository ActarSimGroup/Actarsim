// - AUTHOR: Hector Alvarez-Pol 04/2008
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimSilRingHit_h
#define ActarSimSilRingHit_h 1

#include "TROOT.h"  //for including Rtypes.h

#include "TObject.h"

class ActarSimSilRingHit : public TObject {
private:
  Int_t detectorID;    ///< Tracker model (depends on tracker geo)

  Double_t xpos;   ///< Hit X Position
  Double_t ypos;   ///< Hit Y Position
  Double_t zpos;   ///< Hit Z Position

  Double_t time;       ///< Pulse time (w.r.t. vertex emission)
  Double_t energy;     ///< Total energy deposited
  Double_t eBeforeSil; ///< Energy before entering silicon
  Double_t eAfterSil;  ///< Energy left after silicon

  UInt_t trackID;      ///< Track ID
  UInt_t eventID;      ///< Event ID
  UInt_t runID;        ///< Run ID

  UInt_t particleID;        ///< Particle ID
  Double_t particleCharge;  ///< Particle charge
  Double_t particleMass;    ///< Particle mass

  UInt_t stepsContributing; ///< Steps included in the hit. Neccessary for iterative means

public:
  ActarSimSilRingHit();
  ~ActarSimSilRingHit();

  void SetDetectorID(Int_t det){detectorID = det;}

  void SetXPos(Double_t x){xpos = x;}
  void SetYPos(Double_t y){ypos = y;}
  void SetZPos(Double_t z){zpos = z;}

  void SetTime(Double_t ti){time = ti;}
  void SetEnergy(Double_t ed){energy = ed;}
  void SetEBeforeSil(Double_t eb){eBeforeSil = eb;}
  void SetEAfterSil(Double_t ea){eAfterSil = ea;}

  void SetTrackID(UInt_t tr){trackID = tr;}
  void SetEventID(UInt_t ev){eventID = ev;}
  void SetRunID(UInt_t run){runID = run;}

  void SetParticleID(UInt_t pdgID){particleID = pdgID;}
  void SetParticleCharge(Double_t pdgCharge){particleCharge = pdgCharge;}
  void SetParticleMass(Double_t pdgMass){particleMass = pdgMass;}

  void SetStepsContributing(UInt_t step){stepsContributing = step;}

  Int_t GetDetectorID(){return detectorID;}

  Double_t GetXPos(){return xpos;}
  Double_t GetYPos(){return ypos;}
  Double_t GetZPos(){return zpos;}

  Double_t GetEnergy(){return energy;}
  Double_t GetTime(){return time;}
  Double_t GetEBeforeSil(){return eBeforeSil;}
  Double_t GetEAfterSil(){return eAfterSil;}

  UInt_t GetTrackID(){return trackID;}
  UInt_t GetEventID(){return eventID;}
  UInt_t GetRunID(){return runID;}

  UInt_t GetParticleID(){return particleID;}
  Double_t GetParticleCharge(){return particleCharge;}
  Double_t GetParticleMass(){return particleMass;}

  UInt_t GetStepsContributing(){return stepsContributing;}

  void print(void);

  ClassDef(ActarSimSilRingHit,1) //ROOT CINT
};
#endif
