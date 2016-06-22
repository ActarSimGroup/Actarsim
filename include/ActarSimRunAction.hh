// - AUTHOR: Hector Alvarez-Pol 11/2004
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimRunAction_h
#define ActarSimRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

class ActarSimRunAction : public G4UserRunAction {
public:
  ActarSimRunAction();
  ~ActarSimRunAction();

  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction(const G4Run*);
};
#endif
