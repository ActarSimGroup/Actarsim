/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol     hapol@fpddux.usc.es
//*-- Date: 03/2006
//*-- Last Update: 04/04/06 by Hector Alvarez Pol
// --------------------------------------------------------------
// Description:
//   Physics List Messenger
//
// --------------------------------------------------------------
// Comments:
//
//  06/03/06 Included after full physics revision. Migrated to geant4.8
//           Based on examples/extended/medical/GammaTherapy
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include "ActarSimPhysicsListMessenger.hh"

#include "ActarSimPhysicsList.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4LossTableManager.hh"


ActarSimPhysicsListMessenger::ActarSimPhysicsListMessenger(ActarSimPhysicsList* pPhys)
:pPhysicsList(pPhys){
  //
  // Constructor with commands definition
  //
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

  verbCmd = new G4UIcmdWithAnInteger("/ActarSim/phys/verbose",this);  
  verbCmd->SetGuidance("Set verbose level for processes");
  verbCmd->SetParameterName("pVerb",false);
  verbCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}


ActarSimPhysicsListMessenger::~ActarSimPhysicsListMessenger(){
  //
  // Destructor
  //
  delete gammaCutCmd;
  delete electCutCmd;
  delete protoCutCmd;
  delete allCutCmd;
  delete pListCmd;
  delete verbCmd;
  delete physDir;  
}


void ActarSimPhysicsListMessenger::SetNewValue(G4UIcommand* command,
                                          G4String newValue) {
  //
  // Setting the values from the messenger
  //       
  if( command == gammaCutCmd )
   { pPhysicsList->SetCutForGamma(gammaCutCmd->GetNewDoubleValue(newValue));}
     
  if( command == electCutCmd )
   { pPhysicsList->SetCutForElectron(electCutCmd->GetNewDoubleValue(newValue));}
     
  if( command == protoCutCmd )
   { pPhysicsList->SetCutForPositron(protoCutCmd->GetNewDoubleValue(newValue));}

  if( command == allCutCmd )
    {
      G4double cut = allCutCmd->GetNewDoubleValue(newValue);
      pPhysicsList->SetCutForGamma(cut);
      pPhysicsList->SetCutForElectron(cut);
      pPhysicsList->SetCutForPositron(cut);
    } 

  if( command == verbCmd )
   { G4LossTableManager::Instance()->SetVerbose(verbCmd->GetNewIntValue(newValue));}

  if( command == pListCmd )
   { pPhysicsList->AddPhysicsList(newValue);}
}
