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


void Reducer()
{
  gSystem->Load("libactar.sl");

  Bool_t simuFlag;
  Bool_t gasflag;
  Char_t *simname;
  Char_t *digname;
  Char_t *gasname;
      
  simname="./root_files/sim_files/simFile.root";
  //simname="./root_files/simFile_12C_He4He4_H2He3_12C12C_H2H2_80k.root";

  //Event info;
  TFile *simFile=new TFile(simname);

  //getting the trees
  TTree *simTree=(TTree*)simFile->Get("The_ACTAR_Event_Tree");
  Int_t nentries=simTree->GetEntries();
  cout<<"Number of sim event : "<<nentries<<endl;
  //TTree *T=(TTree*)simFile->Get("digiTree");
      
  //ClonesArray to the silicon Hits
  TClonesArray *silHitsCA=new TClonesArray("ActarSimSilHit",200);
  TBranch *branchSilHits=simTree->GetBranch("silHits");
  branchSilHits->SetAddress(&silHitsCA);
  branchSilHits->SetAutoDelete(kTRUE);
  ActarSimSilHit *silHit=new ActarSimSilHit;
      
  //cout<<"digFile to use: ";
  //cin >> digname;

  digname="./root_files/dig_files/digFile.root";
  //digname="./dig_files/digFile_12C_He4He4_H2He3_12C12C_H2H2_80k.root";

  gROOT->ProcessLine(".L digit.h+");
  
  padsGeometry thePadsGeometry;
  thePadsGeometry.SetGeometryValues(37.,85.,69.,2.,5.,5.);//digit+
  //thePadsGeometry.SetGeometryValues(32.,85.,64.,2.,0.,0.);//digit+
  //thePadsGeometry.SetGeometryValues(0,0,0,32.,85.,64.,100.,2);
  driftManager theDriftManager;

  Char_t dummy[256];
  Char_t gas[256];
  Double_t v_drift,sigma_trans,sigma_long;
  ifstream *gasfile=new ifstream(gasname);
  gasfile->getline(gas,256);
  cout<<gas<<endl;
  gasfile->getline(dummy,256);
  cout<<dummy<<endl;
  *gasfile>>dummy>>dummy>>v_drift;
  *gasfile>>dummy>>dummy>>sigma_trans;
  *gasfile>>dummy>>dummy>>sigma_long;

  //theDriftManager.SetDriftParameters(2015.,170.,147.5,gasname);
  //theDriftManager.SetDriftParameters(2015.,170.,980.66,gasname);

  //#######HeiC4H10 (9:1) Gas MagBoltz Values#########
  //theDriftManager.SetDriftVelocity(9.084e-3);
  //theDriftManager.SetDiffusionParameters(2.356e-5,3.105e-5);
  //#######HeiC4H10 (9:1) Gas MagBoltz Values E=2090/17#########
  //theDriftManager.SetDriftVelocity(6.865e-3);
  //theDriftManager.SetDiffusionParameters(2.369e-5,2.892e-5);
  //#######HeiC4H10 (95:5) Gas MagBoltz Values E=2520/17#########
  //theDriftManager.SetDriftVelocity(7.521e-3);
  //theDriftManager.SetDiffusionParameters(2.611e-5,3.796e-5);


  //Magboltz Drift paramters for Deuterium Gas
  //theDriftManager.SetDriftVelocity(4.7e-3);
  //theDriftManager.SetDiffusionParameters(1.146e-5,2.342e-5);

  //Magboltz Drift paramters for HeiC4H10 (9:1) Gas
  theDriftManager.SetDriftVelocity(6.865e-3);
  theDriftManager.SetDiffusionParameters(2.369e-5,2.892e-5);

  cout<<"Drift Parameters are:"<<endl;  
  cout<<"v_drift---------> "<<theDriftManager.GetDriftVelocity()<<"mm/ns"<<endl;  
  cout<<"D_long----------> "<<theDriftManager.GetLongitudinalDiffusion()<<"mm^2/ns"<<endl;  
  cout<<"D_trans---------> "<<theDriftManager.GetTransversalDiffusion()<<"mm^2/ns"<<endl;  

  Double_t padSize  = thePadsGeometry.GetPadSize();
  Double_t xLength  = thePadsGeometry.GetXLength();
  Double_t yLength  = thePadsGeometry.GetYLength();
  Double_t zLength  = thePadsGeometry.GetZLength();
 
  cout<<"X length===> "<<xLength<<endl;
  cout<<"Y length===> "<<yLength<<endl;
  cout<<"Z length===> "<<zLength<<endl;

  const Int_t numberOfRows   = thePadsGeometry.GetNumberOfRows();
  const Int_t numberOfColumns= thePadsGeometry.GetNumberOfColumns();

  cout<<"Number of Rows: "<<numberOfRows<<", number of Columns: "<<numberOfColumns<<endl;

  Double_t driftVelocity = theDriftManager.GetDriftVelocity();

  TFile *digFile=new TFile(digname);
  cout<<"Opening digitization file: "<<digname<<endl;

  TTree *digiTree=(TTree*)digFile->Get("digiTree");
  Int_t dentries=digiTree->GetEntries();
  cout<<"Number of digit event : "<<dentries<<endl;
  //Int_t digentries=digiTree->GetEntries();
  //cout<<"Number of digit event : "<<digentries<<endl;
 
  //ClonesArray to the signal
  TClonesArray *padSignalCA=new TClonesArray("ActarPadSignal",4000);
  digiTree->SetBranchAddress("padSignals",&padSignalCA);
  ActarPadSignal *padSignal=new ActarPadSignal;

  Double_t PI=3.1415926535897932384626433;
  Double_t deg=180./PI;
  Double_t rad=PI/180.;
  //==================================================================================//

   //read the Tree generated by tree1w and fill two histograms
   
   //note that we use "new" to create the TFile and TTree objects !
   //because we want to keep these objects alive when we leave this function.

   //Root file to fill
   //TFile *outfile = new TFile("./root_files/out_files/Output_12C_He4He4_H2He3_12C12C_H2H2_80k.root","RECREATE");
   TFile *outfile = new TFile("./root_files/out_files/Output.root","RECREATE");

   TTree *out_tree = new TTree("out_tree","out_tree");  

   ///////////////////////////////////////////////

   Double_t Energy_in_silicon=0.;
   Double_t EnergySil;
   Int_t detectorID;
   Double_t SilPosX;
   Double_t SilPosY;
   Double_t SilPosZ;

   Double_t Qtot;

   TSpline3 *bragX;
   TSpline3 *bragY;
   TSpline3 *bragZ;

   Int_t Rmin=0, Rmax=31;
   Int_t Cmin=0, Cmax=63;

   Double_t threshold = 0;
   Double_t Tthreshold = 1.;

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

   //TMatrixD *padChargeTest=0;
   //TMatrixD *padTimeTest=0;
   TMatrixD padChargeTest;
   TMatrixD padTimeTest;

   TMatrixD a(32,64);
   padChargeTest.ResizeTo(a);
   padTimeTest.ResizeTo(a);

   TMatrixD *pointerCharge = &padChargeTest;
   TMatrixD *pointerTime = &padTimeTest;

   //Silicon charge
   //Double_t SilCharge[16];
   //Int_t SilID[16];

   TVectorD SilCharge;
   SilCharge.ResizeTo(16);
   TVectorD SilID;
   SilID.ResizeTo(16);

   TVectorD *pointerSilCharge=&SilCharge;
   TVectorD *pointerSilID=&SilID;
   //Double_t *SilChargePointer=&SilCharge;

   //Branching the out tree
   //out_tree->Branch("PadCharge","TMatrixD",&padCharge,64000,0);
   //out_tree->Branch("PadTime","TMatrixD",&padTime,64000,0);
   //out_tree->Branch("SilCharge",&SilCharge,64000,0);
   out_tree->Branch("PadCharge","TMatrixD",&pointerCharge,64000,0);
   out_tree->Branch("PadTime","TMatrixD",&pointerTime,64000,0);
   out_tree->Branch("SilCharge","TVectorD",&pointerSilCharge,64000,0);
   out_tree->Branch("SilID","TVectorD",&pointerSilID,64000,0);
   //out_tree->Branch("SilCharge",&SilCharge,"energy/D");


    //*********************************************************************************************************//
    //*********************************************************************************************************//
    //**                                                                                                     **//
    //**                                        Event Loop                                                   **//
    //**                                                                                                     **//
    //*********************************************************************************************************//
    //*********************************************************************************************************//

   for (Long64_t jentry=0;jentry<nentries;jentry++) {
     //for (Long64_t jentry=5000;jentry<nentries;jentry++) {
     if(jentry%500==0)cout<<jentry<<endl;
     //if(jentry%2==0)cout<<"¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡   NEW (2nd) EVENT : "<<jentry<<"   !!!!!!!!!!!!!!!!!!!!"<<endl;

     if(jentry%2==0)
       {
	 for(Int_t u=0;u<numberOfRows;u++){
	   for(Int_t k=0;k<numberOfColumns;k++){
	     padChargeTest(u,k)=0.;
	     padTimeTest(u,k)=0.;
	   }
	 }
       }

     silHitsCA->Clear();
     simTree->GetEvent(jentry);

     Int_t nbsilicon= silHitsCA->GetEntries();
     //cout<<" SILICON "<<nbsilicon<<endl;

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
	 //if(k%20==0)cout<<"thisSigmaTime: "<<thisSigmaTime<<endl;
	 if (thisCharge!=0){

	   padChargeTest(padSignal->GetPadRow()-1,padSignal->GetPadColumn()-1)+= thisCharge;
	   padTimeTest(padSignal->GetPadRow()-1,padSignal->GetPadColumn()-1)+= thisCharge*thisTime;

	   // padCharge[padSignal->GetPadRow()-1][padSignal->GetPadColumn()-1]
	   //   += thisCharge;
	   // padTime[padSignal->GetPadRow()-1][padSignal->GetPadColumn()-1]
	   //   += thisCharge*thisTime;
	   //if(jentry<4)cout<<"Row "<<padSignal->GetPadRow()-1<<" Col "<<padSignal->GetPadColumn()-1<<" Charge "<<thisCharge<<endl;

	 }
       }//Loop on ActarPadSignals
	 

       if(jentry%2==1)//To have the 2 parts of the reaction
	 {
	   // Loop on rows & columns
	   for(Int_t c=0;c<numberOfColumns;c++){ //LOOP on Columns
	     for(Int_t r=0;r<numberOfRows;r++){ //LOOP on Rows
	       padTimeTest(r,c)=padTimeTest(r,c)/padChargeTest(r,c);
	       //padTime[r][c]=padTime[r][c]/padCharge[r][c];
	       //padTime[r][c]=gRandom->Gaus(padTime[r][c],sigma_time);
	     }
	   }//End of Loop on rows & columns
	 
	 }

     }

     if(jentry%2!=0){
 
       ///////////Getting the Silicon Hits////////////////

       for(Int_t s=0;s<16;s++){//Loop on the 12 MAYA Si & 4 DSSD
	 //SilCharge[s]=0.;
	 SilCharge(s)=0.;
	 SilID(s)=0.;
       }

       Int_t SilHitID;
       Energy_in_silicon=0.;
       Int_t nbsiliconhits=silHitsCA->GetEntries();
       //cout<<"SilHits->"<<nbsiliconhits<<endl;
       if(nbsiliconhits>2)cout<<"SilHits-> "<<nbsiliconhits<<" Event "<<jentry<<endl;

       if(nbsiliconhits>0){

	 for(Int_t h=0;h<nbsiliconhits;h++){

	   SilHitID=0;
	   silHit=(ActarSimSilHit*)silHitsCA->At(h);
	   //cout<<"SilHits->ParticleID: "<< silHit->GetParticleID()<<", SilHits->ParticleMass: "<< silHit->GetParticleMass()<<", SilHits->DetectorID: "<<silHit->GetDetectorID()<<endl;
	   EnergySil=silHit->GetEnergy();
	   Energy_in_silicon=EnergySil;
	   //if(silHit->GetDetectorID()<5)Energy_in_silicon=gRandom->Gaus(EnergySil,0.004*EnergySil);
	   //else if(silHit->GetDetectorID()>4)Energy_in_silicon=gRandom->Gaus(EnergySil,0.00167*EnergySil);
	   //Energy_in_silicon=gRandom->Gaus(EnergySil,0.008*EnergySil);
	   detectorID=silHit->GetDetectorID();
	   SilPosX=silHit->GetXPos();
	   SilPosY=silHit->GetYPos();
	   SilPosZ=silHit->GetXPos();
	   //cout<<"Detector "<< detectorID<<" hit Energy "<<EnergySil<<" Energy Before "<<silHit->GetEBeforeSil()<<" Energy After "<<silHit->GetEAfterSil()<<" Energy in Silicium "<<Energy_in_silicon<<endl;
	   //cout<<"Energy in silicon: "<<Energy_in_silicon<<endl;
	   //SilCharge[detectorID-1]=Energy_in_silicon;  	    
	   SilCharge(detectorID-1)=Energy_in_silicon;  
	   //SilID(detectorID-1)=silHit->GetDetectorID();
	   if(silHit->GetParticleID()==2212)SilID(detectorID-1)=1;//p
	   //if(silHit->GetParticleID()==1000010020)SilID(detectorID-1)=1;//2H
	   else if(silHit->GetParticleID()==1000020030)SilID(detectorID-1)=2;//3He
	   else if(silHit->GetParticleID()==1000020040)SilID(detectorID-1)=3;//4He
	   else if(silHit->GetParticleID()==1000060120)SilID(detectorID-1)=4;//12C
	   else if(silHit->GetParticleID()==1000060130)SilID(detectorID-1)=5;//13C

	   //break;
	   //cout<<" SilFront "<<SilFront<<" SilLeft "<<SilLeft<<" SilRight "<<SilRight<<endl;

	 }
       }

       if(nbsiliconhits!=0)out_tree->Fill();
       //out_tree->Fill();
    
     }

   }

   out_tree->Write();
   outfile->Close();
    //*********************************************************************************************************//
    //*********************************************************************************************************//
    //**                                                                                                     **//
    //**                                       End of Event Loop                                             **//
    //**                                                                                                     **//
    //*********************************************************************************************************//
    //*********************************************************************************************************//


} 
