

#ifndef HodoParametrisation_H
#define HodoParametrisation_H 1

#include "globals.hh"
#include "G4VPVParameterisation.hh"

class G4VPhysicalVolume;
class G4Tubs;

// Dummy declarations to get rid of warnings ...
class G4Trd;
class G4Trap;
class G4Cons;
class G4Orb;
class G4Sphere;
class G4Torus;
class G4Para;
class G4Hype;
class G4Box;
class G4Polycone;
class G4Polyhedra;

class HodoParametrisation : public G4VPVParameterisation
{ 
  public:
    HodoParametrisation(G4int  NodE, G4int Noe1, G4int Noe2,G4double zpos);
    virtual ~HodoParametrisation();
	
    void ComputeTransformation(const G4int copyNo,G4VPhysicalVolume *physVol) const;
	 void ComputeDimensions (G4Box& ,const G4int copyNo,const G4VPhysicalVolume* physVol) const;
    

  private:  // Dummy declarations to get rid of warnings ...

    void ComputeDimensions (G4Trd&,const G4int,const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Trap&,const G4int,const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Cons&,const G4int,const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Orb&,const G4int,const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Sphere&,const G4int,const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Torus&,const G4int,const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Para&,const G4int,const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Hype&,const G4int,const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Polycone&,const G4int,const G4VPhysicalVolume*) const {}
	void ComputeDimensions (G4Tubs&, const G4int,const G4VPhysicalVolume * ) const{}
    void ComputeDimensions (G4Polyhedra&,const G4int,const G4VPhysicalVolume*) const {}
   

  private:
  //The number of the hodo layers
G4int fNoDE; 
G4int fNoE1; 
G4int fNoE2; 
G4double fzpos;
//the position of every layer and the space between the layers
G4double DEHodoPos[13];//position in x-axis
G4double E1HodoPos[16];//position in y-axis
G4double E2HodoPos[13];//position in y-axis

G4double fzspacing[2];
G4double DEwidth1,DEwidth2,E1width1,E1width2,E2width;
G4double DEthick1,DEthick2,E1thick,E2thick;
G4double Length;
};

#endif
