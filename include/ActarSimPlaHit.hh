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

#ifndef ActarSimPlaHit_h
#define ActarSimPlaHit_h 1

#include "TROOT.h"  //for including Rtypes.h

#include "TObject.h"

class ActarSimPlaHit : public TObject{

private:

  //Crystal identification
  UInt_t type;           //crystal type
  UInt_t copy;           //crystal copy
	
	
	// Y. Ayyad We just add the same information as Silicon to identify the scintillator bar
	//Basic Hit information
	Int_t detectorID;    // Tracker model (depends on tracker geo)
	Double_t detCenterCoordinateX; // center of the present silicon, dypang 090130
	Double_t detCenterCoordinateY; // center of the present silicon, dypang 090130
	Double_t detCenterCoordinateZ; // center of the present silicon, dypang 090130
	
	//Basic Hit information
	Double_t xpos;
	Double_t ypos;
	Double_t zpos;
	
	
	

  Double_t time;    // pulse time (w.r.t. vertex emission)
  Double_t energy;  // total energy deposited
	Double_t eBeforePla; // energy before entering silicon
	Double_t eAfterPla;  // energy left after silicon


  //Event identification
  UInt_t eventID;     // event ID
  UInt_t runID;       // run ID
  UInt_t trackID;       // run ID

  UInt_t particleID;
  Double_t particleCharge;
  Double_t particleMass;
	
	
	UInt_t stepsContributing; //neccessary for iterative means

public:
  ActarSimPlaHit();
  ~ActarSimPlaHit();
	
	void SetDetectorID(Int_t det){detectorID = det;}
	
	void SetDetCenterCoordinateX(Double_t x){detCenterCoordinateX=x;} // center of the present silicon, dypang 090130
	void SetDetCenterCoordinateY(Double_t y){detCenterCoordinateY=y;} // center of the present silicon, dypang 090130
	void SetDetCenterCoordinateZ(Double_t z){detCenterCoordinateZ=z;} // center of the present silicon, dypang 090130
	
	void SetXPos(Double_t x){xpos = x;}
	void SetYPos(Double_t y){ypos = y;}
	void SetZPos(Double_t z){zpos = z;}

  void SetType(UInt_t ty){type = ty;}
  void SetCopy(UInt_t co){copy = co;}


  void SetTime(Double_t ti){time = ti;}
  void SetEnergy(Double_t ed){energy = ed;}
  void SetEBeforePla(Double_t eb){eBeforePla = eb;}
  void SetEAfterPla(Double_t ea){eAfterPla = ea;}
	

  void SetEventID(UInt_t ev){eventID = ev;}
  void SetRunID(UInt_t run){runID = run;}
  void SetTrackID(UInt_t track){trackID = track;}

  void SetParticleID(UInt_t pdgID){particleID = pdgID;}
  void SetParticleCharge(Double_t pdgCharge){particleCharge = pdgCharge;}
  void SetParticleMass(Double_t pdgMass){particleMass = pdgMass;}
	
	void SetStepsContributing(UInt_t step){stepsContributing = step;}
	
	Int_t GetDetectorID(){return detectorID;}
	
	Double_t GetDetCenterCoordinateX(){return detCenterCoordinateX;} // center of the present silicon, dypang 090130
	Double_t GetDetCenterCoordinateY(){return detCenterCoordinateY;} // center of the present silicon, dypang 090130
	Double_t GetDetCenterCoordinateZ(){return detCenterCoordinateZ;} // center of the present silicon, dypang 090130
	
	Double_t GetXPos(){return xpos;}
	Double_t GetYPos(){return ypos;}
	Double_t GetZPos(){return zpos;}
	
	

  Int_t GetType(){return type;}
  Int_t GetCopy(){return copy;}

  Double_t GetEnergy(){return energy;}
  Double_t GetTime(){return time;}
	Double_t GetEBeforeSil(){return eBeforePla;}
	Double_t GetEAfterSil(){return eAfterPla;}
	
	

  UInt_t GetEventID(){return eventID;}
  UInt_t GetRunID(){return runID;}
  UInt_t GetTrackID(){return trackID;}

  UInt_t GetParticleID(){return particleID;}
  Double_t GetParticleCharge(){return particleCharge;}
  Double_t GetParticleMass(){return particleMass;}
	
	UInt_t GetStepsContributing(){return stepsContributing;}
	
	

  void print(void);

  ClassDef(ActarSimPlaHit,1) //ROOT CINT
};
#endif
