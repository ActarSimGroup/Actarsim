/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol
//*-- Date: 11/2004
//*-- Last Update: 1/12/14
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

  MaikoGeoIncludedFlagCmd = new G4UIcmdWithAString("/ActarSim/det/MaikoGeoIncludedFlag",this);
  MaikoGeoIncludedFlagCmd->SetGuidance("Includes the Maiko geometry in the simulation (default off).");
  MaikoGeoIncludedFlagCmd->SetGuidance("  Choice : on, off(default)");
  MaikoGeoIncludedFlagCmd->SetParameterName("choice",true);
  MaikoGeoIncludedFlagCmd->SetDefaultValue("off");
  MaikoGeoIncludedFlagCmd->SetCandidates("on off");
  MaikoGeoIncludedFlagCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  xGasChamberCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/setXLengthGasChamber",this);
  xGasChamberCmd->SetGuidance("Select the half-length X dimension of the Gas Chamber.");
  xGasChamberCmd->SetParameterName("xGasChamber",false);
  xGasChamberCmd->SetRange("xGasChamber>=0.");
  xGasChamberCmd->SetUnitCategory("Length");
  xGasChamberCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  yGasChamberCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/setYLengthGasChamber",this);
  yGasChamberCmd->SetGuidance("Select the half-length Y dimension of the Gas Chamber.");
  yGasChamberCmd->SetParameterName("yGasChamber",false);
  yGasChamberCmd->SetRange("yGasChamber>=0.");
  yGasChamberCmd->SetUnitCategory("Length");
  yGasChamberCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  zGasChamberCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/setZLengthGasChamber",this);
  zGasChamberCmd->SetGuidance("Select the half-length Z dimension of the Gas Chamber.");
  zGasChamberCmd->SetParameterName("zGasChamber",false);
  zGasChamberCmd->SetRange("zGasChamber>=0.");
  zGasChamberCmd->SetUnitCategory("Length");
  zGasChamberCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

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

  mediumMaterCmd = new G4UIcmdWithAString("/ActarSim/det/setMediumMat",this);
  //mediumMaterCmd->SetGuidance("Select Material of the Medium.");
  mediumMaterCmd->SetGuidance("Select Material outside the Chamber.");
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

}


ActarSimDetectorMessenger::~ActarSimDetectorMessenger() {
  //
  // Destructor
  //
  delete ActarSimDir;
  delete detDir;
  delete MaikoGeoIncludedFlagCmd;
  delete xGasChamberCmd;
  delete yGasChamberCmd;
  delete zGasChamberCmd;
  delete gasGeoIncludedFlagCmd;
  delete silGeoIncludedFlagCmd;
  delete sciGeoIncludedFlagCmd;
  delete mediumMaterCmd;
  delete eleFieldCmd;
  delete magFieldCmd;
  delete updateCmd;
  delete printCmd;
}


void ActarSimDetectorMessenger::SetNewValue(G4UIcommand* command,
					    G4String newValue) {
  //
  // Setting the new values and connecting to detector constructor
  //

  if( command == MaikoGeoIncludedFlagCmd )
    ActarSimDetector->SetMaikoGeoIncludedFlag(newValue);

  if(command == xGasChamberCmd)
  {
    ActarSimDetector->SetXGasChamber(xGasChamberCmd->GetNewDoubleValue(newValue));
  }

  if(command == yGasChamberCmd)
  {
    ActarSimDetector->SetYGasChamber(yGasChamberCmd->GetNewDoubleValue(newValue));
  }

  if(command == zGasChamberCmd)
  {
    ActarSimDetector->SetZGasChamber(zGasChamberCmd->GetNewDoubleValue(newValue));
  }

  if( command == gasGeoIncludedFlagCmd )
    ActarSimDetector->SetGasGeoIncludedFlag(newValue);

  if( command == silGeoIncludedFlagCmd )
    ActarSimDetector->SetSilGeoIncludedFlag(newValue);

  if( command == sciGeoIncludedFlagCmd )
    ActarSimDetector->SetSciGeoIncludedFlag(newValue);

  if(command == mediumMaterCmd)
  {
    ActarSimDetector->SetMediumMaterial(newValue);
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

}
