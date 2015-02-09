////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol
//*-- Date: 11/2004
//*-- Last Update: 16/12/14 by Hector Alvarez Pol
// --------------------------------------------------------------
// Description:
//   Actions to perform to generate a primary vertex
//
// --------------------------------------------------------------
// Comments:
//   - 27/01/05 Cleaning and improving calculations
//   - 25/11/04 Created based on example/novice/N01 structure
//   - 16/12/14 Cleaning and ordering for the new ActarSim
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////

#include "ActarSimPrimaryGeneratorAction.hh"

#include "ActarSimDetectorConstruction.hh"
#include "ActarSimGasDetectorConstruction.hh"
#include "ActarSimPrimaryGeneratorMessenger.hh"

#include "ActarSimROOTAnalysis.hh"
#include "ActarSimCinePrimGenerator.hh"
#include "ActarSimKinePrimGenerator.hh"
#include "ActarSimEulerTransformation.hh"

#include "ActarSimBeamInfo.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ThreeVector.hh"
#include "G4Gamma.hh"
#include "G4Geantino.hh"
#include "globals.hh"
#include "Randomize.hh"

#include "G4ParticleDefinition.hh"

# include <cstdlib>
# include <iostream>
# include <fstream>
using namespace std;

ActarSimPrimaryGeneratorAction::ActarSimPrimaryGeneratorAction()
  :gasDetector(0), incidentIon(0),targetIon(0),scatteredIon(0),recoilIon(0),
   beamInteractionFlag("off"),
   realisticBeamFlag("off"), reactionFromEvGenFlag("off"), reactionFromCrossSectionFlag("off"),
   reactionFromFileFlag("off"),reactionFromCineFlag("off"),
   randomThetaFlag("off"),reactionFile("He8onC12_100MeV_Elastic.dat"),reactionFromKineFlag("off"),
   vertexPosition(0) {
  //
  // Constructor: init values are filled
  //

  //Initial Values
  G4ThreeVector zero;
  particleTable = G4ParticleTable::GetParticleTable();

  //create a particleGun
  particleGun = new G4ParticleGun(1);

  //create a messenger for this class
  gunMessenger = new ActarSimPrimaryGeneratorMessenger(this);

  G4ParticleDefinition* pd = particleTable->FindParticle("proton");
  if(pd != 0)
    particleGun->SetParticleDefinition(pd);
  particleGun->SetParticlePosition(zero);
  particleGun->SetParticleTime(0.0);
  particleGun->SetParticlePolarization(zero);
  particleGun->SetParticleCharge(1.0);
  particleGun->SetParticleMomentumDirection(G4ThreeVector(0.0,0.0,1.0));
  particleGun->SetParticleEnergy(1*MeV);

  //create a pointer that gives access to the tabulated xs
  pReadEvGen = new ActarSimEventGenerator();

  reactionQ = 0.0001;   //does 0 work? (QM)

  labEnergy = 100;      // 15MeV*numero de nucleones (EI)

  incidentEnergy=labEnergy;
  // (EN) and (ENI) are taken from the target and projectile ion definitions

  thetaLabAngle = 45 * deg;   // 45 degrees (TH)

  randomThetaMin = 0.*deg;
  randomThetaMax = 180.*deg;
}

ActarSimPrimaryGeneratorAction::~ActarSimPrimaryGeneratorAction() {
  //
  // Simple destructor
  //

  delete particleGun;
  delete gunMessenger;
}

void ActarSimPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
  //
  // Function called at the begining of event
  // Generate most of the primary physics. See the comments on each possible case
  //
  //REMOVE IF NOT NEEDED!!!!!!
  const G4int verboseLevel = G4RunManager::GetRunManager()->GetVerboseLevel();
  //G4cout << G4endl << " ______ VerboseLevel  _______" <<verboseLevel<< G4endl;

  G4ThreeVector zero;
//G4double energyLostInTargetGas = 0; //zero, to be calculated if realisticBeamFlag is on

  //Initial values for reactionFromEvGen
  G4double  LabParticleAngle = 85.0 * deg;
  G4double  LabParticleAngle_rec = 85.0 * deg;
  if(LabParticleAngle){;}
  if(LabParticleAngle_rec){;}

  ActarSimDetectorConstruction* detector = (ActarSimDetectorConstruction*)
    (G4RunManager::GetRunManager()->GetUserDetectorConstruction());

  //reading this parameter from the geometry
  if(!gasDetector) {
    // ActarSimDetectorConstruction* detector = (ActarSimDetectorConstruction*)
    //   (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    gasDetector = (ActarSimGasDetectorConstruction*)(detector->GetGasDetector());
    if(gasDetector->GetDetectorGeometry()=="tube")
      lengthParameter = gasDetector->GetLengthGasTub();
    else
      lengthParameter = gasDetector->GetZGasBox();

    G4cout << "##################################################################" << G4endl
	   << "##### ActarSimPrimaryGeneratorAction::GeneratePrimaries()  #######" << G4endl
	   << "##### Information: Length of gas volume = " << lengthParameter << "    ######"
           << G4endl;
    G4cout << "##################################################################" << G4endl;
  }

  // CASE A1: Beam interaction in the gas
  if(beamInteractionFlag == "on"){
    // The beam is described by the (G4Ions*) incidentIon (or incident particles)
    // and it is tracked in the gas in the even events (beggining at zero, 0,2,4, ...)
    // while in the odd events (1,3,5, ...) the reaction products are tracked, 
    // (CINE, KINE, from file, one output particle, ...) using some parameters
    // (vertex position, remaining beam ion energy, ...) obtained in the beam tracking.
    ActarSimBeamInfo *pBeamInfo = (ActarSimBeamInfo*) 0;

    if(gActarSimROOTAnalysis){
      pBeamInfo = gActarSimROOTAnalysis->GetBeamInfo();
      gActarSimROOTAnalysis->SetBeamInteractionFlag("on");
    }

    if(pBeamInfo->GetStatus() > 1){ 
      // Beam reached vertex: Tracking vertex products!!
      // Resetting the BeamInfo status to 0, for the next beam tracking
      if(verboseLevel>0){ 
        G4cout << G4endl
              << " *************************************************** " << G4endl
              << " * ActarSimPrimaryGeneratorAction::GeneratePrimaries() " << G4endl
              << " * beamInteractionFlag=on, beam.Status>1 ... " << G4endl
              << " * Moving to status 0 and continuing with reaction!" << G4endl;
        G4cout << " *************************************************** "<< G4endl;
      }
      vertexPosition.setX(pBeamInfo->GetXVertex());
      vertexPosition.setY(pBeamInfo->GetYVertex());
      vertexPosition.setZ(pBeamInfo->GetZVertex());
      SetLabEnergy(pBeamInfo->GetEnergyVertex());
      pBeamInfo->SetStatus(0);
    }
    else if(pBeamInfo->GetStatus() == 1){
      // the beam finished the tracking in the previous event without reaching
      // the requested vertex position! This is a faulty case! Aborting present event and continue.
      G4cout << G4endl
             << " *************************************************** " << G4endl
             << " * ActarSimPrimaryGeneratorAction::GeneratePrimaries() " << G4endl
             << " * ERROR STATUS! beamInteractionFlag=on, beam.Status=1 ... " << G4endl
             << " * Moving to status 0 and ABORTING! " << G4endl;
      G4cout << " *************************************************** "<< G4endl;
      pBeamInfo->SetStatus(0);
      anEvent->SetEventAborted();
    }
    else{ //Initial values for CINE or KINE or incident ion definition.
      if(verboseLevel>0){
        G4cout << G4endl
              << " *************************************************** " << G4endl
              << " * ActarSimPrimaryGeneratorAction::GeneratePrimaries() " << G4endl
              << " * beamInteractionFlag=on, beam.Status=0 ... " << G4endl
              << " * Moving to status 1 and tracking the beam!" << G4endl;
        G4cout << " *************************************************** "<< G4endl;
      }
      if(!incidentIon) {	
        incidentIon = (G4Ions*) particleTable->GetIon(2, 8, 0.);  // 8He (S1)
        incidentIonCharge =  2;
        G4cout << G4endl
               << " *************************************************** " << G4endl
               << " * ActarSimPrimaryGeneratorAction::GeneratePrimaries() " << G4endl
               << " * No incident ion selected: using 8He as incident beam" << G4endl;
        G4cout << " *************************************************** "<< G4endl;
      }
      pBeamInfo->SetStatus(1);

      // TODO: Kinematics calculated assuming beam following perfect Z direction. 
      // It the beam has another 4-momentum, the reaction plane and kinematics
      // should be transformed accordingly. Fine tuning, anyway (check if needed)
      particleGun->SetParticleDefinition(incidentIon);
      particleGun->SetParticleCharge(incidentIonCharge);

      // TODO: The probability of interaction is simply constant over the path on the gas.
      // This is the right place for introducing some dependence with the ion energy using the reaction cross section

      // z0 decides the z position of the vertex. The beam is tracked till z0 is reached ...
      G4double z0 = 0;

      if(randomVertexZPositionFlag=="off"){
        z0 = vertexZPosition;
      }
      else if(randomVertexZPositionFlag=="on"){
        z0 = randomVertexZPositionMin + G4UniformRand()*(randomVertexZPositionMax-randomVertexZPositionMin);
      }

      pBeamInfo->SetZVertex(z0);
      pBeamInfo->SetEnergyEntrance(GetIncidentEnergy()); // dypang, 080729

      if(realisticBeamFlag == "on") {
        // Emittance is defined in mm mrad
        // The beamRadiusAtEntrance defines the radius at entrance
        // The polar angle at Entrance is defined by the relation between emitance and radiusAtEntrance.
        // this relation is roughtly emittance =  widthPos * widthAng = 2 * radiusMax * 2 * thetaMax
        // A step function is used for the position and angular distributions
        G4double radiusAtEntrance   = -beamRadiusAtEntrance + (2 * beamRadiusAtEntrance * G4UniformRand());
        G4double thetaWidth =  emittance/(4*beamRadiusAtEntrance); //polar angle width between zero and maximum
        G4double thetaAtEntrance = thetaWidth * G4UniformRand() * mrad;
        G4double phiAtEntrance = G4UniformRand() * twopi; //angle phi of momentum
        G4double phi2AtEntrance = G4UniformRand() * twopi; //angle for defining the entrance point

        G4ThreeVector directionAtEntrance = G4ThreeVector(sin(thetaAtEntrance)*cos(phiAtEntrance),
							  sin(thetaAtEntrance)*sin(phiAtEntrance),
							  cos(thetaAtEntrance));

        //Entrance coordinates (x0,y0,0) with angles (thetaAtEntrance, phiAtEntrance)
        G4double x0 = radiusAtEntrance*cos(phi2AtEntrance);
        G4double y0 = radiusAtEntrance*sin(phi2AtEntrance);
        //G4double z0 = 0.0;
        //G4double z0= -(detector->GetChamberZLength()-detector->GetZGasBoxPosition());
        G4double z0= -29*mm;

        if(verboseLevel>0){
          G4cout << G4endl
                << " *************************************************** " << G4endl
                << " * ActarSimPrimaryGeneratorAction::GeneratePrimaries() " << G4endl
                << " * beamInteractionFlag=on, beam.Status=0, realisticBeamFlag=on " << G4endl
                << " * Beam with entrance emittance:"  << G4endl
                << " * ThetaWidth: " << thetaWidth << G4endl
                << " * RadiusAtEntrance:  " << radiusAtEntrance << G4endl;
          G4cout << " *************************************************** "<< G4endl;
        }

        particleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));
        particleGun->SetParticleMomentumDirection(directionAtEntrance);
        pBeamInfo->SetPositionEntrance(x0,y0,z0);
        pBeamInfo->SetAnglesEntrance(thetaAtEntrance,phiAtEntrance);
      }
      else{ // simplest case: beam at (0,0,0) with direction along Z
        particleGun->SetParticlePosition(G4ThreeVector(0.,0.,0.));
        particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
        pBeamInfo->SetPositionEntrance(0.,0.,0.);
        pBeamInfo->SetAnglesEntrance(0.,0.);
      }

      particleGun->SetParticleTime(0.0);
      particleGun->SetParticlePolarization(zero);

      particleGun->SetParticleEnergy(GetIncidentEnergy()); // dypang, 080729

      particleGun->GeneratePrimaryVertex(anEvent);

      //Histogramming
      if(gActarSimROOTAnalysis)
        gActarSimROOTAnalysis->GenerateBeam(anEvent);
      return;
    }
  }//end of  if(beamInteractionFlag == "on")

  // CASE A2:   Beam not included...
  else if(realisticBeamFlag == "on") {
    //Not anymore a gaussian, but a flat distribution in a given radius around Z axis.
    G4double radiusAtEntrance   = -beamRadiusAtEntrance + (2 * beamRadiusAtEntrance * G4UniformRand());
    G4double phi2AtEntrance = G4UniformRand() * twopi; //angle for defining the entrance point

    //Vertexcoordinates (x0,y0,0) with angles (thetaAtEntrance, phiAtEntrance)
    G4double x0 = radiusAtEntrance*cos(phi2AtEntrance);
    G4double y0 = radiusAtEntrance*sin(phi2AtEntrance);
    // The z0 variable is here used to decide the z position of the vertex. The beam is tracked
    // till z0 is reached ...
    //G4double z0=0.;
    //G4double z0= -(detector->GetChamberZLength()-detector->GetZGasBoxPosition());
    G4double z0= -29*mm;

    if(randomVertexZPositionFlag=="on"){
      z0 = randomVertexZPositionMin+ G4UniformRand()*(randomVertexZPositionMax-randomVertexZPositionMin);
    }
    else{
      z0 = G4UniformRand()*(2.* lengthParameter);
    }

    if(verboseLevel>0){
      G4cout << G4endl
            << " *************************************************** " << G4endl
            << " * ActarSimPrimaryGeneratorAction::GeneratePrimaries() " << G4endl
            << " * beamInteractionFlag=off but realisticBeamFlag=on " << G4endl
            << " * No beam interaction, but vertex generated with some emittance effects:"  << G4endl
            << " * RadiusAtVertex:  " << radiusAtEntrance << G4endl;
      G4cout << " *************************************************** "<< G4endl;
    }

    //setting the vertexPos vector to the previous calculated values
    vertexPosition.setX(x0);
    vertexPosition.setY(y0);
    vertexPosition.setZ(z0);
  }
  // After the beam, now different options for the reaction products!

  // CASE B Reaction from Event-Generator
  if(reactionFromEvGenFlag == "on") {
    if(verboseLevel>0){
      G4cout << G4endl
            << " *************************************************** " << G4endl
            << " * ActarSimPrimaryGeneratorAction::GeneratePrimaries() " << G4endl
            << " * reactionFromEvGenFlag=on                            " << G4endl
            << " * An external event generator is used..."  << G4endl;
      G4cout << " *************************************************** "<< G4endl;
    }
    //Initial values
    G4double particle_energy = 50*MeV;
    G4double particle_energy_rec = 50*MeV;
    G4double momentum_x = 1.;
    G4double momentum_y = 0.;
    G4double momentum_z = 1.5;
    G4double momentum_x_rec = 1.;
    G4double momentum_y_rec = 0.;
    G4double momentum_z_rec = 1.5;
    G4double phi= 0. ;
    if( particle_energy &&  particle_energy_rec ){;}
    if( momentum_x && momentum_x_rec){;}
    if( momentum_y && momentum_y_rec){;}
    if( momentum_z && momentum_z_rec){;}
    //TRANSFER
    // Polar angle:
    G4double theta=0;
    // Random number between 0 and 1
    G4double randnum = G4UniformRand();
    G4double energy_scatt=0;
    G4double slop1=0.,slop2=0.,offset1=0.,offset2=0.;
    G4double theta_recoil=0;
    G4double energy_recoil=0;
    G4double slop_rec=0.,offset_rec=0.;

    //The last angle could be different from 181,
    //extract the nbins from ActarSimEventGenerator (todo)
    for(G4int i=0;i<181;i++) {
      if(randnum>=pReadEvGen->Icross_section[i] &&
	       randnum<pReadEvGen->Icross_section[i+1]) {
        slop1=(pReadEvGen->LabAngle_scatt[i+1]-
	             pReadEvGen->LabAngle_scatt[i])/(pReadEvGen->Icross_section[i+1]-
					     pReadEvGen->Icross_section[i]);
        offset1= pReadEvGen->LabAngle_scatt[i]-slop1*pReadEvGen->Icross_section[i];
        theta=randnum*slop1+offset1;

        for(G4int j=0; j<181; j++) {
          if(theta>=pReadEvGen->LabAngle_scatt[j] &&
	           theta<pReadEvGen->LabAngle_scatt[j+1]) {
            slop2=(pReadEvGen->LabEnergy_scatt[int(pReadEvGen->LabAngle_scatt[j+1])]-
		              pReadEvGen->LabEnergy_scatt[int(pReadEvGen->LabAngle_scatt[j])])/
	                (pReadEvGen->LabAngle_scatt[j+1]-pReadEvGen->LabAngle_scatt[j]);

            offset2= pReadEvGen->LabEnergy_scatt[int(pReadEvGen->LabAngle_scatt[j])]-
	                   slop2*pReadEvGen->LabAngle_scatt[j];
            energy_scatt=theta*slop2+offset2;
            //Angle of the Recoil
            theta_recoil=pReadEvGen->LabAngle_recoil[j];
            //The same for the recoil
            slop_rec=(pReadEvGen->LabEnergy_recoil[int(pReadEvGen->LabAngle_recoil[j+1])]-
		                 pReadEvGen->LabEnergy_recoil[int(pReadEvGen->LabAngle_recoil[j])])/
	                   (pReadEvGen->LabAngle_recoil[j+1]-pReadEvGen->LabAngle_recoil[j]);
            offset_rec=pReadEvGen->LabEnergy_recoil[int(pReadEvGen->LabAngle_scatt[j])]-
	                     slop_rec*pReadEvGen->LabAngle_recoil[j];
      	    energy_recoil=theta_recoil*slop_rec+offset_rec;
          }
        }
        theta=theta*deg;   // Polar angle in radian
        theta_recoil=theta_recoil*deg;   // Polar angle in radian
      }
    }
    // Azimuthal angle:
    phi = G4UniformRand()*twopi;

    momentum_x = sin(theta)*cos(phi);
    momentum_y = sin(theta)*sin(phi);
    momentum_z = cos(theta);

    momentum_x_rec = sin(theta_recoil)*cos(phi+pi);
    momentum_y_rec = sin(theta_recoil)*sin(phi+pi);
    momentum_z_rec = cos(theta_recoil);

    particle_energy = energy_scatt;
    LabParticleAngle = theta;

    particle_energy_rec = energy_recoil;
    LabParticleAngle_rec = theta_recoil;
    //Send 2 vertex
    //Define ions from the macro (talk to Hector->use the ones from CINE)
  }


  // CASE C Reaction products taken from a file (format given by CINE output)
  if(reactionFromFileFlag == "on"){
    // FILE format: first row should contain 6 integers with the info:
    //    scattered ion Z;  scattered ion A;  scattered ion charge state;
    //    recoiled ion Z;  recoil ion A;  recoil ion charge state;
    // The folowing lines contains:
    // scattered ion angle; scattered ion energy; recoil ion angle; recoil ion energy
    // (typical output from CINE, for example)
    // A random line (that is, a random angle) is taken from the event list
    if(verboseLevel>0){
      G4cout << G4endl
            << " *************************************************** " << G4endl
            << " * ActarSimPrimaryGeneratorAction::GeneratePrimaries() " << G4endl
            << " * reactionFromFileFlag = on                           " << G4endl
            << " * An external file with products information is used."  << G4endl;
      G4cout << " *************************************************** "<< G4endl;
    }
    ifstream inputFile(reactionFile);
    if(reactionFile) {
      G4double cine[91*4+1];
      G4int  ion1Z, ion1A, ion1Charge;
      G4int  ion2Z, ion2A, ion2Charge;

      inputFile >> ion1Z;    inputFile >> ion1A;    inputFile >> ion1Charge;
      inputFile >> ion2Z;    inputFile >> ion2A;    inputFile >> ion2Charge;

      if(verboseLevel>1){
        G4cout << ion1Z << " " << ion1A << " " << ion1Charge << " "
	             << ion2Z << " " << ion2A << " " << ion2Charge << G4endl;
      }
      G4int i=0;
      do{
        inputFile >>  cine[i];
        //G4cout << cine[i] << " ";
        i++;
      }while(!inputFile.eof());
      
      G4int rowsInputFile = (G4int) i/4;
      //    cout << "rows" << rowsInputFile << G4endl;
      //Taken a random cinematic from the previous table
      G4int randomRow = (G4int)(G4UniformRand()*rowsInputFile);
      G4double theta1 = pi*cine[randomRow*4]/180;
      G4double theta2 = pi*cine[randomRow*4+2]/180;
      G4double energy1 = cine[randomRow*4+1];
      G4double energy2 = cine[randomRow*4+3];
      G4double phi = twopi *G4UniformRand();   //flat in phi
      G4ThreeVector direction1 = G4ThreeVector(sin(theta1)*cos(phi),
					                                     sin(theta1)*sin(phi),
					                                     cos(theta1));
      G4ThreeVector direction2 = G4ThreeVector(sin(theta2)*cos(phi+pi),
					                                     sin(theta2)*sin(phi+pi),
					                                     cos(theta2));
      if(ion1Z==1 && ion1A==1 && ion1Charge==1){
        G4ParticleDefinition* pd = particleTable->FindParticle("proton");
        if(pd != 0) particleGun->SetParticleDefinition(pd);
      }
      else{
        G4Ions* ion1;
        ion1 = (G4Ions*) particleTable->GetIon(ion1Z, ion1A, 0.0);
        particleGun->SetParticleDefinition(ion1);
        particleGun->SetParticleCharge(ion1Charge);
      }
      particleGun->SetParticlePosition(vertexPosition);

      //time, including the beam tracking before the vertex formation
      if(beamInteractionFlag == "on") {
        ActarSimBeamInfo *pBeamInfo = (ActarSimBeamInfo*) 0;
        if(gActarSimROOTAnalysis) {
          pBeamInfo = gActarSimROOTAnalysis->GetBeamInfo();
          particleGun->SetParticleTime(pBeamInfo->GetTimeVertex());
         }
      }
      else
        particleGun->SetParticleTime(0.0);

      particleGun->SetParticlePolarization(zero);
      particleGun->SetParticleMomentumDirection(direction1);
      particleGun->SetParticleEnergy(energy1*MeV);

      particleGun->GeneratePrimaryVertex(anEvent);

      if(ion2Z==1 && ion2A==1 && ion2Charge==1){
        G4ParticleDefinition* pd = particleTable->FindParticle("proton");
        if(pd != 0) particleGun->SetParticleDefinition(pd);
      }
      else{
        G4Ions* ion2;
        ion2 = (G4Ions*) particleTable->GetIon(ion2Z, ion2Z, 0.0);
        particleGun->SetParticleDefinition(ion2);
        particleGun->SetParticleCharge(ion2Charge);
      }
      particleGun->SetParticlePosition(vertexPosition);

      //time, including the beam tracking before the vertex formation
      if(beamInteractionFlag == "on") {
        ActarSimBeamInfo *pBeamInfo = (ActarSimBeamInfo*) 0;
        if(gActarSimROOTAnalysis) {
          pBeamInfo = gActarSimROOTAnalysis->GetBeamInfo();
          particleGun->SetParticleTime(pBeamInfo->GetTimeVertex());
         }
      }
      else
        particleGun->SetParticleTime(0.0);

      particleGun->SetParticlePolarization(zero);
      particleGun->SetParticleMomentumDirection(direction2);
      particleGun->SetParticleEnergy(energy2*MeV);

      particleGun->GeneratePrimaryVertex(anEvent);
    }
    else G4cout << "File " << reactionFile << " not found." << G4endl;
  }

  //CASE D  Reaction products kinematics calculated using Cine
  else if(reactionFromCineFlag == "on"){
    // The CINE program (W. Mittig) has been translated to C++ ...
    if(verboseLevel>0){
      G4cout << G4endl
            << " *************************************************** " << G4endl
            << " * ActarSimPrimaryGeneratorAction::GeneratePrimaries() " << G4endl
            << " * reactionFromCineFlag = on                           " << G4endl
            << " * Using CINE for the kinematics of reaction products."  << G4endl;
      G4cout << " *************************************************** "<< G4endl;
    }

    if (reactionFromCrossSectionFlag =="on"){
      //TRANSFER
      G4double theta=0;
      G4double randnum = 0.;
      G4double slop1=0.,offset1=0.;
      //Generate Random number between 0-1
      randnum = G4UniformRand();
      //Substitute 181 by the number of bins from ActarSimEventGenerator
      //Retriving the value in the table that correspond to the random number
      for(G4int i=0;i<181;i++) {
        if(randnum>=pReadEvGen->Icross_section[i]
	         && randnum<pReadEvGen->Icross_section[i+1]) {
           //Interpolation
          slop1=(pReadEvGen->theta_xsec[i+1]-
		            pReadEvGen->theta_xsec[i])/(pReadEvGen->Icross_section[i+1]-
					      pReadEvGen->Icross_section[i]);
          offset1= pReadEvGen->theta_xsec[i]-slop1*pReadEvGen->Icross_section[i];
          theta=randnum*slop1+offset1;
        }
      }
      // Theta Angle for the scattered particle in degrees
      //theta=theta*pi/180.0;
      //G4cout<<"ThetaLabAngle=="<<theta<<endl;
      LabParticleAngle = theta;
      //Calling CINE for the kinematical features of the recoil nuclei
    }

    //Initial values for CINE. It is not allowed to create this initial values in the constructor...
    if(!incidentIon) {
      incidentIon =  (G4Ions*) particleTable->GetIon(2, 8, 0.);  // 8He (S1)
      incidentIonCharge =  2;
    }
    if(!targetIon) {
      targetIon = (G4Ions*) particleTable->GetIon(6, 12, 0.);     // C12 (S2)
      targetIonCharge = 6;
    }
    if(!scatteredIon){
      scatteredIon = (G4Ions*) particleTable->GetIon(2, 8, 0.);  // 8He (S3)
      scatteredIonCharge = 2;
    }
    if(!recoilIon){
      recoilIon = (G4Ions*) particleTable->GetIon(6, 12, 0.);   // C12 (S4)
      recoilIonCharge = 6;
    }

    //CINE object...
    ActarSimCinePrimGenerator* CINE = new ActarSimCinePrimGenerator();

    CINE->SetIncidentMass(GetIncidentIon()->GetAtomicMass());
    CINE->SetTargetMass(GetTargetIon()->GetAtomicMass());
    CINE->SetScatteredMass(GetScatteredIon()->GetAtomicMass());
    CINE->SetRecoilMass(GetRecoilIon()->GetAtomicMass());
    CINE->SetReactionQ(GetReactionQ());
    CINE->SetLabEnergy(GetLabEnergy());
    CINE->SetTargetExcitationEnergy(GetTargetIon()->GetExcitationEnergy()/MeV);
    CINE->SetProjectileExcitationEnergy(GetIncidentIon()->GetExcitationEnergy()/MeV);
    // Random generator for scattered angle triggered by a messenger Cmd
    if(randomThetaFlag == "on") {
      if(verboseLevel>0){
        G4cout << G4endl
              << " *************************************************** " << G4endl
              << " * ActarSimPrimaryGeneratorAction::GeneratePrimaries() " << G4endl
              << " * reactionFromCineFlag = on, randomThetaFlag = on     " << G4endl
              << " * Using CINE with random angle theta "  << G4endl
	            << " * with limits in theta between ["
              << randomThetaMin/rad << "," << randomThetaMax/rad << "] rads ( ["
              << randomThetaMin/deg << "," << randomThetaMax/deg << "] degrees)" << G4endl;
      }

      //flat prob in theta from randomThetaMin to randomThetaMin
      SetThetaLabAngle((randomThetaMin + 
                       ((randomThetaMax-randomThetaMin) * G4UniformRand())) * rad);
      CINE->SetThetaLabAngle(GetThetaLabAngle()/deg);

      if(verboseLevel>0) {
        G4cout << G4endl
               << " * Selected angle: "<< GetThetaLabAngle()/rad << " rad ( "
               << GetThetaLabAngle()/deg  << " deg )"<< G4endl;
        G4cout << " *************************************************** "<< G4endl;
      }
    } //end of if(randomThetaFlag == "on")

    if(verboseLevel>0){
      G4cout << G4endl
             << " *************************************************** " << G4endl
             << " * ActarSimPrimaryGeneratorAction::GeneratePrimaries() " << G4endl
             << " * CINE: Setting masses to the following values: " << G4endl
	           << " * incident ion mass: " << GetIncidentIon()->GetAtomicMass() << G4endl
	           << " * target ion mass: " << GetTargetIon()->GetAtomicMass() << G4endl
	           << " * scattered ion mass: " << GetScatteredIon()->GetAtomicMass() << G4endl
	           << " * recoil ion mass: " << GetRecoilIon()->GetAtomicMass() << G4endl;
      G4cout << " * Setting reaction Q to:" << GetReactionQ() << G4endl;
      G4cout << " * Setting labEnergy to:" << GetLabEnergy() << G4endl;
      G4cout << " * Setting targetExcitationEnergy to:"
	           << GetTargetIon()->GetExcitationEnergy() << G4endl;
      G4cout << " * Setting projectileExcitationEnergy to:"
	           << GetIncidentIon()->GetExcitationEnergy() << G4endl;
      G4cout << " * Setting labAngle to:" << GetThetaLabAngle() << " deg" << G4endl;
      G4cout << " *************************************************** "<< G4endl;
    }

    if(verboseLevel>2){
      //Parameters are introduced in CINE... Just a checker
      G4cout << " CINE: Checking parameters..." << CINE->GetS1()
	           << " " << CINE->GetS2() << " " << CINE->GetS3()
	           << " " << CINE->GetS4() << " " << CINE->GetQM()
	           << " " << CINE->GetEI() << " " << CINE->GetEN()
	           << " " << CINE->GetENI() << " " << CINE->GetThetaLabAngle() << endl;
    }
    if(verboseLevel>1){
      //Parameters are introduced in CINE... Just a checker
      G4cout << " " << G4endl;
      G4cout << " CINE: Checking... parameters are introduced in CINE..." << G4endl;
      //G4cout << " verboseLevel>0..." << G4endl;
      G4cout << " mass of the incident particle = " << CINE->GetS1()<< G4endl;
      G4cout << " mass of the target = " << CINE->GetS2() << G4endl;
      G4cout << " mass of the scattered particle = " << CINE->GetS3()<< G4endl;
      G4cout << " mass of the recoil = " << CINE->GetS4() << G4endl;
      G4cout << " Reaction Q = " << CINE->GetQM()<< G4endl;
      G4cout << " LAB energy (total Lab energy in MeV) = " << CINE->GetEI() << G4endl;
      G4cout << " Target excitation energy (positive) = " << CINE->GetEN()<< G4endl;
      G4cout << " Projectile excitation energy (positive) = " << CINE->GetENI() << G4endl;
      G4cout << " CINE->GetThetaLabAngle = " << CINE->GetThetaLabAngle() << G4endl;
      G4cout << " " << G4endl;
    }

    //Calling the relativistic kinematic calculation
    CINE->RelativisticKinematics();

    if(verboseLevel>0){
      G4cout << " *************************************************** "<< G4endl;
      G4cout << " ***** CINE: Relativistic calculation invoked ****** "<< G4endl;
      G4cout << " *************************************************** "<< G4endl;
    }

    G4int stillNoSol=0;
    if(CINE->GetANGAV(1)<0 && randomThetaFlag == "on") {
      while( CINE->GetANGAV(1)<0 && stillNoSol<50) { //50 tries to get a result!
        G4cout << " *************************************************** "<< G4endl;
        G4cout << " * There is no CINE solution" << G4endl;
        G4cout << " * Trying again with CINE and other scattered angle..." << G4endl;
        stillNoSol++;
        SetThetaLabAngle(pi * G4UniformRand() * rad);
        CINE->SetThetaLabAngle(GetThetaLabAngle()/deg);
        if(verboseLevel>1) G4cout << " *** random Theta = "
				                          << GetThetaLabAngle()/rad << " rad = "
				                          << GetThetaLabAngle()/deg  << " deg "<< G4endl;
	      //Calling again the relativistic kinematic calculation
	      CINE->RelativisticKinematics();
      }
      if(stillNoSol>49)
        G4cout << G4endl 
               << " ####################################################### "<< G4endl
	             << " ERROR in ActarSimPrimaryGeneratorAction::GeneratePrimaries()" << G4endl
	             << " There is no CINE solution for any angle" << G4endl
	             << " A dummy solution is used: DO NOT RELY ON THE RESULTS!" << G4endl
	             << " ####################################################### "<< G4endl;

    } 
    if(CINE->GetANGAV(1)<0 && randomThetaFlag == "off") {
      G4cout << G4endl 
             <<" ####################################################### "<< G4endl
	           << " ERROR in ActarSimPrimaryGeneratorAction::GeneratePrimaries()" << G4endl
	           << " There is no CINE solution for this angle" << G4endl
	           << " A dummy solution is used: DO NOT RELY ON THE RESULTS!" << G4endl
	           <<" ####################################################### "<< G4endl;
      theta2 = (CINE->GetANGAV(4))*deg;
      energy1 = CINE->GetANGAV(1);
      energy2 = CINE->GetANGAV(5);
    }
    else if( CINE->GetANGAR(1)<0 && stillNoSol<50) {
      if(verboseLevel>0){
        G4cout << " One solution:" << G4endl;
	      CINE->printResults(0);
      }
      theta2 = (CINE->GetANGAV(4))*deg;
      energy1 = CINE->GetANGAV(1);
      energy2 = CINE->GetANGAV(5);
    }
    else {
      //In this case one should select only one of the two possible cases!
      if(verboseLevel>0){
        G4cout << " Two solutions" << G4endl;
        G4cout << " First solution:" << G4endl;
        CINE->printResults(0);
        G4cout << " Second solution:" << G4endl;
        CINE->printResults(1);
      }
      if((G4int)(2*G4UniformRand()) == 0){
        //theta2 = pi*CINE->GetANGAV(4)/180;
        theta2 = (CINE->GetANGAV(4))*deg;
        energy1 = CINE->GetANGAV(1);
        energy2 = CINE->GetANGAV(5);
      }
      else{
        //theta2 = pi*CINE->GetANGAV(4)/180;
        theta2 = (CINE->GetANGAV(4))*deg;
        energy1 = CINE->GetANGAR(1);
        energy2 = CINE->GetANGAR(5);
      }
    }
    if(verboseLevel>0){
      G4cout << " *************************************************** "<< G4endl;
      G4cout << " At least one solution found... generating primaries." << G4endl;
      G4cout << " " << G4endl;
    }
    theta1 = CINE->GetThetaLabAngle()*deg;      //Note the units coming from CINE !
    G4double phi = twopi * G4UniformRand();         //flat probability in phi
    G4ThreeVector direction1 = G4ThreeVector(sin(theta1)*cos(phi),
					                                   sin(theta1)*sin(phi),
					                                   cos(theta1));
    G4ThreeVector direction2 = G4ThreeVector(sin(theta2)*cos(phi+pi),
					                                   sin(theta2)*sin(phi+pi),
					                                   cos(theta2));

    particleGun->SetParticleDefinition(scatteredIon);
    particleGun->SetParticleCharge(scatteredIonCharge);
    particleGun->SetParticlePosition(vertexPosition);
    //time, including the beam tracking before the vertex formation
    if(beamInteractionFlag == "on") {
      ActarSimBeamInfo *pBeamInfo = (ActarSimBeamInfo*) 0;
      if(gActarSimROOTAnalysis) {
        pBeamInfo = gActarSimROOTAnalysis->GetBeamInfo();
        particleGun->SetParticleTime(pBeamInfo->GetTimeVertex());
      }
    }
    else
      particleGun->SetParticleTime(0.0);

    particleGun->SetParticlePolarization(zero);
    particleGun->SetParticleMomentumDirection(direction1);
    particleGun->SetParticleEnergy(energy1);

    particleGun->GeneratePrimaryVertex(anEvent);

    particleGun->SetParticleDefinition(recoilIon);
    particleGun->SetParticleCharge(recoilIonCharge);
    particleGun->SetParticlePosition(vertexPosition);

    //time, including the beam tracking before the vertex formation
    if(beamInteractionFlag == "on") {
      ActarSimBeamInfo *pBeamInfo = (ActarSimBeamInfo*) 0;
      if(gActarSimROOTAnalysis) {
        pBeamInfo = gActarSimROOTAnalysis->GetBeamInfo();
        particleGun->SetParticleTime(pBeamInfo->GetTimeVertex());
      }
    }
    else
      particleGun->SetParticleTime(0.0);

    particleGun->SetParticlePolarization(zero);
    particleGun->SetParticleMomentumDirection(direction2);
    particleGun->SetParticleEnergy(energy2);

    particleGun->GeneratePrimaryVertex(anEvent);
  }

  //CASE E Reaction products kinematics calculated using Kine
  else if(reactionFromKineFlag == "on"){
    if(verboseLevel>0){
      G4cout << G4endl
             << " *************************************************** " << G4endl
             << " * ActarSimPrimaryGeneratorAction::GeneratePrimaries() " << G4endl
             << " * reactionFromKineFlag = on                           " << G4endl
             << " * Using KINE for the kinematics of reaction products."  << G4endl;
      G4cout << " *************************************************** "<< G4endl;
    }

    //Initial values for KINE. It is not allowed to create this initial values
    //in the constructor...
    if(!incidentIon) {
      incidentIon =  (G4Ions*) particleTable->GetIon(2, 8, 0.);  // 8He (S1)
      incidentIonCharge =  2;
    }
    if(!targetIon) {
      targetIon = (G4Ions*) particleTable->GetIon(6, 12, 0.);     // C12 (S2)
      targetIonCharge = 6;
    }
    if(!scatteredIon){
      scatteredIon = (G4Ions*) particleTable->GetIon(2, 8, 0.);  // 8He (S3)
      scatteredIonCharge = 2;
    }
    if(!recoilIon){
      recoilIon = (G4Ions*) particleTable->GetIon(6, 12, 0.);   // C12 (S4)
      recoilIonCharge = 6;
    }

    //KINE object...
    ActarSimKinePrimGenerator* KINE = new ActarSimKinePrimGenerator();

    KINE->SetMassOfProjectile(GetMassOfProjectile());
    KINE->SetMassOfTarget(GetMassOfTarget());
    KINE->SetMassOfScattered(GetMassOfScattered());
    KINE->SetMassOfRecoiled(GetMassOfRecoiled());

    KINE->SetExEnergyOfProjectile(GetExEnergyOfProjectile());
    KINE->SetExEnergyOfTarget(GetExEnergyOfTarget());
    KINE->SetExEnergyOfScattered(GetExEnergyOfScattered());
    KINE->SetExEnergyOfRecoiled(GetExEnergyOfRecoiled());

    KINE->SetLabEnergy(GetLabEnergy());

    // Random generator for scattered angle triggered by a messenger Cmd
    if(randomThetaFlag == "on") {
      SetThetaCMAngle((randomThetaMin +              // randomThetaMin, randomThetaMax, use exist ones
			                ((randomThetaMax-randomThetaMin) * G4UniformRand())) * rad);
      KINE->SetThetaCMAngle(GetThetaCMAngle()/deg);  // units: degree, deg=pi/180.
      if(verboseLevel>1)
        G4cout << " *** random CM Theta = " << GetThetaCMAngle() << ", it is "
               << GetThetaCMAngle()/rad << " rad = "
               << GetThetaCMAngle()/deg << " deg "<< G4endl;
    }
    else {
      KINE->SetThetaCMAngle(GetThetaCMAngle()/deg);  // units: degree
    }
    if(verboseLevel>1){
      G4cout << " KINE: Setting masses to :" << GetIncidentIon()->GetAtomicMass()
	           << " " << GetTargetIon()->GetAtomicMass()
	           << " " << GetScatteredIon()->GetAtomicMass()
	           << " " << GetRecoilIon()->GetAtomicMass()<< " "<< endl;
      G4cout << " KINE: Setting labEnergy to:" << KINE->GetLabEnergy() << G4endl;
      G4cout << " KINE: Setting targetExcitationEnergy to:"
	           << GetTargetIon()->GetExcitationEnergy() << G4endl;
      G4cout << " KINE: Setting projectileExcitationEnergy to:"
	           << GetIncidentIon()->GetExcitationEnergy() << G4endl;
      G4cout << " Kine: Setting excitation energy of Scattered particle to:"
	           << GetScatteredIon()->GetExcitationEnergy() << G4endl;
      G4cout << " KINE: Setting CM Angle to:" << KINE->GetThetaCMAngle() << " deg" << G4endl;
    }

    //Calling the relativistic kinematic calculation
    KINE->KineKinematics();

    if(KINE->GetNoSolution()) G4cout << "Kine NO solution, check your input!" << G4endl;

    if(verboseLevel>0){
      G4cout << " *************************************************** "<< G4endl;
      G4cout << " ***** KINE: Relativistic calculation invoked ****** "<< G4endl;
      G4cout << " *************************************************** "<< G4endl;
    }

    G4double thetaBeam1, thetaBeam2, energy1, energy2;

    thetaBeam1 = KINE->GetANGAs(0);    // unit: rad
    thetaBeam2 = KINE->GetANGAr(0);    // unit: rad
    energy1    = KINE->GetANGAs(1);    // unit: MeV
    energy2    = KINE->GetANGAr(1);    // unit: MeV

    if(verboseLevel>0){
      G4cout << "Kine: Scattered energy:" << KINE->GetANGAs(1) << " MeV" << G4endl;
      G4cout << "Kine: Recoriled energy:" << KINE->GetANGAr(1) << " MeV" << G4endl;
      G4cout << "Kine: Scattered angle:"  << KINE->GetANGAs(0)/deg << " deg" << G4endl;
      G4cout << "Kine: recoiled  angle:"  << KINE->GetANGAr(0)/deg << " deg" << G4endl;
      G4cout << "Kine: passed Scattered energy:" << energy1 << " MeV" << G4endl;
      G4cout << "Kine: passed Recoil energy:"    << energy2 << " MeV" << G4endl;
      G4cout << "Kine: passed Scattered angle:"  << theta1/deg  << " deg" << G4endl;
      G4cout << "Kine: passed Recoil angle:"     << theta2/deg  << " deg" << G4endl;
      G4cout << "deg=" << deg << ", MeV=" << MeV << G4endl;
    }
    G4double phiBeam1=0., phiBeam2=0.;
    phiBeam1 = twopi * G4UniformRand();         //flat probability in phi
    phiBeam2 = phiBeam1 + pi;

    // Euler transformation here for (theta1, phi1) and (theta2, phi2)
    G4double thetaLab1, phiLab1, thetaLab2, phiLab2;

    ActarSimBeamInfo *pBeamInfo = (ActarSimBeamInfo*) 0;
    if(gActarSimROOTAnalysis) pBeamInfo=gActarSimROOTAnalysis->GetBeamInfo();

    ActarSimEulerTransformation* EulerTransformer = new ActarSimEulerTransformation();

    EulerTransformer->SetThetaInBeamSystem(thetaBeam1);
    EulerTransformer->SetPhiInBeamSystem(phiBeam1);
    EulerTransformer->SetBeamDirectionAtVertexTheta(pBeamInfo->GetThetaVertex());
    EulerTransformer->SetBeamDirectionAtVertexPhi(pBeamInfo->GetPhiVertex());
    EulerTransformer->DoTheEulerTransformationBeam2Lab();   // Euler transformation for particle 1

    thetaLab1 = EulerTransformer->GetThetaInLabSystem();
    phiLab1   = EulerTransformer->GetPhiInLabSystem();
    if(randomPhiAngleFlag=="off") phiLab1 = GetUserPhiAngle()+pi;

    G4ThreeVector direction1 = G4ThreeVector(sin(thetaLab1)*cos(phiLab1),
                                             sin(thetaLab1)*sin(phiLab1),
                                             cos(thetaLab1));

    EulerTransformer->SetThetaInBeamSystem(thetaBeam2);
    EulerTransformer->SetPhiInBeamSystem(phiBeam2);
    EulerTransformer->DoTheEulerTransformationBeam2Lab();   // Euler transformation for particle 2

    thetaLab2 = EulerTransformer->GetThetaInLabSystem();
    phiLab2   = EulerTransformer->GetPhiInLabSystem();
    if(randomPhiAngleFlag=="off") phiLab2 = GetUserPhiAngle();

    G4ThreeVector direction2 = G4ThreeVector(sin(thetaLab2)*cos(phiLab2),
					                                   sin(thetaLab2)*sin(phiLab2),
					                                   cos(thetaLab2));
    delete EulerTransformer;

    particleGun->SetParticleDefinition(scatteredIon);
    particleGun->SetParticleCharge(scatteredIonCharge);
    particleGun->SetParticlePosition(vertexPosition);

    //time, including the beam tracking before the vertex formation
    if(beamInteractionFlag == "on") {
      ActarSimBeamInfo *pBeamInfo = (ActarSimBeamInfo*) 0;
      if(gActarSimROOTAnalysis) {
        pBeamInfo = gActarSimROOTAnalysis->GetBeamInfo();
        particleGun->SetParticleTime(pBeamInfo->GetTimeVertex());
      }
    }
    else
      particleGun->SetParticleTime(0.0);

    particleGun->SetParticlePolarization(zero);
    particleGun->SetParticleMomentumDirection(direction1);
    particleGun->SetParticleEnergy(energy1);

    particleGun->GeneratePrimaryVertex(anEvent);

    particleGun->SetParticleDefinition(recoilIon);
    particleGun->SetParticleCharge(recoilIonCharge);
    particleGun->SetParticlePosition(vertexPosition);

    //time, including the beam tracking before the vertex formation
    if(beamInteractionFlag == "on") {
      ActarSimBeamInfo *pBeamInfo = (ActarSimBeamInfo*) 0;
      if(gActarSimROOTAnalysis) {
        pBeamInfo = gActarSimROOTAnalysis->GetBeamInfo();
        particleGun->SetParticleTime(pBeamInfo->GetTimeVertex());
      }
    }
    else
      particleGun->SetParticleTime(0.0);

    particleGun->SetParticlePolarization(zero);
    particleGun->SetParticleMomentumDirection(direction2);
    particleGun->SetParticleEnergy(energy2);                     // use the exist code for this part.

    particleGun->GeneratePrimaryVertex(anEvent);
 
    G4double ExEnergyScattered= GetExEnergyOfScattered();
    if(ExEnergyScattered>0){   //Photon is emmited isotropically from vertex
      G4double cosTheta_gamma;
      G4double phi_gamma = twopi*G4UniformRand();
      G4double sinTheta_gamma;
      cosTheta_gamma = -1.0 + 2.0*G4UniformRand();
      sinTheta_gamma = sqrt(1 - cosTheta_gamma*cosTheta_gamma);
      G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
      G4String particleName;
      particleGun->SetParticleDefinition(particleTable->FindParticle(particleName="gamma"));
      particleGun->SetParticleMomentumDirection(G4ThreeVector(sinTheta_gamma*cos(phi_gamma),
                                                              sinTheta_gamma*sin(phi_gamma),
                                                              cosTheta_gamma));
      if(ExEnergyScattered<1.500 && ExEnergyScattered>.86)
        particleGun->SetParticleEnergy(ExEnergyScattered-0.854);
      else
        particleGun->SetParticleEnergy(ExEnergyScattered);
      particleGun->SetParticlePosition(vertexPosition);
      particleGun->SetParticleTime(0.0);
      particleGun->GeneratePrimaryVertex(anEvent);
      if(ExEnergyScattered<1.5&&ExEnergyScattered>.86){
        particleGun->SetParticleEnergy(0.854);
        phi_gamma = twopi*G4UniformRand();
        cosTheta_gamma = -1.0 + 2.0*G4UniformRand();
        sinTheta_gamma = sqrt(1 - cosTheta_gamma*cosTheta_gamma);
        particleGun -> SetParticleMomentumDirection(G4ThreeVector(sinTheta_gamma*cos(phi_gamma),
                                                                  sinTheta_gamma*sin(phi_gamma),
                                                                  cosTheta_gamma));
	      particleGun->SetParticlePosition(vertexPosition);
	      particleGun->SetParticleTime(0.0);
	      particleGun->GeneratePrimaryVertex(anEvent);
      }
    }//end if(ExEnergyScattered>0) 
  }

  // CASE F  Particle selected manually (using the messenger commands)
  else{
    if(verboseLevel>0){
      G4cout << G4endl
	           << " *************************************************** " << G4endl
	           << " * ActarSimPrimaryGeneratorAction::GeneratePrimaries() " << G4endl
	           << " * No particular event generator or kinematics code... " << G4endl
	           << " * A single particle is thrown using messenger commands."  << G4endl;
      G4cout << " *************************************************** "<< G4endl;
    }
    
    if(realisticBeamFlag == "on")
      particleGun->SetParticlePosition(vertexPosition);
    else
      particleGun->SetParticlePosition(G4ThreeVector());
    
    //DPLoureiro adding random distribution for polar and azimuthal angles
    G4double cosTheta, sinTheta;
    G4double y_coord;
    y_coord = -1 + 2.0*G4UniformRand();
    y_coord=10*y_coord/185.;
   
    if(randomThetaFlag == "on") {
      G4ParticleDefinition* pd = particleTable->FindParticle("alpha");
      //G4ParticleDefinition* pd = particleTable->FindParticle("proton");
      if(pd != 0)
        particleGun->SetParticleDefinition(pd);
     
      //G4cout<<cos(randomThetaMin*rad)<<" "<<cos(randomThetaMax*rad)<<G4endl;
      G4double CosRandomThetaMin=cos(randomThetaMin);     
      G4double CosRandomThetaMax=cos(randomThetaMax);     
      
      if(CosRandomThetaMin==1. && CosRandomThetaMax==0. ) {
        cosTheta = -1.0 + 2.0*G4UniformRand();
        sinTheta = sqrt(1 - cosTheta*cosTheta);
        //G4cout<<"UNIFORM"<<G4endl;     
      }
      else{
        //Theta = (randomThetaMin + (randomThetaMax-randomThetaMin) * G4UniformRand()) * rad;
        cosTheta = cos(randomThetaMin*rad)+(cos(randomThetaMax*rad)- cos(randomThetaMin*rad))*G4UniformRand();
        //cosTheta = cos(Theta);
        sinTheta = sqrt(1 - cosTheta*cosTheta);
        //G4cout<<"NON UNIFORM"<<G4endl;    
      }
    }
    else{ 
      sinTheta=0;
      cosTheta=1;
    }
    G4double phi;    
    if(randomPhiFlag == "on"){
      G4double CosRandomPhiMin=cos(randomPhiMin);     
      G4double CosRandomPhiMax=cos(randomPhiMax);     
      if(CosRandomPhiMin==1. && CosRandomPhiMax==1.) 
        phi=2*pi*G4UniformRand();
      else
        phi=randomPhiMin + ((randomPhiMax-randomPhiMin) * G4UniformRand()) * rad;
    }
    //else phi=pi/2;
    else phi=0;
    
    if(alphaSourceFlag == "on"){
      G4int i=rand() % 3;
      G4double alpha_energy[3]={5.16,5.49,5.81};
      //G4cout<<alpha_energy[i]<<G4endl;
      particleGun->SetParticleEnergy(alpha_energy[i]*MeV);
    }
    else{
      //particleGun->SetParticleEnergy(5.5*MeV);
      particleGun->SetParticleEnergy(GetLabEnergy());
      //incidentIon =  (G4Ions*) particleTable->GetIon(50, 134, 0.);  // 134Sn
      //incidentIon =  (G4Ions*) particleTable->GetIon(30, 80, 0.);  // 80Zn
      //incidentIon =  (G4Ions*) particleTable->GetIon(38, 90, 0.);  // 90Sr
      //incidentIonCharge =  50;
      //incidentIonCharge =  38;
      //particleGun->SetParticleDefinition(incidentIon);
    }

    //these are the cosines for an isotropic direction
    //particleGun -> SetParticleMomentumDirection(G4ThreeVector(0,0,1));
    //Selecting only positive Z
    //Circle of 2.5 mm radius
    Float_t radius=beamRadiusAtEntrance;
    //Float_t radius=32*mm;
    Float_t X0=0;
    Float_t Y0=0;
    //Float_t Z0= -(detector->GetChamberZLength()-detector->GetZGasBoxPosition());
    //G4double Z0= -29*mm;

    do{
      X0=-1.0 + 2.0*G4UniformRand();
      Y0=-1.0 + 2.0*G4UniformRand();
    }while((pow(X0,2)+pow(Y0,2)>1));
    X0=radius*X0;
    Y0=radius*Y0;
    //G4cout<<X0<<" "<<Y0<<endl;
    //particleGun->SetParticlePosition(G4ThreeVector(0.,0.,Z0));
    //particleGun->SetParticlePosition(G4ThreeVector(X0,Y0,Z0));
    //particleGun->SetParticlePosition(G4ThreeVector(X0,Y0,0.));
    particleGun->SetParticlePosition(G4ThreeVector(0.,0.,0.));
    if(cosTheta>0)
      //particleGun -> SetParticleMomentumDirection(G4ThreeVector(y_coord,sinTheta*sin(phi),cosTheta));
      particleGun -> SetParticleMomentumDirection(G4ThreeVector(sinTheta*cos(phi),sinTheta*sin(phi),cosTheta));
      //particleGun -> SetParticleMomentumDirection(G4ThreeVector(sinTheta*cos(phi),y_coord,cosTheta));
    else
      //particleGun -> SetParticleMomentumDirection(G4ThreeVector(-y_coord,sinTheta*sin(phi),-cosTheta));
      //particleGun -> SetParticleMomentumDirection(G4ThreeVector(-sinTheta*cos(phi),sinTheta*sin(phi),-cosTheta));
      //particleGun -> SetParticleMomentumDirection(G4ThreeVector(-sinTheta*cos(phi),y_coord,-cosTheta));
      particleGun -> SetParticleMomentumDirection(G4ThreeVector(0,0,1));
    
    particleGun->GeneratePrimaryVertex(anEvent);
  }
 
    // G4ParticleDefinition *pdef=particleGun->GetParticleDefinition();   
    // G4cout << " *************************************************** "<< G4endl;
    // G4cout<<pdef->GetAtomicNumber()<<" "<<pdef->GetAtomicMass()<<" "<<particleGun->GetParticleEnergy()/MeV<<endl;
    // G4cout << " *************************************************** "<< G4endl;


  //TODO SOlve this assymetry No theta or energies should be in the argument!
  
  //G4cout<<"HERE I AM!!!!!!!"<<G4endl;  
  ActarSimBeamInfo *pBeamInfo = (ActarSimBeamInfo*) 0;
  if(gActarSimROOTAnalysis){
    //D. Perez
    pBeamInfo = gActarSimROOTAnalysis->GetBeamInfo();
    //G4cout << pBeamInfo << G4endl
    //	   << "Theta1 "<< pBeamInfo->GetThetaEntrance() / deg << " deg"<<G4endl
    //	   << "Theta2 "<< pBeamInfo->GetThetaVertex() / deg << " deg"<<G4endl;
    //Histogramming
    gActarSimROOTAnalysis->GeneratePrimaries(anEvent,pBeamInfo);
    //gActarSimROOTAnalysis->GeneratePrimaries(anEvent,
    //					     theta1,
    //					     theta2,
    //					     energy1,
    //					     energy2);
  }

}
