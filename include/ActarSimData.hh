/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol
//*-- Date: 05/2005
//*-- Last Update:  1/12/2015
// --------------------------------------------------------------
// Description:
//   Data definition
//
// --------------------------------------------------------------
// Comments:
//     - 31/05/05 Data structure for analysis in ACTAR
//
/////////////////////////////////////////////////////////////////

#ifndef ActarSimData_h
#define ActarSimData_h 1

#include "TROOT.h"  //for including Rtypes.h

class ActarSimData{

private:

  Double_t thetaPrim1;                //polar angle for first primary
  Double_t thetaPrim2;                //polar angle for second primary
  Double_t phiPrim1;                  //azimuthal angle for first primary
  Double_t phiPrim2;                  //azimuthal angle for second primary
  Double_t energyPrim1;               //energy for first primary
  Double_t energyPrim2;               //energy for second primary
  Double_t energyOnGasPrim1;          //energy deposited in the gas for first primary
  Double_t energyOnGasPrim2;          //energy deposited in the gas for second primary
  Double_t stepSumLengthOnGasPrim1;   //sum of steps length in the gas for first primary
  Double_t stepSumLengthOnGasPrim2;   //sum of steps length in the gas for second primary
  Int_t    eventID;                   //event number
  Int_t    runID;                     //run number

public:
  ActarSimData();
  virtual ~ActarSimData();

  Double_t GetThetaPrim1(){return thetaPrim1;}
  Double_t GetThetaPrim2(){return thetaPrim2;}
  Double_t GetPhiPrim1(){return phiPrim1;}
  Double_t GetPhiPrim2(){return phiPrim2;}
  Double_t GetEnergyPrim1(){return energyPrim1;}
  Double_t GetEnergyPrim2(){return energyPrim2;}
  Double_t GetEnergyOnGasPrim1(){return energyOnGasPrim1;}
  Double_t GetEnergyOnGasPrim2(){return energyOnGasPrim2;}
  Double_t GetStepSumLengthOnGasPrim1(){return stepSumLengthOnGasPrim1;}
  Double_t GetStepSumLengthOnGasPrim2(){return stepSumLengthOnGasPrim2;}
  Int_t GetEventID(){return eventID;}
  Int_t GetRunID(){return runID;}

  void SetThetaPrim1(Double_t theta){thetaPrim1 = theta;}
  void SetThetaPrim2(Double_t theta){thetaPrim2 = theta;}
  void SetPhiPrim1(Double_t phi){phiPrim1 = phi;}
  void SetPhiPrim2(Double_t phi){phiPrim2 = phi;}
  void SetEnergyPrim1(Double_t energy){energyPrim1 = energy;}
  void SetEnergyPrim2(Double_t energy){energyPrim2 = energy;}
  void SetEnergyOnGasPrim1(Double_t energy){energyOnGasPrim1 = energy;}
  void SetEnergyOnGasPrim2(Double_t energy){energyOnGasPrim2 = energy;}
  void SetStepSumLengthOnGasPrim1(Double_t step){stepSumLengthOnGasPrim1 = step;}
  void SetStepSumLengthOnGasPrim2(Double_t step){stepSumLengthOnGasPrim2 = step;}
  void SetEventID(Int_t ev){eventID = ev;}
  void SetRunID(Int_t ev){runID = ev;}

  ClassDef(ActarSimData,1) //ROOT CINT
};

#endif
