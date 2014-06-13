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
//
// $Id: G4PenelopeQEDBuilder.cc,v 1.1 2004/12/02 10:34:08 vnivanch Exp $
// GEANT4 tag $Name: geant4-08-00-patch-01 $
//
//---------------------------------------------------------------------------
//
// ClassName:   G4PenelopeQEDBuilder
//
// Author:      V.Ivanchenko 03.05.2004
//
// Modified:    D. Perez-Loureiro 07.06.2012
//
//----------------------------------------------------------------------------
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4PenelopeQEDBuilder.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"

//#include "G4PenelopeCompton.hh"
//#include "G4PenelopeGammaConversion.hh"
//#include "G4PenelopePhotoElectric.hh"
//#include "G4PenelopeRayleigh.hh"

//#include "G4eMultipleScattering.hh"

//#include "G4PenelopeIonisation.hh"
//#include "G4PenelopeBremsstrahlung.hh"
//#include "G4PenelopeAnnihilation.hh"

#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"

#include "G4PhysicsListHelper.hh"

// gamma
#include "G4PhotoElectricEffect.hh"
#include "G4PenelopePhotoElectricModel.hh"

#include "G4ComptonScattering.hh"
#include "G4PenelopeComptonModel.hh"

#include "G4GammaConversion.hh"
#include "G4PenelopeGammaConversionModel.hh"

#include "G4RayleighScattering.hh" 
#include "G4PenelopeRayleighModel.hh"

// e-
#include "G4eMultipleScattering.hh"
#include "G4UniversalFluctuation.hh"

#include "G4eIonisation.hh"
#include "G4PenelopeIonisationModel.hh"

#include "G4eBremsstrahlung.hh"
#include "G4PenelopeBremsstrahlungModel.hh"

// e+ only

#include "G4eplusAnnihilation.hh"
#include "G4PenelopeAnnihilationModel.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4PenelopeQEDBuilder::G4PenelopeQEDBuilder(const G4String& name)
   :  G4VPhysicsConstructor(name)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4PenelopeQEDBuilder::~G4PenelopeQEDBuilder()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4PenelopeQEDBuilder::ConstructParticle()
{
  G4Gamma::Gamma();
  G4Electron::Electron();
  G4Positron::Positron();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4PenelopeQEDBuilder::ConstructProcess()
{

  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
  
  // Add standard EM Processes for gamma
  G4ParticleDefinition* particle = G4Gamma::Gamma();

  //Photoelectric
  G4PhotoElectricEffect* thePhotoElectricEffect = new G4PhotoElectricEffect();
  thePhotoElectricEffect->SetModel(new G4PenelopePhotoElectricModel());
  ph->RegisterProcess(thePhotoElectricEffect, particle);
  
  
  //Compton scattering
  G4ComptonScattering* theComptonScattering = new G4ComptonScattering();
  theComptonScattering->SetModel(new G4PenelopeComptonModel());
  ph->RegisterProcess(theComptonScattering, particle);
  
  //Gamma conversion  
  G4GammaConversion* theGammaConversion = new G4GammaConversion();
  theGammaConversion->SetModel(new G4PenelopeGammaConversionModel());
  ph->RegisterProcess(theGammaConversion, particle); 

  //Rayleigh scattering
  G4RayleighScattering* theRayleigh = new G4RayleighScattering();
  theRayleigh->SetModel(new G4PenelopeRayleighModel());
  ph->RegisterProcess(theRayleigh, particle);
  
  
  // Add standard EM Processes for e-
  particle = G4Electron::Electron();
  
  //Multiple Scattering
  G4eMultipleScattering* msc = new G4eMultipleScattering();
  ph->RegisterProcess(msc, particle);
  
  //Ionization
  G4eIonisation* eIoni = new G4eIonisation();
  eIoni->SetEmModel(new G4PenelopeIonisationModel());
  eIoni->SetFluctModel(new G4UniversalFluctuation() );
  ph->RegisterProcess(eIoni, particle);

  //Bremsstrahlung
  G4eBremsstrahlung* eBrem = new G4eBremsstrahlung();
  eBrem->SetEmModel(new G4PenelopeBremsstrahlungModel());
  ph->RegisterProcess(eBrem, particle);
  

  // Add standard EM Processes for e+
  particle = G4Positron::Positron();
    
  //MultipleScattering
  G4eMultipleScattering* msc2 = new G4eMultipleScattering();
  ph->RegisterProcess(msc2, particle); 
  
  // Ionisation
  G4eIonisation* eIoni2 = new G4eIonisation();
  eIoni->SetEmModel(new G4PenelopeIonisationModel());
  eIoni->SetFluctModel(new G4UniversalFluctuation() );
  ph->RegisterProcess(eIoni2, particle);
  
  // Bremsstrahlung
  G4eBremsstrahlung* eBrem2 = new G4eBremsstrahlung();
  eBrem->SetEmModel(new G4PenelopeBremsstrahlungModel());
  ph->RegisterProcess(eBrem2, particle);
  
  //Annihilation
  G4eplusAnnihilation* eAnni = new G4eplusAnnihilation();
  eAnni->SetModel(new G4PenelopeAnnihilationModel());
  ph->RegisterProcess(eAnni, particle);
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

