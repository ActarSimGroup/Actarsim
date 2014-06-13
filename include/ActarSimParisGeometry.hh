
#ifndef ActarSimParisGeometry_hh
#define ActarSimParisGeometry_hh 1

#include <G4ThreeVector.hh>

class ActarSimParisGeometry {
public:
    static G4ThreeVector GetClusterPos(int nr); 
    static G4double 	 GetClusterAngle(int nr); 
    static G4ThreeVector GetDetPos(int nb); 
};

#endif
