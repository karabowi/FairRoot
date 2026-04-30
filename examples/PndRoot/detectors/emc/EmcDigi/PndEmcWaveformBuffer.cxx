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

/*
 * PndEmcWaveformBuffer.cxx
 */

#include "PndEmcWaveformBuffer.h"
#include "PndEmcWaveformData.h"
#include "PndEmcWaveform.h"
#include "PndEmcAbsWaveformSimulator.h"
#include "PndEmcHit.h"
#include "FairLink.h"

#include "TClonesArray.h"
#include <vector>


ClassImp(PndEmcWaveformBuffer);

PndEmcWaveformBuffer::PndEmcWaveformBuffer() : FairWriteoutBuffer(), fStoreWaveformData(kFALSE), fWfDataArray(nullptr) {}

PndEmcWaveformBuffer::PndEmcWaveformBuffer(TString branchName, TString className, TString folderName, Bool_t persistance)
  : FairWriteoutBuffer(branchName, className, folderName, persistance), fStoreWaveformData(kFALSE), fWfDataArray(nullptr)
{
}

PndEmcWaveformBuffer::~PndEmcWaveformBuffer() {
}

void PndEmcWaveformBuffer::FillNewData(PndEmcWaveformData* wfData) {

	FairRootManager* ioman = FairRootManager::Instance();

	//calculate start and active time for timebased simulation framework
	//Only times greater or equal than the current EventTime are accepted by the framework, but the absolute time of the first sample might been set to lower times. To allow this, the timebased simulation time parameters are increased as needed.
	Double_t startTime, activeTime;
	wfData->GetWaveformSimulator()->GetAbsoluteTimeInterval(wfData, startTime, activeTime);
	wfData->SetTimeStamp(startTime);
	wfData->SetTimeOfLastSample(activeTime);
//cout.precision(10);
	//std::cout << "-I- PndEmcWaveformBuffer::FillNewData startTime: " << startTime << " TimeBeforeFirstHit " <<
			//wfData->GetWaveformSimulator()->GetTimeBeforeFirstHit(wfData) << std::endl;
	startTime = startTime<ioman->GetEventTime() ? ioman->GetEventTime() : startTime; //shifting startTime towards greater times
	activeTime += wfData->GetWaveformSimulator()->GetTimeBeforeFirstHit(wfData); // maximal shift in previous step is wfSimulator->GetTimeBeforeFirstHit(), avoid overlapping of generated waves in absolute time domain

	FairWriteoutBuffer::FillNewData(wfData, startTime, activeTime);

}


void PndEmcWaveformBuffer::StoreWaveformData(TString branchName, TString folderName, bool persistance) {
	FairRootManager*  ioman = FairRootManager::Instance();
	fStoreWaveformData = kTRUE;
	fWfDataBranchName = branchName;
	ioman->Register(branchName, "PndEmcWaveformData", folderName, persistance);
	fWfDataArray = 	ioman->GetTClonesArray(branchName);
}



void PndEmcWaveformBuffer::AddNewDataToTClonesArray(FairTimeStamp* data) {

	FairRootManager* ioman = FairRootManager::Instance();
	TClonesArray* myArray = ioman->GetTClonesArray(fBranchName);

	PndEmcWaveformData* wfData = dynamic_cast<PndEmcWaveformData*>(data);
	if (fStoreWaveformData)
		wfData->SetEntryNr(FairLink(-1,ioman->GetEntryNr(), fWfDataBranchName, fWfDataArray->GetEntries()));
	PndEmcWaveform* wave = wfData->GetWaveformSimulator()->Simulate(wfData, myArray);

	if (fVerbose > 1) {
		if(wave) {
			std::cout << "Data Inserted: "  <<  *wave << std::endl;
		} else {
			std::cout << "-E in PndEmcWaveformBuffer::AddNewDataToTClonesArray" <<std::endl;
		}
	}

	if(fStoreWaveformData) {
		new((*fWfDataArray)[fWfDataArray->GetEntries()]) PndEmcWaveformData(*wfData);
	}
}


std::vector<std::pair<double, FairTimeStamp*> > PndEmcWaveformBuffer::Modify(std::pair<double, FairTimeStamp*> oldData, std::pair<double, FairTimeStamp* > newData) {

	PndEmcWaveformData* oldWfData = dynamic_cast<PndEmcWaveformData*>(oldData.second);
	PndEmcWaveformData* newWfData = dynamic_cast<PndEmcWaveformData*>(newData.second);

	(*oldWfData)+=(*newWfData);
	delete newWfData;

	Double_t startTime, activeTime;
	oldWfData->GetWaveformSimulator()->GetAbsoluteTimeInterval(oldWfData, startTime, activeTime);
	oldWfData->SetTimeStamp(startTime);
	oldWfData->SetTimeOfLastSample(activeTime);

	
	activeTime += oldWfData->GetWaveformSimulator()->GetTimeBeforeFirstHit(oldWfData);

	return std::vector<std::pair<double, FairTimeStamp*> >(1, std::pair<double, FairTimeStamp*>(activeTime, oldWfData));
}


double PndEmcWaveformBuffer::FindTimeForData(FairTimeStamp* data) {
	std::map<PndEmcWaveformData, double>::iterator it;
	PndEmcWaveformData myData = *(PndEmcWaveformData*)data;
	it = fData_map.find(myData);
	if (it == fData_map.end())
		return -1;
	else
		return it->second;
}

void PndEmcWaveformBuffer::FillDataMap(FairTimeStamp* data, double activeTime) {
	PndEmcWaveformData myData = *(PndEmcWaveformData*)data;
	fData_map[myData] = activeTime;
}


void PndEmcWaveformBuffer::DeleteOldData() {
	FairWriteoutBuffer::DeleteOldData();
	if(fStoreWaveformData) {
		fWfDataArray->Delete();
	}
}


void PndEmcWaveformBuffer::EraseDataFromDataMap(FairTimeStamp* data) {
	PndEmcWaveformData myData = *(PndEmcWaveformData*)data;
	if (fData_map.find(myData) != fData_map.end())
		fData_map.erase(fData_map.find(myData));
}
