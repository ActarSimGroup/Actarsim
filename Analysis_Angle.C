///////////////////////////////////////////////////////////////////
//*-- AUTHOR : Piotr Konczykowski
//*-- Date: 05/2016
//
// --------------------------------------------------------------
// This macro compares Horizontal and Vertical angles calculate
// by the steps information (GEANT4) or the pad's ones (digitization)
//
//
// --------------------------------------------------------------
// How to run this program:
// 1 - Run the simulation
// 2 - Run the digitization
// 3 - Run this macro inside root
//      .L Analysis_Angle.C;
//      Run()
// --------------------------------------------------------------
/////////////////////////////////////////////////////////////////



#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TF1.h"
#include "TFile.h"
#include "TTree.h"
#include "TProfile.h"
#include "TClonesArray.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TGraph.h"
#include "TSpline.h"

#include "TVector3.h"
#include "MTrack.h"
//#include "digit_piotr.h"
//#include "digit.h"
//#include "include/ActarSimPrimaryInfo.hh"
#include <iostream>

Int_t npeaks=20;

//void Run(Char_t *name="root_files/simFile.root",Char_t *name2="dig_files/digFile.root",Int_t event=0){
void Run(){

  Int_t event=0;

  //gROOT->ProcessLine(".L digit.h+");
  //gROOT->ProcessLine(".L digit_piotr.h+");

  //gSystem->Load("digit_david8_h.so");
  gSystem->Load("libactar.sl");
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1);

  //Flags
  Bool_t TrackVisuFlag=1;
  Bool_t TrackBragFlag=0;

  Bool_t RangeFlag=0;
  Bool_t DEdepFlag=0;
  Bool_t AngleFlag=1;
	
  Int_t NRun=200;
  const Int_t NHAngle=6;
  const Int_t NVAngle=3;

  Double_t StopPoint=2.;//Stopping point will be MaxValue/StopPoint

  gROOT->ProcessLine(".L digit.h+");

  padsGeometry thePadsGeometry;
  thePadsGeometry.SetGeometryValues(0.,0.,0.,155.,150.,105.,0.,0.,0.,2.,5.,5.);//For Phi and Theta study
  //thePadsGeometry.SetGeometryValues(0.,0.,0.,37.,150.,105.,0.,0.,0.,2.,5.,5.);//For Phi study
  //thePadsGeometry.SetGeometryValues(0.,0.,0.,105.,85.,105.,0.,0.,0.,2.,5.,5.);//For Theta study
  //thePadsGeometry.SetGeometryValues(0.,0.,0.,37.,85.,133.,0.,0.,0.,2.,5.,5.);
  //thePadsGeometry.SetGeometryValues(0,0,0,151.,151.,150,151.,2); //Rectangular Pads 30x30x30 cm^3 2mm pads
  
  driftManager theDriftManager;

  Double_t v_drift,sigma_trans,sigma_long;
  Int_t nentries;

  //theDriftManager.SetDriftVelocity(v_drift);  // in mm/ns
  //theDriftManager.SetDiffusionParameters(sigma_long,sigma_trans); // in mm^2/ns 

  //He:iC4H10 (9:1) 122.94 V/cm
  //theDriftManager.SetDriftVelocity(6.865e-3);
  //theDriftManager.SetDiffusionParameters(2.369e-5,2.892e-5);

  //105 mbar iC4H10, E=2150/17
  //theDriftManager.SetDriftVelocity(36.8e-3);
  //theDriftManager.SetDiffusionParameters(18.89e-5,25.83e-5);

  //1 atm iC4H10, E=2150/17
  //theDriftManager.SetDriftVelocity(3.432e-3);
  //theDriftManager.SetDiffusionParameters(0.8179e-5,0.7397e-5);


  //#######iC4H10 (105.9mbar) Gas MagBoltz Values E=2150/17#########
  theDriftManager.SetDriftVelocity(36.43e-3);
  theDriftManager.SetDiffusionParameters(20.62e-5,24.94e-5);

  cout<<"Drift Parameters are:"<<endl;  
  cout<<"v_drift---------> "<<theDriftManager.GetDriftVelocity()<<"mm/ns"<<endl;  
  cout<<"D_long----------> "<<theDriftManager.GetLongitudinalDiffusion()<<"mm^2/ns"<<endl;  
  cout<<"D_trans---------> "<<theDriftManager.GetTransversalDiffusion()<<"mm^2/ns"<<endl;  

  Double_t driftVelocity=theDriftManager.GetDriftVelocity();

  Double_t xBox=thePadsGeometry.GetXLength();
  Double_t yBox=thePadsGeometry.GetYLength();
  Double_t zBox=thePadsGeometry.GetZLength();

  cout<<"xBox "<<xBox<<" yBox "<<yBox<<" zBox "<<zBox<<endl;

  Double_t PI=3.1415926535897932384626433;
  Double_t deg=180./PI;
  Double_t rad=PI/180.;
  //==================================================================================//


  for(const Int_t t=0;t<2;t++)MTrack* T[t]=new MTrack();

  Double_t Start[3],Out[3],End[3],Stop[3];
  Double_t TStart[3],TStop[3];
  Double_t StrideStart[3],StrideStop[3],StrideStartInit[3],StrideStopInit[3],StrideStartFin[3],StrideStopFin[3];
  Double_t StopX[3],StopY[3];
  Double_t StartTrack[3]={150,10,150};
  Double_t Qtot;
  Double_t a,b;
  Double_t astride,bstride;


  Double_t IniPoint[2][2],FinPoint[2][2],IniFitPoint[2][2],FinFitPoint[2][2];
  Double_t IniPoint1[2],FinPoint1[2],IniFitPoint1[2],FinFitPoint1[2];
  Double_t IniPoint2[2],FinPoint2[2],IniFitPoint2[2],FinFitPoint2[2];

  Double_t PointA[3],PointB[3];
  Double_t range;
  Double_t RangeA,RangeB,RangeAB;
  Double_t RangeX,RangeY,RangeXY,RangeW1,RangeW2,RangeW3,RangeP1,RangeP2,RangeP3,RangeG4;

  Double_t DeltaRXY_M[NHAngle][NVAngle],DeltaRXY_S[NHAngle][NVAngle];
  Double_t DeltaRP1_M[NHAngle][NVAngle],DeltaRP2_M[NHAngle][NVAngle],DeltaRP3_M[NHAngle][NVAngle];
  Double_t DeltaRP1_S[NHAngle][NVAngle],DeltaRP2_S[NHAngle][NVAngle],DeltaRP3_S[NHAngle][NVAngle];
  Double_t DeltaRW1_M[NHAngle][NVAngle],DeltaRW2_M[NHAngle][NVAngle],DeltaRW3_M[NHAngle][NVAngle];
  Double_t DeltaRW1_S[NHAngle][NVAngle],DeltaRW2_S[NHAngle][NVAngle],DeltaRW3_S[NHAngle][NVAngle];

  Double_t phi;
  Double_t theta;
  Double_t VAngle;
  Double_t HAngle;
  Double_t StrideHAngle,TotStrideHAngle;
  Double_t Angle3D;
  Double_t LazyG4HAngle;

  TSpline3 *bragRP2;
  TSpline3 *bragRP3;
  TSpline3 *bragRW;
  TSpline3 *bragX;
  TSpline3 *bragY;

  TSpline3 *bragR1;
  TSpline3 *bragR2;

  Double_t padSize  = thePadsGeometry.GetPadSize();
  //Int_t    padType  = thePadsGeometry.GetPadType();
  //Int_t    padLayout= thePadsGeometry.GetPadLayout();
  //Double_t sideBlankSpace = thePadsGeometry.GetSideBlankSpace();
  // Double_t rHexagon = thePadsGeometry.GetRHexagon();

  Double_t tracked_position;
  Double_t tracked_positionV;
 
  const Int_t numberOfRows   = thePadsGeometry.GetNumberOfRows();
  const Int_t numberOfColumns= thePadsGeometry.GetNumberOfColumns();

  cout<<"Number of Rows "<<numberOfRows<<" Number of Columns "<<numberOfColumns<<endl;

  //Int_t Rmin=0, Rmax=31;
  //Int_t Cmin=0, Cmax=127; //Cmax=63;

  Int_t Rmin=0, Rmax=numberOfRows-1;
  Int_t Cmin=0, Cmax=numberOfColumns-1; //Cmax=63;


  //Char_t *name="root_files/sim_files/simFile.root";
  //Char_t *name="root_files/sim_files/simFile_3alpha_iC4H10_105mbar_VA0_z100.root";
  //Char_t *name="root_files/sim_files/simFile_3alpha_iC4H10_105mbar_HA-80_-70_-60_-50_-45_-40_-30_-20_-10.root";
  //Char_t *name="root_files/sim_files/simFile_3alpha_iC4H10_105mbar_HA80_70_60_50_45_40_30_20_10.root";
  //Char_t *name="root_files/sim_files/simFile_gasBox37x150x105_3alpha_iC4H10_105mbar_VA0.root";
  //Char_t *name="root_files/sim_files/simFile_gasBox37x150x105_3alpha_iC4H10_105mbar_VA-80_-70_-60_-50_-45_-40_-30_-20_-10.root";
  //Char_t *name="root_files/sim_files/simFile_gasBox37x150x105_3alpha_iC4H10_105mbar_VA80_70_60_50_45_40_30_20_10.root";
  //Char_t *name="root_files/sim_files/simFile_gasBox105x85x105_alpha_5.8MeV_VA0.root";
  //Char_t *name="root_files/sim_files/simFile_gasBox105x85x105_alpha_5.8MeV_VA0_pos_x1_y15_z-90.root";
  //Char_t *name="root_files/sim_files/simFile_gasBox155x150x105_alpha_5.8MeV_VA0.root";
  //Char_t *name="root_files/sim_files/simFile_gasBox155x150x105_alpha_5.8MeV_HA-80to80.root";
  Char_t *name="root_files/sim_files/simFile_gasBox155x150x105_alpha_5.8MeV_HA-80to80_2000.root";
  //Char_t *name="root_files/sim_files/simFile_gasBox155x150x105_alpha_5.8MeV_HA-75to75.root";
  //Char_t *name="root_files/sim_files/simFile_gasBox155x150x105_alpha_5.8MeV_HA-75to75_2000.root";
  //Char_t *name="root_files/sim_files/simFile_gasBox155x150x105_alpha_5.8MeV_HA0x5.root";

  //Char_t *name2="root_files/dig_files/digFile.root";
  //Char_t *name2="root_files/dig_files/digFile_3alpha_iC4H10_105mbar_VA0_z100.root";
  //Char_t *name2="root_files/dig_files/digFile_3alpha_iC4H10_105mbar_HA-80_-70_-60_-50_-45_-40_-30_-20_-10.root";
  //Char_t *name2="root_files/dig_files/digFile_3alpha_iC4H10_105mbar_HA80_70_60_50_45_40_30_20_10.root";
  //Char_t *name2="root_files/dig_files/digFile_gasBox37x150x105_3alpha_iC4H10_105mbar_VA0.root";
  //Char_t *name2="root_files/dig_files/digFile_gasBox37x150x105_3alpha_iC4H10_105mbar_VA-80_-70_-60_-50_-45_-40_-30_-20_-10.root";
  //Char_t *name2="root_files/dig_files/digFile_gasBox37x150x105_3alpha_iC4H10_105mbar_VA80_70_60_50_45_40_30_20_10.root";
  //Char_t *name2="root_files/dig_files/digFile_gasBox105x85x105_alpha_5.8MeV_VA0.root";
  //Char_t *name2="root_files/dig_files/digFile_gasBox105x85x105_alpha_5.8MeV_VA0_piotrdigit.root";
  //Char_t *name2="root_files/dig_files/digFile_gasBox105x85x105_alpha_5.8MeV_VA0_centerdigit.root";
  //Char_t *name2="root_files/dig_files/digFile_gasBox105x85x105_alpha_5.8MeV_VA0_pos_x1_y15_z-90.root";
  //Char_t *name2="root_files/dig_files/digFile_gasBox155x150x105_alpha_5.8MeV_VA0.root";
  //Char_t *name2="root_files/dig_files/digFile_gasBox155x150x105_alpha_5.8MeV_HA-80to80.root";
  Char_t *name2="root_files/dig_files/digFile_gasBox155x150x105_alpha_5.8MeV_HA-80to80_2000.root";
  //Char_t *name2="root_files/dig_files/digFile_gasBox155x150x105_alpha_5.8MeV_HA-75to75.root";
  //Char_t *name2="root_files/dig_files/digFile_gasBox155x150x105_alpha_5.8MeV_HA-75to75_2000.root";
  //Char_t *name2="root_files/dig_files/digFile_gasBox155x150x105_alpha_5.8MeV_HA0x5.root";

  //Event info;
  TFile *simFile=new TFile(name);
  cout<<"Opening simFile : "<<name<<endl;
  //digitization data, output of digitizationMacro
  TFile *digFile=new TFile(name2);
  cout<<"Opening digFile : "<<name2<<endl;
  if(!digFile) 
    return;
  else{
    //getting the trees
    TTree *simTree=(TTree*)simFile->Get("The_ACTAR_Event_Tree");
    TTree *digTree=(TTree*)digFile->Get("digiTree");
    if(event==0)
      nentries=digTree->GetEntries();
    else
      nentries=event;
    cout<<"digit entries: "<<nentries<<endl;
    Float_t *ta=new Float_t[nentries];
    Float_t *tb=new Float_t[nentries];

    //BeamInfo Branch
    ActarSimBeamInfo *beamInfo = 0; // beamInfo MUST be initialized to point to a NULL or valid object
    simTree->SetBranchAddress("beamInfo",&beamInfo);

    //PrimaryInfoCA
    TClonesArray *primaryInfoCA=new TClonesArray("ActarSimPrimaryInfo",1);
    TBranch *branchPrimaryInfo=simTree->GetBranch("primaryInfo");
    branchPrimaryInfo->SetAddress(&primaryInfoCA);
    branchPrimaryInfo->SetAutoDelete(kTRUE);
    ActarSimPrimaryInfo *thePrimaryInfo=new ActarSimPrimaryInfo;
    //ClonesArray to the tracks
    TClonesArray *simpleTrackCA=new TClonesArray("ActarSimSimpleTrack",400);
    TBranch *branchSimpleTrack= simTree->GetBranch("simpleTrackData");
    branchSimpleTrack->SetAddress(&simpleTrackCA);
    branchSimpleTrack->SetAutoDelete(kTRUE);
    ActarSimSimpleTrack *simpleTrack=new ActarSimSimpleTrack;
    /* //ClonesArray to the silicon Hits
       TClonesArray *silHitsCA=new TClonesArray("ActarSimSilHit",200);
       TBranch *branchSilHits=simTree->GetBranch("silHits");
       //branchSilHits->SetAddress(&silHitsCA);
       //branchSilHits->SetAutoDelete(kTRUE);
       ActarSimSilHit *silHit=new ActarSimSilHit;*/
    
    //ClonesArray to the signal
    TClonesArray *padSignalCA=new TClonesArray("ActarPadSignal",4000);
    digTree->SetBranchAddress("padSignals",&padSignalCA);
    ActarPadSignal *padSignal=new ActarPadSignal;

    TGraph *SimTrack=new TGraph();
    TGraph *SimTrackE=new TGraph();

    TGraph *G4EdepGraph=new TGraph();

    TH1F *h_G4Edep=new TH1F("G4Track","GEANT4 Track Bragg",100, 0.,200.);
    TH2F *h_G4Edep_2=new TH2F("G4Track2","GEANT4 Track Bragg 2",100, 0.,200.,100,0.,0.5);

    TH1F *h_braggP1=new TH1F("TrackBraggP1","Track Bragg, Projection on track, bin=1mm",200, 0.,200.);
    TH1F *h_braggP2=new TH1F("TrackBraggP2","Track Bragg, Projection on track, bin=2mm",100, 0.,200.);
    TH1F *h_braggP3=new TH1F("TrackBraggP3","Track Bragg, Projection on track, bin=3mm",100, 0.,300.);

    TH1F *h_braggW1=new TH1F("TrackBraggW1","Track Bragg, Projection on track proportional to pads size, bin=1mm",200, 0.,200.);
    TH1F *h_braggW2=new TH1F("TrackBraggW2","Track Bragg, Projection on track proportional to pads size, bin=2mm",100, 0.,200.);
    TH1F *h_braggW3=new TH1F("TrackBraggW3","Track Bragg, Projection on track proportional to pads size, bin=3mm",100, 0.,300.);

    TH1F *h_braggX=new TH1F("TrackBraggX","Track Bragg, X projection",100, 0.,200.);
    TH1F *h_braggY=new TH1F("TrackBraggY","Track Bragg, Y projection",100, 0.,200.);

    TH1F *h_braggTest=new TH1F("TrackBraggTest","Track Bragg Test",10, 0.,10.);

    TH1F *h_DeltaRXY=new TH1F("h_DeltaRXY","Range_{Rec}- Range_{GEANT4}",600, -30.,30.);
    TH1F *h_DeltaRW1=new TH1F("h_DeltaRW1","Range_{Rec}- Range_{GEANT4}",600, -30.,30.);
    TH1F *h_DeltaRW2=new TH1F("h_DeltaRW2","Range_{Rec}- Range_{GEANT4}",600, -30.,30.);
    TH1F *h_DeltaRW3=new TH1F("h_DeltaRW3","Range_{Rec}- Range_{GEANT4}",600, -30.,30.);
    TH1F *h_DeltaRP1=new TH1F("h_DeltaRP1","Range_{Rec}- Range_{GEANT4}",600, -30.,30.);
    TH1F *h_DeltaRP2=new TH1F("h_DeltaRP2","Range_{Rec}- Range_{GEANT4}",600, -30.,30.);
    TH1F *h_DeltaRP3=new TH1F("h_DeltaRP3","Range_{Rec}- Range_{GEANT4}",600, -30.,30.);

    TH1F *h_DEdepR=new TH1F("h_DEdepR","E_{dep}(sim)-E_{dep}(calc), Projection on track, bin=2mm",100, -1.,1.);
    TH1F *h_DEdepR45=new TH1F("h_DEdepR45","E_{dep}(sim)-E_{dep}(calc), Projection on track, bin=2#sqrt{2}mm",100, -1.,1.);
    TH1F *h_DEdepRPiotr=new TH1F("h_DEdepRPiotr","E_{dep}(sim)-E_{dep}(calc), Projection on track proportional to pads size, bin=2mm",100, -1.,1.);
    TH1F *h_DEdepX=new TH1F("h_DEdepX","E_{dep}(sim)-E_{dep}(calc), X proj",100, -1.,1.);
    TH1F *h_DEdepY=new TH1F("h_DEdepY","E_{dep}(sim)-E_{dep}(calc), Y proj",100, -5.,5.);
    TH1F *h_DEdepXY=new TH1F("h_DEdepXY","E_{dep}(sim)-E_{dep}(calc), XY proj",100, -1.,1.);

    TH1F *h_DeltaVAngle=new TH1F("h_DeltaVAngle","#phi_{GEANT4}-#phi_{REC}",1000, -0.5,0.5);
    //TH1F *h_DeltaHAngle=new TH1F("h_DeltaHAngle","#theta_{GEANT4}(from each stride, weighted by length)-#theta_{REC}",100, -5.,5.);
    TH1F *h_DeltaHAngle=new TH1F("h_DeltaHAngle","#theta_{GEANT4}(from each stride)-#theta_{REC}",600, -0.1,0.1);
    TH1F *h_TotDeltaHAngle=new TH1F("h_TotDeltaHAngle","#theta_{GEANT4}-#theta_{REC}",300, -5.,5.);
    TH1F *h_DeltaLazyHAngle=new TH1F("h_DeltaLazyHAngle","#theta_{GEANT4}(from starting and stopping points)-#theta_{REC}",300, -5.,5.);

    TH1F *hHorizAngle=new TH1F("hHorizAngle","Horizontal Angle",360,-90,90);
    TH1F *hVertiAngle=new TH1F("hVertiAngle","Vertical Angle",360,-90,90);

    TH1F *h_HAngle=new TH1F("h_HAngle","#theta_{REC}",300, -5.,5.);
    TH1F *h_TotHAngle=new TH1F("h_TotHAngle","#theta_{GEANT4}",300, -5.,5.);

    TString MultiDeltaHAngleName[17];
    TString MultiDeltaHAngle[17];
    TH1F *h_MultiDeltaHAngle[17];

    for(Int_t i=0; i<17; i++)
      {
	MultiDeltaHAngleName[i]=TString::Format("h_MultiDeltaHAngle%d",i*10-80);
	MultiDeltaHAngle[i]=TString::Format("#theta_{GEANT4}-#theta_{REC}, #theta beam = %d deg",i*10-80);
	h_MultiDeltaHAngle[i]=new TH1F(MultiDeltaHAngleName[i],MultiDeltaHAngle[i],600, -0.1,0.1);
      }

    TH1F *h_DeltaVAngle80=new TH1F("h_DeltaVAngle80","#theta_{GEANT4}-#theta_{REC}, #theta beam = 80 deg",200, -0.5,1);
    TH1F *h_DeltaVAngle70=new TH1F("h_DeltaVAngle70","#theta_{GEANT4}-#theta_{REC}, #theta beam = 70 deg",200, -0.5,1);
    TH1F *h_DeltaVAngle60=new TH1F("h_DeltaVAngle60","#theta_{GEANT4}-#theta_{REC}, #theta beam = 60 deg",200, -0.5,1);
    TH1F *h_DeltaVAngle50=new TH1F("h_DeltaVAngle50","#theta_{GEANT4}-#theta_{REC}, #theta beam = 50 deg",200, -0.5,1);
    TH1F *h_DeltaVAngle45=new TH1F("h_DeltaVAngle45","#theta_{GEANT4}-#theta_{REC}, #theta beam = 45 deg",200, -0.5,1);
    TH1F *h_DeltaVAngle40=new TH1F("h_DeltaVAngle40","#theta_{GEANT4}-#theta_{REC}, #theta beam = 40 deg",200, -0.5,1);
    TH1F *h_DeltaVAngle30=new TH1F("h_DeltaVAngle30","#theta_{GEANT4}-#theta_{REC}, #theta beam = 30 deg",200, -0.5,1);
    TH1F *h_DeltaVAngle20=new TH1F("h_DeltaVAngle20","#theta_{GEANT4}-#theta_{REC}, #theta beam = 20 deg",200, -0.5,1);
    TH1F *h_DeltaVAngle10=new TH1F("h_DeltaVAngle10","#theta_{GEANT4}-#theta_{REC}, #theta beam = 10 deg",200, -0.5,1);

    TH1F *h_DeltaVAnglem80=new TH1F("h_DeltaVAnglem80","#theta_{GEANT4}-#theta_{REC}, #theta beam = -80 deg",500, -0.4,0.);
    TH1F *h_DeltaVAnglem70=new TH1F("h_DeltaVAnglem70","#theta_{GEANT4}-#theta_{REC}, #theta beam = -70 deg",500, -0.4,0.);
    TH1F *h_DeltaVAnglem60=new TH1F("h_DeltaVAnglem60","#theta_{GEANT4}-#theta_{REC}, #theta beam = -60 deg",500, -0.4,0.);
    TH1F *h_DeltaVAnglem50=new TH1F("h_DeltaVAnglem50","#theta_{GEANT4}-#theta_{REC}, #theta beam = -50 deg",500, -0.4,0.);
    TH1F *h_DeltaVAnglem45=new TH1F("h_DeltaVAnglem45","#theta_{GEANT4}-#theta_{REC}, #theta beam = -45 deg",500, -0.4,0.);
    TH1F *h_DeltaVAnglem40=new TH1F("h_DeltaVAnglem40","#theta_{GEANT4}-#theta_{REC}, #theta beam = -40 deg",500, -0.4,0.);
    TH1F *h_DeltaVAnglem30=new TH1F("h_DeltaVAnglem30","#theta_{GEANT4}-#theta_{REC}, #theta beam = -30 deg",500, -0.4,0.);
    TH1F *h_DeltaVAnglem20=new TH1F("h_DeltaVAnglem20","#theta_{GEANT4}-#theta_{REC}, #theta beam = -20 deg",500, -0.4,0.);
    TH1F *h_DeltaVAnglem10=new TH1F("h_DeltaVAnglem10","#theta_{GEANT4}-#theta_{REC}, #theta beam = -10 deg",500, -0.4,0.);

    //TH2F *visu_charge=new TH2F("visu_charge","visu_charge",64,0,64,32,0,32);
    //TH2F *visu_time=new TH2F("visu_time","visu_time",64,0,64,32,0,32);
    //TH2F *visu_charge=new TH2F("visu_charge","visu_charge",128,0,128,32,0,32);
    //TH2F *visu_time=new TH2F("visu_time","visu_time",128,0,128,32,0,32);

    TH2F *visu_charge=new TH2F("visu_charge","visu_charge",numberOfColumns,0,numberOfColumns,numberOfRows,0,numberOfRows);
    TH2F *visu_time=new TH2F("visu_time","visu_time",numberOfColumns,0,numberOfColumns,numberOfRows,0,numberOfRows);

    //Double_t threshold = 0.0034;
    Double_t threshold = 0.;
    Double_t thresholdADC = threshold*1e9/30;
    Double_t sigma_time=3;//ns
    Double_t dE_sim=0;

    //Matrix for the charge map
    Double_t **padCharge=new Double_t*[numberOfRows];
    Double_t **padTime=new Double_t*[numberOfRows];
    Double_t **padHeight=new Double_t*[numberOfRows];
    for(Int_t i=0;i<numberOfRows;i++){
      padCharge[i]=new Double_t[numberOfColumns];
      padTime[i]=new Double_t[numberOfColumns];
      padHeight[i]=new Double_t[numberOfColumns];
    }

    if(TrackBragFlag){
      Can_brag=new TCanvas("Can_brag","Can_brag",900,600);
      Can_brag->Divide(3,3);
    }

    if(TrackVisuFlag){
      //Can_sil=new TCanvas("Can_sil","Can_sil",900,900);

      Can_track=new TCanvas("Can_track","Can_track",900,900);
      //Can_track->Divide(1,2);
      //Can_track->cd(2)->Divide(2,1);

      //Can_fig=new TCanvas("Can_fig","Can_fig",900,900);
      //Can_fig->Divide(2,2);
    }

    Int_t nb=0;
    //*********************************************************************************************************//
    //*********************************************************************************************************//
    //**                                                                                                     **//
    //**                                        Event Loop                                                   **//
    //**                                                                                                     **//
    //*********************************************************************************************************//
    //*********************************************************************************************************//
    
    for (Long64_t jentry=0;jentry<nentries;jentry++) 
    //for (Long64_t jentry=16000;jentry<nentries;jentry++) 
    //for (Long64_t jentry=0;jentry<nentries;jentry+=25) 
      {
	if(jentry%100==0 && jentry!=0)cout<<"."<<flush;
	if(jentry%500==0)cout<<jentry<<endl;

	if(jentry%NRun==0 && jentry!=0){
	  h_DeltaRW1->Reset();
	  h_DeltaRW2->Reset();
	  h_DeltaRW3->Reset();
	  h_DeltaRP1->Reset();
	  h_DeltaRP2->Reset();
	  h_DeltaRP3->Reset();
	  h_DeltaRXY->Reset();
	}

	//cout<<endl<<"NEW EVENT "<<jentry<<endl<<endl;

	for(Int_t u=0;u<numberOfRows;u++){
	  for(Int_t k=0;k<numberOfColumns;k++){
	    padCharge[u][k]=0.;
	    padTime[u][k]=0.;
	    padHeight[u][k]=0.;
	  }
	}

	for(Int_t i=0;i<3;i++){
	  Start[i]=Out[i]=End[i]=Stop[i]=0.;
	  StopX[i]=StopY[i]=0.;
	}

	a=b=0.;
	astride=bstride=0.;

	T[0]->ResetLines();
	T[1]->ResetLines();

	visu_charge->Reset();
	visu_time->Reset();

	padSignalCA->Clear();
	simpleTrackCA->Clear();
	nb+=digTree->GetEvent(jentry);
	nb+=simTree->GetEvent(jentry);

	Int_t nbprimaries=primaryInfoCA->GetEntries();
	//cout<<" PRIMARIES "<<nbprimaries<<endl;

	Int_t numberofpads=padSignalCA->GetEntries();
	//cout<<"# Pads fired "<< numberofpads<<endl;

	//Loop on TClonesArray
	Int_t counter=0;
	if( numberofpads>0){
	  Double_t maxenergydeposit=0;
	  Double_t maxenergytime=0;
	  Int_t maxRow=0;
	  for(Int_t k=0;k<numberofpads;k++){
	    padSignal=(ActarPadSignal*)padSignalCA->At(k);
	    Double_t thisCharge = padSignal->GetChargeDeposited();
	    Double_t thisTime   = (padSignal->GetInitTime()+padSignal->GetFinalTime())/2.;
	    Double_t thisSigmaTime=padSignal->GetSigmaTime();
	    //thisTime=gRandom->Gaus(thisTime,thisSigmaTime);
	    //cout<<"Time "<<thisTime<<" Sigma Time "<<thisSigmaTime<<endl;
	    Int_t PadRow=padSignal->GetPadRow();
	    Int_t PadColumn=padSignal->GetPadColumn();
	    //cout<<k<<" "<<PadColumn<<" "<<PadRow<<" "<<thisCharge<<" "<<thisTime<<endl;
	    if(thisCharge>=maxenergydeposit){
	      maxenergydeposit=thisCharge;
	      maxenergytime=thisTime;
	      maxRow=PadRow;
	    }

	    if (thisCharge!=0){
	      padCharge[padSignal->GetPadRow()-1][padSignal->GetPadColumn()-1]
		+= thisCharge;
	      padTime[padSignal->GetPadRow()-1][padSignal->GetPadColumn()-1]
		+= thisCharge*thisTime;
	      //cout<<"Event "<<jentry<<" k "<<k<<" Row "<<padSignal->GetPadRow()-1<<" Col "<<padSignal->GetPadColumn()-1<<" PadCharge "<<padCharge[padSignal->GetPadRow()-1][padSignal->GetPadColumn()-1]<<endl;
	    }

	    Int_t r=padSignal->GetPadRow()-1;
	    Int_t c=padSignal->GetPadColumn()-1;

	  }//Loop on ActarPadSignals

	}

	////////////////////////////////////////////////////
	////////////// Loop on columns & rows //////////////
	////////////////////////////////////////////////////

	for(Int_t c=0;c<numberOfColumns;c++){ //LOOP on Columns
	  for(Int_t r=0;r<numberOfRows;r++){ //LOOP on Rows
	    if(padCharge[r][c] >thresholdADC){
	      //cout<<"Sigma Time "<<thisSigmaTime<<endl;
	      padTime[r][c]/=padCharge[r][c];
	      padTime[r][c]=gRandom->Gaus(padTime[r][c],sigma_time);
	      //padTime[r][c]=gRandom->Gaus(padTime[r][c],thisSigmaTime);
	      padCharge[r][c]*=30/1e9;
	      //padCharge[r][c]*=15/1e9;
	      padHeight[r][c]=padTime[r][c]*driftVelocity;
	      //cout<<c<<" "<<r<<" "<<padHeight[r][c]<<endl;

	      if(r>150 && padCharge[r][c]>0)cout<<"Row "<<r<<" Col "<<c<<" padCharge "<<padCharge[r][c]<<" padTime "<<padTime[r][c]<<endl;

	      if(TrackVisuFlag){
		visu_charge->SetBinContent(c+1.,r+1,padCharge[r][c]);
		visu_time->SetBinContent(c+1.,r+1.,padTime[r][c]);
	      }
	    }
	    else{ 
	      padCharge[r][c]=-1;
	      padTime[r][c]=-1;
	      padHeight[r][c]=-1;
	    }

	  }//End of Loop on Rows
	    
	}//End of Loop on Columns

	//FitMat(padCharge,0,31,0,63,threshold,a,b);	
	FitMat(padCharge,Rmin,Rmax,Cmin,Cmax,threshold,a,b);

	IniPoint[0][0]=0; IniPoint[1][0]=a*IniPoint[0][0]+b/2; 
	//FinPoint[1][0]=32; FinPoint[0][0]=(FinPoint[1][0]-b/2)/a; //if horizontal angle>0
	//FinPoint[0][0]=64; FinPoint[1][0]=a*FinPoint[0][0]+b/2; //if horizontal angle=0
	FinPoint[0][0]=numberOfColumns; FinPoint[1][0]=FinPoint[0][0]*a+b/2;

	//Double_t chi2=FitMat3D(padCharge,padHeight,0,31,0,63,threshold,T[0]);
	Double_t chi2=FitMat3D(padCharge,padHeight,Rmin,Rmax,Cmin,Cmax,threshold,T[0]);

	Int_t t=0;

	if(T[0]->Xm-T[0]->Xh!=0){
	  Double_t tstart=(0-T[0]->Xh)/(T[0]->Xm-T[0]->Xh);
	  Start[0]=0.;
	  Start[1]=tstart*(T[0]->Ym-T[0]->Yh)+T[0]->Yh;
	  Start[2]=tstart*(T[0]->Zm-T[0]->Zh)+T[0]->Zh;

	  Double_t tstop=((Double_t)numberOfColumns*2-T[0]->Xh)/(T[0]->Xm-T[0]->Xh);
	  Stop[0]=(Double_t)numberOfColumns*2;
	  Stop[1]=tstop*(T[0]->Ym-T[0]->Yh)+T[0]->Yh;
	  Stop[2]=tstop*(T[0]->Zm-T[0]->Zh)+T[0]->Zh;

	  Double_t trstart=(10-T[0]->Xh)/(T[0]->Xm-T[0]->Xh);
	  TStart[0]=10.;
	  TStart[1]=trstart*(T[0]->Ym-T[0]->Yh)+T[0]->Yh;
	  TStart[2]=trstart*(T[0]->Zm-T[0]->Zh)+T[0]->Zh;
	}

	/*
	if(T[t]->Xm-T[t]->Xh!=0){
	  Double_t tstart=(-T[0]->Xh)/(T[0]->Xm-T[0]->Xh);
	  Double_t tend=(32*2-T[0]->Xh)/(T[0]->Xm-T[0]->Xh); //if horizontal angle=0 
	}

	Start[0]=tstart*(T[t]->Xm-T[t]->Xh)+T[t]->Xh;
	Start[1]=tstart*(T[t]->Ym-T[t]->Yh)+T[t]->Yh;
	Start[2]=tstart*(T[t]->Zm-T[t]->Zh)+T[t]->Zh;

	End[0]=tend*(T[t]->Xm-T[t]->Xh)+T[t]->Xh;
	End[1]=tend*(T[t]->Ym-T[t]->Yh)+T[t]->Yh;
	End[2]=tend*(T[t]->Zm-T[t]->Zh)+T[t]->Zh;
	

	VAngle=TMath::ATan((End[2]-Start[2])/TMath::Sqrt((End[0]-Start[0])*(End[0]-Start[0])+(End[1]-Start[1])*(End[1]-Start[1])))*deg;
	if(End[0]-Start[0]>0){
	  HAngle=TMath::ATan((End[1]-Start[1])/(End[0]-Start[0]))*deg;
	  Angle3D=TMath::ATan(TMath::Sqrt((End[1]-Start[1])*(End[1]-Start[1])+(End[2]-Start[2])*(End[2]-Start[2]))/(End[0]-Start[0]))*deg;
	}
	else{
	  HAngle=180.+TMath::ATan((End[1]-Start[1])/(End[0]-Start[0]))*deg;
	  Angle3D=180.+TMath::ATan(TMath::Sqrt((End[1]-Start[1])*(End[1]-Start[1])+(End[2]-Start[2])*(End[2]-Start[2]))/(End[0]-Start[0]))*deg;
	}
	*/

	VAngle=TMath::ATan((Stop[2]-Start[2])/TMath::Sqrt((Stop[0]-Start[0])*(Stop[0]-Start[0])+(Stop[1]-Start[1])*(Stop[1]-Start[1])))*deg;
	/*
	if(Stop[0]-Start[0]>0){
	  HAngle=TMath::ATan((Stop[1]-Start[1])/(Stop[0]-Start[0]))*deg;
	  Angle3D=TMath::ATan(TMath::Sqrt((Stop[1]-Start[1])*(Stop[1]-Start[1])+(Stop[2]-Start[2])*(Stop[2]-Start[2]))/(Stop[0]-Start[0]))*deg;
	}
	else{
	  HAngle=180.+TMath::ATan((Stop[1]-Start[1])/(Stop[0]-Start[0]))*deg;
	  Angle3D=180.+TMath::ATan(TMath::Sqrt((Stop[1]-Start[1])*(Stop[1]-Start[1])+(Stop[2]-Start[2])*(Stop[2]-Start[2]))/(Stop[0]-Start[0]))*deg;
	}
	*/

	HAngle=TMath::ATan(a)*deg;

	hVertiAngle->Fill(VAngle);
	hHorizAngle->Fill(HAngle);

	//cout<<"VAngle "<<VAngle<<" HAngle "<<HAngle<<" Angle3D "<<Angle3D<<" HAngle from a "<<TMath::ATan(a)*deg<<" Diff Theta from Start/Stop and a "<<HAngle-TMath::ATan(a)*deg<<endl;

	Double_t OS;//Distance from Track intersection with x=0 plane and (10,10,100) point

	OS=TMath::Sqrt((Start[0]-StartTrack[0])*(Start[0]-StartTrack[0])+(Start[1]-StartTrack[1])*(Start[1]-StartTrack[1])+(Start[2]-StartTrack[2])*(Start[2]-StartTrack[2]));

	//cout<<"OS "<<OS<<endl;

	if(DEdepFlag){

	  Double_t dE_digW=0.;
	  Double_t value=0.;

	  Double_t step=2*TMath::Abs(RangeB-RangeA)/100.;
	  for(Double_t val=RangeA;val<=RangeB;val=val+step){
	    if(val>RangeA){
	      dE_digW+=step*(value+bragRW2->Eval(val))/2/2;//this is because the bragR binning is *2
	      value=bragRW2->Eval(val);
	    }
	    //cout<<"val "<<val<<" value "<<value<<" dE_dig "<<dE_dig<<endl;

	  }

	  //cout<<"dE_digPiotr "<<dE_digPiotr<<" MeV"<<endl;

	  Double_t dE_digP2=0.;
	  value=0.;

	  //Double_t step=2*TMath::Abs(RangeAB)/100.;
	  step=2*TMath::Abs(RangeB-RangeA)/100.;
	  //cout<<"step "<<step<<endl;
	  //for(Double_t val=0;val<=RangeAB;val=val+step){
	  for(Double_t val=RangeA;val<=RangeB;val=val+step){
	    //Double_t value=bragR->Eval(val);

	    //if(val>0){
	    if(val>RangeA){
	      dE_digP2+=step*(value+bragRP2->Eval(val))/2/2;//this is because the bragR binning is *2
	      value=bragRP2->Eval(val);
	      //dE_dig+=step*value;
	    }
	    //cout<<"val "<<val<<" value "<<value<<" dE_dig "<<dE_dig<<endl;
	  }

	  //cout<<"dE_dig "<<dE_dig<<" MeV"<<endl;

	}

	//Getting the simulated track     
	Int_t nbOfStrides=simpleTrackCA->GetEntries();

	//const Int_t NStride=nbOfStrides;
	//cout<<"nbOfStrides "<<nbOfStrides<<" NStride "<<NStride<<endl;
	const Int_t NStride=500;
	
	Double_t StrideX[NStride],StrideY[NStride],StrideZ[NStride];
	Double_t StrideQ[NStride];
	

	Double_t StrideLength;
	Double_t TotalStrideLength=0;
	Double_t StrideCenter[3];
	Double_t G4Range,G4Edep;

	Double_t posmaxG4=0;

	Double_t Elast=0;
	Double_t dE_sim2=0;
	dE_sim=0;
	Int_t m1=0;
	Int_t m2=0;

	Double_t G4VAngle=0;
	Double_t G4HAngle=0;
	Double_t G4StrideHAngle=0;
	Double_t TotG4HAngle=0;
	Double_t G4QTot=0.;

	SimTrack->Set(0);
	G4EdepGraph->Set(0);

	if(nbOfStrides>0){
	  for(Int_t m=0;m<nbOfStrides;m++){
	    simpleTrack=(ActarSimSimpleTrack *)simpleTrackCA->At(m);
	    //cout<<simpleTrack->GetZPre()<<" "<<simpleTrack->GetXPre()<<" "<<simpleTrack->GetZPost()<<" "<<simpleTrack->GetXPost()<<endl;
	    //cout<<simpleTrack->GetEnergyStride()<<" "<<simpleTrack->GetParticleEnergy()<<endl;

	    //cout<<m<<" "<<simpleTrack->GetXPre()<<" "<<simpleTrack->GetYPre()<<" "<<simpleTrack->GetZPre()<<endl;
	    //if(m==0)cout<<"XPre "<<simpleTrack->GetXPre()<<" ZPre "<<simpleTrack->GetZPre()<<" YPre "<<simpleTrack->GetYPre()<<endl;

	    //In ActarSim Y <-> Z and origin at the center of gas box for X and Z
	    //StrideCenter[0]=(simpleTrack->GetXPost()+simpleTrack->GetXPre()+2*32)/2;
	    //StrideCenter[2]=(simpleTrack->GetYPost()+simpleTrack->GetYPre()+2*85)/2;
	    //StrideCenter[1]=(simpleTrack->GetZPost()+simpleTrack->GetZPre()+2*128)/2;

	    StrideCenter[0]=(simpleTrack->GetXPost()+simpleTrack->GetXPre()+2*numberOfRows)/2;
	    //StrideCenter[2]=(simpleTrack->GetYPost()+simpleTrack->GetYPre()+2*85)/2;
	    StrideCenter[2]=(simpleTrack->GetYPost()+simpleTrack->GetYPre()+2*yBox)/2;
	    StrideCenter[1]=(simpleTrack->GetZPost()+simpleTrack->GetZPre()+2*numberOfColumns)/2;

	    //if(m==0)cout<<"StrideCenter[x] "<<StrideCenter[0]<<" StrideCenter[y] "<<StrideCenter[1]<<" StrideCenter[z] "<<StrideCenter[2]<<endl;

	    StrideX[m]=StrideCenter[1];
	    StrideY[m]=StrideCenter[0];
	    StrideZ[m]=StrideCenter[2];

	    StrideLength=TMath::Power(simpleTrack->GetXPost()-simpleTrack->GetXPre(),2);
	    //StrideLength+=TMath::Power(simpleTrack->GetYPost()-simpleTrack->GetYPre(),2);
	    StrideLength+=TMath::Power(simpleTrack->GetZPost()-simpleTrack->GetZPre(),2);
	    StrideLength=TMath::Sqrt(StrideLength);

	    TotalStrideLength+=StrideLength;

	    G4Range=OS+TMath::Sqrt(TMath::Power(StrideCenter[0]-StartTrack[0],2) + TMath::Power(StrideCenter[1]-StartTrack[1],2) + TMath::Power(StrideCenter[2]-StartTrack[2],2));
	    //G4Range=TMath::Sqrt(TMath::Power(StrideCenter[1]-StartTrack[1],2) + TMath::Power(StrideCenter[2]-StartTrack[2],2));
	    G4Edep=simpleTrack->GetEnergyStride()/StrideLength;

	    G4QTot+=simpleTrack->GetEnergyStride();
	    StrideQ[m]=simpleTrack->GetEnergyStride();//or /StrideLength ???

	    //cout<<"S "<<S<<" STRIDEX "<<STRIDEX[S]<<" STRIDEY "<<STRIDEY[S]<<" STRIDENET "<<STRIDENET[S]<<endl;

	    //if(TrackVisuFlag)SimTrack->SetPoint(m,simpleTrack->GetZPre()/2+64,simpleTrack->GetXPre()/2+16);
	    if(TrackVisuFlag)SimTrack->SetPoint(m,simpleTrack->GetZPre()/2+numberOfColumns/2,simpleTrack->GetXPre()/2+numberOfRows/2);
	    /*
	    if(m==0){StrideStartInit[0]=simpleTrack->GetZPre()+128; StrideStartInit[1]=simpleTrack->GetXPre()+32;}
	    if(m==nbOfStrides-1){StrideStopFin[0]=simpleTrack->GetZPost()+128; StrideStopFin[1]=simpleTrack->GetXPost()+32;}
	    StrideStart[1]=simpleTrack->GetXPre()+32;
	    StrideStart[0]=simpleTrack->GetZPre()+128;

	    StrideStop[1]=simpleTrack->GetXPost()+32;
	    StrideStop[0]=simpleTrack->GetZPost()+128;
	    */

	    if(m==0){StrideStartInit[0]=simpleTrack->GetZPre()+numberOfColumns; StrideStartInit[1]=simpleTrack->GetXPre()+numberOfRows;}
	    if(m==nbOfStrides-1){StrideStopFin[0]=simpleTrack->GetZPost()+numberOfColumns; StrideStopFin[1]=simpleTrack->GetXPost()+numberOfRows;}
	    StrideStart[1]=simpleTrack->GetXPre()+numberOfRows;
	    StrideStart[0]=simpleTrack->GetZPre()+numberOfColumns;

	    StrideStop[1]=simpleTrack->GetXPost()+numberOfRows;
	    StrideStop[0]=simpleTrack->GetZPost()+numberOfColumns;

	    if(StrideStop[0]-StrideStart[0]>0){
	      StrideHAngle=TMath::ATan((StrideStop[1]-StrideStart[1])/(StrideStop[0]-StrideStart[0]))*deg;
	    }
	    else{
	      StrideHAngle=180.+TMath::ATan((StrideStop[1]-StrideStart[1])/(StrideStop[0]-StrideStart[0]))*deg;
	    }

	    if(StrideStop[0]-StrideStartInit[0]>0){
	      TotStrideHAngle=TMath::ATan((StrideStop[1]-StrideStartInit[1])/(StrideStop[0]-StrideStartInit[0]))*deg;
	    }
	    else{
	      TotStrideHAngle=180.+TMath::ATan((StrideStop[1]-StrideStartInit[1])/(StrideStop[0]-StrideStartInit[0]))*deg;
	    }

	    //cout<<"Stride "<<m<<" StrideHAngle "<<StrideHAngle<<" StrideLength "<<StrideLength<<" Stride Energy "<<simpleTrack->GetEnergyStride()<<endl;

	    TotG4HAngle+=TotStrideHAngle;
	    //G4HAngle+=StrideHAngle;
	    G4StrideHAngle+=StrideHAngle*StrideLength;
	    //G4HAngle+=StrideHAngle*simpleTrack->GetEnergyStride();

	  }

	  if(RangeFlag){

	    //posmaxG4=G4Range;
	    //cout<<"XPre "<<simpleTrack->GetXPre()<<" ZPre "<<simpleTrack->GetZPre()<<" YPre "<<simpleTrack->GetYPre()<<endl;
	    //cout<<"XPost "<<simpleTrack->GetXPost()<<"  ZPost "<<simpleTrack->GetZPost()<<" YPost "<<simpleTrack->GetYPost()<<endl;

	    //posmaxG4=OS+TMath::Sqrt(TMath::Power(simpleTrack->GetXPost()+32-StartTrack[0],2) + TMath::Power(simpleTrack->GetZPost()-5-StartTrack[1],2) + TMath::Power(simpleTrack->GetYPost()+85-StartTrack[2],2));
	    posmaxG4=OS+TMath::Sqrt(TMath::Power(simpleTrack->GetXPost()+32-StartTrack[0],2) + TMath::Power(simpleTrack->GetZPost()-5-StartTrack[1],2) + TMath::Power(simpleTrack->GetYPost()+150-StartTrack[2],2));

	    //cout<<"G4Range "<<G4Range<<" posmaxG4 "<<posmaxG4<<endl;

	    RangeX=TMath::Sqrt((StopX[0]-StartTrack[0])*(StopX[0]-StartTrack[0])+(StopX[1]-StartTrack[1])*(StopX[1]-StartTrack[1])+(StopX[2]-StartTrack[2])*(StopX[2]-StartTrack[2]));
	    RangeY=TMath::Sqrt((StopY[0]-StartTrack[0])*(StopY[0]-StartTrack[0])+(StopY[1]-StartTrack[1])*(StopY[1]-StartTrack[1])+(StopY[2]-StartTrack[2])*(StopY[2]-StartTrack[2]));

	    RangeXY=TMath::Cos(HAngle*rad)*TMath::Cos(HAngle*rad)*RangeX+TMath::Sin(HAngle*rad)*TMath::Sin(HAngle*rad)*RangeY;
	  
	  }

	}

	FitStride(StrideQ, StrideX, StrideY, StrideZ, nbOfStrides, threshold, astride, bstride);

	Double_t stridechi2=FitStride3D(StrideQ, StrideX, StrideY, StrideZ, nbOfStrides, threshold, T[1]);

	if(T[1]->Xm-T[1]->Xh!=0){
	  Double_t tstart=(0-T[1]->Xh)/(T[1]->Xm-T[1]->Xh);
	  Start[0]=0.;
	  Start[1]=tstart*(T[1]->Ym-T[1]->Yh)+T[1]->Yh;
	  Start[2]=tstart*(T[1]->Zm-T[1]->Zh)+T[1]->Zh;

	  Double_t tstop=((Double_t)numberOfColumns*2-T[1]->Xh)/(T[1]->Xm-T[1]->Xh);
	  Stop[0]=(Double_t)numberOfColumns*2;
	  Stop[1]=tstop*(T[1]->Ym-T[1]->Yh)+T[1]->Yh;
	  Stop[2]=tstop*(T[1]->Zm-T[1]->Zh)+T[1]->Zh;

	  Double_t trstart=(10-T[1]->Xh)/(T[1]->Xm-T[1]->Xh);
	  TStart[0]=10.;
	  TStart[1]=trstart*(T[1]->Ym-T[1]->Yh)+T[1]->Yh;
	  TStart[2]=trstart*(T[1]->Zm-T[1]->Zh)+T[1]->Zh;
	}

	G4VAngle=TMath::ATan((Stop[2]-Start[2])/TMath::Sqrt((Stop[0]-Start[0])*(Stop[0]-Start[0])+(Stop[1]-Start[1])*(Stop[1]-Start[1])))*deg;
	/*
	if(Stop[0]-Start[0]>0){
	  G4HAngle=TMath::ATan((Stop[1]-Start[1])/(Stop[0]-Start[0]))*deg;
	}
	else if(Stop[0]-Start[0]<0){
	  G4HAngle=180.+TMath::ATan((Stop[1]-Start[1])/(Stop[0]-Start[0]))*deg;
	}
	else cout<<"Stop[0] = Start[0]"<<endl;
	//cout<<"G4QTot "<<G4QTot<<endl;
	*/
	G4HAngle=TMath::ATan(astride)*deg;

	TotG4HAngle/=nbOfStrides;
	//G4HAngle/=nbOfStrides;
	G4StrideHAngle/=TotalStrideLength;
	//G4HAngle/=G4QTot;

	LazyG4HAngle=TMath::ATan((StrideStopFin[1]-StrideStartInit[1])/(StrideStopFin[0]-StrideStartInit[0]))*deg;

	//cout<<"ATan a "<<atan(a)*deg<<" Stride aTan a "<<atan(astride)*deg<<" G4HAngle "<<G4HAngle<<" Diff ATan a "<<atan(a)*deg-atan(astride)*deg<<" Diff TMath::ATan a "<<TMath::ATan(a)*deg-TMath::ATan(astride)*deg<<endl;
	//cout<<"LazyG4HAngle "<<LazyG4HAngle<<" G4HAngle "<<G4HAngle<<" TotG4HAngle "<<TotG4HAngle<<" G4HAngle2 "<<G4HAngle2<<" HAngle "<<HAngle<<endl;
	//cout<<"TotG4HAngle "<<TotG4HAngle<<" G4HAngle2 "<<G4HAngle2<<" HAngle "<<HAngle<<endl;
	//cout<<"G4VAngle "<<G4VAngle<<" VAngle "<<VAngle<<" diff "<<G4VAngle-VAngle<<endl;

	h_DeltaVAngle->Fill(G4VAngle-VAngle);

	h_DeltaHAngle->Fill(G4HAngle-HAngle);
	h_TotDeltaHAngle->Fill(TotG4HAngle-HAngle);
	h_DeltaLazyHAngle->Fill(LazyG4HAngle-HAngle);

	h_HAngle->Fill(HAngle);
	h_TotHAngle->Fill(TotG4HAngle);

	//Int_t NEvent=500;
	Int_t NEvent=2000;

	h_MultiDeltaHAngle[(int)jentry/NEvent]->Fill(G4HAngle-HAngle);


	/*
	if(jentry<500)h_DeltaVAngle80->Fill(G4VAngle-VAngle);
	else if(jentry<1000)h_DeltaVAngle70->Fill(G4VAngle-VAngle);
	else if(jentry<1500)h_DeltaVAngle60->Fill(G4VAngle-VAngle);
	else if(jentry<2000)h_DeltaVAngle50->Fill(G4VAngle-VAngle);
	else if(jentry<2500)h_DeltaVAngle45->Fill(G4VAngle-VAngle);
	else if(jentry<3000)h_DeltaVAngle40->Fill(G4VAngle-VAngle);
	else if(jentry<3500)h_DeltaVAngle30->Fill(G4VAngle-VAngle);
	else if(jentry<4000)h_DeltaVAngle20->Fill(G4VAngle-VAngle);
	else if(jentry<4500)h_DeltaVAngle10->Fill(G4VAngle-VAngle);
	*/
	/*
	if(jentry<500)h_DeltaVAnglem80->Fill(G4VAngle-VAngle);
	else if(jentry<1000)h_DeltaVAnglem70->Fill(G4VAngle-VAngle);
	else if(jentry<1500)h_DeltaVAnglem60->Fill(G4VAngle-VAngle);
	else if(jentry<2000)h_DeltaVAnglem50->Fill(G4VAngle-VAngle);
	else if(jentry<2500)h_DeltaVAnglem45->Fill(G4VAngle-VAngle);
	else if(jentry<3000)h_DeltaVAnglem40->Fill(G4VAngle-VAngle);
	else if(jentry<3500)h_DeltaVAnglem30->Fill(G4VAngle-VAngle);
	else if(jentry<4000)h_DeltaVAnglem20->Fill(G4VAngle-VAngle);
	else if(jentry<4500)h_DeltaVAnglem10->Fill(G4VAngle-VAngle);
	*/

	if(TrackBragFlag){
	  //cout<<"RangeA[0] "<<RangeA[0]<<" RangeB[0] "<<RangeB[0]<<endl;
	  //cout<<"RangeA[1] "<<RangeA[1]<<" RangeB[1] "<<RangeB[1]<<endl;

	  Can_brag->cd(1);
	  h_braggP1->Draw();
	  bragRP1->Draw("same");

	  if(RangeFlag){
	    TLine lineP1(posmaxP1, 0, posmaxP1, 10e7);
	    lineP1.SetLineWidth(3);
	    lineP1.Draw("same");

	    TLine lineG4(posmaxG4, 0, posmaxG4, 10e7);
	    lineG4.SetLineWidth(3);
	    lineG4.SetLineColor(kRed);
	    lineG4.Draw("same");
	  }

	  Can_brag->cd(2);
	  h_braggP2->Draw();
	  bragRP2->Draw("same");

	  if(RangeFlag){
	    TLine lineP2(posmaxP2, 0, posmaxP2, 10e7);
	    lineP2.SetLineWidth(3);
	    lineP2.Draw("same");
	    lineG4.Draw("same");
	  }

	  if(DEdepFlag){
	    TLine lineya1(RangeA[0], 0, RangeA[0], 10e7);
	    //TLine lineya1(0, 0, 0, 10e7);
	    lineya1.SetLineWidth(3);
	    lineya1.Draw("same");
	    TLine lineyb1(RangeB[0], 0, RangeB[0], 10e7);
	    //TLine lineyb1(RangeO[0], 0, RangeO[0], 10e7);
	    lineyb1.Draw("same");
	    lineyb1.SetLineWidth(3);
	  }

	  Can_brag->cd(3);
	  h_braggP3->Draw();
	  bragRP3->Draw("same");

	  if(RangeFlag){
	    TLine lineP3(posmaxP3, 0, posmaxP3, 10e7);
	    lineP3.SetLineWidth(3);
	    lineP3.Draw("same");
	    lineG4.Draw("same");
	  }

	  if(DEdepFlag){
	    TLine linemin45(RangeA[0], 0, RangeA[0], 10e7);
	    linemin45.SetLineWidth(3);
	    linemin45.Draw("same");
	    TLine linemax45(RangeB[0], 0, RangeB[0], 10e7);
	    linemax45.Draw("same");
	    linemax45.SetLineWidth(3);
	  }

	  Can_brag->cd(4);

	  h_braggW1->Draw();
	  bragRW1->Draw("same");

	  if(RangeFlag){
	    TLine lineW1(posmaxW1, 0, posmaxW1, 10e7);
	    lineW1.SetLineWidth(3);
	    lineW1.Draw("same");
	    lineG4.Draw("same");
	  }

	  Can_brag->cd(5);

	  h_braggW2->Draw();
	  bragRW2->Draw("same");

	  if(RangeFlag){
	    TLine lineW2(posmaxW2, 0, posmaxW2, 10e7);
	    lineW2.SetLineWidth(3);
	    lineW2.Draw("same");
	    lineG4.Draw("same");
	  }

	  if(DEdepFlag){
	    TLine lineminW(RangeA[0], 0, RangeA[0], 10e7);
	    lineminW.SetLineWidth(3);
	    lineminW.Draw("same");
	    TLine linemaxW(RangeB[0], 0, RangeB[0], 10e7);
	    linemaxW.Draw("same");
	    linemaxW.SetLineWidth(3);
	  }

	  Can_brag->cd(6);

	  h_braggW3->Draw();
	  bragRW3->Draw("same");

	  if(RangeFlag){
	    TLine lineW3(posmaxW3, 0, posmaxW3, 10e7);
	    lineW3.SetLineWidth(3);
	    lineW3.Draw("same");
	    lineG4.Draw("same");
	  }

	  Can_brag->cd(7);
	  h_braggX->Draw();
	  bragX->Draw("same");

	  if(RangeFlag){
	    TLine lineX(posmaxX, 0, posmaxX, 10e7);
	    lineX.SetLineWidth(3);
	    lineX.Draw("same");
	  }

	  if(DEdepFlag){
	    TLine linemin2(MinCol*2, 0, MinCol*2, 10e7);
	    linemin2.SetLineWidth(3);
	    linemin2.Draw("same");
	    TLine linemax2(MaxCol*2, 0, MaxCol*2, 10e7);
	    linemax2.Draw("same");
	    linemax2.SetLineWidth(3);
	  }

	  Can_brag->cd(8);
	  h_braggY->Draw();
	  bragY->Draw("same");

	  if(RangeFlag){
	    TLine lineY(posmaxY, 0, posmaxY, 10e7);
	    lineY.SetLineWidth(3);
	    lineY.Draw("same");
	  }

	  if(DEdepFlag){
	    TLine linemin3(MinRow*2, 0, MinRow*2, 10e7);
	    linemin3.SetLineWidth(3);
	    linemin3.Draw("same");
	    TLine linemax3(MaxRow*2, 0, MaxRow*2, 10e7);
	    linemax3.Draw("same");
	    linemax3.SetLineWidth(3);
	  }

	  Can_brag->cd(9);
	  
	  //h_G4Edep->Draw();	    
	  
	  h_braggW2->Draw();
	  bragRW2->Draw("same");
	  lineG4.Draw("same");
	  
	  G4EdepGraph->SetMarkerStyle(3);
	  G4EdepGraph->SetMarkerSize(1.5);
	  G4EdepGraph->SetMarkerColor(kBlue);
	  G4EdepGraph->Draw("Psame");
	  
	  Can_brag->Update();
	}

	if(TrackVisuFlag){

	  /*
	  Can_fig->cd();
	  h_braggW2->Draw();
	  bragRW2->Draw("same");
	  lineG4.Draw("same");
	  lineW2.Draw("same");
	  Can_fig->Update();
	  */
	  //Can_track->cd(1);
	  Can_track->cd();
	  visu_charge->Draw("colz");

	  SimTrack->SetMarkerStyle(29);
	  SimTrack->SetMarkerSize(2);
	  //SimTrack->SetMarkerColor(kWhite);
	  SimTrack->Draw("Psame");

	  if(DEdepFlag){
	    TLine line1(MinCol, MinRow, MinCol, MaxRow);
	    line1.SetLineStyle(kDashed);
	    line1.SetLineWidth(3);
	    line1.Draw("same");
	    TLine line2(MinCol, MinRow, MaxCol, MinRow);
	    line2.SetLineStyle(kDashed);
	    line2.SetLineWidth(3);
	    line2.Draw("same");
	    TLine line3(MinCol, MaxRow, MaxCol, MaxRow);
	    line3.SetLineStyle(kDashed);
	    line3.SetLineWidth(3);
	    line3.Draw("same");
	    TLine line4(MaxCol, MinRow, MaxCol, MaxRow);
	    line4.SetLineStyle(kDashed);
	    line4.SetLineWidth(3);
	    line4.Draw("same");

	    TLine linedl1(PointA[0]/2., PointA[1]/2., PointB[0]/2., PointB[1]/2.);
	    linedl1.SetLineWidth(6);
	    linedl1.SetLineColor(kGreen);
	    linedl1.Draw("same");


	    TLine linefit1(IniPoint[0][0], IniPoint[1][0], FinPoint[0][0], FinPoint[1][0]);
	    //linefit1.SetLineStyle(kDashed);
	    linefit1.SetLineWidth(4);
	    linefit1.Draw("same");

	  
	    if(m2>1){
	      SimTrack->SetMarkerStyle(29);
	      SimTrack->SetMarkerSize(2);
	      SimTrack->SetMarkerColor(kWhite);
	      SimTrack->Draw("Psame");
	    }

	    if(m1>1){
	      SimTrackE->SetMarkerStyle(29);
	      SimTrackE->SetMarkerSize(2);
	      SimTrackE->SetMarkerColor(kViolet-1);
	      SimTrackE->Draw("Psame");
	    }
	  }
	  /*
	  Can_track->cd(2);
	  visu_time->Draw("colz");
	  */
	  Can_track->Update();
	  Can_track->WaitPrimitive();
	}
     
      }
    
    //*********************************************************************************************************//
    //*********************************************************************************************************//
    //**                                                                                                     **//
    //**                                       End of Event Loop                                             **//
    //**                                                                                                     **//
    //*********************************************************************************************************//
    //*********************************************************************************************************//

    /*
    Can_HAngle=new TCanvas("Can_HAngle","Can_HAngle",900,600);
    h_HAngle->Draw();

    Can_TotHAngle=new TCanvas("Can_TotHAngle","Can_TotHAngle",900,600);
    h_TotHAngle->Draw();
    */
    
    Can_DeltaVAngle=new TCanvas("Can_DeltaVAngle","Can_DeltaVAngle",900,600);
    h_DeltaVAngle->Draw();
    
    Can_DeltaHAngle=new TCanvas("Can_DeltaHAngle","Can_DeltaHAngle",900,600);
    h_DeltaHAngle->Draw();
    
    //Can_TotDeltaHAngle=new TCanvas("Can_TotDeltaHAngle","Can_TotDeltaHAngle",900,600);
    //h_TotDeltaHAngle->Draw();


    Can_TotDeltaHAnglem=new TCanvas("Can_TotDeltaHAnglem","Can_TotDeltaHAnglem",900,600);
    Can_TotDeltaHAnglem->Divide(3,3);
    for(Int_t i=0; i<8; i++)
      {
	Can_TotDeltaHAnglem->cd(i+1);
	h_MultiDeltaHAngle[i]->Fit("gaus","L");
	h_MultiDeltaHAngle[i]->Draw();
      }


    Can_TotDeltaHAngle=new TCanvas("Can_TotDeltaHAngle","Can_TotDeltaHAngle",900,600);
    Can_TotDeltaHAngle->Divide(3,3);
    for(Int_t i=8; i<17; i++)
      {
	Can_TotDeltaHAngle->cd(i-7);
	h_MultiDeltaHAngle[i]->Fit("gaus","L");
	h_MultiDeltaHAngle[i]->Draw();
      }

    /*
    Can_TotDeltaHAngle->Divide(3,3);
    Can_TotDeltaHAngle->cd(1);
    h_DeltaVAngle80->Draw();
    Can_TotDeltaHAngle->cd(2);
    h_DeltaVAngle70->Draw();
    Can_TotDeltaHAngle->cd(3);
    h_DeltaVAngle60->Draw();
    Can_TotDeltaHAngle->cd(4);
    h_DeltaVAngle50->Draw();
    Can_TotDeltaHAngle->cd(5);
    h_DeltaVAngle45->Draw();
    Can_TotDeltaHAngle->cd(6);
    h_DeltaVAngle40->Draw();
    Can_TotDeltaHAngle->cd(7);
    h_DeltaVAngle30->Draw();
    Can_TotDeltaHAngle->cd(8);
    h_DeltaVAngle20->Draw();
    Can_TotDeltaHAngle->cd(9);
    h_DeltaVAngle10->Draw();
    */
    /*
    Can_TotDeltaHAngle->Divide(3,3);
    Can_TotDeltaHAngle->cd(1);
    h_DeltaVAnglem80->Draw();
    Can_TotDeltaHAngle->cd(2);
    h_DeltaVAnglem70->Draw();
    Can_TotDeltaHAngle->cd(3);
    h_DeltaVAnglem60->Draw();
    Can_TotDeltaHAngle->cd(4);
    h_DeltaVAnglem50->Draw();
    Can_TotDeltaHAngle->cd(5);
    h_DeltaVAnglem45->Draw();
    Can_TotDeltaHAngle->cd(6);
    h_DeltaVAnglem40->Draw();
    Can_TotDeltaHAngle->cd(7);
    h_DeltaVAnglem30->Draw();
    Can_TotDeltaHAngle->cd(8);
    h_DeltaVAnglem20->Draw();
    Can_TotDeltaHAngle->cd(9);
    h_DeltaVAnglem10->Draw();
    */
        /*
    Can_TotDeltaHAngle->Divide(3,3);
    Can_TotDeltaHAngle->cd(1);
    h_DeltaHAnglem80->Draw();
    Can_TotDeltaHAngle->cd(2);
    h_DeltaHAnglem70->Draw();
    Can_TotDeltaHAngle->cd(3);
    h_DeltaHAnglem60->Draw();
    Can_TotDeltaHAngle->cd(4);
    h_DeltaHAnglem50->Draw();
    Can_TotDeltaHAngle->cd(5);
    h_DeltaHAnglem45->Draw();
    Can_TotDeltaHAngle->cd(6);
    h_DeltaHAnglem40->Draw();
    Can_TotDeltaHAngle->cd(7);
    h_DeltaHAnglem30->Draw();
    Can_TotDeltaHAngle->cd(8);
    h_DeltaHAnglem20->Draw();
    Can_TotDeltaHAngle->cd(9);
    h_DeltaHAnglem10->Draw();
    */
    /*
    Can_TotDeltaHAngle->Divide(3,3);
    Can_TotDeltaHAngle->cd(1);
    h_DeltaHAngle80->Draw();
    Can_TotDeltaHAngle->cd(2);
    h_DeltaHAngle70->Draw();
    Can_TotDeltaHAngle->cd(3);
    h_DeltaHAngle60->Draw();
    Can_TotDeltaHAngle->cd(4);
    h_DeltaHAngle50->Draw();
    Can_TotDeltaHAngle->cd(5);
    h_DeltaHAngle45->Draw();
    Can_TotDeltaHAngle->cd(6);
    h_DeltaHAngle40->Draw();
    Can_TotDeltaHAngle->cd(7);
    h_DeltaHAngle30->Draw();
    Can_TotDeltaHAngle->cd(8);
    h_DeltaHAngle20->Draw();
    Can_TotDeltaHAngle->cd(9);
    h_DeltaHAngle10->Draw();
    */
    /*
    Can_DeltaLazyHAngle=new TCanvas("Can_DeltaLazyHAngle","Can_DeltaLazyHAngle",900,600);
    h_DeltaLazyHAngle->Draw();
    */

    if(AngleFlag){
      TCanvas* Can_angle=new TCanvas("Can_angle","Can_angle",900,900);
      Can_angle->Divide(2,1);
      Can_angle->cd(1);
      hHorizAngle->Draw();
      Can_angle->cd(2);
      hVertiAngle->Draw();
    }

    if(RangeFlag){
      Can_DeltaR=new TCanvas("Can_DeltaR","Can_DeltaR",900,600);
      Can_DeltaR->Divide(3,3);

      Can_DeltaR->cd(1);
      h_DeltaRW1->Draw();

      Can_DeltaR->cd(2);
      h_DeltaRW2->Draw();

      Can_DeltaR->cd(3);
      h_DeltaRW3->Draw();

      Can_DeltaR->cd(4);
      h_DeltaRP1->Draw();

      Can_DeltaR->cd(5);
      h_DeltaRP2->Draw();

      Can_DeltaR->cd(6);
      h_DeltaRP3->Draw();

      Can_DeltaR->cd(7);
      h_DeltaRXY->Draw();
    }

    if(DEdepFlag){
      Can_DEdep=new TCanvas("Can_DEdep","Can_DEdep",900,600);
      Can_DEdep->Divide(3,2);

      Can_DEdep->cd(1);
      h_DEdepR->Draw();
      TF1* fitgaus=new TF1("fitgaus","gaus",-1,1);
      h_DEdepR->Fit(fitgaus,"RQ","",-1,1);

      Can_DEdep->cd(2);
      h_DEdepR45->Draw();
      TF1* fitgaus1=new TF1("fitgaus1","gaus",-1,1);
      h_DEdepR45->Fit(fitgaus1,"RQ","",-1,1);

      Can_DEdep->cd(3);
      h_DEdepRPiotr->Draw();
      TF1* fitgausPiotr=new TF1("fitgausPiotr","gaus",-1,1);
      h_DEdepRPiotr->Fit(fitgausPiotr,"RQ","",-1,1);

      Can_DEdep->cd(4);
      h_DEdepX->Draw();
      TF1* fitgaus2=new TF1("fitgaus2","gaus",-1,1);
      h_DEdepX->Fit(fitgaus2,"RQ","",-1,1);

      Can_DEdep->cd(5);
      h_DEdepY->Draw();
      TF1* fitgaus3=new TF1("fitgaus3","gaus",-1,1);
      h_DEdepY->Fit(fitgaus3,"RQ","",-1,1);

      Can_DEdep->cd(6);
      h_DEdepXY->Draw();
      TF1* fitgaus4=new TF1("fitgaus4","gaus",-1,1);
      h_DEdepXY->Fit(fitgaus4,"RQ","",-1,1);
    }
    
    for(Int_t j=0;j<NVAngle;j++){
      cout<<"Double_t DeltaRW1_M[6]={"<<DeltaRW1_M[0][j]<<","<<DeltaRW1_M[1][j]<<","<<DeltaRW1_M[2][j]<<","<<DeltaRW1_M[3][j]<<","<<DeltaRW1_M[4][j]<<","<<DeltaRW1_M[5][j]<<"};"<<endl;
      cout<<"Double_t DeltaRW2_M[6]={"<<DeltaRW2_M[0][j]<<","<<DeltaRW2_M[1][j]<<","<<DeltaRW2_M[2][j]<<","<<DeltaRW2_M[3][j]<<","<<DeltaRW2_M[4][j]<<","<<DeltaRW2_M[5][j]<<"};"<<endl;
      cout<<"Double_t DeltaRW3_M[6]={"<<DeltaRW3_M[0][j]<<","<<DeltaRW3_M[1][j]<<","<<DeltaRW3_M[2][j]<<","<<DeltaRW3_M[3][j]<<","<<DeltaRW3_M[4][j]<<","<<DeltaRW3_M[5][j]<<"};"<<endl;

      cout<<"Double_t DeltaRP1_M[6]={"<<DeltaRP1_M[0][j]<<","<<DeltaRP1_M[1][j]<<","<<DeltaRP1_M[2][j]<<","<<DeltaRP1_M[3][j]<<","<<DeltaRP1_M[4][j]<<","<<DeltaRP1_M[5][j]<<"};"<<endl;
      cout<<"Double_t DeltaRP2_M[6]={"<<DeltaRP2_M[0][j]<<","<<DeltaRP2_M[1][j]<<","<<DeltaRP2_M[2][j]<<","<<DeltaRP2_M[3][j]<<","<<DeltaRP2_M[4][j]<<","<<DeltaRP2_M[5][j]<<"};"<<endl;
      cout<<"Double_t DeltaRP3_M[6]={"<<DeltaRP3_M[0][j]<<","<<DeltaRP3_M[1][j]<<","<<DeltaRP3_M[2][j]<<","<<DeltaRP3_M[3][j]<<","<<DeltaRP3_M[4][j]<<","<<DeltaRP3_M[5][j]<<"};"<<endl;

      cout<<"Double_t DeltaRXY_M[6]={"<<DeltaRXY_M[0][j]<<","<<DeltaRXY_M[1][j]<<","<<DeltaRXY_M[2][j]<<","<<DeltaRXY_M[3][j]<<","<<DeltaRXY_M[4][j]<<","<<DeltaRXY_M[5][j]<<"};"<<endl;

      cout<<"Double_t DeltaRW1_S[6]={"<<DeltaRW1_S[0][j]<<","<<DeltaRW1_S[1][j]<<","<<DeltaRW1_S[2][j]<<","<<DeltaRW1_S[3][j]<<","<<DeltaRW1_S[4][j]<<","<<DeltaRW1_S[5][j]<<"};"<<endl;
      cout<<"Double_t DeltaRW2_S[6]={"<<DeltaRW2_S[0][j]<<","<<DeltaRW2_S[1][j]<<","<<DeltaRW2_S[2][j]<<","<<DeltaRW2_S[3][j]<<","<<DeltaRW2_S[4][j]<<","<<DeltaRW2_S[5][j]<<"};"<<endl;
      cout<<"Double_t DeltaRW3_S[6]={"<<DeltaRW3_S[0][j]<<","<<DeltaRW3_S[1][j]<<","<<DeltaRW3_S[2][j]<<","<<DeltaRW3_S[3][j]<<","<<DeltaRW3_S[4][j]<<","<<DeltaRW3_S[5][j]<<"};"<<endl;

      cout<<"Double_t DeltaRP1_S[6]={"<<DeltaRP1_S[0][j]<<","<<DeltaRP1_S[1][j]<<","<<DeltaRP1_S[2][j]<<","<<DeltaRP1_S[3][j]<<","<<DeltaRP1_S[4][j]<<","<<DeltaRP1_S[5][j]<<"};"<<endl;
      cout<<"Double_t DeltaRP2_S[6]={"<<DeltaRP2_S[0][j]<<","<<DeltaRP2_S[1][j]<<","<<DeltaRP2_S[2][j]<<","<<DeltaRP2_S[3][j]<<","<<DeltaRP2_S[4][j]<<","<<DeltaRP2_S[5][j]<<"};"<<endl;
      cout<<"Double_t DeltaRP3_S[6]={"<<DeltaRP3_S[0][j]<<","<<DeltaRP3_S[1][j]<<","<<DeltaRP3_S[2][j]<<","<<DeltaRP3_S[3][j]<<","<<DeltaRP3_S[4][j]<<","<<DeltaRP3_S[5][j]<<"};"<<endl;

      cout<<"Double_t DeltaRXY_S[6]={"<<DeltaRXY_S[0][j]<<","<<DeltaRXY_S[1][j]<<","<<DeltaRXY_S[2][j]<<","<<DeltaRXY_S[3][j]<<","<<DeltaRXY_S[4][j]<<","<<DeltaRXY_S[5][j]<<"};"<<endl;
    }
    
  }//end of else

}


//*************************************************************************************************************************//
//*************************************************************************************************************************//
//*************************************************************************************************************************//
//*************************************************************************************************************************//
//*************************************************************************************************************************//
//*************************************************************************************************************************//
//*************************************************************************************************************************//
//*************************************************************************************************************************//
//*************************************************************************************************************************//
//*************************************************************************************************************************//


//Function that fits the pad track to a straight line using least squares
//Based on Hector analysisExample.C
//Removed the different geometries
//DPLoureiro 10072011

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

void FitStride(Double_t *STRIDENET, Double_t *STRIDEX, Double_t *STRIDEY, Double_t *STRIDEZ, Int_t NSTRIDE, Double_t threshold, Double_t &a, Double_t &b)
{
	Int_t S;
	Double_t A, B, C, UEV, Q, X, Xg, Y, Yg;

	A=B=C=UEV=Q=X=Y=Xg=Yg=0.;

        for(S=0 ; S<NSTRIDE ; S++){

	  if(STRIDENET[S]>threshold)
	    {
	      X= STRIDEX[S];
	      Y= STRIDEY[S];
	      Q+=STRIDENET[S];
	      Xg+=X*STRIDENET[S];
	      Yg+=Y*STRIDENET[S];
	      //cout<<"S "<<S<<" STRIDEX "<<STRIDEX[S]<<" STRIDEY "<<STRIDEY[S]<<" STRIDENET "<<STRIDENET[S]<<endl;
	    }

	}

	Xg/=Q;
	Yg/=Q;
	//cout<<"Xg: "<<Xg<<" Yg: "<<Yg<<endl;

        for(S=0 ; S<NSTRIDE ; S++){

	  if(STRIDENET[S]>threshold)
	    {
	      X= STRIDEX[S];
	      Y= STRIDEY[S];
	      A+=STRIDENET[S]*(X-Xg)*(X-Xg);
	      B+=STRIDENET[S]*(X-Xg)*(Y-Yg);
	      C+=STRIDENET[S]*(Y-Yg)*(Y-Yg);

	    }

	}

	UEV=0.5*(A+C+TMath::Sqrt((A+C)*(A+C)-4*(A*C-B*B)));
	a=B/(UEV-C);
	b=Yg-a*Xg;

}

void FitMat2(Double_t **PADNET, Double_t padSize, Double_t sideBlankSpace,Double_t xLength,Int_t Rmin,
	    Int_t Rmax, Int_t Cmin, Int_t Cmax, Double_t threshold, Double_t &a, Double_t &b){

  Int_t    Row, Col;
  Double_t A, B, C, UEV, Q, X, Xg, Y, Yg;
  A=B=C=UEV=Q=X=Y=Xg=Yg=0.;
  
  for(Row=Rmin;Row<=Rmax;Row++){
    for(Col=Cmin;Col<=Cmax;Col++){
      X=-xLength+(2*Row+1)*padSize/2;
      Y=(2*Col+1)*padSize/2;
      Q+=PADNET[Row][Col];
      Xg+=X*PADNET[Row][Col];
      Yg+=Y*PADNET[Row][Col];
      //if(PADNET[Row][Col])
	//cout<<X<<" "<<Y<<" "<<PADNET[Row][Col]<<endl;
    }
  }
  Xg/=Q;
  Yg/=Q;
  //cout<<endl;
  //cout<<Xg<<" "<<Yg<<" "<<Q<<endl;
  //cout<<threshold<<endl;
  for (Row=Rmin;Row<=Rmax;Row++){
    for(Col=Cmin;Col<=Cmax;Col++){
      if(PADNET[Row][Col]>threshold){
      X=-xLength+(2*Row+1)*padSize/2;
      Y=(2*Col+1)*padSize/2;
      A+=PADNET[Row][Col]*(X-Xg)*(X-Xg);
      B+=PADNET[Row][Col]*(X-Xg)*(Y-Yg);
      C+=PADNET[Row][Col]*(Y-Yg)*(Y-Yg);
      }
    }
  }
 
  //cout<<endl;
 //cout<<A<<" "<<B<<" "<<C<<endl;

  UEV=0.5*(A+C+sqrt((A+C)*(A+C)-4*(A*C-B*B)));
  a=B/(UEV-C);
  b=Yg-a*Xg;
}

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

Double_t FitStride3D(Double_t *STRIDENET, Double_t *STRIDEX, Double_t *STRIDEY, Double_t *STRIDEZ, Int_t NSTRIDE, Double_t threshold, MTrack* T)
{
	// adapted from: http://fr.scribd.com/doc/31477970/Regressions-et-trajectoires-3D
	Int_t S;
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

	Q=Xm=Ym=Zm=0.;
	Sxx=Syy=Szz=Sxy=Sxz=Syz=0.;

        for(S=0 ; S<NSTRIDE ; S++){

	  if(STRIDENET[S]>threshold)
	    {
	      X= STRIDEX[S];
	      Y= STRIDEY[S];
	      Z= STRIDEZ[S];
	      Q+=STRIDENET[S];
	      Xm+=X*STRIDENET[S];
	      Ym+=Y*STRIDENET[S];
	      Zm+=Z*STRIDENET[S];
	      Sxx+=X*X*STRIDENET[S];
	      Syy+=Y*Y*STRIDENET[S];
	      Szz+=Z*Z*STRIDENET[S];
	      Sxy+=X*Y*STRIDENET[S];
	      Sxz+=X*Z*STRIDENET[S];
	      Syz+=Y*Z*STRIDENET[S];
	      //cout<<"R "<<R<<" C "<<C<<" Y "<<Y<<" Ym "<<Ym<<endl;
	    }

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

void FitVertical(Double_t **padCharge,Double_t **padTime,Double_t padSize,Double_t drift_velocity,Double_t angle_proj,Int_t Rmin,
		 Int_t Rmax,Int_t Cmin,Int_t Cmax,Bool_t draw,Double_t &slope,Double_t &offset){

  Float_t max_charge_column=0;
  Float_t max_time_column=0;
  TGraph *gtime=new TGraph();
  Double_t zmin=(2*Cmin+1)*padSize/2.;
  Double_t zmax=(2*Cmax+1)*padSize/2.;
  Int_t k=0;
  for(Int_t m=Cmin;m<Cmax;m++){
    max_charge_column=0;
    max_time_column=0;
      Double_t zPad=0.;
    for(Int_t l=Rmin;l<Rmax;l++){
      //Double_t zPad=0.;
      //padTime[l][m]=padTime[l][m]/padCharge[l][m];
      if(padCharge[l][m]>max_charge_column){
	max_charge_column=padCharge[l][m];
	max_time_column=padTime[l][m];
      }
      else
	continue;
      //if(padCharge[l][m])
      //	cout<<l<<"Maximum "<<max_charge_column<<" "<<max_time_column<<" Pad "<<padCharge[l][m]<<" "<<padTime[l][m]<<endl;
      zPad = (2*m+1)*padSize/2.;
      //if(max_time_column>0){
      //gtime->SetPoint(k,zPad/TMath::Cos(angle_proj),max_time_column);
      //k++;
      //}
    }
      if(max_time_column>0){
	gtime->SetPoint(k,zPad/TMath::Cos(angle_proj),max_time_column);
	k++;
	//cout<<"**************************************************"<<endl;
      }
  }
  //gtime->Print();
  if(draw&&gtime->GetN()>1)
    gtime->Draw("A*");
  else if(draw&&gtime->GetN()<1) cout<<"ERROR! No points"<<endl;
  TF1 *fun =new TF1("fun","pol1",zmin/TMath::Cos(angle_proj),zmax/TMath::Cos(angle_proj));
  if(draw)
  gtime->Fit("fun","RQ");
  else
  gtime->Fit("fun","RQN");
  slope=fun->GetParameter(1)*drift_velocity;
  offset=fun->GetParameter(0)*drift_velocity;
  //cout<<"par1----->"<<fun->GetParameter(1)<<endl;
  //cout<<"velocity----->"<<drift_velocity<<endl;
  //cout<<"slope----->"<<slope<<" offset--->"<<offset<<endl;
}



void FitVertical_X(Double_t **padCharge,Double_t **padTime,Double_t padSize,Double_t drift_velocity,Double_t xLength,Double_t angle_proj,Int_t Rmin,
		 Int_t Rmax,Int_t Cmin,Int_t Cmax,Bool_t draw,Double_t &slope,Double_t &offset){

  Float_t max_charge_row=0;
  Float_t max_time_row=0;
  Float_t max_column=0;
  TGraph *gtime=new TGraph();
  Double_t xmin=-xLength+(2*Rmin+1)*padSize/2.;
  Double_t xmax=-xLength+(2*Rmax+1)*padSize/2.;
  Int_t k=0;
  
  for(Int_t l=Rmin;l<Rmax;l++){
	  max_charge_row=0;
	  max_time_row=0;
	  max_column=0;
	  for(Int_t m=Cmin;m<Cmax;m++){ //Rows
	    Double_t xPad=0.,zPad=0.,yPad=0.;	      
	    if(padCharge[l][m]>max_charge_row){
	      max_charge_row=padCharge[l][m];
	      max_time_row=padTime[l][m];
	      //zPad = (2*m+1)*padSize/2.;
	    }	
	    else continue;      
	    xPad = -xLength + (2*l+1)*padSize/2.;	      	      
	  }
	  xPad =-xLength+(2*l+1)*padSize/2.;
	  
	  if(max_time_row>0){	
	    gtime->SetPoint(k,xPad/TMath::Sin(angle_proj),max_time_row);
	    k++;
	  }	  	  
  }//End of Loop on pads
  //gtime->Print();
  if(draw&&gtime->GetN()>1)
    gtime->Draw("A*");
  else if(draw&&gtime->GetN()<1) cout<<"ERROR! No points"<<endl;
  TF1 *fun =new TF1("fun","pol1",xmin/TMath::Sin(angle_proj),xmax/TMath::Sin(angle_proj));
  if(draw){
  gtime->Fit("fun","RQ");
  //fun->Draw("");
  }
  else
  gtime->Fit("fun","RQN");
  slope=fun->GetParameter(1)*drift_velocity;
  offset=fun->GetParameter(0)*drift_velocity;
  //cout<<"par1----->"<<fun->GetParameter(1)<<endl;
  //cout<<"velocity----->"<<drift_velocity<<endl;
  //cout<<"slope----->"<<slope<<" offset--->"<<offset<<endl;
}


void CentroFit(Double_t **PADNET, Double_t padSize, Double_t sideBlankSpace,Double_t xLength,Int_t Rmin, Int_t Rmax, Int_t Cmin, Int_t Cmax, Double_t threshold, Double_t &a, Double_t &b){

  Int_t numberOfRows= Rmax-Rmin+1;
  Int_t numberOfColumns= Cmax-Cmin+1;
  Double_t *xg=new Double_t[numberOfColumns*numberOfRows];
  Double_t *yg=new Double_t[numberOfColumns*numberOfRows];

  for(Int_t l=0;l<(numberOfColumns*numberOfRows);l++){
      xg[l]=0.;
      yg[l]=0.;
    }

 cout<<" numberOfRows->"<< numberOfRows<<endl;
 cout<<" numberOfColumns->"<< numberOfColumns<<endl;
 
 Double_t a0[2];
 Int_t j=0;
 Double_t sum;
 for(Int_t Col=Cmin;Col<=Cmax;Col++){
   sum=0;
   xg[j]=(2*Col+1)*padSize/2;
   for(Int_t Row=Rmin;Row<=Rmax;Row++){
     yg[j]+=PADNET[Row][Col]*(-xLength+(2*Row+1)*padSize/2);
     sum+=PADNET[Row][Col];
   }
   if(sum!=0){
     yg[j]=yg[j]/sum;
     //cout<<Row<<" "<<Col<<" xg["<<j<<"]->"<<xg[j]<<"\t yg["<<j<<"]->"<<yg[j]<<endl;
     j++;
    }
  }
 TGraph *g=new TGraph(j,xg,yg);
 //g->Draw("A*");
 g->LeastSquareFit(2,a0,0.,xg[j]);
 a=a0[1];
 b=a0[0];
 cout<<"a->"<<a0[1]<<endl;
 cout<<"b->"<<a0[0]<<endl;
 cout<<"****************************************************"<<endl;
 delete []xg;
 delete []yg;
  
}

void GauFit(Double_t **PADNET, Double_t padSize, Double_t sideBlankSpace,Double_t xLength,Int_t Rmin, Int_t Rmax, Int_t Cmin, Int_t Cmax, Double_t threshold, Double_t &a, Double_t &b){

  Int_t numberOfRows= Rmax-Rmin+1;
  Int_t numberOfColumns= Cmax-Cmin+1;

  Double_t *xg=new Double_t[numberOfColumns*numberOfRows];
  Double_t *yg=new Double_t[numberOfColumns*numberOfRows];

  TF1 *ff=new TF1("ff","gaus",Rmin,Rmax); 
  TH1D *y=new TH1D("h","h", numberOfRows,Rmin,Rmax);

  Double_t a0[2];
  Int_t j=0;

 for(Int_t l=0;l<(numberOfColumns*numberOfRows);l++){
      xg[l]=0.;
      yg[l]=0.;
    }
 
 for(Int_t Col=Cmin;Col<=Cmax;Col++){
   xg[j]=(2*Col+1)*padSize/2;
   for(Int_t Row=Rmin;Row<=Rmax;Row++){
     if(PADNET[Row][Col]!=0){
       y->SetBinContent(Row,PADNET[Row][Col]);
       cout<<Row<<" "<<Col<<" "<<PADNET[Row][Col]<<" "<<y->GetBinContent(Row)<<endl;
     }
     else
       continue;
   }
   cout<<"****************************************************"<<endl;
   ff->SetParameters(1,0);
   if(y->GetEntries()!=0){;
     y->Print();
     y->Fit("ff","R0");
     yg[j]=ff->GetParameter(1);
     cout<<yg[j]<<endl;
   }	    
   j++;
 }
 
 TGraph *g=new TGraph(j,xg,yg);
 //g->Draw("A*");
 g->LeastSquareFit(2,a0,0.,xg[j]);
 a=a0[1];
 b=a0[0];
 cout<<"a->"<<a0[1]<<endl;
 cout<<"b->"<<a0[0]<<endl;
 cout<<"****************************************************"<<endl;
 delete []xg;
 delete []yg;
  
}

void FindMax(TSpline3 *sp,Float_t *maximum,Float_t *x){
  Float_t max;
  Float_t posmax=0;
  Float_t x0=60;
  Float_t xmax=300;
  Float_t step=0.2;
  for(Float_t val=x0;val<xmax;val=val+step){
    Float_t value=sp->Eval(val);
    if(value>max){
      max=value;
      posmax=val;
    }
    //cout<<val<<" "<<value<<endl;
  }
  // cout<<"Maximum is at--> "<<posmax<<" "<<max<<endl;
  *maximum=max;
  *x=posmax;
  return; 
}

void FindRange(TSpline3 *sp,Float_t maxval,Float_t maxpos,Float_t theta,Int_t side,Float_t *x){
  Float_t max;
  Float_t posmax=0;
  Float_t x0=maxpos;
  Float_t xmax=300;
  Float_t step=0.2;
  if((side==1&&theta>0)||(side==2&&theta<0)){
    for(Float_t val=x0;val<=xmax;val=val+step){
    Float_t value=sp->Eval(val);
    if(value>=maxval/4){
      max=value;
      posmax=val;
    }
    }
  }
    // cout<<val<<" "<<value<<endl;
  else if((side==2&&theta>0)||(side==1&&theta<0)){
    for(Float_t val=300;val>=0;val=val-step){
    Float_t value=sp->Eval(val);
    if(value>=maxval/4){
      max=value;
      posmax=val;
    }
    }

  }
  //cout<<"Range is at--> "<<posmax<<" "<<max<<endl;
   //*maximum=max;
  *x=posmax;
  return; 
}

void FindRangeX(TSpline3 *sp,Float_t maxval,Float_t maxpos,Float_t *x){
  Float_t max;
  Float_t posmax=0;
  Float_t x0=maxpos;
  Float_t xmax=150;
  Float_t step=0.2;
  for(Float_t val=x0;val<xmax;val=val+step){
    Float_t value=sp->Eval(val);
    if(value>=maxval/4){
      max=value;
      posmax=val;
    }
    // cout<<val<<" "<<value<<endl;
  }
  //cout<<"Range is at--> "<<posmax<<" "<<max<<endl;
   //*maximum=max;
  *x=posmax;
  return; 
}


void peaks(TH1F *h,Int_t np=10){
npeaks=TMath::Abs(np);
//Use TSpectrum to find the peak candidates
 TSpectrum *s = new TSpectrum(2*npeaks);
 Int_t nfound = s->Search(h,2,"",0.05);
 printf("Found %d candidate peaks to fit\n",nfound);
 Double_t *par=new Double_t[3*nfound];
 //Loop on all found peaks. Eliminate peaks at the background level
 Float_t *xpeaks = s->GetPositionX();
 for (Int_t p=0;p<nfound;p++) {
   Float_t xp = xpeaks[p];
   Int_t bin = h->GetXaxis()->FindBin(xp);
   Float_t yp = h->GetBinContent(bin);
   par[3*p] = yp;
   par[3*p+1] = xp;
   par[3*p+2] = 0.5;
 }
 TF1 *fit = new TF1("fit",fpeaks,-1,50,3*nfound);
 //we may have more than the default 25 parameters
 TVirtualFitter::Fitter(h,10+3*npeaks);
 fit->SetParameters(par);
 fit->SetNpx(1000);
 h->Fit("fit");
 Double_t mean_sigma;
 Double_t mean_error;
 for(Int_t m=0;m<npeaks;m++){
   mean_sigma+=fit->GetParameter(3*m+2);
   mean_error+=fit->GetParError(3*m+2);
 }
 mean_sigma/=nfound;
 mean_error/=nfound;
 cout<<npeaks<<"Mean sigma \t"<<mean_sigma<<" Mean error\t"<<mean_error<<endl;

 errors(mean_sigma,mean_error);
}

Double_t fpeaks(Double_t *x,Double_t *par){
 Double_t result=0;
 for (Int_t p=0;p<npeaks;p++) {
   Double_t norm  = par[3*p];
   Double_t mean  = par[3*p+1];
   Double_t sigma = par[3*p+2];
   result += norm*TMath::Gaus(x[0],mean,sigma);
 }
 return result; 
}


void errors(Double_t sigma,Double_t error,Double_t distance=185.){

  Double_t FWHM= 2.35*(TMath::ATan(sigma/distance)*180/TMath::Pi());

  Double_t error_FWHM=2.35*180/TMath::Pi()*(1./(1+pow(sigma/distance,2)))/distance*error;
  
  cout<<distance<<"\t"<<FWHM<<"\t"<<error_FWHM<<endl;
}

Double_t erf(Double_t *x,Double_t *par){

  Float_t val=par[0]/(par[2]-par[1])*(TMath::Erf((x[0]-par[1])/par[3])-TMath::Erf((x[0]-par[2])/par[3]));

  return val;
  
}

void XYProjection(Double_t **padCharge, Double_t *Start, Double_t *Stop, Int_t Rmin, Int_t Rmax, Int_t Cmin, Int_t Cmax, Double_t threshold, TH1F* h_scat_rangeX, TH1F* h_scat_rangeY)
{
  Double_t PI=3.1415926535897932384626433;
  Double_t deg=180./PI;
  Double_t rad=PI/180.;

  Double_t VAngle,HAngle;
  Double_t range;

  VAngle=TMath::ATan((Stop[2]-Start[2])/TMath::Sqrt((Stop[0]-Start[0])*(Stop[0]-Start[0])+(Stop[1]-Start[1])*(Stop[1]-Start[1])))*deg;
  if(Stop[0]-Start[0]>0){
    HAngle=TMath::ATan((Stop[1]-Start[1])/(Stop[0]-Start[0]))*deg;
  }
  else{
    HAngle=180.+TMath::ATan((Stop[1]-Start[1])/(Stop[0]-Start[0]))*deg;
  }

  //cout<<"VAngle :"<<VAngle<<", HAngle :"<<HAngle<<endl;

  h_scat_rangeX->Reset();
  h_scat_rangeY->Reset();

  for(Int_t r=Rmin;r<Rmax+1;r++){//Rows
    for(Int_t c=Cmin;c<Cmax+1;c++){ //columns
      Double_t xPad=0.,yPad=0.,zPad=0.,xProj=0.,yProj=0.;
      if(padCharge[r][c] > threshold){
	xPad = (2*c+1.);
	yPad = (2*r+1.);

	h_scat_rangeX->Fill(xPad,padCharge[r][c]);
	h_scat_rangeY->Fill(yPad,padCharge[r][c]);

      }	    
    }

  }//End of Loop on pads

}

void TrackPadProjection(Double_t **padCharge, Double_t **padHeight, Double_t *Start, Double_t *Stop, Int_t Rmin, Int_t Rmax, Int_t Cmin, Int_t Cmax, Double_t threshold, TH1F* h_scat_range)
{
  Double_t PI=3.1415926535897932384626433;
  Double_t deg=180./PI;
  Double_t rad=PI/180.;

  Double_t VAngle,HAngle;
  Double_t range;

  //Double_t QTot=0.;

  VAngle=TMath::ATan((Stop[2]-Start[2])/TMath::Sqrt((Stop[0]-Start[0])*(Stop[0]-Start[0])+(Stop[1]-Start[1])*(Stop[1]-Start[1])))*deg;
  if(Stop[0]-Start[0]>0){
    HAngle=TMath::ATan((Stop[1]-Start[1])/(Stop[0]-Start[0]))*deg;
  }
  else{
    HAngle=180.+TMath::ATan((Stop[1]-Start[1])/(Stop[0]-Start[0]))*deg;
  }

  //cout<<"VAngle :"<<VAngle<<", HAngle :"<<HAngle<<endl;

  TVector3 v2(Stop[0]-Start[0],Stop[1]-Start[1],Stop[2]-Start[2]);
  Double_t mag=v2.Mag();

  h_scat_range->Reset();

  for(Int_t r=Rmin;r<Rmax+1;r++){//Rows
    for(Int_t c=Cmin;c<Cmax+1;c++){ //columns
      Double_t xPad=0.,yPad=0.,zPad=0.,xProj=0.,yProj=0.;
      if(padCharge[r][c] > threshold){
	xPad = (2*c+1.);
	yPad = (2*r+1.);
	zPad = padHeight[r][c];

	//QTot+=padCharge[r][c];

	TVector3 v1(xPad-Start[0],yPad-Start[1],zPad-Start[2]);
	range=v1*v2/mag;

	h_scat_range->Fill(range,padCharge[r][c]);
      }	    
    }

  }//End of Loop on pads

  //cout<<"QTot Proj Meth "<<QTot<<endl;

}

void WeightedPadProjection(Double_t **padCharge, Double_t **padHeight, Double_t *Start, Double_t *Stop, Int_t Rmin, Int_t Rmax, Int_t Cmin, Int_t Cmax, Double_t threshold, Int_t nbins, TH1F* h_bragg, TH1F* h_latspread)
{
  Double_t PI=3.1415926535897932384626433;
  Double_t deg=180./PI;
  Double_t rad=PI/180.;

  Double_t VAngle,HAngle;
  Double_t range,latrange;
  Double_t lat2;
  Double_t lsig;

  Double_t binSize=h_bragg->GetBinWidth(0);
  Int_t GetNBinX=h_bragg->GetNbinsX();
  //cout<<"NBinX "<<GetNBinX<<endl;
  //const Int_t NBinX=h_bragg->GetNbinsX();
  const Int_t NBinX=GetNBinX;

  //const Int_t latbin=h_latspread->GetNbinsX();;

  //Int_t NTrans[latbin],lbin;
  //Double_t SumTrans[latbin];
  Int_t NTrans[300],lbin;
  Double_t SumdQ[300];
  Double_t SumTrans[300];
  //TH1F *hLatRange[NBinX];

  for(Int_t i=0;i<300;i++){
    NTrans[i]=0;
    SumTrans[i]=SumdQ[i]=0.;
    char title[256];
    sprintf(title,"Lateral Range at %d mm",i);
    //hLatRange[i] = new TH1F(title,title,100, -20., 20.);
  }

  VAngle=TMath::ATan((Stop[2]-Start[2])/TMath::Sqrt((Stop[0]-Start[0])*(Stop[0]-Start[0])+(Stop[1]-Start[1])*(Stop[1]-Start[1])))*deg;
  if(Stop[0]-Start[0]>0){
    HAngle=TMath::ATan((Stop[1]-Start[1])/(Stop[0]-Start[0]))*deg;
  }
  else{
    HAngle=180.+TMath::ATan((Stop[1]-Start[1])/(Stop[0]-Start[0]))*deg;
  }

  //cout<<"VAngle :"<<VAngle<<", HAngle :"<<HAngle<<endl;

  TVector3 v2(Stop[0]-Start[0],Stop[1]-Start[1],Stop[2]-Start[2]);
  Double_t mag=v2.Mag();

  //Perpendicular vector to v2 in XY plane and direction right to tracks
  //Double_t Perp[3]={Start[0]+1,};
  //Perp[0]=Start[0]+1;
  //Perp[1]=(Perp[0]-Start[0])*(Stop[1]-Start[1])-Start[1]*(Stop[0]-Start[0])-1;
  //Perp[2]=0;

  //TVector3 v2t(Perp[0],Perp[1],Perp[2]);
  //Double_t magt=v2t.Mag();

  TVector3 v2pad(Stop[0]-Start[0],Stop[1]-Start[1],0);
  Double_t magpad=v2pad.Mag();

  h_bragg->Reset();
  h_latspread->Reset();

  //Pad charge projection method on the fitted track by weighted slices (Piotr)
  ///¡¡¡ Only odd ntimes !!!///
  const Int_t ntimes=nbins;

  Double_t dR[ntimes];
  Double_t dQ[ntimes];

  Double_t Angle=0;
  if(HAngle>=0 && HAngle<=45)Angle=HAngle;
  else if(HAngle>45 && HAngle<=90)Angle=90-HAngle;
  else if(HAngle>90 && HAngle<=135)Angle=HAngle-90;
  else if(HAngle>135 && HAngle<=180)Angle=180-HAngle;
  else if(HAngle>=-45 && HAngle<0)Angle=-HAngle;
  else if(HAngle>=-90 && HAngle<-45)Angle=90+HAngle;
  else if(HAngle>=-135 && HAngle<-90)Angle=-90-HAngle;
  else if(HAngle>=-180 && HAngle<-135)Angle=180+HAngle;

  //Double_t dl=2*TMath::Sqrt(2)*TMath::Abs(TMath::Cos((45-Angle)*rad))/(Double_t)ntimes;
  Double_t dl=2*(TMath::Cos(Angle*rad)+TMath::Sin(Angle*rad))/(Double_t)ntimes;

  //We divide by TMath::Cos(VAngle*rad) to project on 3D
  Double_t dl3=dl/TMath::Cos(VAngle*rad);

  Double_t sumSi;
  Double_t SurfTot,Surfi,SurfTriCor,SurfSPar,SurfBPar,xi,dxi;

  for(Int_t r=Rmin;r<Rmax+1;r++){//Rows
    for(Int_t c=Cmin;c<Cmax+1;c++){ //columns
      Double_t xPad=0.,yPad=0.,zPad=0.,xProj=0.,yProj=0.;
      if(padCharge[r][c] >threshold){
	xPad = (2*c+1.);
	yPad = (2*r+1.);
	zPad = padHeight[r][c];

	TVector3 v1(xPad-Start[0],yPad-Start[1],zPad-Start[2]);
	TVector3 v1pad(xPad-Start[0],yPad-Start[1],0);
	range=v1*v2/mag;
	latrange=v1pad*v2pad/magpad;
	//lat2=v1*v1-range*range;
	lat2=v1pad*v1pad-latrange*latrange;

	sumSi=0;
	SurfTot=0;//Total Surface of each step band
	Surfi=0;//Surface of each step band inside the pad
	SurfTriCor=0;//outside triangle Surface of the corner step
	SurfSPar=0;//Surface of each small parallelepiped outside of the pad
	SurfBPar=0;//Surface of each big parallelepiped outside of the pad
	//SurfTri=0;//Surface of each triangle outside of the pad

	dxi=dl/TMath::Abs(TMath::Cos(Angle*rad));

	SurfTot=(1+TMath::Tan(Angle*rad))*4/(Double_t)ntimes;
	SurfBPar=2*dl*dl/TMath::Sin(2*Angle*rad);

	//cout<<"HAngle "<<HAngle<<" TMath::Cos(theta) "<<TMath::Cos(HAngle*rad)<<" dl "<<dl<<" dxi "<<dxi<<endl;

	for(Int_t i=1;i<=ntimes/2;i++){//only needs half pad projection
	  Surfi=0;
	  xi=TMath::Tan(Angle*rad)+i*dl/TMath::Cos(Angle*rad);

	  //SurfTot=2*dl/TMath::Cos(HAngle*rad);

	  if(xi<=1){Surfi=SurfTot;}
	  else if(xi>1 && xi<1+dxi){SurfTriCor=(xi-1)*(xi-1)/TMath::Tan(Angle*rad)/2; Surfi=SurfTot-SurfTriCor; SurfSPar=dl*(xi-1)/TMath::Sin(Angle*rad);}
	  else if(xi>=1+dxi){Surfi=SurfTot-SurfSPar-((Int_t)((xi-1)/dxi)-1/2)*SurfBPar;}

	  sumSi+=Surfi;

	  dR[ntimes/2-1+i]=range + dl3/2 + ((Double_t)i-1)*dl3;
	  dR[ntimes/2-i]=range - dl3/2 - ((Double_t)i-1)*dl3;
	  dQ[ntimes/2-1+i]=dQ[ntimes/2-i]=(Surfi/4)*padCharge[r][c]/binSize;

	  lbin=(int)dR[ntimes/2-1+i];
	  NTrans[lbin]++;
	  SumdQ[lbin]+=dQ[ntimes/2-1+i];
	  SumTrans[lbin]+=lat2*dQ[ntimes/2-1+i];
	  //hLatRange[lbin]->Fill(latrange,dQ[ntimes/2-1+i]);

	  lbin=(int)dR[ntimes/2-i];
	  NTrans[lbin]++;
	  SumdQ[lbin]+=dQ[ntimes/2-i];
	  SumTrans[lbin]+=lat2*dQ[ntimes/2-i];
	  //hLatRange[lbin]->Fill(latrange,dQ[ntimes/2-i]);

	  h_bragg->Fill(dR[ntimes/2-1+i],dQ[ntimes/2-1+i]);
	  //second half
	  h_bragg->Fill(dR[ntimes/2-i],dQ[ntimes/2-i]);
	}	  
      }	    
    }

  }//End of Loop on pads

  //TCanvas* Can_latrange=new TCanvas("Can_latrange","Can_latrange",900,900);

  for(Int_t i=0;i<300;i++){
    //if(NTrans[i]!=0)lsig=TMath::Sqrt(SumTrans[i])/NTrans[i];
    //if(NTrans[i]!=0)lsig=TMath::Sqrt(SumTrans[i]/NTrans[i]);
    if(NTrans[i]!=0 && SumdQ[i]!=0)lsig=TMath::Sqrt(SumTrans[i]/SumdQ[i]);
    else lsig=0;

    lsig/=binSize;

    //cout<<i<<" "<<NTrans[i]<<" "<<lsig<<endl;
    h_latspread->Fill(i+0.5,lsig);
    
    //Can_latrange->cd();
    //hLatRange[i]->SetLineWidth(2);
    //hLatRange[i]->Draw();
    //Can_latrange->WaitPrimitive();
    
  }
}
