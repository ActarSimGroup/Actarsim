 ///////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez Pol
//*-- Date: 04/2008
//*-- Last Update: 15/12/14
// --------------------------------------------------------------
// Description:
//   Messenger of the Gas ACTAR detector
//
// --------------------------------------------------------------
// Comments:
//
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include "ActarSimGasDetectorMessenger.hh"

#include "G4RunManager.hh"

#include "ActarSimDetectorConstruction.hh"
#include "ActarSimGasDetectorConstruction.hh"
#include "ActarSimPrimaryGeneratorAction.hh"

#include "G4UIcommand.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4Tokenizer.hh"


ActarSimGasDetectorMessenger::
ActarSimGasDetectorMessenger(ActarSimDetectorConstruction* ActarSimDet,ActarSimGasDetectorConstruction* ActarSimGasDet)
  :ActarSimDetector(ActarSimDet), ActarSimGasDetector(ActarSimGasDet){ 
  //
  // Constructor with fully functionality
  //
  
  detDir = new G4UIdirectory("/ActarSim/det/gas/");
  detDir->SetGuidance("gas detector control");
 
  gasMaterCmd = new G4UIcmdWithAString("/ActarSim/det/gas/setGasMat",this);
  gasMaterCmd->SetGuidance("Select Material of the Gas (for the Gas box and the Chamber).");
  gasMaterCmd->SetParameterName("gasMat",false);
  gasMaterCmd->SetDefaultValue("D2");
  gasMaterCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  gasPresCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/gas/setGasPressure",this);
  gasPresCmd->SetGuidance("Select the Gas Pressure (for the Gas box and the Chamber).");
  gasPresCmd->SetParameterName("gasPressure",false);
  gasPresCmd->SetRange("gasPressure>=0.");
  gasPresCmd->SetUnitCategory("Pressure");
  gasPresCmd->SetDefaultUnit("bar");
  gasPresCmd->SetDefaultValue(1.01325);
  gasPresCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
 
  gasTempCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/gas/setGasTemperature",this);
  gasTempCmd->SetGuidance("Select the Gas Temperature (for the Gas box and the Chamber).");
  gasTempCmd->SetParameterName("gasTemperature",false);
  gasTempCmd->SetRange("gasTemperature>=0.");
  gasTempCmd->SetUnitCategory("Temperature");
  gasTempCmd->SetDefaultUnit("kelvin");
  gasTempCmd->SetDefaultValue(293.15);
  gasTempCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  beamShieldMaterCmd = new G4UIcmdWithAString("/ActarSim/det/gas/setBeamShieldMat",this);
  beamShieldMaterCmd->SetGuidance("Select Material of the beam shield.");
  beamShieldMaterCmd->SetParameterName("gasMat",false);
  beamShieldMaterCmd->SetDefaultValue("iC4H10");
  beamShieldMaterCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  detectorGeometryCmd = new G4UIcmdWithAString("/ActarSim/det/gas/setDetectorGeometry",this);
  detectorGeometryCmd->SetGuidance("Select the geometry of the detector.");
  detectorGeometryCmd->SetGuidance("Choice : box(default), tube.");
  detectorGeometryCmd->SetParameterName("choice",false);
  detectorGeometryCmd->SetDefaultValue("box");
  detectorGeometryCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  setBeamShieldCmd = new G4UIcmdWithAString("/ActarSim/det/gas/setBeamShield",this);
  setBeamShieldCmd->SetGuidance("Sets a beam shield and selects the geometry.");
  setBeamShieldCmd->SetGuidance("Choice : tube, off.");
  setBeamShieldCmd->SetParameterName("choice",false);
  setBeamShieldCmd->SetDefaultValue("tube");
  setBeamShieldCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  xGasBoxCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/gas/setXLengthGasBox",this);
  xGasBoxCmd->SetGuidance("Select the half-length X dimension of the Gas Box.");
  xGasBoxCmd->SetParameterName("xGasBox",false);
  xGasBoxCmd->SetRange("xGasBox>=0.");
  xGasBoxCmd->SetUnitCategory("Length");
  xGasBoxCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  yGasBoxCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/gas/setYLengthGasBox",this);
  yGasBoxCmd->SetGuidance("Select the half-length Y dimension of the Gas Box.");
  yGasBoxCmd->SetParameterName("yGasBox",false);
  yGasBoxCmd->SetRange("yGasBox>=0.");
  yGasBoxCmd->SetUnitCategory("Length");
  yGasBoxCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  zGasBoxCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/gas/setZLengthGasBox",this);
  zGasBoxCmd->SetGuidance("Select the half-length Z dimension of the Gas Box.");
  zGasBoxCmd->SetParameterName("zGasBox",false);
  zGasBoxCmd->SetRange("zGasBox>=0.");
  zGasBoxCmd->SetUnitCategory("Length");
  zGasBoxCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  radiusGasTubCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/gas/setRadiusGasTub",this);
  radiusGasTubCmd->SetGuidance("Select the external radius of the Gas Tube.");
  radiusGasTubCmd->SetParameterName("radiusGasTub",false);
  radiusGasTubCmd->SetRange("radiusGasTub>=0.");
  radiusGasTubCmd->SetUnitCategory("Length");
  radiusGasTubCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  lengthGasTubCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/gas/setLengthGasTub",this);
  lengthGasTubCmd->SetGuidance("Select the half-length of the Gas Tube.");
  lengthGasTubCmd->SetParameterName("lengthGasTub",false);
  lengthGasTubCmd->SetRange("lengthGasTub>=0.");
  lengthGasTubCmd->SetUnitCategory("Length");
  lengthGasTubCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  innerRadiusBeamShieldTubCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/gas/setInnerRadiusBeamShieldTub",this);
  innerRadiusBeamShieldTubCmd->SetGuidance("Select the external radius of the Gas Tube.");
  innerRadiusBeamShieldTubCmd->SetParameterName("innerRadiusBeamShieldTub",false);
  innerRadiusBeamShieldTubCmd->SetRange("innerRadiusBeamShieldTub>=0.");
  innerRadiusBeamShieldTubCmd->SetUnitCategory("Length");
  innerRadiusBeamShieldTubCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  outerRadiusBeamShieldTubCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/gas/setRadiusBeamShieldTub",this);
  outerRadiusBeamShieldTubCmd->SetGuidance("Select the internal radius of the Gas Tube.");
  outerRadiusBeamShieldTubCmd->SetParameterName("outerRadiusBeamShieldTub",false);
  outerRadiusBeamShieldTubCmd->SetRange("outerRadiusBeamShieldTub>=0.");
  outerRadiusBeamShieldTubCmd->SetUnitCategory("Length");
  outerRadiusBeamShieldTubCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  lengthBeamShieldTubCmd = new G4UIcmdWithADoubleAndUnit("/ActarSim/det/gas/setLengthBeamShieldTub",this);
  lengthBeamShieldTubCmd->SetGuidance("Select the half-length of the Gas Tube.");
  lengthBeamShieldTubCmd->SetParameterName("lengthBeamShieldTub",false);
  lengthBeamShieldTubCmd->SetRange("lengthBeamShieldTub>=0.");
  lengthBeamShieldTubCmd->SetUnitCategory("Length");
  lengthBeamShieldTubCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  printCmd = new G4UIcmdWithoutParameter("/ActarSim/det/gas/print",this);
  printCmd->SetGuidance("Prints geometry.");
  printCmd->AvailableForStates(G4State_Idle);

  detDirMix = new G4UIdirectory("/ActarSim/det/gas/mixture/");
  detDirMix->SetGuidance("gas mixture control");

  gasMixtureFlagCmd = new G4UIcmdWithAnInteger("/ActarSim/det/gas/mixture/GasMixture",this);
  gasMixtureFlagCmd->SetGuidance("Set a Gas Mixture with N number of elements (default 0).");
  gasMixtureFlagCmd->SetGuidance("  Choice : 0 (default) to 10");
  //gasMixtureFlagCmd->SetParameterName("GasNumber",false);
  gasMixtureFlagCmd->SetParameterName("N",true,true);
  gasMixtureFlagCmd->SetRange("N>0");
  gasMixtureFlagCmd->SetDefaultValue(2);
  //gasMixtureFlagCmd->SetCandidates("2 3 4 5 6 7 8 9 10");
  //gasMixtureFlagCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  gasMixtureCmd = new G4UIcommand("/ActarSim/det/gas/mixture/setGasMix",this);
  gasMixtureCmd->SetGuidance("Set a Gas Mixture (for the Gas box and the Chamber).");
  gasMixtureCmd->SetGuidance("[usage] /ActarSim/det/gas/setGasMix GasNum GasMat GasRatio");
  gasMixtureCmd->SetGuidance("        GasNum:(int) GasNumber (from 1 to 7)");
  gasMixtureCmd->SetGuidance("        GasMat:(string) Gas Material from the list");
  gasMixtureCmd->SetGuidance("        GasRatio:(double) Gas Ratio in Mixture (from 0 to 1)");

  G4UIparameter* gasMixtureParam;
  gasMixtureParam = new G4UIparameter("GasNum",'i',false);
  gasMixtureParam->SetDefaultValue("1");
  gasMixtureParam->SetParameterRange("GasNum>0");
  gasMixtureParam->SetParameterRange("GasNum<10");
  gasMixtureCmd->SetParameter(gasMixtureParam);
  gasMixtureParam = new G4UIparameter("GasMat",'s',false);
  gasMixtureParam->SetDefaultValue("D2");
  gasMixtureParam->SetParameterCandidates("H2 D2 He Ar CF4 iC4H10");
  gasMixtureCmd->SetParameter(gasMixtureParam);
  gasMixtureParam = new G4UIparameter("GasRatio",'d',false);
  gasMixtureParam->SetDefaultValue("0");
  gasMixtureParam->SetParameterRange("GasRatio>=0.");
  gasMixtureParam->SetParameterRange("GasRatio<=1.");
  gasMixtureCmd->SetParameter(gasMixtureParam);
}


ActarSimGasDetectorMessenger::~ActarSimGasDetectorMessenger() {
  //
  // Destructor
  //
  delete detDir;
  delete detDirMix;
  delete gasMaterCmd;
  delete gasMixtureFlagCmd;
  delete gasMixtureCmd;
  delete gasPresCmd;
  delete gasTempCmd;
  delete beamShieldMaterCmd;
  delete detectorGeometryCmd;
  delete setBeamShieldCmd;
  delete xGasBoxCmd;
  delete yGasBoxCmd;
  delete zGasBoxCmd;
  delete radiusGasTubCmd;
  delete lengthGasTubCmd;
  delete innerRadiusBeamShieldTubCmd;
  delete outerRadiusBeamShieldTubCmd;
  delete lengthBeamShieldTubCmd;
  delete printCmd;
}


void ActarSimGasDetectorMessenger::SetNewValue(G4UIcommand* command,
					    G4String newValue) {
  //
  // Setting the new values and connecting to actions
  //

  if(command == gasMaterCmd)
    {
      //ActarSimGasDetector->DefineGas();
      ActarSimGasDetector->SetGasMaterial(newValue);
      //ActarSimDetector->SetChamberMaterial(newValue);
      ActarSimDetector->UpdateGeometry();
      //ActarSimDetector->PrintDetectorParameters();
    }
  
  if(command == gasMixtureFlagCmd)
    {
      ActarSimGasDetector->SetGasMixture(gasMixtureFlagCmd->GetNewIntValue(newValue));
    }

  if(command == gasMixtureCmd)
    {
      GasMixtureCommand(newValue);
    }

  if(command == gasTempCmd)
  {
    ActarSimGasDetector->SetGasTemperature(gasTempCmd->GetNewDoubleValue(newValue));
  }

  if(command == beamShieldMaterCmd)
  {
    ActarSimGasDetector->SetBeamShieldMaterial(newValue);
  }

  if(command == detectorGeometryCmd)
  {
    ActarSimGasDetector->SetDetectorGeometry(newValue);
  }

  if(command == setBeamShieldCmd)
  {
    ActarSimGasDetector->SetBeamShieldGeometry(newValue);
  }

  if(command == xGasBoxCmd)
  {
    ActarSimGasDetector->SetXGasBox(xGasBoxCmd->GetNewDoubleValue(newValue));
  }

  if(command == yGasBoxCmd)
  {
    ActarSimGasDetector->SetYGasBox(yGasBoxCmd->GetNewDoubleValue(newValue));
    ActarSimDetector->SetYGasBoxPosition(yGasBoxCmd->GetNewDoubleValue(newValue));
  }

  if(command == zGasBoxCmd)
  {
    ActarSimGasDetector->SetZGasBox(zGasBoxCmd->GetNewDoubleValue(newValue));
    ActarSimDetector->SetZGasBoxPosition(zGasBoxCmd->GetNewDoubleValue(newValue));
  }

  if(command == radiusGasTubCmd)
  {
    ActarSimGasDetector->SetRadiusGasTub(radiusGasTubCmd->GetNewDoubleValue(newValue));
  }

  if(command == lengthGasTubCmd)
  {
    ActarSimGasDetector->SetLengthGasTub(lengthGasTubCmd->GetNewDoubleValue(newValue));
  }

  if(command == innerRadiusBeamShieldTubCmd)
  {
    ActarSimGasDetector->SetInnerRadiusBeamShieldTub(innerRadiusBeamShieldTubCmd->GetNewDoubleValue(newValue));
  }

  if(command == outerRadiusBeamShieldTubCmd)
  {
    ActarSimGasDetector->SetOuterRadiusBeamShieldTub(outerRadiusBeamShieldTubCmd->GetNewDoubleValue(newValue));
  }

  if(command == lengthBeamShieldTubCmd)
  {
    ActarSimGasDetector->SetLengthBeamShieldTub(lengthBeamShieldTubCmd->GetNewDoubleValue(newValue));
  }


  if( command == printCmd )
    { ActarSimGasDetector->PrintDetectorParameters(); }

}

void ActarSimGasDetectorMessenger::GasMixtureCommand(G4String newValues){
  //
  // Selection of the mixture gases.
  //

  G4Tokenizer next( newValues );
  // check argument
  fGasNumber = StoI(next());
  fGasMaterial = next();
  fGasRatio = StoD(next());

  //G4cout << " fGasNumber "<<fGasNumber<< " fGasMaterial "<<fGasMaterial<< " fGasRatio "<<fGasRatio<< G4endl;

  ActarSimGasDetector->SetGasMixMaterial(fGasNumber,fGasMaterial,fGasRatio);

}
