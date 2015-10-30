///////////////////////////////////////////////////////////////////
//*-- AUTHOR : Piotr Konczykowski
//*-- Date: 10/2015
//*-- Last Update: 29/10/15
//*-- Copyright: GENP (Univ. Santiago de Compostela)
//
// --------------------------------------------------------------
//
// --------------------------------------------------------------
//
// Common analysis program for ActarSim and Actar data.
// Adapted for the IPNO experiment on the Actar demonstrator
// for the particle identification.
// Select tracks corresponding to a silicon hit, fit it and
// calculate the energy deposit
//
// --------------------------------------------------------------
// How to run this program:
// 1 - Run the simulation
// 2 - Run the digitization
// 3 - Run this macro inside root
//      gSystem->Load("actarsim.sl");
//      .L Analysis_reduce.C+;
//      Analysis_reduce("inputefile","outputFile")

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


void Analysis_reduce(char* inputFile, char* outputFile){
  //For the Particle Identification, choose the portion of track from which the energy deposit is calculated
  Double_t DeltaL=20.;//size of the track portion
  Double_t LOutMax=80.;//distance from the end point on the silicon

  ///FLAGS///
  //Event by event
  Bool_t Track3DVisuFlag=0;
  Bool_t TrackVisuFlag=1;
  Bool_t TrackSourcePosFlag=0;
  Bool_t TrackAngleFlag=0;
  Bool_t TrackBragFlag=0;
  Bool_t TrackRangeFlag=0;
  Bool_t MultiThetaFlag=0;

  Bool_t EventFlag=0;// 0: all event, 1: event by event
  Bool_t ReadWriteFlag=0;// 0: Read, 1: Write
  Int_t DEBUGFLAG=1;// 0: quiet, 1: general, 2: precise

  //At the end of analysis
  Bool_t DeltaTimeFlag=0;
  Bool_t SigmaFlag=0;
  Bool_t ChiFlag=0;
  Bool_t RangeFlag=0;
  Bool_t AngleFlag=0;
  Bool_t DistFlag=0;
  Bool_t RangeVsAngleFlag=0;
  Bool_t SourceFlag=0;
  Bool_t EndTrackFlag=0;
  Bool_t VertexFlag=0;
  Bool_t IdentFlag=1;
  Bool_t PIdentFlag=1;

  Double_t driftVelocity=6.865e-3;

  Bool_t simuFlag;
  Bool_t gasflag;
  Char_t *gasname;
  Double_t sampling;
  char* dEvsETitle;

  cout << "Reading Simulation (1) or real data (0)? ";
  cin >> simuFlag;
  if(simuFlag){
    dEvsETitle="dE vs E, 80 MeV 12C, Track_length>80mm+DeltaL, dl=20mm, ionGasModel, Pad & Si Res";
    gSystem->Load("libactar.sl");
    TFile *f = new TFile(inputFile);
    TTree *t2 = (TTree*)f->Get("out_tree");

    Int_t nentries=t2->GetEntries();
    cout<<"Number of entries : "<<nentries<<endl;
    sampling=1.;
  }
  else if(!simuFlag){
    dEvsETitle="dE vs E, Track_length>80mm+#deltal, #deltal=20mm";
    TFile *f = new TFile(inputFile);
    TTree *t2 = (TTree*)f->Get("pad_signal");

    Int_t nentries=t2->GetEntries();
    cout<<"Number of entries : "<<nentries<<endl;
    sampling=80.;

    //Calibration parameters for the IPNO experiment
    Double_t MAYACAL[12][2];
    Double_t tmpCAL0[12]={0.006281,0.006195,0.006505,0.006501,0.006290,0.007061,0.006469,0.006497,0.006662,0.006555,0.006574,0.006419};
    Double_t tmpCAL1[12]={0.13,0.18,-0.10,-0.04,0.14,-0.38,0.11,0.13,-0.06,0.01,0.00,0.13};
    for(Int_t i=0;i<12;i++){
      MAYACAL[i][0]=tmpCAL0[i];
      MAYACAL[i][1]=tmpCAL1[i];
    }
    driftVelocity*=sampling;
  }

  const Int_t numberOfRows=32;
  const Int_t numberOfColumns=64;

  Int_t beamWidth   = 2;
  Int_t marginWidth = 1;

  Double_t PI=3.1415926535897932384626433;
  Double_t deg=180./PI;
  Double_t rad=PI/180.;

  //
  for(const Int_t t=0;t<2;t++) MTrack* T[t]=new MTrack();

  if(ReadWriteFlag){
    TFile *outfile = new TFile(outputFile,"RECREATE");
    TTree *out_tree = new TTree("out_tree","out_tree");
  }

  //Canvas
  if(Track3DVisuFlag) Can_3D=new TCanvas("Can_3D","Can_3D",900,900);
  if(TrackVisuFlag){
    Can_track=new TCanvas("Can_track","Can_track",900,900);
    Can_track->Divide(1,2);
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

  //
  Double_t Energy_in_silicon=0.;
  Double_t EnergySil;
  Int_t detectorID;
  Double_t SilPosX, SilPosY, SilPosZ;

  Double_t Qtot;

  TSpline3 *bragR, *bragR1, *bragR2;

  Int_t minRowtmp=0, maxRowtmp=31;
  Int_t minColumntmp=0, maxRowtmp=63;

  Int_t minRow[2],maxRow[2],minColumn[2],maxRow[2];

  Double_t Vertex_posX,Vertex_posY;

  Double_t maxZ=170.;
  if(simuFlag==0) maxZ=600.;

  Double_t threshold = 0;
  Double_t timeThreshold = 1.;
  Double_t outThresh=10000.;
  if(simuFlag==0) {
    threshold = 0.;
    outThresh=1000.;
  }

  Double_t tSourceX,tSourceY,t0;
  Double_t SourcePosX,SourcePosZ,SourcePosY;

  Double_t range,range2,rangeX,rangeY;
  Double_t Rm,Cm,Zcor,Zm;
  Int_t min_col,max_col,ncol;
  Int_t min_row,max_row,nrow;
  Bool_t out_track;

  Double_t PadToSilX=51.8;
  Double_t PadToSilY=58.8;

  Double_t a,b;

  Double_t IniPoint[2][2],FinPoint[2][2],IniFitPoint[2][2],FinFitPoint[2][2];
  Double_t IniPoint1[2],FinPoint1[2],IniFitPoint1[2],FinFitPoint1[2];
  Double_t IniPoint2[2],FinPoint2[2],IniFitPoint2[2],FinFitPoint2[2];

  Double_t Start[3],Out[3],End[3];

  Double_t PointA[3][2],PointB[3][2];

  Double_t xv,yv,zv;

  Double_t RangeA[2],RangeB[2],RangeO[2];

  Double_t dist_VO,dist_VF,dist_FO;

  Double_t phi;
  Double_t theta;
  Double_t VAngle;
  Double_t HAngle;
  Double_t Angle3D;

  Double_t xmaxval, xmax, ymaxval, ymax, zmaxval, zmax;
  Double_t range_calcX, range_calcZ, range_calcY;

  Double_t trackX,trackZ,trackY,trackZ2;
  Double_t trackPadX,trackPadZ,trackPadY;
  Double_t track_range,track_rangeXY,track_rangePadXY;

  //3 alphas source position inside Actar
  Double_t SourceX=64;
  Double_t SourceY=-37.43;

  //Matrix for the charge map
  Double_t **padCharge=new Double_t*[numberOfRows];
  Double_t **padTime=new Double_t*[numberOfRows];
  Double_t **padHeight=new Double_t*[numberOfRows];
  for(Int_t j=0;j<numberOfRows;j++){
    padCharge[j]=new Double_t[numberOfColumns];
    padTime[j]=new Double_t[numberOfColumns];
    padHeight[j]=new Double_t[numberOfColumns];
  }

  Double_t PadESig;

  TMatrixD *padChargeTest;
  TMatrixD *padTimeTest;

  Bool_t DSSD_1,DSSD_2,DSSD_3,DSSD_4;
  Bool_t SilLeft,SilRight,SilFront;
  Int_t SilHitID;

  Int_t NTrackOut;

  // Parameters of the tracks on the left and right of beam
  Int_t minColumnleft,minColumnright,maxRowleft,maxRowright;
  Int_t minColumnbeamleft,minColumnbeamright,maxRowbeamleft,maxRowbeamright;//min max columns of track going out of beam
  Int_t tmpminColumnbeamleft,tmpminColumnbeamright,tmpmaxRowbeamleft,tmpmaxRowbeamright;
  Int_t minColumnoutleft,minColumnoutright,maxRowoutleft,maxRowoutright;//min max columns of track going out of gasbox
  Int_t NCBeamoutleft,NCBeamoutright,NCoutleft,NCoutright;
  Int_t minpadout=2;
  Int_t maxpadout=6;
  Bool_t StopLeft=1;
  Bool_t StopRight=1;
  Int_t tmp;
  Bool_t scatside,trackleft,trackright,trackfront1,trackfront2,outbeamtrackleft,outbeamtrackright,GOOD_EVENT;

  // Calculating the Scatter Exit Pads (and if there's one, the recoil's exit pad) from the last Column's Rows
  Int_t minRow1,maxRow1;
  Int_t minRow2,maxRow2;
  Int_t NRout1,NRout2;
  //Double_t RoutCharge1,RoutCharge2,AvPadout1,AvPadout2;
  Int_t config=0;

  // Particle identification
  Int_t particleID;
  Int_t smin,smax;
  Bool_t track1,track2;
  Int_t trackout[2];//0:track left, 1:track right, 2:track front left, 3:track front left
  Int_t SilWall[2];//0:DSSD1_2, 1:DSSD3_4, 2:MAYA LEFT, 3:MAYA RIGHT

  //Silicon charge
  Int_t SilID[16];

  TVectorD *SilCharge;
  TVectorD *SilIDV;

  Int_t nbsiliconhits;

  t2->SetBranchAddress("PadCharge",&padChargeTest);
  t2->SetBranchAddress("PadTime",&padTimeTest);
  t2->SetBranchAddress("SilCharge",&SilCharge);
  if(simuFlag) t2->SetBranchAddress("SilID",&SilIDV);

  if(ReadWriteFlag){
    out_tree->Branch("Energy",&Qtot,"energy/D");
    out_tree->Branch("HAngle",&HAngle,"angle/D");
    out_tree->Branch("VAngle",&VAngle,"angle/D");
    out_tree->Branch("range",&track_range,"rangeProj/D");
    out_tree->Branch("rangeXY",&track_rangeXY,"rangeProjXY/D");
    out_tree->Branch("sourceX",&SourcePosX,"sourceX/D");
    out_tree->Branch("sourceY",&SourcePosY,"sourceY/D");
    out_tree->Branch("sourceZ",&SourcePosZ,"sourceZ/D");
    out_tree->Branch("range_calcX",&range_calcX,"range_calcX/D");
    out_tree->Branch("range_calcY",&range_calcY,"range_calcY/D");
    out_tree->Branch("range_calcZ",&range_calcZ,"range_calcZ/D");
  }

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

  TH1F *h_Vertex_X=new TH1F("h_Vertex_X","Reaction X vertex in mm",200,0.,128.);
  TH1F *h_Vertex_Y=new TH1F("h_Vertex_Y","Reaction Y vertex in mm",200,0.,64.);

  //TH2F* h_dEvsE=new TH2F("h_dEvsE","h_dEvsE",100,10.,80.,100,1.,5.);
  TH2F* h_dEvsE_MAYA=new TH2F("h_dEvsE_MAYA","h_dEvsE_MAYA",1000,0.,60.,1000,0.,10.);
  TH2F* h_dEvsE_DSSD=new TH2F("h_dEvsE_DSSD","h_dEvsE_DSSD",1000,0.,60.,1000,0.,10.);

  TH2F* h_dEvsE_DSSD1=new TH2F("h_dEvsE_DSSD1","h_dEvsE_DSSD1",1000,0.,60.,1000,0.,10.);
  TH2F* h_dEvsE_DSSD2=new TH2F("h_dEvsE_DSSD2","h_dEvsE_DSSD2",1000,0.,60.,1000,0.,10.);
  TH2F* h_dEvsE_DSSD3=new TH2F("h_dEvsE_DSSD3","h_dEvsE_DSSD3",1000,0.,60.,1000,0.,10.);
  TH2F* h_dEvsE_DSSD4=new TH2F("h_dEvsE_DSSD4","h_dEvsE_DSSD4",1000,0.,60.,1000,0.,10.);

  TH1F *h_SilCharge=new TH1F("h_SilCharge","Silicon charge (MeV)",200,0,100);
  TH1F *h_SilCharge0=new TH1F("h_SilCharge0","DSSD0 charge (MeV)",200,0,100);
  TH1F *h_SilCharge1=new TH1F("h_SilCharge1","DSSD1 charge (MeV)",200,0,100);
  TH1F *h_SilCharge2=new TH1F("h_SilCharge2","DSSD2 charge (MeV)",200,0,100);
  TH1F *h_SilCharge3=new TH1F("h_SilCharge3","DSSD3 charge (MeV)",200,0,100);

  TH1F *h_SilMult=new TH1F("h_SilMult","Silicon multiplicity",16,0,16);

  TH2F* h_dEvsE_ALL2=new TH2F("h_dEvsE_ALL2","dEvsE, dE(Pad)/TrackLength",1000,0.,60.,1000,0.,1.);
  TH2F* h_dEvsE_ALL=new TH2F("h_dEvsE_ALL",dEvsETitle,1000,0.,60.,1000,0.,10.);
  TH2F* h_dEvsE_p=new TH2F("h_dEvsE_p","h_dEvsE_p",1000,0.,60.,1000,0.,10.);
  TH2F* h_dEvsE_3He=new TH2F("h_dEvsE_3He","h_dEvsE_3He",1000,0.,60.,1000,0.,10.);
  TH2F* h_dEvsE_4He=new TH2F("h_dEvsE_4He","h_dEvsE_4He",1000,0.,60.,1000,0.,10.);
  TH2F* h_dEvsE_12C=new TH2F("h_dEvsE_12C","h_dEvsE_12C",1000,0.,60.,1000,0.,10.);
  TH2F* h_dEvsE_13C=new TH2F("h_dEvsE_13C","h_dEvsE_13C",1000,0.,60.,1000,0.,10.);

  TH2F* h_dEvsE_MAYA_rej=new TH2F("h_dEvsE_MAYA_rej","h_dEvsE_MAYA_rej",1000,0.,60.,1000,0.,10.);
  TH2F* h_dEvsE_DSSD_rej=new TH2F("h_dEvsE_DSSD_rej","h_dEvsE_DSSD_rej",1000,0.,60.,1000,0.,10.);
  TH2F* h_dEvsE_ALL_rej=new TH2F("h_dEvsE_ALL_rej","h_dEvsE_ALL_rej",1000,0.,60.,1000,0.,10.);
  TH2F* h_dEvsE_p_rej=new TH2F("h_dEvsE_p_rej","h_dEvsE_p_rej",1000,0.,60.,1000,0.,10.);
  TH2F* h_dEvsE_3He_rej=new TH2F("h_dEvsE_3He_rej","h_dEvsE_3He_rej",1000,0.,60.,1000,0.,10.);
  TH2F* h_dEvsE_4He_rej=new TH2F("h_dEvsE_4He_rej","h_dEvsE_4He_rej",1000,0.,60.,1000,0.,10.);
  TH2F* h_dEvsE_12C_rej=new TH2F("h_dEvsE_12C_rej","h_dEvsE_12C_rej",1000,0.,60.,1000,0.,10.);
  TH2F* h_dEvsE_13C_rej=new TH2F("h_dEvsE_13C_rej","h_dEvsE_13C_rej",1000,0.,60.,1000,0.,10.);

  TH1F *h_scat_range=new TH1F("TrackBragg","Track Bragg",200, 0.,200.);

  TH1F *h_scat_range1=new TH1F("TrackBragg1","Track Bragg1",200, 0.,200.);
  TH1F *h_scat_range2=new TH1F("TrackBragg2","Track Bragg2",200, 0.,200.);

  TH1F *h_energy=new TH1F("Energy","Total Charge Deposited",1000, 3000., 5000.);
  TH2F* visu_multiplicity=new TH2F("visu_multiplicity","visu_multiplicity",64,0,64,32,0,32);
  TH2F* visu_totcharge=new TH2F("visu_totcharge","visu_totcharge",64,0,64,32,0,32);

  TH1F *hSourceX=new TH1F("hSourceX","X track position at Y=-37.43mm",500,1,128);
  TH1F *hSourceY=new TH1F("hSourceY","Y track position at X=64mm",400,-100,0);

  TH2F *visu_charge=new TH2F("visu_charge","visu_charge",64,0,64,32,0,32);
  TH2F *visu_time=new TH2F("visu_time","visu_time",64,0,64,32,0,32);

  TH1F *hHorizAngle=new TH1F("hHorizAngle","Horizontal Angle",720,-180,180);
  TH1F *hVertiAngle=new TH1F("hVertiAngle","Vertical Angle",360,-90,90);
  TH1F *h3DAngle=new TH1F("h3DAngle","3D Angle",720,-180,180);

  TH1F *hdistVO=new TH1F("hdistVO","Dist VO",1000,0,500);
  TH1F *hdistVF=new TH1F("hdistVF","Dist VF",1000,0,500);
  TH1F *hdistOF=new TH1F("hdistOF","Dist OF",1000,0,200);
  TH2F *hdistOFvsVO=new TH2F("hdistOFvsVO","Dist OF vs Dist VO",1000,0,500,1000,0,500);
  TH2F *hdistOFvsVF=new TH2F("hdistOFvsVF","Dist OF vs Dist VF",1000,0,500,1000,0,500);
  TH2F *hdistOFvsTheta=new TH2F("hdistOFvsTheta","Dist OF vs Theta 3D",1000,0,360,1000,0,500);

  TH1F *hRange   = new TH1F("hRange","Alpha range in mm",200,50.,150.);
  TH1F *hRangeRest   = new TH1F("hRangeRest","Alpha range in mm, |vert angle| < 10 deg",200,50.,150.);
  TH2F *hRangeVsHoriz = new TH2F("hRangeVsHoriz","Range vs Horizontal angle",180,-90.,90.,200,50,150);

  TH1F *hEndTrackX   = new TH1F("hEndTrackX","End track X (mm)",200,0.,128.);
  TH1F *hEndTrackY   = new TH1F("hEndTrackY","End track Y (mm)",200,0.,64.);
  TH1F *hEndTrackZ   = new TH1F("hEndTrackZ","End track Z (mm)",600,300.,600.);

  TH2F *hEndTrackXY=new TH2F("hEndTrackXY","XY track final position",64,0,128,32,0,64);
  TH2F *hEndTrackXZ=new TH2F("hEndTrackXZ","XZ track final position",64,0,128,60,190,250);

  //Event Loop
  for (Long64_t jentry=0;jentry<nentries;jentry++) {
    if(jentry%500==0) cout << "Event " << jentry << endl;
    if(DEBUGFLAG>0) cout << "¡¡¡¡¡¡¡¡¡¡¡¡¡¡   NEW EVENT : " << jentry << "  !!!!!!!!!" << endl << endl;

    t2->GetEntry(jentry);
    // ActarSim DSSD map (beam view): 1=bottom right, 2=top right, 3=bottom left, 4=top left
    DSSD_1=DSSD_2=DSSD_3=DSSD_4=0;
    SilLeft=SilRight=SilFront=0;
    nbsiliconhits=0;

    Bool_t StopTrack=false;

    for(Int_t s=0;s<16;s++){//Loop on the 12 MAYA Si & 4 DSSD
      SilHitID=0;
      SilID[s]=0;
      if(SilCharge(s)!=0){
        nbsiliconhits++;
	if(s+1==1){DSSD_1=1;SilFront=1;SilFront=1;}
	else if(s+1==2){DSSD_2=1;SilFront=1;}
	else if(s+1==3){DSSD_3=1;SilFront=1;}
	else if(s+1==4){DSSD_4=1;SilFront=1;}
	else if(s+1<11)SilLeft=1;
	else if(s+1>10)SilRight=1;

	if(simuFlag){
          SilID[s]=SilIDV(s);

	  if(s+1<5) SilCharge(s)=gRandom->Gaus(SilCharge(s),0.0116*SilCharge(s));//150keV FWHM
	  else if(s+1>4) SilCharge(s)=gRandom->Gaus(SilCharge(s),0.0116*SilCharge(s));//150keV FWHM
	}
	else if(!simuFlag){
          if(s<4) SilCharge(s)=SilCharge(s)*5.256*1e-3+0.4312;//run177 dssd 140V
	  else if(s>3) SilCharge(s)=SilCharge(s)*MAYACAL[s-4][0]+MAYACAL[s-4][1];
	  h_SilMult->Fill(s);

          if(s==0) h_SilCharge0->Fill(SilCharge(s));
	  else if(s==1) h_SilCharge1->Fill(SilCharge(s));
	  else if(s==2) h_SilCharge2->Fill(SilCharge(s));
	  else if(s==3) h_SilCharge3->Fill(SilCharge(s));
        }
      }//end of if(SilCharge(s)!=0)
    }//end of Loop on the 12 MAYA Si & 4 DSSD
    visu_charge->Reset();
    visu_chargeYZ->Reset();
    visu_time->Reset();
    h3DTrack->Reset();

    h_scat_range->Reset();
    h_scat_range1->Reset();
    h_scat_range2->Reset();

    T[0]->ResetLines();
    T[1]->ResetLines();

    // Loop on columns & rows //////////////
    Double_t maxtime=0;
    Double_t mintime=0;

    for(Int_t c=0;c<numberOfColumns;c++){
      for(Int_t r=0;r<numberOfRows;r++){
        if(padChargeTest(r,c)>threshold){
          if(simuFlag){
            //Pad energy resolution estimation (to be measured)
	    if(padChargeTest(r,c)*30/1e6<20) PadESig=0.085;
	    else if(padChargeTest(r,c)*30/1e6>=20 && padChargeTest(r,c)*30/1e6<=5000)
              PadESig=-1.6215*1e-5*(padChargeTest(r,c)*30/1e6-20)+0.085;
	    else if(padChargeTest(r,c)*30/1e6>5000) PadESig=0.00425;

	    padCharge[r][c]=gRandom->Gaus(padChargeTest(r,c),PadESig*padChargeTest(r,c));
	    padCharge[r][c]*=30/1e9;
	    padTime[r][c]=padTimeTest(r,c);

	    //We left the middle pad band shadowed
	    if(r>13 && r<18)padCharge[r][c]=0;
          }
	  else if(!simuFlag){
            //Pad chargeto energy (MeV) convertion
	    padCharge[r][c]=padChargeTest(r,c)*3.299*1e-5+0.0537;
	    padTime[r][c]=padTimeTest(r,c);
            //Next are noisy channels that I noticed in the end of 1st part. Check if is still the case in the run to analyse
	    if(r==13 && (c==0 || c==1 || c==2)) {
              padCharge[r][c]=0;
              padTime[r][c]=0;
            }
          }
          padHeight[r][c]=padTime[r][c]*driftVelocity;

	  if(maxtime==0){
            maxtime=mintime=padTime[r][c];
          }
	  else if(padTime[r][c]>maxtime) maxtime=padTime[r][c];
	  else if(padTime[r][c]<mintime && padTime[r][c]!=0) mintime=padTime[r][c];

	  if(simuFlag || (!simuFlag && !(r==13 && (c==0 || c==2))))
            visu_charge->SetBinContent(c+1.,r+1,padCharge[r][c]);
	  visu_time->SetBinContent(c+1.,r+1.,padTime[r][c]);
	  h3DTrack->SetBinContent(c+1.,r+1.,padTime[r][c]*driftVelocity/2+1,padCharge[r][c]);
        } //end of if(padChargeTest(r,c)>threshold){
	else{
          padCharge[r][c]=-1;
	  padTime[r][c]=-1;
	  padHeight[r][c]=-1;
        }
      }//End of Loop on Rows
    }//End of Loop on Columns

    h_DeltaTime->Fill(maxtime-mintime);
    h_DeltaHeight->Fill((maxtime-mintime)*driftVelocity);

    //Begining of reaction analysis

    // Calculating the Tracks exit pads on the left & right part of the beam
    minColumnleft=minColumnright=maxRowleft=maxRowright=0;
    minColumnbeamleft=minColumnbeamright=maxRowbeamleft=maxRowbeamright=0;
    tmpminColumnbeamleft=tmpminColumnbeamright=tmpmaxRowbeamleft=tmpmaxRowbeamright=0;
    minColumnoutleft=minColumnoutright=maxRowoutleft=maxRowoutright=0;
    NCBeamoutleft=NCBeamoutright=NCoutleft=NCoutright=0;
    trackleft=trackright=trackfront1=trackfront2=outbeamtrackleft=outbeamtrackright=GOOD_EVENT=0;

    for(Int_t m=0;m<numberOfColumns;m++){
      if(padCharge[numberOfRows/2+beamWidth][m]>threshold) { //LEFT SIDE
        if(m==tmpmaxRowbeamleft+1){
          NCBeamoutleft++;
          maxRowbeamleft=m;
          minColumnbeamleft=tmpminColumnbeamleft;
        }
        else {
          tmpminColumnbeamleft=m;
          NCBeamoutleft=1;
        }
        tmpmaxRowbeamleft=m;
      }
      if(padCharge[numberOfRows-1][m]>threshold) {
        if(m==maxRowoutleft+1) NCoutleft++;
	else {
          minColumnoutleft=m;
          NCoutleft=1;
	}
	maxRowoutleft=m;
      }

      if(padCharge[numberOfRows/2-beamWidth-1][m]>threshold) { //RIGHT SIDE
        if(m==tmpmaxRowbeamright+1){
          NCBeamoutright++;
          maxRowbeamright=m;
          minColumnbeamright=tmpminColumnbeamright;
        }
        else {
          tmpminColumnbeamright=m;
          NCBeamoutright=1;
        }
        tmpmaxRowbeamright=m;
      }
      if(padCharge[0][m]>threshold) {
        if(m==maxRowoutright+1) NCoutright++;
        else {
          minColumnoutright=m;
          NCoutright=1;
	}
        maxRowoutright=m;
      }
    } //end of loop on columns

    if(NCBeamoutleft>minpadout) outbeamtrackleft=1;
    if(NCBeamoutright>minpadout) outbeamtrackright=1;

    if(NCoutleft>minpadout){
      trackleft=1;
      NTrackOut++;
      minColumnleft=TMath::Min(minColumnbeamleft,minColumnoutleft);
      maxRowleft=TMath::Max(maxRowbeamleft,maxRowoutleft);
    }
    if(NCoutright>minpadout){
      trackright=1;
      NTrackOut++;
      minColumnright=TMath::Min(minColumnbeamright,minColumnoutright);
      maxRowright=TMath::Max(maxRowbeamright,maxRowoutright);
    }

    // Calculating the Scatter Exit Pads (and if there's one, the recoil's exit pad) from the last Column's Rows
    minRow1=maxRow1=NRout1=0;
    minRow2=maxRow2=NRout2=0;
    for(Int_t l=0;l<numberOfRows;l++){
      if(padCharge[l][numberOfColumns-1]>threshold) {
        if(l==maxRow1+1) {
          NRout1++;
          maxRow1=l;
	}
	else if(NRout1<minpadout) {
          maxRow1=l;
          minRow1=l;
          NRout1=1;
	}
	else if(NRout1>minpadout-1&&l==maxRow2+1) {
          NRout2++;
	  maxRow2=l;
	}
	else if(NRout1>minpadout-1&&NRout2<minpadout) {
          maxRow2=l;
	  minRow2=l;
	  NRout2=1;
        }
      }
    } //end of loop on number of rows

    if(NRout1>minpadout-1 && (NRout1<maxpadout || minRow1==0 || maxRow1==numberOfColumns-1)) {
      trackfront1=1;
      NTrackOut++;
    }//If track goes in the corner number of pads can be higher than 5
    if(NRout2>minpadout-1 && (NRout2<maxpadout || maxRow2==numberOfColumns-1)){
      trackfront2=1;
      NTrackOut++;
    }

    if(DEBUGFLAG>2)
      cout << "maxRowoutright " << maxRowoutright << " maxRowoutleft " << maxRowoutleft
           << " minRow1 " << minRow1 << " maxRow1 " << maxRow1 << " maxRow2 " << maxRow2 << endl;
    if(DEBUGFLAG>2)
      cout << "NCoutright " << NCoutright << " NCoutleft " << NCoutleft
           << " NRout1 " << NRout1 << " NRout2 " << NRout2 << endl;

    // Corner Case
    if(maxRowoutright==numberOfColumns-1 && minRow1==0){
      if(NCoutright>NRout1){
        trackfront1=0;
        NTrackOut--;
        if(trackfront2){
          trackfront1=1;
          trackfront2=0;
        }
      }
      else if(NCoutright<=NRout1){
        trackright=0;
        NTrackOut--;
      }
    }

    if(maxRowoutleft==numberOfColumns-1 && maxRow1==numberOfRows-1){
      if(NCoutleft>NRout1){
        trackfront1=0;
        NTrackOut--;
        if(trackfront2){
          trackfront1=1;
          trackfront2=0;
        }
      }
      else if(NCoutleft<=NRout1){
        trackleft=0;
        NTrackOut--;
      }
    }

    if(maxRowoutleft==numberOfColumns-1 && maxRow2==numberOfRows-1){
      if(NCoutleft>NRout2){
        trackfront2=0;
        NTrackOut--;
      }
      else if(NCoutleft<=NRout2){
        trackleft=0;
        NTrackOut--;
      }
    }

    if(DEBUGFLAG>1)
      cout << "NTrackOut: " << NTrackOut << " trackleft " << trackleft << " trackright "
           << trackright << " trackfront1 " << trackfront1 << " trackfront2 " << trackfront2 << endl;
    if(DEBUGFLAG>1)
      cout << "DSSD_1: " << DSSD_1 << " DSSD_2: " << DSSD_2
	   << " DSSD_3: " << DSSD_3 << " DSSD_4: " << DSSD_4 << endl;
    if(DEBUGFLAG>1)
      cout << "MAYA LEFT " << SilLeft << " MAYA RIGHT " << SilRight << endl;

    // 1 Silicon hit configurations
    // Config 1 : left silicon hitted & track exit by the left side
    // Config 2 : right silicon hitted & track exit by the right side
    // Config 3 : dssd hitted & track exit by the back side
    // 2 Silcon hits configurations
    // Config 4 : left and right silicon hits & 2 tracks exit by left and right sides
    // Config 5 : left and right silicon hits & 2 tracks exit by left and front sides
    // Config 6 : left and right silicon hits & 2 tracks exit by right and front sides
    // Config 7 : left and right dssd hits & 2 tracks exit by front sides

    if(nbsiliconhits==1){
      if((SilLeft || DSSD_3 || DSSD_4) && trackleft) config=1;
      else if((SilRight || DSSD_1 || DSSD_2) && trackright) config=2;
      else if((DSSD_1 || DSSD_2) && trackfront1 && !trackright) config=3;
      else if((DSSD_3 || DSSD_4) && trackfront1 && !trackleft) config=3;
    }
    else if(nbsiliconhits==2) {
      if(SilLeft && SilRight && trackleft && trackright) config=4;
      if(SilLeft && (DSSD_1 || DSSD_2) && trackleft && trackright) config=4;
      if(SilRight && (DSSD_3 || DSSD_4) && trackright && trackleft) config=4;
      if((DSSD_1 || DSSD_2) && (DSSD_3 || DSSD_4) && trackright && trackleft) config=4;
      if(SilLeft && (DSSD_1 || DSSD_2) && trackleft && trackfront1) config=5;
      if((DSSD_1 || DSSD_2) && (DSSD_3 || DSSD_4) && trackleft && trackfront1 && !trackfront2) config=5;
      if(SilRight && (DSSD_3 || DSSD_4) && trackright && trackfront1) config=6;
      if((DSSD_1 || DSSD_2) && (DSSD_3 || DSSD_4) && trackright && trackfront1 && !trackfront2) config=6;
      if((DSSD_1 || DSSD_2) && (DSSD_3 || DSSD_4) && trackfront1 && trackfront2) config=7;
    }

    if(config!=0) GOOD_EVENT=1;

    if(simuFlag){ //In the case when the scatter & recoil overlap
      if(NTrackOut==1 && (DSSD_1||DSSD_2) && !outbeamtrackleft) GOOD_EVENT=0;
      if(NTrackOut==1 && (DSSD_3||DSSD_4) && !outbeamtrackright) GOOD_EVENT=0;
    }

    // DeteminRowation of the fit boundaries for the tracks depending on the configuration
    if(GOOD_EVENT) {
      if(DEBUGFLAG>0) cout << "Config " << config << endl;
      a=b=0.;
      // If there is no track on left or right of beam, consider it as beam
      if(maxRowright==0) maxRowright=numberOfColumns-1;
      if(maxRowleft==0) maxRowleft=numberOfColumns-1;
      if(maxRowbeamright==0) maxRowbeamright=numberOfColumns-1;
      if(maxRowbeamleft==0) maxRowbeamleft=numberOfColumns-1;

      track1=track2=0;

      for(Int_t t=0;t<2;t++){
        minRow[t]=maxRow[t]=minColumn[t]=maxRow[t]=0;
        trackout[t]=SilWall[t]=-1;
        for(Int_t j=0;j<3;j++){
	  PointA[j][t]=PointA[j][t]=0.;
	}
      }
      for(Int_t i=0;i<3;i++){
	Start[i]=Out[i]=End[i]=0.;
      }

      if(config==1 || config==4 || config==5) {
	if(DEBUGFLAG>1) cout << "Config " << config << endl;
	minRowtmp=numberOfRows/2+2;
	maxRowtmp=numberOfRows-1;
	minColumntmp=minColumnleft;
	if(SilLeft){
	  if(DEBUGFLAG>2) cout << "MAYA" << endl;
	  maxRowtmp=maxRowleft;
	}
	else if(DSSD_3 || DSSD_4){
	  if(DEBUGFLAG>2) cout << "DSSD" << endl;
	  maxRowtmp=numberOfColumns-1;
	}

	if(minColumntmp==maxRowtmp) minColumntmp=maxRowtmp-3;

	if(!track1){
          minRow[0]=minRowtmp;
          maxRow[0]=maxRowtmp;
          minColumn[0]=minColumntmp;
          maxRow[0]=maxRowtmp;
	  if(SilLeft) SilWall[0]=2;
	  else if(DSSD_3 || DSSD_4) SilWall[0]=1;
        }
	else{
          minRow[1]=minRowtmp;
          maxRow[1]=maxRowtmp;
          minColumn[1]=minColumntmp;
          maxRow[1]=maxRowtmp;
          if(SilLeft)SilWall[1]=2;
          else if(DSSD_3 || DSSD_4)SilWall[1]=1;
        }

	track1=1; trackout[0] = 0;
	DSSD_3 = DSSD_4 = 0;
      }//end of config1

      if(config==2 || config==4 || config==6) {
	if(DEBUGFLAG>1) cout << "Config " << config << endl;
	minRowtmp=0;
	minColumntmp=minColumnright;

	if(SilRight){
          if(DEBUGFLAG>2) cout << "MAYA" << endl;
	  maxRowtmp=numberOfRows/2-beamWidth;
	  maxRowtmp=maxRowright;
	}
	else if(DSSD_1 || DSSD_2){
	  if(DEBUGFLAG>2)cout<<"DSSD"<<endl;
	  // Fit parameters depends if tracks are closed to beam or not
	  maxRowtmp=numberOfRows/2-3;
	  maxRowtmp=numberOfColumns-1;
	}
	if(minColumntmp==maxRowtmp) minColumntmp=maxRowtmp-3;
	if(!track1){
	  minRow[0]=minRowtmp;
	  maxRow[0]=maxRowtmp;
	  minColumn[0]=minColumntmp;
	  maxRow[0]=maxRowtmp;
	  track1=1;
	  trackout[0]=1;
	  if(SilRight) SilWall[0]=3;
	  else if(DSSD_1 || DSSD_2) SilWall[0]=0;
	}
	else {
	  minRow[1]=minRowtmp;
	  maxRow[1]=maxRowtmp;
	  minColumn[1]=minColumntmp;
	  maxRow[1]=maxRowtmp;
	  track2=1;
	  trackout[1]=1;
	  if(SilRight) SilWall[1]=3;
	  else if(DSSD_1 || DSSD_2) SilWall[1]=0;
	}
	DSSD_1 = DSSD_2 = 0;
      }//end of config2

      if(config==3 || config==5 || config==6) {
	if(DEBUGFLAG>1) cout << "Config " << config << endl;
	minColumntmp=TMath::Min(maxRowbeamleft,maxRowbeamright);
	maxRowtmp=numberOfColumns-1;
	if(DSSD_1||DSSD_2) {
	  minRowtmp=0;
	  if(maxRowbeamright<numberOfColumns-1){
	    minColumntmp=minColumnbeamright;
	    maxRowtmp=numberOfRows/2-3;
	  }
	  else if(maxRowbeamright==numberOfColumns-1 && maxRowbeamleft<numberOfColumns-1){
	    minColumntmp=(maxRowbeamleft+minColumnbeamleft)/2;
	    maxRowtmp=numberOfRows/2;
	  }
	  else if(maxRowbeamright==numberOfColumns-1 && maxRowbeamleft==numberOfColumns-1 &&
		  minColumnbeamright!=0 && minColumnbeamleft!=0){
	    minColumntmp=minColumnbeamright;
	    maxRowtmp=numberOfRows/2-2;
	  }
	  else if(maxRowbeamright==numberOfColumns-1 && maxRowbeamleft==numberOfColumns-1 &&
		  minColumnbeamright!=0 && minColumnbeamleft==0){
	    minColumntmp=(maxRowbeamright+minColumnbeamright)/2;
	    maxRowtmp=numberOfRows/2-2;
	  }
	  else if(maxRowbeamright==numberOfColumns-1 && maxRowbeamleft==numberOfColumns-1 &&
		  minColumnbeamright==0 && minColumnbeamleft!=0){
	    minColumntmp=(maxRowbeamleft+minColumnbeamleft)/2;
	    maxRowtmp=numberOfRows/2-2;
	  }
	}
	else if(DSSD_3||DSSD_4){
	  maxRowtmp=numberOfRows-1;
	  if(maxRowbeamleft<numberOfColumns-1){
	    minColumntmp=minColumnbeamleft;
	    minRowtmp=numberOfRows/2+2;
	  }
	  else if(maxRowbeamleft==numberOfColumns-1 && maxRowbeamright<numberOfColumns-1){
	    minColumntmp=(maxRowbeamright+minColumnbeamright)/2;
	    minRowtmp=numberOfRows/2-2;
	  }
	  else if(maxRowbeamright==numberOfColumns-1 && maxRowbeamleft==numberOfColumns-1 &&
		  minColumnbeamright!=0 && minColumnbeamleft!=0){
	    minColumntmp=minColumnbeamleft;
	    minRowtmp=numberOfRows/2+1;
	  }
	  else if(maxRowbeamright==numberOfColumns-1 && maxRowbeamleft==numberOfColumns-1 &&
		  minColumnbeamright!=0 && minColumnbeamleft==0){
	    minColumntmp=(maxRowbeamright+minColumnbeamright)/2;
	    minRowtmp=numberOfRows/2+1;
	  }
	  else if(maxRowbeamright==numberOfColumns-1 && maxRowbeamleft==numberOfColumns-1 &&
		  minColumnbeamright==0 && minColumnbeamleft!=0){
	    minColumntmp=(maxRowbeamleft+minColumnbeamleft)/2;
	    minRowtmp=numberOfRows/2+1;
	  }
	}

	if(DSSD_1||DSSD_2) minColumntmp=(2*maxRowbeamright+minColumnbeamright)/3;//IPNO experiment has a middle blind band so fits going wrong and this is needed
	else if(DSSD_3||DSSD_4) minColumntmp=(2*maxRowbeamleft+minColumnbeamleft)/3;

	if(minColumntmp==maxRowtmp) minColumntmp=maxRowtmp-3;

	if(!track1){
	  minRow[0]=minRowtmp;
	  maxRow[0]=maxRowtmp;
	  minColumn[0]=minColumntmp;
	  maxRow[0]=maxRowtmp;
	  track1=1;
	  if(DSSD_1 || DSSD_2){
	    SilWall[0]=0;
	    trackout[0]=3;
	  }
	  else if(DSSD_3 || DSSD_4){
	    SilWall[0]=1;
	    trackout[0]=2;
	  }
	}
	else {
	  minRow[1]=minRowtmp;
	  maxRow[1]=maxRowtmp;
	  minColumn[1]=minColumntmp;
	  maxRow[1]=maxRowtmp;
	  track2=1;
	  if(DSSD_1 || DSSD_2){
	    SilWall[1]=0;
	    trackout[1]=3;
	  }
	  else if(DSSD_3 || DSSD_4){
	    SilWall[1]=1;
	    trackout[1]=2;
	  }
	}
      }//end of config3

      if(config==7) {
	if(DEBUGFLAG>1) cout << "Config " << config << endl;
	if(maxRowbeamright<numberOfColumns-1 && maxRowbeamleft<numberOfColumns-1){
	  minColumn[0]=minColumnbeamright;
	  minColumn[1]=minColumnbeamleft;
	  maxRow[0]=numberOfRows/2-3;
	  minRow[1]=numberOfRows/2+2;
	}
	else if(maxRowbeamright==numberOfColumns-1 && maxRowbeamleft<numberOfColumns-1){
	  minColumn[0]=(maxRowbeamleft+minColumnbeamleft)/2;
	  minColumn[1]=minColumnbeamleft;
	  maxRow[0]=numberOfRows/2;
	  minRow[1]=numberOfRows/2+2;
	}
	else if(maxRowbeamright<numberOfColumns-1 && maxRowbeamleft==numberOfColumns-1){
	  minColumn[0]=minColumnbeamright;
	  minColumn[1]=(maxRowbeamright+minColumnbeamright)/2;
	  maxRow[0]=numberOfRows/2-3;
	  minRow[1]=numberOfRows/2-2;
	}
	else if(maxRowbeamright==numberOfColumns-1 && maxRowbeamleft==numberOfColumns-1 &&
		minColumnbeamright!=0 && minColumnbeamleft!=0){
	  minColumn[0]=minColumnbeamright;
	  minColumn[1]=minColumnbeamleft;
	  maxRow[0]=numberOfRows/2-2;
	  minRow[1]=numberOfRows/2+1;
	}
	else if(maxRowbeamright==numberOfColumns-1 && maxRowbeamleft==numberOfColumns-1 &&
		minColumnbeamright!=0 && minColumnbeamleft==0){
	  minColumn[0]=minColumnbeamright;
	  minColumn[1]=(maxRowbeamright+minColumnbeamright)/2;
	  maxRow[0]=numberOfRows/2-2;
	  minRow[1]=numberOfRows/2+1;
	}
	else if(maxRowbeamright==numberOfColumns-1 && maxRowbeamleft==numberOfColumns-1 &&
		minColumnbeamright==0 && minColumnbeamleft!=0){
	  minColumn[0]=(maxRowbeamleft+minColumnbeamleft)/2;
	  minColumn[1]=minColumnbeamleft;
	  maxRow[0]=numberOfRows/2-2;
	  minRow[1]=numberOfRows/2+1;
	}
	maxRow[0]=numberOfColumns-1;
	maxRow[1]=numberOfColumns-1;
	minRow[0]=0;
	maxRow[1]=numberOfRows-1;
	if(minColumn[0]==maxRow[0]) minColumn[0]=maxRow[0]-3;
	else if(minColumn[1]==maxRow[1]) minColumn[1]=maxRow[1]-3;

	if(!track1){  // Right track
	  track1=1;
	  trackout[0]=3;
	  SilWall[0]=0;
	}
	else {
	  track2=1;
	  trackout[1]=3;
	  SilWall[1]=0;
	}
	if(!track1){  // Left track
	  track1=1;
	  trackout[0]=2;
	  SilWall[0]=1;
	}
	else {
	  track2=1;
	  trackout[1]=2;
	  SilWall[1]=1;
	}
      } //End config 7

      // Fitting and calculating the starting and exit points as well as
      // the energy deposit from a delta L part of the track
      for(Int_t t=0;t<track1+track2;t++){
	if(DEBUGFLAG>0)
	  cout << "minRow " << minRow[t] << " maxRow " << maxRow[t]
	       << " minColumn " << minColumn[t] << " maxRow " << maxRow[t] << endl;
	FitMat(padCharge,minRow[t],maxRow[t],minColumn[t],maxRow[t],threshold,a,b);
	IniPoint[0][t]=0;
	IniPoint[1][t]=a*IniPoint[0][t]+b/2;
	FinPoint[0][t]=64;
	FinPoint[1][t]=FinPoint[0][t]*a+b/2;

	Double_t chi2=FitMat3D(padCharge,padHeight,minRow[t],maxRow[t],minColumn[t],maxRow[t],threshold,T[t]);

	//tstart: parameter to calculate the point Start where the track pass the middle blind band
	//t0: parameter to calculate the point Out where the track exit the pad plane
	//tf: parameter to calculate the point End where the track hit the silicon

	xv=2*minColumn[t]+1.;

	if(T[t]->Ym-T[t]->Yh!=0){
	  if(trackout[t]==0){
	    Double_t t0=(64-T[t]->Yh)/(T[t]->Ym-T[t]->Yh);
	    yv=2*minRow[t]+1.;
	    zv=padHeight[minRow[t]][minColumn[t]];
	  }//track left
	  else if(trackout[t]==1){
	    Double_t t0=(-T[t]->Yh)/(T[t]->Ym-T[t]->Yh);
	    yv=2*maxRow[t]+1.;
	    zv=padHeight[maxRow[t]][minColumn[t]];
	  }//track right
	  else if(trackout[t]==2){
	    Double_t t0=(128-T[t]->Xh)/(T[t]->Xm-T[t]->Xh);
	    yv=2*minRow[t]+1.;
	    zv=padHeight[minRow[t]][minColumn[t]];
	  }//track front left
	  else if(trackout[t]==3){
	    Double_t t0=(128-T[t]->Xh)/(T[t]->Xm-T[t]->Xh);
	    yv=2*maxRow[t]+1.;
	    zv=padHeight[maxRow[t]][minColumn[t]];
	  }//track front right
	  //projection of V(2*minColumn+1,2*minRow+1,padHeight) on T[t]
	  Double_t tstart=-((T[t]->Xh-xv)*(T[t]->Xm-T[t]->Xh)+(T[t]->Yh-yv)*(T[t]->Ym-T[t]->Yh)+(T[t]->Zh-zv)*(T[t]->Zm-T[t]->Zh)) /
	    ((T[t]->Xm-T[t]->Xh)*(T[t]->Xm-T[t]->Xh)+(T[t]->Ym-T[t]->Yh)*(T[t]->Ym-T[t]->Yh)+(T[t]->Zm-T[t]->Zh)*(T[t]->Zm-T[t]->Zh));

	  if(SilWall[t]<2){
	    Double_t tf=(128+PadToSilX-T[t]->Xh)/(T[t]->Xm-T[t]->Xh);
	    End[0]=64.*2+PadToSilX;
	    End[1]=tf*(T[t]->Ym-T[t]->Yh)+T[t]->Yh;
	    End[2]=tf*(T[t]->Zm-T[t]->Zh)+T[t]->Zh;
	  }
	  else if(SilWall[t]==2){
	    Double_t tf=(64.+PadToSilY-T[t]->Yh)/(T[t]->Ym-T[t]->Yh);
	    End[0]=tf*(T[t]->Xm-T[t]->Xh)+T[t]->Xh;
	    End[1]=64.+PadToSilY;
	    End[2]=tf*(T[t]->Zm-T[t]->Zh)+T[t]->Zh;
	  }
	  else if(SilWall[t]==3){
	    Double_t tf=(-PadToSilY-T[t]->Yh)/(T[t]->Ym-T[t]->Yh);
	    End[0]=tf*(T[t]->Xm-T[t]->Xh)+T[t]->Xh;
	    End[1]=-PadToSilY;
	    End[2]=tf*(T[t]->Zm-T[t]->Zh)+T[t]->Zh;
	  }
	}

	if(trackout[t]==0){
	  Out[0]=t0*(T[t]->Xm-T[t]->Xh)+T[t]->Xh;
	  Out[1]=32*2;
	}
	else if(trackout[t]==1){
	  Out[0]=t0*(T[t]->Xm-T[t]->Xh)+T[t]->Xh;
	  Out[1]=0.;
	}
	else if(trackout[t]>1){
	  Out[0]=64*2;
	  Out[1]=t0*(T[t]->Ym-T[t]->Yh)+T[t]->Yh;
	}
	Out[2]=t0*(T[t]->Zm-T[t]->Zh)+T[t]->Zh;

	Start[0]=tstart*(T[t]->Xm-T[t]->Xh)+T[t]->Xh;
	Start[1]=tstart*(T[t]->Ym-T[t]->Yh)+T[t]->Yh;
	Start[2]=tstart*(T[t]->Zm-T[t]->Zh)+T[t]->Zh;

	IniFitPoint[0][t]=Start[0]/2.; IniFitPoint[1][t]=a*IniFitPoint[0][t]+b/2;

	dist_FO=TMath::Sqrt((Out[0]-End[0])*(Out[0]-End[0])+(Out[1]-End[1])*(Out[1]-End[1])+(Out[2]-End[2])*(Out[2]-End[2]));

	Double_t tb=LOutMax/dist_FO;

	PointB[0][t]=tb*(Out[0]-End[0])+End[0];
	PointB[1][t]=tb*(Out[1]-End[1])+End[1];
	PointB[2][t]=tb*(Out[2]-End[2])+End[2];

	Double_t ta=(DeltaL+LOutMax)/dist_FO;

	PointA[0][t]=ta*(Out[0]-End[0])+End[0];
	PointA[1][t]=ta*(Out[1]-End[1])+End[1];
	PointA[2][t]=ta*(Out[2]-End[2])+End[2];

	dist_VO=TMath::Sqrt((Out[0]-Start[0])*(Out[0]-Start[0])+(Out[1]-Start[1])*(Out[1]-Start[1])+(Out[2]-Start[2])*(Out[2]-Start[2]));
	dist_VF=TMath::Sqrt((End[0]-Start[0])*(End[0]-Start[0])+(End[1]-Start[1])*(End[1]-Start[1])+(End[2]-Start[2])*(End[2]-Start[2]));

	VAngle=TMath::ATan((Out[2]-Start[2])/TMath::Sqrt((Out[0]-Start[0])*(Out[0]-Start[0])+(Out[1]-Start[1])*(Out[1]-Start[1])))*deg;
	if(Out[0]-Start[0]>0){
	  HAngle=TMath::ATan((Out[1]-Start[1])/(Out[0]-Start[0]))*deg;
	  Angle3D=TMath::ATan(TMath::Sqrt((Out[1]-Start[1])*(Out[1]-Start[1])+(Out[2]-Start[2])*(Out[2]-Start[2]))/(Out[0]-Start[0]))*deg;
	}
	else{
	  HAngle=180.+TMath::ATan((Out[1]-Start[1])/(Out[0]-Start[0]))*deg;
	  Angle3D=180.+TMath::ATan(TMath::Sqrt((Out[1]-Start[1])*(Out[1]-Start[1])+(Out[2]-Start[2])*(Out[2]-Start[2]))/(Out[0]-Start[0]))*deg;
	}
	hHorizAngle->Fill(HAngle);
	hVertiAngle->Fill(VAngle);
	h3DAngle->Fill(Angle3D);

	hdistVO->Fill(dist_VO);
	hdistVF->Fill(dist_VF);
	hdistOF->Fill(dist_FO);
	hdistOFvsVO->Fill(dist_VO,dist_FO);
	hdistOFvsVF->Fill(dist_VF,dist_FO);
	hdistOFvsTheta->Fill(Angle3D,dist_FO);

	if(DEBUGFLAG>1) cout << "dist_VO: " << dist_VO << ", dist_VF: " << dist_VF << endl;

	TVector3 v2(Out[0]-Start[0],Out[1]-Start[1],Out[2]-Start[2]);
	Double_t mag=v2.Mag();

	h_scat_range->Reset();

	for(Int_t r=minRow[t];r<maxRow[t]+1;r++){//Rows
	  for(Int_t c=minColumn[t];c<maxRow[t]+1;c++){ //columns
	    Double_t xPad=0.,yPad=0.,zPad=0.,xProj=0.,yProj=0.;
	    if(padCharge[r][c] >threshold){
	      xPad = (2*c+1.);
	      yPad = (2*r+1.);
	      zPad = padHeight[r][c];

	      TVector3 v1(xPad-Start[0],yPad-Start[1],zPad-Start[2]);
	      range=v1*v2/mag;

	      //Pad charge projection method on the fitted track by weighted slices (Piotr)
	      const Int_t ntimes=10;

	      Double_t dR[ntimes];
	      Double_t dQ[ntimes];

	      Double_t dl=2/TMath::Cos(HAngle*rad)/(Double_t)ntimes;
	      Double_t dq=padCharge[r][c]/(Double_t)ntimes;

	      Double_t sumSi=0;
	      Double_t SurfTot,Surfi,SurfTriCor,SurfSPar,SurfBPar,dxi;
	      SurfTot=0;//Total Surface of each step band
	      Surfi=0;//Surface of each step band inside the pad
	      SurfTriCor=0;//outside triangle Surface of the corner step
	      SurfSPar=0;//Surface of each small parallelepiped outside of the pad
	      SurfBPar=0;//Surface of each big parallelepiped outside of the pad

	      dxi=dl/TMath::Cos(HAngle*rad);

	      for(Int_t i=1;i<=ntimes/2;i++){//only needs half pad projection
		Surfi=0;
		Double_t xi=TMath::Tan(HAngle*rad)+i*dl/TMath::Cos(HAngle*rad);

		SurfTot=2*dl/TMath::Cos(HAngle*rad);

		if(xi<=1) Surfi=SurfTot;
		else if(xi>1 && xi<1+dxi){
		  SurfTriCor=(xi-1)*(xi-1)/(2*TMath::Tan(HAngle*rad));
		  Surfi=SurfTot-SurfTriCor;
		  SurfSPar=dl*(xi-1)/TMath::Sin(HAngle*rad);
		}
		else if(xi>=1+dxi){
		  SurfBPar=2*dl*dl/TMath::Sin(2*HAngle*rad);
		  Surfi=SurfTot-SurfSPar-((Int_t)((xi-1)/dxi)-1)*SurfBPar-SurfBPar/2;
		}

		sumSi+=Surfi;

		dR[ntimes/2-1+i]=range + dl/2 + ((Double_t)i-1)*dl;
		dR[ntimes/2-i]=range - dl/2 - ((Double_t)i-1)*dl;
		dQ[ntimes/2-1+i]=dQ[ntimes/2-i]=(Surfi/4)*padCharge[r][c];

		h_scat_range->Fill(dR[ntimes/2-1+i],dQ[ntimes/2-1+i]);
		if(t==0) h_scat_range1->Fill(dR[ntimes/2-1+i],dQ[ntimes/2-1+i]);
		else if(t==1) h_scat_range2->Fill(dR[ntimes/2-1+i],dQ[ntimes/2-1+i]);
		//second half
		h_scat_range->Fill(dR[ntimes/2-i],dQ[ntimes/2-i]);
		if(t==0) h_scat_range1->Fill(dR[ntimes/2-i],dQ[ntimes/2-i]);
		else if(t==1) h_scat_range2->Fill(dR[ntimes/2-i],dQ[ntimes/2-i]);
	      }
	    }
	  }
	}//End of Loop on pads

	RangeA[t]=TMath::Sqrt((PointA[0][t]-Start[0])*(PointA[0][t]-Start[0])+(PointA[1][t]-Start[1])*(PointA[1][t]-Start[1]));
	RangeB[t]=TMath::Sqrt((PointB[0][t]-Start[0])*(PointB[0][t]-Start[0])+(PointB[1][t]-Start[1])*(PointB[1][t]-Start[1]));
	RangeO[t]=TMath::Sqrt((Out[0]-Start[0])*(Out[0]-Start[0])+(Out[1]-Start[1])*(Out[1]-Start[1]));

	bragR=new TSpline3(h_scat_range);
	if(t==0) bragR1=new TSpline3(h_scat_range1);
	else if(t==1) bragR2=new TSpline3(h_scat_range2);

	Double_t dE_scat=0.;
	Double_t dE_scat2=0.;
	Double_t step=2*TMath::Abs(RangeB[t]-RangeA[t])/100.;
	Double_t value;

	if(RangeA[t]<1000){
	  for(Double_t val=RangeA[t];val<=RangeB[t];val=val+step){
	    dE_scat+=step*(value+bragR->Eval(val))/2;
	    value=bragR->Eval(val);
	  }
	}

	dE_scat2=dE_scat/dist_VO;
	particleID=0;
	Energy_in_silicon=0.;
	if(SilWall[t]==0){
	  smin=0;
	  smax=1;
	}//DSSD1_2
	else if(SilWall[t]==1){
	  smin=2;
	  smax=3;
	}//DSSD3_4
	else if(SilWall[t]==2){
	  smin=4;
	  smax=9;
	}//MAYA LEFT
	else if(SilWall[t]==3){
	  smin=10;
	  smax=15;
	}//MAYA RIGHT

	for(s=smin;s<=smax;s++){
	  Energy_in_silicon+=SilCharge(s);
	  if(simuFlag) particleID+=SilID[s];
	}

	if(DEBUGFLAG>0) cout << "Energy_in_silicon " << Energy_in_silicon << " dE_Pad " << dE_scat << endl;

	if(dist_FO<LOutMax && dist_VF>(LOutMax+DeltaL)){
	  if(DEBUGFLAG>0) cout << "ACCEPTED" << endl;
	  if(SilWall[t]>1) h_dEvsE_MAYA->Fill(Energy_in_silicon,dE_scat);
	  if(SilWall[t]<2) h_dEvsE_DSSD->Fill(Energy_in_silicon,dE_scat);
	  h_dEvsE_ALL->Fill(Energy_in_silicon,dE_scat);
	  h_dEvsE_ALL2->Fill(Energy_in_silicon,dE_scat2);

	  if(SilCharge(0)>0) h_dEvsE_DSSD1->Fill(Energy_in_silicon,dE_scat);
	  if(SilCharge(1)>0) h_dEvsE_DSSD2->Fill(Energy_in_silicon,dE_scat);
	  if(SilCharge(2)>0) h_dEvsE_DSSD3->Fill(Energy_in_silicon,dE_scat);
	  if(SilCharge(3)>0) h_dEvsE_DSSD4->Fill(Energy_in_silicon,dE_scat);

	  if(simuFlag){
	    if(particleID==1) h_dEvsE_p->Fill(Energy_in_silicon,dE_scat);
	    else if(particleID==2) h_dEvsE_3He->Fill(Energy_in_silicon,dE_scat);
	    else if(particleID==3) h_dEvsE_4He->Fill(Energy_in_silicon,dE_scat);
	    else if(particleID==4) h_dEvsE_12C->Fill(Energy_in_silicon,dE_scat);
	    else if(particleID==5) h_dEvsE_13C->Fill(Energy_in_silicon,dE_scat);
	  }
	}
	else {
	  if(DEBUGFLAG>0) cout << "REJECTED" << endl;
	  if(SilWall[t]>1) h_dEvsE_MAYA_rej->Fill(Energy_in_silicon,dE_scat);
	  if(SilWall[t]<2) h_dEvsE_DSSD_rej->Fill(Energy_in_silicon,dE_scat);
	  h_dEvsE_ALL_rej->Fill(Energy_in_silicon,dE_scat);

	  if(simuFlag){
	    if(particleID==1) h_dEvsE_p_rej->Fill(Energy_in_silicon,dE_scat);
	    else if(particleID==2) h_dEvsE_3He_rej->Fill(Energy_in_silicon,dE_scat);
	    else if(particleID==3) h_dEvsE_4He_rej->Fill(Energy_in_silicon,dE_scat);
	    else if(particleID==4) h_dEvsE_12C_rej->Fill(Energy_in_silicon,dE_scat);
	    else if(particleID==5) h_dEvsE_13C_rej->Fill(Energy_in_silicon,dE_scat);
	  }
	}
	cout << "Particle ID: " << particleID << endl;
      }
    }
    //End of reaction analysis

    if(TrackBragFlag){
      Can_brag->cd(1);
      if(h_scat_range1 && bragR1){
	h_scat_range1->Draw();
	bragR1->Draw("same");
	TLine lineya1(RangeA[0], 0, RangeA[0], 10e7);
	lineya1.SetLineWidth(3);
	lineya1.Draw("same");
	TLine lineyb1(RangeB[0], 0, RangeB[0], 10e7);
	lineyb1.Draw("same");
	lineyb1.SetLineWidth(3);
      }
      Can_brag->cd(2);
      if(h_scat_range2 && bragR2){
	h_scat_range2->Draw();
	bragR2->Draw("same");
	TLine lineya2(RangeA[1], 0, RangeA[1], 10e7);
	lineya2.SetLineWidth(3);
	lineya2.Draw("same");
	TLine lineyb2(RangeB[1], 0, RangeB[1], 10e7);
	lineyb2.Draw("same");
	lineyb2.SetLineWidth(3);
      }
      Can_brag->Update();
    }
    if(Track3DVisuFlag){
      Can_3D->cd();
      h3DTrack->Draw("lego2");
      TPolyLine3D *pl0 = new TPolyLine3D(2);
      pl0->SetPoint(0,T[0]->Xm/2,T[0]->Ym/2,T[0]->Zm/2);
      pl0->SetPoint(1,T[0]->Xh/2,T[0]->Yh/2,T[0]->Zh/2);
      pl0->SetLineColor(3);
      pl0->SetLineWidth(3);
      pl0->Draw("same");

      TPolyLine3D *pl = new TPolyLine3D(2);
      pl->SetPoint(0,PointA[0][0]/2,PointA[1][0]/2,PointA[2][0]/2);
      pl->SetPoint(1,PointB[0][0]/2,PointB[1][0]/2,PointB[2][0]/2);
      pl->SetLineColor(2);
      pl->SetLineWidth(3);
      pl->Draw("same");
      Can_3D->Update();
    }
    if(TrackVisuFlag){
      Can_track->cd(1);
      visu_charge->Draw("colz");
      if(track1 && GOOD_EVENT) {
	TLine linefit1(IniPoint[0][0], IniPoint[1][0], FinPoint[0][0], FinPoint[1][0]);
	linefit1.SetLineStyle(kDashed);
	linefit1.SetLineWidth(5);
	linefit1.Draw("same");
	TLine linefitreg1(IniFitPoint[0][0], IniFitPoint[1][0], FinPoint[0][0], FinPoint[1][0]);
	linefitreg1.SetLineWidth(5);
	linefitreg1.Draw("same");
	TLine linedl1(PointA[0][0]/2., PointA[1][0]/2., PointB[0][0]/2., PointB[1][0]/2.);
	linedl1.SetLineWidth(5);
	linedl1.SetLineColor(kBlue);
	linedl1.Draw("same");
      }

      if(track2 && GOOD_EVENT){
	TLine linefit2(IniPoint[0][1], IniPoint[1][1], FinPoint[0][1], FinPoint[1][1]);
	linefit2.SetLineStyle(kDashed);
	linefit2.SetLineWidth(5);
	linefit2.Draw("same");
	TLine linefitreg2(IniFitPoint[0][1], IniFitPoint[1][1], FinPoint[0][1], FinPoint[1][1]);
	linefitreg2.SetLineWidth(5);
	linefitreg2.Draw("same");
	TLine linedl2(PointA[0][1]/2., PointA[1][1]/2., PointB[0][1]/2., PointB[1][1]/2.);
	linedl2.SetLineWidth(5);
	linedl2.SetLineColor(kGreen+2);
	linedl2.Draw("same");
      }

      TLine line1(0, numberOfRows/2-beamWidth, numberOfColumns, numberOfRows/2-beamWidth);
      line1.Draw("same");
      TLine line2(0, numberOfRows/2+beamWidth, numberOfColumns, numberOfRows/2+beamWidth);
      line2.Draw("same");

      Can_track->cd(2);
      visu_time->Draw("colz");

      Can_track->Update();
      Can_track->WaitPrimitive();
    }
  }
  //End of Event Loop

  if(!ReadWriteFlag && !EventFlag){
    if(DeltaTimeFlag){
      TCanvas* Can_DeltaTime=new TCanvas("Can_DeltaTime","Can_DeltaTime",900,900);
      Can_DeltaTime->Divide(2,1);
      Can_DeltaTime->cd(1);
      h_DeltaTime->Draw();
      Can_DeltaTime->cd(2);
      h_DeltaHeight->Draw();
    }
    if(VertexFlag){
      TCanvas* Can_vertex=new TCanvas("Can_vertex","Can_vertex",900,900);
      Can_vertex->Divide(2,1);
      Can_vertex->cd(1);
      h_Vertex_X->Draw();
      Can_vertex->cd(2);
      h_Vertex_Y->Draw();
    }
    if(AngleFlag){
      TCanvas* Can_angle=new TCanvas("Can_angle","Can_angle",900,900);
      Can_angle->Divide(1,2);
      Can_angle->cd(1);
      h3DAngle->Draw();
      Can_angle->cd(2)->Divide(2,1);
      Can_angle->cd(2)->cd(1);
      hHorizAngle->Draw();
      Can_angle->cd(2)->cd(2);
      hVertiAngle->Draw();
    }

    if(DistFlag){
      TCanvas* Can_dist=new TCanvas("Can_dist","Can_dist",900,900);
      Can_dist->Divide(2,2);
      //Can_dist->cd(1);
      //hdistVO->Draw();
      Can_dist->cd(1);
      hdistVF->Draw();
      Can_dist->cd(2);
      hdistOF->Draw();
      Can_dist->cd(3);
      //hdistOFvsVO->Draw();
      hdistOFvsVF->Draw();
      Can_dist->cd(4);
      hdistOFvsTheta->Draw();
    }

    if(PIdentFlag){
      TCanvas* Can_sil=new TCanvas("Can_sil","Can_sil",900,900);
      Can_sil->Divide(2,2);
      Can_sil->cd(1);
      h_SilCharge0->Draw();
      Can_sil->cd(2);
      h_SilCharge1->Draw();
      Can_sil->cd(3);
      h_SilCharge2->Draw();
      Can_sil->cd(4);
      h_SilCharge3->Draw();
      Can_sil->Update();

      TCanvas* Can_silmult=new TCanvas("Can_silmult","Can_silmult",900,900);
      h_SilMult->Draw();
      Can_silmult->Update();

      TCanvas* Can_pident=new TCanvas("Can_pident","Can_pident",900,900);
      Can_pident->Divide(2,2);
      Can_pident->cd(1);
      h_dEvsE_DSSD1->SetXTitle("Energy in DSSD1 [MeV]");
      h_dEvsE_DSSD1->SetYTitle("Delta E [MeV] (Pads)");
      h_dEvsE_DSSD1->SetMarkerStyle(2);
      h_dEvsE_DSSD1->Draw();

      Can_pident->cd(2);
      h_dEvsE_DSSD2->SetXTitle("Energy in DSSD2 [MeV]");
      h_dEvsE_DSSD2->SetYTitle("Delta E [MeV] (Pads)");
      h_dEvsE_DSSD2->SetMarkerStyle(2);
      h_dEvsE_DSSD2->Draw();

      Can_pident->cd(3);
      h_dEvsE_DSSD3->SetXTitle("Energy in DSSD3 [MeV]");
      h_dEvsE_DSSD3->SetYTitle("Delta E [MeV] (Pads)");
      h_dEvsE_DSSD3->SetMarkerStyle(2);
      h_dEvsE_DSSD3->Draw();

      Can_pident->cd(4);
      h_dEvsE_DSSD4->SetXTitle("Energy in DSSD4 [MeV]");
      h_dEvsE_DSSD4->SetYTitle("Delta E [MeV] (Pads)");
      h_dEvsE_DSSD4->SetMarkerStyle(2);
      h_dEvsE_DSSD4->Draw();
    }

    if(IdentFlag){
      /*
	TCanvas* Can_ident2=new TCanvas("Can_ident2","Can_ident2",900,900);
	h_dEvsE_ALL2->SetXTitle("Energy in Silicon [MeV]");
	h_dEvsE_ALL2->SetYTitle("Delta E [MeV]/Fit Length [mm] (Pads)");
	h_dEvsE_ALL2->SetMarkerStyle(2);
	h_dEvsE_ALL2->Draw();
      */
      TCanvas* Can_ident=new TCanvas("Can_ident","Can_ident",900,900);
      Can_ident->Divide(1,2);
      Can_ident->cd(1);
      h_dEvsE_ALL->SetXTitle("Energy in Silicon [MeV]");
      h_dEvsE_ALL->SetYTitle("Delta E [MeV] (Pads)");
      h_dEvsE_ALL->SetMarkerStyle(2);
      h_dEvsE_ALL->Draw();
      //h_dEvsE_ALL->Draw("colz");
      Can_ident->cd(2)->Divide(2,1);
      Can_ident->cd(2)->cd(1);
      h_dEvsE_MAYA->SetXTitle("Energy in Silicon [MeV]");
      h_dEvsE_MAYA->SetYTitle("Delta E [MeV] (Pads)");
      h_dEvsE_MAYA->SetMarkerStyle(2);
      h_dEvsE_MAYA->Draw();
      Can_ident->cd(2)->cd(2);
      h_dEvsE_DSSD->SetXTitle("Energy in Silicon [MeV]");
      h_dEvsE_DSSD->SetYTitle("Delta E [MeV] (Pads)");
      h_dEvsE_DSSD->SetMarkerStyle(2);
      h_dEvsE_DSSD->Draw();

      if(simuFlag){
	TCanvas* Can_ident_ID=new TCanvas("Can_ident_ID","Can_ident_ID",900,900);
	Can_ident_ID->Divide(1,2);
	Can_ident_ID->cd(1)->Divide(3,1);;
	Can_ident_ID->cd(1)->cd(1);
	//h_dEvsE_p->SetXTitle("Energy in Silicon [MeV]-Proton");
	h_dEvsE_p->SetXTitle("Energy in Silicon [MeV]-2H");
	h_dEvsE_p->SetYTitle("Delta E [MeV] (Pads)");
	h_dEvsE_p->SetMarkerStyle(2);
	h_dEvsE_p->Draw();
	Can_ident_ID->cd(1)->cd(2);
	h_dEvsE_3He->SetXTitle("Energy in Silicon [MeV]-3He");
	h_dEvsE_3He->SetYTitle("Delta E [MeV] (Pads)");
	h_dEvsE_3He->SetMarkerStyle(2);
	h_dEvsE_3He->Draw();
	Can_ident_ID->cd(1)->cd(3);
	h_dEvsE_4He->SetXTitle("Energy in Silicon [MeV]-4He");
	h_dEvsE_4He->SetYTitle("Delta E [MeV] (Pads)");
	h_dEvsE_4He->SetMarkerStyle(2);
	h_dEvsE_4He->Draw();
	Can_ident_ID->cd(2)->Divide(2,1);
	Can_ident_ID->cd(2)->cd(1);
	h_dEvsE_12C->SetXTitle("Energy in Silicon [MeV]-12C");
	h_dEvsE_12C->SetYTitle("Delta E [MeV] (Pads)");
	h_dEvsE_12C->SetMarkerStyle(2);
	h_dEvsE_12C->Draw();
	Can_ident_ID->cd(2)->cd(2);
	h_dEvsE_13C->SetXTitle("Energy in Silicon [MeV]-13C");
	h_dEvsE_13C->SetYTitle("Delta E [MeV] (Pads)");
	h_dEvsE_13C->SetMarkerStyle(2);
	h_dEvsE_13C->Draw();
      }

      TCanvas* Can_ident_rej=new TCanvas("Can_ident_rej","Can_ident_rej",900,900);
      Can_ident_rej->Divide(1,2);
      Can_ident_rej->cd(1);
      h_dEvsE_ALL_rej->SetXTitle("Energy in Silicon [MeV]");
      h_dEvsE_ALL_rej->SetYTitle("Delta E [MeV] (Pads)");
      h_dEvsE_ALL_rej->SetMarkerStyle(2);
      h_dEvsE_ALL_rej->Draw();
      Can_ident_rej->cd(2)->Divide(2,1);
      Can_ident_rej->cd(2)->cd(1);
      h_dEvsE_MAYA_rej->SetXTitle("Energy in Silicon [MeV]");
      h_dEvsE_MAYA_rej->SetYTitle("Delta E [MeV] (Pads)");
      h_dEvsE_MAYA_rej->SetMarkerStyle(2);
      h_dEvsE_MAYA_rej->Draw();
      Can_ident_rej->cd(2)->cd(2);
      h_dEvsE_DSSD_rej->SetXTitle("Energy in Silicon [MeV]");
      h_dEvsE_DSSD_rej->SetYTitle("Delta E [MeV] (Pads)");
      h_dEvsE_DSSD_rej->SetMarkerStyle(2);
      h_dEvsE_DSSD_rej->Draw();

      if(simuFlag){
	TCanvas* Can_ident_ID_rej=new TCanvas("Can_ident_ID_rej","Can_ident_ID_rej",900,900);
	Can_ident_ID_rej->Divide(1,2);
	Can_ident_ID_rej->cd(1)->Divide(3,1);;
	Can_ident_ID_rej->cd(1)->cd(1);
	h_dEvsE_p_rej->SetXTitle("Energy in Silicon [MeV]-Proton");
	h_dEvsE_p_rej->SetYTitle("Delta E [MeV] (Pads)");
	h_dEvsE_p_rej->SetMarkerStyle(2);
	h_dEvsE_p_rej->Draw();
	Can_ident_ID_rej->cd(1)->cd(2);
	h_dEvsE_3He_rej->SetXTitle("Energy in Silicon [MeV]-3He");
	h_dEvsE_3He_rej->SetYTitle("Delta E [MeV] (Pads)");
	h_dEvsE_3He_rej->SetMarkerStyle(2);
	h_dEvsE_3He_rej->Draw();
	Can_ident_ID_rej->cd(1)->cd(3);
	h_dEvsE_4He_rej->SetXTitle("Energy in Silicon [MeV]-4He");
	h_dEvsE_4He_rej->SetYTitle("Delta E [MeV] (Pads)");
	h_dEvsE_4He_rej->SetMarkerStyle(2);
	h_dEvsE_4He_rej->Draw();
	Can_ident_ID_rej->cd(2)->Divide(2,1);
	Can_ident_ID_rej->cd(2)->cd(1);
	h_dEvsE_12C_rej->SetXTitle("Energy in Silicon [MeV]-12C");
	h_dEvsE_12C_rej->SetYTitle("Delta E [MeV] (Pads)");
	h_dEvsE_12C_rej->SetMarkerStyle(2);
	h_dEvsE_12C_rej->Draw();
	Can_ident_ID_rej->cd(2)->cd(2);
	h_dEvsE_13C_rej->SetXTitle("Energy in Silicon [MeV]-13C");
	h_dEvsE_13C_rej->SetYTitle("Delta E [MeV] (Pads)");
	h_dEvsE_13C_rej->SetMarkerStyle(2);
	h_dEvsE_13C_rej->Draw();
      }
    }
  }
}//End Analysis_reduce


void FindMax(TSpline3 *sp,Double_t *maximum,Double_t *x){
  Double_t max;
  Double_t posmax=0;
  Double_t x0=0;
  Double_t xmax=32;
  Double_t step=0.2;
  for(Double_t val=x0;val<xmax;val=val+step){
    Double_t value=sp->Eval(val);
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


void FindRangeX(TSpline3 *sp,Double_t maxval,Double_t maxpos,Double_t theta,Double_t *x){
  Double_t max;
  Double_t posmax=0;
  Double_t x0=maxpos;
  Double_t xmax=128;
  Double_t step=0.2;
  if(theta>0){
    for(Double_t val=x0;val<xmax;val=val+step){
      Double_t value=sp->Eval(val);
      if(value>=maxval/4){
	max=value;
	posmax=val;
      }
    }
  }

  else if(theta<0){
    for(Double_t val=x0;val>0;val=val-step){
      Double_t value=sp->Eval(val);
      if(value>=maxval/4){
	max=value;
	posmax=val;
      }
    }
  }
  //cout<<"X Range is at--> "<<posmax<<" "<<max<<endl;
  //*maximum=max;
  *x=posmax;
  return;
}


void FindRangeY(TSpline3 *sp,Double_t maxval,Double_t maxpos,Double_t *x){
  Double_t max;
  Double_t posmax=0;
  Double_t x0=maxpos;
  Double_t xmax=64;
  Double_t step=0.2;
  for(Double_t val=x0;val<xmax;val=val+step){
    Double_t value=sp->Eval(val);
    if(value>=maxval/4){
      max=value;
      posmax=val;
    }
  }
  //cout<<"X Range is at--> "<<posmax<<" "<<max<<endl;
  //*maximum=max;
  *x=posmax;
  return;
}


void FindRangeZ(TSpline3 *sp,Double_t maxval,Double_t maxpos,Double_t phi,Double_t maxZ, Double_t *x){
  Double_t max;
  Double_t posmax=0;
  Double_t x0=maxpos;
  Double_t xmax=maxZ;
  //Double_t xmax=170;
  Double_t step=0.2;
  if(phi>0){
    for(Double_t val=x0;val<xmax;val=val+step){
      Double_t value=sp->Eval(val);
      if(value>=maxval/4){
	max=value;
	posmax=val;
      }
    }
  }

  else if(phi<0){
    for(Double_t val=x0;val>0;val=val-step){
      Double_t value=sp->Eval(val);
      if(value>=maxval/4){
	max=value;
	posmax=val;
      }
    }
  }
  //cout<<"Y Range is at--> "<<posmax<<" "<<max<<endl;
  //*maximum=max;
  *x=posmax;
  return;
}


void FitStep(Int_t nstep, Double_t *x, Double_t *z, Double_t &a, Double_t &b) {
  Int_t s;
  Double_t A, B, C, UEV, Q, X, Xg, Y, Yg;
  A=B=C=UEV=Q=X=Y=Xg=Yg=0.;

  for (Int_t s=0;s<nstep;s++) {
    Q+=1;
    Xg+=z[s];
    Yg+=x[s];
  }

  Xg/=Q;
  Yg/=Q;
  //cout<<"Xg sim : "<<Xg<<" Yg sim : "<<Yg<<endl;

  for (Int_t s=0;s<nstep;s++) {
    A+=(z[s]-Xg)*(z[s]-Xg);
    B+=(x[s]-Yg)*(z[s]-Xg);
    C+=(x[s]-Yg)*(x[s]-Yg);
  }

  UEV=0.5*(A+C+sqrt((A+C)*(A+C)-4*(A*C-B*B)));
  a=B/(UEV-C);
  b=Yg-a*Xg;
}


void FitMat(Double_t **PADNET, Int_t minRow, Int_t maxRow, Int_t minColumn, Int_t maxRow, Double_t threshold, Double_t &a, Double_t &b){
  Int_t Row, Col;
  Double_t A, B, C, UEV, Q, X, Xg, Y, Yg;
  A=B=C=UEV=Q=X=Y=Xg=Yg=0.;
  for (Row=minRow;Row<=maxRow;Row++)
    for (Col=minColumn;Col<=maxRow;Col++)
      if(PADNET[Row][Col]>threshold) {
	X= Col*2.+1.;
	Y= Row*2.+1.;
	Q+=PADNET[Row][Col];
	Xg+=X*PADNET[Row][Col];
	Yg+=Y*PADNET[Row][Col];
      }
  Xg/=Q;
  Yg/=Q;
  //cout<<"Xg: "<<Xg<<" Yg: "<<Yg<<endl;

  for (Row=minRow;Row<=maxRow;Row++)
    for(Col=minColumn;Col<=maxRow;Col++)
      if(PADNET[Row][Col]>threshold) {
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


void FitMatZ(Double_t **PADNET, Double_t **TIME, Int_t minRow, Int_t maxRow, Int_t minColumn, Int_t maxRow, Double_t threshold, Double_t timeThreshold, Double_t &a, Double_t &b) {
  Int_t Row, Col;
  const Int_t NRow=32;
  Double_t A, B, C, UEV, Q, X, Xg, Y, Yg;
  Double_t Qrow[NRow],Cm[NRow],Zm[NRow];
  Int_t min_col,max_col,ncol;
  //cout<<"Ttreshold: "<<timeThreshold<<", NRow: "<<NRow<<endl;
  A=B=C=UEV=Q=X=Y=Xg=Yg=0.;
  for (Row=minRow;Row<=maxRow;Row++) {
    Zm[Row]=0;
    Qrow[Row]=0;
    Cm[Row]=0.;
    min_col=max_col=ncol=0;
    for (Col=minColumn;Col<=maxRow;Col++) {
      if(PADNET[Row][Col]>threshold && TIME[Row][Col]>timeThreshold) {
	ncol++;
	if(min_col==0)min_col=Col;
	max_col=Col;
	Cm[Row]+=PADNET[Row][Col]*(Col+1);//+1 to avoid problem at Col=0
	Qrow[Row]+=PADNET[Row][Col];
	//cout<<"Row: "<<Row<<", minC: "<<min_col<<", maxC: "<<max_col<<", PAD: "<<PADNET[Row][Col]<<", NRow: "<<NRow<<endl;
      }
    }

    if(ncol!=0) {
      Cm[Row]/=Qrow[Row];
      Cm[Row]-=1.;
      for(Col=min_col;Col<max_col+1;Col++){
	if(PADNET[Row][Col]>threshold && TIME[Row][Col]>timeThreshold) {
	  Zm[Row]+=TMath::Sqrt(TIME[Row][Col]*TIME[Row][Col]-4*(Col-Cm[Row])*(Col-Cm[Row]));
	}
      }
      Zm[Row]/=ncol;
      //cout<<"Row: "<<Row<<", QCol: "<<Qrow[Row]<<", ZmCol: "<<Zm[Row]<<endl;
    }
  }

  for (Row=minRow;Row<=maxRow;Row++) {
    if(Cm[Row] && Zm[Row]) {
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

  for (Row=minRow;Row<=maxRow;Row++){
    if(Cm[Row] && Zm[Row]){
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


//Double_t FitMat3D(Int_t T, TMatrixD PADNET, TMatrixD TIME, Int_t minRow, Int_t maxRow, Int_t minColumn, Int_t maxRow, Double_t threshold, MTrack* T)
Double_t FitMat3D(Double_t **PADNET, Double_t **HEIGHT, Int_t minRow, Int_t maxRow, Int_t minColumn, Int_t maxRow, Double_t threshold, MTrack* T){
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

  //cout<<"minRow "<<minRow<<" maxRow "<<maxRow<<" minColumn "<<minColumn<<" maxRow "<<maxRow<<endl;

  Q=Xm=Ym=Zm=0.;
  Sxx=Syy=Szz=Sxy=Sxz=Syz=0.;

  for (R=minRow;R<=maxRow;R++)
    for (C=minColumn;C<=maxRow;C++)
      if(PADNET[R][C]>threshold && HEIGHT[R][C]){
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
  if(r<0) {
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

void HeightCorrection(Double_t **PADNET, Double_t **HEIGHT, Int_t minRow, Int_t maxRow, Int_t minColumn, Int_t maxRow, Double_t threshold, Double_t timeThreshold){

  Int_t Row, Col;
  const Int_t NRow=32;
  Double_t Qrow[NRow],Cm[NRow],Zm[NRow];
  Int_t min_col,max_col,ncol;

  for (Row=minRow;Row<=maxRow;Row++) {
    Zm[Row]=0;
    Qrow[Row]=0;
    Cm[Row]=0.;
    min_col=max_col=ncol=0;

    for (Col=minColumn;Col<=maxRow;Col++) {
	if(PADNET[Row][Col]>threshold && HEIGHT[Row][Col]>timeThreshold) {
	  ncol++;
	  if(min_col==0)min_col=Col;
	  max_col=Col;
	  Cm[Row]+=PADNET[Row][Col]*(Col+1);//+1 to avoid problem at Col=0
	  Qrow[Row]+=PADNET[Row][Col];
	  //cout<<"Row: "<<Row<<", minC: "<<min_col<<", maxC: "<<max_col<<", PAD: "<<PADNET[Row][Col]<<", NRow: "<<NRow<<endl;
	}
    }

    if(ncol!=0) {
      Cm[Row]/=Qrow[Row];
      Cm[Row]-=1.;
      for(Col=min_col;Col<max_col+1;Col++){
        if(PADNET[Row][Col]>threshold && HEIGHT[Row][Col]>timeThreshold) {
          Zm[Row]+=TMath::Sqrt(HEIGHT[Row][Col]*HEIGHT[Row][Col]-4*(Col-Cm[Row])*(Col-Cm[Row]));
        }
      }

      Zm[Row]/=ncol;
      //cout<<"Row: "<<Row<<", QCol: "<<Qrow[Row]<<", ZmCol: "<<Zm[Row]<<endl;
      for(Col=min_col;Col<max_col+1;Col++) {
        if(PADNET[Row][Col]>threshold && HEIGHT[Row][Col]>timeThreshold) {
          HEIGHT[Row][Col]=Zm[Row];
        }
      }
    }
  }
}
