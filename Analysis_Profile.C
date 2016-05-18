///////////////////////////////////////////////////////////////////
//*-- AUTHOR : Piotr Konczykowski
//*-- Date: 10/2015
//*-- Last Update: 16/05/16
//*-- Copyright: GENP (Univ. Santiago de Compostela)
//
// This macro use the information from the pads (digitization files)
// to make the energy deposit profile along the tracks (Bragg Peak)
// and the transverse diffusion profile. The method used to make the
// profiles is WeightedPadProjection where the pads are cut into 
// slices and the energy deposit aken is proportional to the slice 
// surface
//
// --------------------------------------------------------------
// How to run this program:
// 1 - Run the simulation
// 2 - Run the digitization
// 3 - Run this macro inside root
//      .L Analysis_Profile.C;
//      Run()
// --------------------------------------------------------------
//
// UPDATES :
// - The profile bining can be either fixed or scaled to the pad
// size using the command PadBin 
//
//
/////////////////////////////////////////////////////////////////

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TBrowser.h"
#include "TH2.h"
#include "TRandom.h"
#include <TCanvas.h>
#include <iostream>

#include "TSpline.h"
#include "TVector3.h"
#include "MTrack.h"

using namespace std;

#if defined(__MAKECINT__)
#pragma link C++ class MTrack;
#endif

TSpline3* simbragRGlobal;
Double_t multiGlobal,RShiftGlobal;
const Int_t NBinsGlobal=150;
//float ProjCPUTime=0;

void Run()
{

  ///FLAGS///

  //Event by event
  Bool_t Track3DVisuFlag=0;
  Bool_t TrackVisuFlag=1;
  Bool_t TrackSourcePosFlag=0;
  Bool_t TrackAngleFlag=0;
  Bool_t TrackBragFlag=1;
  Bool_t TrackRangeFlag=0;
  Bool_t MultiThetaFlag=0;

  Bool_t TimerFlag=1;
  Bool_t AlphaBraggFitFlag=0;
  Bool_t TrackAnalysis=1;
  Bool_t CumulTracks=1;

  Bool_t EnergyFlag=0;
  Bool_t RangeFlag=0;
  Bool_t ChiFlag=0;

  gSystem->Load("libactar.sl");

  Bool_t simuFlag;
  Bool_t gasflag;
  Char_t *simname;
  Char_t *digname;
  Char_t *gasname;
      
  if(TimerFlag){
    TStopwatch timer;
    printf("Starting timer\n");
    timer.Start();
  }

  Double_t sigma_time=3;//ns

  Int_t beamOn=1;//1: no beam, 2:with beam

  Int_t Run=2;//(0 : 204 mbar, 1 : 144.6 mbar, 2 : 100 mbar)

  Double_t StopPoint=2.;//Stopping point will be MaxValue/StopPoint

  //cout << "Reading Simulation (1) or real datas (0)? ";
  //cin >> simuFlag;
  simuFlag=1;

  Int_t NSlices=10;
  Int_t NProf=200;
  Int_t NBPeaks=0;
  Double_t LastPointRef=230;

  Double_t LongChisq[1500];
  Double_t LongChisqRange[1500];
  Int_t NChisq=0;
  const Double_t BraggNBin=NBinsGlobal;
  //const Double_t BraggNBin=300.;
  const Double_t BraggBinSize=300./BraggNBin;
  //const Double_t BraggBinSize=300/(Double_t)NBinsGlobal;
  //const Double_t BraggBinSize=2.;
  //cout<<"BraggNBin "<<BraggNBin<<" BraggBinSize "<<BraggBinSize<<endl;

  for(Int_t i=0;i<1500;i++){
    LongChisq[i]=LongChisqRange[i]=0.;
  }

  //cout<<"digFile to use: ";
  //cin >> digname;

  //digname="./root_files/dig_files/digFile.root";
  //digname="./root_files/dig_files/digFile_alpha7MeV.root";
  //digname="./root_files/dig_files/digFile_alpha7MeV_y0.3down.root";
  //digname="./root_files/dig_files/digFile_alpha7MeV_x0.1y0.3.root";
  //digname="./root_files/dig_files/digFile_alpha7MeV_iC4H10_204mb.root";
  //digname="./root_files/dig_files/digFile_alpha7MeV_iC4H10_144mb.root";

  //digname="root_files/dig_files/digFile_alpha5MeV_iC4H10_105mbar_z1_em3.root";
  //digname="root_files/dig_files/digFile_alpha7MeV_iC4H10_105mbar_z1_em3.root";
  //digname="root_files/dig_files/digFile_12C_30MeV_iC4H10_105mbar_z1.root";

  if(Run==0)digname="./root_files/dig_files/digFile_alpha7MeV_iC4H10_204mb_em3.root";
  //if(Run==1)digname="./root_files/dig_files/digFile_alpha7MeV_iC4H10_144mb_em3.root";
  if(Run==1)digname="./root_files/dig_files/digFile_alpha7MeV_iC4H10_144mbar_z1_em3.root";
  //if(Run==2)digname="./root_files/dig_files/digFile.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5_5MeV_iC4H10_100mbar_VA-15_z100_D21.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5_5MeV_iC4H10_100mbar_VA15_z100_D21.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5_5MeV_iC4H10_100mbar_VA0_z100_D21.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5_5MeV_iC4H10_100mbar_VA-20to20_z100.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5_5MeV_iC4H10_105mbar_VA-20to20_z100.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5_5MeV_iC4H10_105mbar_VA-15_z100.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5_5MeV_iC4H10_105mbar_VA15_z100.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5_5MeV_iC4H10_105mbar_VA0_z100.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5_5MeV_iC4H10_105mbar_VA0_z100_D21.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5_5MeV_iC4H10_105mbar_VA0_z100_D20.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5_5MeV_iC4H10_105mbar_VA-15_z100_D20.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5_5MeV_iC4H10_105mbar_VA15_z100_D20.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5_5MeV_iC4H10_105mbar_VA-20to20_z100.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5_5MeV_iC4H10_105mbar_VA-20to20_z100_D20.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5_5MeV_iC4H10_105mbar_VA0_z100_D19.root";

  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA0_z100.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA0_z100_D20.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA-15_z100_D20.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA-15_z100_D21.root";

  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_100mbar_VA-20to20_z100.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA0_z100.root";

  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA0_z100_D20.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA0_z100_D18_75.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA0_z100_D18.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA0_z100_D16.root";

  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_7MeV_iC4H10_105mbar_VA-10to10_z100.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA-10to10_z100.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA0_z100_D19.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA-10_z100_D19.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA-10to10_z100_D19.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA-20to20_z100_D19.root";

  //if(Run==2)digname="./root_files/dig_files/digFile_run974.root";



  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA-4_z100_Radius2mm.root";

  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA0_z100_Radius2mm.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA0_z100_Radius2mm_2.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA-10_z100_Radius2mm.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA10_z100_Radius2mm.root";

  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA0_z100_Radius2mm_Dt16.root";

  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA10_z100_Radius2mm_Dt17.root";

  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA0_z100_Radius2mm_Dt19.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA10_z100_Radius2mm_Dt19.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA-10_z100_Radius2mm_Dt19.root";

  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA0_z100_Radius2mm_Dt19.5.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA10_z100_Radius2mm_Dt19.5.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA-10_z100_Radius2mm_Dt19.5.root";

  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA10_z100_Radius2mm_Dt20.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA0_z100_Radius2mm_Dt20.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA10_z100_Radius2mm_Dt20.root";

  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA-10to10_z100_D20.62.root";

  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA0_z100_Radius2mm_Dt20.7.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA10_z100_Radius2mm_Dt20.7.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA-10_z100_Radius2mm_Dt20.7.root";

  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA10_z100_Radius2mm_Dt21.root";

  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA10_z100_Radius2mm_Dt21.5.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA-10_z100_Radius2mm_Dt21.5.root";

  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA0_z100_Radius2mm_Dt22.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA10_z100_Radius2mm_Dt22.root";

  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA0_z100_Radius2mm_Dt23.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA10_z100_Radius2mm_Dt23.root";



  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5_8MeV_iC4H10_105mbar_VA0_z100.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5.8MeV_VA-10+-1.root";

  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5.8MeV_VA0+-1_Dt20.7.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5.8MeV_VA-10+-1_Dt20.7.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5.8MeV_VA10+-1_Dt20.7.root";

  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5.8MeV_VA0+-1_Dt20.62.root";

  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5.8MeV_VA2+-1_Dt20.62.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5.8MeV_VA4+-1_Dt20.62.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5.8MeV_VA6+-1_Dt20.62.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5.8MeV_VA8+-1_Dt20.62.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5.8MeV_VA10+-1_Dt20.62.root";

  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5.8MeV_VA-2+-1_Dt20.62.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5.8MeV_VA-4+-1_Dt20.62.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5.8MeV_VA-6+-1_Dt20.62.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5.8MeV_VA-8+-1_Dt20.62.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5.8MeV_VA-10+-1_Dt20.62.root";

  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5.8MeV_VA0+-1_Dt20.62_em4.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5.8MeV_VA0+-1_Dt20.62_ionGasModel.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5.8MeV_VA0+-1_Dt20.62_W23.root";

  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5.8MeV_VA-10+-1_Dt19.root";

  //if(Run==2)digname="./root_files/dig_files/digFile_alpha_5.8MeV_100mbar_VA0+-1.root";


  if(Run==2)digname="./root_files/dig_files/digFile_gasBox105x85x105_alpha_5.8MeV_VA0_piotrdigit.root";
  //if(Run==2)digname="./root_files/dig_files/digFile_gasBox105x85x105_alpha_5.8MeV_VA0_centerdigit.root";


  gROOT->ProcessLine(".L digit.h+");
  //gROOT->ProcessLine(".L digit_hapol.h+");
 
  Double_t Wvalue=30;

  if(Run==0)Double_t driftVelocity=19.94E-3;//p=204mbar
  if(Run==1)Double_t driftVelocity=28.63E-3;//p=144mbar
  if(Run==2)Double_t driftVelocity=36.43E-3;//p=105mbar
  //if(Run==2)Double_t driftVelocity=37.97E-3;//p=100mbar
  //if(Run==2)Double_t driftVelocity=36.8E-3;//p=105mbar

  //Double_t driftVelocity=6.865e-3;
  //Double_t driftVelocity=42.26e-3;
  //Double_t driftVelocity=31.03e-3;
  //Double_t driftVelocity=37.97e-3;//p=100mbar
  //Double_t driftVelocity=36.8e-3;//p=105mbar
  //Double_t driftVelocity=19.94e-3;//p=204mbar
  //Double_t driftVelocity=28.63e-3;//p=144mbar

  TFile *digFile=new TFile(digname);
  cout<<"Opening digitization file: "<<digname<<endl;

  TTree *digiTree=(TTree*)digFile->Get("digiTree");
  Int_t dentries=digiTree->GetEntries();
  if(CumulTracks)dentries=NProf;
  cout<<"Number of digit event : "<<dentries<<endl;
  //Int_t digentries=digiTree->GetEntries();
  //cout<<"Number of digit event : "<<digentries<<endl;
 
  //ClonesArray to the signal
  TClonesArray *padSignalCA=new TClonesArray("ActarPadSignal",4000);
  digiTree->SetBranchAddress("padSignals",&padSignalCA);
  ActarPadSignal *padSignal=new ActarPadSignal;

  //const Int_t numberOfRows=32;
  //const Int_t numberOfRows=64;
  const Int_t numberOfRows=100;

  //const Int_t numberOfColumns=64;
  const Int_t numberOfColumns=100;
  //const Int_t numberOfColumns=128;
  //const Int_t numberOfColumns=133;

  cout<<"Number of Rows "<<numberOfRows<<" Number of Columns "<<numberOfColumns<<endl;

  Double_t PI=3.1415926535897932384626433;
  Double_t deg=180./PI;
  Double_t rad=PI/180.;
  //==================================================================================//

  //Canvas
  if(Track3DVisuFlag){	
    Can_3D=new TCanvas("Can_3D","Can_3D",900,900);
  }
  
  if(TrackVisuFlag){
    //Can_sil=new TCanvas("Can_sil","Can_sil",900,900);

    Can_track=new TCanvas("Can_track","Can_track",900,900);
    //Can_track->Divide(1,2);
    //Can_track->cd(2)->Divide(2,1);
  }

  if(TrackSourcePosFlag){
    Can_source=new TCanvas("Can_source","Can_source",900,900);
    Can_source->Divide(2,2);
  }

  if(TrackAngleFlag){
    Can_angle=new TCanvas("Can_angle","Can_angle",900,900);
    Can_angle->Divide(2,1);
  }

  if(TrackBragFlag){
    //Can_brag=new TCanvas("Can_brag","Can_brag",1800,900);
    //Can_brag0=new TCanvas("Can_brag0","Can_brag0",900,600);
    Can_brag=new TCanvas("Can_brag","Can_brag",900,600);
    Can_brag->Divide(1,2);
  }

  if(TrackRangeFlag){
    Can_range=new TCanvas("Can_range","Can_range",1800,900);
    Can_range->Divide(1,2);
  }

  if(MultiThetaFlag){
    TCanvas *Can_multi;
    Can_multi=new TCanvas("Can_multi","Can_multi",900,900);
    Can_multi->Divide(5,2);
  }

  ///////////////////////////////////////////////////////
  ////////// READING ALPHA DEDX ACTARSIM FILE ///////////
  ///////////////////////////////////////////////////////


  if(Run==0)const Int_t ActarBin=111;
  if(Run==1)const Int_t ActarBin=153;
  //if(Run==2)const Int_t ActarBin=213;
  if(Run==2)const Int_t ActarBin=250;

  if(Run==2)const Int_t ActarLatBin=150;

  TGraph *hActarSimAlphadEvsR_FP=new TGraph(ActarBin);
  TGraph *hActarSimAlphadEvsR2=new TGraph(ActarBin);
  TGraph *gActarSimAlphadEvsR=new TGraph(ActarBin);
  TGraph *gActarSimAlphaLatR=new TGraph(ActarLatBin);

  TH1F *hActarSimAlphaLatR = new TH1F("hActarSimAlphaLatR","ActarSim Track Lateral Range",200, 0.,400.);
  //TGraph *gActarSimAlphaLatR=new TGraph(ActarLatBin);

  TH1F *hActarSimdEvsR = new TH1F("hActarSimdEvsR","ActarSim Track Bragg",400, 0.,400.);
  TH2F *hActarSimAlphadEvsR = new TH2F("hActarSimAlphadEvsR","Alpha dE/dx vs Range in HeiC4H10 (ActarSim)",5000,0.,150.,5000,0.,150.);
  Double_t alphaE,dE_elec,dE_nucl,Range,LongStrag,LatStrag;
  Double_t LatRange;
  string unit_E,unit_R,LonSunit,LatSunit;
  Double_t Conv,ConvR;
  Double_t ActarPreEndPointdE=0.;
  Double_t ActarPreEndPoint=0.;
  Double_t ActarEndPoint=0.;
  //Double_t ActarSimRange=0.;
  Double_t ActarSimdEMax=0.;
  Double_t ActarSimdEMaxPos=0.;
  Double_t ActarSimdEMaxPosBin=0.;
  Double_t ActarSimdE[ActarBin],ActarSimrange[ActarBin];
  Int_t ActarEndBin=0;

  string ActarSimline;
  //ifstream myfile ("alphadEvsR_ActarSim.txt");//L111

  Char_t *simname;
  Char_t *simlatname;

  if(Run==0)simname="/home/piotr/workplace/analysis_macros/Profiles/alphaLongProfile/alphadEvsR_7MeV_iC4H10_204mbar_ActarSim_VAngle0_HAngle0_em3.txt";//L113
  if(Run==1)simname="/home/piotr/workplace/analysis_macros/Profiles/alphaLongProfile/alphadEvsR_7MeV_iC4H10_144mbar_ActarSim_VAngle0_HAngle0_em3.txt";//L153
  //if(Run==2)simname="/home/piotr/workplace/analysis_macros/Profiles/alphaLongProfile/alphadEvsR_7MeV_iC4H10_105mbar_ActarSim_VAngle0_HAngle0_em3.txt";//L213
  //if(Run==2)simname="/home/piotr/workplace/analysis_macros/Profiles/alphaLongProfile/alphadEvsR_7MeV_iC4H10_100mbar_ActarSim_fixendadded.txt";//L233
  //if(Run==2)simname="/home/piotr/workplace/analysis_macros/Profiles/alphaLongProfile/alphadEvsR_7MeV_iC4H10_100mbar_ActarSim_fixendadded2.txt";//L233
  //if(Run==2)simname="/home/piotr/workplace/analysis_macros/Profiles/alphaLongProfile/alphadEvsR_5_5MeV_iC4H10_ActarSim_VA0_thr3_8_z100_D21_hapol.txt";//L250
  //if(Run==2)simname="/home/piotr/workplace/analysis_macros/Profiles/alphaLongProfile/alphadEvsR_3alpha_iC4H10_105mbar_ActarSim_VA-10to10_thr3_4_z100_D19_good.txt";//L250
  if(Run==2)simname="/home/piotr/workplace/analysis_macros/Profiles/NewProjAlphaLongProfile/alphadEvsR_3alpha_iC4H10_105mbar_ActarSim_VA0_thr3_4_z100_D19.txt";//L250

  ifstream myfile (simname);

  cout<<"Opening simulation file: "<<simname<<endl;

  if (myfile.is_open())
    {
      for(int k=0;k<ActarBin;k++)
	{
	  getline (myfile,ActarSimline);

	  stringstream ss(ActarSimline);
	  ss >> Range >> dE_elec;

	  if(dE_elec>ActarSimdEMax){ActarSimdEMax=dE_elec; ActarSimdEMaxPos=Range; ActarSimdEMaxPosBin=k;}

	  ActarSimdE[k]=dE_elec;
	  ActarSimrange[k]=Range;

	  hActarSimdEvsR->Fill(Range,dE_elec);
	  gActarSimAlphadEvsR->SetPoint(k,Range,dE_elec);

	  if(k>100 && dE_elec>1e-8)ActarEndBin=k;
	}
      myfile.close();
    }
  else cout << "Unable to open file"; 

  ActarPreEndPointdE=ActarSimdE[ActarEndBin-2];
  ActarPreEndPoint=ActarSimrange[ActarEndBin-2];
  ActarEndPoint=ActarSimrange[ActarEndBin-1];

  simbragRGlobal=new TSpline3(hActarSimdEvsR);

  ////////// READING LATERAL RANGE /////////////

  //if(Run==2)simlatname="/home/piotr/workplace/analysis_macros/Profiles/alphaLatRvsR_7MeV_iC4H10_100mbar_ActarSim.txt";//L235
  //if(Run==2)simlatname="/home/piotr/workplace/analysis_macros/Profiles/alphaLatProfile/alphaLatRvsR_5_5MeV_iC4H10_ActarSim_VA0_thr3_8_z100_D21_hapol.txt";//L250
  //if(Run==2)simlatname="/home/piotr/workplace/analysis_macros/Profiles/alphaLatProfile/alphaLatRvsR_3alpha_iC4H10_105mbar_ActarSim_VA0_thr3_4_z100_D19_good.txt";//L250
  //if(Run==2)simlatname="/home/piotr/workplace/analysis_macros/Profiles/NewProjAlphaLatProfile/alphaLatRvsR_3alpha_iC4H10_105mbar_ActarSim_VA0_thr3_4_z100_D19.txt";//L250
  if(Run==2)simlatname="/home/piotr/workplace/analysis_macros/Profiles/LatProf/alphaLatRvsR_run974_VA-10_alpha_5.8MeV_PadBin_NoTeeth.txt";//L150

  ifstream mylatfile (simlatname);

  cout<<"Opening simulation lateral file: "<<simlatname<<endl;


  if (mylatfile.is_open())
    {
      for(int k=0;k<ActarLatBin;k++)
	{
	  getline (mylatfile,ActarSimline);

	  stringstream ss(ActarSimline);
	  ss >> Range >> LatRange;

	  //ActarSimLatR[k]=LatRange;
	  //ActarSimrange[k]=Range;

	  hActarSimAlphaLatR->Fill(Range+33,LatRange);
	  //hActarSimAlphaLatR->SetPoint(k,Range,LatRange);
	}
      mylatfile.close();
    }
  else cout << "Unable to open file"; 

  //cout<<"ActarSim dEMax "<<ActarSimdEMax<<" dEMaxPos "<<ActarSimdEMaxPos<<endl;

  //////////////////////////////////////////////////////////////
  ////////// END OF READING ALPHA DEDX ACTARSIM FILE ///////////
  //////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////

  MTrack* Track=new MTrack();

  Double_t Qtot;

  TSpline3 *bragR;
  TSpline3 *bragR1;
  TSpline3 *bragR2;

  Double_t maxZ=170.;
  if(simuFlag==0)maxZ=600.;

  //Double_t threshold = 1000;
  //Double_t thresholdeV = threshold*30/1e9;

  Double_t threshold = 0;
  //Double_t threshold = 0.008;
  //Double_t threshold = 0.0045;
  //Double_t threshold = 0.005;
  //Double_t threshold = 0.0034;
  //Double_t thresholdADC = threshold*1e9/30;
  Double_t thresholdADC = threshold*1e9/Wvalue;

  cout<<"threshold "<<threshold<<endl;

  Double_t Tthreshold = 1.;
  Double_t outThresh=10000.;
  //if(simuFlag==0)threshold = 200.; outThresh=1000.;
  if(simuFlag==0)threshold = 0.; outThresh=1000.;

  Bool_t PadBin=1;

  Bool_t RowTeeth,RowHoles;
  Int_t NTeeth[numberOfRows],NRHoles[numberOfRows],NC0,NCHit;
  Int_t NBlock;
  Bool_t MultiBlock,BCHit[numberOfRows];
  Int_t CandidateTrack;
  Bool_t Candidate;

  Double_t range,range2,rangeX,rangeY;
  Double_t Rm,Cm,Zcor,Zm;
  Bool_t out_track;

  Double_t PadToSilX=51.8;
  Double_t PadToSilY=58.8;
  
  Int_t Rmin=0;
  Int_t Rmax=numberOfRows-1;
  Int_t Cmin=0;
  Int_t Cmax=numberOfColumns-1;

  Double_t a,b;

  Double_t RangeA[2],RangeB[2],RangeO,RangeOS;

  Double_t IniPoint[2],FinPoint[2],IniFitPoint[2],FinFitPoint[2];

  Double_t Start[3],Stop[3];
  Double_t TStart[3],TStop[3];

  Double_t PointA[3][2],PointB[3][2];

  Double_t gdEdx[NBinsGlobal];
  Double_t gLatRange[NBinsGlobal];
  Double_t gRange[NBinsGlobal];

  Double_t phi;
  Double_t theta;
  Double_t VAngle;
  Double_t HAngle;
  Double_t Angle3D;

  Double_t xmaxval,xmax;
  Double_t ymaxval,ymax;
  Double_t zmaxval,zmax;
  Double_t range_calcX;
  Double_t range_calcZ;
  Double_t range_calcY;

  Double_t trackX,trackZ,trackY,trackZ2;
  Double_t trackPadX,trackPadZ,trackPadY;
  Double_t track_range,track_rangeXY,track_rangePadXY;

  //3 alphas source position inside Actar
  Double_t SourceX=64;
  //Double_t SourceY=-43.;
  Double_t SourceY=-37.43;
   
  Int_t binshift;
  Double_t RangeShift;

  //Matrix for the charge map
  Double_t **padCharge=new Double_t*[numberOfRows];
  Double_t **padTime=new Double_t*[numberOfRows];
  Double_t **padHeight=new Double_t*[numberOfRows];
  for(Int_t j=0;j<numberOfRows;j++){
    padCharge[j]=new Double_t[numberOfColumns];
    padTime[j]=new Double_t[numberOfColumns];
    padHeight[j]=new Double_t[numberOfColumns];
  }
   
  TMatrixD *padChargeTest;
  TMatrixD *padTimeTest;

  Double_t PadESig;

  //create histograms
  if(simuFlag==1){
    TH2F *visu_chargeYZ=new TH2F("visu_chargeYZ","visu_chargeYZ",32,0,32,85,0,85);
    TH3F *h3DTrack=new TH3F("h3DTrack","h3DTrack",64,0,64,32,0,32,85,0,85);
  }
  else{
    TH2F *visu_chargeYZ=new TH2F("visu_chargeYZ","visu_chargeYZ",32,0,32,300,0.,300.);
    TH3F *h3DTrack=new TH3F("h3DTrack","h3DTrack",64,0,64,32,0,32,300,0.,300.);
  }

  TH1F *h_DeltaTime=new TH1F("h_DeltaTime","Pad_Tmax - PadTmin",512,0.,512.);
  TH1F *h_DeltaHeight=new TH1F("h_DeltaHeight","#Delta t * Vdrift (mm)",500,0.,500.);

  TH1F *h_energy_track=new TH1F("Energy_track","Total Charge Deposited on Pads from track",500, 3.5, 7.);

  ////Bragg stuff////

  TGraph *gLongBragg=new TGraph(NBinsGlobal);
  TGraph *gLatBragg=new TGraph(NBinsGlobal);

  TH1F *h_range=new TH1F("Range in mm","Range in mm",600,0.,300.);
  TH1F *h_bragg=new TH1F("TrackBragg","Track Bragg",BraggNBin, 0., 300.);
  TH1F *h_braggcumul=new TH1F("TrackBraggCumul","Track Bragg fixed last point and added",BraggNBin, 0., 300.);
  TH1F *h_braggcumuladd=new TH1F("TrackBraggAll","Track Bragg added",BraggNBin, 0., 300.);
  TH1F *h_braggcumulnorm=new TH1F("TrackBraggCumulNorm","Track Bragg fixed last point and added",BraggNBin, 0., 300.);
  TH1F *h_braggres=new TH1F("TrackBraggRes","Track Bragg Fit Residual",BraggNBin, 0., 300.);
  TH1F *h_latspread=new TH1F("TrackLatSpread","Track Lateral Spread",BraggNBin, 0., 300.);
  TH1F *h_latspreadres=new TH1F("TrackLatSpreadRes","Track Lateral Spread Residual",BraggNBin, 0., 300.);
  TH1F *h_latcumul=new TH1F("TrackLatCumul","Track Lateral Spread fixed last point and added",BraggNBin, 0., 300.);
  TH1F *h_latcumulnorm=new TH1F("TrackLatCumulNorm","Track Lateral Spread fixed last point and added",BraggNBin, 0., 300.);

  TH1F *h_braggcumulbin[BraggNBin];
  TH1F *h_latbraggcumulbin[BraggNBin];

  //TH2F *hchi2vsvangle=new TH2F("hchi2vsvangle","Longitudinal #chi^{2} vs Vertical Angle",400,-30.,30.,200,0.,0.0001);
  TH2F *hchi2vsvangle=new TH2F("hchi2vsvangle","Longitudinal #chi^{2} vs Vertical Angle",400,-30.,30.,500,0.,0.03);
  TH2F *hlatchi2vsvangle=new TH2F("hlatchi2vsvangle","Transverse #chi^{2} vs Vertical Angle",400,-30.,30.,200,0.,1);

  for(int l=0;l<BraggNBin;l++)
    {
      char title[256];
      sprintf(title,"Longitudinal Range bin %d",l);
      h_braggcumulbin[l]=new TH1F(title,title,100, 0.,10000.);
      char lattitle[256];
      sprintf(lattitle,"Lateral Range bin %d",l);
      h_latbraggcumulbin[l]=new TH1F(lattitle,lattitle,100, 0.,100.);
    }

  TH1F *h_energy=new TH1F("Energy","Total Charge Deposited",1000, 3000., 5000.);
  TH2F* visu_multiplicity=new TH2F("visu_multiplicity","visu_multiplicity",numberOfColumns,0,numberOfColumns,numberOfRows,0,numberOfRows);
  TH2F* visu_totcharge=new TH2F("visu_totcharge","visu_totcharge",numberOfColumns,0,numberOfColumns,numberOfRows,0,numberOfRows);

  //TH2F *visu_charge=new TH2F("visu_charge","visu_charge",128,0,128,64,0,64);
  //TH2F *visu_charge=new TH2F("visu_charge","visu_charge",128,0,128,32,0,32);
  //TH2F *visu_charge=new TH2F("visu_charge","visu_charge",133,0,133,32,0,32);
  TH2F *visu_charge=new TH2F("visu_charge","visu_charge",numberOfColumns,0,numberOfColumns,numberOfRows,0,numberOfRows);

  TH2F *visu_time=new TH2F("visu_time","visu_time",numberOfColumns,0,numberOfColumns,numberOfRows,0,numberOfRows);
  //TH2F *visu_time=new TH2F("visu_time","visu_time",64,0,64,64,0,64);

  TH1F *hHorizAngle=new TH1F("hHorizAngle","Horizontal Angle",720,-180,180);
  TH1F *hVertiAngle=new TH1F("hVertiAngle","Vertical Angle",360,-90,90);
  TH1F *h3DAngle=new TH1F("h3DAngle","3D Angle",720,-180,180);

  TH1F *hRange   = new TH1F("hRange","Alpha range in mm",200,50.,150.);
  TH1F *hRangeRest   = new TH1F("hRangeRest","Alpha range in mm, |vert angle| < 10 deg",200,50.,150.);
  TH2F *hRangeVsHoriz = new TH2F("hRangeVsHoriz","Range vs Horizontal angle",180,-90.,90.,200,50,150);

  //*********************************************************************************************************//
  //*********************************************************************************************************//
  //**                                                                                                     **//
  //**                                        Event Loop                                                   **//
  //**                                                                                                     **//
  //*********************************************************************************************************//
  //*********************************************************************************************************//

  for (Long64_t jentry=0;jentry<dentries;jentry++) {
  //for (Long64_t jentry=0;jentry<1000;jentry++) {
    //for (Long64_t jentry=5000;jentry<nentries;jentry++) {
    if(jentry%10==0 && jentry!=0)cout<<"."<<flush;
    if(jentry%100==0)cout<<jentry<<endl;

    //if(!simuFlag || (simuFlag && jentry%beamOn==0))
    if(jentry%beamOn==0 && simuFlag)
      {
	for(Int_t u=0;u<numberOfRows;u++){
	  NTeeth[u]=NRHoles[u]=BCHit[u]=0;
	  for(Int_t k=0;k<numberOfColumns;k++){
	    padCharge[u][k]=0.;
	    padTime[u][k]=0.;
	  }
	}
      }

    Candidate=0;
    CandidateTrack=0;
    RowTeeth=RowHoles=0;
    MultiBlock=0;
    NC0=NCHit=0;

    Qtot=0;

    visu_charge->Reset();
    visu_chargeYZ->Reset();
    visu_time->Reset();
    h3DTrack->Reset();

    h_latspread->Reset();
    h_bragg->Reset();

    Track->ResetLines();

    //simTree->GetEvent(jentry);

    padSignalCA->Clear();

    digiTree->GetEvent(jentry);

    Int_t numberofpads=padSignalCA->GetEntries();
    //cout<<"# Pads fired "<< numberofpads<<endl;

    if(numberofpads>0){
      for(Int_t k=0;k<numberofpads;k++){
	padSignal=(ActarPadSignal*)padSignalCA->At(k);
	Double_t thisCharge = padSignal->GetChargeDeposited();
	Double_t thisTime   = (padSignal->GetInitTime()+padSignal->GetFinalTime())/2.;
	Double_t thisSigmaTime=padSignal->GetSigmaTime();
	Int_t PadRow=padSignal->GetPadRow();
	Int_t PadColumn=padSignal->GetPadColumn();
        //cout<<"thisSigmaTime: "<<thisSigmaTime<<endl;

	//cout<<"PadRow "<<PadRow<<" PadColumn "<<PadColumn<<" thisCharge: "<<thisCharge<<endl;

	if (thisCharge!=0){

	  //padChargeTest(padSignal->GetPadRow()-1,padSignal->GetPadColumn()-1)+= thisCharge;
	  //padTimeTest(padSignal->GetPadRow()-1,padSignal->GetPadColumn()-1)+= thisCharge*thisTime;

	  padCharge[padSignal->GetPadRow()-1][padSignal->GetPadColumn()-1]
	    += thisCharge;
	  padTime[padSignal->GetPadRow()-1][padSignal->GetPadColumn()-1]
	    += thisCharge*thisTime;

	}
      }//Loop on ActarPadSignals
	 
	   //if(!simuFlag || (simuFlag && jentry%2==1))//To have the 2 parts of the reaction in simu
      if(!simuFlag || (simuFlag && jentry%beamOn==(beamOn-1)))//To have the 2 parts of the reaction in simu
	{
	  // Loop on rows & columns
	  for(Int_t c=0;c<numberOfColumns;c++){ //LOOP on Columns
	    for(Int_t r=0;r<numberOfRows;r++){ //LOOP on Rows
	      if(padCharge[r][c] > thresholdADC){
		//padTimeTest(r,c)=padTimeTest(r,c)/padChargeTest(r,c);
		padTime[r][c]=padTime[r][c]/padCharge[r][c];
		//if(padTime[r][c]>0)cout<<padTime[r][c]<<endl;
		//if(padTime[r][c]>0)cout<<"padTime[r][c] before "<<padTime[r][c];
		padTime[r][c]=gRandom->Gaus(padTime[r][c],sigma_time);
		//padTime[r][c]=gRandom->Gaus(padTime[r][c],thisSigmaTime);
		//if(padTime[r][c]>0)cout<<" padTime[r][c] after "<<padTime[r][c]<<endl;
	      }
	    }
	  }//End of Loop on rows & columns

	  ////////////////////////////////////////////////////
	  ////////////// Loop on columns & rows //////////////
	  ////////////////////////////////////////////////////

	  Double_t maxtime=0;
	  Double_t mintime=0;

	  for(Int_t r=0;r<numberOfRows;r++){ //LOOP on Rows
	    for(Int_t c=0;c<numberOfColumns;c++){ //LOOP on Columns

	      if(padCharge[r][c] > thresholdADC){

		if(simuFlag){
		  /*
		  //Pad energy resolution estimation (to be measured)
		  if(padCharge[r][c]*30/1e6<20)PadESig=0.085;
		  else if(padCharge[r][c]*30/1e6>=20 && padCharge[r][c]*30/1e6<=5000)PadESig=-1.6215*1e-5*(padCharge[r][c]*30/1e6-20)+0.085;
		  else if(padCharge[r][c]*30/1e6>5000)PadESig=0.00425;
		  padCharge[r][c]=gRandom->Gaus(padCharge[r][c],PadESig*padCharge[r][c]);
		  */

		  //cout<<"padChargeTest(r,c): "<<padChargeTest(r,c)<<" padChargeTest(r,c) (keV): "<<padChargeTest(r,c)*30/1e6<<" sigma (%): "<<PadESig*100<<" padCharge[r][c] (keV): "<<padCharge[r][c]*30/1e6<<endl;

		  if(c>=23)
		    {
		      if(c==23)NC0++;
		      if(BCHit[r]==0)NCHit++;
		      BCHit[r]=1;
		      //cout<<"C "<<c<<" R "<<r<<" padCharge[r][c] "<<padCharge[r][c]<<" thresholdADC "<<thresholdADC<<endl;
		      if(c>23 && c<numberOfColumns-1 && padCharge[r][c-1]<=threshold && padCharge[r][c+1]<=thresholdADC){NTeeth[r]++;}// padCharge[r][c]=0;}// cout<<"TEETH : C "<<c<<" R "<<r<<" padCharge[r][c] "<<padCharge[r][c]<<endl; }
		      if(c<numberOfColumns-2 && padCharge[r][c+1]<=thresholdADC && padCharge[r][c+2]>thresholdADC)NRHoles[r]++;
		    }

		  //padCharge[r][c]*=30/1e9;
		  padCharge[r][c]*=Wvalue/1e9;
		  Qtot+=padCharge[r][c];
		  //We left the middle pad band shadowed 
		  //if(r>13 && r<18)padCharge[r][c]=0;
		  /*
		  else{
		    ////////// '0' suppression simulation ////////////
	      
		    //Double_t padADC=padChargeTest(r,c)*30/1e9+5+0.0537;
		    //Double_t padADC=padCharge[r][c]-0.0537;
		    Double_t padADC=padCharge[r][c];
		    padADC/=3.299*1e-5;
		    padADC+=BL[r][c]+gRandom->Gaus(0,BLSIG[r][c]);
		    //padADC+=BL[r][c];

		    //if(padADC<600)cout<<r<<" "<<c<<" padCharge "<<padCharge[r][c]<<" padADC "<<padADC<<endl;

		    //if(padADC<0)padCharge[r][c]=0;//just to try
		    if(padADC<600)padCharge[r][c]=0;
		    //////////END of '0' suppression simulation ////////////
		  }
		  */
		}
		else if(!simuFlag){
		  //Pad charge to energy (MeV) convertion
		  //padCharge[r][c]=padChargeTest(r,c)*3.299*1e-5+0.0537;
		  padCharge[r][c]=padChargeTest(r,c);
		  padTime[r][c]=padTimeTest(r,c);
		  //if(r==13 && (c==0 || c==1 || c==2)){padCharge[r][c]=0; padTime[r][c]=0;} //These are noisy channels that I noticed in the end of 1st part. Check if is still the case in the run to analyse
		}
		//cout<<"TEST Row "<<r<<" Col "<<c<<" padCharge: "<<padCharge[r][c]<<endl;

		padHeight[r][c]=padTime[r][c]*driftVelocity;
		visu_charge->SetBinContent(c+1.,r+1,padCharge[r][c]);
		visu_time->SetBinContent(c+1.,r+1.,padTime[r][c]);
		h3DTrack->SetBinContent(c+1.,r+1.,padTime[r][c]*driftVelocity/2+1,padCharge[r][c]);
	      }
	      else{ 
		padCharge[r][c]=-1;
		padTime[r][c]=-1;
		padHeight[r][c]=-1;
	      }

	    }//End of Loop on Columns
	  }//End of Loop on Rows



	  for(Int_t r=0;r<numberOfRows;r++){ //LOOP on Rows
	    if(NTeeth[r]>=5)RowTeeth=1;
	    if(NRHoles[r]>5)RowHoles=1;

	    NBlock=0;

	    for(Int_t c=23;c<numberOfColumns;c++){ //LOOP on Columns
	      if(padCharge[r][c]>threshold && NBlock==0 && padCharge[r][c+1]<=threshold)NBlock++;
	      else if(padCharge[r][c]>threshold && NBlock==1){MultiBlock=1;}// cout<<"r "<<r<<" c "<<c<<" MultiBlock "<<endl;}
	    }
	  }

	  h_energy_track->Fill(Qtot);

	  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	  //// Fitting and calculating the starting and exit points as well as the energy deposit from a delta L part of the track ////
	  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	  
	  if(!RowTeeth && !RowHoles)Candidate=1;
	  //if(!RowTeeth && !RowHoles && !MultiBlock && NC0==3 && NCHit==3)Candidate=1;
	    

	  //if(TrackAnalysis)
	  if(TrackAnalysis && Candidate)
	    {

	      for(Int_t i=0;i<3;i++){
		Start[i]=Stop[i]=0.;
		TStart[i]=TStop[i]=0.;
	      }
	      Cmin=23;//corresponding to the gap between the alpha source and pad plane
	      FitMat(padCharge,Rmin,Rmax,Cmin,Cmax,threshold,a,b);
	      IniPoint[0]=0; IniPoint[1]=a*IniPoint[0]+b/2; FinPoint[0]=numberOfColumns; FinPoint[1]=FinPoint[0]*a+b/2;

	      Double_t chi2=FitMat3D(padCharge,padHeight,Rmin,Rmax,Cmin,Cmax,threshold,Track);

	      if(Track->Xm-Track->Xh!=0){
		Double_t tstart=(0-Track->Xh)/(Track->Xm-Track->Xh);
		Start[0]=0.;
		Start[1]=tstart*(Track->Ym-Track->Yh)+Track->Yh;
		Start[2]=tstart*(Track->Zm-Track->Zh)+Track->Zh;

		Double_t tstop=((Double_t)numberOfColumns*2-Track->Xh)/(Track->Xm-Track->Xh);
		Stop[0]=(Double_t)numberOfColumns*2;
		Stop[1]=tstop*(Track->Ym-Track->Yh)+Track->Yh;
		Stop[2]=tstop*(Track->Zm-Track->Zh)+Track->Zh;

		Double_t trstart=(10-Track->Xh)/(Track->Xm-Track->Xh);
		TStart[0]=10.;
		TStart[1]=trstart*(Track->Ym-Track->Yh)+Track->Yh;
		TStart[2]=trstart*(Track->Zm-Track->Zh)+Track->Zh;
	      }

	      //cout<<TStart[0]<<" "<<TStart[1]<<" "<<TStart[2]<<endl;
   
	      VAngle=TMath::ATan((Stop[2]-Start[2])/TMath::Sqrt((Stop[0]-Start[0])*(Stop[0]-Start[0])+(Stop[1]-Start[1])*(Stop[1]-Start[1])))*deg;
	      if(Stop[0]-Start[0]>0){
		HAngle=TMath::ATan((Stop[1]-Start[1])/(Stop[0]-Start[0]))*deg;
		Angle3D=TMath::ATan(TMath::Sqrt((Stop[1]-Start[1])*(Stop[1]-Start[1])+(Stop[2]-Start[2])*(Stop[2]-Start[2]))/(Stop[0]-Start[0]))*deg;
	      }
	      else{
		HAngle=180.+TMath::ATan((Stop[1]-Start[1])/(Stop[0]-Start[0]))*deg;
		Angle3D=180.+TMath::ATan(TMath::Sqrt((Stop[1]-Start[1])*(Stop[1]-Start[1])+(Stop[2]-Start[2])*(Stop[2]-Start[2]))/(Stop[0]-Start[0]))*deg;
	      }

	      //cout<<"VAngle :"<<VAngle<<", HAngle :"<<HAngle<<endl;

	      ///// CORRECTING THE LATERAL RANGE //////

	      Double_t trans_cor;

	      for(int k=0;k<ActarLatBin;k++)
		{
		  Range=hActarSimAlphaLatR->GetBinCenter(k);
		  LatRange=hActarSimAlphaLatR->GetBinContent(k);
		  /*
		  if(Start[2]!=0)trans_cor=Range*TMath::Tan(VAngle*rad)/Start[2];
		  else trans_cor=0;

		  LatRange*=TMath::Sqrt(1+trans_cor);
		  */
		  gActarSimAlphaLatR->SetPoint(k,Range,LatRange);
		}

	      ///// FILLING THE CHARGE PROFILE //////

	      //if(h_bragg&&h_latspread)cout<<"HBRAGG & LATSPREAD"<<endl;
	      //if(padCharge&&padHeight)cout<<"PADCHARGE & PADHEIGHT"<<endl;
	      //cout<<Start[0]<<" "<<Stop[0]<<" "<<Rmin<<" "<<Rmax<<" "<<Cmin<<" "<<Cmax<<" "<<threshold<<" "<<NSlices<<endl;
	      Cmin=0;
	      //WeightedPadProjection(padCharge,padHeight,Start,Stop,Rmin,Rmax,Cmin,Cmax,threshold,gdEdx,gLatRange,gRange);
	      WeightedPadProjection3(padCharge,padHeight,Start,Stop,Rmin,Rmax,Cmin,Cmax,threshold,PadBin,gdEdx,gLatRange,gRange);

	      gLongBragg = new TGraph(NBinsGlobal,gRange,gdEdx); 
	      gLatBragg = new TGraph(NBinsGlobal,gRange,gLatRange); 

	      RangeO=TMath::Sqrt((Stop[0]-Start[0])*(Stop[0]-Start[0])+(Stop[1]-Start[1])*(Stop[1]-Start[1]));

	      bragR=new TSpline3(h_bragg);

	      Double_t rmaxval,rmax;
	      rmaxval=0;

	      for(Int_t n=0;n<NBinsGlobal;n++){
		if(gdEdx[n]>rmaxval){rmaxval=gdEdx[n]; rmax=gRange[n];}
	      }

	      Double_t step=0.1;
	      Double_t value;
	      Double_t LastPoint=0.;
	      Double_t max;
	      Double_t posmax=0;
	      Double_t posmax2=0;

	      //for(Double_t val=rmax;val<=RangeO;val=val+step){
	      for(Double_t val=rmax;val<=rmax+30;val=val+step){
		value=gLongBragg->Eval(val);
		if(value<1e-6 && LastPoint==0.)LastPoint=val;
		if(value>=rmaxval/StopPoint){
		  max=value;
		  posmax=val;
		}
	      }

	      //cout<<"event "<<jentry<<" rmaxval "<<rmaxval<<" rmax "<<rmax<<" LastPoint "<<LastPoint<<endl;

	      RangeOS=TMath::Sqrt((TStart[0]-Start[0])*(TStart[0]-Start[0])+(TStart[1]-Start[1])*(TStart[1]-Start[1])+(TStart[2]-Start[2])*(TStart[2]-Start[2]));

	      h_range->Fill(posmax-RangeOS);

	      ///////////////////////////////////////////////////////////////////
	      ////Adjustment using MINUIT for 2 par (profile shift and hight)////
	      ///////////////////////////////////////////////////////////////////

	      if(AlphaBraggFitFlag){

		//NumericalMinimization("Minuit","",ActarPreEndPoint-posmax2,1948/ActarSimdEMax,1);

		RShiftGlobal=ActarPreEndPoint-LastPoint;
		multiGlobal=1948/ActarSimdEMax;
		Double_t multi,RShift;
		NumericalMinimization(gdEdx,gRange,ActarPreEndPoint-LastPoint,multiGlobal,multi,RShift);

		Int_t startbin=0;
		Int_t maxbin=0;
		Int_t endbin=0;
		Double_t valmax=0.;
		Double_t simmaxrange=0.;
		Double_t simrange=0.;

		Double_t trans_cor;

		for(int k=0;k<ActarBin;k++)
		  {
		    gActarSimAlphadEvsR->SetPoint(k,ActarSimrange[k]-RShift,ActarSimdE[k]);

		    if(startbin==0 && ActarSimrange[k]-RShift>=0)startbin=k;
		    else if(startbin!=0 && endbin==0 && ActarSimdE[k]<0.1){endbin=k; simrange=ActarSimrange[k]-RShift;}

		    if(ActarSimdE[k]>valmax){valmax=ActarSimdE[k]; maxbin=k; simmaxrange=ActarSimrange[k]-RShift;}
		    //cout<<k<<" "<<ActarSimrange[k]-RShift<<" "<<ActarSimdE[k]*multi<<" "<<startbin<<" "<<maxbin<<" "<<endbin<<endl;

		    ///// CORRECTING THE LATERAL RANGE //////
		    Range=hActarSimAlphaLatR->GetBinCenter(k);
		    LatRange=hActarSimAlphaLatR->GetBinContent(k);
		    /*
		    if(Start[2]!=0)trans_cor=Range*TMath::Tan(VAngle*rad)/Start[2];
		    else trans_cor=0;

		    LatRange*=TMath::Sqrt(1+trans_cor);
		    */
		    //gActarSimAlphaLatR->SetPoint(k,Range,LatRange);
		    gActarSimAlphaLatR->SetPoint(k,ActarSimrange[k]-RShift,LatRange);

		  }

		Double_t Residu,LatResidu;
		Double_t Realchisq=0.;
		Double_t RealLatchisq=0.;
		Double_t frac=0.;
		Double_t latfrac=0.;
		Double_t err_n=0.;
		Double_t laterr_n=0.;
		Int_t NChi=0;
		Int_t latNChi=0;
		Double_t FixPoint=127;

		//for(int k=0;k<NBinsGlobal;k++)
		for(int k=0;k<BraggNBin;k++)
		  {
		    if(gRange[k]>40 && gdEdx[k]>20){
		      frac=gdEdx[k]-gActarSimAlphadEvsR->Eval(gRange[k],0,"");
		      err_n=1/gdEdx[k];
		      Realchisq+=frac*frac*err_n;		
		      //cout<<k<<" "<<frac<<" "<<h_bragg->GetBinContent(k)<<" "<<err_n<<" "<<Realchisq<<" "<<frac*frac*err_n<<endl;			
		      NChi++;
		      //}

		      //if(gRange[k]>40 && gdEdx[k]>20){
		      latfrac=gLatRange[k]-gActarSimAlphaLatR->Eval(gRange[k],0,"");
		      //err_n=1/gdEdx[k];
		      RealLatchisq+=latfrac*latfrac;		
		      //cout<<k<<" "<<frac<<" "<<h_bragg->GetBinContent(k)<<" "<<err_n<<" "<<Realchisq<<" "<<frac*frac*err_n<<endl;			
		      latNChi++;
		    }
		  }

		if((NChi-2)!=0)Realchisq=Realchisq/(NChi-2);
		//if((NChi)!=0)Realchisq=Realchisq/(2*NChi);

		//if(Realchisq<(0.0004+1e-6*VAngle*VAngle)){
		LongChisq[NChisq]=Realchisq;
		LongChisqRange[NChisq]=VAngle;
		NChisq++;
		//}

		if((latNChi-2)!=0)RealLatchisq=RealLatchisq/(latNChi-2);

		//cout<<"Realchisq "<<Realchisq<<" VAngle "<<VAngle<<endl;
		//cout<<"RealLatchisq "<<RealLatchisq<<" VAngle "<<VAngle<<endl;

		hchi2vsvangle->Fill(VAngle,Realchisq);
		//if(Realchisq<(0.0006+1e-6*VAngle*VAngle))hchi2vsvangle->Fill(VAngle,Realchisq);
		hlatchi2vsvangle->Fill(VAngle,RealLatchisq);

	      }

	      if(CumulTracks){

		//for(Double_t s=0;s<250;s=s+1){
		for(int k=0;k<BraggNBin;k++)
		  {
		    //binshift=(int)(h_bragg->GetBinCenter(k)+140-posfin);
		    //binshift=(int)(gRange[k]+LastPointRef-LastPoint)/BraggBinSize;
		    RangeShift=(gRange[k]+LastPointRef-LastPoint)/BraggBinSize;
		    binshift=(int)RangeShift;
		    //if(gdEdx[k]>0)cout<<"event "<<jentry<<" gdEdx "<<gdEdx[k]<<" RangeShift "<<RangeShift<<" gRange "<<gRange[k]<<" gLatRange "<<gLatRange[k]<<" LastPoint "<<LastPoint<<endl;
		    if(binshift>=0 && binshift<BraggNBin){
		      //gdEdxCumul[k]+=gdEdx[binshift];
		      //gLatRangeCumul[k]+=gLatRange[binshift];

		      //cout<<"event "<<jentry<<" binshift"<<binshift<<" gdEdx "<<gdEdx[k]<<endl;
		      //if(binshift==80)cout<<"binshift 80, event "<<jentry<<" gdEdx "<<gdEdx[k]<<endl;
		      h_braggcumulbin[binshift]->Fill(gdEdx[k]);
		      h_latbraggcumulbin[binshift]->Fill(gLatRange[k]);
		    }

		    h_braggcumuladd->Fill(gRange[k],gdEdx[k]);
		    h_braggcumul->Fill(RangeShift,gdEdx[k]);
		    h_latcumul->Fill(RangeShift,gLatRange[k]);
		    //if((int)RangeShift==80)cout<<"event "<<jentry<<" gRange "<<gRange[k]<<" RangeShift "<<RangeShift<<" GetBinCenter "<<h_latcumul->GetBinCenter(RangeShift)<<" gdEdx "<<gdEdx[k]<<" gLatRange "<<gLatRange[k]<<" h_latcumul->BinContent "<<h_latcumul->GetBinContent(RangeShift)<<" h_latcumul->BinContent(80) "<<h_latcumul->GetBinContent(80)<<endl;
		    //if(abs((int)RangeShift-80)<2)cout<<"event "<<jentry<<" gRange "<<gRange[k]<<" RangeShift "<<RangeShift<<" GetBinCenter "<<h_latcumul->GetBinCenter(RangeShift)<<" gdEdx "<<gdEdx[k]<<" gLatRange "<<gLatRange[k]<<" h_latcumul->BinContent "<<h_latcumul->GetBinContent(RangeShift-0.5)<<endl;
		    //if(abs(RangeShift-80)<0.1)cout<<"event "<<jentry<<" gRange "<<gRange[k]<<" RangeShift "<<RangeShift<<" GetBinCenter "<<h_latcumul->GetBinCenter(RangeShift)<<" gdEdx "<<gdEdx[k]<<" gLatRange "<<gLatRange[k]<<endl;
		    //cout<<"event "<<jentry<<" GetBinCenter "<<h_latcumul->GetBinCenter(RangeShift)<<" gRange "<<gRange[k]<<" RangeShift "<<RangeShift<<" gdEdx "<<gdEdx[k]<<" gLatRange "<<gLatRange[k]<<endl;

		    //if(jentry%99==0)cout<<s-11<<"  "<<h_braggcumul->GetBinContent(s)<<endl;
		    //if(jentry==99)cout<<s<<"  "<<h_braggcumul->GetBinContent(s)<<endl;
		    NBPeaks++;
		  }

	      }

	    }

	  ///////////////End of reaction analysis/////////////////////

	  //if(TrackBragFlag && jentry%99==0){
	  //if(TrackBragFlag && ((CumulTracks && jentry==NProf-1) || !CumulTracks)){
	  //if(TrackBragFlag){
	  if(TrackBragFlag && Candidate){
	    //cout<<"RangeA[0] "<<RangeA[0]<<" RangeB[0] "<<RangeB[0]<<endl;
	    //cout<<"RangeA[1] "<<RangeA[1]<<" RangeB[1] "<<RangeB[1]<<endl;
	    /*
	    Can_brag0->cd();
	    h_braggcumul->SetLineWidth(3);
	    h_braggcumul->Draw();
	    Can_brag0->Update();
	    */
	    Can_brag->cd(1);
	    
	    h_bragg->SetYTitle("Lateral Spread (mm)");
	    h_bragg->SetXTitle("Range (mm)");	
	    h_bragg->SetMaximum(0.12);  
	    h_bragg->SetMinimum(0);
	    h_bragg->Draw();

	    gLongBragg->SetLineWidth(3);
	    gLongBragg->SetLineColor(kBlue);
	    gLongBragg->Draw("same");
	    
	    //h_braggcumul->SetLineWidth(3);
	    //h_braggcumul->Draw();

	    gActarSimAlphadEvsR->SetLineWidth(3);
	    gActarSimAlphadEvsR->SetLineColor(kRed);
	    gActarSimAlphadEvsR->Draw("same");

	    Can_brag->cd(2);
	    
	    h_latspread->SetYTitle("Lateral Spread (mm)");
	    h_latspread->SetXTitle("Range (mm)");	    
	    h_latspread->SetMaximum(4.);  
	    h_latspread->SetMinimum(0.);
	    h_latspread->Draw();

	    gLatBragg->SetLineWidth(3);
	    gLatBragg->SetLineColor(kBlue);
	    gLatBragg->Draw("same");

	    gActarSimAlphaLatR->SetLineColor(kRed);
	    gActarSimAlphaLatR->SetLineWidth(2);
	    gActarSimAlphaLatR->Draw("same");

	    /*
	    hActarSimAlphaLatR->SetLineColor(kRed);
	    hActarSimAlphaLatR->SetLineWidth(2);
	    hActarSimAlphaLatR->Draw("same");

	    gActarSimAlphaLatR->SetLineColor(kGreen);
	    gActarSimAlphaLatR->SetLineWidth(2);
	    gActarSimAlphaLatR->Draw("same");
	    */
	    Can_brag->Update();

	  }

	  if(Track3DVisuFlag){	
	    Can_3D->cd();
	    //h3DTrack->Draw("lego2");
	    //h3DTrack->Draw("COL");
	    //h3DTrack->Draw("ISO");	
	    //h3DTrack->Draw("GLCOL");	
	    h3DTrack->Draw("BOX");

	    if(TrackAnalysis){
	      TPolyLine3D *pl0 = new TPolyLine3D(2);
	      pl0->SetPoint(0,Track->Xm/2,Track->Ym/2,Track->Zm/2);
	      pl0->SetPoint(1,Track->Xh/2,Track->Yh/2,Track->Zh/2);
	      pl0->SetLineColor(3);
	      pl0->SetLineWidth(3);
	      pl0->Draw("same");
	    }
	    Can_3D->Update();
	  }

	  //if(TrackVisuFlag && jentry%99==0){
	  //if(TrackVisuFlag && jentry==NProf-1){
	  //if(TrackVisuFlag && ((CumulTracks && jentry==NProf-1) || !CumulTracks)){
	  //if(TrackVisuFlag){
	  if(TrackVisuFlag && Candidate){

	    Can_track->cd();

	    visu_charge->SetXTitle("Column #");
	    visu_charge->SetYTitle("Row #");
	    visu_charge->Draw("colz");
	    /*
	    if(TrackAnalysis){
	      TLine linefit(IniPoint[0], IniPoint[1], FinPoint[0], FinPoint[1]);
	      linefit.SetLineStyle(kDashed);
	      linefit.SetLineWidth(5);
	      linefit.Draw("same");
	    }
	    */
	    /*
	      Can_track->cd(2);
	      visu_time->Draw("colz");
	    */
	    /*
	    TLine line1(0, 14, numberOfColumns, 14);
	    line1.Draw("same");
	    TLine line2(0, 18, numberOfColumns, 18);
	    line2.Draw("same");
	    */
	    Can_track->Update();
	    Can_track->WaitPrimitive();
	  }
     
	}

    }

  }

  //*********************************************************************************************************//
  //*********************************************************************************************************//
  //**                                                                                                     **//
  //**                                       End of Event Loop                                             **//
  //**                                                                                                     **//
  //*********************************************************************************************************//
  //*********************************************************************************************************//


  if(RangeFlag){
    TCanvas* Can_range=new TCanvas("Can_range","Can_range",900,900);
    h_range->SetLineWidth(2);
    h_range->Draw();
  }


  if(EnergyFlag){
    TCanvas* Can_energy_track=new TCanvas("Can_energy_track","Can_energy_track",900,900);
    //Can_energy_track->Divide(1,2);
    //Can_energy_track->cd(1);
    h_energy_track->SetLineColor(1);       
    h_energy_track->SetLineWidth(2);
    h_energy_track->Draw();
  }

  if(ChiFlag){
    TGraph *gLongChisq=new TGraph(NChisq,LongChisqRange,LongChisq);

    TCanvas* Can_chi2=new TCanvas("Can_chi2","Can_chi2",900,900);
    
    hchi2vsvangle->SetMarkerStyle(3);
    hchi2vsvangle->SetXTitle("Vertical Angle [deg]");
    hchi2vsvangle->Draw();
    //hchi2vsvangle->Fit("pol2");
    
    gLongChisq->SetMarkerStyle(23);
    gLongChisq->Draw("Psame");
    gLongChisq->Fit("pol2");
    //gLongChisq->Draw("Psame");

    TCanvas* Can_latchi2=new TCanvas("Can_latchi2","Can_latchi2",900,900);
    hlatchi2vsvangle->SetMarkerStyle(3);
    hlatchi2vsvangle->SetXTitle("Vertical Angle [deg]");
    hlatchi2vsvangle->Draw();

  }

  if(CumulTracks){

    Double_t LongBraggRange[BraggNBin];
    Double_t LongBraggRangeError[BraggNBin];
    Double_t LatBraggRange[BraggNBin];
    Double_t LatBraggRangeError[BraggNBin];
    Double_t BraggRange[BraggNBin];
    TGraphErrors *gLongBraggErr;
    TGraphErrors *gLatBraggErr;

    for(int l=0;l<BraggNBin;l++){
	LongBraggRange[l]=LatBraggRange[l]=BraggRange[l]=0;
      }

    cout<<"NBPeaks "<<NBPeaks<<endl;

    for(Double_t s=0;s<BraggNBin;s++){
      //cout<<s<<"  "<<h_braggcumul->GetBinCenter(s)<<"  "<<h_braggcumul->GetBinContent(s)/NBPeaks*BraggNBin<<endl;
      //if(s==80)cout<<s<<"  "<<h_latcumul->GetBinCenter(s)<<"  "<<h_latcumul->GetBinContent(s)<<"  "<<h_latcumul->GetBinContent(s)/NBPeaks*BraggNBin<<endl;
      h_braggcumulnorm->Fill(h_braggcumul->GetBinCenter(s),h_braggcumul->GetBinContent(s)/NBPeaks*BraggNBin);
      h_latcumulnorm->Fill(h_latcumul->GetBinCenter(s),h_latcumul->GetBinContent(s)/NBPeaks*BraggNBin);

      BraggRange[(int)s]=s*BraggBinSize+0.5*BraggBinSize;
      LongBraggRange[(int)s]=h_braggcumulbin[(int)s]->GetMean();
      LongBraggRangeError[(int)s]=h_braggcumulbin[(int)s]->GetMeanError();
      LatBraggRange[(int)s]=h_latbraggcumulbin[(int)s]->GetMean();
      LatBraggRangeError[(int)s]=h_latbraggcumulbin[(int)s]->GetMeanError();

      //cout<<"Bin "<<s<<" LongBragg from bin "<<LongBraggRange[(int)s]<<" LongBragg from cumul "<<h_braggcumulnorm->GetBinContent(s)<<endl;
      //if(s>70 && s<100)cout<<"Bin "<<s<<" LongBragg from bin "<<LongBraggRange[(int)s]<<" LongBragg from cumul "<<h_braggcumulnorm->GetBinContent(s)<<endl;
    }

      gLongBraggErr=new TGraphErrors(BraggNBin,BraggRange,LongBraggRange,0,LongBraggRangeError);
      gLatBraggErr=new TGraphErrors(BraggNBin,BraggRange,LatBraggRange,0,LatBraggRangeError);
      
      //for(Double_t s=0;s<250;s=s+1){
      for(Double_t s=0;s<BraggNBin;s=s+1){
	//cout<<h_braggcumulfix->GetBinCenter(s)<<"  "<<h_braggcumulfix->GetBinContent(s)/NProf<<endl;
	cout<<BraggRange[(int)s]<<"  "<<LongBraggRange[(int)s]<<endl;
      }
      
      //for(Double_t s=0;s<250;s=s+1){
      for(Double_t s=0;s<BraggNBin;s=s+1){
	//cout<<h_latspread_allfix->GetBinCenter(s)<<"  "<<h_latspread_allfix->GetBinContent(s)/NProf<<endl;
	cout<<BraggRange[(int)s]<<"  "<<LatBraggRange[(int)s]<<endl;
      }
      
      TCanvas* Can_cumul=new TCanvas("Can_cumul","Can_cumul",900,900);
      Can_cumul->Divide(1,2);
      Can_cumul->cd(1);
      h_braggcumulnorm->SetLineColor(kRed);
      h_braggcumulnorm->SetLineWidth(2);
      h_braggcumulnorm->SetXTitle("Vertical Angle [deg]");
      h_braggcumulnorm->Draw();

      gLongBraggErr->SetLineColor(kBlack);
      gLongBraggErr->SetLineWidth(2);
      gLongBraggErr->Draw("same");

      Can_cumul->cd(2);
      h_latcumulnorm->SetLineColor(kRed);
      h_latcumulnorm->SetLineWidth(2);
      h_latcumulnorm->SetXTitle("Vertical Angle [deg]");
      h_latcumulnorm->Draw();

      gLatBraggErr->SetLineColor(kBlack);
      gLatBraggErr->SetLineWidth(2);
      gLatBraggErr->Draw("same");
  }

  if(TimerFlag)printf("Time at the end of job = %f seconds\n",timer.CpuTime());
  //cout<<"ProjCPUTime "<<ProjCPUTime<<endl;
} 


void FitMat(Double_t **PADNET, Int_t Rmin, Int_t Rmax, Int_t Cmin, Int_t Cmax, Double_t threshold, Double_t &a, Double_t &b)
{
	Int_t Row, Col;
	Double_t A, B, C, UEV, Q, X, Xg, Y, Yg;
	A=B=C=UEV=Q=X=Y=Xg=Yg=0.;
	for (Row=Rmin;Row<=Rmax;Row++)
		for (Col=Cmin;Col<=Cmax;Col++)
			if(PADNET[Row][Col]>threshold)
			{
			  X= Col*2.+1.;
			  Y= Row*2.+1.;
			  Q+=PADNET[Row][Col];
			  Xg+=X*PADNET[Row][Col];
			  Yg+=Y*PADNET[Row][Col];
			}
	Xg/=Q;
	Yg/=Q;
	//cout<<"Xg: "<<Xg<<" Yg: "<<Yg<<endl;

	for (Row=Rmin;Row<=Rmax;Row++)
		for(Col=Cmin;Col<=Cmax;Col++)
			if(PADNET[Row][Col]>threshold)
			{
				X= Col*2.+1.;
				Y= Row*2.+1.;
				A+=PADNET[Row][Col]*(X-Xg)*(X-Xg);
				B+=PADNET[Row][Col]*(X-Xg)*(Y-Yg);
				C+=PADNET[Row][Col]*(Y-Yg)*(Y-Yg);
			}

	UEV=0.5*(A+C+TMath::Sqrt((A+C)*(A+C)-4*(A*C-B*B)));
	a=B/(UEV-C);
	b=Yg-a*Xg;

}


void FitMatZ(Double_t **PADNET, Double_t **TIME, Int_t Rmin, Int_t Rmax, Int_t Cmin, Int_t Cmax, Double_t threshold, Double_t Tthreshold, Double_t &a, Double_t &b)
{
	Int_t Row, Col;
	const Int_t NRow=32;
        Double_t A, B, C, UEV, Q, X, Xg, Y, Yg;
        Double_t Qrow[NRow],Cm[NRow],Zm[NRow];
	Int_t min_col,max_col,ncol;
	//cout<<"Ttreshold: "<<Tthreshold<<", NRow: "<<NRow<<endl;
	A=B=C=UEV=Q=X=Y=Xg=Yg=0.;
	for (Row=Rmin;Row<=Rmax;Row++)
	  {
	    Zm[Row]=0;
	    Qrow[Row]=0;
	    Cm[Row]=0.;
	    min_col=max_col=ncol=0;
	    for (Col=Cmin;Col<=Cmax;Col++)
	      {
		if(PADNET[Row][Col]>threshold && TIME[Row][Col]>Tthreshold)
		  {	
		    ncol++;
		    if(min_col==0)min_col=Col;
		    max_col=Col;
		    Cm[Row]+=PADNET[Row][Col]*(Col+1);//+1 to avoid problem at Col=0
		    Qrow[Row]+=PADNET[Row][Col];
		    //cout<<"Row: "<<Row<<", minC: "<<min_col<<", maxC: "<<max_col<<", PAD: "<<PADNET[Row][Col]<<", NRow: "<<NRow<<endl;
		  }
	      }

	    if(ncol!=0)
	      {
		Cm[Row]/=Qrow[Row];
		Cm[Row]-=1.;	   
		for(Col=min_col;Col<max_col+1;Col++)
		  {
		    if(PADNET[Row][Col]>threshold && TIME[Row][Col]>Tthreshold)
		      {
			Zm[Row]+=TMath::Sqrt(TIME[Row][Col]*TIME[Row][Col]-4*(Col-Cm[Row])*(Col-Cm[Row]));	  
		      }
		  }
		Zm[Row]/=ncol; 
		//cout<<"Row: "<<Row<<", QCol: "<<Qrow[Row]<<", ZmCol: "<<Zm[Row]<<endl;
	      }
	  }
	

	for (Row=Rmin;Row<=Rmax;Row++)
	  {

	    if(Cm[Row] && Zm[Row])
	      {
		X= Row*2.+1.;
		Y= Zm[Row];//*2.;
		Q+=Qrow[Row];
		Xg+=X*Qrow[Row];
		Yg+=Y*Qrow[Row];
		//cout<<"Row: "<<Row<<",Zm: "<<Zm[Row]<<",Cm: "<<Cm[Row]<<endl;
	      }


	  }

	Xg/=Q;
	Yg/=Q;
	//cout<<"Xg: "<<Xg<<",Yg: "<<Yg<<endl;

	for (Row=Rmin;Row<=Rmax;Row++)
	  {
	    if(Cm[Row] && Zm[Row])
	      {
		X= Row*2.+1.;
		Y= Zm[Row];//*2.;
		A+=Qrow[Row]*(X-Xg)*(X-Xg);
		B+=Qrow[Row]*(X-Xg)*(Y-Yg);
		C+=Qrow[Row]*(Y-Yg)*(Y-Yg);
	      }
	  }

	UEV=0.5*(A+C+TMath::Sqrt((A+C)*(A+C)-4*(A*C-B*B)));
	a=B/(UEV-C);
	b=Yg-a*Xg;
}


//Double_t FitMat3D(Int_t T, TMatrixD PADNET, TMatrixD TIME, Int_t Rmin, Int_t Rmax, Int_t Cmin, Int_t Cmax, Double_t threshold, MTrack* T)
Double_t FitMat3D(Double_t **PADNET, Double_t **HEIGHT, Int_t Rmin, Int_t Rmax, Int_t Cmin, Int_t Cmax, Double_t threshold, MTrack* T)
{
	// adapted from: http://fr.scribd.com/doc/31477970/Regressions-et-trajectoires-3D
	Int_t R, C;
	Double_t Q,X,Y,Z;
	Double_t Xm,Ym,Zm;
	Double_t Sxx,Sxy,Syy,Sxz,Szz,Syz;
	Double_t theta;
	Double_t K11,K22,K12,K10,K01,K00;
	Double_t c0,c1,c2;
	Double_t p,q,r,dm2;
	Double_t rho,phi;
	Double_t a,b;
	
	Double_t PI=3.1415926535897932384626433;

	//cout<<"Rmin "<<Rmin<<" Rmax "<<Rmax<<" Cmin "<<Cmin<<" Cmax "<<Cmax<<endl;

	Q=Xm=Ym=Zm=0.;
	Sxx=Syy=Szz=Sxy=Sxz=Syz=0.;
	
	for (R=Rmin;R<=Rmax;R++)
		for (C=Cmin;C<=Cmax;C++)
			if(PADNET[R][C]>threshold && HEIGHT[R][C])
			{
			  X= C*2.+1.;
			  Y= R*2.+1.;
			  Z= HEIGHT[R][C];//*VDRIFT;
			  Q+=PADNET[R][C];
			  Xm+=X*PADNET[R][C];
			  Ym+=Y*PADNET[R][C];
			  Zm+=Z*PADNET[R][C];
			  Sxx+=X*X*PADNET[R][C];
			  Syy+=Y*Y*PADNET[R][C];
			  Szz+=Z*Z*PADNET[R][C];
			  Sxy+=X*Y*PADNET[R][C];
			  Sxz+=X*Z*PADNET[R][C];
			  Syz+=Y*Z*PADNET[R][C];
			  //cout<<"R "<<R<<" C "<<C<<" Y "<<Y<<" Ym "<<Ym<<endl;
			}
	Xm/=Q;
	Ym/=Q;
	Zm/=Q;
	Sxx/=Q;
	Syy/=Q;
	Szz/=Q;
	Sxy/=Q;
	Sxz/=Q;
	Syz/=Q;
	Sxx-=(Xm*Xm);
	Syy-=(Ym*Ym);
	Szz-=(Zm*Zm);
	Sxy-=(Xm*Ym);
	Sxz-=(Xm*Zm);
	Syz-=(Ym*Zm);
	
	theta=0.5*atan((2.*Sxy)/(Sxx-Syy));
	
	K11=(Syy+Szz)*pow(cos(theta),2)+(Sxx+Szz)*pow(sin(theta),2)-2.*Sxy*cos(theta)*sin(theta);
	K22=(Syy+Szz)*pow(sin(theta),2)+(Sxx+Szz)*pow(cos(theta),2)+2.*Sxy*cos(theta)*sin(theta);
	K12=-Sxy*(pow(cos(theta),2)-pow(sin(theta),2))+(Sxx-Syy)*cos(theta)*sin(theta);
	K10=Sxz*cos(theta)+Syz*sin(theta);
	K01=-Sxz*sin(theta)+Syz*cos(theta);
	K00=Sxx+Syy;
	
	c2=-K00-K11-K22;
	c1=K00*K11+K00*K22+K11*K22-K01*K01-K10*K10;
	c0=K01*K01*K11+K10*K10*K22-K00*K11*K22;
		
	p=c1-pow(c2,2)/3.;
	q=2.*pow(c2,3)/27.-c1*c2/3.+c0;
	r=pow(q/2.,2)+pow(p,3)/27.;
	
	if(r>0) {dm2=-c2/3.+pow(-q/2.+sqrt(r),1./3.)+pow(-q/2.-sqrt(r),1./3.);cout<<"R>0"<<endl;}
	if(r<0)
	{
	  //cout<<"R<0"<<endl;
	  rho=sqrt(-pow(p,3)/27.);
	  phi=acos(-q/(2.*rho));
	  //dm2=min(-c2/3.+2.*pow(rho,1./3.)*cos(phi/3.),min(-c2/3.+2.*pow(rho,1./3.)*cos((phi+2.*PI)/3.),-c2/3.+2.*pow(rho,1./3.)*cos((phi+4.*PI)/3.)));
	  dm2=TMath::Min(-c2/3.+2.*pow(rho,1./3.)*cos(phi/3.),TMath::Min(-c2/3.+2.*pow(rho,1./3.)*cos((phi+2.*PI)/3.),-c2/3.+2.*pow(rho,1./3.)*cos((phi+4.*PI)/3.)));
	}
	a=-K10*cos(theta)/(K11-dm2)+K01*sin(theta)/(K22-dm2);
	b=-K10*sin(theta)/(K11-dm2)-K01*cos(theta)/(K22-dm2);

	T->Xm=Xm;
	T->Ym=Ym;
	T->Zm=Zm;
	T->Xh=((1.+b*b)*Xm-a*b*Ym+a*Zm)/(1.+a*a+b*b);
	T->Yh=((1.+a*a)*Ym-a*b*Xm+b*Zm)/(1.+a*a+b*b);
	T->Zh=((a*a+b*b)*Zm+a*Xm+b*Ym)/(1.+a*a+b*b);
	
	T->L2DXY->SetX1(T->Xm);
	T->L2DXY->SetY1(T->Ym);
	T->L2DXY->SetX2(T->Xh);
	T->L2DXY->SetY2(T->Yh);
	
	T->L2DXZ->SetX1(T->Xm);
	T->L2DXZ->SetY1(T->Zm);
	T->L2DXZ->SetX2(T->Xh);
	T->L2DXZ->SetY2(T->Zh);
	
	T->L2DYZ->SetX1(T->Ym);
	T->L2DYZ->SetY1(T->Zm);
	T->L2DYZ->SetX2(T->Yh);
	T->L2DYZ->SetY2(T->Zh);
	
	return(dm2/Q);
}

void TrackPadProjection(Double_t **padCharge, Double_t **padHeight, Double_t *Start, Double_t *Stop, Int_t Rmin, Int_t Rmax, Int_t Cmin, Int_t Cmax, Double_t threshold, TH1F* h_bragg)
{
  Double_t PI=3.1415926535897932384626433;
  Double_t deg=180./PI;
  Double_t rad=PI/180.;

  Double_t VAngle,HAngle;

  VAngle=TMath::ATan((Stop[2]-Start[2])/TMath::Sqrt((Stop[0]-Start[0])*(Stop[0]-Start[0])+(Stop[1]-Start[1])*(Stop[1]-Start[1])))*deg;
  if(Stop[0]-Start[0]>0){
    HAngle=TMath::ATan((Stop[1]-Start[1])/(Stop[0]-Start[0]))*deg;
  }
  else{
    HAngle=180.+TMath::ATan((Stop[1]-Start[1])/(Stop[0]-Start[0]))*deg;
  }

  cout<<"VAngle :"<<VAngle<<", HAngle :"<<HAngle<<endl;

  TVector3 v2(Stop[0]-Start[0],Stop[1]-Start[1],Stop[2]-Start[2]);
  Double_t mag=v2.Mag();

  h_bragg->Reset();

  for(Int_t r=Rmin;r<Rmax+1;r++){//Rows
    for(Int_t c=Cmin;c<Cmax+1;c++){ //columns
      Double_t xPad=0.,yPad=0.,zPad=0.,xProj=0.,yProj=0.;
      if(padCharge[r][c] >threshold){
	xPad = (2*c+1.);
	yPad = (2*r+1.);
	zPad = padHeight[r][c];

	TVector3 v1(xPad-Start[0],yPad-Start[1],zPad-Start[2]);
	range=v1*v2/mag;

	h_bragg->Fill(range,padCharge[r][c]);

      }	    
    }

  }//End of Loop on pads

}

void WeightedPadProjection(Double_t **padCharge, Double_t **padHeight, Double_t *Start, Double_t *Stop, Int_t Rmin, Int_t Rmax, Int_t Cmin, Int_t Cmax, Double_t threshold, Double_t* gdEdx, Double_t* gLatRange, Double_t* gRange)
{
  Bool_t TimerFlag=0;
  if(TimerFlag){
    TStopwatch timer2;
    printf("Starting projection timer\n");
    timer2.Start();
  }

  Double_t PI=3.1415926535897932384626433;
  Double_t deg=180./PI;
  Double_t rad=PI/180.;

  Double_t binSize=300/(Double_t)NBinsGlobal;
  Double_t PadSize=2.;
  Double_t sum[NBinsGlobal],sum2[NBinsGlobal];
  Double_t xGSlice[NBinsGlobal],yGSlice[NBinsGlobal];
  Int_t NSample[NBinsGlobal];//Number of pad sample in each slice
  Double_t wsig[NBinsGlobal];

  for(Int_t i=0;i<NBinsGlobal;i++){
    gdEdx[i]=0;
    gLatRange[i]=0;
    gRange[i]=i*binSize+binSize/2;
    sum[i]=sum2[i]=xGSlice[i]=yGSlice[i]=wsig[i]=0;
    NSample[i]=0;
  }

  const Int_t NSamp=10; 
  Double_t xGSample[NBinsGlobal][NSamp],yGSample[NBinsGlobal][NSamp],dQSample[NBinsGlobal][NSamp];//barycenter and charge of sample

  Double_t VAngle,HAngle;

  Int_t NSlice;
  //cout<<NBinsGlobal<<" "<<binSize<<endl;

  VAngle=TMath::ATan((Stop[2]-Start[2])/TMath::Sqrt((Stop[0]-Start[0])*(Stop[0]-Start[0])+(Stop[1]-Start[1])*(Stop[1]-Start[1])))*deg;
  if(Stop[0]-Start[0]>0){
    HAngle=TMath::ATan((Stop[1]-Start[1])/(Stop[0]-Start[0]))*deg;
  }
  else{
    HAngle=180.+TMath::ATan((Stop[1]-Start[1])/(Stop[0]-Start[0]))*deg;
  }

  //cout<<"WeightedMethod, VAngle :"<<VAngle<<", HAngle :"<<HAngle<<endl;

  TVector3 v2(Stop[0]-Start[0],Stop[1]-Start[1],Stop[2]-Start[2]);
  Double_t mag=v2.Mag();

  TVector3 v2pad(Stop[0]-Start[0],Stop[1]-Start[1],0);
  Double_t magpad=v2pad.Mag();

  Double_t PointI[2],PointIProjPad1[2],PointIProjPad2[2];
  Double_t Proj1[2],Proj2[2];
  Double_t PreProj1[2],PreProj2[2];
  Double_t Vertex[7][2],tmpVertex[7][2],PreVertex1[2],PreVertex2[2],PreVertex3[2];
  Int_t VertexN;

  Int_t VN;
  Double_t xG,yG;

  Double_t dR;
  Double_t dQ;

  TVector3 vA,vB;
  Double_t rA,rB; //projection of pad bottom left corner and top right corner on 3D fitted track
  Double_t dS1,dS2,dSi,dSpre,dStot;
  Double_t PadSide1[2],PadSide2[2];

  Double_t dQtot,dQtotproj;
  Double_t dQpad,dQtmp;
  dQtot=dQtotproj=0;

  for(Int_t r=Rmin;r<Rmax+1;r++){//Rows
    for(Int_t c=Cmin;c<Cmax+1;c++){ //columns
      Double_t xPad=0.,yPad=0.,zPad=0.,xProj=0.,yProj=0.;
      if(padCharge[r][c] >threshold){
	xPad = (PadSize*c+PadSize/2.);
	yPad = (PadSize*r+PadSize/2.);
	zPad = padHeight[r][c];

	dQtot+=padCharge[r][c];

	PreVertex1[0]=PadSide1[0]=xPad-PadSize/2.; 
	PreVertex2[0]=PadSide2[0]=xPad+PadSize/2.;

	if(HAngle>=0){
	  PreVertex1[1]=PadSide1[1]=yPad-PadSize/2.; 
	  PreVertex2[1]=PadSide2[1]=yPad+PadSize/2.;
	}
	else if(HAngle<0){
	  PreVertex1[1]=PadSide1[1]=yPad+PadSize/2.; 
	  PreVertex2[1]=PadSide2[1]=yPad-PadSize/2.;
	}

	//Calculating the 2 extreme pad's edges projection on track to determine the number of slices 
	vA.SetXYZ(PadSide1[0]-Start[0],PadSide1[1]-Start[1],0);
	vB.SetXYZ(PadSide2[0]-Start[0],PadSide2[1]-Start[1],0);

	rA=vA*v2pad/magpad/TMath::Cos(VAngle*rad);
	rB=vB*v2pad/magpad/TMath::Cos(VAngle*rad);
	NSlice=(int)(rB/binSize)-(int)(rA/binSize)+1;

	dSpre=0;
	dQpad=0;
	dStot=0;

	if(NSlice==1 && (Int_t)(rA/binSize)>=0 && (Int_t)(rA/binSize)<NBinsGlobal){gdEdx[(Int_t)(rA/binSize)]+=padCharge[r][c]/binSize; dQtotproj+=padCharge[r][c]; dQpad+=padCharge[r][c];}
	else{
	  //For each slice, we calculate the surface and barycenter from the polygones vertex
	  for(Int_t s=(int)(rA/binSize); s<(int)(rB/binSize); s++)
	    {
	      VN=0;
	      PointI[0]=Start[0]+binSize*(s+1)*(Stop[0]-Start[0])/mag; //Point I correspond to the bin point on the track
	      PointI[1]=Start[1]+binSize*(s+1)*(Stop[1]-Start[1])/mag;

	      //// Close Side Vertex ////
	      tmpVertex[VN][0]=Vertex[VN][0]=PreVertex1[0];
	      tmpVertex[VN][1]=Vertex[VN][1]=PreVertex1[1];
	      VN++;

	      tmpVertex[VN][1]=Vertex[VN][1]=PadSide1[1];
	      tmpVertex[VN][0]=Vertex[VN][0]=PointI[0]+(PointI[1]-Vertex[VN][1])*(Stop[1]-Start[1])/(Stop[0]-Start[0]);

	      if(Vertex[VN][0]>(xPad+PadSize/2)){
		tmpVertex[VN][0]=Vertex[VN][0]=PadSide2[0];
		tmpVertex[VN][1]=Vertex[VN][1]=PadSide1[1];
		VN++;

		tmpVertex[VN][0]=Vertex[VN][0]=xPad+PadSize/2; 
		tmpVertex[VN][1]=Vertex[VN][1]=PointI[1]+(PointI[0]-Vertex[VN][0])*(Stop[0]-Start[0])/(Stop[1]-Start[1]);
		if(Vertex[VN][1]<(yPad-PadSize/2) || Vertex[VN][1]>(yPad+PadSize/2))cout<<"Problem with pad projection, close side, Col "<<c<<" Row "<<r<<endl;
	      }

	      PreVertex1[0]=Vertex[VN][0];
	      PreVertex1[1]=Vertex[VN][1];
	      VN++;

	      //// Further Side Vertex ////
	      tmpVertex[VN][1]=Vertex[VN][1]=PadSide2[1];
	      tmpVertex[VN][0]=Vertex[VN][0]=PointI[0]+(PointI[1]-Vertex[VN][1])*(Stop[1]-Start[1])/(Stop[0]-Start[0]);

	      if(Vertex[VN][0]<(xPad-PadSize/2)){
		tmpVertex[VN][0]=Vertex[VN][0]=xPad-PadSize/2; 
		tmpVertex[VN][1]=Vertex[VN][1]=PointI[1]+(PointI[0]-Vertex[VN][0])*(Stop[0]-Start[0])/(Stop[1]-Start[1]);
		if(Vertex[VN][1]<(yPad-PadSize/2) || Vertex[VN][1]>(yPad+PadSize/2))cout<<"Problem with pad projection, further side, Col "<<c<<" Row "<<r<<endl;
	      }

	      PreVertex2[0]=Vertex[VN][0];
	      PreVertex2[1]=Vertex[VN][1];
	      VN++;

	      if(Vertex[VN-1][1]==PadSide2[1] && (PreVertex3[1]!=PadSide2[1] || s==(int)(rA/binSize))){
		tmpVertex[VN][0]=Vertex[VN][0]=PadSide1[0];
		tmpVertex[VN][1]=Vertex[VN][1]=PadSide2[1];
		VN++;
	      }

	      if(s!=(int)(rA/binSize)){
		tmpVertex[VN][0]=Vertex[VN][0]=PreVertex3[0];
		tmpVertex[VN][1]=Vertex[VN][1]=PreVertex3[1];
		VN++;
	      }

	      PreVertex3[0]=PreVertex2[0];
	      PreVertex3[1]=PreVertex2[1];

	      if(HAngle<0){
		for(Int_t i=0;i<VN;i++){
		  Vertex[i][0]=tmpVertex[VN-1-i][0];
		  Vertex[i][1]=tmpVertex[VN-1-i][1];
		}
	      }

	      Vertex[VN][0]=Vertex[0][0];
	      Vertex[VN][1]=Vertex[0][1];

	      //// Surface and Barycenter ////
	      dSi=0;
	      xG=yG=0.;
	      
	      for(Int_t i=0;i<VN;i++){
		dSi+=0.5*(Vertex[i][0]*Vertex[i+1][1]-Vertex[i+1][0]*Vertex[i][1]);
	      }

	      if(dSi>0){

		for(Int_t i=0;i<VN;i++){
		  xG+=(Vertex[i][0]+Vertex[i+1][0])*(Vertex[i][0]*Vertex[i+1][1]-Vertex[i+1][0]*Vertex[i][1])/(6*dSi);
		  yG+=(Vertex[i][1]+Vertex[i+1][1])*(Vertex[i][0]*Vertex[i+1][1]-Vertex[i+1][0]*Vertex[i][1])/(6*dSi);
		}
	      
		dStot+=dSi;
		dQtmp=dSi*padCharge[r][c]/PadSize/PadSize;
		gdEdx[s]+=dQtmp/binSize;
		dQtotproj+=dQtmp;
		dQpad+=dQtmp;

		sum[s]+=dQtmp;
		sum2[s]+=dQtmp*dQtmp;

		xGSlice[s]+=xG*dQtmp;
		yGSlice[s]+=yG*dQtmp;

		//if(s==12)cout<<"xG "<<xG<<" yG "<<yG<<" dQtmp "<<dQtmp<<" xGSlice "<<xGSlice[s]<<" yGSlice "<<yGSlice[s]<<endl;

		dQSample[s][NSample[s]]=dQtmp;
		xGSample[s][NSample[s]]=xG;
		yGSample[s][NSample[s]]=yG;
		NSample[s]++;

	      }

	      //// For the last slice ////
	      //if(s==(int)(rB/binSize)-1 && rB/binSize-1!=(double)s){//The 2nd case is when point B projection is on a bin
	      if(s==(int)(rB/binSize)-1){
		VN=0;	      
		tmpVertex[VN][0]=Vertex[VN][0]=PreVertex1[0];
		tmpVertex[VN][1]=Vertex[VN][1]=PreVertex1[1];
		VN++;

		if(PreVertex1[1]==PadSide1[1]){
		  tmpVertex[VN][0]=Vertex[VN][0]=PadSide2[0];
		  tmpVertex[VN][1]=Vertex[VN][1]=PadSide1[1];
		  VN++;
		}

		tmpVertex[VN][0]=Vertex[VN][0]=PadSide2[0];
		tmpVertex[VN][1]=Vertex[VN][1]=PadSide2[1];
		VN++;

		tmpVertex[VN][0]=Vertex[VN][0]=PreVertex2[0];
		tmpVertex[VN][1]=Vertex[VN][1]=PreVertex2[1];
		VN++;

		if(HAngle<0){
		  for(Int_t i=0;i<VN;i++){
		    Vertex[i][0]=tmpVertex[VN-1-i][0];
		    Vertex[i][1]=tmpVertex[VN-1-i][1];
		  }
		}

		Vertex[VN][0]=Vertex[0][0];
		Vertex[VN][1]=Vertex[0][1];

		dSi=0;
		xG=yG=0.;
		
		for(Int_t i=0;i<VN;i++){
		  dSi+=0.5*(Vertex[i][0]*Vertex[i+1][1]-Vertex[i+1][0]*Vertex[i][1]);
		  //if(s+1==12)cout<<"Vertex[i][0] "<<Vertex[i][0]<<"Vertex[i][1] "<<Vertex[i][1]<<" Vertex[i+1][0] "<<Vertex[i+1][0]<<" Vertex[i+1][1] "<<Vertex[i+1][1]<<" dSi "<<dSi<<endl;
		}
		if(dSi>0){
		  for(Int_t i=0;i<VN;i++){
		    xG+=(Vertex[i][0]+Vertex[i+1][0])*(Vertex[i][0]*Vertex[i+1][1]-Vertex[i+1][0]*Vertex[i][1])/(6*dSi);
		    yG+=(Vertex[i][1]+Vertex[i+1][1])*(Vertex[i][0]*Vertex[i+1][1]-Vertex[i+1][0]*Vertex[i][1])/(6*dSi);
		  }
		
		  dStot+=dSi;
		  dQtmp=dSi*padCharge[r][c]/PadSize/PadSize;
		  gdEdx[s+1]+=dQtmp/binSize;
		  dQtotproj+=dQtmp;
		  dQpad+=dQtmp;

		  sum[s+1]+=dQtmp;
		  sum2[s+1]+=dQtmp*dQtmp;

		  xGSlice[s+1]+=xG*dQtmp;
		  yGSlice[s+1]+=yG*dQtmp;

		  //if(s+1==12)cout<<"last xG "<<xG<<" yG "<<yG<<" dQtmp "<<dQtmp<<" xGSlice "<<xGSlice[s]<<" yGSlice "<<yGSlice[s]<<" dSi "<<dSi<<endl;

		  dQSample[s+1][NSample[s+1]]=dQtmp;
		  xGSample[s+1][NSample[s+1]]=xG;
		  yGSample[s+1][NSample[s+1]]=yG;
		  NSample[s+1]++;
		}
	      }
	    }
	  //if(TMath::Abs(dQpad-padCharge[r][c])>0.001)cout<<"Col "<<c<<" Row "<<r<<" PadCharge "<<padCharge[r][c]<<" Qproj "<<dQpad<<" NSlice "<<NSlice<<endl;
	}
      }	    
    }
  }//End of Loop on pads
  //cout<<"dQtot "<<dQtot<<" dQtotproj "<<dQtotproj<<endl;

  for(Int_t i=0;i<NBinsGlobal;i++){
    if(NSample[i]>1){
      //if(i==12)cout<<sum[i]<<" "<<xGSlice[i]<<" "<<yGSlice[i]<<endl;
      xGSlice[i]/=sum[i];
      yGSlice[i]/=sum[i];
      for(Int_t s=0;s<NSample[i];s++){
	wsig[i]+=dQSample[i][s]*((xGSample[i][s]-xGSlice[i])*(xGSample[i][s]-xGSlice[i])+(yGSample[i][s]-yGSlice[i])*(yGSample[i][s]-yGSlice[i]));
      }
      //if(i==12)cout<<i<<" "<<gRange[i]<<" "<<sum[i]<<" "<<sum2[i]<<" "<<sum[i]-sum2[i]/sum[i]<<" "<<wsig[i]<<endl;
      wsig[i]=wsig[i]/(sum[i]-sum2[i]/sum[i]);

    }
    gLatRange[i]=wsig[i];
  }

  if(TimerFlag)printf("Time at the end of projection = %f seconds\n",timer2.CpuTime());
}


void WeightedPadProjection3(Double_t **padCharge, Double_t **padHeight, Double_t *Start, Double_t *Stop, Int_t Rmin, Int_t Rmax, Int_t Cmin, Int_t Cmax, Double_t threshold, Bool_t PadBin, Double_t* gdEdx, Double_t* gLatRange, Double_t* gRange)
{
  Bool_t TimerFlag=0;
  if(TimerFlag){
    TStopwatch timer2;
    printf("Starting projection3 timer\n");
    timer2.Start();
  }

  Double_t PI=3.1415926535897932384626433;
  Double_t deg=180./PI;
  Double_t rad=PI/180.;

  float VAngle,HAngle;

  VAngle=TMath::ATan((Stop[2]-Start[2])/TMath::Sqrt((Stop[0]-Start[0])*(Stop[0]-Start[0])+(Stop[1]-Start[1])*(Stop[1]-Start[1])))*deg;
  if(Stop[0]-Start[0]>0){
    HAngle=TMath::ATan((Stop[1]-Start[1])/(Stop[0]-Start[0]))*deg;
  }
  else{
    HAngle=180.+TMath::ATan((Stop[1]-Start[1])/(Stop[0]-Start[0]))*deg;
  }

  //cout<<"WeightedMethod, VAngle :"<<VAngle<<", HAngle :"<<HAngle<<endl;

  float PadSize=2.;
  float binSize;
  if(PadBin)binSize=PadSize/TMath::Cos(VAngle*rad)/TMath::Cos(HAngle*rad);
  else binSize=300/(float)NBinsGlobal;
  //cout<<"WeightedMethod, VAngle :"<<VAngle<<", HAngle :"<<HAngle<<" binSize "<<binSize<<endl;
  float sum[NBinsGlobal],sum2[NBinsGlobal];
  float xGSlice[NBinsGlobal],yGSlice[NBinsGlobal];
  int NSample[NBinsGlobal];//Number of pad sample in each slice
  float wsig[NBinsGlobal];

  const int NSamp=10; 
  float xGSample[NBinsGlobal][NSamp],yGSample[NBinsGlobal][NSamp],dQSample[NBinsGlobal][NSamp];//barycenter and charge of sample

  for(int i=0;i<NBinsGlobal;i++){
    gdEdx[i]=0;
    gLatRange[i]=0;
    gRange[i]=i*binSize+binSize/2;
    sum[i]=sum2[i]=xGSlice[i]=yGSlice[i]=wsig[i]=0;
    NSample[i]=0;
  }

  int NSlice;
  //cout<<NBinsGlobal<<" "<<binSize<<endl;

  TVector2 v2pad(Stop[0]-Start[0],Stop[1]-Start[1]);
  TVector2 vA,vB;
  float magpad=v2pad.Mod();

  float rA,rB; //projection of pad bottom left corner and top right corner (if Theta>0) on 3D fitted track

  float PointI[2];
  float Vertex[7][2],tmpVertex[7][2],PreVertex1[2],PreVertex2[2],PreVertex3[2];
  int VertexN;
  int FirstBin,LastBin;

  int VN;//Number of vertex for calculating Surface and barycenter
  float xG,yG;

  float dSi,dStot;
  float PadSide1[2],PadSide2[2];

  float dQtot,dQtotproj;
  float dQpad,dQtmp;
  dQtot=dQtotproj=0;

  for(int r=Rmin;r<Rmax+1;r++){//Rows
    for(int c=Cmin;c<Cmax+1;c++){ //columns
      float xPad=0.,yPad=0.,zPad=0.;
      if(padCharge[r][c] > threshold){
	xPad = (PadSize*c+PadSize/2.);
	yPad = (PadSize*r+PadSize/2.);
	zPad = padHeight[r][c];

	dQtot+=padCharge[r][c];

	PreVertex1[0]=PadSide1[0]=xPad-PadSize/2.; 
	PreVertex2[0]=PadSide2[0]=xPad+PadSize/2.;

	if(HAngle>=0){
	  PreVertex1[1]=PadSide1[1]=yPad-PadSize/2.; 
	  PreVertex2[1]=PadSide2[1]=yPad+PadSize/2.;
	}
	else if(HAngle<0){
	  PreVertex1[1]=PadSide1[1]=yPad+PadSize/2.; 
	  PreVertex2[1]=PadSide2[1]=yPad-PadSize/2.;
	}

	//Calculating the 2 extreme pad's edges projection on track to determine the number of slices 
	vA.Set(PadSide1[0]-Start[0],PadSide1[1]-Start[1]);
	vB.Set(PadSide2[0]-Start[0],PadSide2[1]-Start[1]);

	rA=vA*v2pad/magpad/TMath::Cos(VAngle*rad);
	rB=vB*v2pad/magpad/TMath::Cos(VAngle*rad);

	FirstBin=(int)(rA/binSize);
	LastBin=(int)(rB/binSize);

	NSlice=LastBin-FirstBin+1;

	if((rB/binSize-(float)(int)(rB/binSize))==0){NSlice--; cout<<"Projection of pad corner on a bin!!!"<<endl;}

	dQpad=0;
	dStot=0;

	if(NSlice==1){
	  dQtmp=padCharge[r][c];
	  gdEdx[FirstBin]+=dQtmp/binSize;
	  dQtotproj+=dQtmp;
	  dQpad+=dQtmp;

	  sum[FirstBin]+=dQtmp;
	  sum2[FirstBin]+=dQtmp*dQtmp;

	  xGSlice[FirstBin]+=xG*dQtmp;
	  yGSlice[FirstBin]+=yG*dQtmp;

	  dQSample[FirstBin][NSample[FirstBin]]=dQtmp;
	  xGSample[FirstBin][NSample[FirstBin]]=xG;
	  yGSample[FirstBin][NSample[FirstBin]]=yG;
	  NSample[FirstBin]++;
	}

	for(int s=FirstBin; s<LastBin; s++)//For each slice, we calculate the surface and barycenter from the polygones vertex
	  {
	    VN=0;
	    PointI[0]=Start[0]+binSize*TMath::Cos(VAngle*rad)*(s+1)*(Stop[0]-Start[0])/magpad; //Point I correspond to the bin point on the track
	    PointI[1]=Start[1]+binSize*TMath::Cos(VAngle*rad)*(s+1)*(Stop[1]-Start[1])/magpad;

	    //// Close Side Vertex ////
	    tmpVertex[VN][0]=Vertex[VN][0]=PreVertex1[0];
	    tmpVertex[VN][1]=Vertex[VN][1]=PreVertex1[1];
	    VN++;

	    tmpVertex[VN][1]=Vertex[VN][1]=PadSide1[1];
	    tmpVertex[VN][0]=Vertex[VN][0]=PointI[0]+(PointI[1]-Vertex[VN][1])*(Stop[1]-Start[1])/(Stop[0]-Start[0]);

	    if(Vertex[VN][0]>(xPad+PadSize/2)){
	      tmpVertex[VN][0]=Vertex[VN][0]=PadSide2[0];
	      tmpVertex[VN][1]=Vertex[VN][1]=PadSide1[1];
	      VN++;

	      tmpVertex[VN][0]=Vertex[VN][0]=xPad+PadSize/2; 
	      tmpVertex[VN][1]=Vertex[VN][1]=PointI[1]+(PointI[0]-Vertex[VN][0])*(Stop[0]-Start[0])/(Stop[1]-Start[1]);
	      //if(Vertex[VN][1]<(yPad-PadSize/2) || Vertex[VN][1]>(yPad+PadSize/2))cout<<"Problem with pad projection, close side, Col "<<c<<" Row "<<r<<endl;
	    }

	    PreVertex1[0]=Vertex[VN][0];
	    PreVertex1[1]=Vertex[VN][1];
	    VN++;

	    //// Further Side Vertex ////
	    tmpVertex[VN][1]=Vertex[VN][1]=PadSide2[1];
	    tmpVertex[VN][0]=Vertex[VN][0]=PointI[0]+(PointI[1]-Vertex[VN][1])*(Stop[1]-Start[1])/(Stop[0]-Start[0]);

	    if(Vertex[VN][0]<(xPad-PadSize/2)){
	      tmpVertex[VN][0]=Vertex[VN][0]=xPad-PadSize/2; 
	      tmpVertex[VN][1]=Vertex[VN][1]=PointI[1]+(PointI[0]-Vertex[VN][0])*(Stop[0]-Start[0])/(Stop[1]-Start[1]);
	      //if(Vertex[VN][1]<(yPad-PadSize/2) || Vertex[VN][1]>(yPad+PadSize/2))cout<<"Problem with pad projection, further side, Col "<<c<<" Row "<<r<<endl;
	    }

	    PreVertex2[0]=Vertex[VN][0];
	    PreVertex2[1]=Vertex[VN][1];
	    VN++;

	    if(Vertex[VN-1][1]==PadSide2[1] && (PreVertex3[1]!=PadSide2[1] || s==FirstBin)){
	      tmpVertex[VN][0]=Vertex[VN][0]=PadSide1[0];
	      tmpVertex[VN][1]=Vertex[VN][1]=PadSide2[1];
	      VN++;
	    }

	    if(s!=FirstBin){
	      tmpVertex[VN][0]=Vertex[VN][0]=PreVertex3[0];
	      tmpVertex[VN][1]=Vertex[VN][1]=PreVertex3[1];
	      VN++;
	    }

	    PreVertex3[0]=PreVertex2[0];
	    PreVertex3[1]=PreVertex2[1];

	    if(HAngle<0){
	      for(int i=0;i<VN;i++){
		Vertex[i][0]=tmpVertex[VN-1-i][0];
		Vertex[i][1]=tmpVertex[VN-1-i][1];
	      }
	    }

	    Vertex[VN][0]=Vertex[0][0];
	    Vertex[VN][1]=Vertex[0][1];

	    //// Surface and Barycenter ////
	    dSi=0;
	    xG=yG=0.;

	    for(int i=0;i<VN;i++){
	      dSi+=0.5*(Vertex[i][0]*Vertex[i+1][1]-Vertex[i+1][0]*Vertex[i][1]);
	    }

	    if(dSi>0){
	      for(int i=0;i<VN;i++){
		xG+=(Vertex[i][0]+Vertex[i+1][0])*(Vertex[i][0]*Vertex[i+1][1]-Vertex[i+1][0]*Vertex[i][1])/(6*dSi);
		yG+=(Vertex[i][1]+Vertex[i+1][1])*(Vertex[i][0]*Vertex[i+1][1]-Vertex[i+1][0]*Vertex[i][1])/(6*dSi);
	      }

	      dStot+=dSi;
	      dQtmp=dSi*padCharge[r][c]/PadSize/PadSize;
	      gdEdx[s]+=dQtmp/binSize;
	      dQtotproj+=dQtmp;
	      dQpad+=dQtmp;

	      sum[s]+=dQtmp;
	      sum2[s]+=dQtmp*dQtmp;

	      xGSlice[s]+=xG*dQtmp;
	      yGSlice[s]+=yG*dQtmp;

	      dQSample[s][NSample[s]]=dQtmp;
	      xGSample[s][NSample[s]]=xG;
	      yGSample[s][NSample[s]]=yG;
	      NSample[s]++;
	    }
	    //// For the last slice ////
	    if(s==LastBin-1){
	      VN=0;	      
	      tmpVertex[VN][0]=Vertex[VN][0]=PreVertex1[0];
	      tmpVertex[VN][1]=Vertex[VN][1]=PreVertex1[1];
	      VN++;

	      if(PreVertex1[1]==PadSide1[1]){
		tmpVertex[VN][0]=Vertex[VN][0]=PadSide2[0];
		tmpVertex[VN][1]=Vertex[VN][1]=PadSide1[1];
		VN++;
	      }

	      tmpVertex[VN][0]=Vertex[VN][0]=PadSide2[0];
	      tmpVertex[VN][1]=Vertex[VN][1]=PadSide2[1];
	      VN++;

	      tmpVertex[VN][0]=Vertex[VN][0]=PreVertex2[0];
	      tmpVertex[VN][1]=Vertex[VN][1]=PreVertex2[1];
	      VN++;

	      if(HAngle<0){
		for(int i=0;i<VN;i++){
		  Vertex[i][0]=tmpVertex[VN-1-i][0];
		  Vertex[i][1]=tmpVertex[VN-1-i][1];
		}
	      }

	      Vertex[VN][0]=Vertex[0][0];
	      Vertex[VN][1]=Vertex[0][1];

	      dSi=0;
	      xG=yG=0.;
	      for(int i=0;i<VN;i++){
		dSi+=0.5*(Vertex[i][0]*Vertex[i+1][1]-Vertex[i+1][0]*Vertex[i][1]);
	      }

	      if(dSi>0){
		for(int i=0;i<VN;i++){
		  xG+=(Vertex[i][0]+Vertex[i+1][0])*(Vertex[i][0]*Vertex[i+1][1]-Vertex[i+1][0]*Vertex[i][1])/(6*dSi);
		  yG+=(Vertex[i][1]+Vertex[i+1][1])*(Vertex[i][0]*Vertex[i+1][1]-Vertex[i+1][0]*Vertex[i][1])/(6*dSi);
		}

		dStot+=dSi;
		dQtmp=dSi*padCharge[r][c]/PadSize/PadSize;
		gdEdx[s+1]+=dQtmp/binSize;
		dQtotproj+=dQtmp;
		dQpad+=dQtmp;

		sum[s+1]+=dQtmp;
		sum2[s+1]+=dQtmp*dQtmp;

		xGSlice[s+1]+=xG*dQtmp;
		yGSlice[s+1]+=yG*dQtmp;

		dQSample[s+1][NSample[s+1]]=dQtmp;
		xGSample[s+1][NSample[s+1]]=xG;
		yGSample[s+1][NSample[s+1]]=yG;
		NSample[s+1]++;
	      }
	    }
	  }
	//if(TMath::Abs(dQpad-padCharge[r][c])>0.001)cout<<"Col "<<c<<" Row "<<r<<" PadCharge "<<padCharge[r][c]<<" Qproj "<<dQpad<<" NSlice "<<NSlice<<endl;
      }	    
    }
  }//End of Loop on pads
  //cout<<"dQtot "<<dQtot<<" dQtotproj "<<dQtotproj<<endl;

  for(int i=0;i<NBinsGlobal;i++){
    if(NSample[i]>1){
      xGSlice[i]/=sum[i];
      yGSlice[i]/=sum[i];
      for(int s=0;s<NSample[i];s++){
	wsig[i]+=dQSample[i][s]*((xGSample[i][s]-xGSlice[i])*(xGSample[i][s]-xGSlice[i])+(yGSample[i][s]-yGSlice[i])*(yGSample[i][s]-yGSlice[i]));
      }
      wsig[i]=TMath::Sqrt(wsig[i]/(sum[i]-sum2[i]/sum[i]));
    }
    gLatRange[i]=wsig[i];
  }

  if(TimerFlag)printf("Time at the end of projection3 = %f seconds\n",timer2.CpuTime());

  //ProjCPUTime+=timer2.CpuTime();
  //cout<<"ProjCPUTime "<<ProjCPUTime<<endl;
}


Double_t par[8],parerr[8];
Double_t nc1[NBinsGlobal];
Double_t x[NBinsGlobal];
Int_t nbins;
const Int_t npar=2;
Double_t chisquare=0;

bool quietfit=0;

void chi2(Int_t &npar, Double_t *gin, Double_t &result, Double_t *par, Int_t flg){
 
  Double_t chisq=0.0;
  Int_t entry=0;
  Double_t err_nc=0.0;
  Int_t NPoints=0;  

  //Loop over all bins  
  for (Int_t ibin=0; ibin < NBinsGlobal; ibin++) 
    {
      Double_t err_nc=0.0;
      Double_t fac=0;
      Double_t fac2=0;
      Int_t j=0;    

      //if(nc1[ibin]!=0 && x[ibin]>100){err_nc=(1.0/nc1[ibin]); NPoints++;}
      if(nc1[ibin]>0.){err_nc=(1.0/nc1[ibin]); NPoints++;}
      else {err_nc=0.0;}
      //else {err_nc=1.0;}
      //if(nc1[]!=0){err_nc=(1.0/bragR->Eval(val));}
      //else {err_nc=1.0;}    

      //fac=(nc1[ibin]-nc_sum(par,ibin));
      //fac=bragR->Eval(val)-par[1]*simbragR->Eval(val-par[0]);
      //fac=nc1[ibin]-par[0]*nc2[ibin+(int*)par[1]];
      //fac=nc1[ibin]-par[0]*nc2[ibin+970];
      //fac=nc1[ibin]-par[0]*simbragRGlobal->Eval((double)ibin/10+par[1]);

      //fac=nc1[ibin]-par[0]*simbragRGlobal->Eval(x[ibin]+par[1]);

      fac=nc1[ibin]-par[0]*simbragRGlobal->Eval(x[ibin]+RShiftGlobal);
      fac2=nc1[ibin]-multiGlobal*simbragRGlobal->Eval(x[ibin]+par[1]);

      //if(nc1[ibin]>0.)cout<<ibin<<"  "<<par[0]<<"  "<<par[1]<<"  "<<fac<<"   "<<fac2<<"  "<<nc1[ibin]<<"  "<<err_nc<<"   "<<(fac*fac+fac2*fac2)*err_nc<<endl;

      //chisq+=(fac*fac)*err_nc;
      //chisq+=(fac2*fac2)*err_nc;	
      chisq+=(fac*fac+fac2*fac2)*err_nc;
      if(chisq<0)cout<<"ACHTUNG "<<ibin<<" nc1[ibin] "<<nc1[ibin]<<"   "<<fac<<"   "<<fac2<<"  "<<"  "<<err_nc<<"   "<<(fac*fac+fac2*fac2)*err_nc<<endl;	    
    }

  //cout<<"chisq=  "<<chisq<<endl;
  //cout<<"chisq=  "<<chisq<<" NPoints=  "<<NPoints<<" chisq/(NPoints-2)=  "<<chisq/(NPoints-2)<<" par[0] "<<par[0]<<" par[1] "<<par[1]<<endl;
  //cout<<"chisq=  "<<chisq<<" NPoints=  "<<NPoints<<" chisq/(NPoints-2)=  "<<chisq/(NPoints-2)<<endl;
  //if(NPoints!=0)chisquare=result=chisq/NPoints;
  //else chisquare=result=chisq;
  //if((NPoints-2)>0)chisquare=result=chisq/(NPoints-2);
  //else chisquare=result=chisq;
  chisquare=result=chisq;
}

int NumericalMinimization(double *gdEdx, double *gRange, double RShift0, double mult0, double &a, double &b)
{  
  for(Int_t i=0;i<NBinsGlobal;i++)
    {
      //cout <<"-------------------"<<endl;
      //cout <<"Bin= "<<i<<endl;
      //x[i] = xaxis->GetBinCenter(i);
      x[i] = gRange[i];
      //cout <<"X= "<<x[i]<<endl;
      nc1[i] = gdEdx[i];   
      //cout <<"Nc1= "<<nc1[i]<<endl;  
      //cout <<i<<" X= "<<x[i]<<" Nc1= "<<nc1[i]<<endl;
      //nc2[i]=hsimBragg->GetBinContent(i);  
      //cout <<"Nc2= "<<nc2[i]<<endl;      
      //cout <<"-------------------"<<endl; 
    }

  //////////////////////////////////////
  //Minimisation 
  //////////////////////////////////////

  if(quietfit){
    TStopwatch timer;
    printf("Starting timer\n");
    timer.Start();
  }

  //Number of parameters to minimize: 5 for the bg and 3 for normalisation of the transitions
  TMinuit *min = new TMinuit(npar);   //  New minimization class for up to 8 variables/parameters
 
  min->SetPrintLevel(-1);

  if(quietfit)cout <<"set chi2  "<<endl;
  
   min->SetFCN(chi2);                //  Tell Minuit about the function (above)	  

  if(quietfit)cout <<"end chi2 "<<endl;
  
  Double_t *arglist=new Double_t[npar*2];
  Int_t ierflg = 0;                //  Needed if you want to check for errors

  //Set parameters
  //min->mnparm(0,"first",value,step,min,max,ierr);
  //min->mnparm(1,"second",value,step,min,max,ierr);
  
  //The mnparm methods define new parameters. The first argument numbers them starting from zero. The second argument gives the parameter a name that you can recognize in the printout that Minuit generates. The other parameters are:
  
  //* value - The initial value of the parameter
  //* step - How far Minuit will move away from the initial value when calculating numerical derivatives.
  //* min - If the parameter is bounded, this specifies the minimum possible value
  //* max - If the parameter is bounded, this specifies the maximum possible value
  //* Set min=max=0 if the parameters are not bounded.  


 //  min->mnparm(0,"X0",3.,0.1,0,0,ierflg);
//   min->mnparm(1,"Y0",0.5,0.1,0,0,ierflg); 

  Char_t pname[20]; 
  TString namepar;
  
  Double_t ival[npar]={mult0,RShift0}; 
  Double_t step[npar]={100,0.5}; 
  Double_t bmin[npar]={mult0-2000,RShift0-15}; 
  Double_t bmax[npar]={mult0+2000,RShift0+15}; 
  
  for(Int_t i=0;i<npar;i++)
    {
      if(quietfit)sprintf(pname,"p%d",i);
    
      //min->mnparm(i,namepar,ival[i],0.001,bmin[i],bmax[i],ierflg);
      //min->mnparm(i,namepar,ival[i],0.1,bmin[i],bmax[i],ierflg);
      min->mnparm(i,namepar,ival[i],step[i],bmin[i],bmax[i],ierflg);
    }

  // arglist[0] = maximum number of iterations 500
  // arglist[1] = number 2
  // ierflag = ??

  arglist[0] = 1;
  min->mnexcm("SET ERR", arglist ,1,ierflg);
 
  //Call the minimisation algorith MIGRAD
  // arglist[0] = maximum number of iterations 500
  // arglist[1] = edm //error distance to minimum assuming a parabole
  // ierflag = ??
  
  arglist[0] = 500;
  arglist[1] = 1.;

  if(quietfit)cout<<"    START: Minimisation procedure" <<endl;
  min->mnexcm("MIGRAD",arglist,2,ierflg);  
  
  //min->Migrad();
  
  for (i=0; i<npar; i++) min->GetParameter(i,par[i],parerr[i]);
 
  a=par[0];
  b=par[1];
 
  if(quietfit)printf("Time at the end of job = %f seconds\n",timer.CpuTime());

  return 0;
}
