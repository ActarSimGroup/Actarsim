//void dig_alphas(Int_t gas,Char_t *input_file="root_files/simFile.root",Char_t *output_file="root_files/digiData.root",Int_t nevents=0)
void dig_reaction(Char_t *gasfile_name,Char_t *input_file="root_files/simFile.root",Char_t *output_file="root_files/digiData.root",Int_t nevents=0,Bool_t bTree=0)
{
  gSystem->Load("libactar.sl");
  gROOT->ProcessLine(".L digitizationMacro.C++");
  thePadsGeometry.SetGeometryValues(0,0,0,151.,150.,151.,151.,2);
  //thePadsGeometry.SetGeometryValues(0,0,0,151.,150.,151.,151.,8);
  Char_t dummy[256];
  Double_t v_drift,sigma_trans,sigma_long;
  ifstream *gasfile=new ifstream(gasfile_name);
  gasfile->getline(dummy,256);
  cout<<dummy<<endl;
  gasfile->getline(dummy,256);
  cout<<dummy<<endl;
  *gasfile>>dummy>>dummy>>v_drift;
  cout<<dummy<<endl;
  *gasfile>>dummy>>dummy>>sigma_trans;
  cout<<dummy<<endl;
  *gasfile>>dummy>>dummy>>sigma_long;
  cout<<dummy<<endl;

  delete gasfile;
  
  theDriftManager.SetDriftVelocity(v_drift);  // in mm/ns
  theDriftManager.SetDiffusionParameters(sigma_long,sigma_trans); // in mm^2/ns 
  if(!gasfile)
    theDriftManager.SetDriftParameters(15000.,300.,400,"deuterium");
    //theDriftManager.SetDriftParameters(15000.,300.,500,"deuterium");
    //theDriftManager.SetDriftParameters(15000.,300.,1013.25,"deuterium");
    //theDriftManager.SetDriftParameters(5000.,300.,710,"isobutane");
  
  cout<<"Drift Parameters are:"<<endl;  
  cout<<"v_drift---------> "<<theDriftManager.GetDriftVelocity()<<"mm/ns"<<endl;  
  cout<<"D_long----------> "<<theDriftManager.GetLongitudinalDiffusion()<<"mm^2/ns"<<endl;  
  cout<<"D_trans---------> "<<theDriftManager.GetTransversalDiffusion()<<"mm^2/ns"<<endl;  
  //theAmplificationManager.SetIsWireOn();
  theAmplificationManager.SetIsWireOff();
  //theAmplificationManager.SetWireAmplificationParameters(0.05,2.,.25);
  //digitEvents(input_file,output_file,nevents,bTree);
  digitEvents(input_file,output_file,nevents);
}
