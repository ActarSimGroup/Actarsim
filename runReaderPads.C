{
  gSystem->Load("libactar.sl");
  gROOT->ProcessLine(".L readerPads.C+");
  thePadsGeometry.SetGeometryValues("ActarTPC");
  theDriftManager.SetDriftVelocity(50.0e-3);
  theDriftManager.SetDiffusionParameters(5.e-4, 5.e-4);
  reader("root_files/sim_files/full_17Fp_elastic_simple.root","root_files/dig_files/full_17Fp_elastic_simple_digi2.root",0,0);
  guiForPads(0);
}
