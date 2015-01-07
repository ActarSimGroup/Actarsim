/////////////////////////////////////////////////////////////////
//*-- AUTHOR: (Original FORTRAN code) Wolfgang Mittig
//*-- AUTHOR (translation to C++): Hector Alvarez-Pol
//*-- Date: 11/2005
//*-- Last Update: 1/12/14
// --------------------------------------------------------------
// Description:
//   PROGRAM CINE CINEMATIQUE RELATIVISTIQUE
//
// --------------------------------------------------------------
// Comments:
//   - 02/11/05 translation from the original FORTRAN 
//      to Geant4-like C++
// 
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include "ActarSimCinePrimGenerator.hh"

#include "globals.hh"
#include "Randomize.hh"

using namespace std;

ActarSimCinePrimGenerator::ActarSimCinePrimGenerator() {
  //
  // Constructor
  //

  //
  //  Original program needs the following data:
  //  char:     1 Title 
  //  G4double: 1-4 Masses 1 to 4,  (CIRE)
  //            5   Reaction Q,     (CIRE)
  //            6   Excitation of scattered particle,
  //            7   Excitation of the recoil
  //            8-9 Lab Energy 1 and 2 
  //        (min/max energies when making the table?) (ELAB1 is isued in CIRE)
  //            10  Delta E (step for energies when making the table?)
  //            11-13 Theta angles 1 and 2
  //            14  Delta Theta
  //

  //Default masses (just an elastic 8He on 12C)
  S1 = 8;  // 8He mass
  S2 = 12; // C12 mass
  S3 = 8;  // 8He mass
  S4 = 12; // C12 mass
 
  EI = 100; // 15MeV*number of nucleons
  EN = 0;
  ENI = 0;

  QM = 0.0001;  //does 0 work?

  TH = 12.3456789;  // in degrees, as the program converts to rad

  ANGAV = new G4double[8];
  ANGAR = new G4double[8];

  for(G4int i = 0;i<8;i++){
    ANGAV[i] = 0.;
    ANGAR[i] = 0.;
  }

  // For debugging
  //  Dump(); 

}


ActarSimCinePrimGenerator::~ActarSimCinePrimGenerator() {
  //
  // Destructor
  //
  delete ANGAV;
  delete ANGAR;
}




void ActarSimCinePrimGenerator::RelativisticKinematics() {
  //
  // Reproduces the relativistic kinematics calculations from
  // CIRE2 subroutine in the original CINE.for code.
  //

  //Arguments:
  // masses: S1, S2, S3, S4 (for incident, target, scattered and recoil masses)
  // reaction q: QM
  // energies: EI, EN, ENI (for lab, excitation of target (positive) and 
  //                        excitation of the projectile (positive))
  // angles: TH             (theta lab angle)
  //
  //Output:
  // given in two vectors called ANGAV and ANGAR (1st and 2nd solutions)
  // the elements are:
  // 0:  Theta CM
  // 1:  ELAB
  // 2:  De/D(Theta)
  // 3:  Jacobian
  // 4:  Theta Lab
  // 5:  Elab S4 
  // 6:  Recoil Jacobian 
  // 7:  
  // Using ANGAV[1] and ANGAR[1] it is possible 
  // to check the number of solutions
  //     if(ANGAV[1] < 0.)   No solution
  //     if(ANGAV[1] >= 0.)  Test ANGAR[1]
  //     if(ANGAR[1] < 0.)   1 Solution (ANGAV)
  //     if(ANGAR[1] >= 0.)  2 Solutions (ANGAV and ANGAR)
 
  //Check
  //Dump();

  //Initial constants
  const G4double U = 931.49401; // dypang 080227
  const G4double TK = 0.0174532925;

  G4double EMI = -ENI/U;
  G4double EM = EN/U;
  G4double Q = QM/U;
  G4double Z1 = S1+EMI;
  G4double Z4 = S4+EM;
  G4double W = EI/U;
  G4double ET = Z1+S2+W;
  G4double P12 = W*(Z1+Z1+W);
  G4double BETA2 = P12/(ET*ET);
  G4double BETA = sqrt(BETA2);
  G4double GA2 = 1./(1.- BETA2);
  G4double GA = sqrt(GA2);
  G4double PSA = (Z1+S2+S3+Z4+W)*(Q+W+EMI-EM)-P12;
  G4double PSB = (Z1+S2-S3+Z4+W)*(Z1+S2+S3-Z4+W)-P12;
  G4double SM2 = ET*ET-P12;
  G4double PS2 = PSA*PSB/(SM2*4.);

  
  /*//IF CHECKS ARE NEEDED...
    G4cout << G4endl << EMI << " " << EM << " " << Q << G4endl;
    G4cout << Z1 << " " << Z4 << " " << W << G4endl;
    G4cout << ET << " " << P12 << " " << BETA2 << G4endl;
    G4cout << BETA << " " << GA2 << " " << GA << G4endl;
    G4cout << PSA << " " << PSB << " " << SM2 << G4endl;
    G4cout << PS2 << G4endl;
  */
  

  //translation note
  //IF(PS2) 6,7,7 
  //are the goto lines for negative, zero and positive results of the argument
  
  if(PS2 < 0.) {
    G4cout << " !! ERROR. NO SOLUTION IN CINE::RelativisticKinematics()   PS2 < 0. Returning " << G4endl;
    ANGAV[1] = -1.;
    return;
  }
  
  G4double PS = sqrt(PS2);
  G4double CT = cos(TH*TK);
  G4double ST = sin(TH*TK);
  G4double E3S = sqrt(S3*S3+PS2);
  G4double A = CT*CT+GA2*ST*ST;
  G4double B = E3S*CT*GA*BETA;
  G4double DELTA = GA2*(PS2-GA2*BETA2*ST*ST*S3*S3);
  G4double WA = -1.;
  ANGAV[1] = -1.;

  
  /*//IF CHECKS ARE NEEDED... 
    G4cout << G4endl << PS << " " << CT << " " << ST << G4endl;
    G4cout << E3S << " " << A << " " << B << G4endl;
    G4cout << DELTA << " " << WA << " " << ANGAV[1] << G4endl;
  */
  
  
  //translation note
  //IF(DELTA) 30,8,8
  if(DELTA < 0.) {
    G4cout << " !! ERROR. NO SOLUTION IN CINE::RelativisticKinematics()  DELTA < 0. Returning " << G4endl;
    return;
  }

  G4double P3=(B+sqrt(DELTA))/A;
  
  //
  //G4cout << P3 << " era P3 " << G4endl;
  
  //dummy boolean for a second iteration of the code
  //searching for a second solution
  G4bool secondIter = 0;
  
  do{//translation solution for the last GOTO 10 at the end of FORTRAN code
    secondIter=0;   //control the do... while loop

    //tranlation note: point 10   
    G4double P32 = P3*P3;
    
    G4double ET3 = sqrt(S3*S3+P32);
    G4double W3 = P32/(S3+ET3);
    G4double CSTE = 1E-8;

    //HAPOL
    //G4cout << G4endl << P32 << " " << ET3<< " " << W3 << " " << CSTE << G4endl;
    
    if(W3-CSTE < 0.) {
      G4cout << " !! ERROR. NO SOLUTION IN CINE::RelativisticKinematics()  W3-CSTE < 0. Returning " << G4endl;
      ANGAR[1] = -1.;
      return; 
    }
    
    G4double CCHI,SCHI;
    G4bool pass41 = 0; //logic aux variable to handle 
    //the nested loops in FORTRAN
    G4double CX;
    
    if(PS <= 0.) {
      CCHI = 1.;
      SCHI = 0.;
      //moves to 41, which is at the end of the next else  
    }
    else{
      CCHI = (P3*CT-GA*BETA*E3S)/(GA*PS);
      SCHI = P3*ST/PS;
      G4double ACX = fabs(CCHI);
      if (ACX-1.> 0.) {              //     IF(ACX-1.) 42,42,12
	if(TH-CSTE < 0.) ;           //12   IF(TH-CSTE) 41,13,13
	else if(180.-TH-CSTE < 0.) ; //13   IF(TH-CSTE) 41,13,13
	else {
	  ANGAR[1]=-1.;
	  return;
	}
      }
      else{
	if(CCHI == 0.) {  // 42  IF(CCHI) 41,40,41
	  CX = 90.000;        //tranlation note:here comes a GOTO 14 
	  pass41 = 1;     //to handle the GOTO 14 line properly
	}       
      }	
    }

    //translation note: point   41 

    if(!pass41){  //valid for all cases except when original 
                  //FORTRAN code passes through GOTO 14
      G4double TGCX = SCHI/CCHI;
      CX = (atan(TGCX))/TK;
      if(CX < 0.) CX = 180.+CX;
    }
    
    pass41=0; //back again to 0

    G4double XJ,AJ;
    
    //tranlation note: point  14 
    if(P3 > 0.){	    
      XJ = GA*PS2*(PS+BETA*CCHI*E3S)/(P32*P3);
      AJ = fabs(XJ);
      if(AJ-1000. >= 0.) XJ = 999.9999; 
    }
    else {
      XJ = 999.9999;
    }
    
    //translation note: point  27 

    G4double ANUM = GA*BETA*P32*ST*(E3S+P3*CT*GA*BETA);
    G4double DENO = (S3+W3)*(GA*BETA*E3S*CT-P3);
    
    G4double DX;  
    
    if(DENO == 0.)  DX = 9999.9;
    else DX = 1000.*U*TK*(ANUM/DENO);
    
    //translation note: point  17 

    G4double E4S = sqrt(Z4*Z4+PS2);
    
    G4double DENOM = GA*(BETA*E4S-PS*CCHI);
    G4double TR;
    G4double TGTR;

    if(DENOM == 0.) TR = 90.;
    else {
      TGTR = PS*SCHI/DENOM;
      TR = (atan(TGTR))/TK;
      if(TR < 0.) TR = 180.+TR;
    }
    
    //tranlation note: point  38 

    G4double P42 = DENOM*DENOM+PS2*SCHI*SCHI;
    G4double P4 = sqrt(P42);
    G4double ER = U*P42/(Z4+sqrt(Z4*Z4+P42));
    
    G4double RJ;
    
    if(P4 > 0.) RJ = 999.9999;
    else {
      RJ = GA*PS2*(PS-BETA*CCHI*E4S)/(P42*P4);
      AJ = fabs(RJ);
      if(AJ-1000. >= 0.) RJ = 999.9999;
    }
    
    //tranlation note: point  33 

    if(WA < 0.) {
      ANGAV[0] = CX;
      WA = W3*U;
      ANGAV[1] = WA;
      ANGAV[2] = DX;
      ANGAV[3] = fabs(XJ);
      ANGAV[4] = TR;
      ANGAV[5] = ER;
      ANGAV[6] = fabs(RJ);
      P3=(B-sqrt(DELTA))/A;      
      //printResults(0);
      if(P3 < 0.) { 
	ANGAR[1] = -1.;
	return;
      }
      else{
	secondIter = 1;
      }    
    }
    else{
      //tranlation note: point  19  
      ANGAR[0] = CX;
      ANGAR[1] = W3*U;
      ANGAR[2] = DX;
      ANGAR[3] = fabs(XJ);
      ANGAR[4] = TR;
      ANGAR[5] = ER;
      ANGAR[6] = fabs(RJ);
      //printResults(1);
      return;
    }
  } while(secondIter == 1);
  
  return;  
}


void ActarSimCinePrimGenerator::Dump() {
  //
  // Dump
  //

  G4cout << G4endl << "ActarSimCinePrimGenerator::Dump()" << G4endl;
  G4cout << "Incident Mass: " << S1 << "  " 
	 << "Target Mass: " << S2  << G4endl;
  G4cout << "Scattered Mass: " << S3 << "  "
	 << "Recoil Mass: " << S4  << G4endl;
  G4cout << "Reaction Q: " << QM << "  "
	 << "Theta Lab Angle: " << TH << "  "
	 << "LAB energy :" << EI << G4endl;
  G4cout << "Target excitation energy:" << EN << "  "
	 << "Projectile excitation energy:" << ENI << G4endl;
  G4cout << G4endl<< "ANGAV:" << ANGAV << "  ";
  for(G4int i=0;i<8;i++)  G4cout << ANGAV[i] << ", ";
  G4cout << G4endl <<"ANGAR:" << ANGAR << "  ";
  for(G4int i=0;i<8;i++)  G4cout << ANGAR[i] << ", ";
    
}

void ActarSimCinePrimGenerator::printResults(G4int sel) {
  //
  // print the results for each solution
  //

  G4int prec = G4cout.precision(6);

  G4cout << G4endl << " ActarSimCinePrimGenerator::printResult(" << sel << ")" << G4endl;
  if(sel == 0){
    G4cout << " CINE result:    Scattered Particle Angle: " << GetThetaLabAngle() 
	   << " deg,    Scattered Particle Energy: " << ANGAV[1]  << " MeV" << G4endl;
    G4cout << " CINE result:    Recoil Particle Angle: " << ANGAV[4]
	   << " deg,    Recoil Particle Energy: " << ANGAV[5]  << " MeV" << G4endl;
  }
  if(sel == 1){
    G4cout << " Second solution in CINE ... " << G4endl; 
    G4cout << " CINE result:    Scattered Particle Angle: " << GetThetaLabAngle() 
	   << " deg,    Scattered Particle Energy: " << ANGAR[1]  << " MeV" << G4endl;
    G4cout << " CINE result:    Recoil Particle Angle: " << ANGAR[4]
	   << " deg,    Recoil Particle Energy: " << ANGAR[5]  << " MeV" << G4endl;
  } 
  G4cout.precision(prec);

}
