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
//	Class Emc2DLocMaxMaxFinder.
//      Searches for local maxima in a cluster based on the ratio
//      between the energy of the maxima crystal and that of
//      its neighbours
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Author List:
//      Phil Strother
//      Helmut Schmuecker
//
// Copyright Information:
//	Copyright (C) 1997	            Imperial College
// Modified:
// M. Babai
//------------------------------------------------------------------------

#include "PndEmc2DLocMaxFinderTEST.h"

#include "PndEmcClusterProperties.h"
#include "PndEmcTwoCoordIndex.h"
#include "PndEmcDigi.h"
#include "PndEmcCluster.h"
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

#include <algorithm>
#include <iostream>

using std::cout;
using std::endl;
int EmcFaultCounter = 0;
int nTotClus = 0;

//----------------
// Constructors --
//----------------

PndEmc2DLocMaxFinderTEST::PndEmc2DLocMaxFinderTEST(Int_t verbose)
  : fClusterArray(0), fDigiArray(0), fGeoPar(new PndEmcGeoPar()), fDigiPar(new PndEmcDigiPar()), fRecoPar(new PndEmcRecoPar()), fPersistance(kFALSE), fMaxECut(0),
    fNeighbourECut(0), fCutSlope(0), fCutOffset(0), fERatioCorr(0), fTheNeighbourLevel(0), FairTask("PndEmc2DLocMaxFinderTEST", verbose), fEmcDigi(0) /////
{
}

//--------------
// Destructor --
//--------------

PndEmc2DLocMaxFinderTEST::~PndEmc2DLocMaxFinderTEST()
{
  //   delete fGeoPar;
  //   delete fDigiPar;
  //   delete fRecoPar;
}

/**
 * @brief Init Task
 *
 * Prepares the TClonesArray of PndEmcDigi and PndEmcCluster for reading.
 *
 * @return InitStatus
 * @retval kSUCCESS success
 */
InitStatus PndEmc2DLocMaxFinderTEST::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndEmcMakeBump::Init: "
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
    cout << "-W- PndEmc2DLocMaxFinder::Init: "
         << "No PndEmcDigi array!" << endl;
    return kERROR;
  }

  fClusterArray = (TClonesArray *)ioman->GetObject("EmcCluster");
  if (!fClusterArray) {
    cout << "-W- PndEmc2DLocMaxFinder::Init: "
         << "No PndEmcCluster array!" << endl;
    return kERROR;
  }

  fEmcDigi = ioman->Register("EmcLinkedDigis", "PndEmcDigi", "Emc", kFALSE);

  fMaxECut = fRecoPar->GetMaxECut();
  fNeighbourECut = fRecoPar->GetNeighbourECut();
  fCutSlope = fRecoPar->GetCutSlope();
  fCutOffset = fRecoPar->GetCutOffset();
  fERatioCorr = fRecoPar->GetERatioCorr();
  //                                            ^
  //                                            |  ....             /
  //                                         1.0|   ....           /|
  //                                            |                 / | fCutSlope
  //                                            |  splitoffs     /  |
  //          MaxE of neighbours - fERatioCorr  |    and        /___|
  // ERatio =  -------------------------------  |  hadrons     /..
  //                    MaxE-fERatioCorr        |             / ..
  //                                            |  ....      /  ..
  //                                            |   ....    /  ...  <-- merged pions
  //                                            |          /  ....      and photons
  //                                            |  ...    /   ....
  //                                         0.0|        /    ....
  //                                            |------------------->
  //                                             0     /     6 7 8
  //                                            <------->             number of neighbours
  //                                            fCutOffset             with energy > fNeighbourECut

  fTheNeighbourLevel = fRecoPar->GetTheNeighbourLevel();
  // This is the range of the search to figure out whether a particular
  // digi is a local max or not.    Which neighbours are
  // looked at is decided by this parameter:
  //        0 = nearest neighbours (this is all logical neighbours including corners)
  //        1 = nearest and next nearest neighbours
  //        etc.

  LOG(info) << " PndEmc2DLocMaxFinder: Intialization successful";
  return kSUCCESS;
}

/**
 * @brief Runs the task
 *
 * For each cluster the local maxima digis are determined and added to the cluster.
 *
 * @param opt unused
 * @return void
 */
void PndEmc2DLocMaxFinderTEST::Exec(Option_t *opt)
{
  int nClusters = fClusterArray->GetEntriesFast();
  PndEmcCoordIndexSet tmp_CoordSet_set;
  nTotClus += nClusters;
  PndEmcMapper *fEmcMap = PndEmcMapper::Instance();

  fEmcDigi->Delete();

  Int_t digiCounterStart = 0; ///// // index of 1st digi of cluster in TClonesArray
  Int_t digiCounterEnd = 0;   ///// // index of last digi of cluster in TClonesArray

  // loop over Clusters
  for (Int_t iCluster = 0; iCluster < nClusters; iCluster++) {

    digiCounterStart = digiCounterEnd; /////
    Double_t E_max = 0;                ///// // highest digi energy in cluster
    Int_t biggest = 0;                 ///// // index of that digi

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
        if (EmcFaultCounter < 5)
          std::cout << "-E in PndEmc2DLocMaxFinderTEST::Exec FairLink: " << digiLinks.GetLink(j) << " to EmcDigi delivers null" << std::endl;
      }
    }
    ///// END (Fetch cluster digis using FairLinks)

    if (fEmcDigi->GetEntriesFast() == 0) { /////
      if (EmcFaultCounter < 5)
        std::cout << "-W- in PndEmc2DLocMaxFinderTEST::Exec: No valid digis in cluster " << iCluster << "... skipping." << endl; /////
      EmcFaultCounter++;                                                                                                         /////
      continue;                                                                                                                  ///// // no digis in this clusters for some reason
    }

    // map <detId,digiIndex>
    //		std::map<Int_t, Int_t> theClustersDigis = theCluster->MemberDigiMap();
    //		std::map<Int_t, Int_t>::iterator theDigiIterator = theClustersDigis.begin();

    //		PndEmcCoordIndexSet allTheNeighbours;
    //		PndEmcCoordIndexSet theNewNeighbours;
    PndEmcCoordIndexSet theCurrentDigi;  ///// replaces theNewNeighbours. Unclear why it is called like that, as it only seems to contain the TCI of the current digi
    std::vector<Int_t> allTheNeighbours; /////

    //		if (theClustersDigis.size()==1){
    if (digiCounterEnd - digiCounterStart == 1) {                         ///// Only 1 digi in cluster
                                                                          //			while( theDigiIterator != theClustersDigis.end() ){
                                                                          //				theCluster->addLocalMax(fDigiArray, theDigiIterator->second);
                                                                          //				++theDigiIterator;
                                                                          //			}
      PndEmcDigi *theDigi = (PndEmcDigi *)fEmcDigi->At(digiCounterStart); /////
      theCluster->addLocalMax(fEmcDigi, digiCounterStart);                /////
    } else {
      for (Int_t iDigi = digiCounterStart; iDigi < digiCounterEnd; iDigi++) {

        PndEmcDigi *theDigi = (PndEmcDigi *)fEmcDigi->At(iDigi); /////
        Int_t detId = theDigi->GetDetectorId();                  /////

        //			while( theDigiIterator != theClustersDigis.end() ){
        //				Int_t detId = theDigiIterator->first;
        PndEmcTwoCoordIndex *theTCI = fEmcMap->GetTCI(detId);

        allTheNeighbours.clear();
        theCurrentDigi.clear();
        theCurrentDigi.insert(theTCI);

        //				theNewNeighbours.clear();

        // Start the newneighbours off, then get all the neighbours from
        // this.  This is kind of curious, since the seed digi shouldn't
        // end up in the neighbour list, but it should work
        //				theNewNeighbours.insert(theTCI);

        /*				getNeighbourDigis(allTheNeighbours, theNewNeighbours,
                  fTheNeighbourLevel, theClustersDigis);
                PndEmcDigi *theDigi = (PndEmcDigi *) fDigiArray->At(theDigiIterator->second);
                if (isALocalMax(theDigi, theCluster, allTheNeighbours)){
                  theCluster->addLocalMax( fDigiArray, theDigiIterator->second); */

        getNeighbourDigis(allTheNeighbours, theCurrentDigi, fTheNeighbourLevel, digiCounterStart, digiCounterEnd); /////
        if (isALocalMax(theDigi, allTheNeighbours)) {                                                              /////
          theCluster->addLocalMax(fEmcDigi, iDigi);                                                                /////
        }
        //				++theDigiIterator;

        if (E_max < theDigi->GetEnergy()) { ///// // Get max digi energy in cluster here, as PndEmcCluster::Maxima is not compatible with timebased sim
          E_max = theDigi->GetEnergy();     /////
          biggest = iDigi;                  /////
        }
      }
      // If no local maxima was added simply add the absolute maxima
      if ((theCluster->LocalMaxMap()).size() == 0) {
        //				theCluster->addLocalMax(theCluster->Maxima(fDigiArray));
        theCluster->addLocalMax(fEmcDigi, biggest); /////
      }
    }
  }
}

/**
 * @brief Check if digi is a local maximum in its cluster.
 *
 * Determines if @p theDigi is a local maximum amongst its neigbors given in
 * @p amongstTheseNeighbours. The digi not only needs to have a higher energy than the
 * neighbors, but has to also be above PndEmcRecoPar::GetMaxECut() and conditions for
 * the ratio of digi energy and neighbour energies have to be fullfilled (see graph/comment in Init()).
 *
 * @param theDigi Digi to check
 * @param amongstTheseNeighbours Neighbours of the digi
 * @return bool
 * @retval true Digi fullfills conditions to be considered a local maximum.
 */
// bool PndEmc2DLocMaxFinder::isALocalMax( const PndEmcDigi *const theDigi, const PndEmcCluster * const theCluster,
//                                        const PndEmcCoordIndexSet &amongstTheseNeighbours ) const
bool PndEmc2DLocMaxFinderTEST::isALocalMax(const PndEmcDigi *const theDigi,
                                           std::vector<Int_t> &amongstTheseNeighbours) const /////
{
  // Loop over all our neighbours and check to see if the one in hand is a local max

  Bool_t result = true;
  Double_t theDigiEnergy = theDigi->GetEnergy();

  if (theDigiEnergy < fMaxECut) {
    // std::cout << "Digi is not a local max because its energy "
    //    << theDigiEnergy << " is too low. " << std::endl;
    result = false;
  } else {
    //		PndEmcCoordIndexSet::const_iterator theNeighbourIterator = amongstTheseNeighbours.begin();
    Int_t theNeighbourIterator = 0; /////

    //		const std::map<Int_t, Int_t> theClustersDigis = theCluster->MemberDigiMap();

    Double_t numberOFneighbours(0.0);
    Double_t neighbourMaxE(0.0);

    while ((theNeighbourIterator != amongstTheseNeighbours.size()) && result) {

      //			std::map<Int_t, Int_t>::const_iterator position = theClustersDigis.find((*theNeighbourIterator)->Index());

      //			if (position != theClustersDigis.end()) {
      //				PndEmcDigi *digi = (PndEmcDigi *) fDigiArray->At(position->second);
      if (amongstTheseNeighbours[theNeighbourIterator] != -1) {                                      /////
        PndEmcDigi *digi = (PndEmcDigi *)fEmcDigi->At(amongstTheseNeighbours[theNeighbourIterator]); /////
        double digiE(digi->GetEnergy());
        if (digiE > theDigiEnergy)
          result = false;
        if (digiE >= neighbourMaxE)
          neighbourMaxE = digiE;
        if (digiE > fNeighbourECut)
          numberOFneighbours += 1.0;
      }
      ++theNeighbourIterator;
    }

    if (numberOFneighbours == 0.0)
      return false;
    else {
      if (fERatioCorr >= fNeighbourECut) {
        std::cout << "Hi this is warning from your PndEmc2DLocMaxFinder,\n"
                  << " please choose a smaller value for fERatioCorr (EmcMakeBump)" << std::endl;
        return false;
      } else {
        if (fCutSlope * (numberOFneighbours - fCutOffset) < (neighbourMaxE - fERatioCorr) / (theDigiEnergy - fERatioCorr))
          result = false;
      }
    }
  }

  if ((result) && (fVerbose > 1)) {
    std::cout << " Digi at (" << theDigi->GetThetaInt() << ", " << theDigi->GetPhiInt() << ") was a local max. Energy = " << theDigi->GetEnergy() << std::endl;
  }
  return result;
}

/**
 * @brief Get the indices of neighbour digis
 *
 * @param[out] allDigiNeighbours vector with indices of the neighbours
 * @param[in,out] currentDigiNeighbours TCI of the digi for which the neighbours are to be found. Gets cleared.
 * @param neighbourLevel ignored
 * @return void
 */
/*void PndEmc2DLocMaxFinder::getNeighbourDigis( PndEmcCoordIndexSet &allDigiNeighbours,
              PndEmcCoordIndexSet &currentDigiNeighbours,
              int neighbourLevel,
              std::map<Int_t,Int_t> theClusterDigis ) const */
void PndEmc2DLocMaxFinderTEST::getNeighbourDigis(std::vector<Int_t> &allDigiNeighbours, PndEmcCoordIndexSet &currentDigiNeighbours, int neighbourLevel, Int_t digiCountStart,
                                                 Int_t digiCountEnd) const /////
{
  PndEmcMapper *fEmcMap = PndEmcMapper::Instance();

  PndEmcCoordIndexSet currentDigisCopy(currentDigiNeighbours);
  //	currentDigiNeighbours.clear(); ///// this is already being cleared in Exec()
  PndEmcCoordIndexSet::iterator theCurrentDigiIterator = currentDigisCopy.begin();
  //	PndEmcCoordIndexSet theNextDigiNeighbours; ///// NOT USED?

  while (theCurrentDigiIterator != currentDigisCopy.end()) {
    PndEmcTwoCoordIndex *theCurrentTCI = *theCurrentDigiIterator;

    //		std::map<Int_t, Int_t>::const_iterator theDigiIterator = theClusterDigis.begin();
    Int_t theDigiIterator = digiCountStart;

    //		while( theDigiIterator != theClusterDigis.end() ) {
    while (theDigiIterator != digiCountEnd) {
      //			Int_t detId = theDigiIterator->first;
      PndEmcDigi *thisDigi = (PndEmcDigi *)fEmcDigi->At(theDigiIterator);
      Int_t detId = thisDigi->GetDetectorId();
      PndEmcTwoCoordIndex *theTCI = fEmcMap->GetTCI(detId);
      bool isneighbour = theCurrentTCI->IsNeighbour(theTCI);
      if (isneighbour)                                ///// EXTEND TO INCLUDE TIMESTAMP?
                                                      //				allDigiNeighbours.insert(theTCI);
        allDigiNeighbours.push_back(theDigiIterator); ///// (after this, allDigiNeighbours has as size the number of digis in this cluster, and the indices of the digis which are
                                                      ///neighbours of the digi under investigation)
      else                                            /////
        allDigiNeighbours.push_back(-1);              /////
      ++theDigiIterator;
    }
    ++theCurrentDigiIterator;
  }
}

void PndEmc2DLocMaxFinderTEST::FinishTask()
{
  if (EmcFaultCounter > 0)
    cout << "-W- in PndEmc2DLocMaxFinderTEST::Exec: No valid digis in cluster - " << EmcFaultCounter << " occurences (" << nTotClus << " clusters in total)" << endl;
}

void PndEmc2DLocMaxFinderTEST::SetParContainers()
{

  // Get run and runtime database
  FairRun *run = FairRun::Instance();
  if (!run) {
	LOG(fatal) << "-E- PndEmc2DLocMaxFinderTEST::SetParContainers: No analysis run";
  }

  FairRuntimeDb *db = run->GetRuntimeDb();
  if (!db) {
	LOG(fatal) << "-E- PndEmc2DLocMaxFinderTEST::SetParContainers: No runtime database";
  }
  // Get Emc digitisation parameter container
  fGeoPar = (PndEmcGeoPar *)db->getContainer("PndEmcGeoPar");
  // Get Emc digitisation parameter container
  fDigiPar = (PndEmcDigiPar *)db->getContainer("PndEmcDigiPar");
  // Get Emc reconstruction parameter container
  fRecoPar = (PndEmcRecoPar *)db->getContainer("PndEmcRecoPar");
}

ClassImp(PndEmc2DLocMaxFinderTEST)
