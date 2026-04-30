//****************************************************************************
//*                   This file is part of PandaRoot.                        *
//*                                                                          *
//*            PandaRoot is distributed under the terms of the               *
//*              GNU General Public License (GPL) version 3,                 *
//*                 copied verbatim in the file "LICENSE".                   *
//*                                                                          *
//*  Copyright (C) 2006 - 2024 FAIR GmbH and copyright holders of PandaRoot  *
//*     The copyright holders are listed in the file "COPYRIGHTHOLDERS".     *
//*               The authors are listed in the file "AUTHORS".              *
//****************************************************************************

/////////////////////////////////////////////////////////////
//
//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	EMC Digi.
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI		
//
// Author List:
//	Xiaorong Shi            Lawrence Livermore National Lab
//	Stephen J. Gowdy        University of Edinburgh
// Dima Melnichuk - adaption for PANDA
//
// Copyright Information:
//	Copyright (C) 1996	Lawrence Livermore National Lab
//
/////////////////////////////////////////////////////////////// 

#include "PndEmcDigi.h"
#include "PndEmcSharedDigi.h"
#include "PndEmcMapper.h"
#include "PndEmcStructure.h"
#include "PndEmcXtal.h"
#include "PndEmcDataTypes.h"
#include "PndDetectorList.h"
#include <iostream>

#include "FairRootManager.h"
#include "FairLogger.h"

#include "TClass.h"
#include "TVector3.h"
#include "TBuffer.h"
#include "Riostream.h"

using namespace std;
TClonesArray *PndEmcDigi::fDigiArrayTBD = nullptr;
// -----   Default constructor   -----------------------------------
PndEmcDigi::PndEmcDigi():FairTimeStamp(0),flocal_time(0),fEnergy(0),fTrackId(-1),fDetectorId(-1),fHitIndex(-1),fPileIndex(-1), fWhere(0,0,0), fThetaInd(0), fPhiInd(0),fTheta(0), fPhi(0), fEvtNo(-1), fDCid(0)
{
}
// -----------------------------------------------------------------


// -----   Destructor   --------------------------------------------
PndEmcDigi::~PndEmcDigi() {}
// -----------------------------------------------------------------
  
PndEmcDigi::PndEmcDigi(Int_t trackid, Int_t id, Float_t energy, Float_t time, Float_t local_time /*check of Tres*/, Int_t hitIndex, Int_t pileIndex, Int_t DCid):FairTimeStamp(time),flocal_time(local_time),fEnergy(energy),fTrackId(trackid),fDetectorId(id),fHitIndex(hitIndex),fPileIndex(pileIndex),fWhere(0,0,0), fThetaInd(0), fPhiInd(0),fTheta(0), fPhi(0),fDCid(DCid)
{
	PndEmcMapper *emcMap=PndEmcMapper::Instance();
	PndEmcTwoCoordIndex* tci=emcMap->GetTCI(id);
	fThetaInd=tci->XCoord();
	fPhiInd=tci->YCoord();

	PndEmcTciXtalMap const &tciXtalMap=PndEmcStructure::Instance()->GetTciXtalMap();
	PndEmcXtal* xtal = tciXtalMap.find(tci)->second;
	fWhere = algPointer()(xtal);
	fTheta = fWhere.Theta();
	fPhi = fWhere.Phi();
	SetLink(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), "EmcHit", hitIndex));
	fEvtNo = FairRootManager::Instance()->GetEntryNr(); //
}

//
// From another Digi
//

PndEmcDigi::PndEmcDigi( const PndEmcDigi& other ) 
: FairTimeStamp(other),flocal_time(other.flocal_time), fEvtNo(other.fEvtNo),
  fEnergy( other.fEnergy ),
  fTrackId(other.fTrackId),
  fDetectorId( other.fDetectorId),
  fHitIndex( other.fHitIndex),fPileIndex( other.fPileIndex),
  fDCid( other.fDCid),
  fThetaInd(0), fPhiInd(0),fTheta(0),fPhi(0), fWhere(0,0,0)
{
	PndEmcMapper *emcMap=PndEmcMapper::Instance();
	PndEmcTwoCoordIndex* tci=emcMap->GetTCI(fDetectorId);
	fThetaInd=tci->XCoord();
	fPhiInd=tci->YCoord();

	PndEmcTciXtalMap const &tciXtalMap=PndEmcStructure::Instance()->GetTciXtalMap();
	PndEmcXtal* xtal = tciXtalMap.find(tci)->second;
	fWhere = algPointer()(xtal);
	fTheta = fWhere.Theta();
	fPhi = fWhere.Phi();
}

void PndEmcDigi::InitDigiArrayTBD()
{
  if (fDigiArrayTBD == nullptr)
    fDigiArrayTBD = new TClonesArray("PndEmcDigi");
}

void PndEmcDigi::SetDetectorId(Int_t id)
{
// PndEmcMapper assumed to instantiate first time with correct parameter before
	PndEmcMapper *emcMap=PndEmcMapper::Instance();
	PndEmcTwoCoordIndex* tci=emcMap->GetTCI(id);
	fThetaInd=tci->XCoord();
	fPhiInd=tci->YCoord();
	fDetectorId=id;

	fThetaInd=tci->XCoord();
	fPhiInd=tci->YCoord();

	PndEmcTciXtalMap const &tciXtalMap=PndEmcStructure::Instance()->GetTciXtalMap();
	PndEmcXtal* xtal = tciXtalMap.find(tci)->second;
	fWhere = algPointer()(xtal);
	fTheta = fWhere.Theta();
	fPhi = fWhere.Phi();

}


//in case of a timebased simulation framework, a crystal might be counted twice in a cluster.
//In order to use subsequent eventbased algorithms, use AbsorbEnergy to merge these.
void PndEmcDigi::AbsorbEnergy(PndEmcDigi& otherDigi) {
	if(otherDigi.equal(this)) {
		fEnergy += otherDigi.GetEnergy();
	} else {
		std::cerr << "-E- in PndEmcDigi::AbsorbEnergy Mismatch in DetectorId. Cannot absorb energy." << std::endl; 
	}
}


TVector3
PndEmcDigi::surfacePosition( const PndEmcXtal* xtal )
{
  return xtal->frontCentre();
}

TVector3
PndEmcDigi::depthPosition( const PndEmcXtal* xtal )
{
	//cout << "depth" << endl;
	
	TVector3 pos = xtal->frontCentre();
	TVector3 norm = xtal->axisVector();
	int module=xtal->myIndex()->Index()/100000000;
	
	if (module==5) //shahslyk
		norm*=fPositionDepthShashlyk;
	else
		norm*=fPositionDepthPWO;
	
	pos += norm;
	
	return pos;
}

void
PndEmcDigi::selectDigiPositionMethod( PositionMethod alg, 
				double positionDepthPWO,
				double positionDepthShashlyk,
				double rescaleFactor)
{

  TVector3 (*algorithm)(const PndEmcXtal *) = nullptr;

  switch ( alg )
	{
		case surface:
			algorithm = PndEmcDigi::surfacePosition;
			fRescaleFactor = rescaleFactor;
			fPositionDepthPWO = positionDepthPWO;
			fPositionDepthShashlyk = positionDepthShashlyk;
			break;
	
		case depth:
			algorithm = PndEmcDigi::depthPosition;
			fRescaleFactor = rescaleFactor;
			fPositionDepthPWO = positionDepthPWO;
			fPositionDepthShashlyk = positionDepthShashlyk;
			break;
	
		default:
			cout << "PndEmcDigi::selectDigiPositionMethod. " 
					<< "Attempted to select unknown digi position method." 
					<< endl;
	}
	
	// Now actually set the pointer
	algPointer() = algorithm;
}

//Set the default digi position method

TVector3 ( *&PndEmcDigi::algPointer() ) ( const PndEmcXtal* )
{
  static TVector3 (*pointer) ( const PndEmcXtal* ) = PndEmcDigi::depthPosition;

  return pointer;
}

double PndEmcDigi::fRescaleFactor = 1.0;
double PndEmcDigi::fPositionDepthPWO = 6.2;
double PndEmcDigi::fPositionDepthShashlyk = 20.9;


bool PndEmcDigi::isNeighbour( const PndEmcDigi* theDigi ) const
{
	PndEmcMapper::Instance(); //: unused variable?PndEmcMapper *emcMap= //[R.K.03/2017] unused variable
	PndEmcTwoCoordIndex* tci=this->GetTCI();

  if ((theDigi->GetTCI() == 0) || (tci == nullptr)) {
    std::cout<<"TwoCoordIndex of digi is not defined"<<std::endl;
		abort();
  }

  if (tci->IsNeighbour(theDigi->GetTCI())) return true;

	return false;
}

PndEmcTwoCoordIndex* PndEmcDigi::GetTCI() const
{
	PndEmcMapper *emcMap=PndEmcMapper::Instance();
	PndEmcTwoCoordIndex* tci=emcMap->GetTCI(fDetectorId);
	return tci;
};

Short_t PndEmcDigi::GetXPad() const {
  // Return the X pad value for clusterization
  
  // Barrel EMC
  if (GetModule()==1 || GetModule()==2)
    return (GetCrystal()+(GetCopy()-1)*10);
  
  // BwEndCap and forward EMC
  if ((GetModule()==4) || (GetModule()==5))
  {
    if (GetCopy()==1) {  return -GetRow()+1;}
    if (GetCopy()==2) {  return -GetRow()+1; }
    if (GetCopy()==3) {  return  GetRow();}
    if (GetCopy()==4) {  return  GetRow();}
  }
  
  // FwEndCap
  if (GetModule()==3)
    return -(GetCrystal()-36);//the minus sign before the paranthesis
			      //is introduced since the geometry of
			      //FwEndCap gets rotated by 180 deg
			      //around the y-axis in PndEmc.cxx;
                               
  //this rotation was done in turn due to the way the geometry was
  //defined in the geometry file of the forward end cap
    
  /*
    if (GetModule()==3 && GetCrystal()==999 && GetRow()==999)
    return GetCrystal();
  */
  
  // Test EMC
  if (GetModule()==6)
    return  GetRow();
  
 return -1000; // failure
}

Short_t PndEmcDigi::GetYPad() const {
  // Return the Y pad value for clusterization
  
  // Barrel EMC
  if (GetModule()==1)
    return (GetRow()+29);
  
  if (GetModule()==2)
    return (-GetRow()+30);
  
  // BwEndCap and forward EMC
  if ((GetModule()==4) || (GetModule()==5))
  {
    if (GetCopy()==1) {  return GetCrystal(); }
    if (GetCopy()==2) {  return -GetCrystal() + 1; }
    if (GetCopy()==3) {  return -GetCrystal() + 1; }
    if (GetCopy()==4) {  return GetCrystal(); }
  }
  
  // FwEndCap
  if (GetModule()==3)
    return (GetRow()- 37);
    
  /*
   if (GetModule()==3 && GetCrystal()==999 && GetRow()==999)
    return GetRow();
  */

  // Test EMC
  if (GetModule()==6)
    return  GetCrystal();
  
  return -1000; // failure
}

Double_t PndEmcDigi::GetEnergy() const
{
	return fEnergy;
}

Int_t PndEmcDigi::GetDCnumber() const {

// --------------------- (experimental) MAP DIGI INTO VIRTUAL DATA CONCENTRATORS (DC) -------------------------

// ----- The current map divides the EMC into rectangular sections of 120 units of XPad,YPad, thus ------------
// ----- each section contains up to 120 crystals (but usually less, especially in the FWEndcap) --------------

	Int_t Module;
	Int_t xpad = GetXPad();
	Int_t ypad = GetYPad();

	if (GetModule() == 1 || GetModule() == 2) Module = 2; // "redefinition" of barrel, easier for the switch
	if (GetModule() == 3) Module = 3;
	if (GetModule() == 4) Module = 4;
	if (GetModule() == 5) Module = 5;

	switch (Module) {
		case 2: // barrel (0<x<160, 0<y<72)
	//---------->TOP ROW 
			if (xpad >= 0 && xpad < 10 && ypad <= 72 && ypad > 60) { return 0; } // put digi in DC0
			if (xpad >= 10 && xpad < 20 && ypad <= 72 && ypad > 60) { return 1; } // put digi in DC1
			if (xpad >= 20 && xpad < 30 && ypad <= 72 && ypad > 60) { return 2; } // put digi in DC2
			if (xpad >= 30 && xpad < 40 && ypad <= 72 && ypad > 60) { return 3; } // put digi in DC3
			if (xpad >= 40 && xpad < 50 && ypad <= 72 && ypad > 60) { return 4; } // etc
			if (xpad >= 50 && xpad < 60 && ypad <= 72 && ypad > 60) { return 5; } 
			if (xpad >= 60 && xpad < 70 && ypad <= 72 && ypad > 60) {return 6; }
			if (xpad >= 70 && xpad < 80 && ypad <= 72 && ypad > 60) { return 7; }
			if (xpad >= 80 && xpad < 90 && ypad <= 72 && ypad > 60) { return 8; }
			if (xpad >= 90 && xpad < 100 && ypad <= 72 && ypad > 60) { return 9; }
			if (xpad >= 100 && xpad < 110 && ypad <= 72 && ypad > 60) { return 10; }
			if (xpad >= 110 && xpad < 120 && ypad <= 72 && ypad > 60) { return 11; }
			if (xpad >= 120 && xpad < 130 && ypad <= 72 && ypad > 60) { return 12; }
			if (xpad >= 130 && xpad < 140 && ypad <= 72 && ypad > 60) {return 13; }
			if (xpad >= 140 && xpad < 150 && ypad <= 72 && ypad > 60) { return 14; }
			if (xpad >= 150 && ypad <= 72 && ypad > 60) { return 15; }
	//---------->2nd ROW 
			if (xpad >= 0 && xpad < 10 && ypad <= 60 && ypad > 48) { return 16; }
			if (xpad >= 10 && xpad < 20 && ypad <= 60 && ypad > 48) { return 17; }
			if (xpad >= 20 && xpad < 30 && ypad <= 60 && ypad > 48) { return 18; }
			if (xpad >= 30 && xpad < 40 && ypad <= 60 && ypad > 48) { return 19; }
			if (xpad >= 40 && xpad < 50 && ypad <= 60 && ypad > 48) { return 20; }
			if (xpad >= 50 && xpad < 60 && ypad <= 60 && ypad > 48) { return 21; }
			if (xpad >= 60 && xpad < 70 && ypad <= 60 && ypad > 48) { return 22; }
			if (xpad >= 70 && xpad < 80 && ypad <= 60 && ypad > 48) { return 23; }
			if (xpad >= 80 && xpad < 90 && ypad <= 60 && ypad > 48) { return 24; }
			if (xpad >= 90 && xpad < 100 && ypad <= 60 && ypad > 48) { return 25; }
			if (xpad >= 100 && xpad < 110 && ypad <= 60 && ypad > 48) { return 26; }
			if (xpad >= 110 && xpad < 120 && ypad <= 60 && ypad > 48) { return 27; }
			if (xpad >= 120 && xpad < 130 && ypad <= 60 && ypad > 48) { return 28; }
			if (xpad >= 130 && xpad < 140 && ypad <= 60 && ypad > 48) { return 29; }
			if (xpad >= 140 && xpad < 150 && ypad <= 60 && ypad > 48) { return 30; }
			if (xpad >= 150 && ypad <= 60 && ypad > 48) { return 31; }
	//---------->3rd ROW 
			if (xpad >= 0 && xpad < 10 && ypad <= 48 && ypad > 36) { return 32; }
			if (xpad >= 10 && xpad < 20 && ypad <= 48 && ypad > 36) { return 33; }
			if (xpad >= 20 && xpad < 30 && ypad <= 48 && ypad > 36) { return 34; }
			if (xpad >= 30 && xpad < 40 && ypad <= 48 && ypad > 36) { return 35; }
			if (xpad >= 40 && xpad < 50 && ypad <= 48 && ypad > 36) { return 36; }
			if (xpad >= 50 && xpad < 60 && ypad <= 48 && ypad > 36) { return 37; }
			if (xpad >= 60 && xpad < 70 && ypad <= 48 && ypad > 36) { return 38; }
			if (xpad >= 70 && xpad < 80 && ypad <= 48 && ypad > 36) { return 39; }
			if (xpad >= 80 && xpad < 90 && ypad <= 48 && ypad > 36) { return 40; }
			if (xpad >= 90 && xpad < 100 && ypad <= 48 && ypad > 36) { return 41; }
			if (xpad >= 100 && xpad < 110 && ypad <= 48 && ypad > 36) { return 42; }
			if (xpad >= 110 && xpad < 120 && ypad <= 48 && ypad > 36) { return 43; }
			if (xpad >= 120 && xpad < 130 && ypad <= 48 && ypad > 36) { return 44; }
			if (xpad >= 130 && xpad < 140 && ypad <= 48 && ypad > 36) { return 45; }
			if (xpad >= 140 && xpad < 150 && ypad <= 48 && ypad > 36) { return 46; }
			if (xpad >= 150 && ypad <= 48 && ypad > 36) { return 47; }
	//---------->4th ROW 
			if (xpad >= 0 && xpad < 10 && ypad <= 36 && ypad > 24) { return 48; }
			if (xpad >= 10 && xpad < 20 && ypad <= 36 && ypad > 24) { return 49; }
			if (xpad >= 20 && xpad < 30 && ypad <= 36 && ypad > 24) { return 50; }
			if (xpad >= 30 && xpad < 40 && ypad <= 36 && ypad > 24) { return 51; }
			if (xpad >= 40 && xpad < 50 && ypad <= 36 && ypad > 24) { return 52; }
			if (xpad >= 50 && xpad < 60 && ypad <= 36 && ypad > 24) { return 53;}
			if (xpad >= 60 && xpad < 70 && ypad <= 36 && ypad > 24) { return 54; }
			if (xpad >= 70 && xpad < 80 && ypad <= 36 && ypad > 24) { return 55; }
			if (xpad >= 80 && xpad < 90 && ypad <= 36 && ypad > 24) { return 56; }
			if (xpad >= 90 && xpad < 100 && ypad <= 36 && ypad > 24) { return 57; }
			if (xpad >= 100 && xpad < 110 && ypad <= 36 && ypad > 24) { return 58; }
			if (xpad >= 110 && xpad < 120 && ypad <= 36 && ypad > 24) { return 59; }
			if (xpad >= 120 && xpad < 130 && ypad <= 36 && ypad > 24) { return 60; }
			if (xpad >= 130 && xpad < 140 && ypad <= 36 && ypad > 24) { return 61; }
			if (xpad >= 140 && xpad < 150 && ypad <= 36 && ypad > 24) { return 62; }
			if (xpad >= 150 && ypad <= 36 && ypad > 24) { return 63; }
	//---------->5th ROW 
			if (xpad >= 0 && xpad < 10 && ypad <= 24 && ypad > 12) { return 64; }
			if (xpad >= 10 && xpad < 20 && ypad <= 24 && ypad > 12) { return 65; }
			if (xpad >= 20 && xpad < 30 && ypad <= 24 && ypad > 12) { return 66; }
			if (xpad >= 30 && xpad < 40 && ypad <= 24 && ypad > 12) { return 67; }
			if (xpad >= 40 && xpad < 50 && ypad <= 24 && ypad > 12) { return 68; }
			if (xpad >= 50 && xpad < 60 && ypad <= 24 && ypad > 12) { return 69; }
			if (xpad >= 60 && xpad < 70 && ypad <= 24 && ypad > 12) { return 70;}
			if (xpad >= 70 && xpad < 80 && ypad <= 24 && ypad > 12) { return 71; }
			if (xpad >= 80 && xpad < 90 && ypad <= 24 && ypad > 12) { return 72; }
			if (xpad >= 90 && xpad < 100 && ypad <= 24 && ypad > 12) { return 73; }
			if (xpad >= 100 && xpad < 110 && ypad <= 24 && ypad > 12) { return 74; }
			if (xpad >= 110 && xpad < 120 && ypad <= 24 && ypad > 12) { return 75;}
			if (xpad >= 120 && xpad < 130 && ypad <= 24 && ypad > 12) { return 76; }
			if (xpad >= 130 && xpad < 140 && ypad <= 24 && ypad > 12) { return 77; }
			if (xpad >= 140 && xpad < 150 && ypad <= 24 && ypad > 12) { return 78; }
			if (xpad >= 150 && ypad <= 24 && ypad > 12) { return 79; }
	//---------->6th ROW (BOTTOM OF BARREL)
			if (xpad >= 0 && xpad < 10 && ypad <= 12) { return 80; }
			if (xpad >= 10 && xpad < 20 && ypad <= 12) { return 81; }
			if (xpad >= 20 && xpad < 30 && ypad <= 12) { return 82; }
			if (xpad >= 30 && xpad < 40 && ypad <= 12) { return 83; }
			if (xpad >= 40 && xpad < 50 && ypad <= 12) { return 84; }
			if (xpad >= 50 && xpad < 60 && ypad <= 12) { return 85; }
			if (xpad >= 60 && xpad < 70 && ypad <= 12) { return 86; }
			if (xpad >= 70 && xpad < 80 && ypad <= 12) { return 87; }
			if (xpad >= 80 && xpad < 90 && ypad <= 12) { return 88; }
			if (xpad >= 90 && xpad < 100 && ypad <= 12) { return 89; }
			if (xpad >= 100 && xpad < 110 && ypad <= 12) { return 90; }
			if (xpad >= 110 && xpad < 120 && ypad <= 12) { return 91; }
			if (xpad >= 120 && xpad < 130 && ypad <= 12) { return 92; }
			if (xpad >= 130 && xpad < 140 && ypad <= 12) { return 93; }
			if (xpad >= 140 && xpad < 150 && ypad <= 12) { return 94; }
			if (xpad >= 150 && ypad <= 12) { return 95; }
			break;
		case 3: // FwEndcap (-36<x<36, -37<y<37)
	//---------->TOP ROW (OF FWENDCAP)
			if (xpad >= -36 && xpad < -24 && ypad <= 37 && ypad > 27) { return 96; }
			if (xpad >= -24 && xpad < -12 && ypad <= 37 && ypad > 27) { return 97; }
			if (xpad >= -12 && xpad < 0 && ypad <= 37 && ypad > 27) { return 98; }
			if (xpad >= 0 && xpad < 12 && ypad <= 37 && ypad > 27) { return 99; }
			if (xpad >= 12 && xpad < 24 && ypad <= 37 && ypad > 27) { return 100; }
			if (xpad >= 24 && ypad <= 37 && ypad > 27) { return 101; }
	//---------->2nd ROW
			if (xpad >= -36 && xpad < -24 && ypad <= 27 && ypad > 17) { return 102; }
			if (xpad >= -24 && xpad < -12 && ypad <= 27 && ypad > 17) { return 103; }
			if (xpad >= -12 && xpad < 0 && ypad <= 27 && ypad > 17) { return 104; }
			if (xpad >= 0 && xpad < 12 && ypad <= 27 && ypad > 17) { return 105; }
			if (xpad >= 12 && xpad < 24 && ypad <= 27 && ypad > 17) { return 106; }
			if (xpad >= 24 && ypad <= 27 && ypad > 17) { return 107; }
	//---------->3rd ROW
			if (xpad >= -36 && xpad < -24 && ypad <= 17 && ypad >  7) { return 108; }
			if (xpad >= -24 && xpad < -12 && ypad <= 17 && ypad >  7) { return 109; }
			if (xpad >= -12 && xpad < 0 && ypad <= 17 && ypad >  7) { return 110; }
			if (xpad >= 0 && xpad < 12 && ypad <= 17 && ypad >  7) { return 111; }
			if (xpad >= 12 && xpad < 24 && ypad <= 17 && ypad >  7) { return 112; }
			if (xpad >= 24 && ypad <= 17 && ypad >  7) { return 113; }
	//---------->4th ROW
			if (xpad >= -36 && xpad < -24 && ypad <=  7 && ypad >  -3) { return 114; }
			if (xpad >= -24 && xpad < -12 && ypad <=  7 && ypad >  -3) { return 115; }
			if (xpad >= -12 && xpad < 0 && ypad <=  7 && ypad > -3) { return 116; }
			if (xpad >= 0 && xpad < 12 && ypad <= 7 && ypad >  -3) { return 117; }
			if (xpad >= 12 && xpad < 24 && ypad <=  7 && ypad > -3) { return 118; }
			if (xpad >= 24 && ypad <=  7 && ypad > -3) { return 119; }
	//---------->5th ROW
			if (xpad >= -36 && xpad < -24 && ypad <= -3 && ypad >  -13) { return 120; }
			if (xpad >= -24 && xpad < -12 && ypad <= -3 && ypad >  -13) { return 121; }
			if (xpad >= -12 && xpad < 0 && ypad <= -3 && ypad > -13) { return 122; }
			if (xpad >= 0 && xpad < 12 && ypad <= -3 && ypad >  -13) { return 123; }
			if (xpad >= 12 && xpad < 24 && ypad <= -3 && ypad > -13) { return 124; }
			if (xpad >= 24 && ypad <= -3 && ypad > -13) { return 125; }
	//---------->6th ROW
			if (xpad >= -36 && xpad < -24 && ypad <= -13 && ypad >  -23) { return 126; }
			if (xpad >= -24 && xpad < -12 && ypad <= -13 && ypad >  -23) { return 127; }
			if (xpad >= -12 && xpad < 0 && ypad <= -13 && ypad > -23) { return 128; }
			if (xpad >= 0 && xpad < 12 && ypad <= -13 && ypad >  -23) { return 129; }
			if (xpad >= 12 && xpad < 24 && ypad <= -13 && ypad > -23) { return 130; }
			if (xpad >= 24 && ypad <= -13 && ypad > -23) { return 131; }
	//---------->7th ROW
			if (xpad >= -36 && xpad < -24 && ypad <= -23 && ypad >  -33) { return 132; }
			if (xpad >= -24 && xpad < -12 && ypad <= -23 && ypad >  -33)  { return 133; }
			if (xpad >= -12 && xpad < 0 && ypad <= -23 && ypad > -33) { return 134; }
			if (xpad >= 0 && xpad < 12 && ypad <= -23 && ypad >  -33) { return 135; }
			if (xpad >= 12 && xpad < 24 && ypad <= -23 && ypad > -33) { return 136; }
			if (xpad >= 24 && ypad <= -23 && ypad > -33) { return 137; }
	//---------->8th ROW (BOTTOM OF FWENDCAP)
			if (ypad <= -33) { return 138; } // put crystals of last edge in 1 DC (~90 crystals)
			break;
		case 4: // BwEndcap (-14<x<15, -14<y<15)	
	//---------->TOP ROW (OF BWENDCAP)
			if (xpad >= -14 && xpad < -7 && ypad <= 15 && ypad > 0) { return 139; }
			if (xpad >= -7 && xpad < 0 && ypad <= 15 && ypad > 0) { return 140; }
			if (xpad >= 0 && xpad < 7 && ypad <= 15 && ypad > 0) { return 141; }
			if (xpad >= 7 && ypad <= 15 && ypad > 0) { return 142; }
	//---------->2nd ROW (BOTTOM OF BWENDCAP)
			if (xpad >= -14 && xpad < -7 && ypad <= 0) { return 143; }
			if (xpad >= -7 && xpad < 0 && ypad <= 0) { return 144; }
			if (xpad >= 0 && xpad < 7 && ypad <= 0) { return 145; }
			if (xpad >= 7 && ypad <= 0 )  { return 146; }
			break;
		case 5: // shashlik (-27<x<1, 0<y<55)
	//---------->TOP ROW (OF SHASHLIK)
			if (xpad >= -27 && xpad < -18 && ypad <= 56 && ypad > 42) { return 147; }
			if (xpad >= -18 && xpad < -9 && ypad <= 56 && ypad > 42) { return 148; }
			if (xpad >= -9 && ypad <= 56 && ypad > 42) { return 149; }
	//---------->2nd ROW
			if (xpad >= -27 && xpad < -18 && ypad <= 42 && ypad > 28) { return 150; }
			if (xpad >= -18 && xpad < -9 && ypad <= 42 && ypad > 28) { return 151; }
			if (xpad >= -9 && ypad <= 42 && ypad > 28) { return 152; }
	//---------->3rd ROW
			if (xpad >= -27 && xpad < -18 && ypad <= 28 && ypad > 14) { return 153; }
			if (xpad >= -18 && xpad < -9 && ypad <= 28 && ypad > 14) { return 154; }
			if (xpad >= -9 && ypad <= 28 && ypad > 14) { return 155; }
	//---------->3rd ROW (BOTTOM OF SHASHLIK)
			if (xpad >= -27 && xpad < -18 && ypad <= 14)  { return 156; }
			if (xpad >= -18 && xpad < -9 && ypad <= 14) { return 157; }
			if (xpad >= -9 && ypad <= 14) { return 158; }
			break;
		default: Error("GetDCnumber","Unknown module");
	}

  LOG(warn) << " PndEmcDigi::GetDCnumber "
            << "returns -1";
  cout << "    (XPad, YPad) = (" << xpad << ", " << ypad << "), Module = " << GetModule() << endl;
	return -1; // failure
}


bool
PndEmcDigi::operator==( const PndEmcDigi& otherDigi ) const
{
  bool isEqual = false;
  
  // 2 EmcDigis are equal is their relative energy difference within the following tolerance
  Double_t energy_tolerance=1e-5;

  if ( this->GetThetaInt() == otherDigi.GetThetaInt() &&
       this->GetPhiInt() == otherDigi.GetPhiInt() &&
       ((this->GetEnergy() - otherDigi.GetEnergy())/this->GetEnergy()<energy_tolerance )) isEqual =true;
  
  return isEqual;
}

bool PndEmcDigi::operator!=(const PndEmcDigi& otherDigi) const 
{
  return ! ( *this == otherDigi );
}

bool PndEmcDigi::operator<( const PndEmcDigi & otherDigi) const
{
  if (fEnergy < otherDigi.fEnergy) 
    return true;
  else if (fEnergy == otherDigi.fEnergy && fTheta< otherDigi.fTheta)
    return true;
  else if (fEnergy == otherDigi.fEnergy && fTheta == otherDigi.fTheta && fPhi < otherDigi.fPhi)
    return true;

  return false;
}



// -----   Public method Print   -----------------------------------
void PndEmcDigi::Print(const Option_t* ) const {
  cout << "EMC digi: cellid=" << GetDetectorId() << ", Energy=" << fEnergy<<", Time="<<GetTimeStamp()<<", Evt="<<fEvtNo;
  if (fTrackId>0) cout << ", TrackID= " << fTrackId;
  cout << ", x=" << where().x() << ", y=" << where().y() << endl; 
}


PndEmcSharedDigi*
PndEmcDigi::dynamic_cast_PndEmcSharedDigi()
{
  return nullptr;
}

const PndEmcSharedDigi*
PndEmcDigi::dynamic_cast_PndEmcSharedDigi() const
{
  return nullptr;
}

ClassImp(PndEmcDigi)
