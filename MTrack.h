//#ifndef MTRACK_H
//#define MTRACK_H

#include <stdio.h>
#include <iostream>
#include <TLine.h>
//#include <Parameters.h>

using namespace std;

class MTrack {
	public:
	MTrack();
	//MTrack(char*);
	~MTrack();
	void ResetLines();

	Int_t zx_s;
	Int_t zx_e;
	Int_t zy_s;
	Int_t zy_e;

	Double_t Xm;
	Double_t Ym;
	Double_t Zm;
	Double_t Xh;
	Double_t Yh;
	Double_t Zh;

	TLine* L2DXY;
	TLine* L2DXZ;
	TLine* L2DYZ;
	TLine* L3D;

	//Double_t ElossTable[NPADX][2];
};

MTrack::MTrack() {
	L2DXY=new TLine();
	L2DXZ=new TLine();
	L2DYZ=new TLine();
	L3D=new TLine();
}
/*
MTrack::MTrack(char* path)
{
	L2DXY=new TLine();
	L2DXZ=new TLine();
	L2DYZ=new TLine();
	L3D=new TLine();

	FILE* f=fopen(path,"r");
	if(f!=NULL)
	{
		int i=0;
		while(fscanf(f,"%d %d",&ElossTable[i][0],&ElossTable[i][1])!=EOF)
		{
			i++;
			if(i>63) break;
		}
		fclose(f);
	}
	else cout << "Eloss File " << path << " doesn't exist... Skipping Eloss Table." << endl;
}
*/

MTrack::~MTrack() {
	delete L2DXY;
	delete L2DXZ;
	delete L2DYZ;
	delete L3D;
	//for(int i=0;i<NPADX;i++)
	//	delete ElossTable[i];
	//delete [] ElossTable;
}

void MTrack::ResetLines() {
	L2DXY->SetX1(-1);
	L2DXY->SetY1(-1);
	L2DXY->SetX2(-1);
	L2DXY->SetY2(-1);

	L2DXZ->SetX1(-1);
	L2DXZ->SetY1(-1);
	L2DXZ->SetX2(-1);
	L2DXZ->SetY2(-1);

	L2DYZ->SetX1(-1);
	L2DYZ->SetY1(-1);
	L2DYZ->SetX2(-1);
	L2DYZ->SetY2(-1);

	L3D->SetX1(-1);
	L3D->SetY1(-1);
	L3D->SetX2(-1);
	L3D->SetY2(-1);
}
//#endif
