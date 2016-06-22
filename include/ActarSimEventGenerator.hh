// - AUTHOR: B. Fernandez-Dominguez 10/2006
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef EVENTGENERATOR_H
#define EVENTGENERATOR_H

#include "globals.hh"
#include <fstream>
#include <iostream>
#include <string>

class ActarSimEventGenerator {
public:
  ActarSimEventGenerator();
  ~ActarSimEventGenerator();

  //    File *pFile;
  G4double LabAngle_scatt[1000];            ///< LabAngle_scatt
  G4double LabEnergy_scatt[1000];           ///< LabEnergy_scatt
  G4double LabAngle_recoil[1000];           ///< LabAngle_recoil
  G4double LabEnergy_recoil[1000];          ///< LabEnergy_recoil
  G4double theta_xsec[5000];                ///< theta_xsec
  G4double CrossSection_t[5000];            ///< CrossSection_t
  G4double Icross_section[5000];            ///< Integrated matrix (cross section)
};
#endif
