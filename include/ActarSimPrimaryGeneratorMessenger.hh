/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol
//*-- Date: 11/2004
//*-- Last Update: 16/12/14 by Hector Alvarez Pol
// --------------------------------------------------------------
// Description:
//   Messenger for the primary generator.
//
// --------------------------------------------------------------
// Comments:
//   - 03/12/04 Created based on example/novice/N03 structure
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#ifndef ActarSimPrimaryGeneratorMessenger_h
#define ActarSimPrimaryGeneratorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class ActarSimPrimaryGeneratorAction;
class G4ParticleTable;
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
  ActarSimPrimaryGeneratorAction* actarSimActionGun;
  G4ParticleTable* particleTable;

  G4UIdirectory*               gunDir;
  G4UIdirectory*               CineDir;
  G4UIcmdWithoutParameter*     listCmd;
  G4UIcmdWithAString*          particleCmd;
  G4UIcmdWithAString*          realisticBeamCmd;
  G4UIcmdWithAString*          beamInteractionCmd;
  G4UIcmdWithAString*          reactionFromFileCmd;
  G4UIcmdWithAString*          reactionFromCrossSectionCmd;
  G4UIcmdWithAString*          reactionFromEvGenCmd;
  G4UIcmdWithAString*          reactionFromCineCmd;
  G4UIcmdWithAString*          reactionFileCmd;
  G4UIcmdWithAString*          randomThetaCmd;
  G4UIcmdWithAString*          randomPhiCmd;
  G4UIcmdWithAString*          alphaSourceCmd;
  G4UIcmdWithAString*          CinerandomThetaCmd;
  G4UIcommand*                 randomThetaValCmd;
  G4UIcommand*                 randomPhiValCmd;
  G4UIcommand*                 CinerandomThetaValCmd;
  G4UIcmdWithADoubleAndUnit*   energyCmd;
  G4UIcmdWith3Vector*          directionCmd;
  G4UIcmdWith3VectorAndUnit*   positionCmd;
  G4UIcmdWithADoubleAndUnit*   timeCmd;
  G4UIcmdWithAString*          randomVertexZPositionCmd;
  G4UIcommand*                 randomVertexZPositionRangeCmd;
  G4UIcmdWithADoubleAndUnit*   vertexZPositionCmd;
  G4UIcmdWith3Vector*          polCmd;
  G4UIcmdWithAnInteger*        numberCmd;

  G4UIcmdWithADouble*          emittanceCmd;             // emittance (in mm mrad)
  G4UIcmdWith3Vector*          beamDirectionCmd;
  G4UIcmdWith3VectorAndUnit*   beamPositionCmd;
  G4UIcmdWithADoubleAndUnit*   beamRadiusAtEntranceCmd;  // emittance (in position at detector entrance)

  G4UIcmdWithAString*          reactionFromKineCmd;
  G4UIdirectory*               KineDir;
  G4UIcmdWithAString*          KineRandomThetaCmd;
  G4UIcommand*                 KineRandomThetaRangeCmd;
  G4UIcmdWithAString*          KineRandomPhiAngleCmd;
  G4UIcommand*                 KineIncidentIonCmd;
  G4UIcommand*                 KineTargetIonCmd;
  G4UIcommand*                 KineScatteredIonCmd;
  G4UIcommand*                 KineRecoilIonCmd;
  G4UIcmdWithADoubleAndUnit*   KineLabEnergyCmd;
  G4UIcmdWithADoubleAndUnit*   KineUserThetaCMCmd;
  G4UIcmdWithADoubleAndUnit*   KineUserPhiAngleCmd;
  G4UIcmdWith3VectorAndUnit*   vertexPositionCmd;

  G4UIcommand* incidentIonCmd;
  G4UIcommand* targetIonCmd;
  G4UIcommand* scatteredIonCmd;
  G4UIcommand* recoilIonCmd;

  G4UIcmdWithADoubleAndUnit* reactionQCmd;
  G4UIcmdWithADoubleAndUnit* labEnergyCmd;
  G4UIcmdWithADoubleAndUnit* thetaLabAngleCmd;

  G4UIcommand*                 ionCmd;

  // for ion shooting
  G4bool   fShootIon;
  G4int    fAtomicNumber;
  G4int    fAtomicMass;
  G4int    fIonCharge;
  G4double fIonExciteEnergy;

  // for Kine command
  G4double fIonMass; // mass in atomic mass unit u

private:
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
