// - AUTHOR: Hector Alvarez-Pol 11/2004
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimEventAction_h
#define ActarSimEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

class ActarSimEventActionMessenger;

class ActarSimEventAction : public G4UserEventAction {
private:
  G4String  drawFlag;
  G4int     printModulo;

  ActarSimEventActionMessenger*  eventMessenger;              ///< Pointer to the messenger

public:
  ActarSimEventAction();
  ~ActarSimEventAction();

  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);

  void SetDrawFlag   (G4String val)  {drawFlag = val;};
  void SetPrintModulo(G4int    val)  {printModulo = val;};
};
#endif
