///////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol
//*-- Date: 05/2006
//*-- Last Update: 29/10/15
//*-- Copyright: GENP (Univ. Santiago de Compostela)
//
// --------------------------------------------------------------
//
// --------------------------------------------------------------
// Technical details...
//  diffusion: gaussian with sigma = sqrt(2Dx/w)
//  where D is the field dependent diff. coefficient, w the drift
//  velocity and x the distance
//  as described in A. Peisert and F. Sauli CERN84-08 (1984)
//
// --------------------------------------------------------------
// How to run this program:
// 1 - Run the simulation
//      actarsim batch1.mac
// 2 - Open a root session
//      root -l
// 3 - Run this macro inside root
//      gSystem->Load("actarsim.sl");
//      .L digitizationMacro.C+
//
//      thePadsGeometry.SetGeometryValues(Int_t geometryType,
//                                        Int_t padType,
//        			          Double_t xLength,
//        			          Double_t yLength,
//        			          Double_t zLength,
//        			          Double_t radius,
//                                        Double_t padSize);
//      where all distances are in mm
//
//      theDriftManager.SetDriftParameters(voltage, height, pressure, gasName);
//
//      theDriftManager.SetMagneticField(Double_t mag);     NOT WORKING YET
//      theDriftManager.SetLorentzAngle(Double_t lor);           in radians
//
//      theAmplificationManager.SetIsWireOn();    for a MAYA-like ACtive TARget
//      theAmplificationManager.SetWireAmplificationParameters(ra,s,h);
//
//      ra: radius of amplification wire: 5, 10, and 20 mu
//       s: spacing between two amplification wires: 2 or 2.3 mm
//       h: distance between the amplification wire and induction pads: 10 mm
//
//      (Optionally you can set theAmplificationManager.SetOldChargeCalculation(); for old Style calculations)
//      digitEvents(inputFile, outputFile, run#, numberOfEvents);
//
//  the number within brackets means:
//  the geometryType (0 for a box, 1 for cylinder)
//  the padType (0 for square, 1 for hexagonal)
//  the size is the radius if a cylinder or the typical pad
//                                 plane half-size if a box
//  the length is the ACTAR full length along beam line
//  the radius is the radius if the cylinder
//  the xLength is the half-length of the box along x
//  the yLength is the half-length of the box along y
//  the zLength is the half-length of the box along z
//  the padSize is the square or hexagonal pad side
//  the voltage is the voltage between the upper cathode and the Frish grid, in Volts
//  the height is the the distance between the upper cathode and the Frish grid, in mm
//  the pressure is the pressure of the active gas
//  the gasName is the name of the gas, right now only two values, "deuterium" of "isobutane"
//  the mag is the magnetic field inside the gas
//  the inputFile (output of the simulation)
//  the outputFile (output of the digitization)
//  the run  numbers (begin in 0)
///////////////////////////////////////////////////////////////////////
//
//  (Please note that if you only call "run/beamOn" once in
//  your macro, you can simply put a 0 as the run number and
//  you'll be done)
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////
// NOTE: it is possible to digitize over the endcaps of a cylinder
//  using the following method:
//
//  1) simulate in GEANT the reaction on a cylinder. Use the typical
//     GEANT4 directions and conventions
//  2) run this digitization macro following the previous instructions,
//  with the following details:
//    a)use a box in when selecting the geometry with xlength and zlength
//      equal to the radius, and yLength equal to the tube length
//    b)call the function
//      thePadsGeometry.SetEndCapModeOn()
//   This will change the data of your track... the direction Z will
//   be converted to Y and Y will be -X. Then, the typical box-like
//   projection on plane XZ will be equivalent to a projection on the
//   end cup of the cylinder.
//

#include "digit.h"
#include <sstream>
#include <TSystem.h>
#include <TFile.h>
#include <TStyle.h>
#include <TClonesArray.h>

using namespace std;

#if defined(__MAKECINT__)
#pragma link C++ class ActarPadSignal;
#pragma link C++ class projectionOnPadPlane;
#pragma link C++ class padsGeometry;
#pragma link C++ class amplificationManager;
#pragma link C++ class driftManager;
#endif

padsGeometry thePadsGeometry;
driftManager theDriftManager;
amplificationManager theAmplificationManager;

void digitEvents(char* inputFile, char* outputFile, Int_t numberOfEvents=0){
  // Digitization event loop
  Bool_t bTreeChargeDistribution=kFALSE;

  gROOT->SetStyle("Default");
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  theDriftManager.ConnectToGeometry(&thePadsGeometry);
  theDriftManager.ConnectToAmplificationManager(&theAmplificationManager);
  //theDriftManager.ConnectToActarPadSignal(&theActarPadSignal); //FUTURE SIGNAL TREATMENT
  theDriftManager.GetStatus();

  //input file and tree
  TFile *file1 = TFile::Open(inputFile);
  TTree* eventTree = (TTree*)file1->Get("The_ACTAR_Event_Tree");

  TClonesArray* simpleTrackCA;
  simpleTrackCA = new TClonesArray("ActarSimSimpleTrack",100);
  TBranch *branchTrack = eventTree->GetBranch("simpleTrackData");
  branchTrack->SetAddress(&simpleTrackCA);
  branchTrack->SetAutoDelete(kTRUE);

  ActarSimSimpleTrack* localTrack = new ActarSimSimpleTrack;

  //output File and Tree for the analysis result
  TFile* outFile = new TFile(outputFile,"RECREATE");
  outFile->cd();
  TTree* digiTree = new TTree("digiTree","digiEvent Tree");

  TClonesArray* padSignalCA;
  padSignalCA = new TClonesArray("ActarPadSignal",50);
  digiTree->Branch("padSignals",&padSignalCA);

  projectionOnPadPlane* projection= new projectionOnPadPlane();

  Int_t nevents=0;
  Int_t stridesPerEvent=0;
  //Int_t siliconhits=0;

  Int_t hits=0;
  TTree *TChargeDistribution;
  if(numberOfEvents) nevents = numberOfEvents;
  else   nevents = eventTree->GetEntries();
  cout<<"nevents= "<<nevents<<endl;
  //Int_t neventsSim = nevents/2;

  Int_t nb = 0;

  for(Int_t i=0;i<nevents;i++){
    if(i%100 == 0) printf("Event with strides:%d\n",i);

    simpleTrackCA->Clear();
    nb += eventTree->GetEvent(i);

    stridesPerEvent = simpleTrackCA->GetEntries();

    if(stridesPerEvent>0) {
      //cout<<"The number of strides is "<<stridesPerEvent<<endl;
      //Clear the ClonesArray before filling it
      padSignalCA->Clear();

      //added this to fill all the pads with charge.
      Int_t numberOfPadsBeforeThisLoopStarted=0;

      Char_t tname[256];
      sprintf(tname,"T%d",i);

      if(bTreeChargeDistribution)
	TChargeDistribution = new TTree(tname,"Charge distributions"); //HAPOL: IS THIS THE RIGHT PLACE??

      for(Int_t h=0;h<stridesPerEvent;h++){
	cout<<"."<<flush;
	localTrack = (ActarSimSimpleTrack*)simpleTrackCA->At(h);

	//Once we know where the track is, we should know where the stride
	//limits are after the drift and diffussion of the electrons...
	projection->SetTrack(localTrack);
	//if(localTrack->GetTrackID()==1){ //restricts to primaries
	if(theDriftManager.CalculatePositionAfterDrift(projection)){
    theDriftManager.CalculatePadsWithCharge(projection,padSignalCA,numberOfPadsBeforeThisLoopStarted);
	}
      }

      cout << endl;
      digiTree->Fill();
    }
    else{
      continue;
    }
  }

  outFile->Write();
  outFile->Close();

  cout<<"Total number of digitized events "<<nevents<<endl;
}

/*
  void defineHistograms(Double_t theRadius, Double_t theLength) {
  //
  //HERE THE HISTOGRAMS ARE DEFINED...
  //IT IS POSSIBLE TO CHANGE THE RANGE AND THE BINNING
  //
  }
*/
