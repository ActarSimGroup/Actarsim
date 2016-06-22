// - AUTHOR: Hector Alvarez-Pol 01/2006
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

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
  G4double fieldComponents[6];      ///< The EM field: follows the G4ElectroMagneticField convention

  G4ChordFinder* theChordFinder;    ///< Chord parameter
  G4MagIntegratorStepper* stepper;  ///< Integrator stepper
  G4EqMagElectricField* equation;   ///< Equation

public:
  ActarSimUniformEMField();
  ActarSimUniformEMField(const G4ThreeVector magFieldVector,
			 const G4ThreeVector elecFieldVector);
  ActarSimUniformEMField(const  ActarSimUniformEMField &p);
  ~ActarSimUniformEMField();

  ActarSimUniformEMField operator = (const ActarSimUniformEMField &p);

  void SetFieldValue(const G4ThreeVector magFieldVector,
		     const G4ThreeVector elecFieldVector);

  //Set the field as pure electric or magnetic    NOT YET USED...
  void SetPureElectricFieldValue(G4ThreeVector fieldVector);
  void SetPureMagneticFieldValue(G4ThreeVector fieldVector);

  ///This is a virtual function in G4ElectroMagneticField to be instanciated here
  ///Return as Bfield[0], [1], [2] the magnetic field x, y & z components
  /// and   as Bfield[3], [4], [5] the electric field x, y & z components
  void GetFieldValue(const G4double thePoint[4], G4double* theField) const;

  G4ThreeVector GetMagneticFieldValue();
  G4ThreeVector GetElectricFieldValue();

  G4bool DoesFieldChangeEnergy() const {return true;}
  //does not work in other way???

  // void DefineUnits();
protected:
  /// Find the global Field Manager
  G4FieldManager* GetGlobalFieldManager();   // static
};
#endif
