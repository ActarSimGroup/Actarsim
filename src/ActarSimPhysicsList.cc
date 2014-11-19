/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol     hapol@fpddux.usc.es
//*-- Date: 11/2004
//*-- Last Update: 04/04/06 by Hector Alvarez Pol
// --------------------------------------------------------------
// Description:
//   Physics List 
//
// --------------------------------------------------------------
// Comments:
//
//  04/03/06 Full physics revision. Migrated to geant4.8
//           Based on examples/extended/medical/GammaTherapy
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include "ActarSimPhysicsList.hh"
#include "ActarSimPhysicsListMessenger.hh"

#include "ActarSimParticlesBuilder.hh"
#include "ActarSimStepLimiterBuilder.hh"
#include "ActarSimDecaysBuilder.hh"

#include "G4EmQEDBuilder.hh"
#include "G4EmMuonBuilder.hh"
#include "G4EmHadronBuilder.hh"
#include "G4LowEnergyQEDBuilder.hh"
#include "G4PenelopeQEDBuilder.hh"

#include "EmHadronElasticBuilder.hh"
#include "EmBinaryCascadeBuilder.hh"
#include "EmIonBinaryCascadeBuilder.hh"
#include "EmGammaNucleusBuilder.hh"
 
//New PhysicsLists Em
#include "PhysListEmStandard.hh"
#include "PhysListEmStandardWVI.hh"
#include "PhysListEmStandardSS.hh"
#include "PhysListEmStandardGS.hh"

#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option1.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmStandardPhysics_option4.hh"

#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"

#include "HadrontherapyIonStandard.hh"

#include "G4UnitsTable.hh"
#include "G4LossTableManager.hh"
#include "G4EmProcessOptions.hh"

#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"

//Piotr 10/09/2014

#include "G4EmConfigurator.hh"

#include "G4IonFluctuations.hh"
#include "G4IonParametrisedLossModel.hh"
#include "G4UniversalFluctuation.hh"

#include "G4BraggIonGasModel.hh"
#include "G4BetheBlochIonGasModel.hh"

#include "G4IonPhysics.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"


ActarSimPhysicsList::ActarSimPhysicsList():  G4VModularPhysicsList(){
  //
  // Constructor. Initializing values
  //
  
  emBuilderIsRegisted = false;
  decayIsRegisted = false;
  stepLimiterIsRegisted = false;
  helIsRegisted = false;
  bicIsRegisted = false;
  ionIsRegisted = false;
  gnucIsRegisted = false;  

  verbose = 0;
  //G4LossTableManager::Instance()->SetVerbose(0);
  G4LossTableManager::Instance();
  defaultCutValue = 1.*mm;
  cutForGamma     = defaultCutValue;
  cutForElectron  = defaultCutValue;
  cutForPositron  = defaultCutValue;
  
  pMessenger = new ActarSimPhysicsListMessenger(this);
  
  // EM physics
  //emPhysicsList = new PhysListEmStandard("local");
  emPhysicsList = new G4EmStandardPhysics(1);

  // Add Physics builders
  RegisterPhysics(new ActarSimParticlesBuilder());
  steplimiter = new ActarSimStepLimiterBuilder();
}


ActarSimPhysicsList::~ActarSimPhysicsList() {
  //
  // Destructor. Nothing to do
  //  
  delete emPhysicsList;
  delete pMessenger;  

}


void ActarSimPhysicsList::ConstructParticle() {
  //
  // Construct Particles
  //

  if(verbose > 0)
    G4cout << "Construct Particles" << G4endl;

  G4VModularPhysicsList::ConstructParticle();
}



void ActarSimPhysicsList::ConstructProcess() {
  //
  // Construct Processes
  //
  /*
  // transportation
  //
  AddTransportation();
  
  // electromagnetic physics list
  //
  emPhysicsList->ConstructProcess();
  */

  if(verbose > 0)
    G4cout << "Construct Processes" << G4endl;

  if(!emBuilderIsRegisted) 
    AddPhysicsList("standard");
  if(!emPhysicsList)
    G4VModularPhysicsList::ConstructProcess();
  else{
   AddTransportation();
   emPhysicsList->ConstructProcess();
  }
  // Define energy interval for loss processes
  G4EmProcessOptions emOptions;
  emOptions.SetMinEnergy(0.1*keV);
  emOptions.SetMaxEnergy(100.*GeV);
  emOptions.SetDEDXBinning(90);
  emOptions.SetLambdaBinning(90);
  //emOptions.SetBuildPreciseRange(false);
  //emOptions.SetApplyCuts(true);
  //emOptions.SetVerbose(0);

}

void ActarSimPhysicsList::AddPhysicsList(const G4String& name){
  //
  // Registering the physics processes
  //
  if(verbose > 0) {
    G4cout << "Add Physics <" << name 
           << "> emBuilderIsRegisted= " << emBuilderIsRegisted
           << G4endl;
  }
  if ((name == "standard") && !emBuilderIsRegisted) {
    RegisterPhysics(new G4EmQEDBuilder());
    RegisterPhysics(steplimiter);
    RegisterPhysics(new G4EmMuonBuilder());
    RegisterPhysics(new G4EmHadronBuilder());
    emBuilderIsRegisted = true;
    G4cout << "ActarSimPhysicsList::AddPhysicsList <" << name << ">" << G4endl;    

  } else if (name == "lowenergy" && !emBuilderIsRegisted) {
    RegisterPhysics(new G4LowEnergyQEDBuilder());
    RegisterPhysics(steplimiter);
    RegisterPhysics(new G4EmMuonBuilder());
    RegisterPhysics(new G4EmHadronBuilder());
    emBuilderIsRegisted = true;
    G4cout << "ActarSimPhysicsList::AddPhysicsList <" << name << ">" << G4endl;
    
  } 

  //DPL
  else if (name == "local") {

    delete emPhysicsList;
    emPhysicsList = new PhysListEmStandard(name);
    G4cout << "ActarSimPhysicsList::AddPhysicsList <" << name << ">" << G4endl;    

  }

  else if ((name == "emstandard_opt0") && !emBuilderIsRegisted) {
    
    //fEmName = name;
    delete emPhysicsList;
    emPhysicsList = new G4EmStandardPhysics(1);
    //RegisterPhysics(steplimiter);
    emBuilderIsRegisted = true;
    G4cout << "ActarSimPhysicsList::AddPhysicsList <" << name << ">" << G4endl; 
  }

  else if (name == "emstandard_opt1") {

    // emName = name;
    delete emPhysicsList;
    emPhysicsList = new G4EmStandardPhysics_option1();
    emBuilderIsRegisted = true;
    G4cout << "ActarSimPhysicsList::AddPhysicsList <" << name << ">" << G4endl;    
   
  } 
  else if (name == "emstandard_opt2") {

    // emName = name; 
    delete emPhysicsList;
    emPhysicsList = new G4EmStandardPhysics_option2();
    emBuilderIsRegisted = true;
    G4cout << "ActarSimPhysicsList::AddPhysicsList <" << name << ">" << G4endl;    
    
  }
  else if (name == "emstandard_opt3") {

    //emName = name;
    delete emPhysicsList;
    emPhysicsList = new G4EmStandardPhysics_option3();
    emBuilderIsRegisted = true;
    G4cout << "ActarSimPhysicsList::AddPhysicsList <" << name << ">" << G4endl;    
   
  }
  else if (name == "emstandard_opt4") {

    //emName = name;
    delete emPhysicsList;
    emPhysicsList = new G4EmStandardPhysics_option4();
    emBuilderIsRegisted = true;
    G4cout << "ActarSimPhysicsList::AddPhysicsList <" << name << ">" << G4endl;    
   
  }
  else if (name == "standardSS") {

    // emName = name;
    delete emPhysicsList;
    emPhysicsList = new PhysListEmStandardSS(name);
    emBuilderIsRegisted = true;
    G4cout << "ActarSimPhysicsList::AddPhysicsList <" << name << ">" << G4endl;    

  }
  else if (name == "standardWVI") {

    //emName = name;
    delete emPhysicsList;
    emPhysicsList = new PhysListEmStandardWVI(name);
    emBuilderIsRegisted = true;
    G4cout << "ActarSimPhysicsList::AddPhysicsList <" << name << ">" << G4endl;    

  }
  else if (name == "standardGS") {
    //emName = name;
    delete emPhysicsList;
    emPhysicsList = new PhysListEmStandardGS(name);
    G4cout << "ActarSimPhysicsList::AddPhysicsList <" << name << ">" << G4endl;    
    
  }
  else if (name == "empenelope"){
    //emName = name;
    delete emPhysicsList;
    emPhysicsList = new G4EmPenelopePhysics();
    G4cout << "ActarSimPhysicsList::AddPhysicsList <" << name << ">" << G4endl;    
    
  }
  else if (name == "emlivermore"){
    //emName = name;
    delete emPhysicsList;
    emPhysicsList = new G4EmLivermorePhysics();
    G4cout << "ActarSimPhysicsList::AddPhysicsList <" << name << ">" << G4endl;    
  }  
  // Register Low Energy  processes for protons and ions
  // Stopping power parameterisation: ICRU49 (default model)
  // Register Standard processes for protons and ions
  else if (name == "ion-standard") {
    if (ionIsRegisted) 
      G4cout << "ActarSimPhysicsList::AddPhysicsList: " << name  
	     << " cannot be registered ---- ion List already existing" 
	     << G4endl;
    else {
      G4cout << "ActarSimPhysicsList::AddPhysicsList: " << name 
	     << " is registered" << G4endl;
      RegisterPhysics( new HadrontherapyIonStandard(name) );
      ionIsRegisted = true;
    }
  }
  else if (name == "penelope" && !emBuilderIsRegisted) {
    RegisterPhysics(new G4PenelopeQEDBuilder());
    RegisterPhysics(steplimiter);
    RegisterPhysics(new G4EmMuonBuilder());
    RegisterPhysics(new G4EmHadronBuilder());
    emBuilderIsRegisted = true;
    G4cout << "ActarSimPhysicsList::AddPhysicsList <" << name << ">" << G4endl;

  } else if (name == "decay" && !decayIsRegisted) {
    RegisterPhysics(new ActarSimDecaysBuilder());
    decayIsRegisted = true;
    G4cout << "ActarSimPhysicsList::AddPhysicsList <" << name << ">" << G4endl;

  } else if (name == "elastic" && !helIsRegisted) {
    RegisterPhysics(new EmHadronElasticBuilder());
    helIsRegisted = true;
    G4cout << "ActarSimPhysicsList::AddPhysicsList <" << name << ">" << G4endl;
    
  } else if (name == "binary" && !bicIsRegisted) {
    RegisterPhysics(new EmBinaryCascadeBuilder());
    bicIsRegisted = true;
    G4cout << "ActarSimPhysicsList::AddPhysicsList <" << name << ">" << G4endl;
    
  } else if (name == "binary_ion" && !ionIsRegisted) {
    RegisterPhysics(new EmIonBinaryCascadeBuilder());
    ionIsRegisted = true;
    G4cout << "ActarSimPhysicsList::AddPhysicsList <" << name << ">" << G4endl;

  } else if (name == "gamma_nuc" && !gnucIsRegisted) {
    RegisterPhysics(new EmGammaNucleusBuilder());
    gnucIsRegisted = true;
    G4cout << "ActarSimPhysicsList::AddPhysicsList <" << name << ">" << G4endl;
    
  } else if (name == "ionGasModels") {
    //AddPhysicsList("emstandard_opt0");
      AddIonGasModels();   
      G4cout << "ActarSimPhysicsList::AddPhysicsList <" << name << ">" << G4endl;
  }

  else {
    G4cout << "ActarSimPhysicsList::AddPhysicsList <" << name << ">" 
           << " fail - module is already regitered or is unknown " << G4endl;
  }
}





void ActarSimPhysicsList::SetCuts(){
  //
  //  Sets the cut on the physics interaction calculations.
  //  " G4VUserPhysicsList::SetCutsWithDefault" method sets 
  //   the default cut value for all particle types 
  //
  SetCutValue(cutForGamma, "gamma");
  SetCutValue(cutForElectron, "e-");
  SetCutValue(cutForPositron, "e+");

  if (verbose>0) DumpCutValuesTable();
}


void ActarSimPhysicsList::SetVerbose(G4int val){
  //
  // Selecting verbosity
  //

  verbose = val;
}


void ActarSimPhysicsList::SetCutForGamma(G4double cut){
  //
  // Setting cut value for the gammas
  //

  cutForGamma = cut;
  SetParticleCuts(cutForGamma, G4Gamma::Gamma());
}


void ActarSimPhysicsList::SetCutForElectron(G4double cut){
  //
  // Setting cut value for the electron
  //

  cutForElectron = cut;
  SetParticleCuts(cutForElectron, G4Electron::Electron());
}


void ActarSimPhysicsList::SetCutForPositron(G4double cut) {
  //
  // Setting cut value for the positron
  //
  cutForPositron = cut;
  SetParticleCuts(cutForPositron, G4Positron::Positron());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void ActarSimPhysicsList::AddIonGasModels()
{
  G4EmConfigurator* em_config = G4LossTableManager::Instance()->EmConfigurator();
  theParticleIterator->reset();
  while ((*theParticleIterator)())
  {
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4String partname = particle->GetParticleName();
    if(partname == "alpha" || partname == "He3" || partname == "GenericIon") {
      G4BraggIonGasModel* mod1 = new G4BraggIonGasModel();
      G4BetheBlochIonGasModel* mod2 = new G4BetheBlochIonGasModel();
      G4double eth = 2.*MeV*particle->GetPDGMass()/proton_mass_c2;
      em_config->SetExtraEmModel(partname,"ionIoni",mod1,"",0.0,eth,
                                 new G4IonFluctuations());
      em_config->SetExtraEmModel(partname,"ionIoni",mod2,"",eth,100*TeV,
                                 new G4UniversalFluctuation());

    }
  }
}
