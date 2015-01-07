/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol
//*-- Date: 05/2006
//*-- Last Update: 07/01/15
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

#include "ActarSimSimpleTrack.hh"

ClassImp(ActarSimSimpleTrack)

ActarSimSimpleTrack::ActarSimSimpleTrack(){
  //
  // Constructor initializing to defaults
  //

  xPre = 0.;
  yPre = 0.;
  zPre = 0.;
  xPost = 0.;
  yPost = 0.;
  zPost = 0.;
  energyStride = 0.;
  strideLength = 0.;
  timePre = 0.;
  timePost = 0.;
  numberSteps = 0;
  strideOrdinal = 0;
  parentTrackID = -999;
  trackID = -999;
  eventID = -999;
  runID = -999;
}

ActarSimSimpleTrack::~ActarSimSimpleTrack(){
  //
  // Destructor. Makes nothing
  //
}

void ActarSimSimpleTrack::Reset(void){
  //
  // Clearing to defaults
  //
  xPre = 0.;
  yPre = 0.;
  zPre = 0.;
  xPost = 0.;
  yPost= 0.;
  zPost = 0.;
  energyStride = 0.;
  particleCharge=0.;
  particleMass=0.;
  particleID=0;
  strideLength = 0.;
  particleEnergy = 0.;
  timePre = 0.;
  timePost = 0.;
  numberSteps = 0;
  strideOrdinal = 0;
  parentTrackID = -999;
  trackID = -999;
  eventID = -999;
  runID = -999;
}

ActarSimSimpleTrack & ActarSimSimpleTrack::operator=(const ActarSimSimpleTrack &right){
  //
  // overloading the copy operator...
  //

  if (this != &right){
    xPre = right.xPre;
    yPre = right.yPre;
    zPre = right.zPre;
    xPost = right.xPost;
    yPost = right.yPost;
    zPost = right.zPost;
    energyStride = right.energyStride;
    particleCharge = right.particleCharge;
    particleMass = right.particleMass;
    particleID = right.particleID;
    strideLength = right.strideLength;
    particleEnergy = right.particleEnergy;
    timePre = right.timePre;
    timePost = right.timePost;
    numberSteps = right.numberSteps;
    strideOrdinal = right.strideOrdinal;
    parentTrackID = right.parentTrackID;
    trackID = right.trackID;
    eventID = right.eventID;
    runID = right.runID;
  }
  return *this;
}
