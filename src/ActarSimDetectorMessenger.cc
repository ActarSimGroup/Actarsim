/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol
//*-- Date: 11/2004
//*-- Last Update: 10/01/16
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

  ACTARTPCDEMOGeoIncludedFlagCmd = new G4UIcmdWithAString("/ActarSim/det/ACTARTPCDEMOGeoIncludedFlag",this);
  ACTARTPCDEMOGeoIncludedFlagCmd->SetGuidance("Includes the ACTARTPC Demonstrator geometry in the simulation (default off).");
  ACTARTPCDEMOGeoIncludedFlagCmd->SetGuidance("  Choice : on, off(default)");
  ACTARTPCDEMOGeoIncludedFlagCmd->SetParameterName("choice",true);
  ACTARTPCDEMOGeoIncludedFlagCmd->SetDefaultValue("off");
  ACTARTPCDEMOGeoIncludedFlagCmd->SetCandidates("on off");
  ACTARTPCDEMOGeoIncludedFlagCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  ACTARTPCGeoIncludedFlagCmd = new G4UIcmdWithAString("/ActarSim/det/ACTARTPCGeoIncludedFlag",this);
  ACTARTPCGeoIncludedFlagCmd->SetGuidance("Includes the ACTARTPC geometry in the simulation (default off).");
  ACTARTPCGeoIncludedFlagCmd->SetGuidance("  Choice : on, off(default)");
  ACTARTPCGeoIncludedFlagCmd->SetParameterName("choice",true);
  ACTARTPCGeoIncludedFlagCmd->SetDefaultValue("off");
  ACTARTPCGeoIncludedFlagCmd->SetCandidates("on off");
  ACTARTPCGeoIncludedFlagCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  worldSizeXCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/setWorldSizeX",this);
  worldSizeXCmd->SetGuidance("Select the half-length X dimension of the World.");
  worldSizeXCmd->SetParameterName("xWorld",false);
  worldSizeXCmd->SetRange("xWorld>=0.");
  worldSizeXCmd->SetUnitCategory("Length");
  worldSizeXCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  worldSizeYCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/setWorldSizeY",this);
  worldSizeYCmd->SetGuidance("Select the half-length Y dimension of the World.");
  worldSizeYCmd->SetParameterName("yWorld",false);
  worldSizeYCmd->SetRange("yWorld>=0.");
  worldSizeYCmd->SetUnitCategory("Length");
  worldSizeYCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  worldSizeZCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/setWorldSizeZ",this);
  worldSizeZCmd->SetGuidance("Select the half-length Z dimension of the World.");
  worldSizeZCmd->SetParameterName("zWorld",false);
  worldSizeZCmd->SetRange("zWorld>=0.");
  worldSizeZCmd->SetUnitCategory("Length");
  worldSizeZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  chamberSizeXCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/setXLengthGasChamber",this);
  chamberSizeXCmd->SetGuidance("Select the half-length X dimension of the Gas Chamber.");
  chamberSizeXCmd->SetParameterName("xGasChamber",false);
  chamberSizeXCmd->SetRange("xGasChamber>=0.");
  chamberSizeXCmd->SetUnitCategory("Length");
  chamberSizeXCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  chamberSizeYCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/setYLengthGasChamber",this);
  chamberSizeYCmd->SetGuidance("Select the half-length Y dimension of the Gas Chamber.");
  chamberSizeYCmd->SetParameterName("yGasChamber",false);
  chamberSizeYCmd->SetRange("yGasChamber>=0.");
  chamberSizeYCmd->SetUnitCategory("Length");
  chamberSizeYCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  chamberSizeZCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/setZLengthGasChamber",this);
  chamberSizeZCmd->SetGuidance("Select the half-length Z dimension of the Gas Chamber.");
  chamberSizeZCmd->SetParameterName("zGasChamber",false);
  chamberSizeZCmd->SetRange("zGasChamber>=0.");
  chamberSizeZCmd->SetUnitCategory("Length");
  chamberSizeZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  chamberCenterXCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/setXCenterChamber",this);
  chamberCenterXCmd->SetGuidance("Select the X offset of the Chamber center.");
  chamberCenterXCmd->SetParameterName("chamberCenterX",false);
  chamberCenterXCmd->SetUnitCategory("Length");
  chamberCenterXCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  chamberCenterYCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/setYCenterChamber",this);
  chamberCenterYCmd->SetGuidance("Select the Y offset of the Chamber center.");
  chamberCenterYCmd->SetParameterName("chamberCenterY",false);
  chamberCenterYCmd->SetUnitCategory("Length");
  chamberCenterYCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  chamberCenterZCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/setZCenterChamber",this);
  chamberCenterZCmd->SetGuidance("Select the Z offset of the Chamber center.");
  chamberCenterZCmd->SetParameterName("chamberCenterZ",false);
  chamberCenterZCmd->SetUnitCategory("Length");
  chamberCenterZCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  gasGeoIncludedFlagCmd = new G4UIcmdWithAString("/ActarSim/det/gasGeoIncludedFlag",this);
  gasGeoIncludedFlagCmd->SetGuidance("Includes the geometry of the gas volume in the simulation (default on).");
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
  
  AT_ProjectSwitchCmd = new G4UIcmdWithAString("/ActarSim/det/AT_ProjectSwitch",this);
  AT_ProjectSwitchCmd->SetGuidance("Select the main geometry features according to the project (default off).");
  AT_ProjectSwitchCmd->SetGuidance("Choice : Actar_TPC (default), SpecMAT");
  AT_ProjectSwitchCmd->SetParameterName("AT_ProjectSwitch",false);
  AT_ProjectSwitchCmd->SetDefaultValue("Actar_TPC");
  AT_ProjectSwitchCmd->SetCandidates("Actar_TPC SpecMAT");
  AT_ProjectSwitchCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  mediumMaterialCmd = new G4UIcmdWithAString("/ActarSim/det/setMediumMat",this);
  mediumMaterialCmd->SetGuidance("Select Material outside the Chamber.");
  mediumMaterialCmd->SetParameterName("mediumMat",false);
  mediumMaterialCmd->SetDefaultValue("Air");
  mediumMaterialCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  chamberMaterialCmd = new G4UIcmdWithAString("/ActarSim/det/setChamberMat",this);
  chamberMaterialCmd->SetGuidance("Select Material in the Chamber (but not in the gas!).");
  chamberMaterialCmd->SetParameterName("chamberMat",false);
  chamberMaterialCmd->SetDefaultValue("Air");
  chamberMaterialCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

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
  delete ACTARTPCDEMOGeoIncludedFlagCmd;
  delete ACTARTPCGeoIncludedFlagCmd;
  delete worldSizeXCmd;
  delete worldSizeYCmd;
  delete worldSizeZCmd;
  delete chamberSizeXCmd;
  delete chamberSizeYCmd;
  delete chamberSizeZCmd;
  delete chamberCenterXCmd;
  delete chamberCenterYCmd;
  delete chamberCenterZCmd;
  delete gasGeoIncludedFlagCmd;
  delete silGeoIncludedFlagCmd;
  delete sciGeoIncludedFlagCmd;
  delete AT_ProjectSwitchCmd;
  delete mediumMaterialCmd;
  delete chamberMaterialCmd;
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

  if( command == ACTARTPCDEMOGeoIncludedFlagCmd )
    ActarSimDetector->SetACTARTPCDEMOGeoIncludedFlag(newValue);

  if( command == ACTARTPCGeoIncludedFlagCmd )
    ActarSimDetector->SetACTARTPCGeoIncludedFlag(newValue);

  if(command == worldSizeXCmd)
    ActarSimDetector->SetWorldSizeX(worldSizeXCmd->GetNewDoubleValue(newValue));

  if(command == worldSizeYCmd)
  ActarSimDetector->SetWorldSizeY(worldSizeYCmd->GetNewDoubleValue(newValue));

  if(command == worldSizeZCmd)
    ActarSimDetector->SetWorldSizeZ(worldSizeZCmd->GetNewDoubleValue(newValue));

  if(command == chamberSizeXCmd)
    ActarSimDetector->SetChamberSizeX(chamberSizeXCmd->GetNewDoubleValue(newValue));

  if(command == chamberSizeYCmd)
    ActarSimDetector->SetChamberSizeY(chamberSizeYCmd->GetNewDoubleValue(newValue));

  if(command == chamberSizeZCmd)
    ActarSimDetector->SetChamberSizeZ(chamberSizeZCmd->GetNewDoubleValue(newValue));

//HAPOL NOW!!! TODO CORRECT IT! JUST TESTING
  //if(command == chamberCenterXCmd) {
  //  ActarSimGasDetector->chamberCenterXCmd(gasBoxSizeXCmd->GetNewDoubleValue(newValue));
  //}





  if( command == gasGeoIncludedFlagCmd )
    ActarSimDetector->SetGasGeoIncludedFlag(newValue);

  if( command == silGeoIncludedFlagCmd )
    ActarSimDetector->SetSilGeoIncludedFlag(newValue);

  if( command == sciGeoIncludedFlagCmd )
    ActarSimDetector->SetSciGeoIncludedFlag(newValue);

  if(command == AT_ProjectSwitchCmd){
    
	if (newValue == "Actar_TPC") ActarSimDetector->SetAT_Project(1);
	if (newValue == "SpecMAT") ActarSimDetector->SetAT_Project(2);
	
  }


  if(command == mediumMaterialCmd)
    ActarSimDetector->SetMediumMaterial(newValue);

  if(command == chamberMaterialCmd)
    ActarSimDetector->SetChamberMaterial(newValue);

  if( command == eleFieldCmd )
		ActarSimDetector->SetEleField(eleFieldCmd->GetNew3VectorValue(newValue));

  if( command == magFieldCmd )
		ActarSimDetector->SetMagField(magFieldCmd->GetNew3VectorValue(newValue));

  if( command == updateCmd ) {
    ActarSimDetector->UpdateGeometry();
    ActarSimDetector->UpdateEMField();
    ActarSimDetector->PrintDetectorParameters();
  }

  if( command == printCmd )
		ActarSimDetector->PrintDetectorParameters();
}
