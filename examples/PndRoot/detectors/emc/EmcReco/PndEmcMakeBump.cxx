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

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//      This module takes Clusters (Connected Regions) and slits them
//      up into Bumps. They are defined by local maxima
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Author List:
//	Xiaorong Shi            Lawrence Livermore National Lab
//	Steve Playfer           University of Edinburgh
//	Stephen J. Gowdy           University of Edinburgh
//      Phil Strother              Imperial College 
// 
// Dima Melnychuk, adaption for PANDA
// Modified:
// M. Babai
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "PndEmcMakeBump.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "FairRunAna.h"
#include "FairLogger.h"

#include "PndEmcDataTypes.h"

#include "PndEmcBump.h"
#include "PndEmcCluster.h"
#include "PndEmcDigi.h"
#include "PndEmcSharedDigi.h"
#include "PndEmc2DLocMaxFinder.h"
#include "PndEmc2DLocMaxFinderTEST.h"
#include "PndEmcExpClusterSplitter.h"
#include "PndEmcPhiBumpSplitter.h"
#include "PndEmcOnlineBumpSplitter.h"
#include "PndEmcTwoCoordIndex.h"

#include "TClonesArray.h"
#include "TROOT.h"

//---------------
// C++ Headers --
//---------------
#include <iostream>

using std::endl;
using std::cout;

Int_t PndEmcMakeBump::fEventCounter=1;
Int_t PndEmcMakeBump::fNrOfEvents=0;
Int_t nProg = 0;

//----------------
// Constructors --
//----------------
PndEmcMakeBump::PndEmcMakeBump(Int_t verbose, Bool_t persistance):
PndPersistencyTask("EMC Bump splitting Task"), fVerbose(verbose)
{
  SetPersistency(persistance);
  this->Add(new PndEmcPhiBumpSplitter());
  this->Add(new PndEmc2DLocMaxFinder());
  this->Add(new PndEmcExpClusterSplitter());
//  this->Add(new PndEmcOnlineBumpSplitter());
//  this->Add(new PndEmc2DLocMaxFinderTEST()); // only used by PndEmcExpClusterSplitter probably
//  this->Add(new PndEmcExpClusterSplitter());
  
  TList* thistasks = this->GetListOfTasks();
  for(Int_t i=0;i<thistasks->GetEntries();i++)
  {
    ((PndPersistencyTask*)thistasks->At(i))->SetVerbose(fVerbose);
  }
  
	SetStorageOfData(GetPersistency());
}

void PndEmcMakeBump::SetStorageOfData(Bool_t val)
{
  SetPersistency(val);
  TList* thistasks = this->GetListOfTasks();
  if (thistasks->GetEntries()>0) ((PndEmcPhiBumpSplitter*)thistasks->At(0))->SetStorageOfData(fPersistance);  
  if (thistasks->GetEntries()>1) ((PndEmcOnlineBumpSplitter*)thistasks->At(1))->SetStorageOfData(fPersistance); 
  if (thistasks->GetEntries()>2) ((PndEmc2DLocMaxFinderTEST*)thistasks->At(2))->SetStorageOfData(fPersistance);
  if (thistasks->GetEntries()>3) ((PndEmcExpClusterSplitter*)thistasks->At(3))->SetStorageOfData(fPersistance);
  return;
}

//--------------
// Destructor --
//--------------
PndEmcMakeBump::~PndEmcMakeBump()
{
}

/**
 * @brief Init Task
 * 
 * @return InitStatus
 * @retval kSUCCESS success
 */
InitStatus PndEmcMakeBump::Init() 
{
  // Get nr of events	
  fNrOfEvents = (FairRootManager::Instance()->GetInTree())->GetEntriesFast();
  LOG(info) << " PndEmcMakeBump::Init: " << fNrOfEvents << " events.";
  return kSUCCESS;
}

/**
 * @brief Runs the task.
 * 
 * Simply outputs the event counter. The subtasks that do the real work are automatically called.
 * 
 * @param opt unused
 * @return void
 */
void PndEmcMakeBump::Exec(Option_t*) 
{
	if (fVerbose>0)
		if (fEventCounter-1==nProg*fNrOfEvents/100) {
			cout << "\r[INFO\t] PndEmcMakeBump: " << nProg << "\% completed." << std::flush;	
			nProg += 1;
		}
	fEventCounter++;
	return;
}

void PndEmcMakeBump::SetParContainers() 
{
}

ClassImp(PndEmcMakeBump)
