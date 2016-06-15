/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez Pol
//*-- Date: 04/2008
//*-- Last Update: 07/01/15
// --------------------------------------------------------------
// Description:
//   A Silicon hit:
//
// --------------------------------------------------------------
// Comments:
//
//
/////////////////////////////////////////////////////////////////

#ifndef ActarSimSilRingHit_h
#define ActarSimSilRingHit_h 1

#include "TROOT.h"  //for including Rtypes.h

#include "TObject.h"

class ActarSimSilRingHit : public TObject{

private:

  //Basic Hit information
  Int_t detectorID;    // Tracker model (depends on tracker geo)

  //Basic Hit information
  Double_t xpos;
  Double_t ypos;
  Double_t zpos;

  Double_t time;    // pulse time (w.r.t. vertex emission)
  Double_t energy;  // total energy deposited
  Double_t eBeforeSil; // energy before entering silicon
  Double_t eAfterSil;  // energy left after silicon

  //Event identification
  UInt_t trackID;     // track ID
  UInt_t eventID;     // event ID
  UInt_t runID;       // run ID

  UInt_t particleID;
  Double_t particleCharge;
  Double_t particleMass;

  UInt_t stepsContributing; //neccessary for iterative means

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
