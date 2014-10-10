/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol hapol@fpddux.usc.es
//*-- Date: 05/2005
//*-- Last Update:  16/05/08
// --------------------------------------------------------------
// Description:
//   Detector construction and complementary definitions
//
// --------------------------------------------------------------
// Comments:
//
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
#include "ActarSimSciDetectorConstruction.hh"

#include "ActarSimGasSD.hh"
#include "ActarSimSilSD.hh"
#include "ActarSimSciSD.hh"

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


ActarSimDetectorConstruction::ActarSimDetectorConstruction()
  :   gasSD(0), silSD(0), sciSD(0),
      solidWorld(0), worldLog(0), worldPhys(0),
      mediumMaterial(0), defaultMaterial(0), emField(0),
      gasGeoIncludedFlag("off"), silGeoIncludedFlag("off"), sciGeoIncludedFlag("off"),
      gasDet(0), silDet(0), sciDet(0){
  //
  // Constructor
  //

  //SD are here defined to avoid problems in the Construct function
  //turning on twice the detectors
  G4SDManager* SDman = G4SDManager::GetSDMpointer();

  //  gas volume
  G4String gasSDname = "gasSD";
  gasSD = new ActarSimGasSD( gasSDname );
  SDman->AddNewDetector( gasSD );
  //  sil volume
  G4String silSDname = "silSD";
  silSD = new ActarSimSilSD( silSDname );
  SDman->AddNewDetector( silSD );
  //  sci volume
  G4String sciSDname = "sciSD";
  sciSD = new ActarSimSciSD( sciSDname );
  SDman->AddNewDetector( sciSD );

  //define materials and set medium material
  DefineMaterials();
  //TODO -> Check if materials support now (G4.9 and later)independent pressure parameter or redefinition...
  SetMediumMaterial("Air");
  SetDefaultMaterial("Galactic");

  //electric and magnetic fields
  emField = new ActarSimUniformEMField();

  //Modular detector construction objects
  gasDet = new ActarSimGasDetectorConstruction(this);
  silDet = new ActarSimSilDetectorConstruction(this);
  sciDet = new ActarSimSciDetectorConstruction(this);

  // create commands for interactive definition of the detector
  detectorMessenger = new ActarSimDetectorMessenger(this);
}

ActarSimDetectorConstruction::~ActarSimDetectorConstruction() {
  //
  // Destructor
  //
  delete emField;
  delete detectorMessenger;
}

G4VPhysicalVolume* ActarSimDetectorConstruction::Construct()
{
  return ConstructActar();
}

G4VPhysicalVolume* ActarSimDetectorConstruction::ConstructActar() {
  //
  // Geometrical definition of the world and gas volume
  //

  G4double worldSizeX = .5*m;
  G4double worldSizeY = .5*m;
  G4double worldSizeZ = .5*m;

  solidWorld = new G4Box("World",                      //its name
			 worldSizeX,worldSizeY,worldSizeZ);   //its size

  worldLog = new G4LogicalVolume(solidWorld,          //its solid
				 mediumMaterial,      //its material
				 "World");            //its name

  worldPhys = new G4PVPlacement(0,                     //no rotation
				G4ThreeVector(),       //at (0,0,0)
				worldLog,            //its logical volume
				"World",               //its name
				0,                     //its mother  volume
				false,                 //no boolean operation
				0);                    //copy number

  //D. Perez-Loureiro Adding an aluminium plate with an slit
  if( plateIncludedFlag == "on"){
  G4double plateSizeX = 15*cm;
  G4double plateSizeY = 15*cm;
  G4double plateSizeZ = 0.5*mm;
  G4double z,a,density;

  G4Box *Alplate=new G4Box("Al_plate",plateSizeX,plateSizeY,plateSizeZ);
 
  G4Material* Al = 
  new G4Material("Aluminum", z= 13., a= 26.98*g/mole, density= 2.7*g/cm3);

  AlplateLog=new G4LogicalVolume(Alplate,Al,"Al_plate");

  // G4double platePosX = 0*cm;
  // G4double platePosY = 0*cm;
  // G4double platePosZ = 18.5*cm;

  //AlplatePhys=new G4PVPlacement(0,G4ThreeVector( platePosX,platePosY,platePosZ),
  //				AlplateLog,"Al_plate",worldLog,false,0); 
  
  AlplatePhys=new G4PVPlacement(0,platePos,
				AlplateLog,"Al_plate",worldLog,false,0); 

  //the Slit

  //G4double slitSizeX = 0.3*mm;
  G4double slitSizeX = 0.05*mm;
  G4double slitSizeY = 0.5*cm;
  G4double slitSizeZ = 1*mm;

  G4Box* Slit=new G4Box("Slit",slitSizeX,slitSizeY,slitSizeZ);

  G4VisAttributes* SlitVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,0.0));
  SlitVisAtt->SetVisibility(true);

  SlitLog=new G4LogicalVolume(Slit,mediumMaterial,"Slit_log");
  //SlitLog->SetVisAttributes(SlitVisAtt);

  // G4double slitPosX = -7.5*cm;
  // G4double slitPosY = 0*cm;
  // G4double slitPosZ = 0*cm;

  //SlitPhys=new G4PVPlacement(0,G4ThreeVector(slitPosX,slitPosY, slitPosZ),SlitLog,"Slit",
  //			       AlplateLog,false,0);

  //G4RotationMatrix *rot=0;
  //rot=new G4RotationMatrix(0,0,pi/2);

  //Assembly of slits
  SlitMask=new G4AssemblyVolume();
  G4RotationMatrix Ra;
  G4ThreeVector Ta;
  G4Transform3D Tr;
  
  // Rotation of the assembly inside the world
  G4RotationMatrix Rm;
  if(slitOrientation=="vertical")
    Rm.setPsi(pi/2);
  //G4RotationMatrix Rm;
   // Fill the assembly by the slits

   //Ta.setX(-0.5*cm); Ta.setY(0.); Ta.setZ(0.);
   //Tr = G4Transform3D(Ra,Ta);
  // SlitMask->AddPlacedVolume(SlitLog, Tr );

   Ta.setX(-1.*cm); Ta.setY(0.); Ta.setZ(0.);
   Tr = G4Transform3D(Ra,Ta);
   SlitMask->AddPlacedVolume(SlitLog, Tr );

   // Ta.setX(-1.5*cm); Ta.setY(0.); Ta.setZ(0.);
   //Tr = G4Transform3D(Ra,Ta);
   // SlitMask->AddPlacedVolume(SlitLog, Tr );

   Ta.setX(-2.*cm); Ta.setY(0.); Ta.setZ(0.);
   Tr = G4Transform3D(Ra,Ta);
   SlitMask->AddPlacedVolume(SlitLog, Tr );

   Ta.setX(-3.*cm); Ta.setY(0.); Ta.setZ(0.);
   Tr = G4Transform3D(Ra,Ta);
   SlitMask->AddPlacedVolume(SlitLog, Tr );

   Ta.setX(-4.*cm); Ta.setY(0.); Ta.setZ(0.);
   Tr = G4Transform3D(Ra,Ta);
   SlitMask->AddPlacedVolume(SlitLog, Tr );

   Ta.setX(0.*cm); Ta.setY(0.); Ta.setZ(0.);
   Tr = G4Transform3D(Ra,Ta);
   SlitMask->AddPlacedVolume(SlitLog, Tr );

   //Ta.setX(0.5*cm); Ta.setY(0.); Ta.setZ(0.);
   //Tr = G4Transform3D(Ra,Ta);
   //SlitMask->AddPlacedVolume(SlitLog, Tr );

   Ta.setX(1.*cm); Ta.setY(0.); Ta.setZ(0.);
   Tr = G4Transform3D(Ra,Ta);
   SlitMask->AddPlacedVolume(SlitLog, Tr );

   //Ta.setX(1.5*cm); Ta.setY(0.); Ta.setZ(0.);
   //Tr = G4Transform3D(Ra,Ta);
   //SlitMask->AddPlacedVolume(SlitLog, Tr );

   Ta.setX(2.*cm); Ta.setY(0.); Ta.setZ(0.);
   Tr = G4Transform3D(Ra,Ta);
   SlitMask->AddPlacedVolume(SlitLog, Tr );

   Ta.setX(3.*cm); Ta.setY(0.); Ta.setZ(0.);
   Tr = G4Transform3D(Ra,Ta);
   SlitMask->AddPlacedVolume(SlitLog, Tr );

   Ta.setX(4.*cm); Ta.setY(0.); Ta.setZ(0.);
   Tr = G4Transform3D(Ra,Ta);
   SlitMask->AddPlacedVolume(SlitLog, Tr );

   // SlitPhys=new G4PVPlacement(rot,slitPos,SlitLog,"Slit",
   //			       AlplateLog,false,0);
   G4ThreeVector Tm( 0,0,0);
   Tr = G4Transform3D(Rm,Tm);
   SlitMask->MakeImprint(AlplateLog, Tr );
  }

 
  //--------------------------
  // Gas volume
  //--------------------------
  if(gasGeoIncludedFlag=="on")
    gasDet->Construct(worldLog);

  //--------------------------
  // Sil volume
  //--------------------------
  if(silGeoIncludedFlag=="on")
    silDet->Construct(worldLog);

  //--------------------------
  // Sci volume
  //--------------------------
  if(sciGeoIncludedFlag=="on")
    sciDet->Construct(worldLog);

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
  //worldLog->SetVisAttributes (G4VisAttributes::Invisible);

  return worldPhys;
}

void ActarSimDetectorConstruction::PrintDetectorParameters() {
  //
  // Print the current detector parameters
  //

  G4cout << G4endl
	 << "--------------------------------------------------------------"
         << G4endl
	 << "-- ActarSimDetectorConstruction::PrintDetectorParameters() --"
	 << G4endl
	 << " The medium material is: " << mediumMaterial << G4endl
	 << G4endl;

  G4cout << G4endl << " The EM field applied has the following components:"
	 << G4endl << " Magnetic component: "
	 << emField->GetMagneticFieldValue().x() << " "
	 << emField->GetMagneticFieldValue().y() << " "
	 << emField->GetMagneticFieldValue().z()
	 << G4endl << " Electric component: "
	 << emField->GetElectricFieldValue().x() << " "
	 << emField->GetElectricFieldValue().y() << " "
	 << emField->GetElectricFieldValue().z()
	 << "--------------------------------------------------------------"
         << G4endl ;

    if (gasGeoIncludedFlag=="on") gasDet->PrintDetectorParameters();
    if (silGeoIncludedFlag=="on") silDet->PrintDetectorParameters();
    if (sciGeoIncludedFlag=="on") sciDet->PrintDetectorParameters();

}


void ActarSimDetectorConstruction::SetMediumMaterial(G4String mat) {
  //
  // Sets the material the medium is made of
  //
  G4Material* pttoMaterial = G4Material::GetMaterial(mat);
  if (pttoMaterial) mediumMaterial = pttoMaterial;

}


void ActarSimDetectorConstruction::SetDefaultMaterial(G4String mat) {
  //
  // Sets the material the medium is made of
  //
  G4Material* pttoMaterial = G4Material::GetMaterial(mat);
  if (pttoMaterial) defaultMaterial = pttoMaterial;

}


void ActarSimDetectorConstruction::UpdateGeometry() {
  //
  // Updates any change on the geometry of the detectors
  //

  G4RunManager::GetRunManager()->DefineWorldVolume(ConstructActar());
}


void ActarSimDetectorConstruction::UpdateEMField() {
  //
  // Setting the uniform EM field
  //
  emField->SetFieldValue(mField,eField);
}


void ActarSimDetectorConstruction::DefineMaterials() {
  //
  // Define the materials to be used
  // (NOTE: it results impossible to define externally (via messenger)
  // the density of the gases, because the materials table is an static element
  // which remains constant from creation to the end of the program)
  //

  G4double a;  // atomic mass
  G4double z;  // atomic number

  G4Element* H  = new G4Element("Hydrogen" ,"H" , z= 1., a=   1.00794*g/mole);
  G4Element* D  = new G4Element("Deuterium","D" , z= 1., a=    2.0140*g/mole);
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

  G4Material* Vacuum =
    new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,
		   kStateGas, 3.e-18*pascal, 2.73*kelvin);
  Vacuum->SetChemicalFormula("NOMATTER");

  //H2_1795 (default 0.09*mg/cm3)
  density	=0.15944*mg/cm3;
  pressure	=1.795*bar;
  temperature	=300.*kelvin;
  G4Material* H2_1795 =
    new G4Material("H2_1795", density, ncomponents=2, kStateGas, temperature, pressure);
  H2_1795->AddElement(H, natoms=1);
  H2_1795->AddElement(H, natoms=1);

  //H2_1800 (default 0.09*mg/cm3)
  density	=0.15988*mg/cm3;
  pressure	=1.800*bar;
  temperature	=300.*kelvin;
  G4Material* H2_1800 =
    new G4Material("H2_1800", density, ncomponents=2, kStateGas, temperature, pressure);
  H2_1800->AddElement(H, natoms=1);
  H2_1800->AddElement(H, natoms=1);

// D2 at lower pressures, added by dypang, 080225
  //D2_40 (default 0.167*mg/cm3)
  density	=6.59E-3*mg/cm3;
  pressure	=0.040*bar;
  temperature	=294.3*kelvin;
  G4Material* D2_40 =
    new G4Material("D2_40", density, ncomponents=2, kStateGas, temperature, pressure);
  D2_40->AddElement(D, natoms=1);
  D2_40->AddElement(D, natoms=1);

  //D2_60 (default 0.167*mg/cm3)
  density	=9.89E-3*mg/cm3;
  pressure	=0.060*bar;
  temperature	=294.3*kelvin;
  G4Material* D2_60 =
    new G4Material("D2_60", density, ncomponents=2, kStateGas, temperature, pressure);
  D2_60->AddElement(D, natoms=1);
  D2_60->AddElement(D, natoms=1);

  //D2_80 (default 0.167*mg/cm3)
  density	=1.32E-2*mg/cm3;
  pressure	=0.080*bar;
  temperature	=294.3*kelvin;
  G4Material* D2_80 =
    new G4Material("D2_80", density, ncomponents=2, kStateGas, temperature, pressure);
  D2_80->AddElement(D, natoms=1);
  D2_80->AddElement(D, natoms=1);

  //D2_100 (default 0.167*mg/cm3)
  density	=1.65E-2*mg/cm3;
  pressure	=0.10*bar;
  temperature	=294.3*kelvin;
  G4Material* D2_100 =
    new G4Material("D2_100", density, ncomponents=2, kStateGas, temperature, pressure);
  D2_100->AddElement(D, natoms=1);
  D2_100->AddElement(D, natoms=1);

  //D2_120 (default 0.167*mg/cm3)
  density	=1.98E-2*mg/cm3;
  pressure	=0.120*bar;
  temperature	=294.3*kelvin;
  G4Material* D2_120 =
    new G4Material("D2_120", density, ncomponents=2, kStateGas, temperature, pressure);
  D2_120->AddElement(D, natoms=1);
  D2_120->AddElement(D, natoms=1);

  //D2_140 (default 0.167*mg/cm3)
  density	=2.31E-2*mg/cm3;
  pressure	=0.140*bar;
  temperature	=294.3*kelvin;
  G4Material* D2_140 =
    new G4Material("D2_140", density, ncomponents=2, kStateGas, temperature, pressure);
  D2_140->AddElement(D, natoms=1);
  D2_140->AddElement(D, natoms=1);

  //D2_160 (default 0.167*mg/cm3)
  density	=2.64E-2*mg/cm3;
  pressure	=0.160*bar;
  temperature	=294.3*kelvin;
  G4Material* D2_160 =
    new G4Material("D2_160", density, ncomponents=2, kStateGas, temperature, pressure);
  D2_160->AddElement(D, natoms=1);
  D2_160->AddElement(D, natoms=1);

  //D2_180 (default 0.167*mg/cm3)
  density	=2.97E-2*mg/cm3;
  pressure	=0.180*bar;
  temperature	=294.3*kelvin;
  G4Material* D2_180 =
    new G4Material("D2_180", density, ncomponents=2, kStateGas, temperature, pressure);
  D2_180->AddElement(D, natoms=1);
  D2_180->AddElement(D, natoms=1);

  //D2_200 (default 0.167*mg/cm3)
  density	=3.30E-2*mg/cm3;
  pressure	=0.200*bar;
  temperature	=294.3*kelvin;
  G4Material* D2_200 =
    new G4Material("D2_200", density, ncomponents=2, kStateGas, temperature, pressure);
  D2_200->AddElement(D, natoms=1);
  D2_200->AddElement(D, natoms=1);

  //D2_220 (default 0.167*mg/cm3)
  density	=3.63E-2*mg/cm3;
  pressure	=0.220*bar;
  temperature	=294.3*kelvin;
  G4Material* D2_220 =
    new G4Material("D2_220", density, ncomponents=2, kStateGas, temperature, pressure);
  D2_220->AddElement(D, natoms=1);
  D2_220->AddElement(D, natoms=1);

  //D2_240 (default 0.167*mg/cm3)
  density	=3.96E-2*mg/cm3;
  pressure	=0.240*bar;
  temperature	=294.3*kelvin;
  G4Material* D2_240 =
    new G4Material("D2_240", density, ncomponents=2, kStateGas, temperature, pressure);
  D2_240->AddElement(D, natoms=1);
  D2_240->AddElement(D, natoms=1);

  //D2_260 (default 0.167*mg/cm3)
  density	=4.29E-2*mg/cm3;
  pressure	=0.260*bar;
  temperature	=294.3*kelvin;
  G4Material* D2_260 =
    new G4Material("D2_260", density, ncomponents=2, kStateGas, temperature, pressure);
  D2_260->AddElement(D, natoms=1);
  D2_260->AddElement(D, natoms=1);

  //D2_280 (default 0.167*mg/cm3)
  density	=4.61E-2*mg/cm3;
  pressure	=0.280*bar;
  temperature	=294.3*kelvin;
  G4Material* D2_280 =
    new G4Material("D2_280", density, ncomponents=2, kStateGas, temperature, pressure);
  D2_280->AddElement(D, natoms=1);
  D2_280->AddElement(D, natoms=1);

  //D2_300 (default 0.167*mg/cm3)
  density	=4.94E-2*mg/cm3;
  pressure	=0.300*bar;
  temperature	=294.3*kelvin;
  G4Material* D2_300 =
    new G4Material("D2_300", density, ncomponents=2, kStateGas, temperature, pressure);
  D2_300->AddElement(D, natoms=1);
  D2_300->AddElement(D, natoms=1);

  //D2_320 (default 0.167*mg/cm3)
  density	=5.27E-2*mg/cm3;
  pressure	=0.320*bar;
  temperature	=294.3*kelvin;
  G4Material* D2_320 =
    new G4Material("D2_320", density, ncomponents=2, kStateGas, temperature, pressure);
  D2_320->AddElement(D, natoms=1);
  D2_320->AddElement(D, natoms=1);

  //D2_340 (default 0.167*mg/cm3)
  density	=5.60E-2*mg/cm3;
  pressure	=0.340*bar;
  temperature	=294.3*kelvin;
  G4Material* D2_340 =
    new G4Material("D2_340", density, ncomponents=2, kStateGas, temperature, pressure);
  D2_340->AddElement(D, natoms=1);
  D2_340->AddElement(D, natoms=1);

  //D2_360 (default 0.167*mg/cm3)
  density	=5.93E-2*mg/cm3;
  pressure	=0.360*bar;
  temperature	=294.3*kelvin;
  G4Material* D2_360 =
    new G4Material("D2_360", density, ncomponents=2, kStateGas, temperature, pressure);
  D2_360->AddElement(D, natoms=1);
  D2_360->AddElement(D, natoms=1);

  //D2_380 (default 0.167*mg/cm3)
  density	=6.26E-2*mg/cm3;
  pressure	=0.380*bar;
  temperature	=294.3*kelvin;
  G4Material* D2_380 =
    new G4Material("D2_380", density, ncomponents=2, kStateGas, temperature, pressure);
  D2_380->AddElement(D, natoms=1);
  D2_380->AddElement(D, natoms=1);

  //D2_400 (default 0.167*mg/cm3)
  density	=6.59E-2*mg/cm3;
  pressure	=0.400*bar;
  temperature	=294.3*kelvin;
  G4Material* D2_400 =
    new G4Material("D2_400", density, ncomponents=2, kStateGas, temperature, pressure);
  D2_400->AddElement(D, natoms=1);
  D2_400->AddElement(D, natoms=1);

  //D2_800 (default 0.167*mg/cm3)
  density	=0.132*mg/cm3;
  pressure	=0.800*bar;
  temperature	=294.3*kelvin;
  G4Material* D2_800 =
    new G4Material("D2_800", density, ncomponents=2, kStateGas, temperature, pressure);
  D2_800->AddElement(D, natoms=1);
  D2_800->AddElement(D, natoms=1);

  //D2_STP (default 0.167*mg/cm3)
  density	=0.167*mg/cm3;
  pressure	=1.01325*bar;
  temperature	=294.3*kelvin;
  G4Material* D2_STP =
    new G4Material("D2_STP", density, ncomponents=2, kStateGas, temperature, pressure);
  D2_STP->AddElement(D, natoms=1);
  D2_STP->AddElement(D, natoms=1);

// end of low pressure D2

  //D2_1695 (default 0.167*mg/cm3)
  density	=0.279*mg/cm3;
  pressure	=1.695*bar;
  temperature	=294.3*kelvin;
  G4Material* D2_1695 =
    new G4Material("D2_1695", density, ncomponents=2, kStateGas, temperature, pressure);
  D2_1695->AddElement(D, natoms=1);
  D2_1695->AddElement(D, natoms=1);

  //D2_1800 (default 0.167*mg/cm3)
  density	=0.297*mg/cm3;
  pressure	=1.800*bar;
  temperature	=294.3*kelvin;
  G4Material* D2_1800 =
    new G4Material("D2_1800", density, ncomponents=2, kStateGas, temperature, pressure);
  D2_1800->AddElement(D, natoms=1);
  D2_1800->AddElement(D, natoms=1);

  //D2_1950 (default 0.167*mg/cm3)
  density	=0.321*mg/cm3;
  pressure	=1.950*bar;
  temperature	=294.3*kelvin;
  G4Material* D2_1950 =
    new G4Material("D2_1950", density, ncomponents=2, kStateGas, temperature, pressure);
  D2_1950->AddElement(D, natoms=1);
  D2_1950->AddElement(D, natoms=1);

  //He_1900 (default 0.18*mg/cm3)
  density	=0.33752*mg/cm3;
  pressure	=1.900*bar;
  temperature	=300.*kelvin;
  G4Material* He_1900 =
    new G4Material("He_1900", density, ncomponents=2, kStateGas, temperature, pressure);
  He_1900->AddElement(He, natoms=1);
  He_1900->AddElement(He, natoms=1);

  //He_2010 (default 0.18*mg/cm3)
  density	=0.35707*mg/cm3;
  pressure	=2.010*bar;
  temperature	=300.*kelvin;
  G4Material* He_2010 =
    new G4Material("He_2010", density, ncomponents=2, kStateGas, temperature, pressure);
  He_2010->AddElement(He, natoms=1);
  He_2010->AddElement(He, natoms=1);

  //Ar_600 (default 1.78*mg/cm3)
  density	=1.05403*mg/cm3;
  pressure	=0.600*bar;
  temperature	=300.*kelvin;
  G4Material* Ar_600 =
    new G4Material("Ar_600", density, ncomponents=2, kStateGas, temperature, pressure);
  Ar_600->AddElement(Ar, natoms=1);
  Ar_600->AddElement(Ar, natoms=1);

  //Ar_710 (default 1.78*mg/cm3)
  density	=1.24727*mg/cm3;
  pressure	=0.710*bar;
  temperature	=300.*kelvin;
  G4Material* Ar_710 =
    new G4Material("Ar_710", density, ncomponents=2, kStateGas, temperature, pressure);
  Ar_710->AddElement(Ar, natoms=1);
  Ar_710->AddElement(Ar, natoms=1);

  //Ar_1005 (default 1.78*mg/cm3)
  density	=1.76551*mg/cm3;
  pressure	=1.005*bar;
  temperature	=300.*kelvin;
  G4Material* Ar_1005 =
    new G4Material("Ar_1005", density, ncomponents=2, kStateGas, temperature, pressure);
  Ar_1005->AddElement(Ar, natoms=1);
  Ar_1005->AddElement(Ar, natoms=1);

  //Ar_1700 (default 1.78*mg/cm3)
  density	=2.98643*mg/cm3;
  pressure	=1.700*bar;
  temperature	=300.*kelvin;
  G4Material* Ar_1700 =
    new G4Material("Ar_1700", density, ncomponents=2, kStateGas, temperature, pressure);
  Ar_1700->AddElement(Ar, natoms=1);
  Ar_1700->AddElement(Ar, natoms=1);

  //Isobutanes (default  2.51*mg/cm3 15 degrees, 1 atm)
  density	=0.372*mg/cm3;
  pressure	=0.150*bar;
  temperature	=288.2*kelvin;
  G4Material* isobutane150 =
    new G4Material("isoC4H10_150", density, ncomponents=2, kStateGas, temperature, pressure) ;
  isobutane150->AddElement(C,4);
  isobutane150->AddElement(H,10);


  density	=0.427*mg/cm3;
  pressure	=0.175*bar;
  temperature	=288.2*kelvin;
  G4Material* isobutane175 =
    new G4Material("isoC4H10_175", density, ncomponents=2, kStateGas, temperature, pressure) ;
  isobutane175->AddElement(C,4);
  isobutane175->AddElement(H,10);

  density	=0.545*mg/cm3;
  pressure	=0.220*bar;
  temperature	=288.2*kelvin;
  G4Material* isobutane220 =
    new G4Material("isoC4H10_220", density, ncomponents=2, kStateGas, temperature, pressure) ;
  isobutane220->AddElement(C,4);
  isobutane220->AddElement(H,10);

  density	=0.743*mg/cm3;
  pressure	=0.300*bar;
  temperature	=288.2*kelvin;
  G4Material* isobutane300 =
    new G4Material("isoC4H10_300", density, ncomponents=2, kStateGas, temperature, pressure) ;
  isobutane300->AddElement(C,4);
  isobutane300->AddElement(H,10);

  density	=1.239*mg/cm3;
  pressure	=0.500*bar;
  temperature	=288.2*kelvin;
  G4Material* isobutane500 =
    new G4Material("isoC4H10_500", density, ncomponents=2, kStateGas, temperature, pressure) ;
  isobutane500->AddElement(C,4);
  isobutane500->AddElement(H,10);

  density	=1.759*mg/cm3;
  pressure	=0.710*bar;
  temperature	=288.2*kelvin;
  G4Material* isobutane710 =
    new G4Material("isoC4H10_710", density, ncomponents=2, kStateGas, temperature, pressure) ;
  isobutane710->AddElement(C,4);
  isobutane710->AddElement(H,10);

  density	=3.220*mg/cm3;
  pressure	=1.300*bar;
  temperature	=288.2*kelvin;
  G4Material* isobutane1300 =
    new G4Material("isoC4H10_1300", density, ncomponents=2, kStateGas, temperature, pressure) ;
  isobutane1300->AddElement(C,4);
  isobutane1300->AddElement(H,10);

  density	=4.657*mg/cm3;
  pressure	=1.880*bar;
  temperature	=288.2*kelvin;
  G4Material* isobutane1880 =
    new G4Material("isoC4H10_1880", density, ncomponents=2, kStateGas, temperature, pressure) ;
  isobutane1880->AddElement(C,4);
  isobutane1880->AddElement(H,10);

  //D. Perez-Loureiro 300911 //Defining CF4
  density	=1.066*mg/cm3;
  pressure	=0.3*bar;
  temperature	=298.2*kelvin;
  G4Material* CF4300 =
    new G4Material("CF4_300", density, ncomponents=2, kStateGas, temperature, pressure) ;
  CF4300->AddElement(C,1);
  CF4300->AddElement(F,4);
 
  density	=3.911*mg/cm3;
  pressure	=1.1*bar;
  temperature	=298.2*kelvin;
  G4Material* CF41100 =
    new G4Material("CF4_1100", density, ncomponents=2, kStateGas, temperature, pressure) ;
 CF41100->AddElement(C,1);
 CF41100->AddElement(F,4);
 
  density	=0.0484*mg/cm3;
  pressure	=0.3*bar;
  temperature	=298.2*kelvin;
  G4Material* He300=
    new G4Material("HeGas_300", z=2, a=4.003*g/mole, density, kStateGas, temperature, pressure);
  if(He300){;} 
  
  density	=0.1776*mg/cm3;
  pressure	=1.1*bar;
  temperature	=298.2*kelvin;
  G4Material* He1100=
    new G4Material("HeGas_1100", z=2, a=4.003*g/mole, density, kStateGas, temperature, pressure);
  if(He1100){;} 
  
  density	=1.746*mg/cm3;
  pressure	=1.1*bar;
  temperature	=298.2*kelvin;
  G4Material* Ar1100=
    new G4Material("ArGas_1100", z=18, a=39.948*g/mole, density, kStateGas, temperature, pressure);
  if(Ar1100){;} 
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

  //Adding isobutane at 25,50,70 mbar

  density	=0.05966*mg/cm3;
  pressure	=0.025*bar;
  temperature	=293.2*kelvin;

  G4Material* isobutane25 =
    new G4Material("isoC4H10_25", density, ncomponents=2, kStateGas, temperature, pressure) ;
  isobutane25->AddElement(C,4);
  isobutane25->AddElement(H,10);

  density	=0.1194*mg/cm3;
  pressure	=0.050*bar;
  temperature	=293.2*kelvin;
  G4Material* isobutane50 =
    new G4Material("isoC4H10_50", density, ncomponents=2, kStateGas, temperature, pressure) ;
  isobutane50->AddElement(C,4);
  isobutane50->AddElement(H,10);

  density	=0.1672*mg/cm3;
  pressure	=0.070*bar;
  temperature	=293.2*kelvin;
  G4Material* isobutane70 =
    new G4Material("isoC4H10_70", density, ncomponents=2, kStateGas, temperature, pressure) ;
  isobutane70->AddElement(C,4);
  isobutane70->AddElement(H,10);




  //////////////////////////////////////////////

  //Air (STP: 1.290*g/cm3)
  density = 1.290*g/cm3;
  G4Material* Air =
    new G4Material("Air", density, ncomponents=2);
  Air->AddElement(N, fractionmass=70.*perCent);
  Air->AddElement(O, fractionmass=30.*perCent);


  //Isobutane (default  2.67*mg/cm3 STP)
  density = 2.67*mg/cm3;
  G4Material* isobutane =
    new G4Material("isoC4H10STP", density, ncomponents=2) ;
  isobutane->AddElement(C,4);
  isobutane->AddElement(H,10);

  //Isobutane (default  2.67*mg/cm3 STP)
  density = 2.67*mg/cm3;
  G4Material* isobutane01 =
    new G4Material("isoC4H10_0.1", 0.001*density, ncomponents=2) ;
  isobutane01->AddElement(C,4);
  isobutane01->AddElement(H,10);

  G4Material* isobutane1 =
    new G4Material("isoC4H10_1", 0.01*density, ncomponents=2) ;
  isobutane1->AddElement(C,4);
  isobutane1->AddElement(H,10);

  G4Material* isobutane10 =
    new G4Material("isoC4H10_10", 0.1*density, ncomponents=2) ;
  isobutane10->AddElement(C,4);
  isobutane10->AddElement(H,10);

  //G4Material* isobutane50 =
  //  new G4Material("isoC4H10_50", 0.5*density, ncomponents=2) ;
  //isobutane50->AddElement(C,4);
  //isobutane50->AddElement(H,10);

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

  // Deuterium at 0.1 atm
  G4Material* Deuterium =
    new G4Material("Deuterium", density= 0.0169*g/cm3, ncomponents=2);
  Deuterium->AddElement(H, natoms=1);
  Deuterium->AddElement(H, natoms=1);




  new G4Material("Silicon"    , z=14., a= 28.09*g/mole, density= 2.330*g/cm3);

  new G4Material("Germanium"  , z=32., a= 72.61*g/mole, density= 5.323*g/cm3);

  new G4Material("Iron"       , z=26., a= 55.85*g/mole, density= 7.870*g/cm3);

  new G4Material("Tungsten"   , z=74., a=183.85*g/mole, density= 19.30*g/cm3);

  new G4Material("Lead"       , z=82., a=207.19*g/mole, density= 11.35*g/cm3);

  new G4Material("Uranium"    , z=92., a=238.03*g/mole, density= 18.95*g/cm3);



  //
  //meterials printout
  //
  //  G4cout << *(G4Material::GetMaterialTable()) << G4endl;

  //default materials of the World
  defaultMaterial  = Vacuum;
}


void ActarSimDetectorConstruction::SetEleField(G4ThreeVector eVector){
  //
  // Setting the uniform electric field vector
  //
  //ATT to the units! There is no elec field unit defined in the program by default!!!
  // I had problems defining the electric field units...
  // Data is supposed to come in kV/cm from the messenger, but... what is the input unit
  // for GEANT4 in the class G4ElectroMagneticField? Not clear to me...
  eField = eVector;
}


void ActarSimDetectorConstruction::SetMagField(G4ThreeVector mVector){
  //
  // Setting the uniform magnetic field vector
  //
  mField = mVector;
}

void ActarSimDetectorConstruction::SetSlitPosition(G4ThreeVector position){
  //Setting slit position;
  slitPos = position;
}

void ActarSimDetectorConstruction::SetPlatePosition(G4ThreeVector position){
  //Setting slit position;
  platePos = position;
}





