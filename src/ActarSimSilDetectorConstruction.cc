/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez
//*-- Date: 04/2008
//*-- Last Update: 07/01/15 by Hector Alvarez
// --------------------------------------------------------------
// Description:
//   Silicon detector description
//
// --------------------------------------------------------------
// Comments:
//
//   - 17/04/08 Modularizing the ACTAR geometry
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include "ActarSimSilDetectorConstruction.hh"
#include "ActarSimDetectorConstruction.hh"
#include "ActarSimGasDetectorConstruction.hh"
#include "ActarSimSilDetectorMessenger.hh"
#include "ActarSimROOTAnalysis.hh"
#include "ActarSimSilSD.hh"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4RunManager.hh"
#include "G4Transform3D.hh"

#include "globals.hh"

ActarSimSilDetectorConstruction::
ActarSimSilDetectorConstruction(ActarSimDetectorConstruction* det)
  :	silBulkMaterial(0),detConstruction(det) {
  //
  // Constructor. Sets the material and the pointer to the Messenger
  //


  SetSilBulkMaterial("Silicon");

  //Options for Silicon and scintillator coverage:
  // 6 bits to indicate which sci wall is present (1) or absent (0)
  // order is:
  // bit1 (lsb) beam output wall
  // bit2 lower (gravity based) wall
  // bit3 upper (gravity based) wall
  // bit4 left (from beam point of view) wall
  // bit5 right (from beam point of view) wall
  // bit6 (msb) beam entrance wall
  SetSideCoverage(1);

  SetXBoxSilHalfLength(100*cm);
  SetYBoxSilHalfLength(100*cm);
  SetZBoxSilHalfLength(100*cm);

  // create commands for interactive definition of the calorimeter
  silMessenger = new ActarSimSilDetectorMessenger(this);
}


ActarSimSilDetectorConstruction::~ActarSimSilDetectorConstruction(){
  //
  // Destructor
  //
  delete silMessenger;
}


G4VPhysicalVolume* ActarSimSilDetectorConstruction::Construct(G4LogicalVolume* chamberLog) {
  //
  // Wrap for the construction functions within the Silicon
  //

  //Introduce here other constructors for materials around the TOF (windows, frames...)
  //which can be controlled by the calMessenger
  //ConstructTOFWorld(chamberLog);
  return ConstructSil(chamberLog);
}



G4VPhysicalVolume* ActarSimSilDetectorConstruction::ConstructSil(G4LogicalVolume* chamberLog) {
  //
  //  Constructs the Silicon detector elements
  //
  //talk with Patricia, Pang and Herve, May 08
  //1) Introduce Silicon and CsI(Tl) detectors alla around. Use MUST style:
  //        Si: 300 microns thick, 100x100mm2, with 128 strides per side
  //(horizontal on one
  //        side and vertical in the opposite).
  //        CsI(Tl): 25x25mm2, 30mm thick, so 16 per Silicon detector.
  //
  //                   <-- 50mm   -->
  //        ------ -- |             x
  //        |    | || |             x wires shaping the field
  //        |    | || |             x
  //        |CsI | || |             x
  //        |    | || |             x
  //        ------ -- |     Gas     x              Gas
  //               Si |             x
  //                  |             x
  //             Al layer (~1 microm)
  //

  //MUST-like silicon detectors (half-sides)
  //I left some "air" in between silicons. Half-length are defectHalfLength shorter.
  // G4double silBulk_x = 49.5*mm;
  // G4double silBulk_y = 49.5*mm;
  // G4double silBulk_z = 0.15*mm;

  G4double silBulk_x = 24.5*mm;
  G4double silBulk_y = 24.5*mm;
  G4double silBulk_z = 0.3575*mm;

  G4double silDSSDBulk_x = 31.5*mm;
  G4double silDSSDBulk_y = 31.5*mm;
  G4double silDSSDBulk_z = 0.75*mm;

  G4double defectHalfLength = 0.5*mm;
  G4double separationFromBox = 3*mm;

  // Printing the final settings...
  G4cout << "##################################################################"
	       << G4endl
	       << "###########  ActarSimSilDetectorConstruction::ConstructSil() #########"
	       << G4endl
	       << " Note that only a thin (1 micron) Al window is defined in front "
	       << G4endl
	       << " of the silicons. No other box around the silicon volume is described."
	       << G4endl
	       << " MAYA detector characteristics: "
	       << G4endl
	       << " x dimension (half-side) of silicon: " << silBulk_x/mm << " mm"
	       << G4endl
	       << " y dimension (half-side) of silicon: " << silBulk_y/mm << " mm"
	       << G4endl
	       << " z dimension (half-side) of silicon: " << silBulk_z/mm << " mm"
	       << G4endl
	       << " DSSD detector characteristics: "
	       << G4endl
	       << " x dimension (half-side) of silicon: " << silDSSDBulk_x/mm << " mm"
	       << G4endl
	       << " y dimension (half-side) of silicon: " << silDSSDBulk_y/mm << " mm"
	       << G4endl
	       << " z dimension (half-side) of silicon: " << silDSSDBulk_z/mm << " mm"
	       << G4endl
	       << " Detector material: "
	       << silBulkMaterial
	       << G4endl;
  G4cout << "##################################################################"
	       << G4endl;

  G4LogicalVolume* silLog(0);
  G4VPhysicalVolume* silPhys(0);

  G4Box* silBox =
    new G4Box("silBox", silBulk_x, silBulk_y, silBulk_z);

  silLog =
    new G4LogicalVolume(silBox, silBulkMaterial, "silLog");

  //DSSD detector
  G4LogicalVolume* silDSSDLog(0);
  G4VPhysicalVolume* silDSSDPhys(0);

  G4Box* silDSSDBox =
    new G4Box("silDSSDBox", silDSSDBulk_x, silDSSDBulk_y, silDSSDBulk_z);

  silDSSDLog =
    new G4LogicalVolume(silDSSDBox, silBulkMaterial, "silDSSDLog");

  //As a function of the lateral coverage
  // 6 bits to indicate which sci wall is present (1) or absent (0)
  // order is:
  // bit1 (lsb) beam output wall
  // bit2 lower (gravity based) wall
  // bit3 upper (gravity based) wall
  // bit4 left (from beam point of view) wall
  // bit5 right (from beam point of view) wall
  // bit6 (msb) beam entrance wall

  //for the moment, let's assume square silicon...
  //if not, more sophisticated approach needed!
  G4int numberOfRowsX = ((G4int) (2*xBoxSilHalfLength/(2*(silBulk_x+defectHalfLength)))) - 1;
  if( (numberOfRowsX+1)*silBulk_x < 2*xBoxSilHalfLength ) numberOfRowsX++;
  G4int numberOfRowsY = ((G4int) (2*yBoxSilHalfLength/(2*(silBulk_x+defectHalfLength)))) - 1;
  if( (numberOfRowsY+1)*silBulk_x < 2*yBoxSilHalfLength ) numberOfRowsY++;
  G4int numberOfRowsZ = ((G4int) (2*zBoxSilHalfLength/(2*(silBulk_x+defectHalfLength)))) - 1;
  if( (numberOfRowsZ+1)*silBulk_x < 2*zBoxSilHalfLength ) numberOfRowsZ++;

  //G4cout << numberOfRowsX << " " <<  numberOfRowsY << " " << numberOfRowsZ << G4endl;

  G4int iterationNumber = 0;

  //By a reason I do not know the correct rotation using Euler angles does not match with standards...
  // I would call rotLeft to rotTop, and so on...
  G4RotationMatrix* rotBack = //beam entrance
    new G4RotationMatrix(0,pi,pi); //if y direction is up and x is right
  //new  G4RotationMatrix(0,pi,0); //if x direction is left and y down
  G4RotationMatrix* rotBottom = //ZX planes (bottom)
    new G4RotationMatrix(0,pi/2,0);
  G4RotationMatrix* rotTop = //ZX planes (top)
    new G4RotationMatrix(0,-pi/2,0);
  G4RotationMatrix* rotLeft = //ZY planes
    new G4RotationMatrix(pi/2,pi/2,-pi/2);
  G4RotationMatrix* rotRight = //ZY planes
    new G4RotationMatrix(-pi/2,pi/2,pi/2);

//   G4int	checker = sideCoverage;
/*
      G4PVPlacement(G4RotationMatrix *pRot,
            const G4ThreeVector &tlate,
                  G4LogicalVolume *pCurrentLogical,
            const G4String& pName,
                  G4LogicalVolume *pMotherLogical,
                  G4bool pMany,
                  G4int  pCopyNo,
                  G4bool pSurfChk=false);
      // Initialise a single volume, positioned in a frame which is rotated by
      // *pRot and traslated by tlate, relative to the coordinate system of the
      // mother volume pMotherLogical.
      // If pRot=0 the volume is unrotated with respect to its mother.
      // The physical volume is added to the mother's logical volume.
      // Arguments particular to G4PVPlacement:
      //   pMany Currently NOT used. For future use to identify if the volume
      //         is meant to be considered an overlapping structure, or not.
      //   pCopyNo should be set to 0 for the first volume of a given type.
      //   pSurfChk if true activates check for overlaps with existing volumes.
      // This is a very natural way of defining a physical volume, and is
      // especially useful when creating subdetectors: the mother volumes are
      // not placed until a later stage of the assembly program.
*/

  // if(!gasDetector) {
  ActarSimGasDetectorConstruction* gasDetector = (ActarSimGasDetectorConstruction*)(detConstruction->GetGasDetector());
  //ActarSimGasDetectorConstruction gasDetector = detConstruction->GetGasDetector();
    G4double xGasBox = gasDetector->GetGasBoxSizeX();
    G4double zGasBox = gasDetector->GetGasBoxSizeZ();
    // }


    //An aluminium plate to see the Pads active area
    //G4double plateSizeX = 32*mm;
    //G4double plateSizeY = yPadSize/2*mm;
    //G4double layerSizeZ = 0.00015*mm;
    G4double layerSizeZ = 0.00012*mm;//I don't have the size of strips so I assume it is 80% of the pitch
    G4double z,a,density;

    G4Box *DSSD_Al_Layer=new G4Box("DSSD_Al_layer",silDSSDBulk_x,silDSSDBulk_y,layerSizeZ);
    //G4Box *MAYA_Al_Layer=new G4Box("MAYA_Al_layer",silBulk_x,silBulk_y,layerSizeZ);

    G4Material* Al =
      new G4Material("Aluminum", z= 13., a= 26.98*g/mole, density= 2.7*g/cm3);

    G4LogicalVolume* DSSD_Al_LayerLog=new G4LogicalVolume(DSSD_Al_Layer,Al,"DSSD_Al_layer");
    //G4LogicalVolume* MAYA_Al_LayerLog=new G4LogicalVolume(MAYA_Al_Layer,Al,"MAYA_Al_layer");

    G4int DSSDAlIterationNumber = 0;
    //G4int MAYAAlIterationNumber = 0;

    // AlLayerPhys=new G4PVPlacement(0,G4ThreeVector( platePosX,platePosY,platePosZ),
    // 				  AlLayerLog,"Al_layer",chamberLog,false,0);

  if(sideCoverage & 0x0001){ // bit1 (lsb) beam output wall
    //iteration on Silicon elements
    /*
    for(G4int rowX=0;rowX<numberOfRowsX;rowX++){  //maybe is rowX=1 the first??
      for(G4int rowY=0;rowY<numberOfRowsY;rowY++){
	iterationNumber++;
        silPhys =
	  new G4PVPlacement(0,G4ThreeVector(-xBoxSilHalfLength + ((rowX+1)*2-1)*(silBulk_x+defectHalfLength),
					    -yBoxSilHalfLength + ((rowY+1)*2-1)*(silBulk_x+defectHalfLength),
					    2*zBoxSilHalfLength + separationFromBox + silBulk_z),
			    silLog, "silPhys", chamberLog, false, iterationNumber);
      }
    }
    */

    for(G4int rowX=0;rowX<2;rowX++){  //maybe is rowX=1 the first??
      for(G4int rowY=0;rowY<2;rowY++){
	/*
        iterationDSSDNumber++;
        silDSSDPhys =
          new G4PVPlacement(0,G4ThreeVector( (rowX-0.5)*2*(silDSSDBulk_x+defectHalfLength),
					     (rowY-0.5)*2*(silDSSDBulk_x+defectHalfLength),
					     //2*zBoxSilHalfLength + separationFromBox + silBulk_z),
					     zBoxSilHalfLength - separationFromBox - silDSSDBulk_z),
			    silDSSDLog, "silDSSDPhys", chamberLog, false, iterationDSSDNumber);
	*/
        iterationNumber++;
        silPhys =
          new G4PVPlacement(0,G4ThreeVector( (rowX-0.5)*2*(silDSSDBulk_x+defectHalfLength),
					     (rowY-0.5)*2*(silDSSDBulk_x+defectHalfLength),
					     //zBoxSilHalfLength - separationFromBox - silDSSDBulk_z),
					     zGasBox + zBoxSilHalfLength),
			    silDSSDLog, "silPhys", chamberLog, false, iterationNumber);

	DSSDAlIterationNumber++;
	//Al Layers
        DSSD_Al_LayerPhys
	  =new G4PVPlacement(0,G4ThreeVector( (rowX-0.5)*2*(silDSSDBulk_x+defectHalfLength),
					      (rowY-0.5)*2*(silDSSDBulk_x+defectHalfLength),
					      zGasBox + zBoxSilHalfLength - (silDSSDBulk_z+layerSizeZ)),
				  DSSD_Al_LayerLog,"DSSD_Al_layer",chamberLog,false,DSSDAlIterationNumber);

	DSSDAlIterationNumber++;
        DSSD_Al_LayerPhys
	  =new G4PVPlacement(0,G4ThreeVector( (rowX-0.5)*2*(silDSSDBulk_x+defectHalfLength),
					      (rowY-0.5)*2*(silDSSDBulk_x+defectHalfLength),
					      zGasBox + zBoxSilHalfLength + (silDSSDBulk_z+layerSizeZ)),
				  DSSD_Al_LayerLog,"DSSD_Al_layer",chamberLog,false,DSSDAlIterationNumber);

      }
    }

    /*
    iterationNumber++;
    silPhys =
      new G4PVPlacement(0,G4ThreeVector(0,
				        0,
					2*zBoxSilHalfLength + separationFromBox + silBulk_z),
			silLog, "silPhys", chamberLog, false, iterationNumber);
    */
  }

  //PLANES ZX
  //a different rotation has to be introduced on each side...
  if((sideCoverage >> 1) & 0x0001){ // bit2 lower [bottom] (gravity based) wall
    for(G4int rowZ=0;rowZ<numberOfRowsZ;rowZ++){
      for(G4int rowX=0;rowX<numberOfRowsX;rowX++){
          iterationNumber++;
          silPhys =
            new G4PVPlacement(rotBottom,G4ThreeVector(-xBoxSilHalfLength + ((rowX+1)*2-1)*(silBulk_x+defectHalfLength),
	 				         -(yBoxSilHalfLength + separationFromBox + silBulk_z),
					         ((rowZ+1)*2-1)*(silBulk_x+defectHalfLength)),
					         silLog, "silPhys", chamberLog, false, iterationNumber);
      }
    }
  }
  if((sideCoverage >> 2) & 0x0001){ // bit3 upper [top] (gravity based) wall
    for(G4int rowZ=0;rowZ<numberOfRowsZ;rowZ++){
      for(G4int rowX=0;rowX<numberOfRowsX;rowX++){
          iterationNumber++;
          silPhys =
            new G4PVPlacement(rotTop,G4ThreeVector(-xBoxSilHalfLength + ((rowX+1)*2-1)*(silBulk_x+defectHalfLength),
	 				         yBoxSilHalfLength + separationFromBox + silBulk_z,
					         ((rowZ+1)*2-1)*(silBulk_x+defectHalfLength)),
					         silLog, "silPhys", chamberLog, false, iterationNumber);
      }
    }
  }

  //PLANES ZY
  if((sideCoverage >> 3) & 0x0001){ // bit4 left (from beam point of view) wall

    for(G4int rowZ=0;rowZ<3;rowZ++){
      for(G4int rowY=0;rowY<2;rowY++){
        iterationNumber++;
        silPhys =
          //new G4PVPlacement(rotLeft,G4ThreeVector(xBoxSilHalfLength - separationFromBox - silBulk_z,
          new G4PVPlacement(rotLeft,G4ThreeVector(xGasBox + xBoxSilHalfLength,
						  (rowY-0.5)*2*(silBulk_y+defectHalfLength),
						  (rowZ-1)*2*(silBulk_x+defectHalfLength)),
			    silLog, "silPhys", chamberLog, false, iterationNumber);
	/*
	MAYAAlIterationNumber++;
	//Al Layers
        MAYA_Al_LayerPhys
	  =new G4PVPlacement(rotLeft,G4ThreeVector(xGasBox + xBoxSilHalfLength  - (silBulk_z+layerSizeZ),
						  (rowY-0.5)*2*(silBulk_y+defectHalfLength),
						  (rowZ-1)*2*(silBulk_x+defectHalfLength)),
				  MAYA_Al_LayerLog,"MAYA_Al_layer",chamberLog,false,MAYAAlIterationNumber);
	*/
      }
    }
  }

  if((sideCoverage >> 4) & 0x0001){ // bit5 right (from beam point of view) wall
    /*
    iterationNumber++;
    silPhys = new G4PVPlacement(rotRight,G4ThreeVector(-(xBoxSilHalfLength - separationFromBox - silBulk_z),
						       0,
						       0),
				silLog, "silPhys", chamberLog, false, iterationNumber);
    */
    for(G4int rowZ=0;rowZ<3;rowZ++){
      for(G4int rowY=0;rowY<2;rowY++){
        iterationNumber++;
        silPhys =
          //new G4PVPlacement(rotRight,G4ThreeVector(-(xBoxSilHalfLength - separationFromBox - silBulk_z),
          new G4PVPlacement(rotRight,G4ThreeVector(-(xGasBox + xBoxSilHalfLength),
						  (rowY-0.5)*2*(silBulk_y+defectHalfLength),
						  (rowZ-1)*2*(silBulk_x+defectHalfLength)),
			    silLog, "silPhys", chamberLog, false, iterationNumber);
	/*
	MAYAAlIterationNumber++;
	//Al Layers
        MAYA_Al_LayerPhys
	  =new G4PVPlacement(rotRight,G4ThreeVector(-(xGasBox + xBoxSilHalfLength)  + (silBulk_z+layerSizeZ),
						  (rowY-0.5)*2*(silBulk_y+defectHalfLength),
						  (rowZ-1)*2*(silBulk_x+defectHalfLength)),
				  MAYA_Al_LayerLog,"MAYA_Al_layer",chamberLog,false,MAYAAlIterationNumber);
	*/
      }
    }
  }

  if((sideCoverage >> 5) & 0x0001){ // bit6 (msb) beam entrance wall
    for(G4int rowX=0;rowX<numberOfRowsX;rowX++){
      for(G4int rowY=0;rowY<numberOfRowsY;rowY++){
        iterationNumber++;
        silPhys =
          new G4PVPlacement(rotBack,G4ThreeVector(-xBoxSilHalfLength + ((rowX+1)*2-1)*(silBulk_y+defectHalfLength),
	 				          -yBoxSilHalfLength + ((rowY+1)*2-1)*(silBulk_x+defectHalfLength),
					          -separationFromBox - silBulk_z),
			                          silLog, "silPhys", chamberLog, false, iterationNumber);
      }
    }
  }

  //------------------------------------------------
  // Sensitive detectors
  //------------------------------------------------
  silLog->SetSensitiveDetector( detConstruction->GetSilSD() );
  silDSSDLog->SetSensitiveDetector( detConstruction->GetSilSD() );

  //------------------------------------------------------------------
  // Visualization attributes
  //------------------------------------------------------------------
  G4VisAttributes* silVisAtt1 = new G4VisAttributes(G4Colour(0,1,0));
  silVisAtt1->SetVisibility(true);
  silLog->SetVisAttributes(silVisAtt1);
  G4VisAttributes* silDSSDVisAtt1 = new G4VisAttributes(G4Colour(0,0.75,0));
  silDSSDVisAtt1->SetVisibility(true);
  silDSSDLog->SetVisAttributes(silDSSDVisAtt1);

  G4VisAttributes* DSSD_Al_LayerVisAtt= new G4VisAttributes(G4Colour(1.0,0.,1.0));
  DSSD_Al_LayerVisAtt->SetVisibility(false);
  DSSD_Al_LayerLog->SetVisAttributes(DSSD_Al_LayerVisAtt);
  /*
  G4VisAttributes* MAYA_Al_LayerVisAtt= new G4VisAttributes(G4Colour(1.0,0.333,1.0));
  MAYA_Al_LayerVisAtt->SetVisibility(false);
  MAYA_Al_LayerLog->SetVisAttributes(MAYA_Al_LayerVisAtt);
  */

  return silPhys;
  return silDSSDPhys;
}

void ActarSimSilDetectorConstruction::SetSilBulkMaterial (G4String mat) {
  //
  // Set the material the silicon bulk is made of
  //

  G4Material* pttoMaterial = G4Material::GetMaterial(mat);
  if (pttoMaterial) silBulkMaterial = pttoMaterial;
}


void ActarSimSilDetectorConstruction::UpdateGeometry() {
  //
  // Updates Silicon detector
  //

  Construct(detConstruction->GetWorldLogicalVolume());
  G4RunManager::GetRunManager()->
    DefineWorldVolume(detConstruction->GetWorldPhysicalVolume());
}


void ActarSimSilDetectorConstruction::PrintDetectorParameters() {
  //
  // Prints Silicon detector parameters. To be filled
  //
  G4cout << "##################################################################"
	       << G4endl
	       << "####  ActarSimSilDetectorConstruction::PrintDetectorParameters() ####"
	       << G4endl
         << " The silicon coverage is " <<  sideCoverage << G4endl
         << "Reminder: 6 bits to indicate which sil wall is present (1) or absent (0)" << G4endl
         << "bit1 (lsb) beam output wall, bit2 lower (gravity based) wall" << G4endl
         << "bit3 upper (gravity based) wall, bit4 left (from beam point of view) wall" << G4endl
         << "bit5 right (from beam point of view) wall, bit6 (msb) beam entrance wall" << G4endl << G4endl;
         G4cout << " The silicon material is: " << silBulkMaterial  << G4endl;
        G4cout << " The silicon parameters are: " << G4endl
           << " xBoxSilHalfLength = " <<  xBoxSilHalfLength/mm
           << "mm,  yBoxSilHalfLength = " <<  yBoxSilHalfLength/mm
           << "mm,  zBoxSilHalfLength = " <<  zBoxSilHalfLength/mm << " mm" << G4endl ;
  G4cout << "##################################################################"
	       << G4endl;
}
