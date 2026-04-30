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

//---------------------------------------------------------------------
// File and Version Information:
// 	$Id: $
//
// Description:
// This task convert PndEmcBumps to PndEmcRecoHit objects
// At the moment PndEmcBump and PndEmcRecoHit have alsmost identically structure
// The main difference that for PndEmcBump position() nad GetEnergy() calculate corresponding values but for PndEmcRecoHit just return corresponding data member
//
// Environment:
//	Software developed for the PANDA Detector at GSI.
//
//------------------------------------------------------------------------

#include "PndEmcMakeRecoHit.h"

#include "PndEmcRecoHit.h"
#include "PndEmcBump.h"
#include "PndEmcRecoPar.h"
#include "PndDetectorList.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TStopwatch.h"
#include "TROOT.h"
#include "TVector3.h"

#include <iostream>
//#include <vector>
//#include <set>

using std::cout;
using std::endl;

PndEmcMakeRecoHit::PndEmcMakeRecoHit(Int_t verbose, Bool_t storerecohits) : fBumpArray(nullptr), fRecoHitArray(nullptr), fRecoPar(new PndEmcRecoPar()), fVerbose(verbose)
{
SetPersistency(storerecohits);
}

//--------------
// Destructor --
//--------------

PndEmcMakeRecoHit::~PndEmcMakeRecoHit()
{ 
}

// -----   Public method Init   -------------------------------
InitStatus PndEmcMakeRecoHit::Init() {
 
  	// Get RootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if ( ! ioman )
	{
		cout << "-E- PndEmcMakeRecoHit::Init: "
		<< "RootManager not instantiated!" << endl;
		return kFATAL;
	}
	
	// Get input array
	fBumpArray = dynamic_cast<TClonesArray *>(ioman->GetObject("EmcBump"));
	if ( ! fBumpArray ) {
		cout << "-W- PndEmcMakeRecoHit::Init: "
		<< "No PndEmcBump array!" << endl;
		return kERROR;
	}
	
	// Create and register output array
	fRecoHitArray = new TClonesArray("PndEmcRecoHit");
	
	ioman->Register("EmcRecoHit","Emc",fRecoHitArray,GetPersistency());

  LOG(info) << " PndEmcMakeRecoHit: Intialization successfull";

  return kSUCCESS;
}


void PndEmcMakeRecoHit::Exec(Option_t*) 
{
	// Reset output array
	if ( ! fRecoHitArray ) Fatal("Exec", "No Cluster Array");
	fRecoHitArray->Delete();

	Int_t nBumps = fBumpArray->GetEntriesFast();

	//loop to build Cluster
	for (Int_t iBump=0; iBump<nBumps; iBump++)
	{
		PndEmcBump* theBump = (PndEmcBump*) fBumpArray->At(iBump);
		Double_t energy=theBump->GetEnergy();
		TVector3 position=theBump->position();
		PndEmcRecoHit* myHit = new((*fRecoHitArray)[iBump]) PndEmcRecoHit(energy,position); 
		myHit->SetLink(FairLink("EmcBump", iBump));
		
	}
}

void PndEmcMakeRecoHit::SetParContainers() {

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) LOG(fatal) << "SetParContainers: No analysis run";

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) LOG(fatal) << "SetParContainers: No runtime database";

  // Get Emc reconstruction parameter container
  fRecoPar = (PndEmcRecoPar*) db->getContainer("PndEmcRecoPar");
}

void PndEmcMakeRecoHit::SetStorageOfData(Bool_t val)
{
  SetPersistency(val);
  return;
}
  

ClassImp(PndEmcMakeRecoHit)
