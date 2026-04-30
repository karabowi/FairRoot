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
 * PairFinderTask.cpp
 *
 *  Created on: Jul 22, 2014
 *      Author: Roman Klasen, roklasen@uni-mainz.de or klasen@kph.uni-mainz.de
 */

#include "PndLmdPairFinderTask.h"

#include <PndGeoHandling.h>
#include <PndLmdContFact.h>
#include <PndLmdHitPair.h>
#include <PndLmdGeometryHelper.h>
#include <PndSdsClusterPixel.h>
#include <PndSdsDigiPixel.h>

#include <FairRootManager.h>
#include <FairRun.h>
#include <FairRuntimeDb.h>
#include <TClonesArray.h>

#include <TCanvas.h>
#include <TH1D.h>

// #include <algorithm>
#include <sstream>
#include <vector>

using std::cerr;
using std::cout;

ClassImp(PndLmdPairFinderTask);

/*
 * actually I don't need empty constructors, but fkn root crashes if no empty constructor is present
 */
PndLmdPairFinderTask::PndLmdPairFinderTask()
  : // @suppress("Class members should be properly initialized")
    PndSdsTask("pairfinder")
{
  digiArray = nullptr;
  recoArray = nullptr;
  clusterCandidateArray = nullptr;
  unsuitable = 0;
  _ignoreClusters = false;
}

PndLmdPairFinderTask::PndLmdPairFinderTask(const char *name)
  : // @suppress("Class members should be properly initialized")
    PndSdsTask("pairfinder with name")
{
  digiArray = nullptr;
  recoArray = nullptr;
  clusterCandidateArray = nullptr;

  if (!strcmp(name, ""))
    SetName(name);
  _ignoreClusters = false;
}

PndLmdPairFinderTask::~PndLmdPairFinderTask()
{
  std::cout << "PairFinderTask destructor called."
            << "\n";
}

InitStatus PndLmdPairFinderTask::Init()
{

  noOfGoodPairs = 0;
  noOfEvents = noOfCombos = 0;
  hitsClustered = distanceTooHigh = 0;
  hitsSinglePixel = 0;
  hitsSinglePixel = 0;
  unsuitable = sumOfPixelHits = 0;
  eventMissedAllPlanes = noOverlap = 0;
  plane0 = plane1 = plane2 = plane3 = 0;

  fInBranchName = "LMDPixelDigis";
  fInRecoBranchName = "LMDHitsPixel";
  fOutBranchName = "LMDPixelPairs";
  fInClusterCandidates = "LMDPixelClusterCand";

  fFolderName = "pndsim";

  _maxDistance = 1250e-4;

  SetBranchNames();

  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    std::cout << "-E- LmdPairFinder::Init: "
              << "RootManager not instantiated!"
              << "\n";
    return kFATAL;
  }

  digiArray = (TClonesArray *)ioman->GetObject(fInBranchName);
  recoArray = (TClonesArray *)ioman->GetObject(fInRecoBranchName);
  clusterCandidateArray = (TClonesArray *)ioman->GetObject(fInClusterCandidates);

  if (!digiArray) {
    std::cout << "-W- LmdPairFinder::Init: "
              << "ERROR, branch name " << fInBranchName << " could not found!"
              << "\n";
    return kERROR;
  }

  if (!recoArray) {
    std::cout << "-W- LmdPairFinder::Init: "
              << "ERROR, branch name " << fInRecoBranchName << " not found!"
              << "\n";
    return kERROR;
  }

  if (!clusterCandidateArray) {
    std::cout << "-W- LmdPairFinder::Init: "
              << "ERROR, branch name " << fInClusterCandidates << " not found!"
              << "\n";
    return kERROR;
  }

  hitPairArray = new TClonesArray("PndLmdHitPair");
  ioman->Register("PndLmdHitPair", "PndLmd", hitPairArray, kTRUE);

  helper = &PndLmdGeometryHelper::getInstance();

  std::cout << "LmdPairFinder::Init(): Initialization successful."
            << "\n";
  return kSUCCESS;
}

void PndLmdPairFinderTask::SetBranchNames()
{
  std::cout << "branch names set to " << fInBranchName << "\n";
}

InitStatus PndLmdPairFinderTask::ReInit()
{
  return InitStatus();
}

void PndLmdPairFinderTask::SetParContainers()
{

  FairRun *ana;
  FairRuntimeDb *rtdb;

  std::cout << "PndLmdPixelClusterTask::SetParContainers() "
            << "\n";
  // Get Base Container
  ana = FairRun::Instance();
  rtdb = ana->GetRuntimeDb();

  PndLmdContFact *themvdcontfact = (PndLmdContFact *)rtdb->getContFactory("PndLmdContFact");
  // read params for lumi alignment
  TList *theAlignLMDContNames = themvdcontfact->GetAlignParNames();
  Info("SetParContainers()", "AlignLMD The container names list contains %i entries", theAlignLMDContNames->GetEntries());
  TIter cfAlIter(theAlignLMDContNames);
  while (TObjString *contname = (TObjString *)cfAlIter()) {
    TString parsetname = contname->String();
    Info("SetParContainers()", "%s", parsetname.Data());
  }

  PndGeoHandling::Instance()->SetParContainers();
}

/*
 * Main SensorHit filter. It stores the sensor row and column info to a PndLmdHitPair object,
 * filters for valid hit pairs and stores the decoded hit in LMD coordinate system to the HitPair.
 * The HitPair contains BOTH original row and col hits as well as LMD xyz Coordinates (as TVector3).
 * This consumes a lot of storage, but storage is cheap and for now we want the info.
 */
void PndLmdPairFinderTask::Exec(Option_t *)
{

  // clear temporary array for next event
  hitPairArray->Clear();

  // Int_t nPixels = digiArray->GetEntriesFast();
  noOfEvents++;

  // display some kind of progress
  if ((noOfEvents % 10000) == 0) {
    cout << "processed " << noOfEvents << "\n";
  }

  // ========== loop over recos in recoArray =========
  Int_t nRecos = recoArray->GetEntriesFast();

  Int_t storedPairsPerEvent = 0;

  // try every cluster combination and check
  for (auto iReco = 0; iReco < nRecos; iReco++) {
    for (auto jReco = iReco + 1; jReco < nRecos; jReco++) {

      PndSdsHit *hitOne = (PndSdsHit *)recoArray->At(iReco);
      PndSdsHit *hitTwo = (PndSdsHit *)recoArray->At(jReco);

      int id1 = hitOne->GetSensorID();
      int id2 = hitTwo->GetSensorID();

      // sort hits so hitOne is ALWAYS upstream
      auto &infoOne = helper->getHitLocationInfo(id1);
      auto &infoTwo = helper->getHitLocationInfo(id2);

      if (infoOne.module_side > infoTwo.module_side) {
        std::swap(hitOne, hitTwo);
        id1 = hitOne->GetSensorID();
        id2 = hitTwo->GetSensorID();
      }

      // from here on, the hits are sorted so hitOne is ALWAYS upstream
      const TVector3 vecOneGlobal = hitOne->GetPosition();
      const TVector3 vecTwoGlobal = hitTwo->GetPosition();

      // make PndLmdHitPair and check for data sanity, then store to vector
      PndLmdHitPair pairCanditate(vecOneGlobal, vecTwoGlobal, id1, id2);

      // is the candidate even on an overlapping area?
      if (!candHitsOverlappingArea(pairCanditate)) {
        noOverlap++;
        continue;
      }

      int overlapId = helper->getOverlapIdFromSensorIDs(pairCanditate.getId1(), pairCanditate.getId2());
      pairCanditate.setOverlapId(overlapId);

      int moduleId = helper->getModuleIDFromSensorID(pairCanditate.getId1());
      pairCanditate.setModuleId(moduleId);

      pixelHit pixelHitOne = getPixelHitFromSdsHit(hitOne);
      pixelHit pixelHitTwo = getPixelHitFromSdsHit(hitTwo);

      pairCanditate.setPixelHits(pixelHitOne._col, pixelHitOne._row, pixelHitTwo._col, pixelHitTwo._row);

      pairCanditate.calculateDistance();
      pairCanditate.check();

      if (!pairCanditate.isSane()) {
        pairCanditate.PrintPair();
        cerr << "====              WARNING:                 ===="
             << "\n";
        cerr << "pair seems valid but did not pass sanity check!"
             << "\n";
        cerr << "==============================================="
             << "\n";
        continue;
      }

      // pair must now be sane, in Panda Global and has overlapID et al.
      if (!pairDistanceValid(pairCanditate)) {
        unsuitable++;
        continue;
      }

      // if the pair survived to this point, it's valid. store!
      getStatistics(pairCanditate);
      new ((*hitPairArray)[storedPairsPerEvent]) PndLmdHitPair(pairCanditate);
      storedPairsPerEvent++;
    }
  }
  return;
}

void PndLmdPairFinderTask::FinishEvent() {}

void PndLmdPairFinderTask::FinishTask()
{

  // also, write statistics
  Int_t sumOfAllPlanes = plane0 + plane1 + plane2 + plane3;
  double plane0Percent = ((double)plane0 / noOfGoodPairs) * 100;
  double plane1Percent = ((double)plane1 / noOfGoodPairs) * 100;
  double plane2Percent = ((double)plane2 / noOfGoodPairs) * 100;
  double plane3Percent = ((double)plane3 / noOfGoodPairs) * 100;
  double allPlanesPercent = ((double)sumOfAllPlanes / noOfGoodPairs) * 100;
  double clusterRatio = ((double)hitsClustered / (double)(hitsSinglePixel + hitsClustered)) * 100;
  double pixelsPerEvent = (double)sumOfPixelHits / (double)noOfEvents;
  double goodPairsPerEvent = (double)noOfGoodPairs / (double)noOfEvents;

  cout << "\n";
  cout << "*************************************************************"
       << "\n";
  cout << "                      pair finder done                       "
       << "\n";
  cout << "*************************************************************"
       << "\n";
  cout << "\n";
  cout << "                     counting statistics:"
       << "\n";
  cout << "\n";
  printf("total events: %d \n", noOfEvents);
  printf("events that missed all sensors: %d \n", eventMissedAllPlanes);
  printf("total pixel hits: %d \n", sumOfPixelHits);
  printf("cluster ratio: %.2f %% \n", clusterRatio);
  printf("pixel hits per event: %.2f \n", pixelsPerEvent);
  printf("----------------------------\n");
  printf("possible hit pair combinations: %d \n", noOfCombos);
  printf("no overlap: %d \n", noOverlap);
  printf("distance too high: %d \n", distanceTooHigh);
  printf("----------------------------\n");
  printf("good pairs: %d \n", noOfGoodPairs);
  printf("good pairs per event: %.2f \n", goodPairsPerEvent);
  printf("----------------------------\n");
  printf("hits on plane 0: %.2f %% \n", plane0Percent);
  printf("hits on plane 1: %.2f %%\n", plane1Percent);
  printf("hits on plane 2: %.2f %%\n", plane2Percent);
  printf("hits on plane 3: %.2f %%\n", plane3Percent);
  printf("hits on all planes: %.2f %% (should be 100%%!) \n", allPlanesPercent);
  cout << "\n";
  cout << "*************************************************************"
       << "\n";

  return;
}

void PndLmdPairFinderTask::Register() {}

bool PndLmdPairFinderTask::pairDistanceValid(PndLmdHitPair &candidate)
{

  // check distance squared
  double distance = candidate.getDistance();
  if (distance > _maxDistance) {
    distanceTooHigh++;
    return false;
  }
  return true;
}

void PndLmdPairFinderTask::getStatistics(PndLmdHitPair &candidate)
{

  // check for overlap
  int fplane;

  auto &infoOne = helper->getHitLocationInfo(candidate.getId1());
  fplane = infoOne.plane;

  // count events per plane
  switch (fplane) {
  case 0: plane0++; break;
  case 1: plane1++; break;
  case 2: plane2++; break;
  case 3: plane3++; break;
  default:
    // should never happen, can only indicate decoding error
    cerr << "WARNING: hit was deemed suitable but plane number is " << fplane << "\n";
    cerr << "This should not happen!"
         << "\n";
  }
  noOfGoodPairs++;
}

pixelHit PndLmdPairFinderTask::getPixelHitFromSdsHit(PndSdsHit *sdsHit)
{

  pixelHit result;
  int hitSensorId;
  double row, col;

  int clusterIndex = sdsHit->GetClusterIndex();

  std::vector<pixelCluster> clusters;
  PndSdsClusterPixel *clusterPixelCand = (PndSdsClusterPixel *)clusterCandidateArray->At(clusterIndex);

  int noOfClusters = clusterPixelCand->GetClusterSize();

  for (int iCluster = 0; iCluster < noOfClusters; iCluster++) {

    PndSdsDigiPixel *mcPixel = (PndSdsDigiPixel *)digiArray->At(clusterPixelCand->GetDigiIndex(iCluster));

    if (!mcPixel) {
      exit(1);
    }

    hitSensorId = mcPixel->GetSensorID();

    col = mcPixel->GetPixelColumn();
    row = mcPixel->GetPixelRow();

    // skip decoding errors
    if (col < 0 || row < 0) {
      continue;
    } else {
      clusters.push_back(pixelCluster(pixelHit(hitSensorId, col, row)));
    }
  }

  sumOfPixelHits += clusters.size();

  // all hits are present in clusters

  /*
   * ============ find clusters ============
   * input: vector<pixelCluster>
   * output vector<pixelCluster>
   *
   * algorithm: Hierarchical Clustering Algorithm, see https://en.wikipedia.org/wiki/Hierarchical_clustering
   * start by putting every pixel hit in a separate cluster (done above).
   * then merge every two clusters that are close enough (1-2 pixels, may be open to adjustment).
   * terminate if no more clusters can be merged or after N iterations for N pixel hits.
   * all remaining clusters contain every pixel hit.
   */

  // for every cluster, check every other cluster
  for (size_t i = 0; i < clusters.size(); i++) {
    // clusters are interchangeable, check every pair only once
    for (size_t j = i + 1; j < clusters.size(); j++) {

      // clusters must be on same sensor
      if (clusters[i]._sensorId != clusters[j]._sensorId) {
        continue;
      }

      if (clusters[i].isNeighbour(clusters[j])) {
        clusters[i].merge(clusters[j]);
        clusters.erase(clusters.begin() + j);
        j--;
      }
    }
  }

  // calculate cluster centers and discard large clusters
  // for statistis: count cluster ratio
  for (size_t i = 0; i < clusters.size(); i++) {
    clusters[i].calculateCenter();

    if (clusters[i].clusterSize > 1) {
      hitsClustered++;

      if (_ignoreClusters || clusters[i].clusterSize > 3) {
        clusters.erase(clusters.begin() + i);
        /*
         * this is important! when you erase cluster i, cluster i+1 becomes cluster i, but the first i
         * becomes i+1 itself.
         * that means, cluster i (former i+1) never gets checked in the first line of the outer for loop!
         */
        i--;
      }
    } else {
      hitsSinglePixel++;
    }
  }
  return pixelHit(hitSensorId, clusters[0].centerCol, clusters[0].centerRow);
}

bool PndLmdPairFinderTask::candHitsOverlappingArea(const PndLmdHitPair &candidate)
{
  int firstSensorId, secondSensorId;
  firstSensorId = candidate.getId1();
  secondSensorId = candidate.getId2();

  // same sensor hit?
  if (firstSensorId == secondSensorId) {
    return false;
  }
  return helper->isOverlappingArea(firstSensorId, secondSensorId);
}

void PndLmdPairFinderTask::Reset() {}
