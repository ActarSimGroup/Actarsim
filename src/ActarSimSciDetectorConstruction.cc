/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez
//*-- Date: 04/2008
//*-- Last Update: 07/01/15 by Hector Alvarez
// --------------------------------------------------------------
// Description:
//   Scintillator detector description
//
// --------------------------------------------------------------
// Comments:
//
//   - 17/04/08 Modularizing the ACTAR geometry
//
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include "ActarSimSciDetectorConstruction.hh"
#include "ActarSimDetectorConstruction.hh"
#include "ActarSimSciDetectorMessenger.hh"
#include "ActarSimROOTAnalysis.hh"
#include "ActarSimSciSD.hh"

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

ActarSimSciDetectorConstruction::
ActarSimSciDetectorConstruction(ActarSimDetectorConstruction* det)
  :	sciBulkMaterial(0),detConstruction(det) {
  //
  // Constructor. Sets the material and the pointer to the Messenger
  //


  SetSciBulkMaterial("CsI");

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

  SetXBoxSciHalfLength(100*cm);
  SetYBoxSciHalfLength(100*cm);
  SetZBoxSciHalfLength(100*cm);

  // create commands for interactive definition of the calorimeter
  sciMessenger = new ActarSimSciDetectorMessenger(this);
}


ActarSimSciDetectorConstruction::~ActarSimSciDetectorConstruction(){
  //
  // Destructor
  //
  delete sciMessenger;
}


G4VPhysicalVolume* ActarSimSciDetectorConstruction::Construct(G4LogicalVolume* worldLog) {
  //
  // Wrap for the construction functions within the TOF
  //

  //Introduce here other constructors for materials around the TOF (windows, frames...)
  //which can be controlled by the calMessenger
  //ConstructTOFWorld(worldLog);
  return ConstructSci(worldLog);
}



G4VPhysicalVolume* ActarSimSciDetectorConstruction::ConstructSci(G4LogicalVolume* worldLog) {
  //
  //  Constructs the scintillator detector elements
  //
  //talk with Patricia, Pang and Herve, May 08
  //1) Introduce Silicon and CsI(Tl) detectors all around. Use MUST style:
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

  //MUST-like scintillator detectors (half-sides)
  //I left some "air" in between crystals. Half-length are 0,1 shorter.
  G4double sciBulk_x = 12.0*mm;   //half length=12.5mm
  G4double sciBulk_y = 12.0*mm;   //half length=12.5mm
  G4double sciBulk_z = 15.0*mm;   //half length=15.0mm

  G4double defectHalfLength = 0.5*mm;
  G4double separationFromBox = 25*mm;

  //Position of the GasBox inside the Chamber
  G4double zGasBoxPosition=detConstruction->GetZGasBoxPosition();

  // Printing the final settings...
  G4cout << "##################################################################"
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
	 << G4endl;


  G4LogicalVolume* sciLog(0);
  G4VPhysicalVolume* sciPhys(0);

//  G4Trd* sciBox = //testing the direction after rotation...
//    new G4Trd("sciBox", 5, 12, 5, 12, 30);
  G4Box* sciBox =
    new G4Box("sciBox", sciBulk_x, sciBulk_y, sciBulk_z);

  sciLog =
    new G4LogicalVolume(sciBox, sciBulkMaterial, "sciLog");

  //As a function of the lateral coverage
  // 6 bits to indicate which sci wall is present (1) or absent (0)
  // order is:
  // bit1 (lsb) beam output wall
  // bit2 lower (gravity based) wall
  // bit3 upper (gravity based) wall
  // bit4 left (from beam point of view) wall
  // bit5 right (from beam point of view) wall
  // bit6 (msb) beam entrance wall

  //for the moment, let's assume square scintillator...
  //if not, more sophisticated approach needed!
  G4int numberOfRowsX = ((G4int) (2*xBoxSciHalfLength/(2*(sciBulk_x+defectHalfLength)))) - 1;
  if( (numberOfRowsX+1)*sciBulk_x <= 2*xBoxSciHalfLength ) numberOfRowsX++;
  G4int numberOfRowsY = ((G4int) (2*yBoxSciHalfLength/(2*(sciBulk_x+defectHalfLength)))) - 1;
  if( (numberOfRowsY+1)*sciBulk_x <= 2*yBoxSciHalfLength ) numberOfRowsY++;
  G4int numberOfRowsZ = ((G4int) (2*zBoxSciHalfLength/(2*(sciBulk_x+defectHalfLength)))) - 1;
  if( (numberOfRowsZ+1)*sciBulk_x <= 2*zBoxSciHalfLength ) numberOfRowsZ++;

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
  if(sideCoverage & 0x0001){ // bit1 (lsb) beam output wall
    //iteration on Scintillator elements
    /*
    for(G4int rowX=0;rowX<numberOfRowsX;rowX++){  //maybe is rowX=1 the first??
      for(G4int rowY=0;rowY<numberOfRowsY;rowY++){
        iterationNumber++;
        sciPhys =
          new G4PVPlacement(0,G4ThreeVector(-xBoxSciHalfLength + ((rowX+1)*2-1)*(sciBulk_x+defectHalfLength),
	 				    -yBoxSciHalfLength + ((rowY+1)*2-1)*(sciBulk_x+defectHalfLength),
					    2*zBoxSciHalfLength + separationFromBox + sciBulk_z),
			                    sciLog, "sciPhys", worldLog, false, iterationNumber);
      }
    }
    */
    for(G4int rowX=0;rowX<4;rowX++){  //maybe is rowX=1 the first??
      for(G4int rowY=0;rowY<4;rowY++){
        iterationNumber++;
        sciPhys =
          new G4PVPlacement(0,G4ThreeVector( (rowX-1.5)*2*(sciBulk_x+defectHalfLength),
					     (rowY-1.5)*2*(sciBulk_x+defectHalfLength),
					     2*zBoxSciHalfLength + separationFromBox + sciBulk_z - zGasBoxPosition),
			    sciLog, "sciPhys", worldLog, false, iterationNumber);
      }
    }
  }

  //PLANES ZX
  //a different rotation has to be introduced on each side...
  if((sideCoverage >> 1) & 0x0001){ // bit2 lower [bottom] (gravity based) wall
    for(G4int rowZ=0;rowZ<numberOfRowsZ;rowZ++){
      for(G4int rowX=0;rowX<numberOfRowsX;rowX++){
          iterationNumber++;
          sciPhys =
            new G4PVPlacement(rotBottom,G4ThreeVector(-xBoxSciHalfLength + ((rowX+1)*2-1)*(sciBulk_x+defectHalfLength),
	 				         -(yBoxSciHalfLength + separationFromBox + sciBulk_z),
					         ((rowZ+1)*2-1)*(sciBulk_x+defectHalfLength) - zGasBoxPosition),
					         sciLog, "sciPhys", worldLog, false, iterationNumber);
      }
    }
  }
  if((sideCoverage >> 2) & 0x0001){ // bit3 upper [top] (gravity based) wall
    for(G4int rowZ=0;rowZ<numberOfRowsZ;rowZ++){
      for(G4int rowX=0;rowX<numberOfRowsX;rowX++){
          iterationNumber++;
          sciPhys =
            new G4PVPlacement(rotTop,G4ThreeVector(-xBoxSciHalfLength + ((rowX+1)*2-1)*(sciBulk_x+defectHalfLength),
	 				         yBoxSciHalfLength + separationFromBox + sciBulk_z,
					         ((rowZ+1)*2-1)*(sciBulk_x+defectHalfLength) - zGasBoxPosition),
					         sciLog, "sciPhys", worldLog, false, iterationNumber);
      }
    }
  }

  //PLANES ZY
  if((sideCoverage >> 3) & 0x0001){ // bit4 left (from beam point of view) wall
    /*
    for(G4int rowZ=0;rowZ<numberOfRowsZ;rowZ++){
      for(G4int rowY=0;rowY<numberOfRowsY;rowY++){
        iterationNumber++;
        sciPhys =
          new G4PVPlacement(rotLeft,G4ThreeVector(xBoxSciHalfLength + separationFromBox + sciBulk_z,
	 				       -yBoxSciHalfLength + ((rowY+1)*2-1)*(sciBulk_x+defectHalfLength),
					       ((rowZ+1)*2-1)*(sciBulk_x+defectHalfLength)),
                                               sciLog, "sciPhys", worldLog, false, iterationNumber);
      }
    }
    */
    for(G4int rowZ=0;rowZ<6;rowZ++){
      for(G4int rowY=0;rowY<4;rowY++){
        iterationNumber++;
        sciPhys =
          new G4PVPlacement(rotLeft,G4ThreeVector(xBoxSciHalfLength + separationFromBox + sciBulk_z,
						  (rowY-1.5)*2*(sciBulk_x+defectHalfLength),
						  zBoxSciHalfLength+(rowZ-2.5)*2*(sciBulk_x+defectHalfLength) - zGasBoxPosition),
                                               sciLog, "sciPhys", worldLog, false, iterationNumber);
      }
    }

  }

  if((sideCoverage >> 4) & 0x0001){ // bit5 right (from beam point of view) wall
    /*
    for(G4int rowZ=0;rowZ<numberOfRowsZ;rowZ++){
      for(G4int rowY=0;rowY<numberOfRowsY;rowY++){
        iterationNumber++;
        sciPhys =
          new G4PVPlacement(rotRight,G4ThreeVector(-(xBoxSciHalfLength + separationFromBox + sciBulk_z),
	 				       -yBoxSciHalfLength + ((rowY+1)*2-1)*(sciBulk_x+defectHalfLength),
					       ((rowZ+1)*2-1)*(sciBulk_x+defectHalfLength)),
                                               sciLog, "sciPhys", worldLog, false, iterationNumber);
      }
    }
    */

    for(G4int rowZ=0;rowZ<2;rowZ++){
      for(G4int rowY=0;rowY<2;rowY++){
	      iterationNumber++;
        sciPhys =
          new G4PVPlacement(rotRight,G4ThreeVector(-(xBoxSciHalfLength + separationFromBox + sciBulk_z),
						   (rowY-0.5)*2*(sciBulk_y+defectHalfLength),
						   zBoxSciHalfLength+(rowZ-0.5)*2*(sciBulk_x+defectHalfLength) - zGasBoxPosition),
						   sciLog, "sciPhys", worldLog, false, iterationNumber);

      }
    }


  }

  if((sideCoverage >> 5) & 0x0001){ // bit6 (msb) beam entrance wall
    for(G4int rowX=0;rowX<numberOfRowsX;rowX++){
      for(G4int rowY=0;rowY<numberOfRowsY;rowY++){
        iterationNumber++;
        sciPhys =
          new G4PVPlacement(rotBack,G4ThreeVector(-xBoxSciHalfLength + ((rowX+1)*2-1)*(sciBulk_x+defectHalfLength),
	 				          -yBoxSciHalfLength + ((rowY+1)*2-1)*(sciBulk_x+defectHalfLength),
					          -separationFromBox - sciBulk_z - zGasBoxPosition),
			                          sciLog, "sciPhys", worldLog, false, iterationNumber);
      }
    }
  }

  //------------------------------------------------
  // Sensitive detectors
  //------------------------------------------------
  sciLog->SetSensitiveDetector( detConstruction->GetSciSD() );

  //------------------------------------------------------------------
  // Visualization attributes
  //------------------------------------------------------------------
  G4VisAttributes* sciVisAtt1 = new G4VisAttributes(G4Colour(0,1,1));
  sciVisAtt1->SetVisibility(true);
  sciLog->SetVisAttributes(sciVisAtt1);

  return sciPhys;
}

void ActarSimSciDetectorConstruction::SetSciBulkMaterial (G4String mat) {
  //
  // Set the material the scintillator bulk is made of
  //

  G4Material* pttoMaterial = G4Material::GetMaterial(mat);
  if (pttoMaterial) sciBulkMaterial = pttoMaterial;
}


void ActarSimSciDetectorConstruction::UpdateGeometry() {
  //
  // Updates Scintillator detector
  //

  Construct(detConstruction->GetWorldLogicalVolume());
  G4RunManager::GetRunManager()->
    DefineWorldVolume(detConstruction->GetWorldPhysicalVolume());
}


void ActarSimSciDetectorConstruction::PrintDetectorParameters() {
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
