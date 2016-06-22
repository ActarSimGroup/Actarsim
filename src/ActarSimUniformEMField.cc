// - AUTHOR: Hector Alvarez-Pol 05/2005
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/
//////////////////////////////////////////////////////////////////
/// \class ActarSimUniformEMField
/// Uniform electric and magnetic fields definition
/////////////////////////////////////////////////////////////////

#include "ActarSimUniformEMField.hh"
#include "G4FieldManager.hh"
#include "G4MagneticField.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4EqMagElectricField.hh"
#include "G4ClassicalRK4.hh"
#include "G4ChordFinder.hh"
#include "G4TransportationManager.hh"

//////////////////////////////////////////////////////////////////
///  Default zero field constructor
ActarSimUniformEMField::ActarSimUniformEMField() {
  fieldComponents[0] = fieldComponents[1] = fieldComponents[2] = 0.;
  fieldComponents[3] = fieldComponents[4] = fieldComponents[5] = 0.;

  equation = new G4EqMagElectricField(this);
  stepper = new G4ClassicalRK4(equation);
  GetGlobalFieldManager()->SetDetectorField(this);
  theChordFinder = new G4ChordFinder((G4MagneticField*)this, 1.0e-2 * mm, stepper);
  GetGlobalFieldManager()->SetChordFinder(theChordFinder);
  //DefineUnits();
}

//////////////////////////////////////////////////////////////////
///  Constructor with initial values for the electric an magnetic fields
ActarSimUniformEMField::ActarSimUniformEMField(const G4ThreeVector magFieldVector,
					       const G4ThreeVector elecFieldVector) {
  equation = new G4EqMagElectricField(this);
  stepper = new G4ClassicalRK4(equation);
  GetGlobalFieldManager()->SetDetectorField(this);
  theChordFinder = new G4ChordFinder((G4MagneticField*)this, 1.0e-2 * mm, stepper);
  GetGlobalFieldManager()->SetChordFinder(theChordFinder);

  SetFieldValue(magFieldVector, elecFieldVector);
  //DefineUnits();
}

//////////////////////////////////////////////////////////////////
///  Copy constructor
ActarSimUniformEMField::ActarSimUniformEMField(const  ActarSimUniformEMField &p)
  : G4ElectroMagneticField(p) {
  for(G4int i=0;i<6;i++)
    fieldComponents[i] = p.fieldComponents[i];

  equation = new G4EqMagElectricField(this);
  stepper = new G4ClassicalRK4(equation);
  GetGlobalFieldManager()->SetDetectorField(this);
  theChordFinder = new G4ChordFinder((G4MagneticField*)this, 1.0e-2 * mm, stepper);
  GetGlobalFieldManager()->SetChordFinder(theChordFinder);
}

//////////////////////////////////////////////////////////////////
///  Operator =
ActarSimUniformEMField ActarSimUniformEMField::operator = (const ActarSimUniformEMField &p) {
  if (&p == this) return *this;
  for (G4int i=0; i<6; i++)
    fieldComponents[i] = p.fieldComponents[i];
  return *this;
}

//////////////////////////////////////////////////////////////////
///  Destructor
ActarSimUniformEMField::~ActarSimUniformEMField() {
  GetGlobalFieldManager()->SetDetectorField(0);
}

//////////////////////////////////////////////////////////////////
/// Set the value of the Global Field to fieldVector along Y
void ActarSimUniformEMField::SetFieldValue(const G4ThreeVector magFieldVector,
					   const G4ThreeVector elecFieldVector) {
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

//////////////////////////////////////////////////////////////////
/// Set the value of the Electric Field to fieldVector
void ActarSimUniformEMField::SetPureElectricFieldValue(G4ThreeVector fieldVector) {
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

//////////////////////////////////////////////////////////////////
/// Set the value of the Magnetic Field to fieldVector
void ActarSimUniformEMField::SetPureMagneticFieldValue(G4ThreeVector fieldVector) {
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

//////////////////////////////////////////////////////////////////
/// This is a virtual function in G4ElectroMagneticField to be instanciated here:
///   Return as Bfield[0], [1], [2] the magnetic field x, y & z components
///   and   as Bfield[3], [4], [5] G4the electric field x, y & z components
void ActarSimUniformEMField::GetFieldValue(const G4double thePoint[4],
					   G4double* theField) const {
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

//////////////////////////////////////////////////////////////////
/// Returns the uniform magnetic field value
G4ThreeVector ActarSimUniformEMField::GetMagneticFieldValue() {
  return G4ThreeVector(fieldComponents[0], fieldComponents[1], fieldComponents[2]);
}

//////////////////////////////////////////////////////////////////
/// Returns the uniform electric field value
G4ThreeVector ActarSimUniformEMField::GetElectricFieldValue() {
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

//////////////////////////////////////////////////////////////////
/// Getting a local pointer to the global field manager
G4FieldManager*  ActarSimUniformEMField::GetGlobalFieldManager() {
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
