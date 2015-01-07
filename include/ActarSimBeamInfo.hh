/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol
//*-- Date: 06/2008
//*-- Last Update: 1/12/14 by Hector Alvarez
// --------------------------------------------------------------
// Description:
//   The information from the beam parameters used for the reaction
//   vertex. Information to be accessed in the ROOT file per event
//
// --------------------------------------------------------------
// Comments:
//
//     - 12/06/08 Data structure for beam parameters
//
/////////////////////////////////////////////////////////////////

#ifndef ActarSimBeamInfo_h
#define ActarSimBeamInfo_h 1

#include "TROOT.h"  //for including Rtypes.h

#include "TObject.h"

class ActarSimBeamInfo : public TObject {

private:

  Double_t energyEntrance;     // Energy at the entrance
  Double_t energyVertex;       // Energy at the vertex

  Double_t thetaEntrance;      // theta emission angle
  Double_t phiEntrance;        // phi emission angle
  Double_t thetaVertex;        // theta vertex angle
  Double_t phiVertex;          // phi vertex angle

  Double_t xEntrance;          // beam entrance position
  Double_t yEntrance;
  Double_t zEntrance;

  Double_t xVertex;            // reaction vertex position
  Double_t yVertex;
  Double_t zVertex;

  Double_t timeVertex;        // time at vertex

  Int_t status;	//! (Does not move to file!)
                // Informs of ion beam status. Used for the dynamical vertex generation
			          // in ActarSimPrimaryGeneratorAction.
			          // Values: 0 ion ready for being tracked. Default value after object creation
			          //         1 ion under tracking
			          //         2 ion reached Z0, the vertex position

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

  inline Double_t GetTimeVertex() const { return timeVertex; }

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

  inline void SetTimeVertex(Double_t t) { timeVertex = t; }

  inline void SetStatus(Int_t s) { status = s; }

  void SetPositionEntrance(Double_t ,Double_t ,Double_t );
  void SetAnglesEntrance(Double_t ,Double_t );
  void SetPositionVertex(Double_t ,Double_t ,Double_t );
  void SetAnglesVertex(Double_t ,Double_t );

  void print(void);

  ClassDef(ActarSimBeamInfo,1) //ROOT CINT
};
#endif
