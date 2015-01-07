/////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol
//*-- Date: 04/2008
//*-- Last Update: 22/12/2014
// --------------------------------------------------------------
// Description:
//   The information from the primaries generated in the reaction
//   vertex. Information to be accessed in the ROOT file per event
//
// --------------------------------------------------------------
// Comments:
//
//
/////////////////////////////////////////////////////////////////

#ifndef ActarSimPrimaryInfo_h
#define ActarSimPrimaryInfo_h 1

#include "TROOT.h"  //for including Rtypes.h

#include "TObject.h" 

class G4PrimaryParticle;

class ActarSimPrimaryInfo : public TObject {
  
private:

  UInt_t nbPrimariesInEvent;   // number of primaries in the event
  Double_t kineticEnergy;      // kinetic energy 
  Double_t theta;              // theta emission angle
  Double_t phi;                // phi emission angle
  
  Int_t PDGcode;               // datamembers copied from G4PrimaryParticle
  Double_t Px;
  Double_t Py;
  Double_t Pz;
  Int_t trackID; 
  Double_t mass;
  Double_t charge;
  Double_t polX;
  Double_t polY;
  Double_t polZ;
  Double_t Weight0;
  Double_t properTime;

  Double_t x0;         //vertex position
  Double_t y0;
  Double_t z0;

  //Event identification  
  UInt_t eventID;     // event ID
  UInt_t runID;       // run ID
  
public:
  
  ActarSimPrimaryInfo();
  ActarSimPrimaryInfo(G4PrimaryParticle*);
  ~ActarSimPrimaryInfo();
  
  inline Int_t GetNbPrimariesInEvent() const { return nbPrimariesInEvent; }
  inline Double_t GetKineticEnergy() const { return kineticEnergy; }
  inline Double_t GetTheta() const { return theta; }
  inline Double_t GetPhi() const { return phi; }
  inline Int_t GetPDGcode() const { return PDGcode; }
  inline Double_t GetPx() const { return Px; }
  inline Double_t GetPy() const { return Py; }
  inline Double_t GetPz() const { return Pz; }
  inline Int_t GetTrackID() const { return trackID; }
  inline Double_t GetMass() const { return mass; }
  inline Double_t GetCharge() const { return charge; }
  inline Double_t GetPolX() const { return polX; }
  inline Double_t GetPolY() const { return polY; }
  inline Double_t GetPolZ() const { return polZ; }
  inline Double_t GetWeight() const { return Weight0; }
  inline Double_t GetProperTime() const { return properTime; }
  inline Double_t GetEventID() const { return eventID; }
  inline Double_t GetRunID() const { return runID; }
  inline Double_t GetX0() const { return x0; }
  inline Double_t GetY0() const { return y0; }
  inline Double_t GetZ0() const { return z0; }


  inline void SetNbPrimariesInEvent(Int_t nb) { nbPrimariesInEvent = nb; }
  inline void SetKineticEnergy(Double_t kin) { kineticEnergy = kin; }
  inline void SetTheta(Double_t t) { theta = t; }
  inline void SetPhi(Double_t p) { phi = p; }
  inline void SetPDGcode(Int_t PDG) { PDGcode = PDG; }
  inline void SetPx(Double_t val) { Px = val; }
  inline void SetPy(Double_t val) { Py = val; }
  inline void SetPz(Double_t val) { Pz = val; }
  inline void SetTrackID(Int_t tr) { trackID = tr; }
  inline void SetMass(Double_t m) { mass = m; }
  inline void SetCharge(Double_t c) { charge = c; }
  inline void SetPolX(Double_t p) { polX = p; }
  inline void SetPolY(Double_t p) { polY = p; }
  inline void SetPolZ(Double_t p) { polZ = p; }
  inline void SetWeight(Double_t w) { Weight0 = w; }
  inline void SetProperTime(Double_t pt) { properTime = pt; }
  inline void SetEventID(UInt_t eID) { eventID = eID; }
  inline void SetRunID(UInt_t rID) { runID = rID; }
  inline void SetX0(UInt_t x) { x0 = x; }
  inline void SetY0(UInt_t y) { y0 = y; }
  inline void SetZ0(UInt_t z) { z0 = z; }

  void SetVertexPosition(Double_t x,Double_t y,Double_t z);
  void SetMomentum(Double_t px,Double_t py,Double_t pz);
  void Set4Momentum(Double_t px,Double_t py,Double_t pz,Double_t E);
  void SetPolarization(Double_t px,Double_t py,Double_t pz);

  void print(void);

  ClassDef(ActarSimPrimaryInfo,1) //ROOT CINT    
};
#endif
