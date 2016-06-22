// - AUTHOR: Hector Alvarez-Pol 05/2006
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/
//////////////////////////////////////////////////////////////////
/// \class ActarSimSimpleTrack
/// A track simplified structure to reduce the space consumption
///  while keeping most of the information. The (huge amount of)
///  GEANT steps are here reduced to few "strides"
/////////////////////////////////////////////////////////////////

#include "ActarSimSimpleTrack.hh"

ClassImp(ActarSimSimpleTrack)

//////////////////////////////////////////////////////////////////
/// Constructor initializing to defaults
ActarSimSimpleTrack::ActarSimSimpleTrack(){
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

//////////////////////////////////////////////////////////////////
/// Destructor. Makes nothing
ActarSimSimpleTrack::~ActarSimSimpleTrack(){
}

//////////////////////////////////////////////////////////////////
/// Clearing to defaults
void ActarSimSimpleTrack::Reset(void){
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

//////////////////////////////////////////////////////////////////
/// Overloading the copy operator...
ActarSimSimpleTrack & ActarSimSimpleTrack::operator=(const ActarSimSimpleTrack &right){
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
