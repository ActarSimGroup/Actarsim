/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol 
//*-- Date: 05/2005
//*-- Last Update: 07/01/15
// --------------------------------------------------------------
// Description:
//   Track structure
//
// --------------------------------------------------------------
// Comments: 
//     - 31/05/05 Track structure for analysis in ACTAR
//
///////////////////////////////////////////////////////////////// 

#include "ActarSimTrack.hh"

ClassImp(ActarSimTrack)

ActarSimTrack::ActarSimTrack(){
  //
  //
  //

  xCoord = 0.;
  yCoord = 0.;
  zCoord = 0.;
  xPreCoord = 0.;
  yPreCoord = 0.;
  zPreCoord = 0.;
  energyStep = 0.;
  parentTrackID = 0;
  trackID = 0;
  eventID = 0;
  runID = 0;
}

ActarSimTrack::~ActarSimTrack(){
  //
  //
  //
}

