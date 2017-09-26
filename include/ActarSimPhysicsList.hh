// - AUTHOR: Hector Alvarez-Pol 11/2004
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimPhysicsList_h
#define ActarSimPhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "globals.hh"
#include "G4EmConfigurator.hh"

class ActarSimPhysicsListMessenger;
class ActarSimStepLimiterBuilder;
class G4VPhysicsConstructor;

class ActarSimPhysicsList: public G4VModularPhysicsList {
private:
  void AddIonGasModels();

  G4double cutForGamma;            ///< Cut energy parameter for gammas
  G4double cutForElectron;         ///< Cut energy parameter for gammas
  G4double cutForPositron;         ///< Cut energy parameter for gammas
  G4int    verbose;                ///< Verbose control
  G4bool   emBuilderIsRegisted;    ///< Register control parameter for library
  G4bool   stepLimiterIsRegisted;  ///< Register control parameter for library
  G4bool   helIsRegisted;          ///< Register control parameter for library
  G4bool   bicIsRegisted;          ///< Register control parameter for library
  G4bool   ionIsRegisted;          ///< Register control parameter for library
  G4bool   gnucIsRegisted;         ///< Register control parameter for library
  G4bool   gasIsRegisted;          ///< Register control parameter for library
  G4bool   stopIsRegisted;         ///< Register control parameter for library

  ActarSimPhysicsListMessenger* pMessenger;  ///< Pointer to messenger
  ActarSimStepLimiterBuilder* steplimiter;   ///< Pointer to step limiter
  G4EmConfigurator* fConfig;        ///< Pointer to a Em physics configurator

  G4VPhysicsConstructor*  emPhysicsList;     ///< Pointer to Physics list

public:
  ActarSimPhysicsList();
  ~ActarSimPhysicsList();

  // SetCuts()
  void ConstructParticle();
  void ConstructProcess();
  void SetCuts();

  void SetCutForGamma(G4double);
  void SetCutForElectron(G4double);
  void SetCutForPositron(G4double);

  void AddPhysicsList(const G4String&);
  void AddPAIModel(const G4String& modname);
  void NewPAIModel(const G4ParticleDefinition* part,
                                          const G4String& modname,
                                          const G4String& procname);
  void SetVerbose(G4int val);
};
#endif
