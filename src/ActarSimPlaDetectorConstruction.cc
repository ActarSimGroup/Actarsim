/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez
//*-- Date: 04/2008
//*-- Last Update: 15/06/16 by Hector Alvarez
// --------------------------------------------------------------
// Description:
//   Scintillator plastic detector description
//
// --------------------------------------------------------------
// Comments:
//
//   - 17/04/08 Modularizing the ACTAR geometry
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include "ActarSimPlaDetectorConstruction.hh"
#include "ActarSimDetectorConstruction.hh"
#include "ActarSimGasDetectorConstruction.hh"
//#include "ActarSimPlaDetectorMessenger.hh"
#include "ActarSimROOTAnalysis.hh"
#include "ActarSimPlaSD.hh"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Trd.hh"
#include "G4Cons.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4RunManager.hh"
#include "G4Transform3D.hh"

#include "globals.hh"

#include "HodoParametrisation.hh"
#include "G4PVParameterised.hh"

ActarSimPlaDetectorConstruction::
ActarSimPlaDetectorConstruction(ActarSimDetectorConstruction* det)
  :	plaBulkMaterial(0),detConstruction(det) {
  //
  // Constructor. Sets the material and the pointer to the Messenger
  //

  SetPlaBulkMaterial("BC408");
  SetSideCoverage(1);
  SetXBoxPlaHalfLength(100*cm);
  SetYBoxPlaHalfLength(100*cm);
  SetZBoxPlaHalfLength(100*cm);

  // create commands for interactive definition of the calorimeter
  //plaMessenger = new ActarSimPlaDetectorMessenger(this);
}

ActarSimPlaDetectorConstruction::~ActarSimPlaDetectorConstruction(){
  //
  // Destructor
  //
 // delete plaMessenger;
}

G4VPhysicalVolume* ActarSimPlaDetectorConstruction::Construct(G4LogicalVolume* worldLog) {
  //
  // Wrap for the construction functions within the TOF
  //

  //Introduce here other constructors for materials around the TOF (windows, frames...)
  //which can be controlled by the calMessenger
  //ConstructTOFWorld(worldLog);
  return ConstructPla(worldLog);
}

G4VPhysicalVolume* ActarSimPlaDetectorConstruction::ConstructPla(G4LogicalVolume* worldLog) {
  //
  //Chamber Y,Z length
  //G4double chamberSizeY=detConstruction->GetChamberYLength();
  G4double chamberSizeZ=detConstruction->GetChamberSizeZ();

  //Gas chamber position inside the chamber
  ActarSimGasDetectorConstruction* gasDet = detConstruction->GetGasDetector();
  G4double zGasBoxPosition=gasDet->GetGasBoxCenterZ();

    //Hodo Box
	G4double hodo_x=105*cm;
	G4double hodo_y=105*cm;
	G4double hodo_z=60.0*cm;

  G4double plaBulk_x = 9.95*mm;   //half length=12.5mm
  G4double plaBulk_y = 100*mm;   //half length=12.5mm
  G4double plaBulk_z = 5.0*mm;   //half length=15.0mm

	//the numbers
	G4double nDE=13; //number of Scint in dE
	G4double nE1=16; //number of Scint in E1
	G4double nE2=13; //number of Scint in E2
	//G4double nTarget=1;//number of Targets

	//G4double yHodo=chamberSizeY/2-1*mm;
	G4double yHodo=0;
	G4double zHodo=5.0*m+chamberSizeZ-zGasBoxPosition;

	//G4double defectHalfLength = 0.5*mm;
	//G4double separationFromBox = 25*mm;

  // Printing the final settings...
  /*G4cout << "##################################################################"
	 << G4endl
	 << "#########  ActarSimSciDetectorConstruction::ConstructSci()  #######"
	 << G4endl
	 << " Note that only a thin (1 micron) Al window is defined in front"
	 << G4endl
	 << " of the silicons. Here only the scintillator volume is described."
	 << G4endl
	 << " x dimension (half-side) of scintillator: " << sciBulk_x/mm << " mm"
	 << G4endl
	 << " y dimension (half-side) of scintillator: " << sciBulk_y/mm << " mm"
	 << G4endl
	 << " z dimension (half-side) of scintillator: " << sciBulk_z/mm << " mm"
	 << G4endl;
  G4cout << "##################################################################"
	 << G4endl;*/

  G4LogicalVolume* plaLog(0);
  G4VPhysicalVolume* plaPhys(0);

 // G4Box* sciBox =
   // new G4Box("sciBox", sciBulk_x, sciBulk_y, sciBulk_z);

 // sciLog =
   // new G4LogicalVolume(sciBox, sciBulkMaterial, "sciLog");


  G4double density;//, pressure, temperature;
  //G4int ncomponents, natoms;
  //G4double fractionmass;

    G4double a;  // atomic mass
    G4double z;  // atomic number

    G4Material* Vacuum =
    new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,
                   kStateGas, 3.e-18*pascal, 2.73*kelvin);
    Vacuum->SetChemicalFormula("NOMATTER");

    //Hodo
    G4Box* solidHodo  = new G4Box("solidhodo",0.5*hodo_x,0.5*hodo_y,0.5*hodo_z);
	G4LogicalVolume* logicHodo  = new G4LogicalVolume(solidHodo,Vacuum,"logichodo",0,0,0);
	G4VPhysicalVolume* physiHodo  = new G4PVPlacement(0,G4ThreeVector(0.,yHodo,zHodo),logicHodo,"physihodo",worldLog,false,0);

	if(physiHodo){;}

	G4VisAttributes* logicHodo_VisAtt = new G4VisAttributes(G4Colour(0,0,1.0));
	//logicHodo->SetVisAttributes(G4VisAttributes::GetInvisible());
	logicHodo->SetVisAttributes(logicHodo_VisAtt);
	//
	G4Box* plaBox =new G4Box("solidbar",0.5*plaBulk_x,0.5*plaBulk_y,0.5*plaBulk_z);
	plaLog  = new G4LogicalVolume(plaBox,plaBulkMaterial,"logicbar",0,0,0);

	G4VPVParameterisation* BarParam  = new HodoParametrisation(nDE,nE1,nE2,-0.0*hodo_z);
	// dummy value : kXAxis -- modified by parameterised volume
	plaPhys =  new G4PVParameterised("plaPhys",plaLog,logicHodo,kXAxis, nDE+nE1+nE2, BarParam);

	G4VisAttributes* logicBar_VisAtt = new G4VisAttributes(G4Colour(0.7,1.0,0.0));
	plaLog->SetVisAttributes(logicBar_VisAtt);

  //------------------------------------------------
  // Sensitive detectors
  //------------------------------------------------
  plaLog->SetSensitiveDetector( detConstruction->GetPlaSD() );

  //------------------------------------------------------------------
  // Visualization attributes
  //------------------------------------------------------------------
  G4VisAttributes* plaVisAtt1 = new G4VisAttributes(G4Colour(1,1,1));
  plaVisAtt1->SetVisibility(true);
  plaLog->SetVisAttributes(plaVisAtt1);

  return plaPhys;
}

void ActarSimPlaDetectorConstruction::SetPlaBulkMaterial (G4String mat) {
  //
  // Set the material the scintillator bulk is made of
  //

  G4Material* pttoMaterial = G4Material::GetMaterial(mat);
  if (pttoMaterial) plaBulkMaterial = pttoMaterial;
}


void ActarSimPlaDetectorConstruction::UpdateGeometry() {
  //
  // Updates Scintillator detector
  //
  Construct(detConstruction->GetWorldLogicalVolume());
  G4RunManager::GetRunManager()->
    DefineWorldVolume(detConstruction->GetWorldPhysicalVolume());
}


void ActarSimPlaDetectorConstruction::PrintDetectorParameters() {
  //
  // Prints Scintillator detector parameters. To be filled
  //

  G4cout << "##################################################################"
	 << G4endl
	 << "####  ActarSimSciDetectorConstruction::PrintDetectorParameters() ####"
	 << G4endl;
    G4cout << "##################################################################"
	 << G4endl;
}
