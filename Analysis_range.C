//D. Perez 280911
//101111 Added silicon hits
//180713 Added PARIS hits
#define NUM_CLUSTERS 16
#define NUM_CRYSTALS 9 

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
//#include "digit.h"
//#include "include/ActarSimPrimaryInfo.hh"
#include <iostream>
//void FindMax(TSpline3 *sp,Float_t *maximum,Float_t *x);
//void FindRange(TSpline3 *sp,Float_t maxval,Float_t maxpos,Float_t *x);
//void FitMat(Double_t **PADNET, Double_t padSize, Double_t sideBlankSpace,Double_t xLength,Int_t Rmin,
//	    Int_t Rmax, Int_t Cmin, Int_t Cmax, Double_t threshold, Double_t &a, Double_t &b);

Int_t npeaks=20;

void Analysis_range(Char_t *gasfile_name,Char_t *name="root_files/simFile.root",Char_t *name2="root_files/digiData.root",Char_t *name3="root_files/Anl_output.root",Int_t event=0,Bool_t draw=0){
  //void prueba(Int_t gas=0,Char_t *name="root_files/simFile.root",Char_t *name2="root_files/digiData.root",Char_t *name3="root_files/prueba.root",Int_t event=0,Bool_t draw=0){
  gROOT->ProcessLine(".L digit.h+");
  //gSystem->Load("digit_david8_h.so");
  gSystem->Load("libactar.sl");
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1);

  //TFile *spline_file=new TFile("p_on_D2_stp.root");
  //TFile *spline_file=new TFile("p_on_D2_500mbar.root");
  //TFile *spline_file=new TFile("p_on_D2_260mbar.root");
  TFile *spline_file=new TFile("p_on_D2_400mbar.root");
  //TSpline3 *spline_range=(TSpline3*)spline_file->Get("Protons_deuteriumSTP");
  //TSpline3 *spline_range=(TSpline3*)spline_file->Get("Protons_deuterium_500mbar");
  //TSpline3 *spline_range=(TSpline3*)spline_file->Get("Protons_deuterium_260mbar");
  if(spline_file)
    TSpline3 *spline_range=(TSpline3*)spline_file->Get("Protons_deuterium_400mbar");
  padsGeometry thePadsGeometry;
  thePadsGeometry.SetGeometryValues(0,0,0,151.,151.,150,151.,2); //Rectangular Pads 30x30x30 cm^3 2mm pads
  //thePadsGeometry.SetGeometryValues(0,0,0,150.,150.,150,150.,2); //Rectangular Pads 30x30x30 cm^3 2mm pads
  //thePadsGeometry.SetGeometryValues(0,0,0,150.,150.,150.,150.,2); //Rectangular Pads 30x30x30 cm^3 2mm pads
  //thePadsGeometry.SetGeometryValues(0,0,0,20.,20.,20.,20.,2); //Rectangular Pads 30x30x30 cm^3 2mm pads
  
  driftManager theDriftManager;

  Char_t dummy[256];
  Char_t gas[256];
  Double_t v_drift,sigma_trans,sigma_long;
  ifstream *gasfile=new ifstream(gasfile_name);
  gasfile->getline(gas,256);
  cout<<gas<<endl;
  gasfile->getline(dummy,256);
   cout<<dummy<<endl;
  *gasfile>>dummy>>dummy>>v_drift;
  //cout<<dummy<<endl;
  *gasfile>>dummy>>dummy>>sigma_trans;
  //cout<<dummy<<endl;
  *gasfile>>dummy>>dummy>>sigma_long;
  //cout<<dummy<<endl;

  delete gasfile;  
  //theDriftManager.SetDriftVelocity(v_drift);  // in mm/ns
  //theDriftManager.SetDiffusionParameters(sigma_long,sigma_trans); // in mm^2/ns 
  //theDriftManager.SetDriftParameters(15000.,300.,1013.25,"deuterium");
  //theDriftManager.SetDriftParameters(15000.,300.,260,"deuterium");
  theDriftManager.SetDriftParameters(15000.,300.,400,"deuterium");
  //theDriftManager.SetDriftParameters(15000.,300.,500,"deuterium");
  //theDriftManager.SetDriftParameters(2000.,300.,150,"isobutane");
  cout<<"Drift Parameters are:"<<endl;  
  cout<<"v_drift---------> "<<theDriftManager.GetDriftVelocity()<<"mm/ns"<<endl;  
  cout<<"D_long----------> "<<theDriftManager.GetLongitudinalDiffusion()<<"mm^2/ns"<<endl;  
  cout<<"D_trans---------> "<<theDriftManager.GetTransversalDiffusion()<<"mm^2/ns"<<endl;  

   
  //Int_t beamWidth   = 0;
  Int_t beamWidth   = 4;
  Int_t marginWidth = 1;
  //Int_t marginWidth = 2;
  //Double_t dynamicRange=1000000.;
  Double_t dynamicRange=1.;
  Double_t PI=3.1415926535897932384626433;
  Double_t deg=180./PI;
  Double_t rad=PI/180.;
  //==================================================================================//

  Double_t padSize  = thePadsGeometry.GetPadSize();
  Int_t    padType  = thePadsGeometry.GetPadType();
  Int_t    padLayout= thePadsGeometry.GetPadLayout();
  Double_t sideBlankSpace = thePadsGeometry.GetSideBlankSpace();
  Double_t rHexagon = thePadsGeometry.GetRHexagon();
  Double_t xLength  = thePadsGeometry.GetXLength();
  Double_t yLength  = thePadsGeometry.GetYLength();
  Double_t zLength  = thePadsGeometry.GetZLength();
  //yLength=100;
  cout<<"Y length===> "<<yLength<<endl;
  Double_t tracked_position;
  Double_t tracked_positionV;
 
  Int_t numberOfRows   = thePadsGeometry.GetNumberOfRows();
  Int_t numberOfColumns= thePadsGeometry.GetNumberOfColumns();
  Int_t HitRows=0;
  Int_t HitColumns=0;

  Int_t *multiplicity=new Int_t[numberOfColumns];
  //Int_t *multiplicity_Y=new Int_t[numberOfRows];
  Double_t mean_mult;
  Double_t suma;
  Double_t Theta3D;
  Double_t Theta3DX;
  for(Int_t k=0;k<numberOfColumns;k++){
    multiplicity[k]=0;
    // cout<<*multiplicity<<endl;  
  }
  //for(Int_t k=0;k<numberOfRows;k++){
  //  multiplicity_Y[k]=0;
    // cout<<*multiplicity<<endl;  
  //}
  //cout<< numberOfRows<<" "<< numberOfColumns<<endl;
  
  //Double_t threshold = 0;
  Double_t threshold = 10000;
  //Double_t sigma_time=3;//ns
  Double_t sigma_time=6;//ns
  //Double_t sigma_time=0;//ns
  //Double_t sigma_time=8;//ns iC4H10
  cout<<sigma_time<<" ns"<<endl;
  //Double_t driftVelocity = theDriftManager.GetDriftVelocity();
  Double_t driftVelocity = theDriftManager.GetDriftVelocity();
  Double_t timeVertex=0.;  
  Double_t E_vertex;

  TH2F *h2=new TH2F("ACTAR","ACTAR", numberOfColumns,0,2*zLength,numberOfRows,-xLength,xLength);
  TH2F *h_beam=new TH2F("Beam","Beam", numberOfColumns,0,2*zLength,numberOfRows,-xLength,xLength);
  //TH2F *h2=new TH2F("ACTAR","",numberOfRows,-xLength,xLength,numberOfColumns,0,2*zLength);
  TH2F *h2XZ=new TH2F("TrackXZ","",150,0,300,150,-150,150);
  TH2F *h2XZbis=new TH2F("TrackXZbis","",300,0,300,300,-150,150);
  TGraph *gXZ=new TGraph();
  TH2F *h2YZ=new TH2F("TrackYZ","",300,0,300,300,-150,150);
  TH2F *h2XY=new TH2F("TrackXY","",300,-150,150,300,-150,150);
  TH2F *hcompare=new TH2F("Compared","",151,-50,50,151,-50,50);
  TH2F *hsim=new TH2F("Simulated","",151,-50,50,151,-50,50);
  TH3F *h3=new TH3F("Track","",300,0,300,300,-150,150,300,-150,150);
  TH2F *h2t=new TH2F("ACTAR_t","ACTAR", numberOfColumns,0,2*zLength,numberOfRows,-xLength,xLength);
  TH2F *h2t_x=new TH2F("ACTAR_t_x","ACTAR time X",numberOfRows,-xLength,xLength,2000,0,2e4);
  TH2F *h2t_z=new TH2F("ACTAR_t_z","ACTAR time Z",numberOfColumns,0,2*zLength,2000,0,2e4);
  TH2F *h2y=new TH2F("ACTAR_y","ACTAR", numberOfColumns,0,2*zLength,numberOfRows,-yLength,yLength);
  TH1F *h_range=new TH1F("Bragg","",150, 0.,300);
  TH1F *h_range_X=new TH1F("Bragg_X","",150, -150.,150);
  TH1F *h_range2=new TH1F("Bragg2","",92, 0.,185);
  TH1F *h_angle=new TH1F("Angle","Angular resolution at 14 cm",300, 0., 300.);
  TH1F *h_energy=new TH1F("Energy","Total Charge Deposited",500, 500., 1000.);
  TH1F *histo=new TH1F("Range","",70, 0.,140);
  TH1F *histo2=new TH1F("Range2","",70, 0.,140);
  TH1F *h_horpos=new TH1F("Hor_pos","Tracked Horizontal position",400, -20.,20);
  TH1F *h_sim_horpos=new TH1F("SimHor_pos","Simulated Horizontal position",800, -100.,100);
  TH1F *h_fit_horpos=new TH1F("FitHor_pos","Fitted Horizontal position",800, -100.,100);
  TH1F *h_fit_horpos2=new TH1F("FitHor_pos2","Fitted Horizontal position",800, -100.,100);
  TH1F *h_residuals=new TH1F("ResHor_pos","Residuals Horizontal position",200, -10.,10);
  TH1F *h_inipos=new TH1F("Ini_pos","Initial position",200, -5.,5);
  TH2F *hinipos_finalpos=new TH2F("positions",";Initial_pos;Final_pos",300,-10,10,300,-20,20);
  h_sim_horpos->SetLineColor(2);
  h_fit_horpos->SetLineColor(3);
  h_fit_horpos2->SetLineColor(4);
  TH1F *h_verpos=new TH1F("Ver_pos","Tracked Vertical position",800, -100.,100);
  TH1F *h_sim_verpos=new TH1F("SimVer_pos","Simulated Vertical position",20, -20.,20);
  h_sim_verpos->SetLineColor(2);
  TH1F *h_verpos2=new TH1F("Ver_pos2","Tracked Vertical position sigma=1",200, -100.,100);
  TH1F *h_verpos2=new TH1F("Ver_pos3","Tracked Vertical position sigma=0.25",200, -50.,50);
  TH1F *h_vangle=new TH1F("Ver_angle","Vertical angle",500, -50.,50);
  TH1F *h_hangle=new TH1F("Hor_angle","Horizontal angle",500, -50.,50);
  TH1F *h_deltaT=new TH1F("DeltaT","",500, -50.,50);
  TH1F *htime=new TH1F("htime","",200, 1000.,3000);

  TH1F *hcharge20=new TH1F("hcharge20","charge column 15;Charge [fC]",500,0,30000);
  TH1F *hcharge30=new TH1F("hcharge30","charge column 10",500,0,30000);
  TH1F *hcharge10=new TH1F("hcharge10","charge column 5",500,0,30000);
  TH1F *htotcharge=new TH1F("hcharge","TOTAL Charge",500,0,30000);

  TGraph *gtime=new TGraph();
  TGraph *gtime_X=new TGraph();
  //gtime->Set(numberOfColumns);
  TSpline3 *brag;
  TSpline3 *bragX;
  //TH2F *siliconH=new TH2F("silhits","",300,-150,150,300,-150,150);
  //TH2F *siliconH2=new TH2F("silhits2","",300,0,300,300,-150,150);
  TH2F *fin_pos=new TH2F("fin_pos","",300,-150,150,300,-100,100);
  //TH1F *siliconID=new TH1F("Sil ID","",70, 0.,140);


  //Final Graphs

  TGraph *graph_anglevsrange=new TGraph();
  TGraph *graph_energyvsrange=new TGraph();
  TGraph *graph_anglevsrangeX=new TGraph();
  graph_anglevsrangeX->SetMarkerColor(4);
  Int_t point_graph=0;
  TCanvas *c;
  TCanvas *c2;
  //TCanvas *canvas=new TCanvas();;
  Int_t nentries=0;

  TH1D *px=0;
  Int_t siliconhits=0;
  Int_t scintillatorhits=0;
  Int_t DeltaT;
  if(draw){
  c=new TCanvas();
  //c2=new TCanvas();
  c->Divide(2,3);
  }
  //Event info;
  TFile *simFile=new TFile(name);
  //digitization data, output of digitizationMacro
  TFile *file=new TFile(name2);
  if(!file) 
    return;
  else{
    //getting the trees
    TTree *simTree=(TTree*)simFile->Get("The_ACTAR_Event_Tree");
    TTree *T=(TTree*)file->Get("digiTree");
    if(event==0)
      nentries=T->GetEntries();
    else
      nentries=event;
    cout<<nentries<<endl;
    Float_t *ta=new Float_t[nentries];
    Float_t *tb=new Float_t[nentries];
    
    //Create the output_file and Tree
    //TFile *histofile=new TFile("histograms_short.root","UPDATE");
    TFile *outfile=new TFile(name3,"RECREATE");
     TTree *out_tree=new TTree("thetree","out_tree");
    Float_t range;
    Float_t rangeZ;
    Float_t range2D;
    Float_t range2D_X;
    Float_t range3D;
    Float_t range3D_X;
    Float_t rangeProj;
    Float_t projected_range;
    Float_t angle;
    Float_t angleH;
    Float_t angleV;
    Float_t angleV_X;
    Float_t angle3D;
    Float_t angle3D_X;
    Float_t total_charge;
    Float_t energy,theta,phi;
    Float_t EnergySil;
    Int_t detectorID;
    Float_t SilPosX;
    Float_t SilPosY;
    Float_t SilPosZ;
    Double_t a=0.;
    Double_t b=0.;  
    // z=a*x+b in the ActarSim coordinate conventions
    Int_t Rmin=0, Rmax=0;
    Int_t Cmin=0, Cmax=0;
    TF1 *fit_track;
    TF1 *fit_track_new;
    TF1 *fit_trackv;
    Double_t sim_hor_pos;
    Double_t sim_fit_pos_full;
    Double_t sim_fit_pos_partial;
    Double_t protonEnergy;
    Double_t protonEnergy_X;
    Int_t track_length;

    
    Double_t theta3D_real=0;
    Double_t Energy_in_gas=0;
    Double_t Energy_in_silicon=0;

    Double_t Residue_Energy=0;
    Double_t Residue_Theta2D=0;
    Double_t Residue_Theta3D=0;
    Double_t Residue_Phi=0;

    //Silicon branches
    //Scintillator Branches

    out_tree->Branch("Columns",&numberOfColumns,"Cols/I");
    out_tree->Branch("track_L",&track_length,"length/I");
    out_tree->Branch("rangeZ",&rangeZ,"rangeProj/F");
    out_tree->Branch("range2D",&range2D,"rangeProj/F");
    out_tree->Branch("range2D_X",&range2D_X,"rangeProj/F");
    out_tree->Branch("range3D",&range3D,"rangeProj/F");
    out_tree->Branch("range3D_X",&range3D_X,"rangeProj/F");
    out_tree->Branch("G4Range",&range,"G4Range/F");
    out_tree->Branch("angle",&angle,"angle/F");
    out_tree->Branch("angleH",&angleH,"angle/F");
    out_tree->Branch("angleV",&angleV,"angle/F");
    out_tree->Branch("anglev_X",&angleV_X,"angle/F");
    out_tree->Branch("angle3D",&angle3D,"angle3D/F");
    out_tree->Branch("angle3D_X",&angle3D_X,"angle3D/F");
    //out_tree->Branch("phi",&phi,"phi/F");
    out_tree->Branch("charge",&total_charge,"total_charge/F");
    out_tree->Branch("energy",&energy,"energy/F");
    out_tree->Branch("Primtheta",&theta,"theta/F");
    out_tree->Branch("Primphi",&phi,"phi/F");
    out_tree->Branch("Primtheta3D",&theta3D_real,"theta3d/F");
    out_tree->Branch("a",&a,"slope/D");
    out_tree->Branch("b",&b,"intercept/D");
    out_tree->Branch("tracked_posH",&tracked_position,"tracked_position/D");
    out_tree->Branch("tracked_posV",&tracked_positionV,"tracked_position/D");
    out_tree->Branch("simulated_position",&sim_hor_pos,"sim_position/D");
    out_tree->Branch("full_fitted_position",&sim_fit_pos_full,"fit_position_F/D");
    out_tree->Branch("partial_fitted_position",&sim_fit_pos_partial,"fit_position_P/D");
   out_tree->Branch("track","TH2F",&h2,32000,0);
    out_tree->Branch("track_G4XZ","TH2F",&h2XZ,32000,0);
    out_tree->Branch("track_XZ","TGraph",&gXZ,32000,0);
    //out_tree->Branch("track_G4YZ","TH2F",&h2YZ,32000,0);
    //out_tree->Branch("track_G43D","TH3F",&h3,32000,0);
    out_tree->Branch("Bragg","TH1F",&h_range,32000,0);
    out_tree->Branch("Bragg_X","TH1F",&h_range_X,32000,0);
    out_tree->Branch("spline_Bragg","TSpline3",&brag,32000,0);
    out_tree->Branch("spline_BraggX","TSpline3",&bragX,32000,0);
    //out_tree->Branch("Rmin",&Rmin,"Rmin/I");
    //out_tree->Branch("Rmax",&Rmax,"Rmax/I");
    //out_tree->Branch("Cmin",&Cmin,"Cmin/I");
    //out_tree->Branch("Cmax",&Cmax,"Cmax/I");
    //out_tree->Branch("protonDirection","TVector3",&protonDirection,32000,1);
    //out_tree->Branch("endPoint","TVector3",&finalPoint,32000,1);
    out_tree->Branch("fit_track","TF1",&fit_track,32000,1);
    out_tree->Branch("fit_trackV","TF1",&fit_trackv,32000,1);
    out_tree->Branch("Delta_T",&DeltaT,"time/I");
    out_tree->Branch("multiplicity",multiplicity);
    //out_tree->Branch("multiplicity_Y",multiplicity_Y);
    out_tree->Branch("mean_multiplicity",&mean_mult);
    out_tree->Branch("protonEnergy",&protonEnergy,"pEnergy/D");
    out_tree->Branch("protonEnergy_X",&protonEnergy_X,"pEnergyX/D");
    out_tree->Branch("E_gas",&Energy_in_gas,"Eg/D");
    out_tree->Branch("E_silicon",&Energy_in_silicon,"Es/D");
    out_tree->Branch("protonEnergy_X",&protonEnergy_X,"pEnergyX/D");
    out_tree->Branch("E_vertex",&E_vertex,"Energy/D");
    out_tree->Branch("Residue_Energy",&Residue_Energy,"Energy/D");
    out_tree->Branch("Residue_Theta2D",&Residue_Theta2D,"Energy/D");
    out_tree->Branch("Residue_Theta3D",&Residue_Theta3D,"Energy/D");
    out_tree->Branch("Residue_Phi",&Residue_Phi,"Energy/D");

    //Silicons
 
    //Scint


    //PARIS
    Int_t ClusterID;
    Int_t DetectorID;
    TString DetectorName;
    Double_t EnergyHit;
    Double_t totalEnergy;
    Double_t EnergyCrystal_LaBr[NUM_CLUSTERS][NUM_CRYSTALS];
    Double_t EnergyCrystal_NaI[NUM_CLUSTERS][NUM_CRYSTALS];

    out_tree->Branch("EnergyCrystal_LaBr",EnergyCrystal_LaBr,"Energy[16][9]/D");
    out_tree->Branch("EnergyCrystal_NaI",EnergyCrystal_NaI,"Energy[16][9]/D");

    TClonesArray *parisHitsCA=new TClonesArray("ActarSimParisHit",2);
    TBranch *branchParisHits=simTree->GetBranch("parisHits");
    branchParisHits->SetAddress(&parisHitsCA);
    branchParisHits->SetAutoDelete(kTRUE);
    ActarSimParisHit *parisHit=new ActarSimParisHit;

    
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
    //ClonesArray to the silicon Hits
    TClonesArray *silHitsCA=new TClonesArray("ActarSimSilHit",200);
    TBranch *branchSilHits=simTree->GetBranch("silHits");
    //branchSilHits->SetAddress(&silHitsCA);
    //branchSilHits->SetAutoDelete(kTRUE);
    ActarSimSilHit *silHit=new ActarSimSilHit;
    
    //ClonesArray to the scintillator Hits
    TClonesArray *sciHitsCA=new TClonesArray("ActarSimSciHit",200);
    TBranch *branchSciHits=simTree->GetBranch("sciHits");
    //branchSciHits->SetAddress(&sciHitsCA);
    //branchSciHits->SetAutoDelete(kTRUE);
    ActarSimSciHit *sciHit=new ActarSimSciHit;
    
    //ClonesArray to the signal
    TClonesArray *padSignalCA=new TClonesArray("ActarPadSignal",4000);
    T->SetBranchAddress("padSignals",&padSignalCA);
    ActarPadSignal *padSignal=new ActarPadSignal;
    
    //Matrix for the charge map
    Double_t **padCharge=new Double_t*[numberOfRows];
    //Double_t **padCharge2=new Double_t*[numberOfRows];
    Double_t **padTime=new Double_t*[numberOfRows];
    //Double_t **padTime2=new Double_t*[numberOfRows];
    //Double_t **padTime3=new Double_t*[numberOfRows];
    for(Int_t i=0;i<numberOfRows;i++){
      padCharge[i]=new Double_t[numberOfColumns];
      //padCharge2[i]=new Double_t[numberOfColumns];
      padTime[i]=new Double_t[numberOfColumns];
    }
    //Double_t padTime[numberOfRows][numberOfColumns];
    out_tree->Branch("PadCharge",&padCharge,"PadCharge[150][150]/D");
    out_tree->Branch("PadTime",&padTime,"PadTime[150][150]/D");

    Int_t nb=0;
    //*********************************************************************************************************//
    //*********************************************************************************************************//
    //**                                                                                                     **//
    //**                                        Event Loop                                                   **//
    //**                                                                                                     **//
    //*********************************************************************************************************//
    //*********************************************************************************************************//
    
    
    for(Int_t jentry=0;jentry<nentries;jentry++){
      // cout<<"kk"<<endl;
      // clean arrays for pad charge and time
      // if(jentry%2==0){
      Residue_Energy=0;
      Residue_Theta2D=0;
      Residue_Theta3D=0;
      Residue_Phi=0;
     for(Int_t u=0;u<numberOfRows;u++){
	for(Int_t k=0;k<numberOfColumns;k++){
	  padCharge[u][k]=0.;
	  //padCharge2[u][k]=0.;
	  padTime[u][k]=0.;
	}
      }

     for(Int_t i=0;i<16;i++){
       for(Int_t j=0;j<9;j++){
	 EnergyCrystal_LaBr[i][j]=0.;
	 EnergyCrystal_NaI[i][j]=0.;
       }      
     }

      //}
      if(jentry%10==0)
	cout<<"."<<flush;
      Char_t histotitle[256];
      sprintf(histotitle,"ACTAR_%d",jentry);
      h2->SetTitle(histotitle);
      if(jentry%2==0)
	h_beam->Reset();
      h2->Reset();
      h2t->Reset();
      h2t_x->Reset();
      h2t_z->Reset();
      h2y->Reset();
      h2XZ->Reset();
      h2YZ->Reset();
      htime->Reset();
      gXZ->Set(0);
      if(px) delete px;
      h_range->Reset();
      h_range_X->Reset();
      timeVertex=0;
      total_charge=0;
      mean_mult=0;
      for(Int_t k=0;k<numberOfColumns;k++)
	multiplicity[k]=0;
      padSignalCA->Clear();
      simpleTrackCA->Clear();
      silHitsCA->Clear();
      parisHitsCA->Clear();
      nb+=T->GetEvent(jentry);
      nb+=simTree->GetEvent(jentry);

      Int_t nbsilicon= silHitsCA->GetEntries();
      if(draw){
	//gSystem->Exec("clear");
	cout<<" SILICON "<<nbsilicon<<endl;
      }

      Int_t nbprimaries=primaryInfoCA->GetEntries();
      if(draw){
	//gSystem->Exec("clear");
	cout<<" PRIMARIES "<<nbprimaries<<endl;
      }

      
      if(jentry%2==0){
	E_vertex=beamInfo->GetEnergyVertex();
	if(draw){
	  cout<<"Energy Beam "<<beamInfo->GetEnergyEntrance()<<endl;
	  cout<<"Energy At vertex "<<beamInfo->GetEnergyVertex()<<endl;
	  cout<<"Z vertex "<<beamInfo->GetZVertex()<<endl;
      }
      }
      if(jentry%2!=0){
      thePrimaryInfo=(ActarSimPrimaryInfo*)primaryInfoCA->At(1);
      Double_t Px=thePrimaryInfo->GetPx();
      Double_t Py=thePrimaryInfo->GetPy();
      Double_t Pz=thePrimaryInfo->GetPz();
      Double_t mag_P=sqrt(Px*Px+Py*Py+Pz*Pz);
      Double_t theta_scat=TMath::ACos(Pz/mag_P);
      Double_t X0=thePrimaryInfo->GetX0();
      Double_t Y0=thePrimaryInfo->GetY0();
      Double_t Z0=thePrimaryInfo->GetZ0();
      Double_t theta_real=TMath::ATan2(Px,Pz);
      //Double_t phi_real=TMath::ATan2(Py,sqrt(Px*Px+Pz*Pz));
      Double_t phi_real=TMath::ATan(Py/sqrt(Px*Px+Pz*Pz));
      Double_t scattered_real=0;
      //scattered_real =TMath::ATan(TMath::Tan(theta_real)/TMath::Cos(phi_real));
      scattered_real =TMath::ACos(TMath::Cos(theta_real)*TMath::Cos(phi_real));
      energy=thePrimaryInfo->GetKineticEnergy();
      theta=theta_real;
      phi=phi_real;
      theta3D_real=scattered_real;
      if(draw){
	cout<<"=================================================="<<endl;
	cout<<X0<<" "<<Y0<<" "<<Z0<<endl;
	cout<<Px<<" "<<Py<<" "<<Pz<<endl;
	cout<<"Proton Energy "<<energy<<" MeV"<<endl;
	cout<<"Horizontal Angle "<<theta_real*deg<<endl;
	cout<<"Vertical Angle "<<phi_real*deg<<endl;
	cout<<"Scatter Angle "<< scattered_real*deg<<" "<<theta_scat*deg<<endl;
      }
      //Getting PARIS info
      Int_t nbparishits=parisHitsCA->GetEntries();
      //cout<<"Event "<<jentry<<"-># Hits in Paris "<<nbparishits<<endl;
      if(nbparishits>0){
	for(Int_t h=0;h<nbparishits;h++){
	  parisHit=(ActarSimParisHit*)parisHitsCA->At(h);
	  ClusterID=parisHit->GetMotherID();
	  DetectorID=parisHit->GetDetectorID();
	  DetectorName=parisHit->GetDetectorName();
	  EnergyHit=parisHit->GetEnergy();
	  //4.61% at 662 keV R=a/sqrt(E)+b, a=0.0379131 b=-0.000497199  
	  Double_t resolution=(0.0379131*sqrt(EnergyHit)-0.000497199)/2.35;
	  EnergyHit=gRandom->Gaus(EnergyHit,resolution);
	  totalEnergy+=EnergyHit;
	  if(DetectorName=="LaBr")
	    EnergyCrystal_LaBr[ClusterID][DetectorID]+=EnergyHit;
	  else
	    EnergyCrystal_NaI[ClusterID][DetectorID]+=EnergyHit;
      
	}  
      }

    //Getting the Silicon Hits
      Bool_t protonHit=0;
      Energy_in_silicon=0;
      if(draw)
      cout<<"ProtonHit-->"<<protonHit<<endl;
      Int_t nbsiliconhits=silHitsCA->GetEntries();
      //if(draw)
	cout<<"SilHits->"<<nbsiliconhits<<endl
      if(nbsiliconhits>0){
       	for(Int_t h=0;h<nbsiliconhits;h++){
       	  silHit=(ActarSimSilHit*)silHitsCA->At(h);
	  //cout<< silHit->GetParticleID()<<endl;
       	  if(silHit->GetParticleCharge()==1){
       	    protonHit=kTRUE;
	    EnergySil=silHit->GetEnergy();
	    Energy_in_silicon=gRandom->Gaus(EnergySil,0.008*EnergySil);
	    detectorID=silHit->GetDetectorID();
	    SilPosX=silHit->GetXPos();
	    SilPosY=silHit->GetYPos();
	    SilPosZ=silHit->GetXPos();
	    if(draw){
	      cout<<"Detector "<< detectorID<<" hit Energy "<<EnergySil<<" Energy Before "<<silHit->GetEBeforeSil()
		  <<" Energy After "<<silHit->GetEAfterSil()<<endl;}
       	    //break;
       	  }
       	}
      }
      if(protonHit==1){
	if(draw)
	  //cout<<"A Proton Hit the silicon!!!!! Event Skipped"<<endl;
	  cout<<"A Proton Hit the silicon!"<<endl;
	//continue;
      }
      } 
      //Getting the simulated track     
      Int_t nbOfStrides=simpleTrackCA->GetEntries();
      Energy_in_gas=0;
      if(nbOfStrides>0){
	for(Int_t m=0;m<nbOfStrides;m++){
	  simpleTrack=(ActarSimSimpleTrack *)simpleTrackCA->At(m);
	  h3->Fill(simpleTrack->GetZPre(),simpleTrack->GetXPre(),simpleTrack->GetYPre());
	  gXZ->SetPoint(m,simpleTrack->GetZPre(),simpleTrack->GetXPre());
	  h2XZ->Fill((simpleTrack->GetZPre()+simpleTrack->GetZPost())/2.,(simpleTrack->GetXPre()+simpleTrack->GetXPost())/2.,1000*simpleTrack->GetEnergyStride());
	  h2XZbis->Fill((simpleTrack->GetZPre()+simpleTrack->GetZPost())/2.,(simpleTrack->GetXPre()+simpleTrack->GetXPost())/2.,1000*simpleTrack->GetEnergyStride());
	  Int_t row=thePadsGeometry.GetPadRowFromXZValue((simpleTrack->GetXPre()+simpleTrack->GetXPost())/2.,(simpleTrack->GetZPre()+simpleTrack->GetZPost())/2.);
	  Int_t column=thePadsGeometry.GetPadColumnFromXZValue((simpleTrack->GetXPre()+simpleTrack->GetXPost())/2.,(simpleTrack->GetZPre()+simpleTrack->GetZPost())/2.);
	  //padCharge2[row-1][column-1]=1000*simpleTrack->GetEnergyStride();
	  if(simpleTrack->GetParticleCharge()==1)
	    Energy_in_gas+=1000*simpleTrack->GetEnergyStride();
	  h2YZ->Fill(simpleTrack->GetZPre(),simpleTrack->GetYPre());
	  h2XY->Fill(simpleTrack->GetXPre(),simpleTrack->GetYPre());
	}
	if(draw)
	  cout<<"Total energy deposit in gas by protons "<<Energy_in_gas<<" keV"<<endl;
      }


      Int_t numberofpads=padSignalCA->GetEntries();
      if(draw)
	cout<<"# Pads fired "<< numberofpads<<endl;
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
	  Int_t PadRow=padSignal->GetPadRow();
	  Int_t PadColumn=padSignal->GetPadColumn();
	  //cout<<k<<" "<<PadColumn<<" "<<PadRow<<" "<<thisCharge<<" "<<thisTime<<endl;
	  if(thisCharge>=maxenergydeposit){
	    maxenergydeposit=thisCharge;
	    maxenergytime=thisTime;
	    maxRow=PadRow;
	  }
	  htime->Fill(thisTime);
	  
	  if (thisCharge!=0){
	    padCharge[padSignal->GetPadRow()-1][padSignal->GetPadColumn()-1]
	      += thisCharge;
	    padTime[padSignal->GetPadRow()-1][padSignal->GetPadColumn()-1]
	      += thisCharge*thisTime;
	  }


	  total_charge+=thisCharge;
	  
	}//Loop on ActarPadSignals
	


	if(draw){
	cout<<"---------------------------------------------------------------------------"<<endl;
	}
	threshold = threshold/dynamicRange;
	
	Float_t charge;	
	Int_t nmul=0;
	Float_t r=0;
	Float_t max_charge_column=0;
	Float_t max_time_column=0;
	Int_t max_row=0;
	Float_t max_charge_row=0;
	Float_t max_time_row=0;
	Int_t max_column=0;
	Int_t pointindex=0;
	Int_t pointindex_X=0;

	Int_t TotalPadMultiplicity=0,LeftPadMultiplicity=0,RightPadMultiplicity=0,BeamPadMultiplicity=0;
	Int_t side=0; //side=0 beam, side=1 left, side=2 right

	for(Int_t l=0;l<numberOfRows;l++){
	  for(Int_t m=0;m<numberOfColumns;m++){
	    if(padCharge[l][m] >threshold){
	      TotalPadMultiplicity++;
	      if(l<numberOfRows/2-beamWidth-1)
		RightPadMultiplicity++;
	      else if(l>numberOfRows/2+beamWidth+1)
		LeftPadMultiplicity++;
	      else
		BeamPadMultiplicity++;		
	    }
	  }
	}

	Bool_t GOOD_EVENT=1;
	for(Int_t m=0;m<numberOfColumns;m++){
	  if(padCharge[0][m]!=0||padCharge[numberOfRows-1][m]!=0){
	    GOOD_EVENT=kFALSE;
	    break;
	  }
	}
	if(jentry%2!=0){
	for(Int_t l=0;l<numberOfRows;l++){
	  if(padCharge[l][0]!=0||padCharge[l][numberOfColumns-1]!=0){
	    GOOD_EVENT=kFALSE;
	    break;
	  }
	}
	}
	if(GOOD_EVENT==0){
	  if(draw)
	    cout<<"Event "<<jentry<<" Skipped!!"<<endl;
	  //continue;
	}

	if(draw)
	cout<<"# Multiplicities TOTAL "<<TotalPadMultiplicity<<" LEFT "<<LeftPadMultiplicity<<" RIGHT "<<RightPadMultiplicity<<" BEAM "<<BeamPadMultiplicity<<endl;

	if(LeftPadMultiplicity>RightPadMultiplicity) side=1;
	else if(RightPadMultiplicity>LeftPadMultiplicity) side=2;
	else side=0;

	if(draw)
	cout<<"SIDE "<<side<<endl;

      Rmin=0; 
      Rmax=0;
      
      if(side==1||side==2){
	
	if(side==1){
	  Rmin = (Int_t) numberOfRows/2+beamWidth;      // we leave 4 rows for beam
	  Rmax = numberOfRows-marginWidth;                // we leave 2 rows for the margin
	  if(Rmin>Rmax){
	    Int_t tmp = Rmax;
	    Rmax = Rmin;
	    Rmin = tmp;
	  }
	  
	}
	else if(side==2){
	  Rmin = marginWidth;
	  Rmax = (Int_t) numberOfRows/2-beamWidth;
	  if(Rmin>Rmax){
	    Int_t tmp = Rmax;
	    Rmax = Rmin;
	    Rmin = tmp;
	  }
	}	
      }
      else{
	if(jentry%2!=0)
	  continue;
	Rmin = (Int_t) numberOfRows/2-beamWidth;
	Rmax = (Int_t) numberOfRows/2+beamWidth;
	if(Rmin>Rmax){
	  Int_t tmp = Rmax;
	  Rmax = Rmin;
	  Rmin = tmp;
	  
	}
      }
      // arguments for function FitMat
      Cmin = marginWidth;
      Cmax = numberOfColumns-marginWidth;
      if(side==0){     
      Rmin=72; 
       Rmax=76;
      }
      //Cmax= numberOfColumns/2;
      a=0;
      b=0;
      Double_t a2=0,b2=0;

      Double_t angle_proj=0;
    if(draw)
      cout<<"Horizontal "<< Rmin<<" "<<Rmax<<" "<<Cmin<<" "<<Cmax<<endl;
      FitMat2(padCharge,padSize,sideBlankSpace,xLength,Rmin,Rmax,Cmin,Cmax,threshold,a2,b2);
      //cout<<"a2 "<<a2<<" b2 "<<b2<<endl;
      if(side==1){
	if(a2>0)
	  angle_proj=TMath::ATan(1/a2);
	else if(a2<0)
	  angle_proj=TMath::Pi()+TMath::ATan(1/a2);
	if(draw)
	  cout<<"a "<<a2<<" b "<<b2<<" Vertex Position "<<b2<<endl;
      }
      else if(side==2){
	if(a2>0)
	  angle_proj=TMath::Pi()+TMath::ATan(1/a2);
	else if(a2<0)
	  angle_proj=TMath::ATan(1/a2);
	if(draw)
 	  cout<<"a "<<a2<<" b "<<b2<<" Vertex Position "<<b2<<endl;
      }
      else{
	if(draw)
	  cout<<"a "<<a2<<" b "<<b2<<endl;
	if(a2!=0)
	  angle_proj=TMath::ATan(1/a2);
      }
      angleH=angle_proj*deg;
      
      Double_t tan_theta2D=0;
      if(a2!=0)
	tan_theta2D=1./a2;	  
      
      if(draw){
	cout<<"ANGLE Proj "<<(angle_proj*deg)<<endl;
      }

         
	for(Int_t m=0;m<numberOfColumns;m++){ //LOOP on Columns
	  charge=0;
	  max_charge_column=0;
	  max_time_column=0;
	  max_row=0;
	  for(Int_t l=0;l<numberOfRows;l++){
	    Double_t xPad=0.,zPad=0.,yPad=0.;
	    if(padCharge[l][m] >threshold){
	      multiplicity[m]+=1;
	      padTime[l][m]=padTime[l][m]/padCharge[l][m];
	      padTime[l][m]=gRandom->Gaus(padTime[l][m],sigma_time);
	      
	      xPad = -xLength + (2*l+1)*padSize/2.;
	      zPad = (2*m+1)*padSize/2.;
	      yPad= -yLength + driftVelocity*padTime[l][m];
	      
	      Float_t r=sqrt(xPad*xPad+zPad*zPad);
	      r=sqrt(zPad*zPad);
	      h2->Fill(zPad,xPad,padCharge[l][m]);
	      if(jentry%2==0)
		h_beam->Fill(zPad,xPad,padCharge[l][m]);
	      //h2->Fill(xPad,zPad,padCharge[l][m]);
	      h2t->Fill(zPad,xPad,padTime[l][m]);
	      h2t_z->Fill(zPad,padTime[l][m],padCharge[l][m]);
	      h2t_x->Fill(xPad,padTime[l][m],padCharge[l][m]);
	      h2y->Fill(zPad,yPad,padTime[l][m]);
	      if(padCharge[l][m]>max_charge_column){
		max_charge_column=padCharge[l][m];
		max_time_column=padTime[l][m];
		max_row=l;
		//zPad = (2*m+1)*padSize/2.;
	      }
	      if((l<(Int_t) numberOfRows/2-beamWidth||l>(Int_t) numberOfRows/2+beamWidth))
		charge+=padCharge[l][m];	      
	    }
	    else{ 
	      padCharge[l][m]=-1;
	      padTime[l][m]=-1;
	    }


	  }
	  zPad = (2*m+1)*padSize/2.;
	
	  if(max_time_column>0){
	    if((l<(Int_t) numberOfRows/2-beamWidth||l>(Int_t) numberOfRows/2+beamWidth))
	      gtime->SetPoint(pointindex,zPad,max_time_column);
	    pointindex++;
	  }
	
	if(multiplicity[m]!=0){
	  nmul++;
	  mean_mult+=multiplicity[m];
	  //cout<<"multiplicity in row "<<m<<" "<<multiplicity[m]<<endl;
	  }
	  h_range->Fill(r,charge);
	  
	  //prev_column=m;
	  
	}
	//End of Loop on pads


	for(Int_t l=0;l<numberOfRows;l++){
	  charge=0;
	  max_charge_row=0;
	  max_time_row=0;
	  max_column=0;
	  for(Int_t m=0;m<numberOfColumns;m++){ //Rows
	    Double_t xPad=0.,zPad=0.,yPad=0.;
	    if(padCharge[l][m] >threshold){
	      multiplicity[m]+=1;

	      if(padCharge[l][m]>max_charge_row){
		max_charge_row=padCharge[l][m];
		max_time_row=padTime[l][m];
		max_column=m;
		//zPad = (2*m+1)*padSize/2.;
	      }	      
	      xPad = -xLength + (2*l+1)*padSize/2.;
	      zPad = (2*m+1)*padSize/2.;
	      yPad= -yLength + driftVelocity*padTime[l][m];

	      
	      //Float_t r=sqrt(xPad*xPad+zPad*zPad);
	      r=sqrt(xPad*xPad);
	      //r=xPad;
	      
	      if(l<(Int_t) numberOfRows/2-beamWidth||l>(Int_t) numberOfRows/2+beamWidth)
		charge+=padCharge[l][m];	      
	    }
	    
	  }


	  xPad =-xLength+(2*l+1)*padSize/2.;
	  
 	  if(max_time_row>0){	
	    if((l<(Int_t) numberOfRows/2-beamWidth||l>(Int_t) numberOfRows/2+beamWidth))
	      gtime_X->SetPoint(pointindex_X,xPad,max_time_row);
	    pointindex_X++;
	  }
	  
	  if(multiplicity[m]!=0){
	    nmul++;
	    mean_mult+=multiplicity[m];
	    //cout<<"multiplicity in row "<<m<<" "<<multiplicity[m]<<endl;
	  }
	  h_range_X->Fill(r,charge);
	  //prev_column=m;
	  
	}//End of Loop on pads




	mean_mult= (Float_t)mean_mult/(Float_t)nmul;
	suma+=mean_mult;
	//cout<<"Mean Multiplicity="<< mean_mult<<endl;
      }




      Double_t G4Range=0;
      Int_t track_L=50;
      //Int_t track_L_real=track_L+10;
      Int_t track_L_real=185;
      if(draw&&protonHit){
	cout<<"charge deposited in gas "<<h_range->Integral()<<" "<<h_range_X->Integral()<<endl;
	cout<<"Energy deposited in gas "<<h_range->Integral()/1e6*30<<" keV "<<h_range_X->Integral()/1e6*30<<endl;
	cout<<energy*1000<<" Total Energy deposited "<<h_range->Integral()/1e6*30+EnergySil*1000<<" keV "<<h_range_X->Integral()/1e6*30+EnergySil*1000<<endl;

      }
      track_length=track_L;
      
      if(jentry%2!=0)
	h2->Add(h_beam);
       brag=new TSpline3(h_range);
       bragX=new TSpline3(h_range_X);
       
       Float_t max,xmax;
       Float_t max2,xmax2;
       //if(h_range->GetIntegral())
       //FindMax(brag,&max,&xmax);
       //cout<<"Maximum is at--> "<<xmax<<" "<<max<<endl;
       Float_t range_calc;
       Float_t range_calcX;
       // if((angleH>45&&angleH<135)||(angleH>225&&angleH<315)){
       //FindMax(bragX,&max2,&xmax2);
       max2=h_range_X->GetMaximum();
       xmax2=h_range_X->GetBinCenter(h_range_X->GetMaximumBin());
       if(draw)
       cout<<"Maximum "<< max2<<" X maximum "<<xmax2 <<endl;	 
       FindRangeX(bragX,max2,xmax2,&range_calcX);
       
       // }
       // else{
       //FindMax(brag,&max,&xmax);
       max=h_range->GetMaximum();
       xmax=h_range->GetBinCenter(h_range->GetMaximumBin());	 
       if(draw)
	 cout<<"Maximum "<< max<<" Z maximum "<<xmax<<" Bin "<<h_range->GetMaximumBin()<<endl;	 
       FindRange(brag,max,xmax,tan_theta2D,side,&range_calc);
	 //}
       if(draw)
       cout<<"Calculated range position--> "<<range_calc<<endl;
       Int_t bin=h_range->GetXaxis()->FindBin(range_calc);
       rangeProj=fabs(range_calc-b2);
       rangeZ=rangeProj;

       //2D Ranges
       range2D=fabs(rangeProj/TMath::Cos(angle_proj));
       if(angle_proj !=0)
       range2D_X=fabs(range_calcX/TMath::Sin(angle_proj));

       Double_t range2DX=fabs(range_calcX/TMath::Sin(angle_proj));
       if(range2D>0&&(angle_proj*deg)>10){
	 graph_anglevsrange->SetPoint(point_graph,angle_proj*deg,range2D);
	 graph_anglevsrangeX->SetPoint(point_graph,angle_proj*deg,range2DX);
	 point_graph++;
       }

       if(draw){
	 cout<<"Maximum is at--> "<<xmax<<" "<<max<<endl;
 	 cout<<"Position range is at--> "<<range_calc<<" "<<rangeProj<<endl;
 	 cout<<"Position rangeX is at--> "<<range_calcX<<" "<<fabs(range_calcX)<<endl;

	 cout<<"Range 2D "<<fabs((rangeProj)/TMath::Cos(angle_proj))<<endl;
	 cout<<"Range 2D X "<<fabs(range_calcX/TMath::Sin(angle_proj))<<endl;
       }

      //Int_t nbOfStrides=simpleTrackCA->GetEntries();
      //G4Range=0;
      
      h2XZ->SetMarkerColor(1);
      h2XZ->SetMarkerStyle(7);
      h2YZ->SetMarkerStyle(7);
      h2YZ->SetMarkerColor(1);
     
     
      if(draw&&numberofpads>0){
	c->cd(1);
	h2->Draw("colz");
	//if(jentry%2!=0)
	  //h_beam->Draw("samecolz");
	//h2XZ->Draw("colz");
	//h2XZbis->Draw("same");
	//h2->Write();
	//gXZ->Draw("*");
	c->cd(2);
	//h2t->Draw("colz");
	gtime->Draw("AP");
	gtime_X->Draw("AP");
	//h2t->Draw("same");
	//if(jentry%2==0)
	  h_range->Draw();
	// else{
	//   c->cd(2)->DrawFrame(0,0,300,Maximum_histo+1000);
	//   histo_pos->Draw("same");
	//   histo_neg->Draw("same");
	// }
	brag->SetLineColor(3);
	brag->SetLineWidth(2);
	brag->Draw("same");
	
	
	c->cd(3);
	h_range_X->Draw();
	bragX->SetLineColor(3);
	bragX->SetLineWidth(2);
	bragX->Draw("same");
	//gtime->Draw("AP");
	//gXZ->Draw("");
	//simfit->SetRange(0,200);
	//simfit->SetLineColor(3);
	//simfit2->SetRange(0,200);
	//simfit2->SetLineColor(6);
	//simfit->Draw("same");
	//simfit2->Draw("same");
	//gXZ->Draw("*");
	//h2XZ->Draw("same");
	c->cd(4);
	//h2y->Draw("colz");
	//h2YZ->Draw("same");
	//funcion->Draw("same");
	//h2t->Draw("colz");
	gtime->Draw("AP");
	gtime_X->Draw("AP");
	//h2t_x->Draw("colz");
	//h2t_z->Draw("colz");
	c->Update();
	//c->cd(1);
	// c2->cd();
	// TH2F *histogram=(TH2F*)h2->Clone("trace");
	// TGraph *graph=(TGraph*)gXZ->Clone("sim_trace");
	// histogram->SetTitle("");
	// histogram->SetXTitle("Z [mm]");
	// histogram->SetYTitle("X [mm]");
	// histogram->GetYaxis()->SetRangeUser(-20,20);
	// histogram->GetXaxis()->SetRangeUser(0,200);
	// histogram->Draw("colz");
	// graph->SetMarkerColor(1);
	// graph->SetMarkerStyle(20);
	// graph->Draw("P");
	c->cd(4);
	//c->WaitPrimitive();
      }

      //Rmin=0; 
      //Rmax=0;
     Double_t totalChargeLeft=0., totalChargeRight=0.; // left: x>0, right: x<0
    // for(Int_t j=marginWidth; j< (numberOfRows/2-beamWidth); j++){
    //   for (Int_t k=marginWidth; k< numberOfColumns-marginWidth; k++){
    //     // total charge on the right side
    //     totalChargeRight += padCharge[j][k];
    //   }
    // }
    // for(Int_t j=(numberOfRows/2+beamWidth); j< (numberOfRows-marginWidth) ; j++){
    //   for (Int_t k=marginWidth; k<numberOfColumns-marginWidth; k++){
    //     // total charge on the left side
    //     totalChargeLeft += padCharge[j][k];
    //   }
    // }
    // //cout<<"totalCharge "<< totalChargeLeft<<" "<<totalChargeRight<< endl;  
    // if(totalChargeLeft>0. || totalChargeRight>0.){
    //   if(totalChargeLeft > totalChargeRight){ //x>0
    //     Rmin = (Int_t) numberOfRows/2+beamWidth;      // we leave 4 rows for beam
    //     Rmax = numberOfRows-marginWidth;                // we leave 2 rows for the margin
    //     if(Rmin>Rmax){
    //       Int_t tmp = Rmax;
    //       Rmax = Rmin;
    //       Rmin = tmp;
    //     }
    //   }
    //   else if(totalChargeLeft < totalChargeRight){ //x<0
    //     // x<0
    //     Rmin = marginWidth;
    //     Rmax = (Int_t) numberOfRows/2-beamWidth;
    //     if(Rmin>Rmax){
    //       Int_t tmp = Rmax;
    //       Rmax = Rmin;
    //       Rmin = tmp;
    //     }
    //   }

    // }
    // arguments for function FitMat
    Cmin = marginWidth;
    Cmax = numberOfColumns-marginWidth;
 
     Double_t tan_theta2D=0;
    if(a2!=0)
      tan_theta2D=1./a2;
    //else
    //  angle_proj=TMath::Pi()+TMath::ATan(a);      
    if(a2!=0)
    Float_t angle_proj2=0;
   if(a2!=0)
     angle_proj2=TMath::ATan(1/a2);
     h_hangle->Fill(angle_proj*deg);
    Double_t slope=0;
    Double_t offset=0;
    //if(simpleTrack->GetYPre()>-42&&simpleTrack->GetYPre()<-38){
    //FitVertical(padCharge,padTime,padSize,1.,angle_proj,Rmin,Rmax,Cmin,Cmax,0,slope,offset);//}
    if(draw)
      cout<<"Vertical "<< Rmin<<" "<<Rmax<<" "<<Cmin<<" "<<Cmax<<endl;
    //FitVertical(padCharge,padTime,padSize,1.,angle_proj,Rmin,Rmax,Cmin,Cmax,draw,slope,offset);//}
    FitVertical(padCharge,padTime,padSize,1.,angle_proj,Rmin,Rmax,Cmin,Cmax,draw,slope,offset);//}
    Double_t slopeX=0;
    Double_t offsetX=0;
    if(draw)
    c->cd(5);
    FitVertical_X(padCharge,padTime,padSize,1.,xLength,angle_proj,Rmin,Rmax,Cmin,Cmax,draw,slopeX,offsetX);//}
    //else 
    //  continue;
    //FitVertical(padCharge,padTime,padSize,driftVelocity,angle_proj,Rmin,Rmax,Cmin,Cmax,slope,offset);
    //FitVertical(padCharge,padTime,padSize,driftVelocity,angle_proj,Rmin,Rmax,Cmin,Cmax,slope);
    Double_t vangle=TMath::ATan(slope*driftVelocity);
    Double_t cos_phi=TMath::Cos(vangle);
    Double_t vangleX=TMath::ATan(slopeX*driftVelocity);
    Double_t cos_phiX=TMath::Cos(vangleX);
    angle=vangle*deg;
    angleV=vangle*deg;
    angleV_X=vangleX*deg;

    if(tan_theta2D>0){
      //Theta3D=180-TMath::ATan(tan_theta2D/cos_phi)*deg;
      //Theta3DX=180-TMath::ATan(tan_theta2D/cos_phiX)*deg;
      Theta3D=TMath::ACos(TMath::Cos(angle_proj)*cos_phi)*deg;
      Theta3DX=TMath::ACos(TMath::Cos(angle_proj)*cos_phiX)*deg;
    }
    else{
      //Theta3D=180+TMath::ATan(tan_theta2D/cos_phi)*deg;
      //Theta3DX=180+TMath::ATan(tan_theta2D/cos_phiX)*deg;
      Theta3D=TMath::ACos(TMath::Cos(angle_proj)*cos_phi)*deg;
      Theta3DX=TMath::ACos(TMath::Cos(angle_proj)*cos_phiX)*deg;
    }

    angle3D=Theta3D;
    angle3D_X=Theta3DX;
      
    if(draw){
      cout<<" SLOPE and VERTICAL ANGLE "<<slope<<" "<<TMath::ATan(slope*driftVelocity)*deg<<endl;
      cout<<" SLOPE and VERTICAL ANGLE X "<<slopeX<<" "<<TMath::ATan(slopeX*driftVelocity)*deg<<endl;
      cout<<"Range3D is "<<fabs(rangeProj/TMath::Cos(angle_proj)/TMath::Cos(vangle))<<endl;
      cout<<"Range3DX is "<<fabs(rangeProj/TMath::Cos(angle_proj)/TMath::Cos(vangleX))<<endl;
      cout<<"Theta3D is "<< Theta3D<<endl;
      cout<<"Theta3DX is "<< Theta3DX<<endl;
    }

    range3D=fabs(rangeProj/TMath::Cos(angle_proj)/TMath::Cos(vangle));
    range3D_X=fabs(rangeProj/TMath::Cos(angle_proj)/TMath::Cos(vangleX));
    protonEnergy=spline_range->Eval(range3D);
    protonEnergy_X=spline_range->Eval(range2D_X);
    if(draw){
      cout<<"Proton Energy "<<protonEnergy/1000<<" MeV"<<endl;
      cout<<"Proton Energy X "<<protonEnergy_X/1000<<" MeV"<<endl;
    
    }

    //Calculation of the residuals
      Residue_Energy=energy-protonEnergy/1000;
      Residue_Theta2D=360+(theta_real-angle_proj)*deg;
      Residue_Theta3D=scattered_real*deg-Theta3D;
      Residue_Phi=(phi_real-vangle)*deg;

    if(draw&&jentry%2!=0){
      cout<<"Evaluating residuals of reconstructed"<<endl;
      cout<<"Energy-> "<<Residue_Energy<<" "<<energy<<" "<<protonEnergy/1000<<" "<<protonEnergy_X/1000<<endl;
      cout<<"Angle2D-> "<<Residue_Theta2D<<" "<<360+theta_real*deg<<" "<<angle_proj*deg<<endl;
      cout<<"Phi-> "<<Residue_Phi<<" "<<phi_real*deg<<" "<<vangle*deg<<" "<<vangleX*deg<<endl;
      cout<<"Angle3D-> "<<Residue_Theta3D<<" "<<scattered_real*deg<<" "<<Theta3D<<" "<<Theta3DX<<endl;
    }

    //Double_t vangle=TMath::ATan(slope*driftVelocity)/rad;
    h_vangle->Fill(vangle);
    //offset=offset-yLength;
    fit_trackv=new TF1("funv","pol1",0,185./cos(angle_proj));
    fit_trackv->SetParameter(1,slope);
    fit_trackv->SetParameter(0,offset);
    //tracked_positionV=offset+slope*(185/cos(angle_proj));
    tracked_positionV=fit_trackv->Eval(rangeProj/cos(angle_proj))*driftVelocity-yLength; 
    //cout<<185/cos(angle_proj)<<" "<< tracked_positionV<<" "<<tracked_positionV*driftVelocity-yLength<<endl; 
    //cin.get();
    h_verpos->Fill(tracked_positionV);

    Double_t zmin = (2*Cmin+1)*padSize/2.;
    Double_t zmax = (2*Cmax+1)*padSize/2.;
    fit_track=new TF1("fun","pol1",zmin,zmax);
    fit_track_new=new TF1("fun3","pol1",zmin,zmax);
    //fit_track=new TF1("fun","[0]*x+[1]",zmin,zmax);
    //ta[jentry]=b+(-1.+2*gRandom->Rndm());
    //ta[jentry]=b;
    if(a2!=0){
    ta[jentry]=-b2/a2;
    //cout<<"b"<<b<<" "<<ta[jentry]<<endl;
    //tb[jentry]=a;
    tb[jentry]=1/a2;
    }
    //h_inipos->Fill(b);
    //h_inipos->Fill(-b2/a2);
    if(thePrimaryInfo)
    h_inipos->Fill(thePrimaryInfo->GetX0());
   fit_track->SetParameters(ta[jentry],tb[jentry]);
    //fit_track_new->SetParameters(ta[jentry],tb[jentry]);
    //fit_track->SetParameters(0.,tb[jentry]);
    /*if(a){
     TCanvas *can=new TCanvas();
     can->Divide(2,2);
     can->cd(1);
     h2XZ->Draw("");
     fit_track->SetLineColor(3);
     fit_track->SetLineWidth(3);
     fit_track->SetRange(0,185);
     fit_track->Draw("same");
     can->cd(2);
     h2->Draw("colz");
     fit_track->Draw("same");
     can->WaitPrimitive();
     can->cd(3);
     h2YZ->Draw("");
     can->cd(4);
     gtime->Draw("AP");
     delete can;
     }*/
 
    //fit_track->SetParameters(a,b);
    fit_track->SetLineColor(2);    
    fit_track_new->SetLineColor(3);    
    //if(fabs(a)<1e-2)
    //cout << endl << "a=" << a << ", b=" << b << ", Cmin=" << Cmin << ", Cmax=" << Cmax << endl << endl;
    //tracked_position=fit_track->Eval(256);
    //tracked_position=fit_track->Eval(range_calc);
    //tracked_position=fit_track->Eval(185);
    tracked_position=fit_track->Eval(rangeProj);
    
   fin_pos->Fill(tracked_position,tracked_positionV);
    projected_range=TMath::Sqrt(range_calc*range_calc+tracked_position*tracked_position);
    histo2->Fill(projected_range);
    if(draw){
      c->cd(1);
      fit_track->SetLineColor(2);
      fit_track->SetLineWidth(1);
      TF1 *fit_track2=(TF1*)fit_track->Clone("new");
      fit_track2->SetLineColor(1);
      fit_track2->SetRange(0,track_L);
      fit_track->SetRange(0,185);
      fit_track->Draw("same");
      //fit_track2->Draw("same"); 
      c->cd(2);
      //fit_track->Draw("same"); 
      /*TVirtualPad *vp=c->cd(3);
	if(jentry==0){
	  vp->DrawFrame(0,-50,180,50);
	  fit_track->Draw("same"); 
	  fit_track2->Draw("same"); 
	}
	else
	fit_track->Draw("same");*/
	c->cd(4);
	//h_horpos->Draw();	
	c->Update();
	//26Nov
	c->WaitPrimitive();
    }

    // if(!draw && Theta3D>160){
    //   TCanvas *c=new TCanvas();
    //   c->Divide(2,2);
    //   c->cd(1);
    //   h2->Draw("colz");
    //   fit_track->SetLineColor(2);
    //   fit_track->SetLineWidth(1);
    //   fit_track->Draw("same");
    //   gXZ->Draw("*");
    // 	c->cd(2);
    // 	//h2t->Draw("colz");
    // 	gtime->Draw("AP");
    // 	//h2t->Draw("same");
    // 	//if(jentry%2==0)
    // 	  h_range->Draw();
    // 	brag->SetLineColor(3);
    // 	brag->SetLineWidth(2);
    // 	brag->Draw("same");		
    // 	c->cd(3);
    // 	h_range_X->Draw();
    // 	bragX->SetLineColor(3);
    // 	bragX->SetLineWidth(2);
    // 	bragX->Draw("same");
    // 	c->cd(4);
    // 	//h2y->Draw("colz");
    // 	//h2YZ->Draw("same");
    // 	//funcion->Draw("same");
    // 	//h2t->Draw("colz");
    // 	h2t_x->Draw("colz");
    // 	//h2t_z->Draw("colz");
    // 	c->Update();
    // 	c->cd(4);
    // 	c->WaitPrimitive();
    // 	c->Close();
    // }


    //cout<<"Fill_tree"<<jentry<<endl;
    if(jentry%2!=0)
      out_tree->Fill();

    Double_t x1=0., y1=0., z1=0., t1=0.; // point1 (x1,y1,z1), nearer to reaction vertex
    Double_t x2=0., y2=0., z2=0., t2=0.; // point2 (x2,y2,z2),further away from reaction vertex than point 1
                                         // so the particle direction is always (point2-point1)
    Int_t padColumn=0;
    Int_t padRow=0;
    Int_t shiftNumber=0;  
    Int_t Column1, Column2;

    //calculate point1, and point2

    if(totalChargeLeft > totalChargeRight){ //x>0
      shiftNumber=0;
      do{
	padRow = Rmax-shiftNumber;
	x2 = -xLength + (2*padRow+1)*padSize/2.;
	z2 = a*x2+b;
	padColumn = (Int_t) z2/padSize;
	Column2=padColumn;
	//cout<<z2<<"***************>"<<padColumn<<endl;
	shiftNumber++;
      } while((padRow > Rmin)               &&
	      ((padColumn<0)                 ||
	       (padColumn > numberOfColumns) ||
	       (padTime[padRow][padColumn]==0.)));
      
      if(padRow <= Rmin)
	 continue;
      if((padColumn<0) || (padColumn > numberOfColumns)){
	cout << "number of Column out of range for point2, in case of x>0 and square pads" << endl;
	cout << "abandon reaction " << jentry << endl;
	continue;
      }
      else{
	t2 = padTime[padRow][padColumn]-timeVertex;
	y2 = -yLength + driftVelocity * t2;
      }

      padRow = Rmin;
        x1 = -xLength + (2*padRow+1)*padSize/2.;
        z1 = a*x1+b;
        padColumn = (Int_t) z1/padSize;
	Column1=padColumn;
        if((padColumn<0) || (padColumn > numberOfColumns) || (padTime[padRow][padColumn]==0.))
	  continue;
	t1 = padTime[padRow][padColumn]-timeVertex;
        y1 = -yLength + driftVelocity * t1;
    }

    else if(totalChargeLeft < totalChargeRight){ //x<0
      
      shiftNumber=0;
      do{
	padRow = Rmin+shiftNumber;
	x2 = -xLength + (2*padRow+1)*padSize/2.;
	z2 = a*x2+b;
	padColumn = (Int_t) z2/padSize;
	Column2=padColumn;
	shiftNumber++;
      } while((padRow < Rmax)      &&
	      ((padColumn<0)                 ||
	       (padColumn > numberOfColumns) ||
	       (padTime[padRow][padColumn]==0.)));
      
      if(padRow >= Rmax)
	continue;
        if((padColumn<0) || (padColumn > numberOfColumns)){
          cout << "number of Column out of range for point2, in case of x<0 and square pads" << endl;
          cout << "abandon reaction " << jentry << endl;
          continue;
        }
        else{
          t2 = padTime[padRow][padColumn]-timeVertex;
          y2 = -yLength + driftVelocity * t2;
        }
        padRow = Rmax;
        x1 = -xLength + (2*padRow+1)*padSize/2.;
        z1 = a*x1+b;
        padColumn = (Int_t) z1/padSize;
	Column1=padColumn;
        if((padColumn<0) || (padColumn >= numberOfColumns) || (padTime[padRow][padColumn]==0.))
          continue;
	t1 = padTime[padRow][padColumn]-timeVertex;
        y1 = -yLength + driftVelocity * t1;
    }
    //   if(z2>2*zLength)
    //  continue;
    //protonDirection.SetX(x2-x1);
    //protonDirection.SetY(y2-y1);
    //protonDirection.SetZ(z2-z1);

    //finalPoint.SetX(x2);
    //finalPoint.SetY(y2);
    //finalPoint.SetZ(rangeProj);
    Float_t angle_proj_trace=TMath::ATan(a);
    if(thePrimaryInfo)
    range     = sqrt((x2-thePrimaryInfo->GetX0())*(x2-thePrimaryInfo->GetX0())+(y2-thePrimaryInfo->GetY0())*(y2-thePrimaryInfo->GetY0())+(z2-thePrimaryInfo->GetZ0())*(z2-thePrimaryInfo->GetX0()));
    //rangeProj = sqrt((x2-0.)*(x2-0.)+                (z2-b)*(z2-b));
    //angle=90-angle_proj_trace*deg;
    //phi=TMath::ATan((t2-t1)*driftVelocity/(Column2-Column1)/2)*deg;
    //t2= padTime[63][90];
    //t1= padTime[Rmin][5];
    //phi=TMath::ATan(slope*driftVelocity)*deg;
    //cout<<"PHI "<<phi<<endl;
    //phi=TMath::ATan((t2-t1)*driftVelocity/(z2-z1))*deg;
    DeltaT=t2-t1;
    //if(DeltaT<-10000)
    //cout<<t2<<" "<<t1<<" "<<t2-t1<<" "<<z2<<" "<<z1<<" "<<Column2<<" "<<Column1<<endl;
    //cout<<"********************************************************************************"<<endl;
    // tracked_positionV=185*TMath::Tan(phi/deg);    
    // h_verpos->Fill(tracked_positionV);
    //tracked_positionV=fit_trackv->Eval(185);    
    //h_verpos->Fill(tracked_positionV);
    //cout<<"tracked_positionV--->"<<tracked_positionV<<endl;
    
    //fit_trackv->SetParameters(0,TMath::Tan(phi/deg));
    //cout<<"---------------------------------------------------------------------------------------"<<endl;
    //cout<<"PROTON TRACKED POSITION "<<x1<<" "<<y1<<" "<<z1<<" "<<endl;
    //cout<<"PROTON TRACKED POSITION "<<x2<<" "<<y2<<" "<<z2<<" "<<endl;
    //cout<<"PROTON RANGE "<<range<<" "<<rangeProj<<endl;
    //cout<<"***************************************************************************************"<<endl;
    //cout<<"Fill_tree"<<jentry<<endl;
    //out_tree->Fill();
    if(draw){
      c->cd(4);
      //fit_trackv->Draw();
      //c->WaitPrimitive();
    }
    //if(a>-1e9&&a<1e9)
    //if(phi)
    //<<endl;
    //graph_anglevsrange->Draw("A*");
    //graph_anglevsrangeX->Draw("*");
    //canvas->Update();
    }//end of loop
    
    


    //*********************************************************************************************************//
    //*********************************************************************************************************//
    //**                                                                                                     **//
    //**                                       End of Event Loop                                             **//
    //**                                                                                                     **//
    //*********************************************************************************************************//
    //*********************************************************************************************************//


    suma=suma/nentries;
    cout<<endl;
    //cout<<"Mean Multiplicity of run="<<suma<<endl;


    delete h2;
    delete h_beam;
    delete h2t;
    delete h_range;
    for (Int_t j=0; j<numberOfRows; j++ ){  
      ;
      //cout<<j<<" Mean Multiplicity of run="<<suma<<endl;
      //delete [](padCharge[j]);
      //delete [](padTime[j]);
    }
    delete [] padCharge;
    delete [] padTime;
    delete [] ta;
    delete [] tb;
    out_tree->Write();
    // outfile->Close();
    // delete outfile;
    
}//end of else
  outfile->Close();
  delete outfile;
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
void FitMat(Double_t **PADNET, Double_t padSize, Double_t sideBlankSpace,Double_t xLength,Int_t Rmin,
	    Int_t Rmax, Int_t Cmin, Int_t Cmax, Double_t threshold, Double_t &a, Double_t &b){

  //cout<<"**************************************************************************************"<<endl;
  //cout<<"Rmin "<<Rmin<<"Rmax "<<Rmax<<"Cmin "<<Cmin<<"Cmax "<<Cmax<<endl;

  Int_t    Row, Col;
  Double_t A, B, C, UEV, Q, X, Xg, Y, Yg;
  A=B=C=UEV=Q=X=Y=Xg=Yg=0.;
  
  for(Row=Rmin;Row<=Rmax;Row++){
    for(Col=Cmin;Col<=Cmax;Col++){
      X=(2*Col+1)*padSize/2;
      //X=-xLength+(2*Row+1)*padSize/2;
      //Y=(2*Col+1)*padSize/2;
      Y=-xLength+(2*Row+1)*padSize/2;
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
	//X=-xLength+(2*Row+1)*padSize/2;
      X=(2*Col+1)*padSize/2;
      //Y=(2*Col+1)*padSize/2;
      Y=-xLength+(2*Row+1)*padSize/2;
      //cout<<X<<" "<<Y<<" "<<PADNET[Row][Col]<<endl;
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
