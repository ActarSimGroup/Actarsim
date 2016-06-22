// - AUTHOR: Hector Alvarez-Pol 05/2005
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimTrack_h
#define ActarSimTrack_h 1

//#include "G4Types.hh"

#include "TROOT.h"  //for including Rtypes.h

//#ifndef __CINT__
//#include "G4String.hh" //Explicit inclusion for root dictionary
//using namespace std;
//#else
//struct G4String;
//#endif

class ActarSimTrack{
private:
  Double_t xCoord;      ///< X coordinate
  Double_t yCoord;      ///< Y coordinate
  Double_t zCoord;      ///< Z coordinate
  Double_t xPreCoord;   ///< X coordinate at the start of step
  Double_t yPreCoord;   ///< Y coordinate at the start of step
  Double_t zPreCoord;   ///< Z coordinate at the start of step
  Double_t energyStep;  ///< Energy step
  Int_t parentTrackID;  ///< Parent Track ID
  Int_t trackID;        ///< Track ID
  Int_t eventID;        ///< Event ID
  Int_t runID;          ///< Run ID

public:
  ActarSimTrack();
  virtual ~ActarSimTrack();

  Double_t GetXCoord(){return xCoord;}
  Double_t GetYCoord(){return yCoord;}
  Double_t GetZCoord(){return zCoord;}
  Double_t GetXPreCoord(){return xPreCoord;}
  Double_t GetYPreCoord(){return yPreCoord;}
  Double_t GetZPreCoord(){return zPreCoord;}
  Double_t GetEnergyStep(){return energyStep;}
  Int_t GetTrackID(){return trackID;}
  Int_t GetParentTrackID(){return parentTrackID;}
  Int_t GetEventID(){return eventID;}
  Int_t GetRunID(){return runID;}

  void SetXCoord(Double_t xc){xCoord = xc;}
  void SetYCoord(Double_t yc){yCoord = yc;}
  void SetZCoord(Double_t zc){zCoord = zc;}
  void SetXPreCoord(Double_t xc){xPreCoord = xc;}
  void SetYPreCoord(Double_t yc){yPreCoord = yc;}
  void SetZPreCoord(Double_t zc){zPreCoord = zc;}
  void SetEnergyStep(Double_t energy){energyStep = energy;}
  void SetTrackID(Int_t track){trackID = track;}
  void SetParentTrackID(Int_t pt){parentTrackID = pt;}
  void SetEventID(Int_t ev){eventID = ev;}
  void SetRunID(Int_t ev){runID = ev;}

  ClassDef(ActarSimTrack,1) //ROOT CINT
};
#endif
