// HodoParametrisation.cc 
// Sunyelei  ËïÒ¶ÀÚ
//email:sunyp09@gmail.com
#include "HodoParametrisation.hh" 

#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
HodoParametrisation::HodoParametrisation(G4int  NodE, G4int Noe1,G4int Noe2,G4double zpos)
{
	fNoDE=NodE;
	fNoE1=Noe1;
	fNoE2=Noe2;
	fzpos=zpos;
	fzspacing[0]=107*mm;//Gap Between DE and E1
	fzspacing[1]=22*mm;//gap Between E1 and E2

	DEHodoPos[0]=-45.605*cm;//DE position in x-axis
	DEHodoPos[1]=-35.505*cm;
	DEHodoPos[2]=-25.405*cm;
	DEHodoPos[3]=-15.305*cm;
	DEHodoPos[4]=-8.205 *cm;
	DEHodoPos[5]=-4.105*cm;
	DEHodoPos[6]=0.0*cm;
	DEHodoPos[7]=4.105*cm;
	DEHodoPos[8]=8.205*cm;
	DEHodoPos[9]=15.305*cm;
	DEHodoPos[10]=25.405*cm;
	DEHodoPos[11]=35.505*cm;
	DEHodoPos[12]=45.605*cm;

	E1HodoPos[0]=-456.*mm;//E1 position in y-axis
	E1HodoPos[1]=-380.*mm;
	E1HodoPos[2]=-304.*mm;
	E1HodoPos[3]=-228.*mm;
	E1HodoPos[4]=-152.*mm;
	E1HodoPos[5]=-94.5*mm;
	E1HodoPos[6]=-56.5*mm;
	E1HodoPos[7]=-18.5*mm;
	E1HodoPos[8]=19.5*mm;
	E1HodoPos[9]=57.5*mm;
	E1HodoPos[10]=95.5*mm;
	E1HodoPos[11]=153.*mm;
	E1HodoPos[12]=229.*mm;
	E1HodoPos[13]=305.*mm;
	E1HodoPos[14]=381.*mm;
	E1HodoPos[15]=457.*mm;

	E2HodoPos[0] = -456 *mm;//E2 position in y-axis
	E2HodoPos[1] = -380 *mm;
	E2HodoPos[2] = -304 *mm;
	E2HodoPos[3] = -228 *mm;
	E2HodoPos[4] = -152 *mm;
	E2HodoPos[5] = -76  *mm;
	E2HodoPos[6] = 0.0  *mm;
	E2HodoPos[7] = 76   *mm;
	E2HodoPos[8] = 152  *mm;
	E2HodoPos[9] = 228  *mm;	  
	E2HodoPos[10] = 304 *mm; 
	E2HodoPos[11] = 380 *mm; 
	E2HodoPos[12] = 456 *mm;

	DEwidth1=100.*mm;DEwidth2=40.0*mm;
	DEthick1=5.0*mm;  DEthick2=5.5*mm;

	E1width1=75.0*mm; E1width2=38.0*mm;
	E1thick=60.0*mm;

	E2width=75.0*mm; E2thick=60.0*mm;

	Length=100.0*cm;

	if( fNoDE > 13||fNoE1>16||fNoE2>13 )
	  //{ G4Exception("scsiParameterisation construction: NoScintBars >82");}
	  { G4Exception("","",JustWarning,"scsiParameterisation construction: NoScintBars >82");}

}

HodoParametrisation::~HodoParametrisation()
{;}

void HodoParametrisation::ComputeTransformation
(const G4int copyNo,G4VPhysicalVolume *physVol) const
{
	G4double Xposition=0.,Yposition=0.,Zposition=0.;

	//For DE positon  De certer is the Hodo chamber center
	if(copyNo<fNoDE){
		Xposition=DEHodoPos[copyNo];
		Yposition=0.0*cm;
		Zposition=fzpos;
	}
	//For E1 position
	else if(copyNo>(fNoDE-1)&&copyNo<(fNoDE+fNoE1)){
		Xposition=0.0*cm;
		Yposition=E1HodoPos[copyNo-fNoDE];
		Zposition=fzpos+fzspacing[0]+DEthick1/2+E1thick/2;
	}
	//For E2 position
	else if(copyNo>(fNoDE+fNoE1-1)&&copyNo<(fNoDE+fNoE1+fNoE2)){
		Xposition=0.0*cm;
		Yposition=E2HodoPos[copyNo-fNoDE-fNoE1];
		Zposition=fzpos+fzspacing[0]+fzspacing[1]+E1thick/2+E2thick/2;
	}

	G4ThreeVector origin(Xposition,Yposition,Zposition);
	physVol->SetTranslation(origin);
	physVol->SetRotation(0);
}

void HodoParametrisation::ComputeDimensions
(G4Box& ScintBar, const G4int copyNo, const G4VPhysicalVolume*) const
{
	G4double XHalfLength=0.,YHalfLength=0.,ZHalfLength=0.;

	//For DE dimension
	if(copyNo<4||(copyNo>8&&copyNo<13)){
		XHalfLength=0.5*DEwidth1;
		YHalfLength=0.5*Length;
		ZHalfLength=0.5*DEthick1;
	}
	if(copyNo>3&&copyNo<9){
		XHalfLength=0.5*DEwidth2;
		YHalfLength=0.5*Length;
		ZHalfLength=0.5*DEthick2;
	}
	//For E1 dimension
	else if((copyNo>12&&copyNo<18)||(copyNo>23&&copyNo<29)){
		XHalfLength=0.5*Length;
		YHalfLength=0.5*E1width1;
		ZHalfLength=0.5*E1thick;
	}
	else if(copyNo>17&&copyNo<24){
		XHalfLength=0.5*Length;
		YHalfLength=0.5*E1width2;
		ZHalfLength=0.5*E1thick;
	}
	//For E2 dimension
	else if(copyNo>28&&copyNo<42){
		XHalfLength=0.5*Length;
		YHalfLength=0.5*E2width;
		ZHalfLength=0.5*E2thick;
	}
	ScintBar.SetXHalfLength(XHalfLength);
	ScintBar.SetYHalfLength(YHalfLength);
	ScintBar.SetZHalfLength(ZHalfLength);
}
