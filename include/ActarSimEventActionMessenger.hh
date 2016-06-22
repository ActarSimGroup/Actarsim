// - AUTHOR: Hector Alvarez-Pol 11/2004
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimEventActionMessenger_h
#define ActarSimEventActionMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class ActarSimEventAction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;

class ActarSimEventActionMessenger: public G4UImessenger {
private:
  ActarSimEventAction*  eventAction;        ///< Pointer to event action class
  G4UIdirectory*        eventDir;           ///< Directory for the commands
  G4UIcmdWithAString*   DrawCmd;            ///< Draw the tracks in the event
  G4UIcmdWithAnInteger* PrintCmd;           ///< Print events modulo n

public:
  ActarSimEventActionMessenger(ActarSimEventAction*);
  ~ActarSimEventActionMessenger();

  void SetNewValue(G4UIcommand*, G4String);
};
#endif
