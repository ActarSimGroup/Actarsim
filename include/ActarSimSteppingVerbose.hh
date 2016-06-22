// - AUTHOR: Hector Alvarez-Pol 11/2004
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

class ActarSimSteppingVerbose;

#ifndef ActarSimSteppingVerbose_h
#define ActarSimSteppingVerbose_h 1

#include "G4SteppingVerbose.hh"

class ActarSimSteppingVerbose : public G4SteppingVerbose {
public:
  ActarSimSteppingVerbose();
  ~ActarSimSteppingVerbose();

  void StepInfo();
  void TrackingStarted();
};
#endif
