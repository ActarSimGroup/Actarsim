/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol hapol@fpddux.usc.es
//*-- Date: 05/2005
//*-- Last Update: 28/10/05
// --------------------------------------------------------------
// Description:
//   Data definition 
//
// --------------------------------------------------------------
// Comments: 
//     - 31/05/05 Data structure for analysis in ACTAR
//
///////////////////////////////////////////////////////////////// 

#include "ActarSimData.hh"

ClassImp(ActarSimData)

ActarSimData::ActarSimData(){
  //
  // Constructor with initial values
  //
  thetaPrim1 = 0;
  thetaPrim2 = 0;
  phiPrim1 = 0;
  phiPrim2 = 0;
  energyPrim1 = 0;
  energyPrim2 = 0;
  energyOnGasPrim1 = 0;
  energyOnGasPrim2 = 0;
  stepSumLengthOnGasPrim1 = 0;
  stepSumLengthOnGasPrim2 = 0;
  protonLastPositionCode=0.; // dypang 080302
  eventID = 0;
  runID = 0;
}

ActarSimData::~ActarSimData(){
  //
  //
  //
}

