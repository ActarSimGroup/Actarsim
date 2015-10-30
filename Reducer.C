///////////////////////////////////////////////////////////////////
//*-- AUTHOR : Piotr Konczykowski
//*-- Date: 10/2015
//*-- Last Update: 29/10/15
//*-- Copyright: GENP (Univ. Santiago de Compostela)
//
// --------------------------------------------------------------
//
// --------------------------------------------------------------
// Merge the informations on the pads (digfile) and silicon (simfile)
// into a root file with the same structure as the Actar data
// to be used in a common analysis program (Analysis_reduce.C)
// Save only the events with a silicon hit
// --------------------------------------------------------------
// How to run this program:
// 1 - Run the simulation and the digitization
// 2 - Open a root session
//      root -l
// 3 - Run this macro inside root
//      .x Reducer.C
// Check the configuration area (line 46) to set the macro parameters

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TBrowser.h"
#include "TH2.h"
#include "TRandom.h"
#include <TCanvas.h>
#include <iostream>

#include "TVector3.h"
#include "MTrack.h"

using namespace std;

#if defined(__MAKECINT__)
#pragma link C++ class MTrack;
#endif

void Reducer() {
  gSystem->Load("libactar.sl");
  gROOT->ProcessLine(".L digit.h+");

  Bool_t simuFlag, gasflag;
  Char_t *simname, *digname,  *gasname, *outputname;

  //CONFIGURATION AREA
  simname="./root_files/simFile_40MeV_He3He3_He4He4_iongasmod.root";
  digname="./dig_files/digFile_40MeV_He3He3_He4He4_iongasmod.root";
  gasname="./gases/isobutane.dat";
  outputname="./output/Output_40MeV_He3He3_He4He4_iongasmod.root";
  padsGeometry thePadsGeometry;
  thePadsGeometry.SetGeometryValues(37.,85.,69.,2.,5.,5.);
  driftManager theDriftManager;
  Int_t minRow=0, maxRow=31;
  Int_t minColumn=0, maxColumn=63;
  Double_t threshold = 0;
  Double_t timeThreshold = 1.;
  //END OF CONFIGURATION AREA

  //Event info;
  TFile *simFile=new TFile(simname);

  TTree *simTree=(TTree*)simFile->Get("The_ACTAR_Event_Tree");
  Int_t nentries=simTree->GetEntries();
  cout << "Number of sim event : " << nentries << endl;

  //ClonesArray to the silicon Hits
  TClonesArray *silHitsCA=new TClonesArray("ActarSimSilHit",200);
  TBranch *branchSilHits=simTree->GetBranch("silHits");
  branchSilHits->SetAddress(&silHitsCA);
  branchSilHits->SetAutoDelete(kTRUE);
  ActarSimSilHit *silHit=new ActarSimSilHit;

  Char_t dummy[256];
  Char_t gas[256];
  Double_t v_drift,sigma_trans,sigma_long;
  ifstream *gasfile=new ifstream(gasname);
  gasfile->getline(gas,256);
  cout << "Reading from the gas parameters file: " << gasname << endl;
  cout << gas << endl;
  gasfile->getline(dummy,256);

  cout << dummy <<endl;
  *gasfile >> dummy >> dummy >> v_drift;
  *gasfile >> dummy >> dummy >> sigma_trans;
  *gasfile >> dummy >> dummy >> sigma_long;

  theDriftManager.SetDriftVelocity(v_drift);
  theDriftManager.SetDiffusionParameters(sigma_long,sigma_trans);

  cout << "Drift Parameters are:" << endl;
  cout << "v_drift:  " << theDriftManager.GetDriftVelocity() << "mm/ns" << endl;
  cout << "D_long:  " << theDriftManager.GetLongitudinalDiffusion() << "mm^2/ns" << endl;
  cout << "D_trans:  " << theDriftManager.GetTransversalDiffusion() << "mm^2/ns" << endl;

  const Int_t numberOfRows   = thePadsGeometry.GetNumberOfRows();
  const Int_t numberOfColumns= thePadsGeometry.GetNumberOfColumns();
  cout << "Pad Plane parameters are:" << endl;
  cout << "X length: " << thePadsGeometry.GetXLength() << endl;
  cout << "Y length: " << thePadsGeometry.GetYLength() << endl;
  cout << "Z length: " << thePadsGeometry.GetZLength() << endl;
  cout << "Number of Rows: " << numberOfRows<< endl;
  cout << "Number of Columns: " << numberOfColumns << endl << endl;

  TFile *digFile=new TFile(digname);
  cout << "Opening digitization file: " << digname <<endl;

  TTree *digiTree=(TTree*)digFile->Get("digiTree");
  Int_t dentries=digiTree->GetEntries();
  cout << "Number of digit event: " << dentries << endl;

  //ClonesArray to the signal
  TClonesArray *padSignalCA=new TClonesArray("ActarPadSignal",4000);
  digiTree->SetBranchAddress("padSignals",&padSignalCA);
  ActarPadSignal *padSignal=new ActarPadSignal;

  Double_t PI=3.1415926535897932384626433;
  Double_t deg=180./PI;
  Double_t rad=PI/180.;

  //Output file and Tree
  TFile *outfile = new TFile(outputname,"RECREATE");
  TTree *out_tree = new TTree("out_tree","out_tree");

  Double_t Energy_in_silicon=0.;
  Double_t EnergySil;
  Int_t detectorID;
  Double_t SilPosX, SilPosY, SilPosZ;
  Double_t Qtot;
  TSpline3 *bragX, *bragY, *bragZ;
  Double_t charge, Ccharge;
  Double_t Rm,Cm,Zcor,Zm;

  //Matrix for the charge map
  Double_t **padCharge=new Double_t*[numberOfRows];
  Double_t **padTime=new Double_t*[numberOfRows];
  Double_t **padHeight=new Double_t*[numberOfRows];
  for(Int_t j=0;j<numberOfRows;j++){
    padCharge[j]=new Double_t[numberOfColumns];
    padTime[j]=new Double_t[numberOfColumns];
  }

  TMatrixD padChargeTest;
  TMatrixD padTimeTest;

  TMatrixD matrixSize(32,64);
  padChargeTest.ResizeTo(matrixSize);
  padTimeTest.ResizeTo(matrixSize);

  TMatrixD *pointerCharge = &padChargeTest;
  TMatrixD *pointerTime = &padTimeTest;

  TVectorD SilCharge;
  SilCharge.ResizeTo(16);
  TVectorD SilID;
  SilID.ResizeTo(16);

  TVectorD *pointerSilCharge = &SilCharge;
  TVectorD *pointerSilID = &SilID;

  //Branching the out tree
  out_tree->Branch("PadCharge","TMatrixD",&pointerCharge,64000,0);
  out_tree->Branch("PadTime","TMatrixD",&pointerTime,64000,0);
  out_tree->Branch("SilCharge","TVectorD",&pointerSilCharge,64000,0);
  out_tree->Branch("SilID","TVectorD",&pointerSilID,64000,0);

  //     Event Loop
  for (Long64_t jentry=0;jentry<nentries;jentry++) {
    if(jentry%500==0) cout << "Event " << jentry << endl;

    if(jentry%2==0){ //for even events -> reaction products?
      for(Int_t u=0;u<numberOfRows;u++){
        for(Int_t k=0;k<numberOfColumns;k++){
          padChargeTest(u,k)=0.;
	        padTimeTest(u,k)=0.;
        }
      }
    }

    silHitsCA->Clear();
    simTree->GetEvent(jentry);

    Int_t nbsilicon=silHitsCA->GetEntries();
    padSignalCA->Clear();
    digiTree->GetEvent(jentry);

    Int_t numberofpads=padSignalCA->GetEntries();

    if(numberofpads>0){
      for(Int_t k=0;k<numberofpads;k++){
        padSignal=(ActarPadSignal*)padSignalCA->At(k);
        Double_t thisCharge = padSignal->GetChargeDeposited();
	      Double_t thisTime   = (padSignal->GetInitTime()+padSignal->GetFinalTime())/2.;
	      Double_t thisSigmaTime=padSignal->GetSigmaTime();
	      Int_t PadRow=padSignal->GetPadRow();
	      Int_t PadColumn=padSignal->GetPadColumn();

        if (thisCharge!=0){
          padChargeTest(padSignal->GetPadRow()-1,padSignal->GetPadColumn()-1)+= thisCharge;
	        padTimeTest(padSignal->GetPadRow()-1,padSignal->GetPadColumn()-1)+= thisCharge*thisTime;
        }
      }//Loop on ActarPadSignals

      if(jentry%2==1) {//for odd events -> beam??
	      // Loop on rows & columns
	      for(Int_t c=0;c<numberOfColumns;c++){
          for(Int_t r=0;r<numberOfRows;r++){
            padTimeTest(r,c)=padTimeTest(r,c)/padChargeTest(r,c);
          }
        }//End of Loop on rows & columns
      }
    }//End of if(numberofpads>0) loop
    if(jentry%2!=0){ //for even events -> reaction products?
      //Getting the Silicon Hits
      for(Int_t s=0;s<16;s++){//Loop on the 12 MAYA Si & 4 DSSD
        SilCharge(s)=0.;
        SilID(s)=0.;
      }

      Int_t SilHitID;
      Energy_in_silicon=0.;
      Int_t nbsiliconhits=silHitsCA->GetEntries();

      if(nbsiliconhits>2)
        cout << "SilHits-> " << nbsiliconhits << " Event " << jentry << endl;
      if(nbsiliconhits>0){
        for(Int_t h=0;h<nbsiliconhits;h++){
          SilHitID=0;
          silHit=(ActarSimSilHit*)silHitsCA->At(h);
	        EnergySil=silHit->GetEnergy();
	        Energy_in_silicon=EnergySil;
	        detectorID=silHit->GetDetectorID();
	        SilPosX=silHit->GetXPos();
	        SilPosY=silHit->GetYPos();
	        SilPosZ=silHit->GetXPos();
	        SilCharge(detectorID-1)=Energy_in_silicon;
	        if(silHit->GetParticleID()==2212) SilID(detectorID-1)=1; //p
	        else if(silHit->GetParticleID()==1000020030)SilID(detectorID-1)=2; //3He
	        else if(silHit->GetParticleID()==1000020040)SilID(detectorID-1)=3; //4He
	        else if(silHit->GetParticleID()==1000060120)SilID(detectorID-1)=4; //12C
	        else if(silHit->GetParticleID()==1000060130)SilID(detectorID-1)=5; //13C
        }
      }
      if(nbsiliconhits!=0)out_tree->Fill();
    }
  }
  out_tree->Write();
  outfile->Close();
}
