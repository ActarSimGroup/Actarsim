// - AUTHOR: Hector Alvarez-Pol 01/2006
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimSteppingAction_h
#define ActarSimSteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class ActarSimDetectorConstruction;
class ActarSimEventAction;

class ActarSimSteppingAction : public G4UserSteppingAction {
private:
  ActarSimDetectorConstruction* detector;    ///< NOT USED
  ActarSimEventAction*          eventaction; ///< NOT USED
public:
  ActarSimSteppingAction(ActarSimDetectorConstruction*, ActarSimEventAction*);
  ~ActarSimSteppingAction();

  void UserSteppingAction(const G4Step*);
};
#endif
