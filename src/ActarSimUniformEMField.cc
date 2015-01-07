/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol 
//*-- Date: 05/2005
//*-- Last Update: 07/01/15
// --------------------------------------------------------------
// Description:
//  Uniform electric and magnetic fields definition
//
// --------------------------------------------------------------
// Comments:
//
//   - 29/11/05 Updated to include electric fields. The new
//          implementation allows the definition of arbitrary uniform
//          electric or magnetic fields working simultaneously
//   - 24/05/05 Created based on ExN02
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include "ActarSimUniformEMField.hh"
#include "G4FieldManager.hh"
#include "G4MagneticField.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4EqMagElectricField.hh"
#include "G4ClassicalRK4.hh"
#include "G4ChordFinder.hh"
#include "G4TransportationManager.hh"

ActarSimUniformEMField::ActarSimUniformEMField() {
  //
  //  Default zero field constructor
  //
  fieldComponents[0] = fieldComponents[1] = fieldComponents[2] = 0.;
  fieldComponents[3] = fieldComponents[4] = fieldComponents[5] = 0.;

  equation = new G4EqMagElectricField(this);
  stepper = new G4ClassicalRK4(equation);
  GetGlobalFieldManager()->SetDetectorField(this);
  theChordFinder = new G4ChordFinder((G4MagneticField*)this, 1.0e-2 * mm, stepper);
  GetGlobalFieldManager()->SetChordFinder(theChordFinder);
  //DefineUnits();
}


ActarSimUniformEMField::ActarSimUniformEMField(const G4ThreeVector magFieldVector,
					       const G4ThreeVector elecFieldVector) {
  //
  //  Constructor with initial values for the electric an magnetic fields
  //

  equation = new G4EqMagElectricField(this);
  stepper = new G4ClassicalRK4(equation);
  GetGlobalFieldManager()->SetDetectorField(this);
  theChordFinder = new G4ChordFinder((G4MagneticField*)this, 1.0e-2 * mm, stepper);
  GetGlobalFieldManager()->SetChordFinder(theChordFinder);

  SetFieldValue(magFieldVector, elecFieldVector);
  //DefineUnits();
}


ActarSimUniformEMField::ActarSimUniformEMField(const  ActarSimUniformEMField &p)
: G4ElectroMagneticField(p) {
  //
  //  Copy constructor
  //
  for(G4int i=0;i<6;i++)
    fieldComponents[i] = p.fieldComponents[i];

  equation = new G4EqMagElectricField(this);
  stepper = new G4ClassicalRK4(equation);
  GetGlobalFieldManager()->SetDetectorField(this);
  theChordFinder = new G4ChordFinder((G4MagneticField*)this, 1.0e-2 * mm, stepper);
  GetGlobalFieldManager()->SetChordFinder(theChordFinder);
}


ActarSimUniformEMField ActarSimUniformEMField::operator = (const ActarSimUniformEMField &p) {
  if (&p == this) return *this;
  for (G4int i=0; i<6; i++)
    fieldComponents[i] = p.fieldComponents[i];
  return *this;
}


ActarSimUniformEMField::~ActarSimUniformEMField() {
  //
  // Destructor
  //

  GetGlobalFieldManager()->SetDetectorField(0);
}


void ActarSimUniformEMField::SetFieldValue(const G4ThreeVector magFieldVector,
					   const G4ThreeVector elecFieldVector) {
  //
  // Set the value of the Global Field to fieldVector along Y
  //
  if(elecFieldVector!=G4ThreeVector(0.,0.,0.) ||
     magFieldVector!=G4ThreeVector(0.,0.,0.) ) {
    fieldComponents[0] = magFieldVector.x();
    fieldComponents[1] = magFieldVector.y();
    fieldComponents[2] = magFieldVector.z();
    fieldComponents[3] = elecFieldVector.x();
    fieldComponents[4] = elecFieldVector.y();
    fieldComponents[5] = elecFieldVector.z();
  }
  else {
    // If the new field's value is Zero, then it is best to
    //  insure that it is not used for propagation.
    GetGlobalFieldManager()->SetDetectorField(0);
  }
}


void ActarSimUniformEMField::SetPureElectricFieldValue(G4ThreeVector fieldVector) {
  //
  // Set the value of the Electric Field to fieldVector
  //
  if(fieldVector!=G4ThreeVector(0.,0.,0.) ) {
    fieldComponents[0] = 0.;
    fieldComponents[1] = 0.;
    fieldComponents[2] = 0.;
    fieldComponents[3] = fieldVector.x();
    fieldComponents[4] = fieldVector.y();
    fieldComponents[5] = fieldVector.z();
  }
  else {
    // If the new field's value is Zero, then it is best to
    //  insure that it is not used for propagation.
    GetGlobalFieldManager()->SetDetectorField(0);
  }
}

void ActarSimUniformEMField::SetPureMagneticFieldValue(G4ThreeVector fieldVector) {
  //
  // Set the value of the Magnetic Field to fieldVector
  //
  if(fieldVector!=G4ThreeVector(0.,0.,0.) ) {
    fieldComponents[0] = fieldVector.x();
    fieldComponents[1] = fieldVector.y();
    fieldComponents[2] = fieldVector.z();
    fieldComponents[3] = 0.;
    fieldComponents[4] = 0.;
    fieldComponents[5] = 0.;
  }
  else {
    // If the new field's value is Zero, then it is best to
    //  insure that it is not used for propagation.
    GetGlobalFieldManager()->SetDetectorField(0);
  }
}


void ActarSimUniformEMField::GetFieldValue(const G4double thePoint[4],
					   G4double* theField) const {
  //
  //  This is a virtual function in G4ElectroMagneticField to be instanciated here:
  //   Return as Bfield[0], [1], [2] the magnetic field x, y & z components
  //    and   as Bfield[3], [4], [5] G4the electric field x, y & z components
  //
  if(thePoint) {;} //avoid the warning message

  theField[0] = fieldComponents[0];
  theField[1] = fieldComponents[1];
  theField[2] = fieldComponents[2];
  theField[3] = fieldComponents[3];
  theField[4] = fieldComponents[4];
  theField[5] = fieldComponents[5];
  /*
 G4cout  << " GGGGGGGGG  "
       << thePoint[0] << " " << thePoint[1] << " "<< thePoint[2] << " " << thePoint[3] << "      "
       << theField[0] << " "<< theField[1] << " "<< theField[2] << " "<< theField[3] << " "
       << theField[4] << " "<< theField[5] << G4endl;
  */
}


G4ThreeVector ActarSimUniformEMField::GetMagneticFieldValue() {
  //
  // Returns the uniform magnetic field value
  //

  return G4ThreeVector(fieldComponents[0], fieldComponents[1], fieldComponents[2]);
}


G4ThreeVector ActarSimUniformEMField::GetElectricFieldValue() {
  //
  // Returns the uniform electric field value
  //

  return G4ThreeVector(fieldComponents[3],
		       fieldComponents[4],
		       fieldComponents[5]);
}

/*
G4bool DoesFieldChangeEnergy(){
  //
  // Returns true if any electric field  component is present;
  // otherwise false
  //

  if(fieldComponents[3] != 0. ||
     fieldComponents[4] != 0. ||
     fieldComponents[5] != 0. )
    return true;
  else
    return false;
}
*/

G4FieldManager*  ActarSimUniformEMField::GetGlobalFieldManager() {
  //
  // Getting a local pointer to the global field manager
  //

  return G4TransportationManager::GetTransportationManager()->GetFieldManager();
}

/*
#include "G4UnitsTable.hh"
void ActarSimUniformEMField::DefineUnits() {
   //
   // Defining units for the electric field...why G4 has not the Electric field
   // units defined?
   //

   //G4UnitDefinition(const G4String& name, const G4String& symbol,
   //                 const G4String& category, G4double value);

   G4UnitDefinition("volt/meter","V/m","Electric Field",volt/meter);
   G4UnitDefinition("kilovolt/meter","kV/m","Electric Field",kilovolt/meter);
   G4UnitDefinition("kilovolt/centimeter","kV/cm","Electric Field",kilovolt/centimeter);

 }
*/
