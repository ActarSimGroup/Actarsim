/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol
//*-- Date: 11/2004
//*-- Last Update: 1/12/14
// --------------------------------------------------------------
// Description:
//   Messenger for the event actions
//
// --------------------------------------------------------------
// Comments:
//   - 03/12/04 Created based on example/novice/N03 structure
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#ifndef ActarSimEventActionMessenger_h
#define ActarSimEventActionMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class ActarSimEventAction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;

class ActarSimEventActionMessenger: public G4UImessenger
{
  public:
    ActarSimEventActionMessenger(ActarSimEventAction*);
   ~ActarSimEventActionMessenger();
    
    void SetNewValue(G4UIcommand*, G4String);
    
  private:
    ActarSimEventAction*  eventAction;
    G4UIdirectory*        eventDir;   
    G4UIcmdWithAString*   DrawCmd;
    G4UIcmdWithAnInteger* PrintCmd;
};

#endif
