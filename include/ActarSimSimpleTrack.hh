/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol hapol@fpddux.usc.es
//*-- Date: 05/2006
//*-- Last Update: 13/06/06
// --------------------------------------------------------------
// Description:
//   A track simplified structure to reduce the space consumption
//   while keeping most of the information. The (huge amount of)
//   GEANT steps are here reduced to few "strides"
//
// --------------------------------------------------------------
// Comments:
//
//     - 13/06/06 Derive class from TObject to introduce in TClonesArray
//     - 16/05/06 Simplified track structure for analysis in ACTAR
//
/////////////////////////////////////////////////////////////////

#ifndef ActarSimSimpleTrack_h
#define ActarSimSimpleTrack_h 1

#include "TROOT.h"  //for including Rtypes.h
#include "TObject.h"

class ActarSimSimpleTrack : public TObject{

private:

  Double_t xPre;          //coordinates of the preStepPoint for the first step in the stride
  Double_t yPre;
  Double_t zPre;
  Double_t xPost;         //coordinates of the postStepPoint for the last step in the stride
  Double_t yPost;
  Double_t zPost;
  Double_t energyStride;  //total energy on the stride (sum over all steps)
  Double_t particleCharge;
  Double_t particleMass;
  Int_t particleID;
  Double_t strideLength;  //stride length (sum over all steps length)
  Double_t timePre;       //time of preStepPoint for the first step in the stride
  Double_t timePost;      //time of postStepPoint for the last step in the stride
  Int_t numberSteps;      //number of steps in the stride
  Int_t strideOrdinal;    //stride order
  Int_t parentTrackID;

//Note that with the new TClonesArray access this data is not neccesary,
//as they are in the event tree and very well structured. TODO REMOVE!!!
  Int_t trackID;
  Int_t eventID;
  Int_t runID;

public:
  ActarSimSimpleTrack();
  ~ActarSimSimpleTrack();

  void Reset(void);

  ActarSimSimpleTrack& operator=(const ActarSimSimpleTrack &right);

  Double_t GetXPre(){return xPre;}
  Double_t GetYPre(){return yPre;}
  Double_t GetZPre(){return zPre;}
  Double_t GetXPost(){return xPost;}
  Double_t GetYPost(){return yPost;}
  Double_t GetZPost(){return zPost;}

  Double_t GetEnergyStride(){return energyStride;}
  Double_t GetParticleCharge(){return particleCharge;}
  Double_t GetParticleMass(){return particleMass;}
  Int_t GetParticleID(){return particleID;}
  Double_t GetStrideLength(){return strideLength;}
  Double_t GetTimePre(){return timePre;}
  Double_t GetTimePost(){return timePost;}
  Int_t GetNumberSteps(){return numberSteps;}
  Int_t GetStrideOrdinal(){return strideOrdinal;}
  Int_t GetTrackID(){return trackID;}
  Int_t GetParentTrackID(){return parentTrackID;}
  Int_t GetEventID(){return eventID;}
  Int_t GetRunID(){return runID;}

  void SetXPre(Double_t xc){xPre = xc;}
  void SetYPre(Double_t yc){yPre = yc;}
  void SetZPre(Double_t zc){zPre = zc;}
  void SetXPost(Double_t xc){xPost = xc;}
  void SetYPost(Double_t yc){yPost = yc;}
  void SetZPost(Double_t zc){zPost = zc;}
  void SetEnergyStride(Double_t energy){energyStride = energy;}
  void SetParticleCharge(Double_t pc){particleCharge=pc;}
  void SetParticleMass(Double_t pm){particleMass=pm;}
  void SetParticleID(Int_t pi){particleID=pi;}
  void SetStrideLength(Double_t len){strideLength = len;}
  void SetTimePre(Double_t te){timePre = te;}
  void SetTimePost(Double_t te){timePost = te;}
  void SetNumberSteps(Int_t num){numberSteps = num;}
  void SetStrideOrdinal(Int_t num){strideOrdinal = num;}
  void SetTrackID(Int_t track){trackID = track;}
  void SetParentTrackID(Int_t pt){parentTrackID = pt;}
  void SetEventID(Int_t ev){eventID = ev;}
  void SetRunID(Int_t ev){runID = ev;}

  ClassDef(ActarSimSimpleTrack,1) //ROOT CINT
};

#endif
