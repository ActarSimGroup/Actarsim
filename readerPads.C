///////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol
//*-- Date: 06/2005
//*-- Last Update: 11/11/15
//*-- Copyright: GENP (Univ. Santiago de Compostela)
//
// --------------------------------------------------------------
// This macro plots the output of the ActarSim digitization:
//    - plots the so-called ActarPadSignal's which contains the
//      pads calculated after digitization of the track strides.
//      The strides are groups of one or more than one steps
//      created to reduce the huge amount of information stored
//      if full Tracks are used. Strides are calculated only for
//      the two primaries of the reaction. In the digitization,
//      the electrons from the strides drift and the projections
//      and the charge induced in the pads are calculated as a
//      function of geometry and drift parameters. This macro
//      shows the pads structure resulting from the digitization
//
// --------------------------------------------------------------
// How to run this program:
// 1 - Run the simulation
//      actarsim batch1.mac (or any other way you know)
// 2 - Open a root session
//      root -l
// 3 - Run the digitization macro inside root
//      gSystem->Load("libactar.sl");
//      .L digitizationMacro.C+
//      ...more instruction in the corresponding macro...
// 3 - Run this macro inside root
//      gSystem->Load("libactar.sl");
//      .L readerPads.C+
//
// [NOTE: in the commands related with thePadsGeometry, copy the lines
//        you have used in digitizationMacro.C]
//
//      thePadsGeometry.SetGeometryValues(UInt_t geometryType,
//                                        UInt_t padType,
//        			          Double_t xLength,
//        			          Double_t yLength,
//        			          Double_t zLength,
//        			          Double_t radius,
//                                        Double_t padSize)
//      where all distances are in mm
//
//      thePadsGeometry.SetEndCapModeOn()
//
//      reader(inputFile, digiFile, run#, event#)
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
//  the inputFile (output of the digitization)
//  the run and the event numbers
//  (event number is needed only for the track viewer).
//  Both numbers begin in 0 !!
//  Write reader(inputFile, geoType, length, size, run#,-1)
//  to see all the events together!
//
//  (Please note that if you only call "run/beamOn" once in
//  your macro, you can simply put a 0 as the run number and
//  you'll be done)
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////
//
// NOTE: it is possible to show the digitization over the endcaps
//       of a cylinder using the following method:
//
//  1) run this visualization macro following the previous instructions,
//  with the following details:
//    a)use a box in when selecting the geometry with xlength and zlength
//      equal to the radius, and yLength equal to the tube length
//    b)call the function
//      thePadsGeometry.SetEndCapModeOn()
//   before of the call to the function reader(...)
//
//   This will change the data of your track... the direction Z will
//   be converted to Y and Y will be -X. Then, the typical box-like
//   projection on plane XZ will be equivalent to a projection on the
//   end cup of the cylinder.
//

#include "digit.h"
#include <TSystem.h>
#include <TFile.h>
#include <TStyle.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TLatex.h>
#include <TControlBar.h>
#include <TTimer.h>
using namespace std;

#if defined(__MAKECINT__)
#pragma link C++ class ActarPadSignal;
#pragma link C++ class projectionOnPadPlane;
#pragma link C++ class padsGeometry;
#pragma link C++ class amplificationManager;
#pragma link C++ class driftManager;
#endif

const char* inputSimFile_g;
const char* inputDigiFile_g;
Int_t run_g = 0;
Int_t event_g = 0;
TTimer* timer;
Int_t shown=0;

//TCanvas c3("c3","Pads viewer",0,0,600,600);

padsGeometry thePadsGeometry;
driftManager theDriftManager;

void reader(const char* inputSimFile, const char* inputDigiFile, Int_t run, Int_t event, Int_t select);

void guiForPads(Int_t firstEvent=0) {
  // This macro shows one event and the derivative in both sides of
  // the twon ionization chamber (left and right labels have no meaning)
  //.L readTree.C;

  event_g=firstEvent;
  TControlBar *menu = new TControlBar("vertical","ACTARSIM Pads Viewer",800,650);
  menu->AddButton("      First      ","reader(inputSimFile_g,inputDigiFile_g,run_g, 0)","First event");
  menu->AddButton("      Go to event...      ","reader(inputSimFile_g,inputDigiFile_g,run_g, event_g, 1)","Write the event number on interpreter");
  menu->AddButton("      Next       ","reader(inputSimFile_g,inputDigiFile_g,run_g, event_g)","Next event");
  menu->AddButton("      Previous       ","reader(inputSimFile_g,inputDigiFile_g,run_g, event_g-2)","Previous event");
  menu->AddButton("      Run continuously       ","loop()","Press and see the events...");
  menu->AddButton("      Stop       ","loopStop()","Stops the exhibition");
  menu->AddButton("      See all the events   ","reader(inputSimFile_g,inputDigiFile_g,run_g, -1)","See all the events added");
  menu->AddButton("      Quit       ",".q","Quit Root");
  gROOT->SaveContext();
  menu->Show();
  reader(inputSimFile_g,inputDigiFile_g,run_g, 0, 0);
}

void loop(){
  timer = new TTimer(1000);
  timer->SetCommand("reader(inputSimFile_g,inputDigiFile_g,run_g, event_g)");
  timer->TurnOn();
  cout <<" loop() recalled"<<endl;
}

void loopStop(){
  timer->Stop();
}

void reader(const char* inputSimFile, const char* inputDigiFile, Int_t run, Int_t event, Int_t select=0){

  gROOT->SetStyle("Default");
  //gSystem->Load("actarsim.sl");
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  if(!shown){
    cout << "Reading input simulation file " << inputSimFile << endl;
    cout << " input digitization file " << inputDigiFile << endl;
    cout << " for run " << run << " and event " << event << " (- 1 means all events)." << endl;}
  inputSimFile_g = inputSimFile;
  inputDigiFile_g = inputDigiFile;
  run_g = run;
  event_g = event;

  //HERE THE HISTOGRAMS ARE DEFINED...
  //IT IS POSSIBLE TO CHANGE THE RANGE AND THE BINNING
  UInt_t histoBins = 250;
  UInt_t histoBins2 = 250;
  histoBins = thePadsGeometry.GetNumberOfRows();
  histoBins2 = thePadsGeometry.GetNumberOfColumns();

  if(!shown){
  cout << "Values taken for the visualization" << endl
       << "bins in X: " <<  histoBins<< endl
       << "bins in Z: " <<  histoBins2<< endl
       << "X size (half-length of box): " <<  thePadsGeometry.GetXLength()<< endl
       << "Y size (half-length of box): " <<  thePadsGeometry.GetYLength()<< endl
       << "Z size (half-length of box or cylinder): " <<  thePadsGeometry.GetZLength()<< endl
       << "radius (if cylinder): " <<  thePadsGeometry.GetRadius()<< endl;
  }

  //c3.cd();
  TH2D* htrackInPads=0;
  TH2D* hdriftTimeInPads=0;
  TH2D* hdepth1InPads=0;
  TH2D* hdepth2InPads=0;
  if(thePadsGeometry.GetGeoType() == 0){  //Box
    if(thePadsGeometry.GetEndCapMode()){  //Projecting on EndCap
      htrackInPads =
	new TH2D("htrackInPads",
		 "All tracks in the XY Pads Plane (endCup mode)",
		 histoBins, -(thePadsGeometry.GetXLength())+thePadsGeometry.GetSideBlankSpaceX(),
		 thePadsGeometry.GetXLength()-thePadsGeometry.GetSideBlankSpaceX(),
		 histoBins2, -(thePadsGeometry.GetZLength())+thePadsGeometry.GetSideBlankSpaceZ(),
		 thePadsGeometry.GetZLength()-thePadsGeometry.GetSideBlankSpaceZ());//in [mm]
      htrackInPads->SetYTitle("Y [mm]");
      htrackInPads->SetXTitle("X [mm]");

      hdriftTimeInPads =
	new TH2D("hdriftTimeInPads",
		 "All tracks in the XZ Pads Plane with drift time",
		 histoBins, -(thePadsGeometry.GetXLength())+thePadsGeometry.GetSideBlankSpaceX(),
		 thePadsGeometry.GetXLength()-thePadsGeometry.GetSideBlankSpaceX(),
		 histoBins2, -(thePadsGeometry.GetZLength())+thePadsGeometry.GetSideBlankSpaceZ(),
		 thePadsGeometry.GetZLength()-thePadsGeometry.GetSideBlankSpaceZ());//in [mm]
      hdriftTimeInPads->SetYTitle("Y [mm]");
      hdriftTimeInPads->SetXTitle("X [mm]");

      hdepth1InPads =
	new TH2D("hdepth1InPads",
		 "track In the Drift-Z Pads Plane",
		 histoBins, 0., 2.1*thePadsGeometry.GetYLength()/theDriftManager.GetDriftVelocity(),
		 histoBins2,  -(thePadsGeometry.GetZLength())+thePadsGeometry.GetSideBlankSpaceZ(),
		 thePadsGeometry.GetZLength()-thePadsGeometry.GetSideBlankSpaceZ());
      hdepth1InPads->SetYTitle("Y [mm]");
      hdepth1InPads->SetXTitle("(drift) time [ns]");

      hdepth2InPads =
	new TH2D("hdepth2InPads",
		 "track In the Drift-X Pads Plane",
		 histoBins, 0, 2.1*thePadsGeometry.GetYLength()/theDriftManager.GetDriftVelocity(),
		 histoBins, -(thePadsGeometry.GetXLength())+thePadsGeometry.GetSideBlankSpaceX(),
		 thePadsGeometry.GetXLength()-thePadsGeometry.GetSideBlankSpaceX());
      hdepth2InPads->SetYTitle("X [mm]");
      hdepth2InPads->SetXTitle("(drift) time [ns]");
    }
    else{ //Projecting on lower pad plane (ACTAR_TPC or MAYA)
      htrackInPads =
	new TH2D("htrackInPads",
		 "All tracks in the XZ Pads Plane",
		 histoBins, -(thePadsGeometry.GetXLength())+thePadsGeometry.GetSideBlankSpaceX(),
		 thePadsGeometry.GetXLength()-thePadsGeometry.GetSideBlankSpaceX(),
		 histoBins2, -(thePadsGeometry.GetZLength())+thePadsGeometry.GetSideBlankSpaceZ(),
		 thePadsGeometry.GetZLength()-thePadsGeometry.GetSideBlankSpaceZ());// in [mm]
      htrackInPads->SetYTitle("Z [mm]");
      htrackInPads->SetXTitle("X [mm]");

      hdriftTimeInPads =
	new TH2D("hdriftTimeInPads",
		 "All tracks in the XZ Pads Plane with drift time",
		 histoBins, -(thePadsGeometry.GetXLength())+thePadsGeometry.GetSideBlankSpaceX(),
		 thePadsGeometry.GetXLength()-thePadsGeometry.GetSideBlankSpaceX(),
		 histoBins2, -(thePadsGeometry.GetZLength())+thePadsGeometry.GetSideBlankSpaceZ(),
		 thePadsGeometry.GetZLength()-thePadsGeometry.GetSideBlankSpaceZ());// in [mm]
      hdriftTimeInPads->SetYTitle("Z [mm]");
      hdriftTimeInPads->SetXTitle("X [mm]");

      hdepth1InPads =
	new TH2D("hdepth1InPads",
		 "track In the Drift-Z Pads Plane",
		 histoBins, 0, 2.1*thePadsGeometry.GetYLength()/theDriftManager.GetDriftVelocity(),
		 histoBins2, -(thePadsGeometry.GetZLength())+thePadsGeometry.GetSideBlankSpaceZ(),
		 thePadsGeometry.GetZLength()-thePadsGeometry.GetSideBlankSpaceZ());
      hdepth1InPads->SetYTitle("Z [mm]");
      hdepth1InPads->SetXTitle("(drift) time [ns]");

      hdepth2InPads =
	new TH2D("hdepth2InPads",
		 "track In the Drift-X Pads Plane",
		 histoBins, 0, 2.1*thePadsGeometry.GetYLength()/theDriftManager.GetDriftVelocity(),
		 histoBins, -(thePadsGeometry.GetXLength())+thePadsGeometry.GetSideBlankSpaceX(),
		 thePadsGeometry.GetXLength()-thePadsGeometry.GetSideBlankSpaceX());
      hdepth2InPads->SetYTitle("X [mm]");
      hdepth2InPads->SetXTitle("(drift) time [ns]");
    }
  }
  else if(thePadsGeometry.GetGeoType() == 1){ // Tube
    htrackInPads =
      new TH2D("htrackInPads",
	       "All tracks in the #phi; Z Pads Plane",
	       histoBins, -3.1416, 3.1416,
	       histoBins2, -(thePadsGeometry.GetZLength())+thePadsGeometry.GetSideBlankSpaceZ(),
	       thePadsGeometry.GetZLength()-thePadsGeometry.GetSideBlankSpaceZ());// in [mm]
    htrackInPads->SetYTitle("Z [mm]");
    htrackInPads->SetXTitle("#phi [rad]");

    hdriftTimeInPads =
      new TH2D("hdriftTimeInPads",
	       "All tracks in the #phi; Z Pads Plane with drift time",
	       histoBins, -3.1416, 3.1416,
	       histoBins2, -(thePadsGeometry.GetZLength())+thePadsGeometry.GetSideBlankSpaceZ(),
	       thePadsGeometry.GetZLength()-thePadsGeometry.GetSideBlankSpaceZ());// in [mm]
    hdriftTimeInPads->SetYTitle("Z [mm]");
    hdriftTimeInPads->SetXTitle("#phi [rad]");

    hdepth1InPads =
      new TH2D("hdepth1InPads",
	       "track In the Drift-Z Pads Plane",
	       histoBins, 0, 1.1*thePadsGeometry.GetRadius()/theDriftManager.GetDriftVelocity(),
	       histoBins2, -(thePadsGeometry.GetZLength())+thePadsGeometry.GetSideBlankSpaceZ(),
	       thePadsGeometry.GetZLength()-thePadsGeometry.GetSideBlankSpaceZ());
    hdepth1InPads->SetYTitle("Z [mm]");
    hdepth1InPads->SetXTitle("(drift) time [ns]");

    hdepth2InPads =
      new TH2D("hdepth2InPads",
	       "track In the Drift-phi Pads Plane",
	       histoBins, 0, 1.1*thePadsGeometry.GetRadius()/theDriftManager.GetDriftVelocity(),
	       histoBins, -3.1416, 3.1416);
    hdepth2InPads->SetYTitle("#phi [rad]");
    hdepth2InPads->SetXTitle("(drift) time [ns]");
  }

  TFile *simFile = TFile::Open(inputSimFile);
  TTree *eventTree = (TTree*)simFile->Get("The_ACTAR_Event_Tree");
  Int_t neventsSim = eventTree->GetEntries();

  TFile *digiFile = TFile::Open(inputDigiFile);
  TTree *digiTree = (TTree*)digiFile->Get("digiTree");
  Int_t nevents = digiTree->GetEntries();

  if(!shown) {
    cout << "neventsSim=" << neventsSim << endl;
    cout << "nevents=" << nevents << endl;
  }
  if(select==1)  {
    Int_t num=0;
    cout << "Set the event number you wanna see: " <<endl;
    cin >> num;
    if(num>nevents) {
      cout << "Warning: Event larger than tree size!" <<endl;
    }
    else event = num;
  }

  TClonesArray* padSignalCA;
  ActarPadSignal** beamPadSginal;
  ActarPadSignal** productPadSignal;
  padSignalCA = new TClonesArray("ActarPadSignal",50);
  digiTree -> Branch("padSignals",&padSignalCA);

  ActarPadSignal* padSignal = new ActarPadSignal;

  TBranch *branchDigi = digiTree->GetBranch("padSignals");
  branchDigi->SetAddress(&padSignalCA);
  branchDigi->SetAutoDelete(kTRUE);

  Int_t padsPerEvent = 0;
  Int_t nb = 0;
  Int_t beamPadsWithSignalPerEvent, productPadsWithSignalPerEvent;


  if(event!=-1){ //only one event to be displayed
    padSignalCA->Clear();
    nb += digiTree->GetEvent(event);
    padsPerEvent = padSignalCA->GetEntries();
    if(padsPerEvent>0) {
      //padSignal = new ActarPadSignal*[padsPerEvent];
      //for(Int_t w=0;w<padsPerEvent;w++){
      //  padSignal[w] = (ActarPadSignal*) padSignalCA->At(w);
      //}
      for(Int_t h=0;h<padsPerEvent;h++){
	padSignal = (ActarPadSignal*) padSignalCA->At(h);

	if(thePadsGeometry.GetGeoType() == 0){
	  if(thePadsGeometry.GetEndCapMode()){
	    htrackInPads->Fill(thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).X()+thePadsGeometry.GetSideBlankSpaceX(),
			       thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).Z()
			       - thePadsGeometry.GetZLength(),
			       padSignal->GetChargeDeposited());
	    hdriftTimeInPads->Fill(thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).X()+thePadsGeometry.GetSideBlankSpaceX(),
				   thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).Z()+thePadsGeometry.GetSideBlankSpaceZ()
				   - thePadsGeometry.GetZLength(),
				   padSignal->GetInitTime());
	    hdepth1InPads->Fill(padSignal->GetInitTime(),
				thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).Z()+thePadsGeometry.GetSideBlankSpaceZ()
				- thePadsGeometry.GetZLength(),
				padSignal->GetChargeDeposited());
	    hdepth2InPads->Fill(padSignal->GetInitTime(),
				thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).X()+thePadsGeometry.GetSideBlankSpaceX(),
				padSignal->GetChargeDeposited());
	  }
	  else{
	    htrackInPads->Fill(thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).X()+thePadsGeometry.GetSideBlankSpaceX(),
			       thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).Z()+thePadsGeometry.GetSideBlankSpaceZ(),
			       padSignal->GetChargeDeposited());
	    hdriftTimeInPads->Fill(thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).X()+thePadsGeometry.GetSideBlankSpaceX(),
				   thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).Z()+thePadsGeometry.GetSideBlankSpaceZ(),
				   padSignal->GetInitTime());
	    hdepth1InPads->Fill(padSignal->GetInitTime(),
				thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).Z()+thePadsGeometry.GetSideBlankSpaceZ(),
				padSignal->GetChargeDeposited());
	    hdepth2InPads->Fill(padSignal->GetInitTime(),
				thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).X()+thePadsGeometry.GetSideBlankSpaceX(),
				padSignal->GetChargeDeposited());
	  }
	}
	else if(thePadsGeometry.GetGeoType() == 1){
	  htrackInPads->Fill(thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).Phi(),
			     thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).Z()+thePadsGeometry.GetSideBlankSpaceZ(),
			     padSignal->GetChargeDeposited());
	  hdriftTimeInPads->Fill(thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).Phi(),
				 thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).Z()+thePadsGeometry.GetSideBlankSpaceZ(),
				 padSignal->GetInitTime());
	  hdepth1InPads->Fill(padSignal->GetInitTime(),
			      thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).Z()+thePadsGeometry.GetSideBlankSpaceZ(),
			      padSignal->GetChargeDeposited());
	  hdepth2InPads->Fill(padSignal->GetInitTime(),
			      thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).X()+thePadsGeometry.GetSideBlankSpaceX(),
			      padSignal->GetChargeDeposited());
	}
      }
    }
  }
  else {
    for(Int_t i=0;i<nevents;i++){
      if(i%1000 == 0) printf("Event:%d\n",i);
      padSignalCA->Clear();
      nb += digiTree->GetEvent(i);
      padsPerEvent = padSignalCA->GetEntries();

      if(padsPerEvent>0) {
	//padSignal = new ActarPadSignal*[padsPerEvent];
	//for(Int_t w=0;w<padsPerEvent;w++){
	//  padSignal[w] = (ActarPadSignal*) padSignalCA->At(w);
	//}
	for(Int_t h=0;h<padsPerEvent;h++){
	  padSignal = (ActarPadSignal*) padSignalCA->At(h);

	  if(thePadsGeometry.GetGeoType() == 0){  //box
	    if(thePadsGeometry.GetEndCapMode()){  //projection on endcap
	      htrackInPads->Fill(thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).X()+thePadsGeometry.GetSideBlankSpaceX(),
				 thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).Z()+thePadsGeometry.GetSideBlankSpaceZ()
				 - thePadsGeometry.GetZLength(),
				 padSignal->GetChargeDeposited());
	      hdriftTimeInPads->Fill(thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).X()+thePadsGeometry.GetSideBlankSpaceX(),
				     thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).Z()+thePadsGeometry.GetSideBlankSpaceZ()
				     - thePadsGeometry.GetZLength(),
				     padSignal->GetInitTime());
	      hdepth1InPads->Fill(padSignal->GetInitTime(),
				  thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).Z()+thePadsGeometry.GetSideBlankSpaceZ()
				  - thePadsGeometry.GetZLength(),
				  padSignal->GetChargeDeposited());
	      hdepth2InPads->Fill(padSignal->GetInitTime(),
				  thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).X()+thePadsGeometry.GetSideBlankSpaceX(),
				  padSignal->GetChargeDeposited());
	    }
	    else{ //Projecting on lower pad plane (ACTAR_TPC or MAYA)
	      htrackInPads->Fill(thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).X()+thePadsGeometry.GetSideBlankSpaceX(),
				 thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).Z()+thePadsGeometry.GetSideBlankSpaceZ(),
				 padSignal->GetChargeDeposited());
	      hdriftTimeInPads->Fill(thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).X()+thePadsGeometry.GetSideBlankSpaceX(),
				     thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).Z()+thePadsGeometry.GetSideBlankSpaceZ(),
				     padSignal->GetInitTime());
	      hdepth1InPads->Fill(padSignal->GetInitTime(),
				  thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).Z()+thePadsGeometry.GetSideBlankSpaceZ(),
				  padSignal->GetChargeDeposited());
	      hdepth2InPads->Fill(padSignal->GetInitTime(),
				  thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).X()+thePadsGeometry.GetSideBlankSpaceX(),
				  padSignal->GetChargeDeposited());
	    }
	  }
	  else if(thePadsGeometry.GetGeoType() == 1){ //tube
	    htrackInPads->Fill(thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).Phi(),
			       thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).Z()+thePadsGeometry.GetSideBlankSpaceZ(),
			       padSignal->GetChargeDeposited());
	    hdriftTimeInPads->Fill(thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).Phi(),
				   thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).Z()+thePadsGeometry.GetSideBlankSpaceZ(),
				   padSignal->GetInitTime());
	    hdepth1InPads->Fill(padSignal->GetInitTime(),
				thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).Z()+thePadsGeometry.GetSideBlankSpaceZ(),
			        padSignal->GetChargeDeposited());
	    hdepth2InPads->Fill(padSignal->GetInitTime(),
				thePadsGeometry.CoordinatesCenterOfPad(padSignal->GetPadNumber()).Phi(),
				padSignal->GetChargeDeposited());
	  }
	}
      }
    }
  }

  if(thePadsGeometry.GetGeoType() == 0){
    if(thePadsGeometry.GetEndCapMode()){
      TCanvas* c3 = new TCanvas("c3","Pads in pad (XY) plane; box geometry; end cap mode",0,0,600,600);
      TLatex l;
      l.SetTextAlign(12);
      l.SetTextSize(0.05);
      //l.DrawLatex(0.1,0.1,"Pads in pad (XY) plane; box geometry; end cap mode");

      c3->SetFillColor(0);
      c3->Divide(2,2);
      c3->Draw();
      c3->cd(1);
      htrackInPads->Draw("ZCOL");

      l.SetTextAlign(12);
      l.SetTextSize(0.05);
      l.DrawLatex(-0.6*thePadsGeometry.GetXLength(),1.08*thePadsGeometry.GetZLength(),"Color code: induced charge");

      c3->cd(2);
      hdriftTimeInPads->Draw("ZCOL");

      TLatex l2;
      l2.SetTextAlign(12);
      l2.SetTextSize(0.05);
      l2.DrawLatex(-0.5*thePadsGeometry.GetXLength(),1.08*thePadsGeometry.GetZLength(),"Color code: drift time");

      c3->cd(3);
      hdepth1InPads->Draw("ZCOL");
      c3->cd(4);
      hdepth2InPads->Draw("ZCOL");
    }
    else{
      TCanvas* c3 = new TCanvas("c3","Pads in pad (XZ) plane; box geometry",0,0,600,600);
      TLatex l;
      l.SetTextAlign(12);
      l.SetTextSize(0.05);
      //l.DrawLatex(0.1,0.1,"Pads in pad (XZ) plane; box geometry");

      c3->SetFillColor(0);
      c3->Divide(2,2);
      c3->Draw();
      TVirtualPad * c3_1 = c3->cd(1);
      c3_1->SetLogz();
      htrackInPads->Draw("ZCOL");

      l.SetTextAlign(12);
      l.SetTextSize(0.05);
      l.DrawLatex(-0.6*thePadsGeometry.GetXLength(),1.08*thePadsGeometry.GetZLength(),"Color code: induced charge");

      TVirtualPad * c3_2 = c3->cd(2);
      c3_2->SetLogz();
      hdriftTimeInPads->Draw("ZCOL");

      TLatex l2;
      l2.SetTextAlign(12);
      l2.SetTextSize(0.05);
      l2.DrawLatex(-0.5*thePadsGeometry.GetXLength(),1.08*thePadsGeometry.GetZLength(),"Color code: drift time");

      TVirtualPad * c3_3 = c3->cd(3);
      c3_3->SetLogz();
      hdepth1InPads->Draw("ZCOL");

      l.SetTextAlign(12);
      l.SetTextSize(0.05);
      l.DrawLatex(800,1.08*thePadsGeometry.GetZLength(),"Color code: induced charge");

      TVirtualPad * c3_4 = c3->cd(4);
      c3_4->SetLogz();
      hdepth2InPads->Draw("ZCOL");

      l.SetTextAlign(12);
      l.SetTextSize(0.05);
      l.DrawLatex(800,1.08*thePadsGeometry.GetZLength(),"Color code: induced charge");
    }
  }
  else if(thePadsGeometry.GetGeoType() == 1){
    TCanvas* c3 = new TCanvas("c3","Pads in pad (phi Z) plane; cylinder geometry",0,0,600,600);
    TLatex l;
    l.SetTextAlign(12);
    l.SetTextSize(0.05);
    //l.DrawLatex(0.1,0.1,"Pads in pad (phi Z) plane; cylinder geometry");

    c3->SetFillColor(0);
    c3->Divide(2,2);
    c3->Draw();
    c3->cd(1);
    htrackInPads->Draw("ZCOL");

    l.SetTextAlign(12);
    l.SetTextSize(0.05);
    l.DrawLatex(-2,1.08*thePadsGeometry.GetZLength(),"Color code: induced charge");

    c3->cd(2);
    hdriftTimeInPads->Draw("ZCOL");

    TLatex l2;
    l2.SetTextAlign(12);
    l2.SetTextSize(0.05);
    l2.DrawLatex(-2,1.08*thePadsGeometry.GetZLength(),"Color code: drift time");

    c3->cd(3);
    hdepth1InPads->Draw("ZCOL");
    c3->cd(4);
    hdepth2InPads->Draw("ZCOL");
  }

  gPad->Modified();
  gPad->Update();

  cout <<" Event number "<< event <<" shown." <<endl;
  event_g++;
  shown=1;
}
