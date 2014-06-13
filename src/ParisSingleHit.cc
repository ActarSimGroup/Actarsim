//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//

#include "ParisSingleHit.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4UnitsTable.hh"

G4Allocator<ParisSingleHit> ParisSingleHitAllocator;

ParisSingleHit::ParisSingleHit()
{;}

ParisSingleHit::~ParisSingleHit()
{;}

ParisSingleHit::ParisSingleHit(const ParisSingleHit &from) : G4VHit()
{
	if ( this == &from ) return;

	edep = from.edep;	
	pos  = from.pos;
	trackID = from.trackID;
	parentID = from.parentID;
	primaryID = from.primaryID;		
	detID = from.detID;
	motherID = from.motherID;

	ToF = from.ToF;
	processName  = from.processName;
	particleName = from.particleName;	
	detName = from.detName;
	motherDetName = from.motherDetName;
	PDGcode = from.PDGcode;
	
}

const ParisSingleHit& ParisSingleHit::operator=(const ParisSingleHit &from)
{
	if ( this == &from ) return (*this);
	
	edep = from.edep;
	pos  = from.pos;
	trackID = from.trackID;
	parentID = from.parentID;
	primaryID = from.primaryID;			
	detID = from.detID;
	motherID = from.motherID;

	ToF = from.ToF;
	processName = from.processName;
	particleName = from.particleName;	
	detName = from.detName;
	motherDetName = from.motherDetName;
	PDGcode = from.PDGcode;
	
	return (*this);
}

G4int ParisSingleHit::operator==(const ParisSingleHit &from) const
{
	return (this==&from) ? 1 : 0;
}

void ParisSingleHit::Draw()
{
	G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
	if(pVVisManager)
	{
		G4Circle circle(pos);
		circle.SetScreenSize(0.04); circle.SetFillStyle(G4Circle::filled);
		
		G4Colour colour(1.,0.,0.);
		G4VisAttributes attribs(G4Colour(1.,0.,0.)); circle.SetVisAttributes(attribs);
		pVVisManager->Draw(circle);
	}
}

void ParisSingleHit::Print()
{

	G4cout << "--------------- ParisSingleHit::Print() --------------------- " << G4endl;
	G4cout << "trackID: " << trackID 
		<< ", parentID: " << parentID
		<< ", primaryID: " << primaryID	
	 	<< ", particleName: " << particleName 
		<< ", PDG: " << PDGcode
	 	<< ", processName: " << processName 
		<< G4endl
	 	<< "detID: " << detID 
	 	<< ", detName: " << detName 
		<< ", motherID: " << motherID 
	 	<< ", motherDetName: " << motherDetName << G4endl; 
	G4cout << "edep: " << edep / keV << " keV"
	 	<< ", pos: " << pos 
	 	<< ", ToF: " << ToF  / ns << " ns" << G4endl; 
	G4cout << "--------------------------------------------------------------" << G4endl;
}


