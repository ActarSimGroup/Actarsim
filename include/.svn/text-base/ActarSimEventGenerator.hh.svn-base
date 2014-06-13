/////////////////////////////////////////////////////////////////
//
//*-- Created by: 10/05/06 B. Fernandez-Dominguez bfd@ns.ph.liv.ac.uk
// --------------------------------------------------------------
// Description:
//   Event generator kinematics reader
//
// --------------------------------------------------------------
// Comments: Based on R3B read kinematics
// 
// --------------------------------------------------------------
//
/////////////////////////////////////////////////////////////////

#ifndef EVENTGENERATOR_H
#define EVENTGENERATOR_H

#include "globals.hh"
#include <fstream>
#include <iostream>
#include <string>

class ActarSimEventGenerator
{
  public:
    ActarSimEventGenerator();
   ~ActarSimEventGenerator();
   
  public:
//    File *pFile;
  G4double LabAngle_scatt[1000];
  G4double LabEnergy_scatt[1000];	
  G4double LabAngle_recoil[1000];
  G4double LabEnergy_recoil[1000];
  G4double theta_xsec[5000];
  G4double CrossSection_t[5000];

//Integrated matrix (cross section) 
  G4double Icross_section[5000];
						
};
#endif
