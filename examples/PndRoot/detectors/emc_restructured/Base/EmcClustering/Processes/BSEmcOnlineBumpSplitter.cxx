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
//	Class BSEmcOnlineBumpSplitter.
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

#include "BSEmcOnlineBumpSplitter.h"

#include <algorithm>

#include "TMathBase.h"
#include "TStopwatch.h"
#include "TVector3.h"

#include "FairLink.h"
#include "FairParSet.h"
#include "fairlogger/Logger.h"

#include "PndContainerI.h"
#include "PndMutableContainerI.h"
#include "PndParameterRegister.h"

#include "BSEmcCluster.h"

#include "BSEmcCrystalPositionPar.h"
#include "BSEmcDigi.h"
#include "BSEmcGeoNeighbouringRelationPar.h"
#include "BSEmcSubCluster.h"

int EmcOnlFaultCounter = 0;
int nTotOnlClus = 0;
static Double_t BumpOnlRtime = 0;
static Double_t BumpOnlCtime = 0;

BSEmcOnlineBumpSplitter::BSEmcOnlineBumpSplitter() : PndProcess{"BSEmcOnlineBumpSplitter"}, fNeighbouringRelationParName(), fPositionParName() {}

BSEmcOnlineBumpSplitter::~BSEmcOnlineBumpSplitter() {}

void BSEmcOnlineBumpSplitter::SetDetectorName(const std::string &t_detectorName)
{
  fDetectorName = t_detectorName;
  if (fPositionParName == "") {
    fPositionParName = BSEmcCrystalPositionPar::fgParameterName + fDetectorName;
  }
  if (fNeighbouringRelationParName == "") {
    fNeighbouringRelationParName = BSEmcGeoNeighbouringRelationPar::fgParameterName + fDetectorName;
  }

  fParameterList.push_back(fNeighbouringRelationParName);
  fParameterList.push_back(fPositionParName);
}

void BSEmcOnlineBumpSplitter::SetupParameters(const PndParameterRegister *t_paramRegister)
{
  fNeighbouringRelationPar = dynamic_cast<BSEmcGeoNeighbouringRelationPar *>(t_paramRegister->GetParameter(fNeighbouringRelationParName.c_str()));
  fPositionPar = dynamic_cast<BSEmcCrystalPositionPar *>(t_paramRegister->GetParameter(fPositionParName.c_str()));
}

void BSEmcOnlineBumpSplitter::RequestDataContainer(PndContainerRegister *t_register)
{
  t_register->Request({fDigiBranchname, "BSEmcDigi", kFALSE, kFALSE});
  t_register->Request({fClusterBranchname, "BSEmcCluster", kTRUE, kTRUE});
  t_register->Request({fSubClusterBranchname, "BSEmcSubCluster", kTRUE, kTRUE});
}

void BSEmcOnlineBumpSplitter::GetDataContainer(PndContainerRegister *t_register)
{
  fDigiArray = t_register->GetInput<BSEmcDigi>(fDigiBranchname);
  fClusterArray = t_register->GetOutput<BSEmcCluster>(fClusterBranchname);
  fSubClusterArray = t_register->GetOutput<BSEmcSubCluster>(fSubClusterBranchname);

  if (fDigiBranchname != "") {
    t_register->SetAsDefaultBranchFor(fDigiBranchname, "BSEmcDigi");
  }
  if (fClusterBranchname != "") {
    t_register->SetAsDefaultBranchFor(fClusterBranchname, "BSEmcCluster");
  }
  if (fSubClusterBranchname != "") {
    t_register->SetAsDefaultBranchFor(fSubClusterBranchname, "BSEmcSubCluster");
  }
}

void BSEmcOnlineBumpSplitter::Process()
{
  fSubClusterArray->Reset();

  std::vector<BSEmcSubCluster *> subclusterCont = Subcluster(fDigiArray->GetVectorOfPtrToConst(), fClusterArray->GetVector());
  for (BSEmcSubCluster *subcluster : subclusterCont) {
    fSubClusterArray->CreateCopy(*subcluster);
    delete subcluster;
  }
}

/**
 * @brief Runs the task
 *
 * Very fast and simple bump splitter, intended for online usage. For each cluster, the two most energetic digis are retrieved.
 * If they are not neighbours, and the energy difference is above a certain threshold, the cluster is split into bumps.
 * Each bump has as location and timestamp that of the retrieved high-energy digis. The cluster energy is divided according to the energy deposition in those digis.
 *
 * @param opt unused
 * @return void
 */
std::vector<BSEmcSubCluster *> BSEmcOnlineBumpSplitter::Subcluster(const std::vector<const BSEmcDigi *> &t_digiCont, const std::vector<BSEmcCluster *> &t_clusterCont)
{
  fSubClusterVec.clear();
  // -----   Timer for bump splitting  -------------------------------------
  TStopwatch BumpOnlTimer;
  BumpOnlTimer.Start();

  int nClusters = t_clusterCont.size();
  nTotOnlClus += nClusters;
  Int_t nrOfDigis = t_digiCont.size();
  // loop over Clusters
  for (Int_t iCluster = 0; iCluster < nClusters; iCluster++) {
    BSEmcCluster *theCluster = t_clusterCont.at(iCluster);
    if (theCluster->GetEnergy() < 0.03) {
      continue; // skip low energy clusters
    }

    if (nrOfDigis == 0) {
      if (EmcOnlFaultCounter < 5) {
        LOG(warn) << "BSEmcOnlineBumpSplitter::Exec: No valid digis in cluster " << iCluster << "... skipping.";
      }
      EmcOnlFaultCounter++;
      continue; // no digis in this clusters for some reason
    }

    if (nrOfDigis == 1) { // Only 1 digi in cluster
      SingleBumpCluster(theCluster, iCluster, t_digiCont);
    } else {

      double *energies = new double[nrOfDigis];
      int *index = new int[nrOfDigis];

      for (Int_t iDigi = 0; iDigi < nrOfDigis; iDigi++) {
        const BSEmcDigi *theDigi = t_digiCont.at(iDigi);
        energies[iDigi] = theDigi->GetEnergy();
      }

      TMath::Sort(nrOfDigis, energies, index, true);   // sorted in descending order
      if (energies[index[0]] > fSeedEnergyThreshold) { // don't bother splitting if the energy of the most energetic digi is below fSeedEnergyThreshold
        if (energies[index[1]] > energies[index[0]] * 0.5 &&
            energies[index[1]] > fSeedEnergyThreshold) { // if the energy of the second most energetic digi is at least half of that of the most energetic one
          const BSEmcDigi *seed1 = t_digiCont.at(index[0]);
          const BSEmcDigi *seed2 = t_digiCont.at(index[1]);
          Double_t E1 = seed1->GetEnergy();
          Double_t E2 = seed2->GetEnergy();

          if (!fNeighbouringRelationPar->AreNeighbours(seed1->GetDetectorId(), seed2->GetDetectorId())) { // if the two digis aren't neighbours, split the cluster up
            BSEmcSubCluster *theNewBump1 = AddBump();
            theNewBump1->SetParentClusterIndex(iCluster);
            theNewBump1->SetLink(FairLink("EmcCluster", iCluster));
            TVector3 seed1Position = fPositionPar->GetPositionData(seed1->GetDetectorId()).GetCentre();
            theNewBump1->SetPosition(seed1Position);
            theNewBump1->SetTimeStamp(seed1->GetTimeStamp());
            theNewBump1->SetEnergy(E1 * (theCluster->GetEnergy()) / (E1 + E2));
            Double_t weight1 = E1 / (E1 + E2);
            for (const BSEmcDigiInfo_t &digi : theCluster->GetDigis()) {
              theNewBump1->AddDigi({digi.fDigiIdx, digi.fDetectorId, weight1});
              theNewBump1->AddDigiLink(t_digiCont.at(digi.fDigiIdx)->GetEntryNr());
            }
            BSEmcSubCluster *theNewBump2 = AddBump();
            theNewBump2->SetParentClusterIndex(iCluster);
            theNewBump2->SetLink(FairLink("EmcCluster", iCluster));
            TVector3 seed2Position = fPositionPar->GetPositionData(seed2->GetDetectorId()).GetCentre();
            theNewBump2->SetPosition(seed2Position);
            theNewBump2->SetTimeStamp(seed2->GetTimeStamp());
            theNewBump2->SetEnergy(E2 * (theCluster->GetEnergy()) / (E1 + E2));
            Double_t weight2 = E2 / (E1 + E2);
            for (const BSEmcDigiInfo_t &digi : theCluster->GetDigis()) {
              theNewBump2->AddDigi({digi.fDigiIdx, digi.fDetectorId, weight2});
              theNewBump2->AddDigiLink(t_digiCont.at(digi.fDigiIdx)->GetEntryNr());
            }
          } else { // otherwise, make a single bump from this cluster
            // goto noBumpSplit;
            SingleBumpCluster(theCluster, iCluster, t_digiCont);
          }
        } else {
          // goto noBumpSplit;
          SingleBumpCluster(theCluster, iCluster, t_digiCont);
        }
      } else {
        // goto noBumpSplit;
        SingleBumpCluster(theCluster, iCluster, t_digiCont);
      }
      delete[] energies;
      delete[] index;
    }
  }

  BumpOnlTimer.Stop();

  BumpOnlRtime += BumpOnlTimer.RealTime(); // keep track how much time was spent on clustering
  BumpOnlCtime += BumpOnlTimer.CpuTime();

  std::sort(fSubClusterVec.begin(), fSubClusterVec.end(), [](const BSEmcSubCluster *a, const BSEmcSubCluster *b) { return a->GetTimeStamp() < b->GetTimeStamp(); });
  return fSubClusterVec;
}

/**
 * @brief Adds a new PndEmcBump to fBumpArray and returns it.
 *
 * @return PndEmcBump*
 */
BSEmcSubCluster *BSEmcOnlineBumpSplitter::AddBump()
{
  BSEmcSubCluster *subcluster = new BSEmcSubCluster();
  fSubClusterVec.push_back(subcluster);
  return subcluster;
}

void BSEmcOnlineBumpSplitter::SingleBumpCluster(BSEmcCluster *myCluster, Int_t iClus, const std::vector<const BSEmcDigi *> &t_digiCont)
{
  BSEmcSubCluster *theNewBump = AddBump();
  theNewBump->SetParentClusterIndex(iClus);
  theNewBump->SetLink(FairLink("EmcCluster", iClus));
  theNewBump->SetPosition(myCluster->GetPosition());
  theNewBump->SetTimeStamp(myCluster->GetTimeStamp());
  theNewBump->SetEnergy(myCluster->GetEnergy());
  for (const BSEmcDigiInfo_t &digi : myCluster->GetDigis()) {
    theNewBump->AddDigi(digi);
    theNewBump->AddDigiLink(t_digiCont.at(digi.fDigiIdx)->GetEntryNr());
  }
}

void BSEmcOnlineBumpSplitter::PostProcess()
{
  if (EmcOnlFaultCounter > 5) {
    LOG(warn) << "BSEmcOnlineBumpSplitter::Exec: No valid digis in cluster - " << EmcOnlFaultCounter << " occurences (" << nTotOnlClus << " clusters in total)";
  }
  LOG(debug) << "BSEmcOnlineBumpSplitter: real time: " << BumpOnlRtime << " s, cpu time: " << BumpOnlCtime << " s.";
}

ClassImp(BSEmcOnlineBumpSplitter)
