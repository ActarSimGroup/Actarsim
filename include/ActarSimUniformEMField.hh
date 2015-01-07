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
//   - 29/11/05 Updated to include electric fields. The new 
//          implementation allows the definition of arbitrary uniform
//          electric or magnetic fields working simultaneously
//   - 24/05/05 Created based on ExN02
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#ifndef ActarSimUniformEMField_H
#define ActarSimUniformEMField_H 1

#include "G4ElectroMagneticField.hh"
#include "G4ThreeVector.hh"

class G4ChordFinder;
class G4MagIntegratorStepper;
class G4EqMagElectricField;
class G4FieldManager; 

class ActarSimUniformEMField: public G4ElectroMagneticField {

private:
  
  G4double fieldComponents[6];
  // The EM field: follows the G4ElectroMagneticField convention

  G4ChordFinder* theChordFinder;
  G4MagIntegratorStepper* stepper;
  G4EqMagElectricField* equation;

  
public:
  
  ActarSimUniformEMField();
  ActarSimUniformEMField(const G4ThreeVector magFieldVector, 
			 const G4ThreeVector elecFieldVector);
  ActarSimUniformEMField(const  ActarSimUniformEMField &p); 
  ~ActarSimUniformEMField();  
  
  //assignment operator
  ActarSimUniformEMField operator = (const ActarSimUniformEMField &p);
  
  void SetFieldValue(const G4ThreeVector magFieldVector, 
		     const G4ThreeVector elecFieldVector);
  
  //Set the field as pure electric or magnetic    NOT YET USED... 
  void SetPureElectricFieldValue(G4ThreeVector fieldVector);
  void SetPureMagneticFieldValue(G4ThreeVector fieldVector);
  
  void GetFieldValue(const G4double thePoint[4], G4double* theField) const;
  //This is a virtual function in G4ElectroMagneticField to be instanciated here
  //Return as Bfield[0], [1], [2] the magnetic field x, y & z components
  // and   as Bfield[3], [4], [5] the electric field x, y & z components
  
  G4ThreeVector GetMagneticFieldValue();
  G4ThreeVector GetElectricFieldValue();
  
  G4bool DoesFieldChangeEnergy() const {return true;} 
  //does not work in other way???

  // void DefineUnits();

protected:
  
  // Find the global Field Manager
   G4FieldManager* GetGlobalFieldManager();   // static 
};


#endif
