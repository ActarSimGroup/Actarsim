///////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol
//*-- Date: 05/2006
//*-- Last Update: 20/06/06
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
//      .L digitizationMacro.C+;
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
//      theAmplificationManager.SetIsWireOn();
//      theAmplificationManager.SetWireAmplificationParameters(ra,s,h);
//
//      ra: radius of amplification wire: 5, 10, and 20 mu
//       s: spacing between two amplification wires: 2 or 2.3 mm
//       h: distance between the amplification wire and induction pads: 10 mm
//
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
// ActarPadSignal theActarPadSignal;
 // void progressbar(int percent)
 // {
 //   static stringstream bars;
 //   static int x = 0;
 //   string slash[4];
 //   slash[0] = "\\";
 //   slash[1] = "-";
 //   slash[2] = "/";
 //   slash[3] = "|";
 //   bars << "|";
 //   cout << "\r"; // carriage return back to beginning of line
 //   cout << bars.str() << " " << slash[x] << " " << percent << " %"; // print the bars and percentage
 //   fflush(stdout); 
 //   x++; // increment to make the slash appear to rotate
 //   if(x == 4)
 //     x = 0; // reset slash animation
 // }



//void digitEvents(char* inputFile, char* outputFile, Int_t numberOfEvents=0,Bool_t bTree=0){
void digitEvents(char* inputFile, char* outputFile, Int_t numberOfEvents=0){
  Bool_t bTree=kFALSE; 
 //
  // Function containing the event loop
  //

  gROOT->SetStyle("Default");
  //gSystem->Load("actarsim.sl"); //SHOULD BE LOADED BEFORE!
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  theDriftManager.ConnectToGeometry(&thePadsGeometry);
  theDriftManager.ConnectToAmplificationManager(&theAmplificationManager);
//   theDriftManager.ConnectToActarPadSignal(&theActarPadSignal);
  theDriftManager.GetStatus();

  // move calculation of the Mathieson Factors here to reduce number of function calls
  Double_t k1p = theAmplificationManager.GetMathiesonFactorK1P();
  Double_t k2p = theAmplificationManager.GetMathiesonFactorK2P();
  Double_t k3p = theAmplificationManager.GetMathiesonFactorK3P();
  Double_t k1n = theAmplificationManager.GetMathiesonFactorK1N();
  Double_t k2n = theAmplificationManager.GetMathiesonFactorK2N();
  Double_t k3n = theAmplificationManager.GetMathiesonFactorK3N();

  //input file and tree
  TFile *file1 = TFile::Open(inputFile);
  TTree* eventTree = (TTree*)file1->Get("The_ACTAR_Event_Tree");

  TClonesArray* simpleTrackCA;
  simpleTrackCA = new TClonesArray("ActarSimSimpleTrack",100);
  TBranch *branchTrack = eventTree->GetBranch("simpleTrackData");
  branchTrack->SetAddress(&simpleTrackCA);
  branchTrack->SetAutoDelete(kTRUE);

  //ClonesArray to the silicon Hits
  // TClonesArray *silHitsCA=new TClonesArray("ActarSimSilHit",100);
  //TBranch *branchSilHits=eventTree->GetBranch("silHits");
  //branchSilHits->SetAddress(&silHitsCA);
  //branchSilHits->SetAutoDelete(kTRUE);

  //ActarSimSilHit* silHit=new ActarSimSilHit;


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
  Int_t siliconhits=0;

  Int_t hits=0;
  TTree *T;
  if(numberOfEvents)    nevents = numberOfEvents;
  else   nevents = eventTree->GetEntries();

//   Int_t neventsSim = nevents/2;

  Int_t nb = 0;
  
  for(Int_t i=0;i<nevents;i++){
    if(i%1 == 0) printf("Event with strides:%d\n",i);
    //if(i%10 == 0) printf("Event with strides:%d\n",i);
    //Int_t percent=Float_t(i)/nevents*100;
    //if(i%10==0)
    //progressbar(percent);
  
  simpleTrackCA->Clear();
  //silHitsCA->Clear();
  nb += eventTree->GetEvent(i);
  
  stridesPerEvent = simpleTrackCA->GetEntries();
  //siliconhits = silHitsCA->GetEntries();
  if(siliconhits>0){
    ;
    //silHit=(ActarSimSilHit*)silHitsCA->At(0);
    //cout<<"silicon hitted "<<siliconhits<<"by an "<<silHit->GetParticleCharge()<<" Energy "<<silHit->GetEnergy()<<endl;
  }
  //if(stridesPerEvent>0&& siliconhits>0) {
  //  hits++;
  cout<<"# of strides-> "<<stridesPerEvent<<endl;
  if(stridesPerEvent>0) {
    
    //Clear the ClonesArray before filling it
    padSignalCA->Clear();

    Int_t numberOfPadsBeforeThisLoopStarted=0;
    
    //if(i%2!=0){ // comment this condition if digitization of beam tracks is not necessary.
    //cout<<"The number of strides is "<<stridesPerEvent<<endl;
    Char_t tname[256];
    sprintf(tname,"T%d",i);
    if(bTree)
      T=new TTree(tname,"Charge distributions");
    for(Int_t h=0;h<stridesPerEvent;h++){
      //cout<<"Stride # "<<h<<endl;
      cout<<"."<<flush;
      localTrack = (ActarSimSimpleTrack*)simpleTrackCA->At(h);
      
      //         cout << "strideLength=" << localTrack->GetStrideLength() << endl;
      
      //Once we know where the track is, we should know where the stride
      //limits are after the drift and diffussion of the electrons...
      projection->SetTrack(localTrack);
      //           if(localTrack->GetTrackID()==1){
      if(theDriftManager.CalculatePositionAfterDrift(projection)){
	if(bTree)
	  theDriftManager.CalculatePadsWithCharge(k1p, k2p, k3p, k1n, k2n, k3n,projection,padSignalCA,numberOfPadsBeforeThisLoopStarted,T);
	else  
	  theDriftManager.CalculatePadsWithCharge(k1p, k2p, k3p, k1n, k2n, k3n,projection,padSignalCA,numberOfPadsBeforeThisLoopStarted,0);
	//theDriftManager.CalculatePadsWithCharge(k1p, k2p, k3p, k1n, k2n, k3n,projection,padSignalCA,numberOfPadsBeforeThisLoopStarted);
	//gPad->WaitPrimitive();
	//             theDriftManager.CalculatePadsWithCharge(projection,padSignalCA,digiTree);
      }
    }
    cout<<endl;
    //}//end if(i%2!=0) 
    digiTree->Fill();
  }
  else{
    //cout<<"=================>Event Skipped"<<endl;
    //digiTree->Fill();
    continue;
  }
}
//   digiTree->Scan("padSignals.chargeDeposited");

outFile->Write();
outFile->Close();
//if(bTree)
//delete T;
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
