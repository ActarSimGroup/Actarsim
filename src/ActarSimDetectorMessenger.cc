/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol hapolyo@usc.es
//*-- Date: 11/2004
//*-- Last Update: 04/04/06
// --------------------------------------------------------------
// Description:
//   Messenger for the detector construction
//
// --------------------------------------------------------------
// Comments:
//   - 03/12/04: based on example/novice/N03 structure
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include "ActarSimDetectorMessenger.hh"
#include "ActarSimDetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWithoutParameter.hh"


ActarSimDetectorMessenger::
ActarSimDetectorMessenger(ActarSimDetectorConstruction* ActarSimDet)
  :ActarSimDetector(ActarSimDet) {
  //
  // Constructor with fully functionality
  //
  ActarSimDir = new G4UIdirectory("/ActarSim/");
  ActarSimDir->SetGuidance("UI commands of ActarSim program");

  detDir = new G4UIdirectory("/ActarSim/det/");
  detDir->SetGuidance("Detector control");  

  gasGeoIncludedFlagCmd = new G4UIcmdWithAString("/ActarSim/det/gasGeoIncludedFlag",this);
  gasGeoIncludedFlagCmd->SetGuidance("Includes the geometry of the gas volume in the simulation (default off).");
  gasGeoIncludedFlagCmd->SetGuidance("  Choice : on, off(default)");
  gasGeoIncludedFlagCmd->SetParameterName("choice",true);
  gasGeoIncludedFlagCmd->SetDefaultValue("off");
  gasGeoIncludedFlagCmd->SetCandidates("on off");
  gasGeoIncludedFlagCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  silGeoIncludedFlagCmd = new G4UIcmdWithAString("/ActarSim/det/silGeoIncludedFlag",this);
  silGeoIncludedFlagCmd->SetGuidance("Includes the geometry of the silicons in the simulation (default off).");
  silGeoIncludedFlagCmd->SetGuidance("  Choice : on, off(default)");
  silGeoIncludedFlagCmd->SetParameterName("choice",true);
  silGeoIncludedFlagCmd->SetDefaultValue("off");
  silGeoIncludedFlagCmd->SetCandidates("on off");
  silGeoIncludedFlagCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  sciGeoIncludedFlagCmd = new G4UIcmdWithAString("/ActarSim/det/sciGeoIncludedFlag",this);
  sciGeoIncludedFlagCmd->SetGuidance("Includes the geometry of the scintillator in the simulation (default off).");
  sciGeoIncludedFlagCmd->SetGuidance("  Choice : on, off(default)");
  sciGeoIncludedFlagCmd->SetParameterName("choice",true);
  sciGeoIncludedFlagCmd->SetDefaultValue("off");
  sciGeoIncludedFlagCmd->SetCandidates("on off");
  sciGeoIncludedFlagCmd->AvailableForStates(G4State_PreInit,G4State_Idle);


  plateIncludedFlagCmd = new G4UIcmdWithAString("/ActarSim/det/plateIncludedFlag",this); 
  plateIncludedFlagCmd->SetGuidance("Includes the Al plate in the simulation (default off).");
  plateIncludedFlagCmd->SetGuidance("  Choice : on, off(default)");
  plateIncludedFlagCmd->SetParameterName("choice",true);
  plateIncludedFlagCmd->SetDefaultValue("off");
  plateIncludedFlagCmd->SetCandidates("on off");
  plateIncludedFlagCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  plateOrientationCmd = new G4UIcmdWithAString("/ActarSim/det/slitAngle",this); 
  plateOrientationCmd->SetGuidance("Sets the position of the slits in the Al plate (default horizontal).");
  plateOrientationCmd->SetGuidance("  Choice : vertical, horizontal(default)");
  plateOrientationCmd->SetParameterName("choice",true);
  plateOrientationCmd->SetDefaultValue("horizontal");
  plateOrientationCmd->SetCandidates("vertical  horizontal");
  plateOrientationCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  mediumMaterCmd = new G4UIcmdWithAString("/ActarSim/det/setMediumMat",this);
  mediumMaterCmd->SetGuidance("Select Material of the Medium.");
  mediumMaterCmd->SetParameterName("mediumMat",false);
  mediumMaterCmd->SetDefaultValue("Air");
  mediumMaterCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  eleFieldCmd = new G4UIcmdWith3Vector("/ActarSim/det/setEleField",this);
  eleFieldCmd->SetGuidance("Define electric field.");
  eleFieldCmd->SetGuidance("Usage: /ActarSim/det/setEleField  Ex  Ey  Ez  (in MV/mm)");
  eleFieldCmd->SetGuidance("Use the command  update  after setting a field.");
  eleFieldCmd->SetParameterName("Ex","Ey","Ez",false,false);
  //eleFieldCmd->SetDefaultUnit("kV/cm");
  //eleFieldCmd->SetUnitCategory("Electric field");
  eleFieldCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  magFieldCmd = new G4UIcmdWith3VectorAndUnit("/ActarSim/det/setMagField",this);
  magFieldCmd->SetGuidance("Define magnetic field.");
  magFieldCmd->SetGuidance("Usage: /ActarSim/det/setMagField  Bx  By  Bz  unit");
  magFieldCmd->SetGuidance("Use the command  update  after setting a field.");
  magFieldCmd->SetParameterName("Bx","By","Bz",false,false);
  magFieldCmd->SetUnitCategory("Magnetic flux density");
  magFieldCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  updateCmd = new G4UIcmdWithoutParameter("/ActarSim/det/update",this);
  updateCmd->SetGuidance("Update geometry.");
  updateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
  updateCmd->SetGuidance("if you changed geometrical value(s).");
  updateCmd->AvailableForStates(G4State_Idle);

  printCmd = new G4UIcmdWithoutParameter("/ActarSim/det/print",this);
  printCmd->SetGuidance("Prints geometry.");
  printCmd->AvailableForStates(G4State_Idle);

  slitPositionCmd = new G4UIcmdWith3VectorAndUnit("/ActarSim/det/slitPos",this);
  slitPositionCmd->SetGuidance("Define slit position");
  slitPositionCmd->SetGuidance("Usage /ActarSim/det/slitPos X Y Z unit");
  slitPositionCmd->SetParameterName("X","Y","Z",true,true);
  slitPositionCmd->SetDefaultUnit("cm");

  platePositionCmd = new G4UIcmdWith3VectorAndUnit("/ActarSim/det/platePos",this);
  platePositionCmd->SetGuidance("Define plate position");
  platePositionCmd->SetGuidance("Usage /ActarSim/det/platePos X Y Z unit");
  platePositionCmd->SetParameterName("X","Y","Z",true,true);
  platePositionCmd->SetDefaultUnit("mm");
}


ActarSimDetectorMessenger::~ActarSimDetectorMessenger() {
  //
  // Destructor
  //
  delete ActarSimDir;
  delete detDir;
  delete gasGeoIncludedFlagCmd;
  delete silGeoIncludedFlagCmd;
  delete sciGeoIncludedFlagCmd;
  delete mediumMaterCmd;
  delete eleFieldCmd;
  delete magFieldCmd;
  delete updateCmd;
  delete printCmd;
  delete slitPositionCmd;
  delete platePositionCmd;
}


void ActarSimDetectorMessenger::SetNewValue(G4UIcommand* command,
					    G4String newValue) {
  //
  // Setting the new values and connecting to detector constructor
  //

  if( command == gasGeoIncludedFlagCmd )
    ActarSimDetector->SetGasGeoIncludedFlag(newValue);

  if( command == silGeoIncludedFlagCmd )
    ActarSimDetector->SetSilGeoIncludedFlag(newValue);

  if( command == sciGeoIncludedFlagCmd )
    ActarSimDetector->SetSciGeoIncludedFlag(newValue);

  if(command == plateIncludedFlagCmd)
    ActarSimDetector->SetPlateIncludedFlag(newValue);

  if(command == mediumMaterCmd)
  {
    ActarSimDetector->SetMediumMaterial(newValue);
  }

  if(command == plateOrientationCmd)
  {
    ActarSimDetector->SetSlitOrientation(newValue);
  }

  if( command == eleFieldCmd )
    {
		ActarSimDetector->SetEleField(eleFieldCmd->GetNew3VectorValue(newValue));
	}

  if( command == magFieldCmd )
    {
		ActarSimDetector->SetMagField(magFieldCmd->GetNew3VectorValue(newValue));
	}

  if( command == updateCmd ) {
    ActarSimDetector->UpdateGeometry();
    ActarSimDetector->UpdateEMField();
    ActarSimDetector->PrintDetectorParameters();
  }

  if( command == printCmd )
    {
		ActarSimDetector->PrintDetectorParameters();
	}

  if( command == slitPositionCmd)
    ActarSimDetector->SetSlitPosition(slitPositionCmd->GetNew3VectorValue(newValue));


  if( command == platePositionCmd)
    ActarSimDetector->SetPlatePosition(platePositionCmd->GetNew3VectorValue(newValue));
}
