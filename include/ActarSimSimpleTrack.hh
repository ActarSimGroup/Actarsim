// - AUTHOR: Hector Alvarez-Pol 05/2006
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimSimpleTrack_h
#define ActarSimSimpleTrack_h 1

#include "TROOT.h"  //for including Rtypes.h
#include "TObject.h"

class ActarSimSimpleTrack : public TObject {
private:
  Double_t xPre;            ///< Coordinate X of the preStepPoint for the first step in the stride
  Double_t yPre;            ///< Coordinate Y of the preStepPoint for the first step in the stride
  Double_t zPre;            ///< Coordinate Z of the preStepPoint for the first step in the stride
  Double_t xPost;           ///< Coordinate X of the postStepPoint for the last step in the stride
  Double_t yPost;           ///< Coordinate Y of the postStepPoint for the last step in the stride
  Double_t zPost;           ///< Coordinate Z of the postStepPoint for the last step in the stride
  Double_t energyStride;    ///< Total energy on the stride (sum over all steps)
  Double_t particleCharge;  ///< Particle charge
  Double_t particleMass;    ///< Particle mass
  Int_t particleID;         ///< Particle ID
  Double_t strideLength;    ///< Stride length (sum over all steps length)
  Double_t particleEnergy;  ///< Particle energy before the step and thus before the stride
  Double_t timePre;         ///< Time of preStepPoint for the first step in the stride
  Double_t timePost;        ///< Time of postStepPoint for the last step in the stride
  Int_t numberSteps;        ///< Number of steps in the stride
  Int_t strideOrdinal;      ///< Stride order
  Int_t parentTrackID;      ///< Parent Track ID

  //Note that with the new TClonesArray access this data is not neccesary,
  //as they are in the event tree and very well structured. TODO REMOVE!!!

  Int_t trackID;   ///< Track ID
  Int_t eventID;   ///< Event ID
  Int_t runID;     ///< Run ID

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
  Double_t GetParticleEnergy(){return particleEnergy;}
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
  void SetParticleCharge(Double_t parcha){particleCharge=parcha;}
  void SetParticleMass(Double_t pm){particleMass=pm;}
  void SetParticleID(Int_t piD){particleID=piD;}
  void SetStrideLength(Double_t len){strideLength = len;}
  void SetParticleEnergy(Double_t penergy){particleEnergy = penergy;}
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
