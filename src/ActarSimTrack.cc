// - AUTHOR: Hector Alvarez-Pol 05/2005
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/
//////////////////////////////////////////////////////////////////
/// \class ActarSimTrack
///  Track structure
/////////////////////////////////////////////////////////////////

#include "ActarSimTrack.hh"

ClassImp(ActarSimTrack)

//////////////////////////////////////////////////////////////////
/// Constructor
ActarSimTrack::ActarSimTrack(){
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

//////////////////////////////////////////////////////////////////
/// Destructor
ActarSimTrack::~ActarSimTrack(){
}
