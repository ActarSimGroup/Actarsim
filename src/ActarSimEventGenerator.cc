// - AUTHOR: B. Fernandez-Dominguez 10/2006
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/
//////////////////////////////////////////////////////////////////
/// \class ActarSimEventGenerator
/// Event generator kinematics reader
/////////////////////////////////////////////////////////////////

#include "ActarSimEventGenerator.hh"

using namespace std;

//////////////////////////////////////////////////////////////////
/// Constructor, all job here
ActarSimEventGenerator::ActarSimEventGenerator(){

  // G4double a,b,c,d,e,f;
  // G4double Integral = 0.;
  int i;
  // int nbins;
  // const G4int MAX = 140;
  // char buffer[MAX];

  //Initial values
  for(i=0;i<1000;i++) {
    LabAngle_scatt[i]  = 0.;
    LabEnergy_scatt[i] = 0.;
    LabAngle_recoil[i]  = 0.;
    LabEnergy_recoil[i] = 0.;
  }

  for(i=0;i<5000;i++) {
    theta_xsec[i] = 0.;
    CrossSection_t[i] = 0.;
    Icross_section[i] = 0.;
  }

  //Read Kinematics File
  /*
    ifstream infile("data/kine/kinematics_77nidp.dat");

    for(i=0;i<24;i++)
      {
	infile.getline(buffer, MAX);
	//readout header
	//	  G4cout << "i=" << i << G4endl;
      }

    i = -1;
    while( !infile.eof() )
      {
	i++;
	infile >> LabAngle_scatt[i] >> a>> LabEnergy_scatt[i] >> b >> c >> d >>
	  LabAngle_recoil[i] >> LabEnergy_recoil[i] >> e >> f ;

	//	G4cout<<"Lab_Angle="<<LabAngle_scatt[i]<<"  Lab_Energy="<<LabEnergy_scatt[i]<<G4endl;


      }

  */

  //Read cross section file

  /*

 ifstream infile_xsec("data/kine/xs_77ni_dp_0MeV_p.dat");

 i = -1;
 while( !infile_xsec.eof() )
   {
     i++;
     infile_xsec >> theta_xsec[i] >> CrossSection_t[i];	//readout of data
     //G4cout << "i="<<i << " " << T_xsec[i] << " " << CrossSection_t[i] <<  G4endl;
     nbins = i;
   }


 //Integral of cross section

 for(i=0;i<nbins;i++)
   {
     Integral += CrossSection_t[i];
     //G4cout << " Integral " << Integral << G4endl;
   }

 //Building the probability distribution (Integral of the xs between 0 and a given theta)

 for(i=0;i<nbins;i++)
   {
     if(i==0)
       {
	 Icross_section[0] = CrossSection_t[0]/Integral;
	 // G4cout << "Cross_section[0] " << CrossSection_t[0]  << G4endl;
	 //G4cout << "Icross_section[0] " <<  Icross_section[0] << G4endl;
       }
     if (i!=0)
       {
	 Icross_section[i] = Icross_section[i-1] + CrossSection_t[i]/Integral;
	 //G4cout << "i" << i << "Icross_section[i]" <<  Icross_section[i]   << G4endl;
       }

   }

  */

}

//////////////////////////////////////////////////////////////////
/// Destructor
ActarSimEventGenerator::~ActarSimEventGenerator() {
}
