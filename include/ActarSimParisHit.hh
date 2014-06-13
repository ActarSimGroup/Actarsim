/////////////////////////////////////////////////////////////////
//*-- AUTHOR : David Perez Loureiro     loureiro@ganil.fr
//*-- Date: 07/2013
//*-- Last Update:
// --------------------------------------------------------------
// Description:
//   A Paris hit:
//
// --------------------------------------------------------------
// Comments:
//
//
/////////////////////////////////////////////////////////////////

#ifndef ActarSimParisHit_h
#define ActarSimParisHit_h 1

#include "TROOT.h"  //for including Rtypes.h

#include "TObject.h"
#include "TString.h"

class ActarSimParisHit: public TObject{

private:

  //Basic Hit information

  Int_t detectorID;                      //Detector Identification
  Int_t motherID;                        //Mother volume (Cluster)

  Double_t xpos;                         //X position
  Double_t ypos;                         //Y position
  Double_t zpos;                         //Z position

  Double_t time;                         //time (w.r.t. vertex emission)
  Double_t energy;                       //total energy deposited

  //Event Identification

  UInt_t trackID;                        // track ID
  UInt_t eventID;                        // event ID
  UInt_t runID;                          // run ID

  UInt_t particleID;
  Double_t particleCharge;
  Double_t particleMass;

  TString process_name;
  TString detector_name;

public:
  ActarSimParisHit();
  ~ActarSimParisHit();

  //Setters

  void SetDetectorID(Int_t det){detectorID=det;}
  void SetMotherID(Int_t det){motherID=det;}
  void SetXPos(Double_t x){xpos = x;}
  void SetYPos(Double_t y){ypos = y;}
  void SetZPos(Double_t z){zpos = z;}

  void SetTime(Double_t ti){time = ti;}
  void SetEnergy(Double_t ed){energy = ed;}

  void SetTrackID(UInt_t tr){trackID = tr;}
  void SetEventID(UInt_t ev){eventID = ev;}
  void SetRunID(UInt_t run){runID = run;}

  void SetParticleID(UInt_t pdgID){particleID = pdgID;}
  void SetParticleCharge(Double_t pdgCharge){particleCharge = pdgCharge;}
  void SetParticleMass(Double_t pdgMass){particleMass = pdgMass;}

  void SetProcessName(TString name) {  process_name = name; } 
  void SetDetectorName(TString name) {  detector_name = name; } 

  //Getters
  Int_t GetDetectorID(){ return detectorID;}
  Int_t GetMotherID(){ return motherID;}
  Double_t GetXPos(){return xpos;}
  Double_t GetYPos(){return ypos;}
  Double_t GetZPos(){return zpos;}
  
  Double_t GetTime(){return time;}
  Double_t GetEnergy(){return energy;}

  UInt_t GetTrackID(){return trackID;}
  UInt_t GetEventID(){return eventID;}
  UInt_t GetRunID(){return runID;}

  UInt_t GetParticleID(){return particleID;}
  Double_t GetParticleCharge(){return particleCharge;}
  Double_t GetParticleMass(){return particleMass;}

  TString GetProcessName() {  return process_name; } 
  TString GetDetectorName() {  return detector_name; } 

  void Print(void);
  
  ClassDef(ActarSimParisHit,1) //ROOT CINT
};
#endif
