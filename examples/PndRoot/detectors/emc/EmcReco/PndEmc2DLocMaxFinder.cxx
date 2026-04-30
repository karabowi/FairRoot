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

#include "PndEmc2DLocMaxFinder.h"

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

//----------------
// Constructors --
//----------------

PndEmc2DLocMaxFinder::PndEmc2DLocMaxFinder(Int_t verbose)
  : PndPersistencyTask("PndEmc2DLocMaxFinder", verbose), fClusterArray(0), fDigiArray(0), fGeoPar(new PndEmcGeoPar()), fDigiPar(new PndEmcDigiPar()), fRecoPar(new PndEmcRecoPar()),
    fMaxECut(0), fNeighbourECut(0), fCutSlope(0), fCutOffset(0), fERatioCorr(0), fTheNeighbourLevel(0)
{
  SetPersistency(kFALSE);
}

//--------------
// Destructor --
//--------------

PndEmc2DLocMaxFinder::~PndEmc2DLocMaxFinder()
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
InitStatus PndEmc2DLocMaxFinder::Init()
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

  LOG(info) << " PndEmc2DLocMaxFinder: Intialization successfull";
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
void PndEmc2DLocMaxFinder::Exec(Option_t *)
{
  int nClusters = fClusterArray->GetEntriesFast();
  PndEmcCoordIndexSet tmp_CoordSet_set;

  PndEmcMapper *fEmcMap = PndEmcMapper::Instance();

  // loop over Clusters
  for (Int_t iCluster = 0; iCluster < nClusters; iCluster++) {
    PndEmcCluster *theCluster = (PndEmcCluster *)fClusterArray->At(iCluster);

    // map <detId,digiIndex>
    std::map<Int_t, Int_t> theClustersDigis = theCluster->MemberDigiMap();
    std::map<Int_t, Int_t>::iterator theDigiIterator = theClustersDigis.begin();

    PndEmcCoordIndexSet allTheNeighbours;
    PndEmcCoordIndexSet theNewNeighbours;

    if (theClustersDigis.size() == 1) {
      while (theDigiIterator != theClustersDigis.end()) {
        theCluster->addLocalMax(fDigiArray, theDigiIterator->second);
        ++theDigiIterator;
      }
    } else {
      while (theDigiIterator != theClustersDigis.end()) {
        Int_t detId = theDigiIterator->first;
        PndEmcTwoCoordIndex *theTCI = fEmcMap->GetTCI(detId);

        allTheNeighbours.clear();
        theNewNeighbours.clear();

        // Start the newneighbours off, then get all the neighbours from
        // this.  This is kind of curious, since the seed digi shouldn't
        // end up in the neighbour list, but it should work
        theNewNeighbours.insert(theTCI);

        getNeighbourDigis(allTheNeighbours, theNewNeighbours, fTheNeighbourLevel, theClustersDigis);
        PndEmcDigi *theDigi = (PndEmcDigi *)fDigiArray->At(theDigiIterator->second);
        if (isALocalMax(theDigi, theCluster, allTheNeighbours)) {
          theCluster->addLocalMax(fDigiArray, theDigiIterator->second);
        }
        ++theDigiIterator;
      }
      // If no local maxima was added simply add the absolute maxima
      if ((theCluster->LocalMaxMap()).size() == 0) {
        if (FairRunAna::Instance()->IsTimeStamp()) {
          theCluster->addLocalMax(fDigiArray, theCluster->MaximaAsIndex(fDigiArray));
        } else {
          theCluster->addLocalMax(theCluster->Maxima(fDigiArray));
        }
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
 * the ratio of digi energy and neighbor energies have to be fullfilled (see graph/comment in Init()).
 *
 * @param theDigi Digi to check
 * @param theCluster Cluster of the digi
 * @param amongstTheseNeighbours Neighbors of the digi
 * @return bool
 * @retval true Digi fullfills conditions to be considered a local maximum.
 */
bool PndEmc2DLocMaxFinder::isALocalMax(const PndEmcDigi *const theDigi, const PndEmcCluster *const theCluster, const PndEmcCoordIndexSet &amongstTheseNeighbours) const
{
  // Loop over all our neighbours and check to see if the one in hand is a local max

  Bool_t result = true;
  Double_t theDigiEnergy = theDigi->GetEnergy();

  if (theDigiEnergy < fMaxECut) {
    // std::cout << "Digi is not a local max because its energy "
    //    << theDigiEnergy << " is too low. " << std::endl;
    result = false;
  } else {
    PndEmcCoordIndexSet::const_iterator theNeighbourIterator = amongstTheseNeighbours.begin();

    const std::map<Int_t, Int_t> theClustersDigis = theCluster->MemberDigiMap();

    Double_t numberOFneighbours(0.0);
    Double_t neighbourMaxE(0.0);

    while ((theNeighbourIterator != amongstTheseNeighbours.end()) && result) {

      std::map<Int_t, Int_t>::const_iterator position = theClustersDigis.find((*theNeighbourIterator)->Index());

      if (position != theClustersDigis.end()) {
        PndEmcDigi *digi = (PndEmcDigi *)fDigiArray->At(position->second);
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

  if ((result) && (fVerbose > 0)) {
    std::cout << " Digi at (" << theDigi->GetThetaInt() << ", " << theDigi->GetPhiInt() << ") was a local max. Energy = " << theDigi->GetEnergy() << std::endl;
  }
  return result;
}

/**
 * @brief Get the TCIs of neighbor digis
 *
 * @param[out] allDigiNeighbours TCIs of the neighbors
 * @param[in,out] currentDigiNeighbours TCI of the digi for which the neighbors are to be found. Gets cleared.
 * @param neighbourLevel ignored
 * @param theClusterDigis std::map of detector IDs and digi index in TClonesArray, as returned by PndEmcCluster::MemberDigiMap().
 * @return void
 */
void PndEmc2DLocMaxFinder::getNeighbourDigis(PndEmcCoordIndexSet &allDigiNeighbours, PndEmcCoordIndexSet &currentDigiNeighbours,
                                             int, // neighbourLevel //[R.K.03/2017] unused variable(s)
                                             std::map<Int_t, Int_t> theClusterDigis) const
{
  PndEmcMapper *fEmcMap = PndEmcMapper::Instance();

  PndEmcCoordIndexSet currentDigisCopy(currentDigiNeighbours);
  currentDigiNeighbours.clear();
  PndEmcCoordIndexSet::iterator theCurrentDigiIterator = currentDigisCopy.begin();
  PndEmcCoordIndexSet theNextDigiNeighbours;

  while (theCurrentDigiIterator != currentDigisCopy.end()) {
    PndEmcTwoCoordIndex *theCurrentTCI = *theCurrentDigiIterator;

    std::map<Int_t, Int_t>::const_iterator theDigiIterator = theClusterDigis.begin();
    while (theDigiIterator != theClusterDigis.end()) {
      Int_t detId = theDigiIterator->first;
      PndEmcTwoCoordIndex *theTCI = fEmcMap->GetTCI(detId);
      bool isneighbour = theCurrentTCI->IsNeighbour(theTCI);
      if (isneighbour)
        allDigiNeighbours.insert(theTCI);
      ++theDigiIterator;
    }
    ++theCurrentDigiIterator;
  }
}

void PndEmc2DLocMaxFinder::SetParContainers()
{

  // Get run and runtime database
  FairRun *run = FairRun::Instance();
  if (!run)
    LOG(fatal) << "-E- PndEmc2DLocMaxFinder::SetParContainers: No analysis run";

  FairRuntimeDb *db = run->GetRuntimeDb();
  if (!db)
	  LOG(fatal) << "-E- PndEmc2DLocMaxFinder::SetParContainers No runtime database";
  // Get Emc digitisation parameter container
  fGeoPar = (PndEmcGeoPar *)db->getContainer("PndEmcGeoPar");
  // Get Emc digitisation parameter container
  fDigiPar = (PndEmcDigiPar *)db->getContainer("PndEmcDigiPar");
  // Get Emc reconstruction parameter container
  fRecoPar = (PndEmcRecoPar *)db->getContainer("PndEmcRecoPar");
}

ClassImp(PndEmc2DLocMaxFinder)
