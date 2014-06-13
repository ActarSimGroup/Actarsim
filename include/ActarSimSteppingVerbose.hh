/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol hapol@fpddux.usc.es
//*-- Date: 11/2004
//*-- Last Update: 28/10/05
// --------------------------------------------------------------
// Description:
//   Controls the verbosity during the step (please, not too much:-)
//
// --------------------------------------------------------------
// Comments:
//   - 30/11/04 Created based on example/novice/N03 structure
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

class ActarSimSteppingVerbose;

#ifndef ActarSimSteppingVerbose_h
#define ActarSimSteppingVerbose_h 1

#include "G4SteppingVerbose.hh"

class ActarSimSteppingVerbose : public G4SteppingVerbose
{
 public:   

   ActarSimSteppingVerbose();
  ~ActarSimSteppingVerbose();

   void StepInfo();
   void TrackingStarted();

};

#endif
