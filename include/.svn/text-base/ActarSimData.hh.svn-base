///////////////////////////////////////////////////////////////// 
//*-- AUTHOR : Hector Alvarez-Pol hapol@fpddux.usc.es
//*-- Date: 05/2005
//*-- Last Update:  28/10/05
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

//#include "G4Types.hh"
#include "TROOT.h"  //for including Rtypes.h

//#ifndef __CINT__
//#include "G4String.hh" //Explicit inclusion for root dictionary
//using namespace std;
//#else
//struct G4String;
//#endif

class ActarSimData{  
  
private:
  
  Double_t thetaPrim1;
  Double_t thetaPrim2;
  Double_t phiPrim1;
  Double_t phiPrim2;
  Double_t energyPrim1;
  Double_t energyPrim2;
  Double_t energyOnGasPrim1;
  Double_t energyOnGasPrim2;
  Double_t stepSumLengthOnGasPrim1;
  Double_t stepSumLengthOnGasPrim2;
  Double_t protonLastPositionCode; // 0-7, the position of proton in the last step
  Double_t protonLastPositionx;
  Double_t protonLastPositiony;
  Double_t protonLastPositionz;
  Int_t    eventID;
  Int_t    runID;
  
public:
  ActarSimData();
  ~ActarSimData();

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
  Double_t GetProtonLastPositionCode(){return protonLastPositionCode;} // dypang 080302
  Double_t GetProtonLastPositionx(){return protonLastPositionx;}
  Double_t GetProtonLastPositiony(){return protonLastPositiony;}
  Double_t GetProtonLastPositionz(){return protonLastPositionz;}
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
  void SetProtonLastPositionCode(Double_t pCode){protonLastPositionCode=pCode;} // dypang 080302
  void SetProtonLastPositionx(Double_t PLP) {protonLastPositionx=PLP;}
  void SetProtonLastPositiony(Double_t PLP) {protonLastPositiony=PLP;}
  void SetProtonLastPositionz(Double_t PLP) {protonLastPositionz=PLP;}
  void SetEventID(Int_t ev){eventID = ev;}
  void SetRunID(Int_t ev){runID = ev;}

  ClassDef(ActarSimData,1) //ROOT CINT
};

#endif
