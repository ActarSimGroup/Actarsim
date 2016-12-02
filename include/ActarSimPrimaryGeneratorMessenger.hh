// - AUTHOR: Hector Alvarez-Pol 11/2004
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimPrimaryGeneratorMessenger_h
#define ActarSimPrimaryGeneratorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class ActarSimPrimaryGeneratorAction;
class G4ParticleTable;
class G4IonTable;
class G4UIdirectory;
class G4UIcommand;
class G4UIcmdWithoutParameter;
class G4UIcmdWith3Vector;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithAnInteger;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;

class ActarSimPrimaryGeneratorMessenger: public G4UImessenger {
private:
  ActarSimPrimaryGeneratorAction* actarSimActionGun; ///< Pointer to main primary generator class
  G4ParticleTable* particleTable;                    ///< Pointer to the global particle table
  G4IonTable* ionTable;                              ///< Pointer to the global particle table

  G4UIdirectory*               gunDir;                 ///< Directory for gun commands
  G4UIdirectory*               CineDir;                ///< Directory for CINE commands
  G4UIcmdWithoutParameter*     listCmd;                ///< List available particles.
  G4UIcmdWithAString*          particleCmd;            ///< Select the incident particle.
  G4UIcmdWithAString*          realisticBeamCmd;       ///< Simulates beam emittance according to emittance parameters.
  G4UIcmdWithAString*          beamInteractionCmd;     ///< Simulates the beam energy loss in gas.
  G4UIcmdWithAString*          reactionFromFileCmd;    ///< Select a reaction from an input file
  G4UIcmdWithAString*          reactionFromCrossSectionCmd; ///< DO NOT USE. Simulates beam/target from the cross-sections. DO NOT USE.
  G4UIcmdWithAString*          reactionFromEvGenCmd;   ///< DO NOT USE. Simulates beam/target from event generator. DO NOT USE.
  G4UIcmdWithAString*          reactionFromCineCmd;    ///< Select a reaction using Cine
  G4UIcmdWithAString*          reactionFileCmd;        ///< Select the reaction definition file.
  G4UIcmdWithAString*          randomThetaCmd;         ///< Select a random Theta angle for the scattered particle.
  G4UIcmdWithAString*          randomPhiCmd;           ///< Select a random Phi angle for the scattered particle.
  G4UIcmdWithAString*          alphaSourceCmd;         ///< NOT VALIDATED. CHECK THIS COMMAND!
  G4UIcmdWithAString*          CinerandomThetaCmd;     ///< Select a random Theta angle for the scattered particle.
  G4UIcommand*                 randomThetaValCmd;      ///< Sets the limits in the Theta angle for the scattered particle.
  G4UIcommand*                 randomPhiValCmd;        ///< Sets the limits in the Phi angle for the scattered particle.
  G4UIcommand*                 CinerandomThetaValCmd;  ///< Sets the limist in the Theta angle for the scattered particle.
  G4UIcmdWithADoubleAndUnit*   energyCmd;              ///< Sets the kinetic energy of the primary particle
  G4UIcmdWith3Vector*          directionCmd;           ///< Set momentum direction.
  G4UIcmdWithADoubleAndUnit*   beamThetaCmd;           ///< Sets theta angle for beam (in degrees)
  G4UIcmdWithADoubleAndUnit*   beamPhiCmd;             ///< Sets phi angle for beam (in degrees)
  G4UIcmdWith3VectorAndUnit*   positionCmd;            ///< Set starting position of the particle.
  G4UIcmdWithADoubleAndUnit*   timeCmd;                ///< Set initial time of the particle.
  G4UIcmdWithAString*          randomVertexZPositionCmd;      ///< Randomize the reaction vertex Z position
  G4UIcommand*                 randomVertexZPositionRangeCmd; ///< Set the min and max Z-value of random vertex position
  G4UIcmdWithADoubleAndUnit*   vertexZPositionCmd;     ///< Set the Z-value of the reaction vertex.
  G4UIcmdWith3Vector*          polCmd;                 ///< Set polarization.
  G4UIcmdWithAnInteger*        numberCmd;              ///< Set number of particles to be generated in a single event

  G4UIcmdWithADouble*          emittanceCmd;           ///< Selects the value of the emittance [in mm mrad].
  G4UIcmdWith3Vector*          beamDirectionCmd;       ///< Set beam momentum direction.
  G4UIcmdWith3VectorAndUnit*   beamPositionCmd;        ///< Set beam starting position.
  G4UIcmdWithADoubleAndUnit*   beamRadiusAtEntranceCmd; ///< Selects the beam radius at entrance of ACTAR.

  G4UIdirectory*               KineDir;                ///< Directory for CINE commands
  G4UIcmdWithAString*          reactionFromKineCmd;    ///< Select a reaction using Kine
  G4UIcmdWithAString*          KineRandomThetaCmd;     ///< Randomize Theta_CM of outgoing particles
  G4UIcommand*                 KineRandomThetaRangeCmd; ///< Sets the limits in the Theta angle for the scattered particle.
  G4UIcmdWithAString*          KineRandomPhiAngleCmd;  ///< Randomize Lab Phi angles of out-going particles
  G4UIcommand*                 KineIncidentIonCmd;     ///< Set properties of incident ion to be generated.
  G4UIcommand*                 KineTargetIonCmd;       ///< Set properties of target ion to be generated.
  G4UIcommand*                 KineScatteredIonCmd;    ///< Set properties of scattered ion to be generated.
  G4UIcommand*                 KineRecoilIonCmd;       ///< Set properties of recoil ion to be generated.
  G4UIcmdWithADoubleAndUnit*   KineLabEnergyCmd;       ///< Sets the laboratory energy.
  G4UIcmdWithADoubleAndUnit*   KineUserThetaCMCmd;     ///< Sets theta CM angle for scattered particle (in degrees)
  G4UIcmdWithADoubleAndUnit*   KineUserPhiAngleCmd;    ///< User set phi angle for outgoing particle in the Lab system (in degrees)
  G4UIcmdWith3VectorAndUnit*   vertexPositionCmd;      ///< Set the position of the vertex.

  G4UIcommand* incidentIonCmd;               ///< Set properties of incident ion to be generated.
  G4UIcommand* targetIonCmd;                 ///< Set properties of target ion to be generated.
  G4UIcommand* scatteredIonCmd;              ///< Set properties of scattered ion to be generated.
  G4UIcommand* recoilIonCmd;                 ///< Set properties of recoil ion to be generated.

  G4UIcmdWithADoubleAndUnit* reactionQCmd;       ///< Sets the reaction Q
  G4UIcmdWithADoubleAndUnit* labEnergyCmd;       ///< Sets the laboratory energy
  G4UIcmdWithADoubleAndUnit* thetaLabAngleCmd;   ///< Sets theta lab angle for the scattered particle (degrees)

  G4UIcommand*                 ionCmd;          ///< Set properties of ion to be generated.

  // for ion shooting
  G4bool   fShootIon;                 ///< Internal variables for ion creation
  G4int    fAtomicNumber;             ///< Internal variables for ion creation: atomic number
  G4int    fAtomicMass;               ///< Internal variables for ion creation: atomic mass
  G4int    fIonCharge;                ///< Internal variables for ion creation: ion charge
  G4double fIonExciteEnergy;          ///< Internal variables for ion creation: ion excitation energy

  // for Kine command
  G4double fIonMass;                 ///< Internal variables for ion creation: ion mass in atomic mass unit u

  void IonCommand(G4String newValues);
  void incidentIonCommand(G4String newValues);
  void targetIonCommand(G4String newValues);
  void scatteredIonCommand(G4String newValues);
  void recoilIonCommand(G4String newValues);
  void KineIncidentIonCommand(G4String newValues);
  void KineTargetIonCommand(G4String newValues);
  void KineScatteredIonCommand(G4String newValues);
  void KineRecoilIonCommand(G4String newValues);

public:
  ActarSimPrimaryGeneratorMessenger(ActarSimPrimaryGeneratorAction*);
  ~ActarSimPrimaryGeneratorMessenger();

  void SetNewValue(G4UIcommand*, G4String);
  G4String GetCurrentValue(G4UIcommand * command);
};
#endif
