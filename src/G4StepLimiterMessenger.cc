//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
// $Id: G4StepLimiterMessenger.cc,v 1.1 2004/12/02 10:34:08 vnivanch Exp $
// GEANT4 tag $Name: geant4-08-00-patch-01 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4StepLimiterMessenger.hh"

#include "G4StepLimiterPerRegion.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4StepLimiterMessenger::G4StepLimiterMessenger(G4StepLimiterPerRegion* stepM)
:stepLimiter(stepM)
{
  stepMaxCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/phys/stepMax",this);
  stepMaxCmd->SetGuidance("Set max allowed step length for the default region");
  stepMaxCmd->SetParameterName("mxStep",false);
  stepMaxCmd->SetRange("mxStep>0.");
  stepMaxCmd->SetUnitCategory("Length");
  stepMaxCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4StepLimiterMessenger::~G4StepLimiterMessenger()
{
  delete stepMaxCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4StepLimiterMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == stepMaxCmd)
    { stepLimiter->SetMaxStep(stepMaxCmd->GetNewDoubleValue(newValue));}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
