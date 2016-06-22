// - AUTHOR: Hector Alvarez-Pol 03/2006
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimPhysicsListMessenger_h
#define ActarSimPhysicsListMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class ActarSimPhysicsList;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;

class ActarSimPhysicsListMessenger: public G4UImessenger {
private:
  ActarSimPhysicsList*       pPhysicsList;  ///< Pointer to the Physic List

  G4UIdirectory*             physDir;       ///< Directory
  G4UIcmdWithADoubleAndUnit* gammaCutCmd;   ///< Set gamma cut
  G4UIcmdWithADoubleAndUnit* electCutCmd;   ///< Set electron cut
  G4UIcmdWithADoubleAndUnit* protoCutCmd;   ///< Set positron cut
  G4UIcmdWithADoubleAndUnit* allCutCmd;     ///< Set cut for all
  G4UIcmdWithAnInteger*      verbCmd;       ///< Set verbose level for processes
  G4UIcmdWithAString*        pListCmd;      ///< Add modula physics list
  G4UIcmdWithAString*        RmPListCmd;    ///< Remove all modula physics list

public:
  ActarSimPhysicsListMessenger(ActarSimPhysicsList* );
  ~ActarSimPhysicsListMessenger();

  void SetNewValue(G4UIcommand*, G4String);
};
#endif
