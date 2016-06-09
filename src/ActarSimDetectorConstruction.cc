/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol
//*-- Date: 05/2005
//*-- Last Update:  10/02/16
// --------------------------------------------------------------
// Description:
//   Detector construction and complementary definitions
//
// --------------------------------------------------------------
// Comments:
//
//   - 10/02/16 Splitting ConstructActar according to TPC project.
//   - 26/11/15 Recovering old functionality and correcting
//   - 17/04/08 Modularizing the detectors construction
//   - 04/04/06 Multigeometry with the possibility of updating.
//              Gas as a sensitive detector...
//   - 24/05/05 Created based on calGamma simulation
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include "ActarSimDetectorConstruction.hh"
#include "ActarSimDetectorMessenger.hh"

#include "ActarSimGasDetectorConstruction.hh"
#include "ActarSimSilDetectorConstruction.hh"
#include "ActarSimSilRingDetectorConstruction.hh"
#include "ActarSimSciDetectorConstruction.hh"
#include "ActarSimSciRingDetectorConstruction.hh"
#include "ActarSimPlaDetectorConstruction.hh"

#include "ActarSimGasSD.hh"
#include "ActarSimSilSD.hh"
#include "ActarSimSilRingSD.hh"
#include "ActarSimSciSD.hh"
#include "ActarSimSciRingSD.hh"
#include "ActarSimPlaSD.hh"

#include "ActarSimROOTAnalysis.hh"

#include "G4RotationMatrix.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "globals.hh"
#include "G4AssemblyVolume.hh"

//_______________________________________________________________________________________________________
ActarSimDetectorConstruction::ActarSimDetectorConstruction()
  :   gasSD(0), silSD(0), silRingSD(0), sciSD(0),sciRingSD(0),plaSD(0),
      solidWorld(0), worldLog(0), chamberLog(0), AlplateLog(0), DiamondLog(0), SupportLog(0),
      worldPhys(0), chamberPhys(0), AlplatePhys(0), DiamondPhys(0), SupportPhys(0),
      mediumMaterial(0), defaultMaterial(0), chamberMaterial(0), windowMaterial(0),
      emField(0), MaikoGeoIncludedFlag("off"), ACTARTPCGeoIncludedFlag("off"),
      gasGeoIncludedFlag("on"), silGeoIncludedFlag("off"), sciGeoIncludedFlag("off"),
      gasDet(0), silDet(0),silRingDet(0), sciDet(0), sciRingDet(0), plaDet(0){
   
   /*!
    *  ActarSimDetectorConstruction standard constructor
	*  
    */

  //default values of half-length -> size of World (2x2x2 m3)
  worldSizeX = 1.*m;
  worldSizeY = 1.*m;
  worldSizeZ = 1.*m;
  //default values of half-length -> size of Chamber (1x1x1 m3)
  chamberSizeX = 0.5*m;
  chamberSizeY = 0.5*m;
  chamberSizeZ = 0.5*m;

  //SD are here defined to avoid problems in the Construct function
  //turning on twice the detectors
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  //  gas volume sensitive detector
  G4String gasSDname = "gasSD";
  gasSD = new ActarSimGasSD( gasSDname );
  SDman->AddNewDetector( gasSD );
  //  sil volume sensitive detector
  G4String silSDname = "silSD";
  silSD = new ActarSimSilSD( silSDname );
  SDman->AddNewDetector( silSD );
  //  silRing volume sensitive detector
  G4String silRingSDname = "silRingSD";
  silRingSD = new ActarSimSilRingSD( silRingSDname );
  SDman->AddNewDetector( silRingSD );
  //  sci volume sensitive detector
  G4String sciSDname = "sciSD";
  sciSD = new ActarSimSciSD( sciSDname );
  SDman->AddNewDetector( sciSD );
  //  sciRing volume sensitive detector
  G4String sciRingSDname = "sciRingSD";
  sciRingSD = new ActarSimSciRingSD( sciRingSDname );
  SDman->AddNewDetector( sciRingSD );
  // pla volume sensitive detector
  G4String plaSDname = "plaSD";
  plaSD = new ActarSimPlaSD( plaSDname );
  SDman->AddNewDetector( plaSD );

  //define default materials and set medium, default, chamber, window default materials
  DefineMaterials();
  SetMediumMaterial("Air");
  SetDefaultMaterial("Galactic");
  SetChamberMaterial("Galactic");
  SetWindowMaterial("Mylar");

  //electric and magnetic fields
  emField = new ActarSimUniformEMField();

  //Modular detector construction objects
  gasDet = new ActarSimGasDetectorConstruction(this);
  silDet = new ActarSimSilDetectorConstruction(this);
  silRingDet = new ActarSimSilRingDetectorConstruction(this);
  sciDet = new ActarSimSciDetectorConstruction(this);
  sciRingDet = new ActarSimSciRingDetectorConstruction(this);
  plaDet = new ActarSimPlaDetectorConstruction(this);

  // create commands for interactive definition of the detector
  detectorMessenger = new ActarSimDetectorMessenger(this);
}

//_______________________________________________________________________________________________________
ActarSimDetectorConstruction::~ActarSimDetectorConstruction() {
   /*!
    *  ActarSimDetectorConstruction standard destructor
	*  
    */
  if (emField    != NULL) delete emField;
  if (gasDet     != NULL) delete gasDet;
  if (silDet     != NULL) delete silDet;
  if (silRingDet != NULL) delete silRingDet;
  if (sciDet     != NULL) delete sciDet;
  if (sciRingDet != NULL) delete sciRingDet;
  if (plaDet     != NULL) delete plaDet;
  /*
  if (gasSD      != NULL) delete gasSD;
  if (silSD      != NULL) delete silSD;
  if (silRingSD  != NULL) delete silRingSD;
  if (sciSD      != NULL) delete sciSD;
  if (sciRingSD  != NULL) delete sciRingSD;
  if (plaSD      != NULL) delete plaSD;
  */
  if (detectorMessenger != NULL) delete detectorMessenger;
}

//_______________________________________________________________________________________________________
G4VPhysicalVolume* ActarSimDetectorConstruction::Construct() {
  /*!
   *  GEANT4 MANDATORY METHOD for the geometry implementation.
   *  
   *  The geometry of the detector is constructed according to user-specified flags. 
   *    Available detectors:
   *     -ACTAR_TPC 
   *     -ACTAR demonstrator
   *     -Maiko geometry
   *     -SpecMAT geometry
   *     -Generic geometry
   *
   *  Returns a pointer to the World Physical Volume according to G4 specifications
   *
   *  Only one geometry option can be specified. 
   *  In case of multiple geometry definiotn an error message is displayed and a NULL pointer returned.
   *  
   */

  G4int geo = 0;
  G4int i_geo = 1;
   
  if(ACTARTPCGeoIncludedFlag == "on"){
    geo = 1; i_geo += 1;
  }
  
  if(ACTARTPCDEMOGeoIncludedFlag == "on"){
    geo = 2; i_geo += 1;
  }
  
  if(SpecMATGeoIncludedFlag == "on"){
    geo = 3; i_geo += 1;	
  }
  
  if(MaikoGeoIncludedFlag == "on"){
    geo = 4; i_geo += 1;
  }
  
  if(OthersGeoIncludedFlag == "on"){
    geo = 5; i_geo += 1;
  }
  
  if (i_geo > 2){
    G4cout<<"ERROR, multiple geometry definition. Verify inputfile. (geo="<< geo <<", i_geo="<<i_geo<<")"<<G4endl;
    exit (-1);
  }

  //Build the DETECTOR according to the specified layout		
  switch (geo){
	  
  case 0: G4cout << "Building empty geometry" <<G4endl;
    return ConstructEmptyWorld();  

  case 1: G4cout << "Building ACTAR_TPC geometry" <<G4endl;
    return ConstructActarTPC();  
    
  case 2: G4cout << "Building ACTAR_TPC Demonstrator geometry"<<G4endl;
    return ConstructActarTPCDEMO();

  case 3: G4cout << "Building SpecMAT geometry"<<G4endl;
    return ConstructSpecMAT();
    
  case 4: G4cout << "Building MAIKO geometry"<<G4endl;
    return ConstructMAIKO();
    
  case 5: G4cout << "Building Other geometry"<<G4endl;
    return ConstructOthers();
    
  default: G4cout << "UNKNOWN geometry"<<G4endl;
	         
  }
	
 
  return NULL;
}

//_______________________________________________________________________________________________________
G4VPhysicalVolume* ActarSimDetectorConstruction::ConstructEmptyWorld() {
  /*!
   *   This methods constructs an empty World volume with default sizes.
   *   
   *   It is used if no geometry is specified when inizialization is called.
   *
   */
	
  solidWorld = new G4Box("World",                //its name
			 worldSizeX,worldSizeY,worldSizeZ);  //its size

  worldLog = new G4LogicalVolume(solidWorld,     //its solid
				 mediumMaterial,                 //its material
				 "World");                       //its name

  worldPhys = new G4PVPlacement(0,     //no rotation
				G4ThreeVector(),       //at (0,0,0)
				worldLog,              //its logical volume
				"World",               //its name
				0,                     //its mother  volume
				false,                 //no boolean operation
				0);                    //copy number

  return worldPhys;
}

//_______________________________________________________________________________________________________
G4VPhysicalVolume* ActarSimDetectorConstruction::ConstructActarTPC() {
  /*!
   *   This methods constructs the ActarTPC geometry
   *
   *   A-The World is a Box with HALF-SIZES OF 1.5m x 1.5m x 1.5m (user-settings overridden).
   *
   *   B-The scattering chamber is a box with user-defined sizes 
   *   (default HALF-LENGTH values are 0.5x0.5x0.5 m^3 set in the constructor)
   *   The position is user-defined
   *
   *   C-Hard-coded subvolumes:
   *
   *
   *   D-Optional sub-volumes and ancillaries:
   *     -Gas volume (see ActarSimGasDetectorConstruction)
   *     -Sil volume (see ActarSimSilDetectorConstruction)
   *     -Sci volume (see ActarSimSciDetectorConstruction)
   *     
   *   The Analysis is eventually configured according to the implemented geometry.
   *
   *   Returns a pointer to the world's physical volume.
   */
  
  
  //--------------------------
  //World Volume
  //--------------------------
  
  //Geometrical definition of the world, half sizes
  SetWorldSizeX(1.5*m);
  SetWorldSizeY(1.5*m);
  SetWorldSizeZ(1.5*m);
  
  solidWorld = new G4Box("World",                //its name
			 worldSizeX,worldSizeY,worldSizeZ);  //its size

  worldLog = new G4LogicalVolume(solidWorld,     //its solid
				 mediumMaterial,                 //its material
				 "World");                       //its name

  worldPhys = new G4PVPlacement(0,     //no rotation
				G4ThreeVector(),       //at (0,0,0)
				worldLog,              //its logical volume
				"World",               //its name
				0,                     //its mother  volume
				false,                 //no boolean operation
				0);                    //copy number

  
  //--------------------------
  //Scattering Chamber
  //--------------------------
  //Definition of the ACTAR-TPC detector
  //Chamber half-lengths are (150, 105, 150)mm,
  //also selectable using /ActarSim/det/setXLengthGasChamber... in the macros
  //Last sentence is not true if chamberSizeX,Y,Z are reinitialized here - T.M. Feb 2016
  chamberSizeX = 200.*mm;
  chamberSizeY = 105.*mm;
  chamberSizeZ = 200.*mm;

  //Chamber X,Y,Z Center
  chamberCenterX = 0.*m;
  chamberCenterY = 0.*m; 
  chamberCenterZ = 0.*m;
 

  G4Box* solidChamber = new G4Box("Chamber",         //its name
				  chamberSizeX,chamberSizeY,chamberSizeZ);   //its size

  G4LogicalVolume* chamberLog = new G4LogicalVolume(solidChamber, //its solid
						    chamberMaterial,
						    "Chamber");            //its name

  G4VPhysicalVolume* chamberPhys = new G4PVPlacement(0,                     //no rotation
						     G4ThreeVector(chamberCenterX,
								   chamberCenterY,
								   chamberCenterZ),
						     chamberLog,            //its logical volume
						     "Chamber",             //its name
						     worldLog,              //its mother  volume
						     false,                 //no boolean operation
						     0);
								    
  if(chamberPhys){;}

  //--------------------------
  // Gas volume
  //--------------------------
  if(gasGeoIncludedFlag=="on")
    gasDet->Construct(chamberLog);

  //--------------------------
  //An Aluminium plate to see the Pads active area just below the gas volume
  //--------------------------
  G4double plateSizeX = 128.*mm;
  G4double plateSizeY = 4.54/2*mm;
  G4double plateSizeZ = 128.*mm;

  G4Box *Alplate=new G4Box("Al_plate",plateSizeX,plateSizeY,plateSizeZ);
  AlplateLog=new G4LogicalVolume(Alplate,G4Material::GetMaterial("Aluminium"),"Al_plate");

  G4double platePosX = 0.*cm;
  G4double platePosY =-chamberSizeY + plateSizeY;
  G4double platePosZ = 0.*cm;

  AlplatePhys=new G4PVPlacement(0,G4ThreeVector( platePosX,platePosY,platePosZ),
				AlplateLog,"Al_plate",chamberLog,false,0);

  G4VisAttributes* plateVisAtt= new G4VisAttributes(G4Colour(1.0,0.,1.0));
  plateVisAtt->SetVisibility(true);
  AlplateLog->SetVisAttributes(plateVisAtt);

  //--------------------------
  // Sil volume
  //--------------------------
  if(silGeoIncludedFlag=="on")
    silDet->Construct(chamberLog);

  //--------------------------
  // Sci volume
  //--------------------------
  if(sciGeoIncludedFlag=="on")
    sciDet->Construct(chamberLog);

  //--------------------------
  // Histogramming
  //--------------------------
  if(gActarSimROOTAnalysis)
    gActarSimROOTAnalysis->Construct(worldPhys);

  //--------------------------
  //Connection to the analysis only for those detectors included!
  //--------------------------
  if (gActarSimROOTAnalysis) {
    if (gasGeoIncludedFlag=="on") gActarSimROOTAnalysis->SetGasAnalOn();
    if (silGeoIncludedFlag=="on") gActarSimROOTAnalysis->SetSilAnalOn();
    if (sciGeoIncludedFlag=="on") gActarSimROOTAnalysis->SetSciAnalOn();
    gActarSimROOTAnalysis->InitAnalysisForExistingDetectors();
  }

  //visibility
  worldLog->SetVisAttributes (G4VisAttributes::Invisible);

  return worldPhys;
}

//_______________________________________________________________________________________________________
G4VPhysicalVolume* ActarSimDetectorConstruction::ConstructActarTPCDEMO() {
  /*!
   *   This methods constructs the ActarTPC DEMONSTRATOR geometry
   *
   *   A-The World is a Box with HALF-SIZES OF 1.5m x 1.5m x 1.5m (user-settings overridden).
   *
   *   B-The scattering chamber is a box with HARD-CODED HALF SIZES 
   *     its position is FIXED at the center of the World's volume  
   *   
   *   C-Hard-coded Sub-volumes and ancillaries:
   *     -Beam entrance window
   *     -Field cage support
   *     -Al plate
   *     -Beam diamond detector
   *
   *   D-Optional sub-volumes and ancillaries:
   *     -Gas volume (see ActarSimGasDetectorConstruction)
   *     -Sil volume (see ActarSimSilDetectorConstruction)
   *     -Sci volume (see ActarSimSciDetectorConstruction)
   *     
   *   The Analysis is eventually configured according to the implemented geometry.
   *
   *   Returns a pointer to the world's physical volume.
   */
	
  //--------------------------
  //World Volume
  //--------------------------
  
  //Geometrical definition of the world, half sizes
  SetWorldSizeX(1.5*m);
  SetWorldSizeY(1.5*m);
  SetWorldSizeZ(1.5*m);
  
  solidWorld = new G4Box("World",                //its name
			 worldSizeX,worldSizeY,worldSizeZ);  //its size

  worldLog = new G4LogicalVolume(solidWorld,     //its solid
				 mediumMaterial,                 //its material
				 "World");                       //its name

  worldPhys = new G4PVPlacement(0,     //no rotation
				G4ThreeVector(),       //at (0,0,0)
				worldLog,              //its logical volume
				"World",               //its name
				0,                     //its mother  volume
				false,                 //no boolean operation
				0);                    //copy number

				

  //--------------------------
  //Scattering Chamber
  //--------------------------
  //Definition of the ACTAR-TPC Demonstrator
  //Chamber half-lengths are (95, 105, 120)mm,
  //also selectable using /ActarSim/det/setXLengthGasChamber... in the macros
  //Last sentence is not true if chamberSizeX,Y,Z are reinitialized here - T.M. Feb 2016
  chamberSizeX = 95.*mm;
  chamberSizeY = 105.*mm;
  chamberSizeZ = 120.*mm;

  //Chamber X,Y,Z Center
  chamberCenterX = 0.*m;
  chamberCenterY = 0.*m;                      //OLD chamberSizeY-(yGasBoxPosition+yPadSize)
  //chamberCenterY = 105.*mm-85.*mm-4.54*mm;  //So Y centered with GasBox
  chamberCenterZ = 0.*m;                      //OLD zGasBoxPosition
 

  G4Box* solidChamber = new G4Box("Chamber",         //its name
				  chamberSizeX,chamberSizeY,chamberSizeZ);   //its size

  G4LogicalVolume* chamberLog = new G4LogicalVolume(solidChamber, //its solid
						    chamberMaterial,
						    "Chamber");            //its name

  G4VPhysicalVolume* chamberPhys = new G4PVPlacement(0,                     //no rotation
						     G4ThreeVector(chamberCenterX,
								   chamberCenterY,
								   chamberCenterZ),
						     chamberLog,            //its logical volume
						     "Chamber",             //its name
						     worldLog,              //its mother  volume
						     false,                 //no boolean operation
						     0);

  if(chamberPhys){;}


  //--------------------------
  //Beam entrance Window in Chamber
  //--------------------------
  G4double window_outer_radius =  36.*mm;
  G4double window_inner_radius =   0.*mm;
  G4double window_half_length  =   5.*mm;
  G4double startAngle          =   0.*deg;
  G4double spanningAngle       = 360.*deg;

  G4Tubs *window = new G4Tubs("Window",window_inner_radius,window_outer_radius,window_half_length,
			      startAngle,spanningAngle);
  G4VisAttributes* windowVisAtt= new G4VisAttributes(G4Colour(1.0,0.,0.));
  windowVisAtt->SetVisibility(true);

  G4LogicalVolume* window_log = new G4LogicalVolume(window,chamberMaterial,"window_log",0,0,0);
  window_log->SetVisAttributes(windowVisAtt);

  G4double windowPosX = 0.*mm;
  G4double windowPosY = 0.*mm;
  G4double windowPosZ = -chamberSizeZ+window_half_length+22.*mm;//enter window is situated 22mm inside chamber

  G4VPhysicalVolume* window_phys=new G4PVPlacement(0,G4ThreeVector(windowPosX,windowPosY,windowPosZ),
						   window_log,"window",chamberLog,false,0);

  if(window_phys){;}

  //--------------------------
  //Support of the field cage
  //--------------------------
  G4RotationMatrix* rotLeft = new G4RotationMatrix(pi/2,pi/2,-pi/2);//ZY planes
  G4RotationMatrix* rotRight = new G4RotationMatrix(-pi/2,pi/2,pi/2);//ZY planes

  G4double Support_x  =  8. *mm;
  G4double Support_x2 =  6.4*mm;
  G4double Support_y  = 85. *mm;
  //G4double Support_y = 89.54*mm;
  G4double Support_z  =  1.6*mm;

  G4Box* SupportBox = new G4Box("SupportBox", Support_x, Support_y, Support_z);
  G4Box* SupportBox2 = new G4Box("SupportBox2", Support_x2, Support_y, Support_z);

  SupportLog=new G4LogicalVolume(SupportBox,G4Material::GetMaterial("Lead"),"SupportLog");

  //TO BE IMPROVED: should take the pad x syze from a variable, will do it later
  //SupportPhys=new G4PVPlacement(0,G4ThreeVector(32+6.175-8,0,64+6.575),
  SupportPhys=new G4PVPlacement(0,G4ThreeVector(32+6.175-8,-105+85+4.54,64+6.575),
    				SupportLog,"Support",chamberLog,false,0);
  //SupportPhys=new G4PVPlacement(0,G4ThreeVector(-32-6.175+8,0,64+6.575),
  SupportPhys=new G4PVPlacement(0,G4ThreeVector(-32-6.175+8,-105+85+4.54,64+6.575),
    				SupportLog,"Support",chamberLog,false,1);

  G4VisAttributes* SupportVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  SupportVisAtt->SetVisibility(true);
  SupportLog->SetVisAttributes(SupportVisAtt);

  SupportLog=new G4LogicalVolume(SupportBox2,G4Material::GetMaterial("Lead"),"SupportLog");

  //SupportPhys=new G4PVPlacement(rotRight,G4ThreeVector(32+4.575,0,64+4.975-6.4),//should take the pad x syze from a variable, will do it later
  SupportPhys=new G4PVPlacement(rotRight,G4ThreeVector(32+4.575,-105+85+4.54,64+4.975-6.4),//should take the pad x syze from a variable, will do it later
    				SupportLog,"Support",chamberLog,false,2);

  //SupportPhys=new G4PVPlacement(rotLeft,G4ThreeVector(-32-4.575,0,64+4.975-6.4),//should take the pad x syze from a variable, will do it later
  SupportPhys=new G4PVPlacement(rotLeft,G4ThreeVector(-32-4.575,-105+85+4.54,64+4.975-6.4),//should take the pad x syze from a variable, will do it later
    				SupportLog,"Support",chamberLog,false,2);

  SupportVisAtt->SetVisibility(true);
  SupportLog->SetVisAttributes(SupportVisAtt);

  //--------------------------
  //A Diamond detector in front of the DSSD detectors to catch the beam
  //--------------------------
  G4double Diamond_x = 12.*mm;
  G4double Diamond_y = 12.*mm;
  G4double Diamond_z = 0.75*mm;

  G4Box* DiamondBox = new G4Box("DiamondBox", Diamond_x, Diamond_y, Diamond_z);

  DiamondLog=new G4LogicalVolume(DiamondBox,G4Material::GetMaterial("Lead"),"DiamondLog");
  DiamondPhys=new G4PVPlacement(0,G4ThreeVector(0,0,64+23),//should take the pad x syze from a variable, will do it later
    				DiamondLog,"Diamond",chamberLog,false,0);

  G4VisAttributes* DiamondVisAtt= new G4VisAttributes(G4Colour(1.0,0.,1.0));
  DiamondVisAtt->SetVisibility(true);
  DiamondLog->SetVisAttributes(DiamondVisAtt);

  //--------------------------
  // Gas volume
  //--------------------------
  if(gasGeoIncludedFlag=="on")
    gasDet->Construct(chamberLog);

  //--------------------------
  //An Aluminium plate to see the Pads active area just below the gas volume
  //--------------------------
  G4double plateSizeX = 32.*mm;
  G4double plateSizeY = 4.54/2*mm;
  G4double plateSizeZ = 64.*mm;

  G4Box *Alplate=new G4Box("Al_plate",plateSizeX,plateSizeY,plateSizeZ);
  AlplateLog=new G4LogicalVolume(Alplate,G4Material::GetMaterial("Aluminium"),"Al_plate");

  G4double platePosX = 0.*cm;
  //G4double platePosY = -gasDet->GetGasBoxSizeY() +2* plateSizeY;
  G4double platePosY =-chamberSizeY + plateSizeY;
  G4double platePosZ = 0.*cm;

  AlplatePhys=new G4PVPlacement(0,G4ThreeVector( platePosX,platePosY,platePosZ),
				AlplateLog,"Al_plate",chamberLog,false,0);

  G4VisAttributes* plateVisAtt= new G4VisAttributes(G4Colour(1.0,0.,1.0));
  plateVisAtt->SetVisibility(true);
  AlplateLog->SetVisAttributes(plateVisAtt);

  //--------------------------
  // Sil volume
  //--------------------------
  if(silGeoIncludedFlag=="on")
    silDet->Construct(chamberLog);

  //--------------------------
  // Sci volume
  //--------------------------
  if(sciGeoIncludedFlag=="on")
    sciDet->Construct(chamberLog);

  //--------------------------
  // Histogramming
  //--------------------------
  if (gActarSimROOTAnalysis)
    gActarSimROOTAnalysis->Construct(worldPhys);

  //--------------------------
  //connection to the analysis only for those detectors included!
  //--------------------------
  if (gActarSimROOTAnalysis) {
    if (gasGeoIncludedFlag=="on") gActarSimROOTAnalysis->SetGasAnalOn();
    if (silGeoIncludedFlag=="on") gActarSimROOTAnalysis->SetSilAnalOn();
    if (sciGeoIncludedFlag=="on") gActarSimROOTAnalysis->SetSciAnalOn();
    gActarSimROOTAnalysis->InitAnalysisForExistingDetectors();
  }

  //visibility
  worldLog->SetVisAttributes (G4VisAttributes::Invisible);

  return worldPhys;
}

//_______________________________________________________________________________________________________
G4VPhysicalVolume* ActarSimDetectorConstruction::ConstructSpecMAT() {
  /*!
   *   This methods builds the SpecMAT geometry
   *
   *   A-The World Volume is a box with user-specified dimensions
   *     (default HALF LENGTHS are 1.0m x 1.0m x 1.0m defined in the constructor)
   *
   *   B-The scattering chamber is .... still to be defined (tube or hexagonal prism)
   *   
   *   C-Hard-coded Sub-volumes and ancillaries:
   *
   *
   *   D-Optional sub-volumes and ancillaries:
   *     -Gas volume (see ActarSimGasDetectorConstruction)
   *     -Array of scintillation detectors (see ActarSimScintillatorDetectorConstruction)
   *     
   *   The Analysis is eventually configured according to the implemented geometry.
   *
   *   Returns a pointer to the world's physical volume.
   */
	
  //--------------------------
  //World Volume
  //--------------------------
  
  solidWorld = new G4Box("World",                //its name
			 worldSizeX,worldSizeY,worldSizeZ);  //its size

  worldLog = new G4LogicalVolume(solidWorld,     //its solid
				 mediumMaterial,                 //its material
				 "World");                       //its name

  worldPhys = new G4PVPlacement(0,     //no rotation
				G4ThreeVector(),       //at (0,0,0)
				worldLog,              //its logical volume
				"World",               //its name
				0,                     //its mother  volume
				false,                 //no boolean operation
				0);                    //copy number

  //--------------------------
  //Scattering Chamber
  //--------------------------
  
  //--------------------------
  //Gas Volume
  //--------------------------
	
  //--------------------------
  //Array of scintillation detectors
  //--------------------------	
	
	
  return worldPhys;
}

//_______________________________________________________________________________________________________
G4VPhysicalVolume* ActarSimDetectorConstruction::ConstructMAIKO() {
  /*!
   *   This methods builds the MAIKO geometry
   *
   *   A-The World Volume is a box with FIXED (user settings overridden)
   *     (HALF SIZES of 6.0m x 6.0m x 6.0m)
   *
   *   B-The scattering chamber is a Box with user defined half-sizes
   *   
   *   C-Hard-coded Sub-volumes and ancillaries:
   *     -Beam exit window
   *     -Maiko ancillaries: silRingDet, sciRingDet, plaDet
   *
   *   D-Optional sub-volumes and ancillaries:
   *     -Gas volume (see ActarSimGasDetectorConstruction)
   *     -DO WE KEEP Sci and Sil or are they included in the "Rings" ??????????????????
   *     
   *   The Analysis is eventually configured according to the implemented geometry.
   *
   *   Returns a pointer to the world's physical volume.
   */
 
  //--------------------------
  //World Volume
  //--------------------------
  
  //Geometrical definition of the world, half sizes
  //Hard coded geometry for the MAIKO detector (216 m^3 box!)
  SetWorldSizeX(6.0*m);
  SetWorldSizeY(6.0*m);
  SetWorldSizeZ(6.0*m);
  
  //Build the World Volume
  solidWorld = new G4Box("World",                //its name
			 worldSizeX,worldSizeY,worldSizeZ);  //its size

  worldLog = new G4LogicalVolume(solidWorld,     //its solid
				 mediumMaterial,                 //its material
				 "World");                       //its name

  worldPhys = new G4PVPlacement(0,     //no rotation
				G4ThreeVector(),       //at (0,0,0)
				worldLog,              //its logical volume
				"World",               //its name
				0,                     //its mother  volume
				false,                 //no boolean operation
				0);                    //copy number

				
  //--------------------------
  //Scattering Chamber
  //--------------------------
				
  G4Box* solidChamber = new G4Box("Chamber",         //its name
				  chamberSizeX,chamberSizeY,chamberSizeZ);   //its size

  G4LogicalVolume* chamberLog = new G4LogicalVolume(solidChamber, //its solid
						    chamberMaterial,
						    "Chamber");            //its name

  G4VPhysicalVolume* chamberPhys = new G4PVPlacement(0,                     //no rotation
						     G4ThreeVector(chamberCenterX,
								   chamberCenterY,
								   chamberCenterZ),
						     chamberLog,            //its logical volume
						     "Chamber",             //its name
						     worldLog,              //its mother  volume
						     false,                 //no boolean operation
						     0);

  if(chamberPhys){;}

  //--------------------------
  // Beam exit Window in Chamber
  //--------------------------
  G4double innerRadius = 0.*cm;
  G4double outerRadius = 10.*cm;
  G4double hz = 0.037*mm;
  G4double startAngle = 0.*deg;
  G4double spanningAngle = 360.*deg;

  G4VPhysicalVolume* mylarWin;

  G4Tubs* mwindow= new G4Tubs("mwindow",
			      innerRadius,
			      outerRadius,
			      hz,
			      startAngle,
			      spanningAngle);

  G4LogicalVolume* mwindowLog = new G4LogicalVolume(mwindow,          //its solid
						    windowMaterial,      //its material
						    "mwindow");            //its name

  mylarWin = new G4PVPlacement(0,                     //no rotation
			       G4ThreeVector(0,0,chamberSizeZ),  //OLD G4ThreeVector(0,0,chamberSizeZ+zGasBoxPosition),
			       mwindowLog,            //its logical volume
			       "mwindow",               //its name
			       worldLog,                     //its mother  volume
			       false,                 //no boolean operation
			       0);                    //copy number

  if(mylarWin){;}

  G4VisAttributes* mylarVisAtt = new G4VisAttributes(G4Colour(1.0,0.0,0.0));
  mylarVisAtt->SetVisibility(true);
  mwindowLog->SetVisAttributes(mylarVisAtt);

  //--------------------------
  // Gas volume
  //--------------------------
  if(gasGeoIncludedFlag=="on")
    gasDet->Construct(chamberLog);

  //--------------------------
  // Sil volume
  //--------------------------
  if(silGeoIncludedFlag=="on")
    silDet->Construct(chamberLog);

  //--------------------------
  // Sci volume
  //--------------------------
  if(sciGeoIncludedFlag=="on")
    sciDet->Construct(chamberLog);

  //--------------------------
  // Maiko ancillaries
  //--------------------------
  silRingDet->Construct(worldLog);
  sciRingDet->Construct(worldLog);
  plaDet->Construct(worldLog);
  

  // Histogramming
  if (gActarSimROOTAnalysis)
    gActarSimROOTAnalysis->Construct(worldPhys);

  //connection to the analysis only for those detectors included!
  if (gActarSimROOTAnalysis) {
    if (gasGeoIncludedFlag=="on") gActarSimROOTAnalysis->SetGasAnalOn();
    if (silGeoIncludedFlag=="on") gActarSimROOTAnalysis->SetSilAnalOn();
    if (sciGeoIncludedFlag=="on") gActarSimROOTAnalysis->SetSciAnalOn();
    gActarSimROOTAnalysis->InitAnalysisForExistingDetectors();
  }

  //visibility
  worldLog->SetVisAttributes (G4VisAttributes::Invisible);

  return worldPhys;
}

//_______________________________________________________________________________________________________
G4VPhysicalVolume* ActarSimDetectorConstruction::ConstructOthers() {
  /*!
   *   This methods builds the Other geometries
   *
   *   A-The World Volume is a box with user defined half sizes
   *
   *   B-The scattering chamber is (to be defined)
   *
   *   
   *   C-Hard-coded Sub-volumes and ancillaries: (none yet)
   *
   *
   *   D-Optional sub-volumes and ancillaries: (none yet)
   *	
   *
   *
   *
   *   Returns a pointer to the world's physical volume.
   */	
	
  //--------------------------
  //World Volume
  //--------------------------
	    
  solidWorld = new G4Box("World",                //its name
			 worldSizeX,worldSizeY,worldSizeZ);  //its size

  worldLog = new G4LogicalVolume(solidWorld,     //its solid
				 mediumMaterial,                 //its material
				 "World");                       //its name

  worldPhys = new G4PVPlacement(0,     //no rotation
				G4ThreeVector(),       //at (0,0,0)
				worldLog,              //its logical volume
				"World",               //its name
				0,                     //its mother  volume
				false,                 //no boolean operation
				0);                    //copy number

  return worldPhys;
}



//_______________________________________________________________________________________________________
void ActarSimDetectorConstruction::PrintDetectorParameters() {
  /*!
   *   Print current detector parameters
   *
   *   
   */	

  G4cout << G4endl
	 << "--------------------------------------------------------------" << G4endl
	 << "--------------------------------------------------------------" << G4endl
	 << "-- ActarSimDetectorConstruction::PrintDetectorParameters()  --" << G4endl
	 << "--                                                          --" << G4endl	  	 
	 << "-- Geometry FLAGS:                                          --" << G4endl	  
	 << "--    -ACTARTPCGeoIncludedFlag    : "<< ACTARTPCGeoIncludedFlag << "                      --" << G4endl
	 << "--    -ACTARTPCDEMOGeoIncludedFlag: "<< ACTARTPCDEMOGeoIncludedFlag << "                      --" << G4endl
	 << "--    -SpecMATGeoIncludedFlag     : "<< SpecMATGeoIncludedFlag << "                      --" << G4endl
	 << "--    -MaikoGeoIncludedFlag       : "<< MaikoGeoIncludedFlag << "                      --" << G4endl
	 << "--    -OthersGeoIncludedFlag      : "<< OthersGeoIncludedFlag << "                      --" << G4endl
	 << "-- Ancillary FLAGS:                                          --" << G4endl	  
	 << "--    -gasGeoIncludedFlag    : "<< gasGeoIncludedFlag << "                      --" << G4endl
	 << "--    -silGeoIncludedFlag    : "<< silGeoIncludedFlag << "                      --" << G4endl
	 << "--    -sciGeoIncludedFlag    : "<< sciGeoIncludedFlag << "                      --" << G4endl
	 
	 << "--                                                          --" << G4endl	  
	 << "--                                                          --" << G4endl	  
	 << "--                                                          --" << G4endl	  
	 << "-- The medium material is: "<< G4endl << mediumMaterial << G4endl << G4endl << G4endl
	 << "-- The chamber material is: "<< G4endl << chamberMaterial << G4endl << G4endl;
  G4cout << G4endl << " The EM field applied has the following components:"
	 << G4endl << " Magnetic component: "
	 << emField->GetMagneticFieldValue().x() << " "
	 << emField->GetMagneticFieldValue().y() << " "
	 << emField->GetMagneticFieldValue().z()
	 << G4endl << " Electric component: "
	 << emField->GetElectricFieldValue().x() << " "
	 << emField->GetElectricFieldValue().y() << " "
	 << emField->GetElectricFieldValue().z() << G4endl
	 << "--------------------------------------------------------------" << G4endl;
	 

  if (gasGeoIncludedFlag=="on") gasDet->PrintDetectorParameters();
  if (silGeoIncludedFlag=="on") silDet->PrintDetectorParameters();
  if (sciGeoIncludedFlag=="on") sciDet->PrintDetectorParameters();
  if (MaikoGeoIncludedFlag=="on") {
    silRingDet->PrintDetectorParameters();
    sciRingDet->PrintDetectorParameters();
    plaDet->PrintDetectorParameters();
  }
  
  G4cout << "--------------------------------------------------------------" << G4endl;
  G4cout << "--------------------------------------------------------------" << G4endl << G4endl << G4endl;
}

//_______________________________________________________________________________________________________
void ActarSimDetectorConstruction::SetMediumMaterial(G4String mat) {
  /*!
   * Sets the material the medium is made of
   */
  G4Material* pttoMaterial = G4Material::GetMaterial(mat);
  if (pttoMaterial) mediumMaterial = pttoMaterial;
}

//_______________________________________________________________________________________________________
void ActarSimDetectorConstruction::SetDefaultMaterial(G4String mat) {
  /*!
   * Sets the default material
   */
  G4Material* pttoMaterial = G4Material::GetMaterial(mat);
  if (pttoMaterial) defaultMaterial = pttoMaterial;
}

//_______________________________________________________________________________________________________
void ActarSimDetectorConstruction::SetChamberMaterial(G4String mat) {
  /*!
   * Sets the material the chamber is made of (the same as GasBox)
   */
  //DefineMaterials();
  G4Material* pttoMaterial = G4Material::GetMaterial(mat);
  if (pttoMaterial) chamberMaterial = pttoMaterial;
  //G4cout << " The chamber gas material is: " << chamberMaterial  << G4endl;
}

//_______________________________________________________________________________________________________
void ActarSimDetectorConstruction::SetUpdateChamberMaterial(G4Material* mater) {
  /*!
   * Sets the material the chamber is made of (the same as GasBox)
   * define by user (T and P)
   */
  chamberMaterial = mater;
  //G4cout << " The chamber gas material is: " << chamberMaterial  << G4endl;
}

//_______________________________________________________________________________________________________
void ActarSimDetectorConstruction::SetWindowMaterial (G4String mat) {
  /*!
   * Sets the material the window is made of
   */
  G4Material* pttoMaterial = G4Material::GetMaterial(mat);
  if (pttoMaterial) windowMaterial = pttoMaterial;
}

//_______________________________________________________________________________________________________
void ActarSimDetectorConstruction::UpdateGeometry() {
  /*!
   * Updates any change on the geometry of the detectors
   */
  G4RunManager::GetRunManager()->DefineWorldVolume(this->Construct());
}

//_______________________________________________________________________________________________________
void ActarSimDetectorConstruction::UpdateEMField() {
  /*!
   * Setting the uniform EM field
   */
  emField->SetFieldValue(mField,eField);
}

//_______________________________________________________________________________________________________
void ActarSimDetectorConstruction::DefineMaterials() {
  /*!
   * Define the materials to be used
   * (NOTE: it results impossible to define externally (via messenger)
   * the density of the gases, because the materials table is an static element
   * which remains constant from creation to the end of the program)
   *
   *  Do we really need those definitions here? 
   *
   */

  G4double a;  // atomic mass
  G4double z;  // atomic number

  G4Element* H  = new G4Element("Hydrogen" ,"H" , z= 1., a=   1.00794*g/mole);
  //G4Element* D  = new G4Element("Deuterium","D" , z= 1., a=    2.0140*g/mole);
  G4Element* He = new G4Element("Helium"   ,"He", z= 2., a=    4.0026*g/mole);
  G4Element* C  = new G4Element("Carbon"   ,"C",  z=6.,  a=   12.0107*g/mole);
  G4Element* N  = new G4Element("Nitrogen" ,"N" , z= 7., a=  14.00674*g/mole);
  G4Element* O  = new G4Element("Oxygen"   ,"O" , z= 8., a=   15.9994*g/mole);
  G4Element* F  = new G4Element("Fluorine" ,"F",  z=9.,  a=18.9984032*g/mole);
  G4Element* Na = new G4Element("Sodium"   ,"Na", z=11., a=  22.98977*g/mole);
  G4Element* S  = new G4Element("Sulphur"  ,"S",  z=16., a=    32.066*g/mole);
  G4Element* Ar = new G4Element("Argon"    ,"Ar", z=18., a=   39.9481*g/mole);
  G4Element* Zn = new G4Element("Zinc",     "Zn", z=30., a=     65.39*g/mole);
  G4Element* Ge = new G4Element("Germanium","Ge", z=32., a=     72.61*g/mole);
  G4Element* Br = new G4Element("Bromine"  ,"Br", z=35., a=    79.904*g/mole);
  G4Element* Cd = new G4Element("Cadmium"  ,"Cd", z=48., a=   112.411*g/mole);
  G4Element* Te = new G4Element("Tellurium","Te", z=52., a=    127.60*g/mole);
  G4Element* I  = new G4Element("Iodine"   ,"I",  z=53., a= 126.90447*g/mole);
  G4Element* Cs = new G4Element("Cesium"   ,"Cs", z=55., a= 132.90545*g/mole);
  G4Element* Ba = new G4Element("Barium"   ,"Ba", z=56., a=   137.327*g/mole);
  G4Element* La = new G4Element("Lanthanum","La", z=57., a=  138.9055*g/mole);
  G4Element* Ce = new G4Element("Cerium"   ,"Ce", z=58., a=   140.116*g/mole);
  G4Element* Lu = new G4Element("Lutecium" ,"Lu", z=71., a=   174.967*g/mole);
  G4Element* W  = new G4Element("Tungsten" ,"W" , z=74., a=    183.84*g/mole);
  G4Element* Pb = new G4Element("Lead"     ,"Pb", z=82., a=    207.20*g/mole);
  G4Element* Bi = new G4Element("Bismuth"  ,"Bi", z=83., a= 208.98038*g/mole);

  //
  // define materials
  //
  G4double density, pressure, temperature;
  G4int ncomponents, natoms;
  G4double fractionmass;

  G4Material* Al =
    new G4Material("Aluminium", z= 13., a= 26.98*g/mole, density= 2.7*g/cm3);

  G4Material* Cu =
    new G4Material("Copper", z= 29., a= 63.546*g/mole, density= 8.96*g/cm3);

  if(Al){;}
  if(Cu){;}

  G4Material* Vacuum =
    new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,
		   kStateGas, 3.e-18*pascal, 2.73*kelvin);
  Vacuum->SetChemicalFormula("NOMATTER");

  density	=0.0688*mg/cm3;
  pressure	=0.3*bar;
  temperature	=298.2*kelvin;
  G4Material* HeCF4300=
    new G4Material("HeCF4_300", density,ncomponents=3,kStateGas, temperature, pressure);
  HeCF4300->AddElement (He,natoms=98);
  HeCF4300->AddElement (C,natoms=2);
  HeCF4300->AddElement (F,natoms=8);

  density	=0.1376*mg/cm3;
  pressure	=0.6*bar;
  temperature	=298.2*kelvin;
  G4Material* HeCF4600=
    new G4Material("HeCF4_600", density,ncomponents=3,kStateGas, temperature, pressure);
  HeCF4600->AddElement (He,natoms=98);
  HeCF4600->AddElement (C,natoms=2);
  HeCF4600->AddElement (F,natoms=8);

  density	=0.1835*mg/cm3;
  pressure	=0.8*bar;
  temperature	=298.2*kelvin;
  G4Material* HeCF4800=
    new G4Material("HeCF4_800", density,ncomponents=3,kStateGas, temperature, pressure);
  HeCF4800->AddElement (He,natoms=98);
  HeCF4800->AddElement (C,natoms=2);
  HeCF4800->AddElement (F,natoms=8);

  density	=0.2521*mg/cm3;
  pressure	=1.1*bar;
  temperature	=298.2*kelvin;
  G4Material* HeCF41100=
    new G4Material("HeCF4_1100", density,ncomponents=3,kStateGas, temperature, pressure);
  HeCF41100->AddElement (He,natoms=98);
  HeCF41100->AddElement (C,natoms=2);
  HeCF41100->AddElement (F,natoms=8);

  density	=1.818*mg/cm3;
  pressure	=1.1*bar;
  temperature	=298.2*kelvin;
  G4Material* ArCF41100=
    new G4Material("ArCF4_1100", density,ncomponents=3,kStateGas, temperature, pressure);
  ArCF41100->AddElement (Ar,natoms=98);
  ArCF41100->AddElement (C,natoms=2);
  ArCF41100->AddElement (F,natoms=8);

  //////////////////////////////////////////////
  //Air (STP: 1.290*mg/cm3)
  density = 1.290*mg/cm3;
  G4Material* Air =
    new G4Material("Air", density, ncomponents=2);
  Air->AddElement(N, fractionmass=70.*perCent);
  Air->AddElement(O, fractionmass=30.*perCent);

  //Methane (default  0.7174*mg/cm3 STP)
  density = 0.7174*mg/cm3;
  G4Material* methane =
    new G4Material("CH4", density, ncomponents=2) ;
  methane->AddElement(C,1);
  methane->AddElement(H,4);

  //Propane (default  2.005*mg/cm3 STP)
  density = 2.005*mg/cm3;
  G4Material* propane =
    new G4Material("C3H8", density, ncomponents=2) ;
  propane->AddElement(C,3);
  propane->AddElement(H,8);

  G4Material* H2O =
    new G4Material("Water", density= 1.000*g/cm3, ncomponents=2);
  H2O->AddElement(H, natoms=2);
  H2O->AddElement(O, natoms=1);
  H2O->SetChemicalFormula("H_2O");
  H2O->GetIonisation()->SetMeanExcitationEnergy(75.0*eV);

  G4Material* BGO =
    new G4Material("BGO", density= 7.10*g/cm3, ncomponents=3);
  BGO->AddElement(O , natoms=12);
  BGO->AddElement(Ge, natoms= 3);
  BGO->AddElement(Bi, natoms= 4);

  G4Material* SodiumIodine =
    new G4Material("NaI",density= 3.67*g/cm3,ncomponents=2);
  SodiumIodine->AddElement(Na , natoms=1);
  SodiumIodine->AddElement(I , natoms=1);

  G4Material* BariumFluoride =
    new G4Material("BaF",density= 4.89*g/cm3,ncomponents=2);
  BariumFluoride->AddElement(Ba , natoms=1);
  BariumFluoride->AddElement(F , natoms=2);

  G4Material*  CesiumIodine=
    new G4Material("CsI",density= 4.51*g/cm3,ncomponents=2);
  CesiumIodine->AddElement(Cs, natoms=1);
  CesiumIodine->AddElement(I, natoms=1);

  G4Material* CeriumFluoride =
    new G4Material("CeF3",density= 6.16*g/cm3,ncomponents=2);
  CeriumFluoride->AddElement(Ce , natoms=1);
  CeriumFluoride->AddElement(F , natoms=3);

  G4Material* LantanumBromide =
    new G4Material("LaBr3",density= 5.29*g/cm3,ncomponents=2);
  LantanumBromide->AddElement(La , natoms=1);
  LantanumBromide->AddElement(Br , natoms=3);

  G4Material*  LSO=
    new G4Material("LSO",density= 7.4*g/cm3,ncomponents=3);
  LSO->AddElement(Lu , natoms=2);
  LSO->AddElement(S , natoms=1);
  LSO->AddElement(O , natoms=5);

  G4Material* CdZnTe=
    new G4Material("CdZnTe",density= 5.78*g/cm3 ,ncomponents=3);
  CdZnTe->AddElement(Cd, natoms=9);
  CdZnTe->AddElement(Zn, natoms=1);
  CdZnTe->AddElement(Te, natoms=10);

  G4Material* PWO=
    new G4Material("PWO",density= 8.28*g/cm3 ,ncomponents=3);
  PWO->AddElement(Pb, natoms=1);
  PWO->AddElement(W, natoms=1);
  PWO->AddElement(O, natoms=4);

  //Mylar
  G4Material* mylar=new G4Material("Mylar",density= 1.4*g/cm3,ncomponents=3);
  mylar -> AddElement(H,0.042);
  mylar -> AddElement(C,0.625);
  mylar -> AddElement(O,0.333);

  G4Material*  Scint = new G4Material("BC408", density= 1.032*g/cm3, ncomponents=2);
  Scint->AddElement(C, natoms=9);
  Scint->AddElement(H, natoms=10);

  new G4Material("Silicon"    , z=14., a= 28.09*g/mole, density= 2.330*g/cm3);

  new G4Material("Germanium"  , z=32., a= 72.61*g/mole, density= 5.323*g/cm3);

  new G4Material("Iron"       , z=26., a= 55.85*g/mole, density= 7.870*g/cm3);

  new G4Material("Tungsten"   , z=74., a=183.85*g/mole, density= 19.30*g/cm3);

  new G4Material("Lead"       , z=82., a=207.19*g/mole, density= 11.35*g/cm3);

  new G4Material("Uranium"    , z=92., a=238.03*g/mole, density= 18.95*g/cm3);

  //
  //materials printout
  //
  //  G4cout << *(G4Material::GetMaterialTable()) << G4endl;

  //default materials of the World
  defaultMaterial  = Vacuum;
}

//_______________________________________________________________________________________________________
void ActarSimDetectorConstruction::SetEleField(G4ThreeVector eVector){
  /*!
   * Setting the uniform electric field vector
   *
   * ATT to the units! There is no elec field unit defined in the program by default!!!
   * I had problems defining the electric field units...
   * Data is supposed to come in kV/cm from the messenger, but... what is the input unit
   * for GEANT4 in the class G4ElectroMagneticField? Not clear to me...
   */
  eField = eVector;
}

//_______________________________________________________________________________________________________
void ActarSimDetectorConstruction::SetMagField(G4ThreeVector mVector){
  /*!
   * Setting the uniform magnetic field vector
   */
  mField = mVector;
}
