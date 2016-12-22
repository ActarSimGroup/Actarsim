// - AUTHOR: Hector Alvarez-Pol 01/2006
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimBeamInfo_h
#define ActarSimBeamInfo_h 1

#include "TROOT.h"  //for including Rtypes.h

#include "TObject.h"

class ActarSimBeamInfo : public TObject {
private:
  Double_t energyEntrance;     ///< Energy at the gas chamber entrance
  Double_t energyVertex;       ///< Energy at the reaction vertex

  Double_t thetaEntrance;      ///< Theta emission angle at entrance
  Double_t phiEntrance;        ///< Phi emission angle at entrance
  Double_t thetaVertex;        ///< Theta vertex angle at the reaction vertex
  Double_t phiVertex;          ///< Phi vertex angle at the reaction vertex

  Double_t xEntrance;          ///< X beam at entrance position
  Double_t yEntrance;          ///< Y beam at entrance position
  Double_t zEntrance;          ///< Z beam at entrance position

  Double_t xVertex;            ///< X beam at reaction vertex position
  Double_t yVertex;            ///< Y beam at reaction vertex position
  Double_t zVertex;            ///< Z beam at reaction vertex position

  Double_t nextZVertex;        ///< Z beam for next reaction

  Double_t timeVertex;         ///< Time at reaction vertex postion

  Double_t mass;               ///< Mass of beam
  Double_t charge;             ///< Charge of beam

  Int_t eventID;               ///< EventID
  Int_t runID;                 ///< RunID

  /// Informs of ion beam status. Used for the dynamical vertex generation
  /// in ActarSimPrimaryGeneratorAction.
  /// Values are 0: ion ready for being tracked. Default value after object creation.
  ///         1: ion under tracking.
  ///        2: ion reached Z0, the vertex position.
  Int_t status;	  //!

public:
  ActarSimBeamInfo();
  ~ActarSimBeamInfo();

  inline Double_t GetEnergyEntrance() const { return energyEntrance; }
  inline Double_t GetEnergyVertex() const { return energyVertex; }

  inline Double_t GetThetaEntrance() const { return thetaEntrance; }
  inline Double_t GetPhiEntrance() const { return phiEntrance; }

  inline Double_t GetThetaVertex() const { return thetaVertex; }
  inline Double_t GetPhiVertex() const { return phiVertex; }

  inline Double_t GetXEntrance() const { return xEntrance; }
  inline Double_t GetYEntrance() const { return yEntrance; }
  inline Double_t GetZEntrance() const { return zEntrance; }

  inline Double_t GetXVertex() const { return xVertex; }
  inline Double_t GetYVertex() const { return yVertex; }
  inline Double_t GetZVertex() const { return zVertex; }

  inline Double_t GetNextZVertex() const { return nextZVertex; }

  inline Double_t GetTimeVertex() const { return timeVertex; }

  inline Double_t GetMass() const { return mass; }
  inline Double_t GetCharge() const { return charge; }
  inline Double_t GetEventID() const { return eventID; }
  inline Double_t GetRunID() const { return runID; }

  inline Int_t GetStatus() const { return status; }

  inline void SetEnergyEntrance(Double_t e) { energyEntrance = e; }
  inline void SetEnergyVertex(Double_t e) { energyVertex = e; }

  inline void SetThetaEntrance(Double_t t) { thetaEntrance = t; }
  inline void SetPhiEntrance(Double_t p) { phiEntrance = p; }

  inline void SetThetaVertex(Double_t t) { thetaVertex = t; }
  inline void SetPhiVertex(Double_t p) { phiVertex = p; }

  inline void SetXEntrance(Double_t x) { xEntrance = x; }
  inline void SetYEntrance(Double_t y) { yEntrance = y; }
  inline void SetZEntrance(Double_t z) { zEntrance = z; }

  inline void SetXVertex(Double_t x) { xVertex = x; }
  inline void SetYVertex(Double_t y) { yVertex = y; }
  inline void SetZVertex(Double_t z) { zVertex = z; }

  inline void SetNextZVertex(Double_t z) { nextZVertex = z; }

  inline void SetTimeVertex(Double_t t) { timeVertex = t; }

  inline void SetMass(Double_t m) { mass = m; }
  inline void SetCharge(Double_t c) { charge = c; }
  inline void SetEventID(UInt_t eID) { eventID = eID; }
  inline void SetRunID(UInt_t rID) { runID = rID; }

  inline void SetStatus(Int_t s) { status = s; }

  void SetPositionEntrance(Double_t ,Double_t ,Double_t );
  void SetAnglesEntrance(Double_t ,Double_t );
  void SetPositionVertex(Double_t ,Double_t ,Double_t );
  void SetAnglesVertex(Double_t ,Double_t );

  void print(void);

  ClassDef(ActarSimBeamInfo,1) //ROOT CINT
};
#endif
