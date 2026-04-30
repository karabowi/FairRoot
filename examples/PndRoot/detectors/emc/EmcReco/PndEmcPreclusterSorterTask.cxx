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
 * PndEmcPreclusterSorterTask.cxx
 */

#include "PndEmcPreclusterSorterTask.h"

#include "PndEmcCluster.h"
#include "PndEmcPrecluster.h"
#include "PndEmcPreclusterRingSorter.h"

#include "TClonesArray.h"

ClassImp(PndEmcPreclusterSorterTask);

PndEmcPreclusterSorterTask::PndEmcPreclusterSorterTask() {
}

PndEmcPreclusterSorterTask::~PndEmcPreclusterSorterTask() {
}


void PndEmcPreclusterSorterTask::AddNewDataToTClonesArray(FairTimeStamp* data)
{

	 FairRootManager* ioman = FairRootManager::Instance();
	 TClonesArray* myArray = ioman->GetTClonesArray(fOutputBranch);
	 if (fVerbose > 1){
		 std::cout << "-I- PndEmcPreclusterSorterTask::AddNewDataToTClonesArray Data: " ;
		 if (fClusterType==0) std::cout << "(Type=" << fClusterType << ") " <<  *(PndEmcCluster*)(data) << std::endl;
		 else if (fClusterType==1) std::cout << "(Type=" << fClusterType << ") " <<  *(PndEmcPrecluster*)(data) << std::endl;

	 }
	 if (fClusterType==0) new ((*myArray)[myArray->GetEntries()]) PndEmcCluster(*(PndEmcCluster*)(data)); // <-- Here (set to use PndEmcCluster)
	 else if (fClusterType==1) new ((*myArray)[myArray->GetEntries()]) PndEmcPrecluster(*(PndEmcPrecluster*)(data)); // <-- and here (set to use PndEmcPrecluster)

}

FairRingSorter* PndEmcPreclusterSorterTask::InitSorter(Int_t numberOfCells, Double_t widthOfCells) const{
	return new PndEmcPreclusterRingSorter(numberOfCells, widthOfCells);
}
