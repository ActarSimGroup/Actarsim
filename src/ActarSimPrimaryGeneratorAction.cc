////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol
//*-- Date: 11/2004
//*-- Last Update: 21/06/16 by Hector Alvarez Pol
// --------------------------------------------------------------
// Description:
//   Actions to perform to generate a primary vertex
//
// --------------------------------------------------------------
// Comments:
//   - 04/12/15 Complete cleanup and recovering of functions
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

  //NOTE: FOR THE MOMENT THIS IS NOT ACCESIBLE. REVIEW IN THE FUTURE
  //create a pointer that gives access to the tabulated xs
  //pReadEvGen = new ActarSimEventGenerator();

  reactionQ = 0.0001;   //does 0 work? (QM)
  labEnergy = 100 *MeV;      // 15MeV*numero de nucleones (EI)
  incidentEnergy = 10 *MeV;
  // (EN) and (ENI) are taken from the target and projectile ion definitions
  thetaLabAngle = 45 * deg;   // 45 degrees (TH)
  randomThetaMin = 0.*deg;
  randomThetaMax = 180.*deg;
  beamRadiusAtEntrance = 0.;

  beamDirectionFlag = 1; // 0 : direction given by angles, 1 : direction given by vector

  //additional initial values for some variables
  SetBeamMomentumDirection(G4ThreeVector(0.0,0.0,1.0));
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
  const G4int verboseLevel = G4RunManager::GetRunManager()->GetVerboseLevel();
  if(verboseLevel>0)
    G4cout << G4endl << " _____ G4RunManager VerboseLevel = " <<verboseLevel<< G4endl;

  G4ThreeVector zero;
  G4double theta1=0.;
  G4double theta2=0.;
  G4double energy1=0.;
  G4double energy2=0.;

  ActarSimDetectorConstruction* detector = (ActarSimDetectorConstruction*)
    (G4RunManager::GetRunManager()->GetUserDetectorConstruction());

  //reading this parameter from the geometry
  if(!gasDetector) {
    gasDetector = (ActarSimGasDetectorConstruction*)(detector->GetGasDetector());
    if(gasDetector->GetDetectorGeometry()=="tube")
      lengthParameter = gasDetector->GetLengthGasTub();
    else
      lengthParameter = gasDetector->GetGasBoxSizeZ();

    if(verboseLevel>0){
      G4cout << "##################################################################" << G4endl
	      << "##### ActarSimPrimaryGeneratorAction::GeneratePrimaries()  #######" << G4endl
	      << "##### Information: Length of gas volume = " << lengthParameter << "    ######" << G4endl;
      G4cout << "##################################################################" << G4endl;
    }
  }
  //
  // CASE A1: Beam interaction in the gas. First the beam part...
  // While the beam is only emitted if beamInteractionFlag == "on"
  // the reaction products are always produced later in the code.
  //
  if(beamInteractionFlag == "on"){
    // The beam is described by the (G4Ions*) incidentIon (or incident particles)
    // and it is tracked in the gas in the even events (begining at zero, 0,2,4, ...)
    // while in the odd events (1,3,5, ...) the reaction products are tracked,
    // (CINE, KINE, from file, one output particle, ...) using some parameters
    // (vertex position, remaining beam ion energy, ...) obtained in the beam tracking.
    ActarSimBeamInfo *pBeamInfo = (ActarSimBeamInfo*) 0;

    if(gActarSimROOTAnalysis){
      pBeamInfo = gActarSimROOTAnalysis->GetBeamInfo();
      gActarSimROOTAnalysis->SetBeamInteractionFlag("on");
    }

    if(pBeamInfo->GetStatus() > 1){
      // Beam reached vertex: Tracking vertex products (later in this code)
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
    } //end of if(pBeamInfo->GetStatus() > 1)
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
    } //end of else if(pBeamInfo->GetStatus() == 1)
    else{ //corresponding to beam status equal to 0,
      // Setting the parameters to send a beam particle
      // they come from CINE or KINE or incident ion definition.
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
      pBeamInfo->SetCharge(incidentIonCharge);
      pBeamInfo->SetMass(incidentIon->GetPDGMass());

      // vertex_z0 decides the z position of the vertex. The beam is tracked till z0 is reached ...
      G4double vertex_z0 = 0;

      if(randomVertexZPositionFlag=="off"){
        vertex_z0 = vertexZPosition;
      }
      else if(randomVertexZPositionFlag=="on"){
        vertex_z0 = randomVertexZPositionMin + G4UniformRand()*(randomVertexZPositionMax-randomVertexZPositionMin);
      }
      // TODO: The probability of interaction is simply constant over the path on the gas.
      // This is the right place for introducing some dependence with the ion energy using
      // the reaction cross section, or even a simple exponential if a constant cross sections
      // approximation is suitable (non-resonant beam)

      pBeamInfo->SetZVertex(vertex_z0);
      pBeamInfo->SetEnergyEntrance(GetIncidentEnergy());

      if(realisticBeamFlag == "on") {
        // Emittance is defined in mm mrad
        // The polar angle at Entrance is defined by the relation between emitance and radiusAtEntrance.
        // this relation is roughtly emittance =  widthPos * widthAng ~ 2 * radiusMax * 2 * thetaMax
        // A step function is used for the position and angular distributions
        G4double radiusAtEntrance = beamRadiusAtEntrance * G4UniformRand(); //from 0 to beamRadiusAtEntrance
        G4double thetaWidth =  emittance/(4*beamRadiusAtEntrance/mm); //polar angle width between zero and maximum (as emittance is in mm*mrads, thetaWidth is given in mrads!!)
        G4double thetaAtEntrance = thetaWidth * G4UniformRand() * mrad;
        G4double phiAtEntrance = G4UniformRand() * twopi; //angle phi of momentum
        G4double phi2AtEntrance = G4UniformRand() * twopi; //angle for defining the entrance point

        G4ThreeVector directionAtEntrance = G4ThreeVector(sin(thetaAtEntrance)*cos(phiAtEntrance),
							                                            sin(thetaAtEntrance)*sin(phiAtEntrance),
							                                            cos(thetaAtEntrance));

        //Entrance coordinates (x0,y0,0) with angles (thetaAtEntrance, phiAtEntrance)
        G4double x0 = beamPosition.x() + radiusAtEntrance*cos(phi2AtEntrance);
        G4double y0 = beamPosition.y() + radiusAtEntrance*sin(phi2AtEntrance);
        G4double z0 = beamPosition.z();

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
      } //end of if(realisticBeamFlag == "on")
      else{ // simplest case: beam at (0,0,0) with direction along Z
	      particleGun->SetParticlePosition(beamPosition);
	      particleGun->SetParticleMomentumDirection(beamMomentumDirection);
	      pBeamInfo->SetPositionEntrance(beamPosition.x(),beamPosition.y(),beamPosition.z());
	      pBeamInfo->SetAnglesEntrance(0.,0.);
      }
      particleGun->SetParticleTime(0.0);
      particleGun->SetParticlePolarization(zero);
      particleGun->SetParticleEnergy(GetIncidentEnergy());
      particleGun->GeneratePrimaryVertex(anEvent);

      //Histogramming
      if(gActarSimROOTAnalysis)
        gActarSimROOTAnalysis->GenerateBeam(anEvent);

      return;  //end of the function after generating the beam...
               //waiting for next event for the reaction products.
    }
  }//end of  if(beamInteractionFlag == "on")

  // CASE A2:  now the beam is not included...
  else if(realisticBeamFlag == "on") {
    // Despite the name of the flag, there is no beam interacting in the gas.
    // RealisticBeamFlag means a reaction products being generated according to a
    // realistic vertex posisioning as if a beam were interacting.
    // Not anymore a gaussian, but a flat distribution in a given radius around Z axis.
    G4double radiusAtEntrance = beamRadiusAtEntrance * G4UniformRand(); //from 0 to beamRadiusAtEntrance
    G4double phi2AtEntrance = G4UniformRand() * twopi; //angle for defining the entrance point

    //Vertexcoordinates (vertex_x0,vertex_y0,vertex_z0) with angles (thetaAtEntrance, phiAtEntrance)
    G4double vertex_x0 = radiusAtEntrance*cos(phi2AtEntrance)+beamPosition.x();
    G4double vertex_y0 = radiusAtEntrance*sin(phi2AtEntrance)+beamPosition.y();
    G4double vertex_z0 = 0.;

    if(randomVertexZPositionFlag=="on"){
      vertex_z0 = randomVertexZPositionMin+ G4UniformRand()*(randomVertexZPositionMax-randomVertexZPositionMin);
    }
    else{
      //vertex_z0 = G4UniformRand()*(2.* lengthParameter);
      vertex_z0 = -lengthParameter + G4UniformRand()*lengthParameter;//If Z origin is at the center of GasBox

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
    vertexPosition.setX(vertex_x0);
    vertexPosition.setY(vertex_y0);
    vertexPosition.setZ(vertex_z0);
  }

  // After the beam, now different options for the reaction products!
  //
  // CASE B Reaction from Event-Generator
  //
  if(reactionFromEvGenFlag == "on") {
    if(verboseLevel>0){
      G4cout << G4endl
            << " *************************************************** " << G4endl
            << " * WARNING: THIS IS NOT IMPLEMENTED!!!"  << G4endl
            << " * ActarSimPrimaryGeneratorAction::GeneratePrimaries() " << G4endl
            << " * reactionFromEvGenFlag=on                            " << G4endl
            << " * An external event generator is used..."  << G4endl
            << " * WARNING: THIS IS NOT IMPLEMENTED!!!"  << G4endl;
      G4cout << " *************************************************** "<< G4endl;
    }
    /* REMOVING THE COMPLETE CASE B CODE
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

    */
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
      //TODO Modify completely this funtion!!! Dynamic objetc to avoid hardcoded size in arrays and limits
      G4double cine[91*4+1];
      G4int  ion1Z, ion1A, ion1Charge;
      G4int  ion2Z, ion2A, ion2Charge;

      inputFile >> ion1Z;    inputFile >> ion1A;    inputFile >> ion1Charge;
      inputFile >> ion2Z;    inputFile >> ion2A;    inputFile >> ion2Charge;

      if(verboseLevel>1){
        G4cout << G4endl
              << " First ion Z, A and Charge: "
              << ion1Z << " " << ion1A << " " << ion1Charge << G4endl
              << " Second ion Z, A and Charge: "
              << ion2Z << " " << ion2A << " " << ion2Charge << G4endl;
      }
      G4int i=0;
      do{
        inputFile >>  cine[i];
        i++;
      }while(!inputFile.eof());

      G4int rowsInputFile = (G4int) i/4;
      //Taken a random cinematic from the previous table
      G4int randomRow = (G4int)(G4UniformRand()*rowsInputFile);
      theta1 = pi*cine[randomRow*4]/180;
      theta2 = pi*cine[randomRow*4+2]/180;
      energy1 = cine[randomRow*4+1];
      energy2 = cine[randomRow*4+3];
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
    else { //no file found
      G4cout << " *************************************************** " << G4endl
             << "File " << reactionFile << " not found." << G4endl;
      G4cout << " *************************************************** "<< G4endl;
    }
  } //end of if(reactionFromFileFlag == "on")

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
	           << " * incident ion (atomic) mass: " << GetIncidentIon()->GetAtomicMass() << G4endl
	           << " * target ion (atomic) mass: " << GetTargetIon()->GetAtomicMass() << G4endl
	           << " * scattered ion (atomic) mass: " << GetScatteredIon()->GetAtomicMass() << G4endl
	           << " * recoil ion (atomic) mass: " << GetRecoilIon()->GetAtomicMass() << G4endl;
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
    if(verboseLevel>2){
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

    if(verboseLevel>1){
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
      G4cout << " KINE: Setting (atomic) masses to :" << GetIncidentIon()->GetAtomicMass()
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

    if(verboseLevel>1){
      G4cout << " *************************************************** "<< G4endl;
      G4cout << " ***** KINE: Relativistic calculation invoked ****** "<< G4endl;
      G4cout << " *************************************************** "<< G4endl;
    }

    G4double thetaBeam1, thetaBeam2;

    thetaBeam1 = KINE->GetANGAs(0);    // unit: rad
    thetaBeam2 = KINE->GetANGAr(0);    // unit: rad
    energy1    = KINE->GetANGAs(1);    // unit: MeV
    energy2    = KINE->GetANGAr(1);    // unit: MeV

    if(verboseLevel>1){
      G4cout << "Kine: Scattered energy:" << KINE->GetANGAs(1) << " MeV" << G4endl;
      G4cout << "Kine: Recoiled energy:" << KINE->GetANGAr(1) << " MeV" << G4endl;
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
    particleGun->SetParticleEnergy(energy2);

    particleGun->GeneratePrimaryVertex(anEvent);

    //TODO: Check if this part can be used and the status
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

    if(!particleGun->GetParticleDefinition()) {
      G4ParticleDefinition* pd = particleTable->FindParticle("proton");
      if(pd != 0) particleGun->SetParticleDefinition(pd);
    }

    //Random distribution for polar and azimuthal angles

    G4double theta;

    if(randomThetaFlag == "on") {

      G4double CosRandomThetaMin=cos(randomThetaMin);
      G4double CosRandomThetaMax=cos(randomThetaMax);

      if(CosRandomThetaMin==1. && CosRandomThetaMax==0. ) {
	theta=2*pi*G4UniformRand();
      }
      else{
	theta=randomThetaMin + ((randomThetaMax-randomThetaMin) * G4UniformRand()) * rad;
      }
    }
    else{
      theta=GetUserThetaAngle();
    }

    if(theta){;}

    G4double phi;

    if(randomPhiFlag == "on"){

      G4double CosRandomPhiMin=cos(randomPhiMin);
      G4double CosRandomPhiMax=cos(randomPhiMax);

      if(CosRandomPhiMin==1. && CosRandomPhiMax==1.)
        phi=2*pi*G4UniformRand();
      else
        phi=randomPhiMin + ((randomPhiMax-randomPhiMin) * G4UniformRand()) * rad;
    }
    else phi=GetUserPhiAngle();

    if(phi){;}

    if(alphaSourceFlag == "on"){
      G4ParticleDefinition* pd = particleTable->FindParticle("alpha");
      if(pd != 0) particleGun->SetParticleDefinition(pd);
      G4int i=rand() % 3;
      G4double alpha_energy[3]={5.15,5.48,5.8};
      particleGun->SetParticleEnergy(alpha_energy[i]*MeV);
    }
    else{
      particleGun->SetParticleEnergy(GetIncidentEnergy());
    }

    //Piotr: for the p/alpha discrimination test
    //G4double ParticleEnergy = GetIncidentEnergy() * G4UniformRand();
    //particleGun->SetParticleEnergy(ParticleEnergy);

    G4double radiusAtEntrance = beamRadiusAtEntrance * G4UniformRand(); //from 0 to beamRadiusAtEntrance

    G4double phi2AtEntrance = G4UniformRand() * twopi; //angle for defining the entrance point

    //Entrance coordinates (x0,y0,0)
    G4double x0 = beamPosition.x() + radiusAtEntrance*cos(phi2AtEntrance);
    G4double y0 = beamPosition.y() + radiusAtEntrance*sin(phi2AtEntrance);
    G4double z0 = beamPosition.z();

    //particleGun->SetParticlePosition(beamPosition);
    particleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));

    //Particle momentum
    if(randomThetaFlag == "on" || randomPhiFlag == "on" || !beamDirectionFlag) particleGun->SetParticleMomentumDirection(G4ThreeVector(sin(theta)*cos(phi),sin(phi),cos(theta)*cos(phi) ) );
    else if(beamDirectionFlag) particleGun->SetParticleMomentumDirection(beamMomentumDirection);

    particleGun->GeneratePrimaryVertex(anEvent);
  }

  ActarSimBeamInfo *pBeamInfo = (ActarSimBeamInfo*) 0;
  if(gActarSimROOTAnalysis){
    pBeamInfo = gActarSimROOTAnalysis->GetBeamInfo();
    //Histogramming
    gActarSimROOTAnalysis->GeneratePrimaries(anEvent,pBeamInfo);
  }

}
