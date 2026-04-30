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

#include "PndEmcOnlineBumpSplitter.h"

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

using std::cout;
using std::endl;
int EmcOnlFaultCounter = 0;
int nTotOnlClus = 0;
static Double_t BumpOnlRtime = 0;
static Double_t BumpOnlCtime = 0;

//----------------
// Constructors --
//----------------

PndEmcOnlineBumpSplitter::PndEmcOnlineBumpSplitter(Int_t verbose)
  : fDigiArray(0), fClusterArray(0), fBumpArray(0), fEmcDigi(0), fGeoPar(new PndEmcGeoPar()), fDigiPar(new PndEmcDigiPar()), fRecoPar(new PndEmcRecoPar()), fPersistance(kTRUE),
    fClusterPosParam(), fSeedEnergyThreshold(0.03), FairTask("PndEmcOnlineBumpSplitter", verbose)
{
  fClusterPosParam.clear();
}

//--------------
// Destructor --
//--------------

PndEmcOnlineBumpSplitter::~PndEmcOnlineBumpSplitter() {}

/**
 * @brief Init Task
 *
 * Prepares the TClonesArray of PndEmcDigi and PndEmcCluster for reading.
 *
 * @return InitStatus
 * @retval kSUCCESS success
 */
InitStatus PndEmcOnlineBumpSplitter::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndEmcOnlineBumpSplitter::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Geometry loading
  fGeoPar->InitEmcMapper();
  PndEmcStructure::Instance();

  // Get input array
  if (FairRunAna::Instance()->IsTimeStamp()) {
    fDigiArray = (TClonesArray *)ioman->GetObject("EmcDigiClusterBase");
  } else {
    fDigiArray = (TClonesArray *)ioman->GetObject("EmcDigi");
  }
  if (!fDigiArray) {
    cout << "-W- PndEmcOnlineBumpSplitter::Init: "
         << "No PndEmcDigi array!" << endl;
    return kERROR;
  }

  fClusterArray = (TClonesArray *)ioman->GetObject("EmcCluster");
  if (!fClusterArray) {
    cout << "-W- PndEmcOnlineBumpSplitter::Init: "
         << "No PndEmcCluster array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fBumpArray = ioman->Register("EmcBumpOnline", "PndEmcBump", "Emc", fPersistance);

  fEmcDigi = ioman->Register("EmcLinkedDigis", "PndEmcDigi", "Emc", kFALSE);

  LOG(info) << " PndEmcOnlineBumpSplitter: Intialization successful";
  return kSUCCESS;
}

/**
 * @brief Runs the task
 *
 * Very fast and simple bump splitter, intended for online usage. For each cluster, the two most energetic digis are retrieved. If they are not neighbours, and the energy
 * difference is above a certain threshold, the cluster is split into bumps. Each bump has as location and timestamp that of the retrieved high-energy digis. The cluster energy is
 * divided according to the energy deposition in those digis.
 *
 * @param opt unused
 * @return void
 */
void PndEmcOnlineBumpSplitter::Exec(Option_t *opt)
{

  // -----   Timer for bump splitting  -------------------------------------
  TStopwatch BumpOnlTimer;
  BumpOnlTimer.Start();

  int nClusters = fClusterArray->GetEntriesFast();
  nTotOnlClus += nClusters;
  PndEmcMapper *fEmcMap = PndEmcMapper::Instance();

  fEmcDigi->Delete();

  Int_t digiCounterStart = 0; // index of 1st digi of cluster in TClonesArray
  Int_t digiCounterEnd = 0;   // index of last digi of cluster in TClonesArray

  // loop over Clusters
  for (Int_t iCluster = 0; iCluster < nClusters; iCluster++) {

    digiCounterStart = digiCounterEnd;

    PndEmcCluster *theCluster = (PndEmcCluster *)fClusterArray->At(iCluster);
    if (theCluster->GetEnergy() < 0.03)
      continue; // skip low energy clusters

    ///// START (Fetch cluster digis using FairLinks)
    FairMultiLinkedData digiLinks = theCluster->GetLinksWithType(FairRootManager::Instance()->GetBranchId("EmcDigiSorted"));

    for (Int_t j = 0; j < digiLinks.GetNLinks(); j++) {

      PndEmcDigi *myDigi = (PndEmcDigi *)FairRootManager::Instance()->GetCloneOfLinkData(digiLinks.GetLink(j));
      if (myDigi) {
        PndEmcDigi *newDigi = new ((*fEmcDigi)[fEmcDigi->GetEntriesFast()]) PndEmcDigi(*myDigi); // load digis into TClonesArray using FairLinks
        delete (myDigi);
        digiCounterEnd++;
      } else {
        if (EmcOnlFaultCounter < 5)
          std::cout << "-E in PndEmcOnlineBumpSplitter::Exec FairLink: " << digiLinks.GetLink(j) << " to EmcDigi delivers null" << std::endl;
      }
    }
    ///// END (Fetch cluster digis using FairLinks)

    Int_t nrOfDigis = digiCounterEnd - digiCounterStart;

    if (nrOfDigis == 0) {
      if (EmcOnlFaultCounter < 5)
        std::cout << "-W- in PndEmcOnlineBumpSplitter::Exec: No valid digis in cluster " << iCluster << "... skipping." << endl;
      EmcOnlFaultCounter++;
      continue; // no digis in this clusters for some reason
    }

    /*		noBumpSplit: {
          PndEmcBump* theNewBump = AddBump();
          theNewBump->MadeFrom(iCluster);
          theNewBump->SetLink(FairLink("EmcCluster", iCluster));
          theNewBump->SetPosition(theCluster->where());
          theNewBump->SetTimeStamp(theCluster->GetTimeStamp());
          theNewBump->SetEnergy(theCluster->GetEnergy());
          continue;
        }*/

    if (nrOfDigis == 1) { // Only 1 digi in cluster
                          //			goto noBumpSplit;
      SingleBumpCluster(theCluster, iCluster);
    } else {

      double *energies = new double[nrOfDigis];
      int *index = new int[nrOfDigis];

      for (Int_t iDigi = digiCounterStart; iDigi < digiCounterEnd; iDigi++) {
        PndEmcDigi *theDigi = (PndEmcDigi *)fEmcDigi->At(iDigi);
        energies[iDigi - digiCounterStart] = theDigi->GetEnergy();
      }

      TMath::Sort(nrOfDigis, energies, index, true);   // sorted in descending order
      if (energies[index[0]] > fSeedEnergyThreshold) { // don't bother splitting if the energy of the most energetic digi is below fSeedEnergyThreshold
        if (energies[index[1]] > energies[index[0]] * 0.5 &&
            energies[index[1]] > fSeedEnergyThreshold) { // if the energy of the second most energetic digi is at least half of that of the most energetic one
          PndEmcDigi *seed1 = (PndEmcDigi *)fEmcDigi->At(index[0] + digiCounterStart);
          PndEmcDigi *seed2 = (PndEmcDigi *)fEmcDigi->At(index[1] + digiCounterStart);
          Double_t E1 = seed1->GetEnergy();
          Double_t E2 = seed2->GetEnergy();

          if (!seed1->isNeighbour(seed2)) { // if the two digis aren't neighbours, split the cluster up
            PndEmcBump *theNewBump1 = AddBump();
            theNewBump1->MadeFrom(iCluster);
            theNewBump1->SetLink(FairLink("EmcCluster", iCluster));
            theNewBump1->SetPosition(seed1->where());
            theNewBump1->SetTimeStamp(seed1->GetTimeStamp());
            theNewBump1->SetEnergy(E1 * (theCluster->GetEnergy()) / (E1 + E2));

            PndEmcBump *theNewBump2 = AddBump();
            theNewBump2->MadeFrom(iCluster);
            theNewBump2->SetLink(FairLink("EmcCluster", iCluster));
            theNewBump2->SetPosition(seed2->where());
            theNewBump2->SetTimeStamp(seed2->GetTimeStamp());
            theNewBump2->SetEnergy(E2 * (theCluster->GetEnergy()) / (E1 + E2));
          } else { // otherwise, make a single bump from this cluster
            // goto noBumpSplit;
            SingleBumpCluster(theCluster, iCluster);
          }
        } else {
          // goto noBumpSplit;
          SingleBumpCluster(theCluster, iCluster);
        }
      } else {
        // goto noBumpSplit;
        SingleBumpCluster(theCluster, iCluster);
      }
      delete[] energies;
      delete[] index;
    }
  }

  BumpOnlTimer.Stop();

  BumpOnlRtime += BumpOnlTimer.RealTime(); // keep track how much time was spent on clustering
  BumpOnlCtime += BumpOnlTimer.CpuTime();
}

/**
 * @brief Adds a new PndEmcBump to fBumpArray and returns it.
 *
 * @return PndEmcBump*
 */
PndEmcBump *PndEmcOnlineBumpSplitter::AddBump()
{
  TClonesArray &clref = *fBumpArray;
  Int_t size = clref.GetEntriesFast();
  return new (clref[size]) PndEmcBump();
}

void PndEmcOnlineBumpSplitter::SingleBumpCluster(PndEmcCluster *myCluster, Int_t iClus)
{
  PndEmcBump *theNewBump = AddBump();
  theNewBump->MadeFrom(iClus);
  theNewBump->SetLink(FairLink("EmcCluster", iClus));
  theNewBump->SetPosition(myCluster->where());
  theNewBump->SetTimeStamp(myCluster->GetTimeStamp());
  theNewBump->SetEnergy(myCluster->GetEnergy());
}

void PndEmcOnlineBumpSplitter::FinishTask()
{
  if (EmcOnlFaultCounter > 5)
    cout << "-W- in PndEmcOnlineBumpSplitter::Exec: No valid digis in cluster - " << EmcOnlFaultCounter << " occurences (" << nTotOnlClus << " clusters in total)" << endl;
  LOG(info) << " PndEmcOnlineBumpSplitter: real time: " << BumpOnlRtime << " s, cpu time: " << BumpOnlCtime << " s.";
}

void PndEmcOnlineBumpSplitter::SetParContainers()
{

  // Get run and runtime database
  FairRun *run = FairRun::Instance();
  if (!run) {
    std::cout << "-E- PndEmcOnlineBumpSplitter::SetParContainers No analysis run" << std::endl;
  }

  FairRuntimeDb *db = run->GetRuntimeDb();
  if (!db) {
	LOG(fatal) << "-E- PndEmcOnlineBumpSplitter::SetParContainers: No runtime database";
  }
  // Get Emc digitisation parameter container
  fGeoPar = (PndEmcGeoPar *)db->getContainer("PndEmcGeoPar");
  // Get Emc digitisation parameter container
  fDigiPar = (PndEmcDigiPar *)db->getContainer("PndEmcDigiPar");
  // Get Emc reconstruction parameter container
  fRecoPar = (PndEmcRecoPar *)db->getContainer("PndEmcRecoPar");
}

ClassImp(PndEmcOnlineBumpSplitter)
