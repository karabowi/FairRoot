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
// Description:
//	Class PndEmcOnlineBumpSplitter.
//      Implementation of OnlineBumpSplitter which splits clusters based on
//      local maxima, determined in a 0th order approximation, i.e. by looking
//		for the two most energetic digis and checking if they are neighbours.
//
// Environment:
//	Software developed for the PANDA experiment at GSI.
//
// Author List:
//      Marcel Tiemens
//------------------------------------------------------------------------

#include "PndEmcInspectClusters.h"

#include "PndEmcClusterProperties.h"
#include "PndEmcTwoCoordIndex.h"
#include "PndEmcDigi.h"
#include "PndEmcCluster.h"
#include "PndEmcBump.h"
#include "PndEmcRecoPar.h"
#include "PndEmcGeoPar.h"
#include "PndEmcDigiPar.h"
#include "PndEmcStructure.h"
#include "PndEmcMapper.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TStopwatch.h"

#include <algorithm>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;

//----------------
// Constructors --
//----------------

PndEmcInspectClusters::PndEmcInspectClusters(Int_t verbose)
  : fClusterArray(0), fGeoPar(new PndEmcGeoPar()), fDigiPar(new PndEmcDigiPar()), fRecoPar(new PndEmcRecoPar()), fClusterPosParam(), fNrOfDigis(0), fNrOfEvents(0),
    fEventCounter(0), fClusterCounter(0), stoptime(0), nInspProg(0), FairTask("PndEmcInspectClusters", verbose)
{
  fClusterPosParam.clear();
}

//--------------
// Destructor --
//--------------

PndEmcInspectClusters::~PndEmcInspectClusters() {}

/**
 * @brief Init Task
 *
 * Prepares the TClonesArray of PndEmcCluster for reading.
 *
 * @return InitStatus
 * @retval kSUCCESS success
 */
InitStatus PndEmcInspectClusters::Init()
{

  LOG(info) << " PndEmcInspectClusters: start Init()";
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndEmcInspectClusters::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get nr of events
  TTree *tIn = (ioman->GetInTree());
  fNrOfEvents = tIn->GetEntriesFast();
  fNrOfDigis = tIn->Draw("EmcClusterTemp.fEnergy>>hist", "", "goff");

  // Get input array
  fClusterArray = (TClonesArray *)ioman->GetObject("EmcClusterTemp");
  if (!fClusterArray) {
    cout << "-W- PndEmcInspectClusters::Init: "
         << "No PndEmcCluster array!" << endl;
    return kERROR;
  }

  // declare histograms to be filled
  hEventMultiplicity = new TH1I("hEventMultiplicity", "Nr of events per clusters", 20, 0, 20);

  cout << "=> " << fNrOfDigis << " clusters in " << fNrOfEvents << " events." << endl;

  fNrOfDigis /= 100; // to save a calculation step for the progress counter

  LOG(info) << " PndEmcInspectClusters: Intialization successful";
  return kSUCCESS;
}

/**
 * @brief Runs the task
 *
 * Draws EmcClusters and displays their content.
 *
 * @param opt unused
 * @return void
 */
void PndEmcInspectClusters::Exec(Option_t *opt)
{

  fTimer.Start(kFALSE);
  int time = 0; // integer to make estimated time an integer (printing digits doesn't make sense for the estimate)
  double starttime = 0;
  double factor = 0;

  Int_t nClusters = fClusterArray->GetEntriesFast();
  Int_t evtno = 0;
  Bool_t dontAdd = false;
  std::vector<Int_t> evtNrVect;
  for (int j = 0; j < nClusters; j++) {
    PndEmcCluster *myCluster = (PndEmcCluster *)fClusterArray->At(j);

    if (fClusterCounter - 1 == nInspProg * fNrOfDigis) {
      nInspProg += 1;
    }
    fClusterCounter++;
    starttime = fTimer.RealTime();
    fTimer.Continue();
    if (starttime - stoptime > 1) { // update timer every 1 s
      if (fClusterCounter)
        factor = (fNrOfDigis * 100 - fClusterCounter) / fClusterCounter; // (total #clusters - current #clusters = #clusters still needed to process)
      time = factor * starttime;                                         // * (time needed to process current #clusters / current #clusters = average time per cluster)
      cout << "\r                                                                                         " << std::flush;
      cout << "\r[INFO\t] PndEmcInspectClusters: " << nInspProg << "\% completed (about " << time << " seconds remaining)" << std::flush;
      stoptime = starttime;
    }

    FairMultiLinkedData digiLinks = myCluster->GetLinksWithType(FairRootManager::Instance()->GetBranchId("EmcDigiSorted"));

    for (int iDigi = 0; iDigi < digiLinks.GetNLinks(); iDigi++) {
      PndEmcDigi *theDigi = (PndEmcDigi *)FairRootManager::Instance()->GetCloneOfLinkData(digiLinks.GetLink(iDigi));
      if (theDigi) {
        evtno = theDigi->GetEventNr();
        dontAdd = false; // begin by assuming the current evt nr is not used yet
        for (int iNr = 0; iNr < evtNrVect.size(); iNr++) {
          if (evtno == evtNrVect[iNr]) { // is the current evt nr is used,
            dontAdd = true;              // tag that it shouldn't be added to the vector
            break;
          }
        }
        if (!dontAdd)
          evtNrVect.push_back(evtno); // if this evt nr is not used yet, add it to the vector with unique evt nrs
      } else {
        cout << "-E in PndEmcInspectClusters::Exec FairLink: " << digiLinks.GetLink(iDigi) << " to EmcDigi delivers null" << std::endl;
      }
    }
  }
  // sort(evtNrVect.begin(), evtNrVect.end());
  // evtNrVect.erase( unique(evtNrVect.begin(), evtNrVect.end()), evtNrVect.end());
  hEventMultiplicity->Fill(evtNrVect.size());
  fEventCounter++;
}

void PndEmcInspectClusters::FinishTask()
{

  cout << "[INFO\t] PndEmcInspectClusters: " << fEventCounter << "\ttimebunches processed." << endl;

  TCanvas *cN = new TCanvas("cN", "Multiplicity", 1);
  cN->cd(1);
  hEventMultiplicity->Draw("");
}

void PndEmcInspectClusters::SetParContainers()
{

  // Get run and runtime database
  FairRun *run = FairRun::Instance();
  if (!run) {
	LOG(fatal) << "-E- PndEmcInspectClusters::SetParContainers: No analysis run";
  }

  FairRuntimeDb *db = run->GetRuntimeDb();
  if (!db)
	LOG(fatal) << "-E- PndEmcInspectClusters::SetParContainers: No runtime database";
  // Get Emc digitisation parameter container
  fGeoPar = (PndEmcGeoPar *)db->getContainer("PndEmcGeoPar");
  // Get Emc digitisation parameter container
  fDigiPar = (PndEmcDigiPar *)db->getContainer("PndEmcDigiPar");
  // Get Emc reconstruction parameter container
  fRecoPar = (PndEmcRecoPar *)db->getContainer("PndEmcRecoPar");
}

ClassImp(PndEmcInspectClusters)
