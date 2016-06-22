// - AUTHOR: Hector Alvarez-Pol 03/2006
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/
//////////////////////////////////////////////////////////////////
/// \class ActarSimPhysicsListMessenger
/// Physics List Messenger
/////////////////////////////////////////////////////////////////

#include "ActarSimPhysicsListMessenger.hh"
#include "ActarSimPhysicsList.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4LossTableManager.hh"

//////////////////////////////////////////////////////////////////
/// Constructor with commands definition.
/// Commands included in this AnalysisMessenger:
/// - /ActarSim/phys/setGCut
/// - /ActarSim/phys/setECut
/// - /ActarSim/phys/setPCut
/// - /ActarSim/phys/setCuts
/// - /ActarSim/phys/addPhysics
/// - /ActarSim/phys/RemovePhysics
/// - /ActarSim/phys/verbose
ActarSimPhysicsListMessenger::ActarSimPhysicsListMessenger(ActarSimPhysicsList* pPhys)
  :pPhysicsList(pPhys){

  physDir = new G4UIdirectory("/ActarSim/phys/");
  physDir->SetGuidance("physics list commands");

  gammaCutCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/phys/setGCut",this);
  gammaCutCmd->SetGuidance("Set gamma cut.");
  gammaCutCmd->SetParameterName("Gcut",false);
  gammaCutCmd->SetUnitCategory("Length");
  gammaCutCmd->SetRange("Gcut>0.0");
  gammaCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  electCutCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/phys/setECut",this);
  electCutCmd->SetGuidance("Set electron cut.");
  electCutCmd->SetParameterName("Ecut",false);
  electCutCmd->SetUnitCategory("Length");
  electCutCmd->SetRange("Ecut>0.0");
  electCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  protoCutCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/phys/setPCut",this);
  protoCutCmd->SetGuidance("Set positron cut.");
  protoCutCmd->SetParameterName("Pcut",false);
  protoCutCmd->SetUnitCategory("Length");
  protoCutCmd->SetRange("Pcut>0.0");
  protoCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  allCutCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/phys/setCuts",this);
  allCutCmd->SetGuidance("Set cut for all.");
  allCutCmd->SetParameterName("cut",false);
  allCutCmd->SetUnitCategory("Length");
  allCutCmd->SetRange("cut>0.0");
  allCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  pListCmd = new G4UIcmdWithAString("/ActarSim/phys/addPhysics",this);
  pListCmd->SetGuidance("Add modula physics list.");
  pListCmd->SetParameterName("PList",false);
  pListCmd->AvailableForStates(G4State_PreInit);

  RmPListCmd = new G4UIcmdWithAString("/ActarSim/phys/RemovePhysics",this);
  RmPListCmd->SetGuidance("Remove all modula physics list.");
  RmPListCmd->AvailableForStates(G4State_Idle);

  verbCmd = new G4UIcmdWithAnInteger("/ActarSim/phys/verbose",this);
  verbCmd->SetGuidance("Set verbose level for processes");
  verbCmd->SetParameterName("pVerb",false);
  verbCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

//////////////////////////////////////////////////////////////////
/// Destructor
ActarSimPhysicsListMessenger::~ActarSimPhysicsListMessenger(){
  delete gammaCutCmd;
  delete electCutCmd;
  delete protoCutCmd;
  delete allCutCmd;
  delete pListCmd;
  delete RmPListCmd;
  delete verbCmd;
  delete physDir;
}

//////////////////////////////////////////////////////////////////
/// Setting the values using the ActarSimPhysicsList interface
void ActarSimPhysicsListMessenger::SetNewValue(G4UIcommand* command,
					       G4String newValue) {

  if( command == gammaCutCmd )
    pPhysicsList->SetCutForGamma(gammaCutCmd->GetNewDoubleValue(newValue));

  if( command == electCutCmd )
    pPhysicsList->SetCutForElectron(electCutCmd->GetNewDoubleValue(newValue));


  if( command == protoCutCmd )
    pPhysicsList->SetCutForPositron(protoCutCmd->GetNewDoubleValue(newValue));

  if( command == allCutCmd ) {
    G4double cut = allCutCmd->GetNewDoubleValue(newValue);
    pPhysicsList->SetCutForGamma(cut);
    pPhysicsList->SetCutForElectron(cut);
    pPhysicsList->SetCutForPositron(cut);
  }

  if( command == verbCmd )
    G4LossTableManager::Instance()->SetVerbose(verbCmd->GetNewIntValue(newValue));

  if( command == pListCmd )
    pPhysicsList->AddPhysicsList(newValue);

  if( command == RmPListCmd )
    pPhysicsList->~ActarSimPhysicsList();
}
