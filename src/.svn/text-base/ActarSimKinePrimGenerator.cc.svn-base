/////////////////////////////////////////////////////////////////
//*-- AUTHOR:  (Original FORTRAN code) M.S. Golovkov
//*-- AUTHOR (translation to C++): Pang Danyang (pang@ganil.fr)
//*-- Date: 02/2008
// --------------------------------------------------------------
// Description:
//   program to calculate relativistic kinematics of binary reaction
// --------------------------------------------------------------
// Comments:
//   - 27/02/08 translation from the original FORTRAN
//      to Geant4-like C++
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include "ActarSimKinePrimGenerator.hh"

#include "globals.hh"
#include "Randomize.hh"

using namespace std;

ActarSimKinePrimGenerator::ActarSimKinePrimGenerator() {
  //
  // Constructor
  //

  //
  //  Original program needs the following data:
  //  wm(4) -> masses in MeV (here we use Atomic mass unit u)
  //     1  -> beam
  //     2  -> target
  //     3  -> scattered particle
  //     4  -> recoil
  //  tb    -> incident energy in MeV (lab)
  //  tetacmp -> angle of the scattered particle in cms (rad)
  //
  //  output:
  //  ANGAs[0] -> angle  of scattered particle (3) in lab (rad)
  //  ANGAs[1] -> energy of scattered particle (3) in lab (MeV)
  //  ANGAr[0] -> angle  of recoiled particle (4) in lab (rad)
  //  ANGAr[1] -> energy of recoiled particle (4) in lab (MeV)

  //Default masses (just an elastic 8He on 12C)
  m1 = 8.;  // 8He mass
  m2 = 12.; // C12 mass
  m3 = 8.;  // 8He mass
  m4 = 12.; // C12 mass

  ex1=0.0; // excitation energy of projectile
  ex2=0.0; // excitation energy of target
  ex3=0.0; // excitation energy of scattered particle
  ex4=0.0; // excitation energy of recoil

  tb = 100; // incident energy

  thetacmsInput = 12.3456789;  // in degrees, as the program converts to rad

  ANGAs = new G4double[2];
  ANGAr = new G4double[2];

  for(G4int i = 0;i<2;i++){
    ANGAs[i] = 0.;
    ANGAr[i] = 0.;
  }

  NoSolution=FALSE;
}

ActarSimKinePrimGenerator::~ActarSimKinePrimGenerator() {
  //
  // Destructor
  //
  delete ANGAs;
  delete ANGAr;
}

void ActarSimKinePrimGenerator::KineKinematics() {
  //
  // Reproduces the relativistic kinematics calculations from
  // kin_cmlabf.f of M.S. Golovkov
  //

  //Arguments:
  // masses: m1, m2, m3, m4 (for incident, target, scattered and recoil masses)
  // exciatation energies: ex1, ex2, ex3, ex4
  // energies: Tb (lab energy in MeV)
  // angles: thetacmsInput   (theta CM angle of the scattered particle in degree)
  //
  //Output:
  // given in two vectors called ANGAs and ANGAr (scattered and recoiled particle)
  // the elements are:
  // 0:  Theta lab (in rad)
  // 1:  ELAB (in MeV)

  //Initial constants
  const G4double U = 931.49401; // dypang 080227
  const G4double rad2deg = 0.0174532925;
  const G4double PI=3.14159265358979323846;

  G4double wm1=m1*U+ex1;
  G4double wm2=m2*U+ex2;
  G4double wm3=m3*U+ex3;
  G4double wm4=m4*U+ex4;

  G4double eb=tb+wm1;
  G4double pb2=tb*tb+2.0*tb*wm1;
  G4double pb=sqrt(pb2);
  G4double beta=pb/(eb+wm2);
  G4double gamma=1.0/sqrt(1.0-beta*beta);

  G4double thetacms=thetacmsInput*rad2deg;  // degree to radian

  G4double thetacmr=PI-thetacms;
  G4double e=tb+wm1+wm2;
  G4double e_cm2 = e*e-pb2;
  G4double e_cm  = sqrt(e_cm2);
  G4double t_cm  = e_cm-wm3-wm4;

  if(t_cm<0.0){
    G4cout << "Kine No solution!";
    NoSolution=TRUE;
    return;
  }

  G4double t_cm2=t_cm*t_cm;
  G4double t3_cm=(t_cm2+2.*wm4*t_cm)/(t_cm+wm3+wm4)/2.0;
  G4double t4_cm=(t_cm2+2.*wm3*t_cm)/(t_cm+wm3+wm4)/2.0;
  G4double p3_cm2=t3_cm*t3_cm+2.0*t3_cm*wm3;
  G4double p3_cm =sqrt(p3_cm2);
  G4double tg_thetalabs=p3_cm*sin(thetacms)/(gamma*(p3_cm*cos(thetacms)+beta*sqrt(p3_cm*p3_cm+wm3*wm3)));

  if(tg_thetalabs>=1.0e6){
    ANGAs[0]=PI/2;
  }
  else{
    ANGAs[0]=atan(tg_thetalabs);
  }

  if(ANGAs[0]<0.0) ANGAs[0]=PI+ANGAs[0];

  G4double p4_cm2=t4_cm*t4_cm+2.*t4_cm*wm4;
  G4double p4_cm =sqrt(p4_cm2);
  G4double tg_thetalabr=p4_cm*sin(thetacmr)/(gamma*(p4_cm*cos(thetacmr)+beta*sqrt(p4_cm*p4_cm+wm4*wm4)));

  if(tg_thetalabr>1.0e6){
    ANGAr[0]=PI/2.0;
  }
  else{
    ANGAr[0]=atan(tg_thetalabr);
  }

  if(ANGAr[0]<0.0) ANGAr[0]=PI+ANGAr[0];

// Lorentz transformations to lab -----

  G4double p3_cmx = p3_cm*sin(thetacms);
  G4double p3_cmz = p3_cm*cos(thetacms);
  G4double p3_labx = p3_cmx;
  G4double p3_labz = gamma*(p3_cmz+beta*(t3_cm+wm3));
  G4double p3_lab = sqrt(p3_labx*p3_labx+p3_labz*p3_labz);
  ANGAs[1]=sqrt(p3_lab*p3_lab+wm3*wm3)-wm3;

  G4double p4_cmx = p4_cm*sin(thetacmr);
  G4double p4_cmz = p4_cm*cos(thetacmr);
  G4double p4_labx = p4_cmx;
  G4double p4_labz = gamma*(p4_cmz+beta*(t4_cm+wm4));
  G4double p4_lab = sqrt(p4_labx*p4_labx+p4_labz*p4_labz);
  ANGAr[1] = sqrt(p4_lab*p4_lab+wm4*wm4)-wm4;

//  PrintResults();

  return;
}

void ActarSimKinePrimGenerator::Dump() {
  //
  // Dump
  //

  G4double rad2deg= 0.0174532925;

  G4cout << G4endl << " ActarSimKinePrimGenerator::printResult()" << G4endl;

  G4cout << "Incident Mass: "  << m1  << "  "
	     << "Target Mass: "    << m2  << G4endl;
  G4cout << "Scattered Mass: " << m3 << "  "
	     << "Recoil Mass: "    << m4  << G4endl;
  G4cout << "Theta CM Angle: " << thetacmsInput << "  "
	     << "LAB energy :" << tb << G4endl;
  G4cout << "Scattered excitation energy: " << ex3 << G4endl;

  G4cout << "Lab Scattering angle:" << ANGAs[0]/rad2deg << ", Scattering energy=" << ANGAs[1] << endl;
  G4cout << "      Lab Recoil angle:" << ANGAr[0]/rad2deg << ", Recoil energy=" << ANGAr[1] << endl;

}

void ActarSimKinePrimGenerator::PrintResults() {
  //
  // print the results for each solution
  //

	G4double rad2deg= 0.0174532925;

  G4int prec = G4cout.precision(6);

  G4cout << G4endl << " ActarSimKinePrimGenerator::printResult()" << G4endl;

  G4cout << "Incident Mass: "  << m1  << "  "
	     << "Target Mass: "    << m2  << G4endl;
  G4cout << "Scattered Mass: " << m3 << "  "
	     << "Recoil Mass: "    << m4  << G4endl;
  G4cout << "Theta CM Angle: " << thetacmsInput << "  "
	     << "LAB energy :" << tb << G4endl;
  G4cout << "Scattered excitation energy: " << ex3 << G4endl;

  G4cout << "Lab Scattering angle:" << ANGAs[0]/rad2deg << ", Scattering energy=" << ANGAs[1] << endl;
  G4cout << "      Lab Recoil angle:" << ANGAr[0]/rad2deg << ", Recoil energy=" << ANGAr[1] << endl;

  G4cout.precision(prec);
}
