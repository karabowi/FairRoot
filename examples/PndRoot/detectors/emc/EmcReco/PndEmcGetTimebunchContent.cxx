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
//	This object constructs a list of clusters from a list of digis.
//	Online version.
//
// Environment:
//	Software developed for the PANDA Detector at GSI.
//


#include "PndEmcGetTimebunchContent.h"

#include "PndEmcXClMoments.h"
#include "PndEmcStructure.h"
#include "PndEmcMapper.h"
#include "PndEmcGeoPar.h"
#include "PndEmcDigiPar.h"
#include "PndEmcRecoPar.h"
#include "PndEmcCluster.h"
#include "PndEmcDigi.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TH1.h"

#include <iostream>
#include <vector>

using std::cout;
using std::endl;


static Int_t evtCounter = 0;
Int_t fNrOfBrokenEvents = 0;
std::vector<Int_t> evtVector;
std::vector<Int_t> tagArray;

PndEmcGetTimebunchContent::PndEmcGetTimebunchContent(Int_t verbose)
  : FairTask("EmcTimebunchTask", verbose), fDigiArray(nullptr), fDigiFunctor(nullptr), fDigiEnergyTresholdBarrel(0.), fDigiEnergyTresholdFWD(0.), fDigiEnergyTresholdBWD(0.),
    fDigiEnergyTresholdShashlyk(0.), fClusterActiveTime(0.), fGeoPar(new PndEmcGeoPar()), fRecoPar(new PndEmcRecoPar()), fNrOfEvents(0)
{
}

//--------------
// Destructor --
//--------------

PndEmcGetTimebunchContent::~PndEmcGetTimebunchContent()
{ 
}

// -----   Public method Init   -------------------------------
InitStatus PndEmcGetTimebunchContent::Init() {

	// Get RootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if ( ! ioman )
	{
    LOG(error) << " PndEmcGetTimebunchContent::Init: "
               << "RootManager not instantiated!";
    return kFATAL;
	}

	if(!FairRunAna::Instance()->IsTimeStamp()) {
    LOG(error) << " PndEmcGetTimebunchContent::Init: "
               << "This task can be activated only online";
    return kFATAL;
	}

	// Get nr of events	
	fNrOfEvents = (ioman->GetInTree())->GetEntriesFast();

	// Get input array
	fDigiArray = (TClonesArray*) ioman->GetObject("EmcDigiSorted"); 

	if (!fDigiArray) {
    LOG(warn) << " PndEmcGetTimebunchContent::Init: "
              << "No PndEmcDigi array!";
    return kERROR;
	}

	// searching for time gaps in digi stream
	fDigiFunctor = new TimeGap();

	fGeoPar->InitEmcMapper();  
	PndEmcStructure::Instance();

	// get some parameters from the parbase
	fDigiEnergyTresholdBarrel=fRecoPar->GetEnergyThresholdBarrel();
	fDigiEnergyTresholdFWD=fRecoPar->GetEnergyThresholdFWD();
	fDigiEnergyTresholdBWD=fRecoPar->GetEnergyThresholdBWD();
	fDigiEnergyTresholdShashlyk=fRecoPar->GetEnergyThresholdShashlyk();

	//convert from seconds to nanoseconds...in parfile everything is seconds...here we need nanoseconds
	if (fClusterActiveTime == 0.) fClusterActiveTime=fRecoPar->GetClusterActiveTime() * 1.0e9; 

	cout << "Minimum Time Between Timebunches: " << fClusterActiveTime << " ns"<<endl;

	cout << "=> " << fNrOfEvents << " events." << endl;

	for (Int_t i=0; i<fNrOfEvents; i++) tagArray.push_back(0); // array that keeps track which events have already been marked. Initialise all entries to false (0).

	hEventMultiplicity = new TH1I("hEventMultiplicity","Number of events per timebunch",20,0,20);

  LOG(info) << " PndEmcGetTimebunchContent: Intialization successful";
  return kSUCCESS;
}


void PndEmcGetTimebunchContent::Exec(Option_t* opt) {

	// Reset output arrays and get all digis up to a certain time gap specified by fClusterActiveTime
	if(FairRunAna::Instance()->IsTimeStamp()) {
		fDigiArray->Delete();
		fDigiArray->AbsorbObjects(FairRootManager::Instance()->GetData("EmcDigiSorted", fDigiFunctor, fClusterActiveTime));
	}	

	evtCounter++;
	if (evtCounter%250 == 0) {
		cout << "[INFO\t] PndEmcGetTimebunchContent: " << evtCounter << " timebunches processed." << endl;	
	}


// --------------------- START OF ALGORITHM ---------------------------

	Int_t nDigis = fDigiArray->GetEntriesFast();
	Int_t currentEvtno = 0;
	Int_t nHits = 0;
	std::vector<Int_t> evtList;
	Bool_t update = true;

	if (fVerbose>1){
		cout<<"DigiList length: "<<nDigis<<endl;
	}

	//loop over all digis
	
	for (Int_t iDigi=0; iDigi<nDigis; ++iDigi) {

		/* Get a new digi from the digi array */
		PndEmcDigi* theDigi = (PndEmcDigi*) fDigiArray->At(iDigi);

		// thresholds: if energy is below the corresponding threshold, digi is not considered in clustering
		Int_t module=theDigi->GetModule();
		if ((module==1||module==2) && (theDigi->GetEnergy()< fDigiEnergyTresholdBarrel)) continue;
		if ((module==3) && (theDigi->GetEnergy()< fDigiEnergyTresholdFWD)) continue;
		if ((module==4) && (theDigi->GetEnergy()< fDigiEnergyTresholdBWD)) continue;
		if ((module==5) && (theDigi->GetEnergy()< fDigiEnergyTresholdShashlyk)) continue;

		FairMultiLinkedData digiLinks = theDigi->GetLinksWithType(FairRootManager::Instance()->GetBranchId("EmcHit"));

		if (fVerbose>2)	cout<<"Digi: "<< iDigi << " is made from " << digiLinks.GetNLinks() << " hits." << endl;

		for (int iHit=0; iHit<digiLinks.GetNLinks(); iHit++){
			PndEmcHit* theHit = (PndEmcHit*)FairRootManager::Instance()->GetCloneOfLinkData(digiLinks.GetLink(iHit));
			if(theHit) {
			
				nHits++;
				currentEvtno = theHit->GetMcList().at(0);
				if (fVerbose>2)	cout<<"Event nr: "<< currentEvtno << endl;

				// check if current evtno already exists in this timebunch
				update = true;
				for (Int_t iL=0; iL<evtList.size(); iL++) {
					if (evtList[iL] == currentEvtno) { // if it does,
						update = false; // don't write the current evtno in the list
						break; // and stop checking
					}
					else update = true;
				}
				if (update == true)	evtList.push_back(currentEvtno);

			} else {
				cout << "-E in PndEmcGetTimebunchContent::Exec FairLink: " << digiLinks.GetLink(iHit) << " to EmcHit delivers null" << std::endl;
			}
		}
	}

	if (fVerbose>1){
		cout<<"\tNr of hits: "<<nHits<<endl;
	}

	//check if current evtno's already exist
	for (Int_t iLs=0; iLs<evtList.size(); iLs++) {
		if (tagArray[evtList[iLs]] == 1) continue; // don't bother checking events that have already been marked
		for (Int_t iEv=0; iEv<evtVector.size(); iEv++) {
			if (evtVector[iEv] == evtList[iLs]) {// if it does,
				if (tagArray[evtVector[iEv]] == 0) { // and if it hasn't already been marked,
					fNrOfBrokenEvents++; // keep track of nr of broken events (events that have spread out over multiple timebunches)
					tagArray[evtVector[iEv]] = 1; // and mark as broken
				}
				break;
			}
		}
	}

	hEventMultiplicity->Fill(evtList.size()); // put nr of events in this timebunch in a histo

	evtVector.insert(evtVector.end(), evtList.begin(), evtList.end()); // add list of current evtno's to the total list

}


void PndEmcGetTimebunchContent::FinishTask() {

	if (fVerbose>0){
		cout << "Size of eventnr vector: " << evtVector.size() << endl;
	}

	cout << " => " << fNrOfBrokenEvents << " events have been spread out over multiple timebunches." << endl;

	TCanvas *cN = new TCanvas("cN","Event multiplicity",1);
	cN->cd(1);
	hEventMultiplicity->GetXaxis()->SetTitle("Nr of events per timebunch");
	hEventMultiplicity->Draw("");

}

void PndEmcGetTimebunchContent::SetParContainers() {

	// Get run and runtime database
	FairRun* run = FairRun::Instance();
	if ( ! run ) LOG(fatal) << "SetParContainers: No analysis run";

	FairRuntimeDb* db = run->GetRuntimeDb();
	if ( ! db ) LOG(fatal) << "SetParContainers: No runtime database";

	// Get Emc geometry parameter container
	fGeoPar = (PndEmcGeoPar*) db->getContainer("PndEmcGeoPar");

	// Get Emc reconstruction parameter container
	fRecoPar = (PndEmcRecoPar*) db->getContainer("PndEmcRecoPar");
}


ClassImp(PndEmcGetTimebunchContent)
