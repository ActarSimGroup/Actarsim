// - AUTHOR: Hector Alvarez-Pol 05/2005
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/
//////////////////////////////////////////////////////////////////
/// \class ActarSimData
/// The information from the track of the primaries generated at
/// the reaction vertex. All data members are accesible in the
/// branch theData of The_Event_Tree.
/////////////////////////////////////////////////////////////////

#include "ActarSimData.hh"

ClassImp(ActarSimData)

//////////////////////////////////////////////////////////////////
/// Constructor with initial values
ActarSimData::ActarSimData() {
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
  eventID = 0;
  runID = 0;
}

//////////////////////////////////////////////////////////////////
/// Destructor
ActarSimData::~ActarSimData() {
}
