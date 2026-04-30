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
//	This object constructs a list of packaged clusters from a list of sorted clusters.
//	Online version.
//
// Environment:
//	Software developed for the PANDA Detector at GSI.
//


#include "PndEmcPackClusters.h"

#include "PndEmcClusterProperties.h"
#include "PndEmcXClMoments.h"
#include "PndEmcStructure.h"
#include "PndEmcMapper.h"
#include "PndEmcRecoPar.h"
#include "PndEmcCluster.h"
#include "PndEmcDigi.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TROOT.h"
#include "TVector3.h"

#include <iostream>

using namespace std;


static Int_t evtCounter = 0;

PndEmcPackClusters::PndEmcPackClusters(Int_t verbose)
  : FairTask("EmcClusterPackingTask", verbose), fClusterArray(nullptr), fClusterFunctor(nullptr), fTimebunchCutTime(0.), fNrOfEvents(0), fRecoPar(new PndEmcRecoPar()),
    fAutoTime(kTRUE)
{
}

//--------------
// Destructor --
//--------------

PndEmcPackClusters::~PndEmcPackClusters()
{ 
}

// -----   Public method Init   -------------------------------
InitStatus PndEmcPackClusters::Init() {

	// Get RootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if ( ! ioman )
	{
    LOG(error) << " PndEmcPackClusters::Init: "
               << "RootManager not instantiated!";
    return kFATAL;
	}

	if(!FairRunAna::Instance()->IsTimeStamp()) {
    LOG(error) << " PndEmcPackClusters::Init: "
               << "This task can be activated only online";
    return kFATAL;
	}

	// Get input array
	fClusterArray = (TClonesArray*) ioman->GetObject("EmcClusterSorted"); 
	if (!fClusterArray) {
    LOG(warn) << " PndEmcPackClusters::Init: "
              << "No PndEmcCluster array!";
    return kERROR;
	}
	//fDigiArray = (TClonesArray*) ioman->GetObject("EmcDigiClusterBase"); // for timebased, use sorted digis

	// for subsequent methods we need the "event grouping" as given by the TS buffer 
	// --> fClusterArray becomes an output array. 
	//
	// can be removed once PndEmcCluster object has been rewritten
	fClusterArray = new TClonesArray("PndEmcCluster");		
	ioman->Register("EmcCluster", "Emc", fClusterArray, kTRUE);
	//fDigiOutArray = new TClonesArray("PndEmcDigi");		
	//ioman->Register("EmcDigiClusterBase", "Emc", fDigiOutArray, kTRUE);

	// searching for time gaps in cluster stream
	fClusterFunctor = new TimeGap();


	// Get nr of events	
	fNrOfEvents = (ioman->GetInTree())->GetEntriesFast();
	TVector3 *cuttingTime = (TVector3*)(ioman->GetInTree())->GetUserInfo()->First();
	if (cuttingTime && fAutoTime) {fTimebunchCutTime = cuttingTime->X(); cout << "\n-I- PndEmcPackClusters::Init: Reading time from file: ";}

	//convert from seconds to nanoseconds...in parfile everything is seconds...here we need nanoseconds
	if (fTimebunchCutTime == 0.) fTimebunchCutTime=fRecoPar->GetClusterActiveTime() * 1.0e9; 

	cout << "fTimebunchCutTime: " << fTimebunchCutTime << " ns"<<endl;

  LOG(info) << " PndEmcPackClusters: Intialization successful";
  return kSUCCESS;
}


void PndEmcPackClusters::Exec(Option_t* opt) {

	if (fVerbose>2){
		fTimer.Start();
	}

	evtCounter++;

	if (evtCounter%500==0) {
		cout << "\r[INFO\t] PndEmcPackClusters: " << evtCounter << "\ttimebunches repacked." << std::flush;	
	}

	// Reset output arrays and get all digis up to a certain time gap specified by fTimebunchCutTime
	fClusterArray->Delete();
	fClusterArray->AbsorbObjects(FairRootManager::Instance()->GetData("EmcClusterSorted", fClusterFunctor, fTimebunchCutTime));

	//fDigiOutArray->Delete();
	//fDigiOutArray->AbsorbObjects(fDigiArray);


}


void PndEmcPackClusters::FinishTask() {
	std::cout << "\nProcessed in total " << evtCounter << " timebunches." << std::endl;
	if(fVerbose>2) {
		fTimer.Stop();
		Double_t rtime = fTimer.RealTime();
		Double_t ctime = fTimer.CpuTime();
		fTimer.Reset();
		cout << "PndEmcPackClusters, Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	}
}

void PndEmcPackClusters::SetParContainers() {

	// Get run and runtime database
	FairRun* run = FairRun::Instance();
	if ( ! run ) LOG(fatal) << "SetParContainers: No analysis run";

	FairRuntimeDb* db = run->GetRuntimeDb();
	if ( ! db ) LOG(fatal) << "SetParContainers: No runtime database";

	// Get Emc reconstruction parameter container
	fRecoPar = (PndEmcRecoPar*) db->getContainer("PndEmcRecoPar");

}


ClassImp(PndEmcPackClusters)
