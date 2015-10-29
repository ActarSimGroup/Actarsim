///////////////////////////////////////////////////////////////////
//*-- AUTHOR : Hector Alvarez-Pol
//*-- Date: 06/2006
//*-- Last Update: 28/10/15
//*-- Copyright: GENP (Univ. Santiago de Compostela)
//
// --------------------------------------------------------------
//
// --------------------------------------------------------------
// Technical details...
//  diffusion: gaussian with sigma = sqrt(2Dx/w)
//  where D is the field dependent diff. coefficient, w the drift
//  velocity and x the distance
//  as described in A. Peisert and F. Sauli CERN84-08 (1984)
//
// --------------------------------------------------------------
// How to run this program:
// 1 - Run the simulation
//      actarsim batch1.mac
// 2 - Open a root session
//      root -l
// 3 - Run this macro inside root
//      gSystem->Load("actarsim.sl");
//      .L digitizationMacro.C+;
//
//      thePadsGeometry.SetGeometryValues(Int_t geometryType,
//                                        Int_t padType,
//                                        Int_t layout,
//        			          Double_t xLength,
//        			          Double_t yLength,
//        			          Double_t zLength,
//        			          Double_t radius,
//                                        Double_t padSize);
//      where all distances are in mm
//
//      theDriftManager.SetDriftVelocity(Double_t velocity);      in mm/ns
//      theDriftManager.SetDiffusionParameters(Double_t long,
//	       				Double_t trans);          in mm^2/ns
//      theDriftManager.SetMagneticField(Double_t mag);    NOT WORKING YET
//      theDriftManager.SetLorentzAngle(Double_t lor);          in radians
//
//      theAmplificationManager.SetIsWireOn();    for a MAYA-like ACtive TARget
//      theAmplificationManager.SetWireAmplificationParameters(ra,s,h);
//
//      ra: radius of amplification wire: 5, 10, and 20 mu
//       s: spacing between two amplification wires: 2 or 2.3 mm
//       h: distance between the amplification wire and induction pads: 10 mm
//
//      (Optionally you can set theAmplificationManager.SetOldChargeCalculation(); for old Style calculations)
//      digitEvents(inputFile, outputFile, run#, numberOfEvents);
//
//  the number within brackets means:
//  the geometryType (0 for a box, 1 for cylinder)
//  the padType (0 for square, 1 for hexagonal)
//  the layout, only effective for hexagonal pads, (0, MAYA-type, 1: rotated 90 degrees)
//  the radius is the radius if the cylinder
//  the xLength is the half-length of the box along x
//  the yLength is the half-length of the box along y
//  the zLength is the half-length of the box along z
//  the padSize is the square or hexagonal pad side
//  the velocity is the drift velocity in the gas
//  the long and trans are the longitudinal and transversal diffusion
//                           coefficients in the gas
//  the mag is the magnetic field inside the gas
//  the inputFile (output of the simulation)
//  the outputFile (output of the digitization)
//  the run  numbers (begin in 0)
///////////////////////////////////////////////////////////////////////
//
// NOTE: it is possible to digitize over the endcaps of a cylinder
//  using the following method:
//
//  1) simulate in GEANT the reaction on a cylinder. Use the typical
//     GEANT4 directions and conventions
//  2) run this digitization macro following the previous instructions,
//  with the following details:
//    a)use a box in when selecting the geometry with xlength and zlength
//      equal to the radius, and yLength equal to the tube length
//    b)call the function
//      thePadsGeometry.SetEndCapModeOn()
//   This will change the data of your track... the direction Z will
//   be converted to Y and Y will be -X. Then, the typical box-like
//   projection on plane XZ will be equivalent to a projection on the
//   end cup of the cylinder.
//

#include <TObject.h>
#include <TVector3.h>
#include "include/ActarSimSimpleTrack.hh"
#include "include/ActarSimSilHit.hh"
#include <TTree.h>
#include <TF2.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <TMath.h>
#include <TClonesArray.h>
#include <TString.h>
#include <TRandom.h>

using namespace std;

Int_t DIGI_DEBUG=1; //A global DEBUG variable:
                    //0 absolutly no output (quiet)
                    //1 minimum output when trouble, status or warnings
                    //2 tracking the functions behavior
                    //3 tracking with increased verbosity
                    //4 full verbosity

class ActarPadSignal;
class projectionOnPadPlane;
class padsGeometry;
class amplificationManager;
class driftManager;

Float_t Polya(Float_t param=3.2){
  //
  // Gain distribution according to a Polya function
  // The first time this function is called, the integral of the Polya function,
  // [ taken from Bellazzini et al NIMA 581 (2007) 246 ]
  // is calculated (with N=1).
  // Returns a random gain according to the gain distribution.
  //
  static Short_t firstcall=0;
  static Float_t integral[1000];

  Int_t check=0;
  Int_t i=0;
  Float_t f,buff[1000];
  Float_t lambda;
  Float_t step=0.01;
  Float_t shift=0.005;
  Float_t pran=0;

  if(firstcall==0){
    for(i=0;i<1000;i++){
      lambda=i*step+shift; //gain: number of electrons produced for a single incoming electron
      buff[i]=pow(param,param)/TMath::Gamma(param)*pow(lambda,param-1)*exp(-param*lambda);
      if(i>0)
        integral[i]=integral[i-1]+buff[i];
      else
	      integral[i]=buff[i];
    }
    firstcall=1;
  }
  while(check==0){
    f=gRandom->Rndm();
    if(f>0.0001 && f<0.9999) check=1;
  }

  i=0;
  while(i<1000){
    if(f<integral[i]/integral[999]){
      pran=i*step+shift;
      break;
    }
    i++;
  }
  return pran;
}


class ActarPadSignal : public TObject {
 private:
  //Basic Pad information
  Int_t padNumber;                 //pad control number
  Int_t padRow;                    //pad address: row
  Int_t padColumn;                 //pad address: column

  Int_t numberOfStrides;           //number of strides on the pad

  //TIME CONTENT IS GOING TO BE SOON MODIFIED TO REPRODUCE A GET SIGNAL
  Double_t initTime;                //first induction time
  Double_t finalTime;               //last induction time
  Double_t sigmaTime;               //sigma in induction time

  Double_t chargeDeposited;         //charge deposited

  Int_t eventID;
  Int_t runID;

 public:
  ActarPadSignal();
  ~ActarPadSignal();

  void Reset(void);

  ActarPadSignal& operator=(const ActarPadSignal &right);

  Int_t GetPadNumber(){return padNumber;}
  Int_t GetPadRow(){return padRow;}
  Int_t GetPadColumn(){return padColumn;}

  Int_t GetNumberOfStrides(){return numberOfStrides;}

  Double_t GetInitTime(){return initTime;}
  Double_t GetFinalTime(){return finalTime;}
  Double_t GetSigmaTime(){return sigmaTime;}
  Double_t GetChargeDeposited(){return chargeDeposited;}

  Int_t GetEventID(){return eventID;}
  Int_t GetRunID(){return runID;}

  void SetPadNumber(Int_t pad){padNumber=pad;}
  void SetPadRow(Int_t pad){padRow=pad;}
  void SetPadColumn(Int_t pad){padColumn=pad;}

  void SetNumberOfStrides(Int_t num){numberOfStrides=num;}

  void SetInitTime(Double_t time){initTime=time;}
  void SetFinalTime(Double_t time){finalTime=time;}
  void SetSigmaTime(Double_t time){sigmaTime=time;}
  void SetChargeDeposited(Double_t cha){chargeDeposited = cha;}

  void SetEventID(Int_t id){eventID=id;}
  void SetRunID(Int_t id){runID=id;}

  ClassDef(ActarPadSignal,1);
};

ActarPadSignal::ActarPadSignal(){
  padNumber=0; padRow=0; padColumn=0;
  numberOfStrides=0;
  initTime=0.; finalTime=0.; sigmaTime=0.;
  chargeDeposited=0.;
  eventID=0; runID=0;

}
ActarPadSignal::~ActarPadSignal(){
}

void ActarPadSignal::Reset(void){
  // clearing to defaults
  padNumber=0; padRow=0; padColumn=0;
  numberOfStrides=0;
  initTime=0.; finalTime=0.; sigmaTime=0.;
  chargeDeposited=0.;
  eventID=0; runID=0;
}

ActarPadSignal& ActarPadSignal::operator=(const ActarPadSignal &right){
  // overloading the copy operator, similar as it in the ActarSimSimpleTrack class

  if(this != &right){
    padNumber = right.padNumber;
    padRow    = right.padRow;
    padColumn = right.padColumn;
    numberOfStrides = right.numberOfStrides;
    initTime  = right.initTime;
    finalTime = right.finalTime;
    sigmaTime = right.sigmaTime;
    chargeDeposited = right.chargeDeposited;
    eventID   = right.eventID;
    runID     = right.runID;
  }
  return *this;
}


class projectionOnPadPlane{

 private:
  ActarSimSimpleTrack* track;    //the track to be projected
  TVector3* pre;                 //projection of the initial point
  TVector3* post;                //projection of the final point
  Double_t timePre;              //drift time of the initial point
  Double_t timePost;             //drift time of the final point

  Double_t sigmaLongAtPadPlane;        //spatial spread at the pad plane
  Double_t sigmaTransvAtPadPlane;      //time spread at the pad plane

  Int_t position;               //0 still not calculated
                                //1 if any point is closer to the (0,0,z) than a delta (rho<delta)
                                //2 else if both points lies within the limits of the beamShielding
                                //3 else if one point is within the limits of the beamShielding
                                //4 else if both point lie in the gas outside the beamShielding
                                //5 if any point lies outside of the gas volume
 public:
  projectionOnPadPlane();
  virtual ~projectionOnPadPlane();

  ActarSimSimpleTrack* GetTrack(){return track;}
  TVector3* GetPre(){return pre;}
  TVector3* GetPost(){return post;}
  Double_t GetTimePre(){return timePre;}
  Double_t GetTimePost(){return timePost;}
  Double_t GetSigmaLongAtPadPlane(){return sigmaLongAtPadPlane;}
  Double_t GetSigmaTransvAtPadPlane(){return sigmaTransvAtPadPlane;}
  Int_t GetPosition(){return position;}

  void SetTrack(ActarSimSimpleTrack* tr){track = tr;}
  void SetPre(TVector3* ve){pre = ve;}
  void SetPost(TVector3* ve){post = ve;}
  void SetTimePre(Double_t time){timePre = time;}
  void SetTimePost(Double_t time){timePost = time;}
  void SetSigmaLongAtPadPlane(Double_t del){sigmaLongAtPadPlane = del;}
  void SetSigmaTransvAtPadPlane(Double_t del){sigmaTransvAtPadPlane = del;}
  void SetPosition(Int_t pos){position=pos;}

  ClassDef(projectionOnPadPlane,1);
};

projectionOnPadPlane::projectionOnPadPlane(){
  track=0; pre=new TVector3(1,1,1); post=new TVector3(1,1,1);
  timePre=-1.; timePost=-1.;
  sigmaLongAtPadPlane=-1.; sigmaTransvAtPadPlane=-1.;
  position=0;
}
projectionOnPadPlane::~projectionOnPadPlane(){
  delete pre;
  delete post;
}


class padsGeometry{

 private:
  Int_t numberOfColumns;    //columns: determined by the Z length
  Int_t numberOfRows;       //rows: determined by the sizes of pads and ACTAR
  Int_t numberOfPads;       //number of pads in the detector
                             //PADS, ROWS & COLUMNS begin in 1
                              //(if numberOfRows=80,
                             //then there are rows from 1 to 80).
  Int_t geoType;            //geometry type (0 box, 1 tube)
  Int_t padType;            //pad type (0 square, 1 hexagon)
  Int_t padLayout;          // layout pattern for hexagon pads
                            //   (0: MAYA like, 1: the one introduced by Hector)

  Double_t padSize;          //pads size (square side or hexagon side)
  Double_t rHexagon;         //for hexagon only, the apothem

  Double_t radius;           //      cylinder: radius
  Double_t xLength;         //all are half-length! dimension for the box case
  Double_t yLength;
  Double_t zLength;
  Double_t sideBlankSpaceX; // length of blank space between the GasBox and the Pad (both side in X direction)
  Double_t sideBlankSpaceZ; // length of blank space between the GasBox and the Pad (both side in Z direction)

  Double_t deltaProximityBeam; //to avoid strides to close to the beam
  Double_t sizeBeamShielding;  //radius of the beam shielding cylinder

  Int_t endCapMode;         //set to 1 for projection on the end cups

 public:
  padsGeometry();
  virtual ~padsGeometry();

  void SetPadsGeometry(void);

  void SetGeometryValues(Int_t geo, Int_t pad, Int_t layout, Double_t x, Double_t y, Double_t z,
			 Double_t ra, Double_t psi, Double_t gapx, Double_t gapz){
    geoType=geo;  padType=pad; padLayout=layout;
    xLength = x; yLength = y; zLength = z;
    radius=ra; padSize=psi;
    if(padType == 1)rHexagon = 0.8660254037844386467868626478 * padSize;
    else  rHexagon=0;
    sideBlankSpaceX=gapx; sideBlankSpaceZ=gapz;
    SetPadsGeometry();
  }

  void SetNumberOfColumns(Int_t col){numberOfColumns=col;}
  void SetNumberOfRows(Int_t row){numberOfRows=row;}
  void SetNumberOfPads(Int_t pad){numberOfPads=pad;}
  void SetGeoType(Int_t type){geoType=type;}
  void SetPadType(Int_t type){padType=type;}
  void SetPadLayout(Int_t layout){padLayout=layout;}
  void SetPadSize(Double_t si){padSize=si;}
  void SetRHexagon(Double_t si){rHexagon=si;}
  void SetXLength(Double_t x){xLength=x;}
  void SetYLength(Double_t y){yLength=y;}
  void SetZLength(Double_t z){zLength=z;}
  void SetSideBlankSpaceX(Double_t gapx){sideBlankSpaceX=gapx;}
  void SetSideBlankSpaceZ(Double_t gapz){sideBlankSpaceZ=gapz;}
  void SetRadius(Double_t ra){radius=ra;}
  void SetDeltaProximityBeam(Double_t de){deltaProximityBeam=de;}
  void SetSizeBeamShielding(Double_t le){sizeBeamShielding=le;}
  void SetEndCapModeOn(){endCapMode=1;}
  void SetEndCapModeOff(){endCapMode=0;}


  Int_t  GetNumberOfColumns(void){return numberOfColumns;}
  Int_t  GetNumberOfRows(void){return numberOfRows;}
  Int_t  GetNumberOfPads(void){return numberOfPads;}
  Int_t GetGeoType(void){return geoType;}
  Int_t GetPadType(void){return padType;}
  Int_t GetPadLayout(void){return padLayout;}
  Double_t GetPadSize(void){return padSize;}
  Double_t GetRHexagon(void){return rHexagon;}
  Double_t GetXLength(void){return xLength;}
  Double_t GetYLength(void){return yLength;}
  Double_t GetZLength(void){return zLength;}
  Double_t GetSideBlankSpaceX(void){return sideBlankSpaceX;}
  Double_t GetSideBlankSpaceZ(void){return sideBlankSpaceZ;}
  Double_t GetRadius(void){return radius;}
  Double_t GetDeltaProximityBeam(void){return deltaProximityBeam;}
  Double_t GetSizeBeamShielding(void){return sizeBeamShielding;}
  Int_t GetEndCapMode(void){return endCapMode;}

  TVector3 CoordinatesCenterOfPad(Int_t pad);
  Int_t IsInPadNumber(TVector3* point);
  Int_t GetPadColumnFromXZValue(Double_t x, Double_t z);
  Int_t GetPadRowFromXZValue(Double_t x, Double_t z);

  Int_t CalculatePad(Int_t r, Int_t c){
    //Pad number calculation from row and column (PADS, ROWS & COLUMNS begin in 1)
    if(r<=0 || r>numberOfRows || c<=0 || c>numberOfColumns){
      if(DIGI_DEBUG>4) {
        cout << "WARNING: padsGeometry:CalculatePad(" << r << "," << c
             <<"): row or column number out of range!" << " row=" << r << ", col=" << c << endl;
      }
      return 0;
    }
    else return ((r-1) * numberOfColumns + (c-1) + 1);
  }

  Int_t CalculateColumn(Int_t p){
    //Column calculation from pad (PADS, ROWS & COLUMNS begin in 1)
    if(p>numberOfPads || p==0) return 0;
    if(p%numberOfColumns==0) return numberOfColumns;
    else return p%numberOfColumns;}

  Int_t CalculateRow(Int_t p){
    //Row calculation from pad (PADS, ROWS & COLUMNS begin in 1)
    if(p>numberOfPads || p==0) return 0;
    else return (Int_t)(((p-1)/numberOfColumns)+1);}

  ClassDef(padsGeometry,1);
};

padsGeometry::padsGeometry(){
  numberOfColumns=0; numberOfRows=0; numberOfPads=0;
  geoType=999; padType=999; padLayout=0;
  padSize=0.; rHexagon=0.;
  xLength=0; yLength=0; zLength=0;
  sideBlankSpaceX=0.; sideBlankSpaceZ=0.;
  radius=0.;
  deltaProximityBeam=0.; sizeBeamShielding=0.;
  endCapMode=0;
}

padsGeometry::~padsGeometry(){
}

void padsGeometry::SetPadsGeometry(void){
  // the pads geometry should be calculated using this function
  // from the sizes and types of ACTAR geometry and pads geometry
  if(DIGI_DEBUG)
    cout << "________________________________________________________" << endl
         << "In padsGeometry::SetPadsGeometry() " << endl
         << "Note that the calculation of the pads geometry could "<< endl
         << "modify slightly the size of the pad you have introduced." << endl;
  if(geoType == 0 && padType == 0){ //box and square pad
    numberOfRows = (Int_t) (2*(xLength-sideBlankSpaceX)/padSize);
    if(DIGI_DEBUG)
      cout << "User selected a box with square pads" << endl
	         << "User selected a padSize = " << padSize;
    padSize = (2*(xLength-sideBlankSpaceX)) / numberOfRows;
    if(DIGI_DEBUG)
      cout << " after the calculation: padSize = " << padSize <<endl;
    numberOfColumns = ((Int_t) (2*(zLength-sideBlankSpaceZ) / padSize)) - 1;
    if((numberOfColumns+1)*padSize <= 2*(zLength-sideBlankSpaceZ)) numberOfColumns++;
    numberOfPads = numberOfRows*numberOfColumns;
    if(DIGI_DEBUG)
      cout  << "________________________________________________________" << endl
	          << " Output of padsGeometry::SetPadsGeometry() " << endl
	          << " numberOfRows = "<< numberOfRows
	          << ", numberOfColumns = " << numberOfColumns << endl
	          << "________________________________________________________"<< endl;
  }
  else if(geoType == 0 && padType == 1 && padLayout==0){ //box and hexagonal pad with MAYA-type layout
    numberOfColumns = (Int_t) (zLength/rHexagon);
    if(DIGI_DEBUG)
      cout << "User selected a box with hexagonal pads (MAYA-type)" << endl
	         << "User selected a padSize = " << padSize
	         << " and therefore a rHexagon =  " << rHexagon<< endl;
    rHexagon =  zLength / numberOfColumns;
    padSize = 1.154700538379251529013 * rHexagon;
    if(DIGI_DEBUG)
      cout << " after the calculation: padSize = " << padSize
	         << " and therefore a rHexagon =  " << rHexagon << endl;
    numberOfRows = (Int_t) ((2.*xLength-2.*padSize)/(1.5*padSize))+1;
    sideBlankSpaceX = (2.*xLength-(numberOfRows-1)*1.5*padSize-2.*padSize )/2.;
    numberOfPads = numberOfRows*numberOfColumns;
    if(DIGI_DEBUG)
      cout  << "________________________________________________________" << endl
	          << " Output of padsGeometry::SetPadsGeometry() " << endl
	          << " numberOfRows = "<< numberOfRows
	          << ", numberOfColumns = " << numberOfColumns << endl
	          << "________________________________________________________"<< endl;
  }
  else if(geoType == 0 && padType == 1 && padLayout==1){ //box and hexagonal pad
    numberOfRows = (Int_t) (xLength/rHexagon);
    if(DIGI_DEBUG)
      cout << "User selected a box with hexagonal pads (rotated wrt MAYA-type)" << endl
           << "User selected a padSize = " << padSize
           << " and therefore a rHexagon =  " << rHexagon<< endl;
    rHexagon =  xLength / numberOfRows;
    padSize = 1.154700538379251529013 * rHexagon;
    if(DIGI_DEBUG)
      cout << " after the calculation: padSize = " << padSize
           << " and therefore a rHexagon =  " << rHexagon << endl;
    numberOfColumns = (Int_t) ((2.*zLength-2.*padSize)/(1.5*padSize)) + 1;
    sideBlankSpaceZ = (2.*zLength -(numberOfColumns-1)*1.5*padSize-2.*padSize)/2.;
    numberOfPads = numberOfRows*numberOfColumns;
    if(DIGI_DEBUG)
      cout  << "________________________________________________________" << endl
            << " Output of padsGeometry::SetPadsGeometry() " << endl
            << " numberOfRows = "<< numberOfRows
            << ", numberOfColumns = " << numberOfColumns << endl
            << "________________________________________________________"<< endl;
  }
  else if(geoType == 1 && padType == 0){ //cylinder and square pad
    numberOfRows = (Int_t) (2*TMath::Pi()*radius / padSize);
    if(DIGI_DEBUG)
      cout << "User selected a cylinder with square pads (on the cylindrical walls)" << endl
	         << "User selected a padSize = " << padSize;
    padSize = 2*TMath::Pi()*radius / numberOfRows;
    if(DIGI_DEBUG)
      cout << " after the calculation: padSize = " << padSize <<endl;
    numberOfColumns = ((Int_t) (2*zLength / padSize)) - 1;
    if( (numberOfColumns+1)*padSize <=  2*zLength ) numberOfColumns++;
    numberOfPads = numberOfRows*numberOfColumns;
    if(DIGI_DEBUG)
      cout  << "________________________________________________________" << endl
	          << " Output of padsGeometry::SetPadsGeometry() " << endl
	          << " numberOfRows = "<< numberOfRows
	          << ", numberOfColumns = " << numberOfColumns << endl
	          << "________________________________________________________"<< endl;
  }
  else if(geoType == 1 && padType == 1){ //cylinder and hexagonal pad
    numberOfRows = (Int_t) (TMath::Pi()*radius / rHexagon);
    if(DIGI_DEBUG)
      cout << "User selected a cylinder with hexagonal pads" << endl
           << "User selected a padSize = " << padSize
           << " and therefore a rHexagon =  " << rHexagon<< endl;
    rHexagon = TMath::Pi()*radius / numberOfRows;
    padSize = 1.154700538379251529013 * rHexagon;
    if(DIGI_DEBUG)
      cout << " after the calculation: padSize = " << padSize
           << " and therefore a rHexagon =  " << rHexagon << endl;
    numberOfColumns = ((Int_t) (2*zLength / (1.5*padSize))) - 1;
    if( (numberOfColumns+1)*1.5*padSize <=  2*zLength ) numberOfColumns++;
    numberOfPads = numberOfRows*numberOfColumns;
    if(DIGI_DEBUG)
      cout  << "________________________________________________________" << endl
            << " Output of padsGeometry::SetPadsGeometry() " << endl
            << " numberOfRows = "<< numberOfRows
            << ", numberOfColumns = " << numberOfColumns << endl
            << "________________________________________________________"<< endl;
  }
  else {
    if(DIGI_DEBUG)
      cout << "ERROR: No valid geometry... Have you called "
	         << "SetGeometryValues() with valid arguments?" << endl << endl;
  }
}

Int_t padsGeometry::IsInPadNumber(TVector3* point){
  //calculates the pad number where the point is
  Int_t column; Int_t row;
  if(geoType == 0 && padType == 0) { //box and square pad
    row = (Int_t) (((point->X() - sideBlankSpaceX + xLength)/ padSize) + 1);
    column = (Int_t) ((point->Z() - sideBlankSpaceZ)/ padSize)+1);
    if(column > 0 && column < numberOfColumns+1
       && row > 0 && row < numberOfRows+1) {
      if(DIGI_DEBUG>2)
	      cout << "In padsGeometry::IsInPadNumber()" << endl
	           << " Pad (" << row << "," << column << ") for point "
	           << point->X() << ","<< point->Y() << ","<< point->Z()<< endl;
      return CalculatePad(row,column);
    }
    else{
      if(DIGI_DEBUG)
	      cout << "ERROR: in padsGeometry::IsInPadNumber()" << endl
	           << " Invalid pad returned from requested point "
	           << point->X() << ","<< point->Y() << ","<< point->Z()<< endl;
      return 0;
    }
  }
  else if(geoType == 0 && padType == 1 && padLayout == 0){ //box and hexagonal pad and MAYA-type layout
    if(point->X()< -xLength || point->X()>xLength || point->Z()< 0 || point->Z()>2*zLength) {
      if(DIGI_DEBUG)
	      cout << "ERROR: in padsGeometry::IsInPadNumber()" << endl
	           << " Invalid pad returned from requested point "
	           << point->X() << ","<< point->Y() << ","<< point->Z()<< endl;
      return 0;
    }
    row = (Int_t) ((point->X() + xLength - sideBlankSpaceX)/(1.5*padSize))+1;
    column = (Int_t) (point->Z()/ (2*rHexagon)) + 1;
    Double_t shorterDist = padSize; Int_t candidate=0; point->SetY(-yLength);
    for(Int_t i=0;i<2;i++){   //checking if it is on the next column
      for(Int_t j=-1;j<1;j++){   //checking if it is on the previous row
	       if((column+i)>numberOfColumns || (row+j)<1 || (row+j)>numberOfRows) continue;
         TVector3 distance = *point - CoordinatesCenterOfPad(CalculatePad(row+j,column+i));
	       if (distance.Mag() <= rHexagon){
           return CalculatePad(row+j,column+i);
         }
         if(distance.Mag() <= shorterDist) {
	         shorterDist = distance.Mag();
	         candidate = CalculatePad(row+j,column+i);
         }
      }
    }
    if(DIGI_DEBUG>2)
      cout << "In padsGeometry::IsInPadNumber()" << endl
	         << " Pad (" << row << "," << column << ") for point "
	         << point->X() << ","<< point->Y() << ","<< point->Z()<< endl;
    return candidate;
  }
  else if(geoType == 0 && padType == 1 && padLayout == 1){ //box and hexagonal pad
    if(point->X()< -xLength || point->X()>xLength || point->Z()< 0 || point->Z()>2*zLength) {
      if(DIGI_DEBUG)
        cout << "ERROR: in padsGeometry::IsInPadNumber()" << endl
             << " Invalid pad returned from requested point "
             << point->X() << ","<< point->Y() << ","<< point->Z()<< endl;
      return 0;
    }
    row = (Int_t) (((point->X() + xLength)/ (2*rHexagon)) + 1);
    column = (Int_t) (((point->Z()-sideBlankSpaceZ)/(1.5*padSize))+1);
    Double_t shorterDist = padSize; Int_t candidate=0; point->SetY(-yLength);
    for(Int_t i=0;i<2;i++){   //checking if it is on the next row
      for(Int_t j=-1;j<1;j++){   //checking if it is on the previous column
        if(row+i>numberOfRows || column+j<1 || column+j>numberOfColumns) continue;
        TVector3 distance = *point - CoordinatesCenterOfPad(CalculatePad(row+i,column+j));
        if (distance.Mag() <= rHexagon) return CalculatePad(row+i,column+j);
        if( distance.Mag() <= shorterDist ) {
          shorterDist = distance.Mag();
          candidate = CalculatePad(row+i,column+j);
        }
      }
    }
    if(DIGI_DEBUG>2)
      cout << "In padsGeometry::IsInPadNumber()" << endl
           << " Pad (" << row << "," << column << ") for point "
           << point->X() << ","<< point->Y() << ","<< point->Z()<< endl;
    return candidate;
  }
  else if(geoType == 1 && padType == 0){ //cylinder and square pad
    if(point->Phi()>=0) row =(Int_t)(numberOfRows * (point->Phi()) / (2*TMath::Pi())) +1;
    else row =(Int_t)(numberOfRows * ( point->Phi() + (2*TMath::Pi())) / (2*TMath::Pi())) +1;
    column = (Int_t) ((point->Z() / padSize)+1);
    if(column > 0 && column < numberOfColumns+1 &&
       row > 0 && row < numberOfRows+1) {
      if(DIGI_DEBUG>2)
	      cout << "In padsGeometry::IsInPadNumber()" << endl
	           << " Pad (" << row << "," << column << ") for point "
	           << point->X() << ","<< point->Y() << ","<< point->Z()<< endl;
      return CalculatePad(row,column);
    }
    else{
      if(DIGI_DEBUG)
	      cout << "ERROR: in padsGeometry::IsInPadNumber()" << endl
	           << " Invalid pad returned from requested point "
	           << point->X() << ","<< point->Y() << ","<< point->Z()<< endl;
      return 0;
    }
  }
  else if(geoType == 1 && padType == 1){ //cylinder and hexagonal pad
    if(point->Z()<0 || point->Z()>2*zLength) {
      if(DIGI_DEBUG)
        cout << "ERROR: in padsGeometry::IsInPadNumber()" << endl
             << " Invalid pad returned from requested point "
             << point->X() << ","<< point->Y() << ","<< point->Z()<< endl;
      return 0;
    }
    if(point->Phi()>=0) row= (Int_t)(numberOfRows * (point->Phi()) / (2*TMath::Pi())) + 1;
    else row= (Int_t)(numberOfRows * (point->Phi()+(2*TMath::Pi())) / (2*TMath::Pi())) + 1;
    column = (Int_t)((point->Z()/(1.5*padSize))+1);
    Double_t shorterDist = padSize; Int_t candidate=0; point->SetPerp(radius);
    for(Int_t i=0;i<2;i++){   //checking if it is on the next row
      for(Int_t j=-1;j<1;j++){   //checking if it is on the previous column
        if(row+i>numberOfRows || column+j<1 || column+j>numberOfColumns) continue;
        TVector3 distance = *point - CoordinatesCenterOfPad(CalculatePad(row+i,column+j));
        if( distance.Mag() <= shorterDist ) {
          shorterDist = distance.Mag();
          candidate = CalculatePad(row+i,column+j);
        }
      }
    }
    if(DIGI_DEBUG>2)
      cout << "In padsGeometry::IsInPadNumber()" << endl
           << " Pad (" << row << "," << column << ") for point "
           << point->X() << ","<< point->Y() << ","<< point->Z()<< endl;
    return candidate;
  }
  else {
    if(DIGI_DEBUG)
      cout << "No valid geometry... Have you called "
	         <<"SetGeometryValues() with valid arguments?" <<endl<<endl;
    return 0;
  }
}

Int_t padsGeometry::GetPadColumnFromXZValue(Double_t x, Double_t z){
  //calculates the pad column number by x, z-values of a point
  // NOTE: column number here start from 1
  // column number returned here is allowed to be out of the range of the chamber
  TVector3 point(x, -yLength, z);
  TVector3 vec;

  Int_t column=0, row=0;
  if(geoType == 0 && padType == 0){ //box and square pad
    column = (Int_t) (((z - sideBlankSpaceZ) / padSize)+1);
    return column;
  }
  else if(geoType == 0 && padType == 1 && padLayout == 0){ //box and hexagonal pad with MAYA-type layout
    column = (Int_t) ((point.Z()/(2*rHexagon))+1);
    return column;
  }
  else if(geoType == 0 && padType == 1 && padLayout == 1){ //box and hexagonal pad
    row = (Int_t) (((point.X() + xLength)/ (2*rHexagon)) + 1);
    column = (Int_t) (((point.Z()-sideBlankSpaceZ)/(1.5*padSize))+1);
    Double_t shorterDist = padSize; Int_t candidate=0; point.SetY(-yLength);
    for(Int_t i=0;i<2;i++){   //checking if it is on the next row
      for(Int_t j=-1;j<1;j++){   //checking if it is on the previous column
        vec.SetXYZ(-xLength + ((2*(row+i))-1)*rHexagon,
                   -yLength,
                   padSize*((column+j)*1.5-0.5)+sideBlankSpaceZ);
        if((column+j)%2==0) vec.SetX(vec.X()-rHexagon);
        TVector3 distance = point - vec;
        if (distance.Mag() <= rHexagon) return column+j;
        if( distance.Mag() <= shorterDist ) {
          shorterDist = distance.Mag();
          candidate = column+j;
        }
      }
    }
    if(DIGI_DEBUG>2)
      cout << "In padsGeometry::IsInPadNumber()" << endl
           << " Pad (" << row << "," << column << ") for point "
           << point.X() << ","<< point.Y() << ","<< point.Z()<< endl;
    return candidate;
  }
  else {
    cout << "No valid geometry... Have you called "
         <<"SetGeometryValues() with valid arguments?" <<endl<<endl;
    return 0;
  }
}

Int_t padsGeometry::GetPadRowFromXZValue(Double_t x, Double_t z){
  //calculates the pad column number by x, z-values of a point
  // NOTE: column number here start from 1
  // column number returned here is allowed to be out of the range of the chamber
  TVector3 point(x, -yLength, z);
  TVector3 vec;

  Int_t column=0, row=0;
  if(geoType == 0 && padType == 0){ //box and square pad
    row =  (Int_t) numberOfRows/2.+ ((x / padSize)+1);
    return row;
  }
  else {
    cout << "No valid geometry... Have you called"
         <<"SetGeometryValues() with valid arguments?" <<endl<<endl;
    return 0;
  }
}


TVector3 padsGeometry::CoordinatesCenterOfPad(Int_t pad){
  if(pad==0 || pad> numberOfPads) {
    if(DIGI_DEBUG)
      cout << "ERROR in padsGeometry::CoordinatesCenterOfPad() " << endl
			     << " Invalid pad number " << pad
			     << " (0 or larger than maximum pad number)" << endl;
    TVector3 vec(0,0,0); //HAPOL IS THIS RIGHT? SHOULD IT BE FAR AWAY?
    return vec;
  }
  Int_t row = CalculateRow(pad);
  Int_t column =  CalculateColumn(pad);
  if(geoType == 0 && padType == 0){ //box and square pad
    TVector3 vec(-xLength + (row-0.5)*padSize, -yLength,(column-0.5)*padSize);
    if(DIGI_DEBUG>2)
      cout <<  "________________________________________________________" << endl
	         << " Output of padsGeometry::CoordinatesCenterOfPad(" << pad << ") " << endl
	         <<  " row = "<< row << ", column = " <<  column << endl
	         << " x = "<<  vec.x() << ", y = " <<  vec.y() << ", z = " << vec.z() << endl
           << "________________________________________________________"<< endl;
    return vec;
  }
  else if(geoType == 0 && padType == 1 && padLayout == 0){ //box and hexagonal pad with MAYA-type layout
    TVector3 vec(-xLength + sideBlankSpaceX + padSize*((row*1.5)-0.5),
                 -yLength,
                 (2*column-1)*rHexagon);
    if(row%2==0) vec.SetZ(vec.Z()-rHexagon);
    if(DIGI_DEBUG>2)
      cout <<  "________________________________________________________" << endl
	         << " Output of padsGeometry::CoordinatesCenterOfPad(" << pad << ") " << endl
	         <<  " row = "<< row << ", column = " <<  column << endl
	         << " x = "<<  vec.x() << ", y = " <<  vec.y() << ", z = " << vec.z() << endl
           << "________________________________________________________"<< endl;
    return vec;
  }
  else if(geoType == 0 && padType == 1 && padLayout == 1){ //box and hexagonal pad
    TVector3 vec(-xLength + ((2*row)-1)*rHexagon,
		             -yLength,
		             padSize*((column*1.5)-0.5)+sideBlankSpaceZ);
    if(column%2==0) vec.SetX(vec.X()-rHexagon);
    if(DIGI_DEBUG>2)
      cout <<  "________________________________________________________" << endl
           << " Output of padsGeometry::CoordinatesCenterOfPad(" << pad << ") " << endl
           <<  " row = "<< row << ", column = " <<  column << endl
           << " x = "<<  vec.x() << ", y = " <<  vec.y() << ", z = " << vec.z() << endl
           << "________________________________________________________"<< endl;
    return vec;
  }
  else if(geoType == 1 && padType == 0){ //cylinder and square pad
    TVector3 vec(radius, radius, (column-0.5)*padSize);
    vec.SetPerp(radius);
    vec.SetPhi( (row-0.5) * 2 * TMath::Pi() /  numberOfRows );
    if(DIGI_DEBUG>2)
      cout <<  "________________________________________________________" << endl
	         << " Output of padsGeometry::CoordinatesCenterOfPad(" << pad << ") " << endl
           <<  " row = "<< row << ", column = " <<  column << endl
	         << " x = "<<  vec.x() << ", y = " <<  vec.y() << ", z = " << vec.z() << endl
	         << "________________________________________________________"<< endl;
    return vec;
  }
  else if(geoType == 1 && padType == 1){ //cylinder and hexagonal pad
    TVector3 vec(radius, radius, padSize*((column*1.5)-0.5));
    vec.SetPerp(radius);
    if(column%2==1) vec.SetPhi( (row-0.5) * 2 * TMath::Pi() /  numberOfRows ) ;
    else vec.SetPhi( (row-1) * 2 * TMath::Pi() /  numberOfRows);
    if(DIGI_DEBUG>2)
      cout <<  "________________________________________________________" << endl
           << " Output of padsGeometry::CoordinatesCenterOfPad(" << pad << ") " << endl
           <<  " row = "<< row << ", column = " <<  column << endl
           << " x = "<<  vec.x() << ", y = " <<  vec.y() << ", z = " << vec.z() << endl
           << "________________________________________________________"<< endl;
    return vec;
  }
  else {
    if(DIGI_DEBUG)
      cout << "No valid geometry... Have you called "
	         <<"SetGeometryValues() with valid arguments?" <<endl<<endl;
    TVector3 vec3(1.,1.,1.); //HAPOL IS THIS RIGHT? SHOULD IT BE FAR AWAY?
    return vec3;
  }
}


class amplificationManager{

 private:
  Int_t isWire;
  Double_t radiusOfAmpliWire; // radius of amplification wire (ra in Mathieson)
  Double_t pitchOfAmpliWire;  // distance between two neighbouring amplification wires, (s in Mathieson)
  Double_t ACseparation;      // anode (Wire) and cathode (pads) separation, (h in Mathieson)
  Double_t K1P, K2P, K3P;     // K1, K2, and K3 in Mathieson, NIMA270(1988)602 for X directions
  Double_t K1N, K2N, K3N;     // K1, K2, and K3 for Y direction
  Double_t lambdaP, lambdaN;  // lambda in Mathieson, NIMA270(1988)602,
                              //   for x (parallel      to the wire) and
                              //       y (perpendicular to the wire), respectively
  Double_t rhoP, rhoN;        // relative induction charge, rho in Mathieson, for X and Y

 public:
  amplificationManager();
  virtual ~amplificationManager();

  void SetIsWireOn(){
    isWire=1;
  }

  void SetIsWireOff(){
    isWire=0;
  }

  Int_t GetIsWire(){
    return isWire;
  }

  void SetWireAmplificationParameters(Double_t ra, Double_t s, Double_t h);

  void SetRadiusOfAmpliWire(Double_t ra) {radiusOfAmpliWire = ra;}
  void SetPitchOfAmpliWire(Double_t s) {pitchOfAmpliWire = s;}
  void SetACseparation(Double_t h) {ACseparation = h;}
  void SetMathiesonFactorK1P(Double_t k1p) {K1P = k1p;}
  void SetMathiesonFactorK2P(Double_t k2p) {K2P = k2p;}
  void SetMathiesonFactorK3P(Double_t k3p) {K3P = k3p;}
  void SetMathiesonFactorK1N(Double_t k1n) {K1N = k1n;}
  void SetMathiesonFactorK2N(Double_t k2n) {K2N = k2n;}
  void SetMathiesonFactorK3N(Double_t k3n) {K3N = k3n;}

  Double_t GetRadiusOfAmpliWire() {return radiusOfAmpliWire;}
  Double_t GetPitchOfAmpliWire()  {return pitchOfAmpliWire;}
  Double_t GetACseparation()      {return ACseparation;}
  Double_t GetMathiesonFactorK1P() {return K1P;}
  Double_t GetMathiesonFactorK2P() {return K2P;}
  Double_t GetMathiesonFactorK3P() {return K3P;}
  Double_t GetMathiesonFactorK1N() {return K1N;}
  Double_t GetMathiesonFactorK2N() {return K2N;}
  Double_t GetMathiesonFactorK3N() {return K3N;}

  Double_t CalculateRhoP(Double_t x){
    //calculation of the relative induction charge for X (see Mathieson paper)
    lambdaP= x/ACseparation;
    Double_t commonFactor=tanh(K2P*lambdaP)*tanh(K2P*lambdaP);
    rhoP=K1P*(1.-commonFactor)/(1.+K3P*commonFactor);
    return rhoP;
  }

  Double_t CalculateRhoN(Double_t y){
    //calculation of the relative induction charge for Y (see Mathieson paper)
    lambdaN= y/ACseparation;
    Double_t commonFactor=tanh(K2N*lambdaN)*tanh(K2N*lambdaN);
    rhoN=K1N*(1.-commonFactor)/(1.+K3N*commonFactor);
    return rhoN;
  }
  ClassDef(amplificationManager,1);
};

amplificationManager::amplificationManager(){
  isWire=0;
  radiusOfAmpliWire=0.02; // 20 mu
  pitchOfAmpliWire=2.0;   // 2 mm
  ACseparation=10.0;      // 10 mm
  K1P=1.; K2P=1.; K3P=1.;     // K1, K2, and K3 in Mathieson, NIMA270(1988)602 for X directions
  K1N=1.; K2N=1.; K3N=1.;     // K1, K2, and K3 for Y direction
  lambdaP=1.; lambdaN=1.;     // lambda in Mathieson, NIMA270(1988)602,
                              //   for x (parallel      to the wire) and
                              //       y (perpendicular to the wire), respectively
  rhoP=0.; rhoN=0.;        // relative induction charge, rho in Mathieson, for X and Y
}
amplificationManager::~amplificationManager(){
}

void amplificationManager::SetWireAmplificationParameters(Double_t ra, Double_t s, Double_t h){
  //     radiusOfAmpliWire = ra;
  //     pitchOfAmpliWire  = s;
  //     ACseparation      = h;
  Double_t k1p=0.0, k2p=0.0, k3p=0.0, k1n=0.0, k2n=0.0, k3n=0.0;

  Double_t ras=ra/s;
  Double_t ras1=1.5e-3, ras2=2.5e-3, ras3=3.75e-3, ras4=5.25e-3, ras5=7.5e-3;

  Double_t ras01=ras -ras1, ras02=ras -ras2, ras03=ras -ras3, ras04=ras -ras4, ras05=ras -ras5;
  Double_t ras12=ras1-ras2, ras13=ras1-ras3, ras14=ras1-ras4, ras15=ras1-ras5;
  Double_t ras21=ras2-ras1, ras23=ras2-ras3, ras24=ras2-ras4, ras25=ras2-ras5;
  Double_t ras31=ras3-ras1, ras32=ras3-ras2, ras34=ras3-ras4, ras35=ras3-ras5;
  Double_t ras41=ras4-ras1, ras42=ras4-ras2, ras43=ras4-ras3, ras45=ras4-ras5;
  Double_t ras51=ras5-ras1, ras52=ras5-ras2, ras53=ras5-ras3, ras54=ras5-ras4;

  Double_t hs = h/s;

  if(hs <= 1.40){ // K3 factor for direction parallel to wire or normal to wire are different
    // calculate K3P
    Double_t a14 = -0.26171, a13 = 1.0914, a12 = -1.61916, a11 = 0.765601, a10 = 0.602428;
    Double_t a24 = -0.287172,a23 = 1.20289,a22 = -1.79557, a21 = 0.876833, a20 = 0.547614;
    Double_t a34 = -0.356592,a33 = 1.45366,a32 = -2.1068,  a31 = 1.02757,  a30 =0.492266;
    Double_t a44 = -0.398035,a43 = 1.62109,a42 = -2.34602, a41 = 1.17147,  a40 =0.433379;
    Double_t a54 = -0.449798,a53 = 1.83572,a52 = -2.66741, a51 = 1.37198,  a50 =0.355622;

    Double_t K3P1=a14*pow(hs,4.)+a13*pow(hs,3.)+a12*hs*hs+a11*hs+a10;
    Double_t K3P2=a24*pow(hs,4.)+a23*pow(hs,3.)+a22*hs*hs+a21*hs+a20;
    Double_t K3P3=a34*pow(hs,4.)+a33*pow(hs,3.)+a32*hs*hs+a31*hs+a30;
    Double_t K3P4=a44*pow(hs,4.)+a43*pow(hs,3.)+a42*hs*hs+a41*hs+a40;
    Double_t K3P5=a54*pow(hs,4.)+a53*pow(hs,3.)+a52*hs*hs+a51*hs+a50;

    k3p=K3P1*(ras02*ras03*ras04*ras05)/(ras12*ras13*ras14*ras15)+
        K3P2*(ras01*ras03*ras04*ras05)/(ras21*ras23*ras24*ras25)+
        K3P3*(ras01*ras02*ras04*ras05)/(ras31*ras32*ras34*ras35)+
        K3P4*(ras01*ras02*ras03*ras05)/(ras41*ras42*ras43*ras45)+
        K3P5*(ras01*ras02*ras03*ras04)/(ras51*ras52*ras53*ras54);

    // calculate K3N
    a14 =-0.56927 ;a13 =2.15238; a12 =-2.68646; a11 =0.815535; a10 =0.929974;
    a24 =-0.601139;a23 =2.2645;  a22 =-2.80946; a21 =0.828462; a20 =0.932558;
    a34 =-0.615971;a33 =2.34445; a32 =-2.92218; a31 =0.844926; a30 =0.935198;
    a44 =-0.79293 ;a43 =2.94533; a42 =-3.58834; a41 = 1.08576; a40 =0.908493;
    a54 =-0.841875;a53 =3.10651; a52 =-3.74454; a51 =1.09552;  a50 =0.914561;

    Double_t K3N1=a14*pow(hs,4.)+a13*pow(hs,3.)+a12*hs*hs+a11*hs+a10;
    Double_t K3N2=a24*pow(hs,4.)+a23*pow(hs,3.)+a22*hs*hs+a21*hs+a20;
    Double_t K3N3=a34*pow(hs,4.)+a33*pow(hs,3.)+a32*hs*hs+a31*hs+a30;
    Double_t K3N4=a44*pow(hs,4.)+a43*pow(hs,3.)+a42*hs*hs+a41*hs+a40;
    Double_t K3N5=a54*pow(hs,4.)+a53*pow(hs,3.)+a52*hs*hs+a51*hs+a50;

    k3n=K3N1*(ras02*ras03*ras04*ras05)/(ras12*ras13*ras14*ras15)+
        K3N2*(ras01*ras03*ras04*ras05)/(ras21*ras23*ras24*ras25)+
        K3N3*(ras01*ras02*ras04*ras05)/(ras31*ras32*ras34*ras35)+
        K3N4*(ras01*ras02*ras03*ras05)/(ras41*ras42*ras43*ras45)+
        K3N5*(ras01*ras02*ras03*ras04)/(ras51*ras52*ras53*ras54);
  }
  else if(hs > 1.40){ // K3 factor for direction parallel to wire or normal to wire are the same
    Double_t a13 = -0.003152, a12 = 0.05202, a11 = -0.3206, a10 = 0.8442;
    Double_t a23 = -0.003285, a22 = 0.05351, a21 = -0.3215, a20 = 0.8072;
    Double_t a33 = -0.003576, a32 = 0.05678, a31 = -0.3279, a30 = 0.7774;
    Double_t a43 = -0.003753, a42 = 0.05816, a41 = -0.3257, a40 = 0.7409;
    Double_t a53 = -0.003850, a52 = 0.05845, a51 = -0.3184, a50 = 0.6947;

    Double_t K3P1=a13*pow(hs,3.)+a12*hs*hs+a11*hs+a10;
    Double_t K3P2=a23*pow(hs,3.)+a22*hs*hs+a21*hs+a20;
    Double_t K3P3=a33*pow(hs,3.)+a32*hs*hs+a31*hs+a30;
    Double_t K3P4=a43*pow(hs,3.)+a42*hs*hs+a41*hs+a40;
    Double_t K3P5=a53*pow(hs,3.)+a52*hs*hs+a51*hs+a50;

    k3p=K3P1*(ras02*ras03*ras04*ras05)/(ras12*ras13*ras14*ras15)+
        K3P2*(ras01*ras03*ras04*ras05)/(ras21*ras23*ras24*ras25)+
        K3P3*(ras01*ras02*ras04*ras05)/(ras31*ras32*ras34*ras35)+
        K3P4*(ras01*ras02*ras03*ras05)/(ras41*ras42*ras43*ras45)+
        K3P5*(ras01*ras02*ras03*ras04)/(ras51*ras52*ras53*ras54);

    k3n=k3p;
  }

  // calculate K1 and K2
  Double_t sqrtK3P = sqrt(k3p);
  Double_t sqrtK3N = sqrt(k3n);

  // K2=PI*(1-sqrt(K3)/2)/2, PI/2=1.571
  k2p=1.571*(1.0-sqrtK3P/2.0);
  k2n=1.571*(1.0-sqrtK3N/2.0);

  // K1=K2*sqrt(K3)/(4.*atan(sqrt(K3)))
  k1p=k2p*sqrtK3P/(4.*atan(sqrtK3P));
  k1n=k2n*sqrtK3N/(4.*atan(sqrtK3N));

  SetRadiusOfAmpliWire(ra);
  SetPitchOfAmpliWire(s);
  SetACseparation(h);
  SetMathiesonFactorK1P(k1p);
  SetMathiesonFactorK2P(k2p);
  SetMathiesonFactorK3P(k3p);
  SetMathiesonFactorK1N(k1n);
  SetMathiesonFactorK2N(k2n);
  SetMathiesonFactorK3N(k3n);
}


class driftManager{

 private:
  padsGeometry* padsGeo;          //ACTAR pads geometry class
  amplificationManager* ampManager;
  Double_t longitudinalDiffusion; //gas longitudinal diff. for e-
  Double_t transversalDiffusion;  //gas transversal diff. for e-
  Double_t driftVelocity;         //gas drift velocity for e
  Double_t lorentzAngle;

  Double_t magneticField;         //magnetic field (T)
                                  //for a box: normal to E
                                  //for a cylinder: parallel to Z
  //creo que si pongo diffusion y drift no deberia poner campo magnetico
  //sino parameters de deriva asociados al campo magnetico...

  Double_t padPlaneRadius;
  Double_t gasWvalue;      //The W-value is defined as the average energy lost by
                           //the incident particle per ion pair formed. Due to the
                           //competing mechanism of the energy loss, i.e. excitation,
                           //W-value is always greater than the ionization energy.
                           //It runs from 25 to 45 eV for most gases of interest.

  Bool_t oldChargeCalculation; //Make it True if you want to test the old style of calculation

 public:
  driftManager();
  virtual ~driftManager();

  void SetDiffusionParameters(Double_t lon, Double_t tra){
    longitudinalDiffusion = lon;
    transversalDiffusion = tra;
  }

  void SetDriftParameters(Double_t votage, Double_t height, Double_t pressure, TString gasName);

  void SetDriftVelocity(Double_t vel){driftVelocity=vel;}
  void SetLorentzAngle(Double_t vel){lorentzAngle=vel;}
  void SetMagneticField(Double_t vel){magneticField=vel;}
  void SetGasWvalue(Double_t value){gasWvalue=value;}
  void SetOldChargeCalculation(void){oldChargeCalculation=kTRUE;}
  void SetNewChargeCalculation(void){oldChargeCalculation=kFALSE;}

  Double_t GetLongitudinalDiffusion(void){return longitudinalDiffusion;}
  Double_t GetTransversalDiffusion(void){return transversalDiffusion;}
  Double_t GetDriftVelocity(void){return driftVelocity;}
  Double_t GetLorentzAngle(void){return lorentzAngle;}
  Double_t GetMagneticField(void){return magneticField;}
  Double_t GetGasWvalue(void){return gasWvalue;}
  Bool_t GetOldChargeCalculation(void){return oldChargeCalculation;}

  void GetStatus(void);

  void ConnectToGeometry(padsGeometry* pad){padsGeo = pad;}
  void ConnectToAmplificationManager(amplificationManager* amp){ampManager = amp;}
  Int_t CalculatePositionAfterDrift(projectionOnPadPlane* pro);
  void CalculatePadsWithCharge(projectionOnPadPlane* pro, TClonesArray* clo,
                               Int_t &numberOfPadsBeforeThisLoopStarted,TTree *T);
  void CalculatePadsWithCharge_oldStyle(Double_t k1p, Double_t k2p, Double_t k3p,
                                        Double_t k1n, Double_t k2n, Double_t k3n,
                                        projectionOnPadPlane* pro, TClonesArray* clo,
                                        Int_t &numberOfPadsBeforeThisLoopStarted) {
  ClassDef(driftManager,1);
};

driftManager::driftManager(){
  padsGeo=0;
  ampManager=0;
  longitudinalDiffusion=0.;
  transversalDiffusion=0.;
  driftVelocity=0.;
  lorentzAngle=0.;magneticField=0.;
  gasWvalue=30.;
  oldChargeCalculation=kFALSE;
}

driftManager::~driftManager(){
}

void driftManager::SetDriftParameters(Double_t voltage, Double_t height, Double_t pressure, TString gasName){
  // units: voltage: volts, for MAYA, this is the voltage between the upper cathode and the Frish grid
  //        height:  mm, for MAYA, this is the distance between the upper cathode and the Frish grid
  //        pressure: mbar, pressure of the active gas
  // cf:  simulation report of D.Y. Pang
  if(gasName=="deuterium"){
    Double_t fieldStrength= voltage/(height/10.);            // E=V/D, in volts/cm
    Double_t eOverP = fieldStrength/(pressure*0.75006);      // E/P,   in volts cm^-1 torr^-1
    Double_t a0=13.759, a1=0.480, a2=0.0242, a3=0.0126;      // coefficients for velocity (W)
    Double_t velocity = exp(a0+a1*log(eOverP)+a2*pow(log(eOverP),2)+a3*pow(log(eOverP),3)); // in cm/s
    a0=-1.314, a1=0.710,   a2=-0.0497, a3=-0.00582;          // coefficients for diffusion (D/mu)
    Double_t   a4=0.00732, a5=0.000901;
    Double_t dOverMu  = exp(a0+a1*log(eOverP)+a2*pow(log(eOverP),2)+a3*pow(log(eOverP),3)
                        +a4*pow(log(eOverP),4)+a5*pow(log(eOverP),5)); // in volts
    Double_t diffusion = dOverMu*(velocity/fieldStrength);   // in cm^2/s
    velocity = velocity*1.0e-8;        // in mm/ns
    diffusion = diffusion*1.0e-7;      // in mm^2/ns
    SetDriftVelocity(velocity);                              // in mm/ns
    SetDiffusionParameters(diffusion,diffusion); // we assume longitudinalDiffusion==transversalDiffusion
    if(DIGI_DEBUG) cout << "For voltage=" << voltage << " V, pressure=" << pressure
                        << " mbar, and " << gasName << " gas" << endl;
    if(DIGI_DEBUG) cout << "drift velocity=" << velocity << " mm/ns, diffusion parameter is "
                        << diffusion << " mm^2/ns" << endl;
  }
  else if(gasName=="isobutane"){
    Double_t fieldStrength = (voltage/1000.)/(height/10.); // E=V/D, in kV/cm
    Double_t eOverP = fieldStrength/(pressure*0.0009869);  // E/P, in kV/(cm atm).
    if(eOverP<0.1 || eOverP >1.8) cout << "**** NOTE: drift parameters calculated in an extrapolated region! ****" << endl;
    Double_t a0=-0.1441, a1=2.981, a2=0.6421, a3=-0.5853;  // coefficients for velocity
    Double_t velocity = a0 + a1*eOverP + a2*pow(eOverP,2) + a3*pow(eOverP,3); // in cm/mus
    a0=1.2948, a1=-1.7737, a2=0.1617, a3=-0.003537;
    fieldStrength = fieldStrength*1000.;  // for sigma, E is in V/cm
    Double_t logE= log(fieldStrength);
    Double_t sigma0x = exp(a0+a1*logE+a2*pow(logE,2)+a3*pow(logE,3)); // in cm^{1/2}
    Double_t diffusion = 0.5*velocity*pow(sigma0x,2); // D=W*sigma0x^2/2, in cm^2/mus
    velocity = velocity/100.; // in mm/ns
    diffusion= diffusion/10.; // in mm^2/ns
    SetDriftVelocity(velocity);    // in mm/ns
    SetDiffusionParameters(diffusion,diffusion); // we assume longitudinalDiffusion==transversalDiffusion
    if(DIGI_DEBUG) cout << "For voltage=" << voltage << " V, pressure=" << pressure
                        << " mbar, and " << gasName << " gas" << endl;
    if(DIGI_DEBUG) cout << "drift velocity=" << velocity << " mm/ns, diffusion parameter is "
                        << diffusion << " mm^2/ns" << endl;
  }
  else{
    if(DIGI_DEBUG) cout << endl << "drift and diffusion parameters for this gas are not implemented yet!" << endl << endl;
  }
}

Int_t driftManager::CalculatePositionAfterDrift(projectionOnPadPlane* pro) {
  // calculates the position on the pads plane after the electron swarm drift
  Double_t driftDistPre=0.;
  Double_t driftDistPost=0.;
  Double_t rhoPre = sqrt(pro->GetTrack()->GetYPre()*
                    pro->GetTrack()->GetYPre() +
                    pro->GetTrack()->GetXPre()*
                    pro->GetTrack()->GetXPre());
  Double_t rhoPost = sqrt(pro->GetTrack()->GetYPost()*
                     pro->GetTrack()->GetYPost() +
                     pro->GetTrack()->GetXPost()*
                     pro->GetTrack()->GetXPost());

  TRandom *random=new TRandom();
  random->SetSeed(0);
  if(padsGeo->GetEndCapMode()==1) ;
  else{
    if(pro->GetTrack()->GetZPre() <= 0 ||
       pro->GetTrack()->GetZPre() >= 2 * padsGeo->GetZLength() ||
       pro->GetTrack()->GetZPost() <= 0 ||
       pro->GetTrack()->GetZPost() >= 2 * padsGeo->GetZLength()) pro->SetPosition(5); // out of range
    else if(rhoPre  < padsGeo->GetDeltaProximityBeam() ||
            rhoPost < padsGeo->GetDeltaProximityBeam()) pro->SetPosition(1); //if any point is closer to the (0,0,z) than a delta
    else if(rhoPre  < padsGeo->GetSizeBeamShielding() &&
            rhoPost < padsGeo->GetSizeBeamShielding()) pro->SetPosition(2); //if both points lies within the beamShielding
    else if(rhoPre  < padsGeo->GetSizeBeamShielding() ||
            rhoPost < padsGeo->GetSizeBeamShielding()) pro->SetPosition(3); //if one point is within the beamShielding
    else pro->SetPosition(4); //if both points lie outside of beamShielding, still to be checked after as a function of geoType
  }
  if(padsGeo->GetGeoType()==1) { //cylinder
    if(pro->GetPosition() == 4 &&
       rhoPre <= padsGeo->GetRadius() &&
       rhoPost <= padsGeo->GetRadius()) pro->SetPosition(4);
    else  pro->SetPosition(5);
    //if the pads goes out of the gas chamber
    if( pro->GetPosition()==5 ) return 0;
    Double_t phiPre = atan2(pro->GetTrack()->GetYPre(),
                            pro->GetTrack()->GetXPre());
    Double_t phiPost = atan2(pro->GetTrack()->GetYPost(),
                             pro->GetTrack()->GetXPost());
    driftDistPre = padsGeo->GetRadius() - rhoPre;
    if(lorentzAngle==0.) {
      //
      //if no magnetic field, the cloud limits drift to the same point in
      // phiZ plane. The drift time is obtained from the differences in rho
      //
      pro->GetPre()->SetPerp(padsGeo->GetRadius());    //rho is the pad plane rho
      pro->GetPre()->SetPhi(phiPre);                 //phi is not changed
      pro->GetPre()->SetZ(pro->GetTrack()->GetZPre());   //Z is not changed
      pro->SetTimePre(pro->GetTrack()->GetTimePre() + driftDistPre / driftVelocity);
      pro->GetPost()->SetPerp(padsGeo->GetRadius());   //rho is the pad plane rho
      pro->GetPost()->SetPhi(phiPost);               //phi is not changed
      pro->GetPost()->SetZ(pro->GetTrack()->GetZPost()); //Z is not changed
      pro->SetTimePost(pro->GetTrack()->GetTimePost() + (padsGeo->GetRadius() - rhoPost) / driftVelocity);
    }
    else{
      // THIS CASE REQUIRES RETHINKING: B IS USUALLY PARALLEL TO E IN THIS CASE
      // BASICALLY AFFECTS TO THE DRIFT PARAMETERS WITHOUT A LORENTZ ANGLE
      // IS NECCESARY A  B NORMAL TO E  CASE IN CYLINDRICAL GEOMETRY??
      //if the magnetic field is set, the displacement is more complex...
      //NOT YET DONE... SIMPLY COPYING THE PREVIOUS CASE
      //if(DIGI_DEBUG)
      if(DIGI_DEBUG)
        cout <<  "________________________________________________________" << endl
	           << " Output of driftManager::CalculatePositionAfterDrift()" << endl
	           <<  " NOT YET INTRODUCED A CYLINDRIC ACTAR WITH MAGNETIC FIELD!" << endl;
      pro->GetPre()->SetPerp(padsGeo->GetRadius());    //rho is the pad plane rho
      pro->GetPre()->SetPhi(phiPre);                 //phi is not changed
      pro->GetPre()->SetZ(pro->GetTrack()->GetZPre());   //Z is not changed
      pro->SetTimePre(pro->GetTrack()->GetTimePre() + driftDistPre / driftVelocity);
      pro->GetPost()->SetPerp(padsGeo->GetRadius());   //rho is the pad plane rho
      pro->GetPost()->SetPhi(phiPost);               //phi is not changed
      pro->GetPost()->SetZ(pro->GetTrack()->GetZPost()); //Z is not changed
      pro->SetTimePost(pro->GetTrack()->GetTimePost() + (padsGeo->GetRadius() - rhoPost) / driftVelocity);
    }
  }

  if(padsGeo->GetGeoType()==0){  //box
    if(padsGeo->GetEndCapMode()==1){
      Double_t tempY = pro->GetTrack()->GetYPre();                                 //storing old Y
      pro->GetTrack()->SetYPre(pro->GetTrack()->GetZPre()-2*padsGeo->GetYLength());  //move Z to Y
      pro->GetTrack()->SetZPre(-tempY+padsGeo->GetZLength());                      //move Y to Z
      tempY= pro->GetTrack()->GetYPost();
      pro->GetTrack()->SetYPost(pro->GetTrack()->GetZPost()-2*padsGeo->GetYLength());
      pro->GetTrack()->SetZPost(-tempY+padsGeo->GetZLength());
      pro->SetPosition(5);
      //repeating here the general position selection rules after the change of
      //coordinates required to project on the endcaps
      if(pro->GetTrack()->GetZPre() <= 0 ||
         pro->GetTrack()->GetZPre() >= 2 * padsGeo->GetZLength() ||
         pro->GetTrack()->GetZPost() <= 0 ||
         pro->GetTrack()->GetZPost() >= 2 * padsGeo->GetZLength()) pro->SetPosition(5);
      else if(rhoPre < padsGeo->GetDeltaProximityBeam() ||
              rhoPost <padsGeo->GetDeltaProximityBeam()) pro->SetPosition(1);
      else if(rhoPre < padsGeo->GetSizeBeamShielding() &&
              rhoPost < padsGeo->GetSizeBeamShielding()) pro->SetPosition(2);
      else if(rhoPre < padsGeo->GetSizeBeamShielding() ||
              rhoPost < padsGeo->GetSizeBeamShielding()) pro->SetPosition(3);
      else pro->SetPosition(4); //still to be checked after as a function of the geoType
    }

    if(pro->GetPosition() == 4 &&
       pro->GetTrack()->GetYPre() <= 2 * padsGeo->GetYLength() &&
       pro->GetTrack()->GetYPre() >= 0 &&
       pro->GetTrack()->GetXPost() <= padsGeo->GetXLength() &&
       pro->GetTrack()->GetXPost() >= (-padsGeo->GetXLength())) pro->SetPosition(4);
    else  pro->SetPosition(5);
    //if the pads goes out of the gas chamber
    if(pro->GetPosition()==5) return 0;

    driftDistPre = pro->GetTrack()->GetYPre();
    driftDistPost= pro->GetTrack()->GetYPost();

    if(lorentzAngle==0.) {
      //if no magnetic field, the cloud limits drift to the same point in
      // XZ space. The drift time is obtained from the differences in Y
      pro->SetSigmaTransvAtPadPlane(sqrt(driftDistPre*2*transversalDiffusion/driftVelocity));

      pro->GetPre()->SetX(pro->GetTrack()->GetXPre());      //X is not changed
      pro->GetPre()->SetY(0.);          //Y is the pad plane
      pro->GetPre()->SetZ(pro->GetTrack()->GetZPre());      //Z is not changed
      pro->SetTimePre(pro->GetTrack()->GetTimePre() + driftDistPre / driftVelocity);
      pro->GetPost()->SetX(pro->GetTrack()->GetXPost());
      pro->GetPost()->SetY(0.);         //Y is the pad plane
      pro->GetPost()->SetZ(pro->GetTrack()->GetZPost());
      pro->SetTimePost(pro->GetTrack()->GetTimePost()+driftDistPost/ driftVelocity);
    }
    else{
      //if the magnetic field is set, the displacement is more complex...
      driftDistPre = driftDistPre / cos(lorentzAngle);
      Double_t newX = pro->GetTrack()->GetXPre() +
                      (pro->GetTrack()->GetYPre()) * tan(lorentzAngle);
      pro->GetPre()->SetX(newX);                         //X is changed by Lorentz angle
      pro->GetPre()->SetY(0.);       //Y is the pad plane
      pro->GetPre()->SetZ(pro->GetTrack()->GetZPre());   //Z is not changed
      pro->SetTimePre(pro->GetTrack()->GetTimePre() + driftDistPre / driftVelocity);
      newX= pro->GetTrack()->GetXPost() +
            (pro->GetTrack()->GetYPost()) * tan(lorentzAngle);
      pro->GetPost()->SetX(newX);                        //X is changed by Lorentz angle
      pro->GetPost()->SetY(0.); plane
      pro->GetPost()->SetZ(pro->GetTrack()->GetZPost()); //Z is not changed
      pro->SetTimePost(pro->GetTrack()->GetTimePost() +
                       (pro->GetTrack()->GetYPost()/cos(lorentzAngle))/driftVelocity);
    }
  }
  pro->SetSigmaLongAtPadPlane(sqrt(driftDistPre*2*longitudinalDiffusion/driftVelocity));
  pro->SetSigmaTransvAtPadPlane(sqrt(driftDistPre*2*transversalDiffusion/driftVelocity));

  if(DIGI_DEBUG>1)
    cout <<  "________________________________________________________" << endl
         << " Output of driftManager::CalculatePositionAfterDrift()" << endl
	       <<  "pre = (" <<  pro->GetTrack()->GetXPre() << ","
	       <<  pro->GetTrack()->GetYPre() << ","
	       <<  pro->GetTrack()->GetZPre() << ")" << endl
	       <<  " post = (" <<  pro->GetTrack()->GetXPost() << ","
	       <<  pro->GetTrack()->GetYPost() << ","
	       <<  pro->GetTrack()->GetZPost() << ")" << endl
	       << " pre Projected = (" <<   pro->GetPre()->X() << ","
	       <<  pro->GetPre()->Y() << ","
	       <<  pro->GetPre()->Z() << ") timePre = " <<  pro->GetTimePre() << endl
	       <<  " post Projected = (" <<  pro->GetPost()->X() << ","
	       <<  pro->GetPost()->Y() << ","
	       <<  pro->GetPost()->Z() << ") timePost = " << pro->GetTimePost() << endl
	       << "________________________________________________________"<< endl;

  return 1;
}

void driftManager::CalculatePadsWithCharge(projectionOnPadPlane* pro, TClonesArray* clo,
                                           Int_t &numberOfPadsBeforeThisLoopStarted, TTree* T) {
  //
  // Calculates the pads with charge after the electron swarm drift
  //

  Double_t halfPadSize = padsGeo->GetPadSize()/2.;
  Double_t rHexagon   = padsGeo->GetRHexagon();

  TVector3* preOfThisProjection  = pro->GetPre();
  TVector3* postOfThisProjection = pro->GetPost();
  Double_t preOfThisProjectionX  = preOfThisProjection->X();
  Double_t preOfThisProjectionZ  = preOfThisProjection->Z();
  Double_t postOfThisProjectionX = postOfThisProjection->X();
  Double_t postOfThisProjectionZ = postOfThisProjection->Z();

  Int_t initPad = padsGeo->IsInPadNumber(preOfThisProjection);
  Int_t finalPad = padsGeo->IsInPadNumber(postOfThisProjection);
  Int_t initColumn = padsGeo->CalculateColumn(initPad);
  Int_t initRow = padsGeo->CalculateRow(initPad);
  Int_t finalColumn = padsGeo->CalculateColumn(finalPad);
  Int_t finalRow = padsGeo->CalculateRow(finalPad);

  //Int_t numberofpoints=0;
  //TH2F *hist=new TH2F("h2","Padplane",151,0,300,151,-150,150);
  //TGraph *g=new TGraph();
  //TCanvas *c=new TCanvas();

  Double_t strideLength=pro->GetTrack()->GetStrideLength();
  Double_t energyStride=pro->GetTrack()->GetEnergyStride();


  if(DIGI_DEBUG>2)
    cout <<  "________________________________________________________" << endl
         << " Output of driftManager::CalculatePadsWithCharge()" << endl
         << " From (pre) " << initPad  << " (" << initRow << "," << initColumn
         << ") to (post) " << finalPad  << " (" << finalRow << ","
         << finalColumn << ")" << endl
         << " Stride Length " << strideLength << " mm, Energy "
         << 1000*energyStride << " keV" << endl
         << " Initial pos " << preOfThisProjectionX << " "
         << preOfThisProjectionZ << endl
         << " Final pos " << postOfThisProjectionX << " "
         << postOfThisProjectionZ << endl;

  Double_t energyPerPair=GetGasWvalue(); // W value in eV

  //Calculating electrons produced every 0.5 mm
  Int_t nsteps=strideLength/0.5; //0 if below 0.5, ...
  Double_t sigmaTrans=pro->GetSigmaTransvAtPadPlane();
  Double_t sigmaLong=pro->GetSigmaLongAtPadPlane();

  Double_t *EnergyStep;
  Int_t *NumberOfElectrons;
  EnergyStep= new Double_t[nsteps+2];
  NumberOfElectrons=new Int_t[nsteps+2];

  Int_t numberOfRows=padsGeo->GetNumberOfRows();
  Int_t numberOfColumns=padsGeo->GetNumberOfColumns();

  //HAPOL HARDCODED! SOLVE IT! ->HINT: Create a TClonesArray of an object with the data
  Int_t chargeOnPads[151][151]={0};
  Int_t chargeOnPadsAmplified[151][151]={0};
  Int_t chargeOnPadsTotal[151][151]={0};

  /* Int_t **chargeOnPads; */
  /* chargeOnPads=new Int_t*[numberOfRows]; */
  /* Int_t **chargeOnPadsAmplified=new Int_t*[numberOfRows]; */
  /* Int_t **chargeOnPadsTotal=new Int_t*[numberOfRows]; */
  /* for(Int_t nrows=0;nrows<numberOfRows;nrows++){ */
  /*   chargeOnPads[nrows]=new Int_t[numberOfColumns]; */
  /*   chargeOnPadsAmplified[nrows]=new Int_t[numberOfColumns]; */
  /*   chargeOnPadsTotal[nrows]=new Int_t[numberOfColumns]; */
  /* } */

  Int_t *rowList=new Int_t[4000];
  Int_t *columnList=new Int_t[4000];
  //for(Int_t u=0;u<numberOfRows;u++){
  //  for(Int_t k=0;k<numberOfColumns;k++)
  //    chargeOnPads[u][k]=0.;
  //}

  Double_t sumX=0;
  Double_t sumZ=0;
  Int_t electrons_lost=0;

  for(Int_t k=0;k<(nsteps+1);k++){
    //the step is the strideLength if below 0.5 mm and is between 0.5 mm and 1 mm otherwise
    Double_t stepx=(postOfThisProjectionX-preOfThisProjectionX)/(nsteps+1);
    Double_t stepz=(postOfThisProjectionZ-preOfThisProjectionZ)/(nsteps+1);
    EnergyStep[k]=energyStride/(nsteps+1);
    Int_t electrons = 1e6 * EnergyStep[k] / energyPerPair;
    NumberOfElectrons[k]=gRandom->Poisson(electrons);
  }

  Double_t electron_posX = 0;
  Double_t electron_posZ = 0;
  Int_t padRow = 0;
  Int_t padColumn = 0;
  for(Int_t istep=0;istep<=nsteps;istep++){
    for(Int_t u=0;u<numberOfRows;u++){
      for(Int_t k=0;k<numberOfColumns;k++){
        chargeOnPads[u][k]=0.;//Reset electrons on each step
  	    chargeOnPadsAmplified[u][k]=0;
      }
    }
    Double_t strideCenterX= preOfThisProjectionX+(istep+0.5)*stepx;
    Double_t strideCenterZ= preOfThisProjectionZ+(istep+0.5)*stepz;

    //g->SetPoint(numberofpoints,strideCenterZ,strideCenterX);
    //numberofpoints++;

    Double_t energyStride=EnergyStep[istep];
    for(Int_t ielectron=0;ielectron<NumberOfElectrons[istep]; ielectron++){
      electron_posX = gRandom->Gaus(strideCenterX,sigmaTrans); //Better if we also random starting position
      electron_posZ = gRandom->Gaus(strideCenterZ,sigmaTrans);
      padRow = padsGeo->GetPadRowFromXZValue(electron_posX,electron_posZ);
      padColumn = padsGeo->GetPadColumnFromXZValue(electron_posX,electron_posZ);
      if(padRow!=0 && padColumn!=0 && padRow<152 && padColumn<152 ){ //HAPOL SOLVE THE HARDCODED DATA
	      chargeOnPads[padRow-1][padColumn-1]++;
	      chargeOnPadsAmplified[padRow-1][padColumn-1]+=1000*Polya();
      }
      else electrons_lost++;
    }

    for(Int_t u=0;u<numberOfRows;u++){
      for(Int_t k=0;k<numberOfColumns;k++){
	      if(chargeOnPads[u][k]>0){
	         chargeOnPadsTotal[u][k]+=chargeOnPadsAmplified[u][k];
         }
       }
     }
   }//End of Loop on steps

  Int_t padswithsignal=0;
  for(Int_t u=0;u<numberOfRows;u++){
    for(Int_t k=0;k<numberOfColumns;k++){
      if(chargeOnPadsTotal[u][k]>0){
        rowList[padswithsignal]=u+1;
        columnList[padswithsignal]=k+1;
        padswithsignal++;
      }
    }
  }

  Int_t numberOfPadsWithSignal=0;
  Int_t padUnderTest;
  TVector3 centerPad;

  numberOfPadsWithSignal=padswithsignal;

  if(numberOfPadsWithSignal>0) {
    Float_t total_charge=0;
    ActarPadSignal** thePadSignal;
    thePadSignal = new ActarPadSignal*[numberOfPadsWithSignal];

    for(Int_t iterOnPads=0;iterOnPads<numberOfPadsWithSignal;iterOnPads++){
      padUnderTest = padsGeo->CalculatePad(rowList[iterOnPads],columnList[iterOnPads]);

      Float_t charge=chargeOnPadsTotal[rowList[iterOnPads]-1][columnList[iterOnPads]-1];

      total_charge+=charge;
      if(DIGI_DEBUG)
	      cout << rowList[iterOnPads] << " " << columnList[iterOnPads]
             << " ====================>Charge " << charge << endl;
      //hist->SetBinContent(columnList[iterOnPads],rowList[iterOnPads],charge);

      if(DIGI_DEBUG && (rowList[iterOnPads]<0 || columnList[iterOnPads]<0))
	      cout << "something WRONG: (row, column) = (" << rowList[iterOnPads] << "," << columnList[iterOnPads]
             << ")" << ", CHARGE=" <<  charge << "iterOnPads=" << iterOnPads << endl;

      //Let us create and fill as many padSignals as pads in the event
      thePadSignal[iterOnPads] = new ActarPadSignal();
      thePadSignal[iterOnPads]->SetPadNumber(padUnderTest);
      thePadSignal[iterOnPads]->SetPadRow(rowList[iterOnPads]);
      thePadSignal[iterOnPads]->SetPadColumn(columnList[iterOnPads]);
      thePadSignal[iterOnPads]->SetNumberOfStrides(1); //to solve
      thePadSignal[iterOnPads]->SetInitTime(pro->GetTimePre());
      thePadSignal[iterOnPads]->SetFinalTime(pro->GetTimePost());
      //thePadSignal[iterOnPads]->SetSigmaTime(pro->GetSigmaLongAtPadPlane()); //in mm
      thePadSignal[iterOnPads]->SetSigmaTime(pro->GetSigmaLongAtPadPlane()/driftVelocity); //in ns
      //cout<<thePadSignal[iterOnPads]->GetSigmaTime()<<endl;
      thePadSignal[iterOnPads]->SetChargeDeposited(charge);
      thePadSignal[iterOnPads]->SetEventID(pro->GetTrack()->GetEventID());
      thePadSignal[iterOnPads]->SetRunID(pro->GetTrack()->GetRunID());

      new((*clo)[iterOnPads+numberOfPadsBeforeThisLoopStarted])ActarPadSignal(*thePadSignal[iterOnPads]);
      thePadSignal[iterOnPads]->Reset();
    }
    //hist->Draw("colz");
    //g->Draw("*same");
    //c->Update();
    //c->WaitPrimitive();
    numberOfPadsBeforeThisLoopStarted+=numberOfPadsWithSignal;
    if(DIGI_DEBUG){
      cout<<"total charge-->"<<total_charge<<" "<<total_charge/(pro->GetTrack()->GetEnergyStride()*1000)*100<<"% ot total"<<endl;
      cout<<"numberOfPadsBeforeThisLoopStarted "<<numberOfPadsBeforeThisLoopStarted<<endl;
    }
    for (Int_t i=0;i<numberOfPadsWithSignal;i++) delete thePadSignal[i];
    delete thePadSignal;

  }//if numberOfPadsWith Signal>0

  delete[] EnergyStep;
  delete[] NumberOfElectrons;
  delete[] rowList;
  delete[] columnList;
  //delete c;
  //delete hist;
  //delete out;
  //numberOfRows=72;
  /* for (Int_t jj=0; jj<numberOfRows; jj++){ */
  /*   cout<<numberOfRows<<" "<<jj<<" Deleting chargeOnPads ..."<<endl; */
  /*   delete[](chargeOnPads[jj]); */
  /* } */
  /*  for (Int_t jj=0; jj<numberOfRows; jj++){ */
  /*  cout<<numberOfRows<<" "<<jj<<" Deleting chargeOnPadsAmplified..."<<endl; */
  /*  delete[](chargeOnPadsAmplified[jj]); */
  /*  } */
  /*  for (Int_t jj=0; jj<numberOfRows; jj++){ */
  /*  cout<<numberOfRows<<" "<<jj<<" Deleting chargeOnPadsTotal..."<<endl; */
  /*   delete[](chargeOnPadsTotal[jj]); */
  /* } */
  //cout<<"HERE!!"<<endl;
  /* delete[] chargeOnPads; */
  /* delete[] chargeOnPadsAmplified; */
  /* delete[] chargeOnPadsTotal; */
  //cout<<"HERE!!"<<endl;

}

void driftManager::CalculatePadsWithCharge_oldStyle(Double_t k1p, Double_t k2p, Double_t k3p,
                                           Double_t k1n, Double_t k2n, Double_t k3n,
                                           projectionOnPadPlane* pro,
                                           TClonesArray* clo, Int_t &numberOfPadsBeforeThisLoopStarted) {
                                             //TClonesArray* clo, TTree* tree) {
  // calculates the pads with charge after the electron swarm drift
  //
  Double_t halfPadSize = padsGeo->GetPadSize()/2.;
  Double_t rHexagon   = padsGeo->GetRHexagon();

  TVector3* preOfThisProjection  = pro->GetPre();
  TVector3* postOfThisProjection = pro->GetPost();
  Double_t preOfThisProjectionX  = preOfThisProjection->X();
  Double_t preOfThisProjectionZ  = preOfThisProjection->Z();
  Double_t postOfThisProjectionX = postOfThisProjection->X();
  Double_t postOfThisProjectionZ = postOfThisProjection->Z();

  Int_t initPad = padsGeo->IsInPadNumber(preOfThisProjection); //init pad
  Int_t finalPad = padsGeo->IsInPadNumber(postOfThisProjection); //final pad
/*  Int_t initPad = 1;
  Int_t finalPad = 10201;   // for the purpose of range resolution calculation, remove it afterward!*/
  Int_t initColumn = padsGeo->CalculateColumn(initPad);
  Int_t initRow = padsGeo->CalculateRow(initPad);
  Int_t finalColumn = padsGeo->CalculateColumn(finalPad);
  Int_t finalRow = padsGeo->CalculateRow(finalPad);

  Double_t strideCenterX=(preOfThisProjectionX+postOfThisProjectionX)/2.;
  Double_t strideCenterZ=(preOfThisProjectionZ+postOfThisProjectionZ)/2.;

  if(DIGI_DEBUG)
    cout <<  "________________________________________________________" << endl
   << " Output of driftManager::CalculatePadsWithCharge()" << endl
   << " From (pre) " << initPad  << " (" << initRow << "," << initColumn
   << ") to (post) " << finalPad  << " (" << finalRow << ","
   << finalColumn << ")" << endl;

  //calculate the vector between pre and post projections
  TVector3 strideOnPadPlane = postOfThisProjection - preOfThisProjection;
  Double_t alpha = 0; char buffer[1000];
  Double_t sigma = pro->GetSigmaTransvAtPadPlane();

  if(padsGeo->GetGeoType()==0) {//box
    //first, avoid the strides with points with rho<delta which are not
    //well defined in this situation
    if(padsGeo->GetSizeBeamShielding() == 0. ) {
      if( pro->GetPosition() == 5 || pro->GetPosition() == 0) return;
    }
    else
      if( pro->GetPosition() != 4) return;

    alpha = atan2(strideOnPadPlane.X(),strideOnPadPlane.Z());
  }
  else if (padsGeo->GetGeoType()==1) {//cylinder
    //first, avoid the strides with points with rho<delta which are not
    //well defined in this situation
    if( pro->GetPosition() != 4 ) return;
    //In this case we will use the differences of alpha in the pseudoplane [mu,Z]
    //where mu = phi * side
    padPlaneRadius = padsGeo->GetRadius();
    alpha = atan2((postOfThisProjection->Phi()-preOfThisProjection->Phi())*padPlaneRadius,
      strideOnPadPlane.Z());
  }


  if(DIGI_DEBUG) {
    cout <<  "______________________________TF2__________________________" << endl
   << " Output of driftManager::CalculatePadsWithCharge()" << endl
   << " strideOnPadPlane coordinates (" << strideOnPadPlane.x() << ","
   << strideOnPadPlane.y() << "," <<  strideOnPadPlane.z() << endl
   <<  ") with distance  " << strideOnPadPlane.Mag()
   << " and angle w.r.t. Z axis " << alpha << endl;
    if(padsGeo->GetGeoType()==1)
      cout   << "[the angle is calculated from differences in Phi] "<< endl;
  }

  if(padsGeo->GetGeoType()==0) {//box
    if(ampManager->GetIsWire()==0) {
/*      sprintf(buffer,
       "(%f/(2.50663*%f))*exp((-((x-%f)*%f-(y-%f)*%f)*((x-%f)*%f-(y-%f)*%f))/(2*%f*%f))",
        pro->GetTrack()->GetEnergyStride()*1000,sigma,
        pro->GetPre()->Z(),sin(alpha),pro->GetPre()->X(),cos(alpha),
        pro->GetPre()->Z(),sin(alpha),pro->GetPre()->X(),cos(alpha),
        sigma,sigma);*/
      sprintf(buffer,
             "(%f/(2.50663*%f))*exp(-((x-%f)*(x-%f)+(y-%f)*(y-%f))/(2*%f*%f))",
              pro->GetTrack()->GetEnergyStride()*1000,sigma,
              strideCenterZ, strideCenterZ, strideCenterX, strideCenterX,
              sigma,sigma);
    }
    else if(ampManager->GetIsWire()==1){

      Double_t pitchWire = ampManager->GetPitchOfAmpliWire();
      Double_t         L = ampManager->GetACseparation(); // distance between wire and pads plane
      Double_t zWire =        (preOfThisProjectionZ + postOfThisProjectionZ)/2.;
      Double_t xWire = Int_t(((preOfThisProjectionX + postOfThisProjectionX)/2.)/pitchWire+0.5)*pitchWire;

//       cout << "end point z=" << pro->GetPost()->Z() << ", x=" << pro->GetPost()->X()
//            << ", y=" << pro->GetPost()->Y() << ", removable diagnoisis output, dypang 09051627" << endl;

      sprintf(buffer,
             "%f*%f*((1.-pow(tanh(%f*abs(%f-x)/%f),2.))/(1.+%f*pow(tanh(%f*abs(%f-x)/%f),2.)))*%f*((1.-pow(tanh(%f*abs(%f-y)/%f),2.))/(1.+%f*pow(tanh(%f*abs(%f-y)/%f),2.)))",
              pro->GetTrack()->GetEnergyStride()*100.,
              k1p, k2p, zWire, L, k3p, k2p, zWire, L,
              k1n, k2n, xWire, L, k3n, k2n, xWire, L
              ); // formula of E. Mathieson
//       sprintf(buffer,
//              "%f*(%f/pow(%f*%f+(%f-x)*(%f-x)+(%f-y)*(%f-y),1.5)-(3.*%f)/pow( 9.*%f*%f+(%f-x)*(%f-x)+(%f-y)*(%f-y),1.5)+(5.*%f)/pow(25.*%f*%f+(%f-x)*(%f-x)+(%f-y)*(%f-y),1.5)-(7.*%f)/pow(49.*%f*%f+(%f-x)*(%f-x)+(%f-y)*(%f-y),1.5)+(9.*%f)/pow(81.*%f*%f+(%f-x)*(%f-x)+(%f-y)*(%f-y),1.5)+(11.*%f)/pow(121.*%f*%f+(%f-x)*(%f-x)+(%f-y)*(%f-y),1.5)+(13.*%f)/pow(169.*%f*%f+(%f-x)*(%f-x)+(%f-y)*(%f-y),1.5))",
//               pro->GetTrack()->GetEnergyStride(),L,L,L,
//               zWire,zWire,xWire,xWire,L,L,L,
//               zWire,zWire,xWire,xWire,L,L,L,
//               zWire,zWire,xWire,xWire,L,L,L,
//               zWire,zWire,xWire,xWire,L,L,L,
//               zWire,zWire,xWire,xWire,L,L,L,
//               zWire,zWire,xWire,xWire,L,L,L,
//               zWire,zWire,xWire,xWire,L,L,L
//               ); // formula used by Thomas and Manuel

/*       cout << "Pre(" << pro->GetPre()->X() << "," << pro->GetPre()->Z() << ")" << endl;
       cout << "Post(" << pro->GetPost()->X() << "," << pro->GetPost()->Z() << ")" << endl;*/
    }
  }
  else if (padsGeo->GetGeoType()==1) {//cylinder
    sprintf(buffer,
      "(%f/(2.50663*%f))*exp((-((x-%f)*%f-(y-%f)*%f)*((x-%f)*%f-(y-%f)*%f))/(2*%f*%f))",
      pro->GetTrack()->GetEnergyStride()*1000,sigma,
      preOfThisProjectionZ,sin(alpha),preOfThisProjection->Phi()*padPlaneRadius,cos(alpha),
      preOfThisProjectionZ,sin(alpha),preOfThisProjection->Phi()*padPlaneRadius,cos(alpha),
      sigma,sigma);
  }


  if(DIGI_DEBUG>2)
    cout << endl << " Function to integrate " << buffer << endl;

  TF2 *f2=0;
  if(padsGeo->GetGeoType()==0) {//box
    if(ampManager->GetIsWire()==0){
      f2 = new TF2("f2",buffer,
                 strideCenterZ-10*sigma,
                 strideCenterZ+10*sigma,
                 strideCenterX-10*sigma,
                 strideCenterX+10*sigma);
      }//adjust the number before the sigma!
    else if(ampManager->GetIsWire()==1){ // wire amplification
/*      f2 = new TF2("f2",buffer,
                 pro->GetPre()->Z()-2.*ampManager->GetACseparation(),
                 pro->GetPre()->Z()+2.*ampManager->GetACseparation(),
                 pro->GetPre()->X()-2.*ampManager->GetACseparation(),
                 pro->GetPre()->X()+2.*ampManager->GetACseparation());*/
      f2 = new TF2("f2",buffer); // xmin, xmax, ymin, ymax actually do not matter.
      }
  }
  else if (padsGeo->GetGeoType()==1) {//cylinder
    f2 =  new TF2("f2",buffer,
      preOfThisProjectionZ-10*sigma,
      preOfThisProjectionZ+10*sigma,
      preOfThisProjection->Phi()*padPlaneRadius-10*sigma,
      preOfThisProjection->Phi()*padPlaneRadius+10*sigma);//adjust the number before the sigma!
  }

  //Swap the initial an final row and columns if track goes back
  if(initRow>finalRow){
    Int_t tempRow = initRow;
    initRow = finalRow;
    finalRow = tempRow;
  }
  if(initColumn>finalColumn){
    Int_t tempColumn = initColumn;
    initColumn = finalColumn;
    finalColumn = tempColumn;
  }

  //we need to know which pads are going to be checked!
  //IDEA! Use the vector to determine the conditions to take a pad:
  //  1 - scalar product(strideOnPadPlane,padCenter-pro->GetPre())>0
  //  2 - scalar product(strideOnPadPlane,padCenter-pro->GetPost())<0

  Int_t securityFactor = (Int_t)(2*sigma/padsGeo->GetPadSize());
  if(securityFactor<1) securityFactor = 1;

//      Int_t securityFactor=5;

  //TO BE IMPROVED... TESTING TOO MUCH PADS
  Int_t rowsUnderTest    = (finalRow   +securityFactor+1) - (initRow   -securityFactor);
  Int_t columnsUnderTest = (finalColumn+securityFactor+1) - (initColumn-securityFactor);

  if(DIGI_DEBUG)
    cout <<  "________________________________________________________" << endl
         << " Output of driftManager::CalculatePadsWithCharge()" << endl
         << " securityFactor " << securityFactor
         << ", rowsUnderTest  " << rowsUnderTest  << " (from "
         << initRow-securityFactor << " to " << initRow-securityFactor+rowsUnderTest
         << "), columnsUnderTest " << columnsUnderTest
         << " (from " << initColumn-securityFactor << " to "
         << initColumn-securityFactor+rowsUnderTest << ")" << endl;

  Double_t charge=0; Int_t numberOfPadsWithSignal=0;
  Int_t padUnderTest; TVector3 centerPad;
  Int_t rowList[40000]={0}; Int_t columnList[40000]={0};
  Int_t rowNumber=0, colNumber=0;
  for(Int_t i = 0;i<rowsUnderTest;i++){
    for(Int_t j = 0;j<columnsUnderTest;j++){
      //interval 2D [ax,bx][ay,by]
      padUnderTest = padsGeo->CalculatePad(initRow-securityFactor+i,
             initColumn-securityFactor+j);
      centerPad = padsGeo->CoordinatesCenterOfPad(padUnderTest);
      if(DIGI_DEBUG){
        cout <<  "________________________________________________________" << endl
             << " Output of driftManager::CalculatePadsWithCharge()" << endl
             << " scalar with pre " << strideOnPadPlane.Dot(centerPad-(*(pro->GetPre())))
             << " scalar with post " << strideOnPadPlane.Dot(centerPad-(*(pro->GetPost())))
             << endl;
      }

      rowNumber = initRow-securityFactor+i;
      colNumber = initColumn-securityFactor+j;

      if(   rowNumber>=1 && rowNumber <= padsGeo->GetNumberOfRows()
         && colNumber>=1 && colNumber <= padsGeo->GetNumberOfColumns()){
        rowList[numberOfPadsWithSignal] = rowNumber;
        columnList[numberOfPadsWithSignal] = colNumber;
        numberOfPadsWithSignal++;
      }
    }
  }
  if(DIGI_DEBUG)
    cout <<  "________________________________________________________" << endl
   << " Output of driftManager::CalculatePadsWithCharge()" << endl
   << "  numberOfPadsWithSignal = " << numberOfPadsWithSignal<<endl;

  Double_t phiPad, xPad, zPad;

  if( numberOfPadsWithSignal>0) {
    ActarPadSignal** thePadSignal;
    thePadSignal = new ActarPadSignal*[numberOfPadsWithSignal];

    for(Int_t iterOnPads=0;iterOnPads<numberOfPadsWithSignal;iterOnPads++){
      padUnderTest = padsGeo->CalculatePad(rowList[iterOnPads],columnList[iterOnPads]);
      centerPad = padsGeo->CoordinatesCenterOfPad(padUnderTest);
      if(DIGI_DEBUG)
  cout <<  "________________________________________________________" << endl
       << " Output of driftManager::CalculatePadsWithCharge()" << endl
       << " Calculating charge for pad " << padUnderTest << " ("
       << rowList[iterOnPads] << "," << columnList[iterOnPads] << ")" << endl;

      phiPad = centerPad.Phi();
      xPad = centerPad.X();
      zPad = centerPad.Z();

      if(padsGeo->GetGeoType()==0) {//box
        if(padsGeo->GetPadType()==0){  //square pad
          charge = f2->Integral(zPad-halfPadSize,zPad+halfPadSize,xPad-halfPadSize,xPad+halfPadSize);
// in this integration, the x, and y values should be relative to the center pad.
        }
  else if(padsGeo->GetPadType()==1){  //hexagonal pad
          if(padsGeo->GetPadLayout()==0){ // MAYA-type layout
            charge = f2->Integral(zPad-rHexagon,
                                  zPad+rHexagon,
                                  xPad-1.5*halfPadSize,
                                  xPad+1.5*halfPadSize);
          }
          else if(padsGeo->GetPadLayout()==1){
            charge = f2->Integral(zPad-1.5*halfPadSize,
                                  zPad+1.5*halfPadSize,
                                  xPad-rHexagon,
                                  xPad+rHexagon);
          }
        }
      }
      else if (padsGeo->GetGeoType()==1) {//cylinder
  if(padsGeo->GetPadType()==0)  //square pad
    charge = f2->Integral(zPad-halfPadSize,
        zPad+halfPadSize,
        (phiPad*padPlaneRadius)-halfPadSize,
        (phiPad*padPlaneRadius)+halfPadSize);
  else if(padsGeo->GetPadType()==1)  //hexagonal pad
    charge = f2->Integral(zPad-1.5*halfPadSize,
        zPad+1.5*halfPadSize,
        (phiPad*padPlaneRadius)-rHexagon,
        (phiPad*padPlaneRadius)+rHexagon);
      }

      if(DIGI_DEBUG && (rowList[iterOnPads]<0 || columnList[iterOnPads]<0))
        cout << "some thing WRONG: (row, column) = ("    << rowList[iterOnPads] << ","
             << columnList[iterOnPads] << ")" << ", CHARGE="<<  charge << "iterOnPads=" << iterOnPads << endl;

      //Let us create and fill as many padSignals as pads in the event
      thePadSignal[iterOnPads] = new ActarPadSignal();
      thePadSignal[iterOnPads]->SetPadNumber(padUnderTest);
      thePadSignal[iterOnPads]->SetPadRow(rowList[iterOnPads]);
      thePadSignal[iterOnPads]->SetPadColumn(columnList[iterOnPads]);
      thePadSignal[iterOnPads]->SetNumberOfStrides(1); //to solve
      thePadSignal[iterOnPads]->SetInitTime(pro->GetTimePre());
      thePadSignal[iterOnPads]->SetFinalTime(pro->GetTimePost());
      thePadSignal[iterOnPads]->SetSigmaTime(pro->GetSigmaLongAtPadPlane());
      thePadSignal[iterOnPads]->SetChargeDeposited(charge);
      thePadSignal[iterOnPads]->SetEventID(pro->GetTrack()->GetEventID());
      thePadSignal[iterOnPads]->SetRunID(pro->GetTrack()->GetRunID());

      new((*clo)[iterOnPads+numberOfPadsBeforeThisLoopStarted])ActarPadSignal(*thePadSignal[iterOnPads]);
      thePadSignal[iterOnPads]->Reset();

    }

    numberOfPadsBeforeThisLoopStarted+=numberOfPadsWithSignal;

    delete f2;
    for (Int_t i=0;i<numberOfPadsWithSignal;i++) delete thePadSignal[i];
    delete thePadSignal;

  }
}

void driftManager::GetStatus(void){
  cout << "Connected to geometry "<< padsGeo << endl
       <<"with longitudinalDiffusion = " << longitudinalDiffusion
       <<", transversalDiffusion = " << transversalDiffusion << endl
       <<"driftVelocity = " << driftVelocity
       <<", magneticField = " << magneticField << endl;
}
