/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez Pol     hapolyo@usc.es
//*-- Date: 04/2008
//*-- Last Update:
// --------------------------------------------------------------
// Description:
//   A Scintillator hit:
//
// --------------------------------------------------------------
// Comments:
// Added information about particleCharge, particleMass, and particleID -- dypang 090305
//
/////////////////////////////////////////////////////////////////

#ifndef ActarSimSciHit_h
#define ActarSimSciHit_h 1

#include "TROOT.h"  //for including Rtypes.h

#include "TObject.h"

class ActarSimSciHit : public TObject{

private:

  //Crystal identification
  UInt_t type;           //crystal type
  UInt_t copy;           //crystal copy

  Double_t time;    // pulse time (w.r.t. vertex emission)
  Double_t energy;  // total energy deposited

  //Event identification
  UInt_t eventID;     // event ID
  UInt_t runID;       // run ID

  UInt_t particleID;
  Double_t particleCharge;
  Double_t particleMass;

public:
  ActarSimSciHit();
  ~ActarSimSciHit();

  void SetType(UInt_t ty){type = ty;}
  void SetCopy(UInt_t co){copy = co;}

  void SetTime(Double_t ti){time = ti;}
  void SetEnergy(Double_t ed){energy = ed;}

  void SetEventID(UInt_t ev){eventID = ev;}
  void SetRunID(UInt_t run){runID = run;}

  void SetParticleID(UInt_t pdgID){particleID = pdgID;}
  void SetParticleCharge(Double_t pdgCharge){particleCharge = pdgCharge;}
  void SetParticleMass(Double_t pdgMass){particleMass = pdgMass;}

  Int_t GetType(){return type;}
  Int_t GetCopy(){return copy;}

  Double_t GetEnergy(){return energy;}
  Double_t GetTime(){return time;}

  UInt_t GetEventID(){return eventID;}
  UInt_t GetRunID(){return runID;}

  UInt_t GetParticleID(){return particleID;}
  Double_t GetParticleCharge(){return particleCharge;}
  Double_t GetParticleMass(){return particleMass;}

  void print(void);

  ClassDef(ActarSimSciHit,1) //ROOT CINT
};
#endif
