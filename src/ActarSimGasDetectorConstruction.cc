/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez    hapolyo@usc.es
//*-- Date: 04/2008
//*-- Last Update: 17/05/08 by Hector Alvarez
// --------------------------------------------------------------
// Description:
//   Gas volume detector description
//
// --------------------------------------------------------------
// Comments:
//
//   - 17/04/08 Modularizing the ACTAR geometry
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include "ActarSimGasDetectorConstruction.hh"
#include "ActarSimDetectorConstruction.hh"
#include "ActarSimGasDetectorMessenger.hh"
#include "ActarSimROOTAnalysis.hh"
#include "ActarSimGasSD.hh"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4RunManager.hh"
#include "G4Transform3D.hh"

#include "globals.hh"

ActarSimGasDetectorConstruction::
ActarSimGasDetectorConstruction(ActarSimDetectorConstruction* det)
  :	detConstruction(det){
  //
  // Constructor. Sets the material and the pointer to the Messenger
  //

  SetBeamShieldMaterial("Iron");
  SetGasMaterial("isoC4H10STP");

  //Default value for the volume is a Box
  SetDetectorGeometry("box");

  //default size of GasBox (1x1x1 m3)
  xGasBox = 1 * m;
  yGasBox = 1 * m;
  zGasBox = 1 * m;

  //default size of GasTub (pi x 1 x 1 m3 )
  radiusGasTub = 1 * m;
  lengthGasTub = 1 * m;

  //default size of BeamShieldTub (pi x 1 x 1 m3 )
  innerRadiusBeamShieldTub = 0.1*m;
  outerRadiusBeamShieldTub = 0.10001*m;
  lengthBeamShieldTub = 0.95 * m;

  // create commands for interactive definition of the calorimeter
  gasMessenger = new ActarSimGasDetectorMessenger(this);
}


ActarSimGasDetectorConstruction::~ActarSimGasDetectorConstruction(){
  //
  // Destructor
  //
  delete gasMessenger;
}


G4VPhysicalVolume* ActarSimGasDetectorConstruction::Construct(G4LogicalVolume* worldLog) {
  //
  // Wrap for the construction functions within the TOF
  //

  //Introduce here other constructors for materials around the TOF (windows, frames...)
  //which can be controlled by the calMessenger
  //ConstructTOFWorld(worldLog);
  return ConstructGas(worldLog);
}



G4VPhysicalVolume* ActarSimGasDetectorConstruction::ConstructGas(G4LogicalVolume* worldLog) {
  //
  //  Constructs the Gas volume detector elements
  //
  //////////////////////////////////////////////////////////////////////
  //      GAS VOLUME
  //
  // Several geometries are possible. Select the different options using
  // the messenger commands
  //
  //////////////////////////////////////////////////////////////////////

  G4double gasVolumeCenterPosX = 0.*m;
  G4double gasVolumeCenterPosY = 0.*m;
  G4double gasVolumeCenterPosZ = 0.*m;

  //luciteBoxIncluded="on";

  G4LogicalVolume* gasLog(0);                   //pointer to logic gas
  G4VPhysicalVolume* gasPhys(0);                //pointer to physic gas

  if(detectorGeometry == "box"){
    G4cout << "##################################################################"
	   << G4endl
	   << "######  ActarSimGasDetectorConstruction::ConstructActar()  #######"
	   << G4endl
	   << " Box-like gas geometry."
	   << G4endl;
    G4cout << " Box Parameters: "
      	   << G4endl
	   << " xGasBox = " <<  xGasBox
	   << ",  yGasBox = " <<  yGasBox
	   << ",  zGasBox = " <<  zGasBox
      	   << G4endl
	   << " gasMaterial: " <<  gasMaterial
	   << G4endl;
    G4cout << "##################################################################"
	   << G4endl;

    //centered in (0,0,zGasBox) to have origin in the detector entrance
    gasVolumeCenterPosZ = zGasBox;

    G4Box* gasBox;
    gasBox = new G4Box("gasBox",
		       xGasBox,yGasBox,zGasBox);

    gasLog = new G4LogicalVolume(gasBox,
				 gasMaterial,
				 "gasLog");

   
    //Adding Lucite GasBox
 
    if(luciteBoxIncluded=="on"){
 //--------------------------
  // Gas Box
  //-------------------------- 
  //Added Lucite for gasbox
  G4double a;  // atomic mass
  G4double z;  // atomic number
  G4double density;  //density
  G4int ncomponents;
  G4Element* H  = new G4Element("Hydrogen" ,"H" , z= 1., a=   1.00794*g/mole);
  G4Element* C  = new G4Element("Carbon"   ,"C",  z=6.,  a=   12.0107*g/mole);
  G4Element* O  = new G4Element("Oxygen"   ,"O" , z= 8., a=   15.9994*g/mole);

  G4Material* Lct =
  new G4Material("Lucite", density= 1.185*g/cm3, ncomponents=3);
  Lct->AddElement(C, 59.97*perCent);
  Lct->AddElement(H, 8.07*perCent);
  Lct->AddElement(O, 31.96*perCent);

  //if(ParisGeoIncludedFlag=="on"){
  G4double boxSizeX=xGasBox+10*mm;
  G4double boxSizeY=yGasBox+10*mm;
  G4double boxSizeZ=zGasBox+10*mm;

  G4double boxPositionX=gasVolumeCenterPosX;
  G4double boxPositionY=gasVolumeCenterPosY;
  G4double boxPositionZ=gasVolumeCenterPosZ;
  
  //G4double boxPositionX=0;
  //G4double boxPositionY=0;
  //G4double boxPositionZ=0;

  gasVolumeCenterPosX=0;
  gasVolumeCenterPosY=0;
  gasVolumeCenterPosZ=0;

  //--------------------------
  // Beam Window in Gas Box
  //-------------------------- 
  G4double window_outer_radius = 3*mm;
  G4double window_inner_radius = 0*mm;
  G4double window_length = 5*mm;
  G4double startAngle = 0.*deg;
  G4double spanningAngle = 360.*deg;

 G4Tubs *window = new G4Tubs("Window",window_inner_radius,window_outer_radius,window_length,
			     startAngle,spanningAngle);
 G4VisAttributes* windowVisAtt= new G4VisAttributes(G4Colour(1.0,0.,0.));
    windowVisAtt->SetVisibility(true);
    
    G4LogicalVolume* window_log = new G4LogicalVolume(window,gasMaterial,"window_log",0,0,0);
    
    //window_log->SetVisAttributes(windowVisAtt);
    
    G4double windowPosZ=-boxSizeZ+5*mm;
    G4double windowPosY=0*cm;
    G4double windowPosX=0*mm;
  
    G4RotationMatrix *rot=0;


  G4Box *LuciteBox=new G4Box("LuciteBox", boxSizeX, boxSizeY, boxSizeZ);

  G4LogicalVolume *LuciteBox_log=new G4LogicalVolume(LuciteBox,Lct,"LuciteBoxLog");

  G4VPhysicalVolume *LuciteBox_phys=new G4PVPlacement(0,G4ThreeVector( boxPositionX, boxPositionY, boxPositionZ),LuciteBox_log,"GasBox",worldLog,false,0);

    G4VPhysicalVolume* window_phys=new G4PVPlacement(rot,G4ThreeVector(windowPosX,windowPosY,windowPosZ),window_log,"window",LuciteBox_log,false,0);
    
 gasPhys = new G4PVPlacement(0,G4ThreeVector(gasVolumeCenterPosX,
						gasVolumeCenterPosY,
						gasVolumeCenterPosZ),
				gasLog,"gasPhys",LuciteBox_log,false,0);

    }
    else
 gasPhys = new G4PVPlacement(0,G4ThreeVector(gasVolumeCenterPosX,
						gasVolumeCenterPosY,
						gasVolumeCenterPosZ),
				gasLog,"gasPhys",worldLog,false,0);

  }
  else if(detectorGeometry == "tube"){
    G4cout << "##################################################################"
	   << G4endl
	   << "########  ActarSimGasDetectorConstruction::ConstructActar()  ########"
	   << G4endl
	   << " Tube-like gas geometry."
	   << G4endl;
    G4cout << " Tube Parameters: "
      	   << G4endl
	   << " radiusGasTub = " <<  radiusGasTub
	   << ",  lengthGasTub = " <<  lengthGasTub
      	   << G4endl
	   << " gasMaterial: " <<  gasMaterial
	   << G4endl;
    G4cout << "##################################################################"
	   << G4endl;

    //centered in (0,0,lengthGasTub) to have origin in the detector entrance
    gasVolumeCenterPosZ = lengthGasTub;

    G4Tubs* gasTub;
    gasTub = new G4Tubs("gasTub",
			0*mm,radiusGasTub,lengthGasTub,0,twopi);

    gasLog = new G4LogicalVolume(gasTub,
				 gasMaterial,
			 	 "gasLog");

    gasPhys = new G4PVPlacement(0,G4ThreeVector(gasVolumeCenterPosX,
						gasVolumeCenterPosY,
						gasVolumeCenterPosZ),
				gasLog,"gasPhys",worldLog,false,0);
  }
  else {
    G4cout << G4endl
	   << " ERROR in ActarSimGasDetectorConstruction::ConstructActar(). No valid volume type defined "
	   << G4endl;
  }

  G4LogicalVolume* beamShieldLog(0);                //pointer to logic gas
  G4VPhysicalVolume* beamShieldPhys;                //pointer to physic gas

  if( beamShieldPhys){;}

  if( beamShieldGeometry == "tube"){
    G4cout << "##################################################################"
	   << G4endl
	   << "########  ActarSimGasDetectorConstruction::ConstructActar()  ########"
	   << G4endl
	   << " Beam shielding geometry."
	   << G4endl;
    G4cout << " Tube Parameters: "
      	   << G4endl
	   << " innerRadiusBeamShieldTub = " <<  innerRadiusBeamShieldTub
	   << ", outerRadiusBeamShieldTub = " <<  outerRadiusBeamShieldTub
      	   << G4endl
	   << " lengthBeamShieldTub = " <<  lengthBeamShieldTub
	   << ", beamShieldMaterial: " <<  beamShieldMaterial
	   << G4endl;
    G4cout << "##################################################################"
	   << G4endl;


    G4Tubs* beamShieldTub;
    beamShieldTub = new G4Tubs("beamShieldTub",
			       innerRadiusBeamShieldTub,
			       outerRadiusBeamShieldTub,
			       lengthBeamShieldTub,
			       0,twopi);

    beamShieldLog = new G4LogicalVolume(beamShieldTub,
					beamShieldMaterial,
					"beamShieldLog");

    beamShieldPhys = new G4PVPlacement(0,G4ThreeVector(),
				       beamShieldLog,"beamShieldPhys",gasLog,false,0);


  }

  //------------------------------------------------
  // Sensitive detectors
  //------------------------------------------------
  gasLog->SetSensitiveDetector( detConstruction->GetGasSD() );

  //------------------------------------------------------------------
  // Visualization attributes
  //------------------------------------------------------------------
  worldLog->SetVisAttributes (G4VisAttributes::Invisible);
  G4VisAttributes* gasVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,0.0));
  G4VisAttributes* beamShieldVisAtt = new G4VisAttributes(G4Colour(1.0,0.0,1.0));
  gasVisAtt->SetVisibility(true);
  gasLog->SetVisAttributes(gasVisAtt);
  if( beamShieldGeometry == "tube")  beamShieldLog->SetVisAttributes(beamShieldVisAtt);

  return gasPhys;
}


void ActarSimGasDetectorConstruction::SetGasMaterial (G4String mat) {
  //
  // Sets the material the gas is made of
  //
  G4Material* pttoMaterial = G4Material::GetMaterial(mat);
  if (pttoMaterial) gasMaterial = pttoMaterial;
}


void ActarSimGasDetectorConstruction::SetBeamShieldMaterial(G4String mat) {
  //
  // Sets the material the medium is made of
  //
  G4Material* pttoMaterial = G4Material::GetMaterial(mat);
  if (pttoMaterial) beamShieldMaterial = pttoMaterial;

}


void ActarSimGasDetectorConstruction::SetDetectorGeometry(G4String type) {
  //
  // Sets the geometry of the detector (box or tube)
  //
  detectorGeometry = type;
}

void ActarSimGasDetectorConstruction::SetLuciteBox(G4String type) {
  //
  // Sets the geometry of the detector (box or tube)
  //
  luciteBoxIncluded = type;
}


void ActarSimGasDetectorConstruction::SetBeamShieldGeometry(G4String type) {
  //
  // Sets the geometry of the detector (box or tube)
  //
  beamShieldGeometry = type;
}



void ActarSimGasDetectorConstruction::UpdateGeometry() {
  //
  // Updates Gas detector
  //

  Construct(detConstruction->GetWorldLogicalVolume());
  G4RunManager::GetRunManager()->
    DefineWorldVolume(detConstruction->GetWorldPhysicalVolume());
}


void ActarSimGasDetectorConstruction::PrintDetectorParameters() {
  //
  // Prints Gas volume detector parameters. To be filled
  //

  G4cout << "##################################################################"
	 << G4endl
	 << "##  ActarSimGasDetectorConstruction::PrintDetectorParameters() ###"
	 << G4endl
	 << " The gas volume is a " ;
  if(detectorGeometry == "box")
    G4cout << "box; its parameters are:" << G4endl;
  if(detectorGeometry == "tube")
    G4cout << "tube; its parameters are:" << G4endl;
  G4cout << " The gas material is: " << gasMaterial  << G4endl;
  if(detectorGeometry == "box")
    G4cout << " The gasBox size is : " << xGasBox/cm << "x" << yGasBox/cm
	   << "x" << zGasBox/cm << " cm3 " << G4endl << G4endl ;
  if(detectorGeometry == "tube")
    G4cout << " The gasTube parameters are: " << G4endl
	   << " radiusGasTub = " <<  radiusGasTub
	   << ",  lengthGasTub = " <<  lengthGasTub << G4endl ;
  if( beamShieldGeometry == "tube"){
    G4cout << " The beam shielding parameters are:"  << G4endl
	   << " innerRadiusBeamShieldTub = " <<  innerRadiusBeamShieldTub
	   << ", outerRadiusBeamShieldTub = " <<  outerRadiusBeamShieldTub
      	   << G4endl
	   << " lengthBeamShieldTub = " <<  lengthBeamShieldTub
	   << ", beamShieldMaterial: " <<  beamShieldMaterial
	   << G4endl;
  }

  G4cout << "##################################################################"
	 << G4endl;
}
