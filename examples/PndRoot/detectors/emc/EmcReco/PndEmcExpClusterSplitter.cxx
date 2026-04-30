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
// File and Version Information:
// 	$Id:$
//
// Description:
//	Class PndEmcExpClusterSplitter.
//      Implementation of ClusterSplitter which splits
//      on the basis of exponential distance from the bump centroid.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Adapted for the PANDA experiment at GSI
//
// Author List:
//      Phil Strother
//
// Copyright Information:
//	Copyright (C) 1997               Imperial College
//
// Modified:
// M. Babai
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "PndEmcExpClusterSplitter.h"

//---------------
// C++ Headers --
//---------------
//#include <vector>
//#include <set>
//#include <map>
#include <iostream>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"
#include "TClonesArray.h"

#include "PndEmcClusterProperties.h"
#include "PndEmcXClMoments.h"

#include "PndEmcRecoPar.h"
#include "PndEmcGeoPar.h"
#include "PndEmcDigiPar.h"
#include "PndEmcStructure.h"
#include "PndEmcMapper.h"

#include "PndDetectorList.h"
#include "PndEmcTwoCoordIndex.h"
#include "PndEmcBump.h"
#include "PndEmcCluster.h"
#include "PndEmcDigi.h"
#include "PndEmcSharedDigi.h"
#include "PndEmcXtal.h"
#include "PndEmcDataTypes.h"
using std::endl;

//----------------
// Constructors --
//----------------

PndEmcExpClusterSplitter::PndEmcExpClusterSplitter(Int_t verbose)
 : PndPersistencyTask("PndEmcExpClusterSplitter", verbose), fDigiArray(0), fClusterArray(0), fBumpArray(0), fSharedDigiArray(0), fGeoPar(new PndEmcGeoPar()),
 fDigiPar(new PndEmcDigiPar()), fRecoPar(new PndEmcRecoPar()), fClusterPosParam(), fMoliereRadius(0), fMoliereRadiusShashlyk(0), fExponentialConstant(0), fParArray1(TArrayD()), fParArray2(TArrayD()), fParArray3(TArrayD()), fParArray4(TArrayD()), fMaxIterations(0),
 fCentroidShift(0), fMaxBumps(0), fMinDigiEnergy(0)
{
 fClusterPosParam.clear();
 SetPersistency(kTRUE);
}

//--------------
// Destructor --
//--------------

PndEmcExpClusterSplitter::~PndEmcExpClusterSplitter()
{
 // 	delete fGeoPar;
 // 	delete fDigiPar;
 // 	delete fRecoPar;
}

/**
 * @brief Init Task
 *
 * @return InitStatus
 * @retval kSUCCESS success
 */
InitStatus PndEmcExpClusterSplitter::Init()
{

 // Get RootManager
 FairRootManager *ioman = FairRootManager::Instance();
 if (!ioman) {
  cout << "-E- PndEmcExpClusterSplitter::Init: "
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
  cout << "-W- PndEmcExpClusterSplitter::Init: "
   << "No PndEmcDigi array!" << endl;
  return kERROR;
 }

 fClusterArray = dynamic_cast<TClonesArray *>(ioman->GetObject("EmcCluster"));
 if (!fClusterArray) {
  cout << "-W- PndEmcExpClusterSplitter::Init: "
   << "No PndEmcCluster array!" << endl;
  return kERROR;
 }

 fMoliereRadius = fRecoPar->GetMoliereRadius();                 // Mr in cm
 fMoliereRadiusShashlyk = fRecoPar->GetMoliereRadiusShashlyk(); // Mr in cm
 fExponentialConstant = fRecoPar->GetExponentialConstant();
 fParArray1 = fRecoPar->GetParArray1();
 fParArray2 = fRecoPar->GetParArray2();
 fParArray3 = fRecoPar->GetParArray3();
 fParArray4 = fRecoPar->GetParArray4();
 // Energy fall off with distance from centre of cluster is
 // exp(-a*dist/Mr) Mr is the moliere radius.  dist is distance from
 // centre in cm, a is the above parameter.  The optimized value for
 // logarithimc cluster positioning is 2.5 For linear cluster
 // positioning a should be set to 1.5

 fMaxIterations = fRecoPar->GetMaxIterations();
 // Set the max number of iterations that the splitting algorithm is allowed
 // to do before it decides that enough is enough.

 fCentroidShift = fRecoPar->GetCentroidShift();
 // Set the tolerance level to which it is said that a bump is
 // said not to have moved since the last iteration.
 // The default is a millimetre.
 fMaxBumps = fRecoPar->GetMaxBumps();
 // Set an upper limit on the number of bumps allowed in a cluster if it
 // is to be split.

 fMinDigiEnergy = fRecoPar->GetMinDigiEnergy();
 // Set minimum SharedDigi energy to 20keV.

 if (!strcmp(fRecoPar->GetEmcClusterPosMethod(), "lilo")) {
  cout << "Lilo cluster position method" << endl;
  fClusterPosParam.push_back(fRecoPar->GetOffsetParmA());
  fClusterPosParam.push_back(fRecoPar->GetOffsetParmB());
  fClusterPosParam.push_back(fRecoPar->GetOffsetParmC());
 }

 // Create and register output array
 fBumpArray = new TClonesArray("PndEmcBump");
 ioman->Register("EmcBump", "Emc", fBumpArray, GetPersistency());

 fSharedDigiArray = new TClonesArray("PndEmcSharedDigi");
 ioman->Register("EmcSharedDigi", "Emc", fSharedDigiArray, GetPersistency());

 HowManyDidis = 0;

 LOG(info) << " PndEmcExpClusterSplitter: Intialization successfull";
 return kSUCCESS;
}

/**
 * @brief Runs the task
 *
 * The algorithm is as follows: We will index each bump by its
 * maximum digi's PndEmcTwoCoordIndex.  We will set up a list of
 * bump centroids which to start with will be synonymous with the
 * location of the maxima.  We then apportion a weight to each
 * digi, according to its distance from the centroids.  We then
 * construct the bumps according to these weights, which will
 * presumably give a different set of centroids.  This is repeated
 * until the centroids are static within tolerance, or we reach
 * the maximum number of iterations.
 *
 * @param opt unused
 * @return void
 */
void PndEmcExpClusterSplitter::Exec(Option_t *)
{

 PndEmcMapper *fEmcMap = PndEmcMapper::Instance();
 // Reset output array
 if (!fBumpArray)
  Fatal("Exec", "No Bump Array");
 fBumpArray->Delete();
 if (!fSharedDigiArray)
  Fatal("Exec", "No Shared Digi Array");
 fSharedDigiArray->Delete();

 int nClusters = fClusterArray->GetEntriesFast();

 for (Int_t iCluster = 0; iCluster < nClusters; iCluster++) {

  PndEmcCluster *theCluster = (PndEmcCluster *)fClusterArray->At(iCluster);

  int Module = theCluster->GetModule();

  int numberOfBumps = -1;
  numberOfBumps = (theCluster->LocalMaxMap()).size();

  if (numberOfBumps <= 1 || numberOfBumps >= fMaxBumps) {
   // Limit the max number of bumps in the cluster to 8 (default)
   // in this case, we clearly have a cluster, but no bumps to speak of.
   // Make 1 bump with weights all equal to 1

   std::map<Int_t, Int_t>::const_iterator theDigiIterator;

   PndEmcBump *theNewBump = AddBump();
   theNewBump->SetModule(theCluster->GetModule());
   theNewBump->MadeFrom(iCluster);
   theNewBump->SetLink(FairLink("EmcCluster", iCluster));

   for (theDigiIterator = theCluster->MemberDigiMap().begin(); theDigiIterator != theCluster->MemberDigiMap().end(); ++theDigiIterator) {
    PndEmcDigi *theDigi = (PndEmcDigi *)fDigiArray->At(theDigiIterator->second);
    AddSharedDigi(theDigi, 1.0); // PndEmcSharedDigi* sharedDigi= //[R.K.03/2017] unused variable
    Int_t iSharedDigi = fSharedDigiArray->GetEntriesFast() - 1;
    theNewBump->addDigi(fSharedDigiArray, iSharedDigi);
   }
  } else {

   if((Module != 1) && (Module != 2)){
    std::map<Int_t, Int_t> theMaximaDigis = theCluster->LocalMaxMap();
    std::map<Int_t, Int_t>::iterator theMaximaDigisIterator;
    std::map<PndEmcTwoCoordIndex *, TVector3 *> theCentroidPoints;
    std::map<PndEmcTwoCoordIndex *, TVector3 *> theMaximaPoints;
    std::map<PndEmcTwoCoordIndex *, PndEmcBump *> theIndexedBumps;
    std::map<PndEmcTwoCoordIndex *, TVector3 *> theAllDigiPoints;

    std::map<Int_t, Int_t> theDigiDict = theCluster->MemberDigiMap();

    double totalEnergy = 0;

    for (theMaximaDigisIterator = theMaximaDigis.begin(); theMaximaDigisIterator != theMaximaDigis.end(); ++theMaximaDigisIterator) {
     PndEmcDigi *theMaxDigi = (PndEmcDigi *)fDigiArray->At(theMaximaDigisIterator->second);

     Int_t detId = theMaximaDigisIterator->first;
     PndEmcTwoCoordIndex *theTCI = fEmcMap->GetTCI(detId);

     totalEnergy += theMaxDigi->GetEnergy();

     TVector3 *digiLocation = new TVector3(theMaxDigi->where());
     TVector3 *sameLocation = new TVector3(theMaxDigi->where());

     theMaximaPoints.insert(std::map<PndEmcTwoCoordIndex *, TVector3 *>::value_type(theTCI, digiLocation));
     theCentroidPoints.insert(std::map<PndEmcTwoCoordIndex *, TVector3 *>::value_type(theTCI, sameLocation));
    }

    std::map<Int_t, Int_t>::iterator theAllDigisIterator;
    // This loop works out the location of all the digis in the cluster

    for (theAllDigisIterator = theDigiDict.begin(); theAllDigisIterator != theDigiDict.end(); ++theAllDigisIterator) {
     Int_t detId = theAllDigisIterator->first;
     PndEmcTwoCoordIndex *theTCI = fEmcMap->GetTCI(detId);
     PndEmcDigi *theDigi = (PndEmcDigi *)fDigiArray->At(theAllDigisIterator->second);
     TVector3 *digiLocation = new TVector3(theDigi->where());
     theAllDigiPoints.insert(std::map<PndEmcTwoCoordIndex *, TVector3 *>::value_type(theTCI, digiLocation));
    }

    theMaximaDigisIterator = theMaximaDigis.begin();

    // Now we can create the EmcBumps

    // The algorithm is as follows: We will index each bump by its
    // maximum digi's PndEmcTwoCoordIndex.  We will set up a list of
    // bump centroids which to start with will be synonymous with the
    // location of the maxima.  We then apportion a weight to each
    // digi, according to its distance from the centroids.  We then
    // construct the bumps according to these weights, which will
    // presumably give a different set of centroids.  This is repeated
    // until the centroids are static within tolerance, or we reach
    // the maximum number of iterations.

    Int_t iterations = 0;

    Double_t averageCentroidShift;

    do {
     if (fVerbose >= 3) {
      std::cout << "iteration No " << iterations << std::endl;
     }
     averageCentroidShift = 0.0;

     // First clean up the old bumps
     std::map<PndEmcTwoCoordIndex *, PndEmcBump *>::iterator theBumpKiller = theIndexedBumps.begin();
     while (theBumpKiller != theIndexedBumps.end()) {
      PndEmcBump *theBump = theBumpKiller->second;
      delete theBump;
      ++theBumpKiller;
     }
     theIndexedBumps.clear();

     // Then loop over all the maxima and assign weights accordingly
     for (theMaximaDigisIterator = theMaximaDigis.begin(); theMaximaDigisIterator != theMaximaDigis.end(); ++theMaximaDigisIterator) {
      Int_t detId = theMaximaDigisIterator->first;
      PndEmcTwoCoordIndex *theCurrentMaximaTCI = fEmcMap->GetTCI(detId);

      if (fVerbose >= 3) {
       std::cout << "***************** current maximum: theta = " << theCurrentMaximaTCI->XCoord() << ", phi = " << theCurrentMaximaTCI->YCoord() << "*********" << std::endl;
      }

      // Create the bump which will correspond to this digi maxima
      PndEmcBump *theNewBump = new PndEmcBump();
      theNewBump->MadeFrom(iCluster);
      theIndexedBumps.insert(std::map<PndEmcTwoCoordIndex *, PndEmcBump *>::value_type(theCurrentMaximaTCI, theNewBump));

      // Now we will look over all the digis and add each of them
      // to this Bump with an appropriate weight

      for (theAllDigisIterator = theDigiDict.begin(); theAllDigisIterator != theDigiDict.end(); ++theAllDigisIterator) {
       PndEmcDigi *theCurrentDigi = (PndEmcDigi *)fDigiArray->At(theAllDigisIterator->second);
       PndEmcTwoCoordIndex *theCurrentTCI = theCurrentDigi->GetTCI();

       Double_t weight;

       // We are on the first pass and the digi is not a local max, or we are not on the
       // first pass.   Assign a weight according to the distance from the centroid position.

       Double_t myEnergy = 0;
       Double_t myDistance = 0;

       // Now share the digi out according to its distance from the maxima,
       // and the maxima energies

       Double_t totalDistanceEnergy = 0;

       // Moliere Radius for Shashlyk is different
       Double_t MoliereRadius;
       if (theCurrentDigi->GetModule() == 5)
        MoliereRadius = fMoliereRadiusShashlyk;
       else
        MoliereRadius = fMoliereRadius;

       std::map<PndEmcTwoCoordIndex *, TVector3 *>::iterator theMaxPointsIterator;

       for (theMaxPointsIterator = theCentroidPoints.begin(); theMaxPointsIterator != theCentroidPoints.end(); ++theMaxPointsIterator) {
        PndEmcTwoCoordIndex *theMaxPointsTCI = theMaxPointsIterator->first;

        TVector3 *theMaxPoint = theMaxPointsIterator->second;
        TVector3 *theCurrentDigiPoint = theAllDigiPoints.find(theCurrentTCI)->second;

        Double_t theDistance;

        // This next bit just checks to see if the maxima point in
        // hand is the same as the crystal from which we are
        // trying to find distance - just an FP trap really.

        if ((*theCurrentTCI) == (*theMaxPointsTCI)) {
         theDistance = 0.0;
        } else {
         TVector3 distance(*theMaxPoint - *theCurrentDigiPoint);

         theDistance = distance.Mag();
        }

        if (*theCurrentMaximaTCI == *(theMaxPointsTCI)) {
         // i.e. the maximum we are trying to find the distance from is
         // the one for which we are currently trying to make a bump
         myDistance = theDistance;
         Int_t iCurentMaxDigi = (theDigiDict.find(theMaxPointsTCI->Index()))->second;
         myEnergy = ((PndEmcDigi *)fDigiArray->At(iCurentMaxDigi))->GetEnergy();
        }

        Int_t iMaxPoint = (theDigiDict.find(theMaxPointsTCI->Index()))->second;
        totalDistanceEnergy += ((PndEmcDigi *)fDigiArray->At(iMaxPoint))->GetEnergy() * exp(-fExponentialConstant * theDistance / MoliereRadius);
       }

       if (totalDistanceEnergy > 0.0)
        weight = myEnergy * exp(-fExponentialConstant * myDistance / MoliereRadius) / (totalDistanceEnergy);
       else
        weight = 0;

       if (fVerbose >= 3) {
        std::cout << "\t digi theta = " << theCurrentDigi->GetTCI()->XCoord() << ", phi = " << theCurrentDigi->GetTCI()->YCoord() << std::endl;
        std::cout << "energy = " << theCurrentDigi->GetEnergy() << ", weight = " << weight << std::endl;
       }
       PndEmcSharedDigi *sharedDigi = AddSharedDigi(theCurrentDigi, weight);

       if (fVerbose >= 3) {
        std::cout << "shared digi energy = " << sharedDigi->GetEnergy() << std::endl;
       }

       Int_t iSharedDigi = fSharedDigiArray->GetEntriesFast() - 1;
       if (sharedDigi->GetEnergy() > fMinDigiEnergy) {
        theNewBump->addDigi(fSharedDigiArray, iSharedDigi);
       } else {
        fSharedDigiArray->RemoveAt(iSharedDigi);
        fSharedDigiArray->Compress();
       }
      }

      // Compute the shift of the centroid we have just calculated
      TVector3 *theOldCentroid = theCentroidPoints.find(theCurrentMaximaTCI)->second;

      PndEmcClusterProperties clusterProperties(*theNewBump, fSharedDigiArray);

      TVector3 newbumppos = clusterProperties.Where(fRecoPar->GetEmcClusterPosMethod(), fClusterPosParam);
      theNewBump->SetPosition(newbumppos);
      TVector3 centroidShift(*theOldCentroid - newbumppos);
      averageCentroidShift += centroidShift.Mag();
     }

     averageCentroidShift /= (Double_t)numberOfBumps;

     // Put the new centroids in the list of centroid points,
     // remembering to delete the old ones.
     std::map<PndEmcTwoCoordIndex *, TVector3 *>::iterator theCentroidPointsIterator = theCentroidPoints.begin();
     for (theCentroidPointsIterator = theCentroidPoints.begin(); theCentroidPointsIterator != theCentroidPoints.end(); ++theCentroidPointsIterator) {
      delete theCentroidPointsIterator->second;
     }
     theCentroidPoints.clear();

     std::map<PndEmcTwoCoordIndex *, PndEmcBump *>::iterator theIndexedBumpsIterator;
     for (theIndexedBumpsIterator = theIndexedBumps.begin(); theIndexedBumpsIterator != theIndexedBumps.end(); ++theIndexedBumpsIterator) {
      TVector3 *theNewCentroid = new TVector3((theIndexedBumpsIterator->second)->where());
      theCentroidPoints.insert(std::map<PndEmcTwoCoordIndex *, TVector3 *>::value_type(theIndexedBumpsIterator->first, theNewCentroid));
     }

     iterations++;

    } while (iterations < fMaxIterations && averageCentroidShift > fCentroidShift);
    // End of do loop

    // Finally append the new bumps to the TClonesArray.
    std::map<PndEmcTwoCoordIndex *, PndEmcBump *>::iterator theBumpsIterator;
    PndEmcBump *theBump;

    for (theBumpsIterator = theIndexedBumps.begin(); theBumpsIterator != theIndexedBumps.end(); ++theBumpsIterator) {
     theBump = theBumpsIterator->second;
     Int_t size_ba = fBumpArray->GetEntriesFast();
     PndEmcBump *theNextBump = new ((*fBumpArray)[size_ba]) PndEmcBump(*(theBump));
     if (fVerbose > 0)
      std::cout << "Bump Created!" << std::endl;
     theNextBump->SetInsertHistory(kFALSE);
     theNextBump->SetLink(FairLink("EmcCluster", iCluster));
     PndEmcCluster *myCluster = (PndEmcCluster *)fClusterArray->At(iCluster);
     theNextBump->AddLinks(myCluster->GetTrackEntering());
     theNextBump->SetTimeStamp(myCluster->GetTimeStamp());
     theNextBump->SetModule(myCluster->GetModule());
     theNextBump->SetTimeStampError(myCluster->GetTimeStampError());
    }

    std::map<PndEmcTwoCoordIndex *, TVector3 *>::iterator theGrimReaper = theMaximaPoints.begin();
    for (theGrimReaper = theMaximaPoints.begin(); theGrimReaper != theMaximaPoints.end(); ++theGrimReaper) {
     delete theGrimReaper->second;
    }
    theMaximaPoints.clear();

    for (theGrimReaper = theAllDigiPoints.begin(); theGrimReaper != theAllDigiPoints.end(); ++theGrimReaper) {
     delete theGrimReaper->second;
    }
    theAllDigiPoints.clear();

    for (theGrimReaper = theCentroidPoints.begin(); theGrimReaper != theCentroidPoints.end(); ++theGrimReaper) {
     delete theGrimReaper->second;
    }
    theCentroidPoints.clear();
   }
   else{
    std::map<Int_t, Int_t> theMaximaDigis = theCluster->LocalMaxMap();
    std::map<Int_t, Int_t>::iterator theMaximaDigisIterator;
    std::map<PndEmcTwoCoordIndex *, TVector3 *> theCentroidPoints;
    std::map<PndEmcTwoCoordIndex *, TVector3 *> theMaximaPoints;
    std::map<PndEmcTwoCoordIndex *, PndEmcBump *> theIndexedBumps;
    std::map<PndEmcTwoCoordIndex *, TVector3 *> theAllDigiPoints;
    std::map<PndEmcTwoCoordIndex *, Double_t> theBumpEnergy;

    std::map<Int_t, Int_t> theDigiDict = theCluster->MemberDigiMap();

    double totalEnergy = 0;

    for (theMaximaDigisIterator = theMaximaDigis.begin(); theMaximaDigisIterator != theMaximaDigis.end(); ++theMaximaDigisIterator) {
     PndEmcDigi *theMaxDigi = (PndEmcDigi *)fDigiArray->At(theMaximaDigisIterator->second);

     Int_t detId = theMaximaDigisIterator->first;
     PndEmcTwoCoordIndex *theTCI = fEmcMap->GetTCI(detId);

     totalEnergy += theMaxDigi->GetEnergy();

     TVector3 *digiLocation = new TVector3(theMaxDigi->where());
     TVector3 *sameLocation = new TVector3(theMaxDigi->where());

     theMaximaPoints.insert(std::map<PndEmcTwoCoordIndex *, TVector3 *>::value_type(theTCI, digiLocation));
     theCentroidPoints.insert(std::map<PndEmcTwoCoordIndex *, TVector3 *>::value_type(theTCI, sameLocation));
     theBumpEnergy.insert(std::map<PndEmcTwoCoordIndex *, Double_t>::value_type(theTCI, theMaxDigi->GetEnergy()));
    }

    std::map<Int_t, Int_t>::iterator theAllDigisIterator;

    for (theAllDigisIterator = theDigiDict.begin(); theAllDigisIterator != theDigiDict.end(); ++theAllDigisIterator) {
     Int_t detId = theAllDigisIterator->first;
     PndEmcTwoCoordIndex *theTCI = fEmcMap->GetTCI(detId);
     PndEmcDigi *theDigi = (PndEmcDigi *)fDigiArray->At(theAllDigisIterator->second);
     TVector3 *digiLocation = new TVector3(theDigi->where());
     theAllDigiPoints.insert(std::map<PndEmcTwoCoordIndex *, TVector3 *>::value_type(theTCI, digiLocation));
    }

    theMaximaDigisIterator = theMaximaDigis.begin();
    Int_t iterations = 0;

    Double_t averageCentroidShift;

    do {
     if (fVerbose >= 3) {
      std::cout << "iteration No " << iterations << std::endl;
     }
     averageCentroidShift = 0.0;

     std::map<PndEmcTwoCoordIndex *, PndEmcBump *>::iterator theBumpKiller = theIndexedBumps.begin();
     while (theBumpKiller != theIndexedBumps.end()) {
      PndEmcBump *theBump = theBumpKiller->second;
      delete theBump;
      ++theBumpKiller;
     }
     theIndexedBumps.clear();


     for (theMaximaDigisIterator = theMaximaDigis.begin(); theMaximaDigisIterator != theMaximaDigis.end(); ++theMaximaDigisIterator) {
      Int_t detId = theMaximaDigisIterator->first;
      PndEmcTwoCoordIndex *theCurrentMaximaTCI = fEmcMap->GetTCI(detId);

      if (fVerbose >= 3) {
       std::cout << "***************** current maximum: theta = " << theCurrentMaximaTCI->XCoord() << ", phi = " << theCurrentMaximaTCI->YCoord() << "*********" << std::endl;
      }


      PndEmcBump *theNewBump = new PndEmcBump();
      theNewBump->MadeFrom(iCluster);
      theIndexedBumps.insert(std::map<PndEmcTwoCoordIndex *, PndEmcBump *>::value_type(theCurrentMaximaTCI, theNewBump));
      for (theAllDigisIterator = theDigiDict.begin(); theAllDigisIterator != theDigiDict.end(); ++theAllDigisIterator) {
       PndEmcDigi *theCurrentDigi = (PndEmcDigi *)fDigiArray->At(theAllDigisIterator->second);
       PndEmcTwoCoordIndex *theCurrentTCI = theCurrentDigi->GetTCI();

       Double_t weight;
       Double_t myEnergy = 0;
       Double_t myDistance = 0;
       Int_t myCurentMaxDigi = -1;


       Double_t totalDistanceEnergy = 0;
       Double_t MoliereRadius;
       if (theCurrentDigi->GetModule() == 5)
        MoliereRadius = fMoliereRadiusShashlyk;
       else
        MoliereRadius = fMoliereRadius;

       std::map<PndEmcTwoCoordIndex *, TVector3 *>::iterator theMaxPointsIterator;

       for (theMaxPointsIterator = theCentroidPoints.begin(); theMaxPointsIterator != theCentroidPoints.end(); ++theMaxPointsIterator) {
        PndEmcTwoCoordIndex *theMaxPointsTCI = theMaxPointsIterator->first;

        TVector3 *theMaxPoint = theMaxPointsIterator->second;
        TVector3 *theCurrentDigiPoint = theAllDigiPoints.find(theCurrentTCI)->second;

        Double_t theDistance;

        if ((*theCurrentTCI) == (*theMaxPointsTCI)) {
         theDistance = 0.0;
        } else {
         TVector3 distance(*theMaxPoint - *theCurrentDigiPoint);

         theDistance = distance.Mag();
        }

        if (*theCurrentMaximaTCI == *(theMaxPointsTCI)) {

         myDistance = theDistance;
         Int_t iCurentMaxDigi = (theDigiDict.find(theMaxPointsTCI->Index()))->second;
         myCurentMaxDigi = iCurentMaxDigi;
         myEnergy = ((PndEmcDigi *)fDigiArray->At(iCurentMaxDigi))->GetEnergy();
        }

        Int_t iMaxPoint = (theDigiDict.find(theMaxPointsTCI->Index()))->second;

        totalDistanceEnergy += ((PndEmcDigi *)fDigiArray->At(iMaxPoint))->GetEnergy() * LateralDevelopment(theBumpEnergy.find(theMaxPointsTCI)->second, theCurrentDigiPoint, &(((PndEmcDigi *)fDigiArray->At(iMaxPoint))->where()), theMaxPoint, MoliereRadius);
       }
       theMaxPointsIterator = theCentroidPoints.find(theCurrentMaximaTCI);
       if (totalDistanceEnergy > 0.0)
        weight = myEnergy * LateralDevelopment(theBumpEnergy.find(theCurrentMaximaTCI)->second, theAllDigiPoints.find(theCurrentTCI)->second, &(((PndEmcDigi *)fDigiArray->At(myCurentMaxDigi))->where()), theMaxPointsIterator->second, MoliereRadius) / (totalDistanceEnergy);
       else
        weight = 0;

       if (fVerbose >= 3) {
        std::cout << "\t digi theta = " << theCurrentDigi->GetTCI()->XCoord() << ", phi = " << theCurrentDigi->GetTCI()->YCoord() << std::endl;
        std::cout << "energy = " << theCurrentDigi->GetEnergy() << ", weight = " << weight << std::endl;
       }
       PndEmcSharedDigi *sharedDigi = AddSharedDigi(theCurrentDigi, weight);

       if (fVerbose >= 3) {
        std::cout << "shared digi energy = " << sharedDigi->GetEnergy() << std::endl;
       }

       Int_t iSharedDigi = fSharedDigiArray->GetEntriesFast() - 1;
       if (sharedDigi->GetEnergy() > fMinDigiEnergy) {
        theNewBump->addDigi(fSharedDigiArray, iSharedDigi);
       } else {
        fSharedDigiArray->RemoveAt(iSharedDigi);
        fSharedDigiArray->Compress();
       }
      }
      TVector3 *theOldCentroid = theCentroidPoints.find(theCurrentMaximaTCI)->second;

      PndEmcClusterProperties clusterProperties(*theNewBump, fSharedDigiArray);

      TVector3 newbumppos = clusterProperties.Where(fRecoPar->GetEmcClusterPosMethod(), fClusterPosParam);
      theNewBump->SetPosition(newbumppos);
      theNewBump->SetEnergy(clusterProperties.Energy());
      TVector3 centroidShift(*theOldCentroid - newbumppos);
      averageCentroidShift += centroidShift.Mag();
     }

     averageCentroidShift /= (Double_t)numberOfBumps;

     std::map<PndEmcTwoCoordIndex *, TVector3 *>::iterator theCentroidPointsIterator = theCentroidPoints.begin();
     for (theCentroidPointsIterator = theCentroidPoints.begin(); theCentroidPointsIterator != theCentroidPoints.end(); ++theCentroidPointsIterator) {
      delete theCentroidPointsIterator->second;
     }
     theCentroidPoints.clear();
     theBumpEnergy.clear();

     std::map<PndEmcTwoCoordIndex *, PndEmcBump *>::iterator theIndexedBumpsIterator;
     for (theIndexedBumpsIterator = theIndexedBumps.begin(); theIndexedBumpsIterator != theIndexedBumps.end(); ++theIndexedBumpsIterator) {
      TVector3 *theNewCentroid = new TVector3((theIndexedBumpsIterator->second)->where());
      theCentroidPoints.insert(std::map<PndEmcTwoCoordIndex *, TVector3 *>::value_type(theIndexedBumpsIterator->first, theNewCentroid));
      theBumpEnergy.insert(std::map<PndEmcTwoCoordIndex *, Double_t>::value_type(theIndexedBumpsIterator->first, (theIndexedBumpsIterator->second)->GetEnergy()));
     }

     iterations++;

    } while (iterations < fMaxIterations && averageCentroidShift > fCentroidShift);

    std::map<PndEmcTwoCoordIndex *, PndEmcBump *>::iterator theBumpsIterator;
    PndEmcBump *theBump;

    for (theBumpsIterator = theIndexedBumps.begin(); theBumpsIterator != theIndexedBumps.end(); ++theBumpsIterator) {
     theBump = theBumpsIterator->second;
     Int_t size_ba = fBumpArray->GetEntriesFast();
     PndEmcBump *theNextBump = new ((*fBumpArray)[size_ba]) PndEmcBump(*(theBump));
     if (fVerbose > 0)
      std::cout << "Bump Created!" << std::endl;
     theNextBump->SetInsertHistory(kFALSE);
     theNextBump->SetLink(FairLink("EmcCluster", iCluster));
     PndEmcCluster *myCluster = (PndEmcCluster *)fClusterArray->At(iCluster);
     theNextBump->AddLinks(myCluster->GetTrackEntering());
     theNextBump->SetTimeStamp(myCluster->GetTimeStamp());
     theNextBump->SetModule(myCluster->GetModule());
     theNextBump->SetTimeStampError(myCluster->GetTimeStampError());
    }

    std::map<PndEmcTwoCoordIndex *, TVector3 *>::iterator theGrimReaper = theMaximaPoints.begin();
    for (theGrimReaper = theMaximaPoints.begin(); theGrimReaper != theMaximaPoints.end(); ++theGrimReaper) {
     delete theGrimReaper->second;
    }
    theMaximaPoints.clear();

    for (theGrimReaper = theAllDigiPoints.begin(); theGrimReaper != theAllDigiPoints.end(); ++theGrimReaper) {
     delete theGrimReaper->second;
    }
    theAllDigiPoints.clear();

    for (theGrimReaper = theCentroidPoints.begin(); theGrimReaper != theCentroidPoints.end(); ++theGrimReaper) {
     delete theGrimReaper->second;
    }
    theCentroidPoints.clear();

   }

  }

  Int_t nBumps = (theCluster->LocalMaxMap()).size();
  theCluster->SetNBumps(nBumps);
 }

 // At that moment internal state fEnergy and fWhere of Clusters are
 // not initialized, the following make it possible to see energy and
 // position from output root file
 Int_t nBump = fBumpArray->GetEntriesFast();

 Double_t fTimeError; // CalibTimeOfaDigi, //[R.K.03/2017] unused variable
 Double_t WeightedFactor1(0.), NormWeightedFactor1(0.), AverageTime1(0.);
 // Double_t WeightedFactor2(0.), NormWeightedFactor2(0.), AverageTime2(0.);
 // Double_t WeightedFactor3(0.), NormWeightedFactor3(0.), AverageTime3(0.);
 for (Int_t i = 0; i < nBump; i++) {
  PndEmcBump *tmpbump = (PndEmcBump *)fBumpArray->At(i);
  PndEmcClusterProperties clusterProperties(*tmpbump, fSharedDigiArray);
  if (!tmpbump->IsEnergyValid())
   tmpbump->SetEnergy(clusterProperties.Energy());
  if (!tmpbump->IsPositionValid())
   tmpbump->SetPosition(clusterProperties.Where(fRecoPar->GetEmcClusterPosMethod(), fClusterPosParam));
  PndEmcXClMoments xClMoments(*tmpbump, fSharedDigiArray);
  tmpbump->SetZ20(xClMoments.AbsZernikeMoment(2, 0, 15));
  tmpbump->SetZ53(xClMoments.AbsZernikeMoment(5, 3, 15));
  tmpbump->SetLatMom(xClMoments.Lat());

  // for time information
  WeightedFactor1 = 0.; //= WeightedFactor2 = WeightedFactor3 = 0.;
  AverageTime1 = 0.;    // AverageTime2 = AverageTime3 = 0.;
  Double_t fMaxDigiEnergy = -1.;
  const std::vector<Int_t> &listOfDigi = tmpbump->DigiList();
  for (size_t id = 0; id < listOfDigi.size(); ++id) {
   PndEmcDigi *theDigi = (PndEmcDigi *)fSharedDigiArray->At(listOfDigi[id]);
   // CalibTimeOfaDigi = digiCalibrator.CalibrationEvtTimeByDigi(theDigi, kFALSE);
   fTimeError = digiCalibrator.GetTimeResolutionOfDigi(theDigi);
   WeightedFactor1 += 1. / fTimeError / fTimeError;
   // WeightedFactor2 += theDigi->GetEnergy()*1./fTimeError/fTimeError;
   // WeightedFactor3 += theDigi->GetEnergy();
   if (theDigi->GetEnergy() > fMaxDigiEnergy) {
    fMaxDigiEnergy = theDigi->GetEnergy();
    tmpbump->SetEventNo(theDigi->fEvtNo);
    // tmpbump->SetTimeStamp1(theDigi->GetTimeStamp());
    // tmpbump->fSeedPosition = theDigi->where();
   }
  }
  for (size_t id = 0; id < listOfDigi.size(); ++id) {
   PndEmcDigi *theDigi = (PndEmcDigi *)fSharedDigiArray->At(listOfDigi[id]);
   digiCalibrator.CalibrationEvtTimeByDigi(theDigi, kFALSE); // CalibTimeOfaDigi =  //[R.K.03/2017] unused variable
   fTimeError = digiCalibrator.GetTimeResolutionOfDigi(theDigi);
   NormWeightedFactor1 = 1. / fTimeError / fTimeError;
   NormWeightedFactor1 /= WeightedFactor1;
   AverageTime1 += NormWeightedFactor1 * theDigi->GetTimeStamp();

   // NormWeightedFactor2 = theDigi->GetEnergy()*1./fTimeError/fTimeError;
   // NormWeightedFactor2 /= WeightedFactor2;
   // AverageTime2 += NormWeightedFactor2*theDigi->GetTimeStamp();

   // NormWeightedFactor3 = theDigi->GetEnergy();
   // NormWeightedFactor3 /= WeightedFactor3;
   // AverageTime3 += NormWeightedFactor3*theDigi->GetTimeStamp();
  }
  tmpbump->SetTimeStamp(AverageTime1);
  HowManyDidis += tmpbump->NumberOfDigis();
  // tmpbump->SetTimeStamp1(AverageTime1);
  // tmpbump->SetTimeStamp2(AverageTime2);
  // tmpbump->SetTimeStamp3(AverageTime3);
  // end for time information
 }

 if (fVerbose >= 1) {
  std::cout << "PndEmcExpClusterSplitter:: Number of clusters = " << nClusters << std::endl;
  std::cout << "PndEmcExpClusterSplitter:: Number of bumps = " << nBump << std::endl;
 }
}

/**
 * @brief Adds a new PndEmcBump to fBumpArray and returns it.
 *
 * @return PndEmcBump*
 */
PndEmcBump *PndEmcExpClusterSplitter::AddBump()
{
 TClonesArray &clref = *fBumpArray;
 Int_t size = clref.GetEntriesFast();
 return new (clref[size]) PndEmcBump();
}

/**
 * @brief Adds a new PndEmcSharedDigi to fSharedDigiArray and returns it.
 *
 * @param digi Digi which is shared
 * @param weight Weight of digi in this shared digi
 * @return PndEmcSharedDigi*
 */
PndEmcSharedDigi *PndEmcExpClusterSplitter::AddSharedDigi(PndEmcDigi *digi, Double_t weight)
{
 TClonesArray &clref = *fSharedDigiArray;
 Int_t size = clref.GetEntriesFast();
 return new (clref[size]) PndEmcSharedDigi(*digi, weight);
}


Double_t PndEmcExpClusterSplitter::LateralDevelopment(const Double_t TotEnergy, const TVector3 *DetPos, const TVector3 *SeedPos, const TVector3 *ShowerCenter, const Double_t RM)
{
 Double_t CenterTheta = TMath::RadToDeg() * (ShowerCenter->Theta());
 Double_t p1 = fParArray1.At(0) * exp(-fParArray1.At(1) * TotEnergy) + fParArray1.At(2) * pow(CenterTheta - fParArray1.At(3),2) + fParArray1.At(4);
 Double_t p2 = fParArray2.At(0) * exp(-fParArray2.At(1) * TotEnergy) + fParArray2.At(2) * pow(CenterTheta - fParArray2.At(3),2) + fParArray2.At(4);
 Double_t p3 = fParArray3.At(0) * exp(-fParArray3.At(1) * TotEnergy) + fParArray3.At(2) * pow(CenterTheta - fParArray3.At(3),2) + fParArray3.At(4);
 //Double_t p4 = fParArray4.At(0) * exp(-fParArray4.At(1) * TotEnergy) + fParArray4.At(2) * pow(CenterTheta - fParArray4.At(3),2) + fParArray4.At(4);
 Double_t p4 = (fParArray4.At(0) * pow(CenterTheta - fParArray4.At(1),2) + fParArray4.At(2)) * exp(-(fParArray4.At(3)*CenterTheta + fParArray4.At(4)) * TotEnergy) + fParArray4.At(5) * pow(CenterTheta - fParArray4.At(6),2) + fParArray4.At(7);

 Double_t r = DetPos->Mag() * TMath::Sin(ShowerCenter->Angle(*DetPos));
 Double_t r_seed = SeedPos->Mag() * TMath::Sin(ShowerCenter->Angle(*SeedPos));

 Double_t xi = r - p2 * r * exp( -pow(r/p3/RM, p4) );
 Double_t xi_seed = r_seed - p2 * r_seed * exp( -pow(r_seed/p3/RM,p4) );
 Double_t delta_xi = xi - xi_seed;

 if (delta_xi < 0) delta_xi = 0;
 return exp( -p1 * delta_xi / RM );
}


/**
 * @brief Called at end of task.
 *
 * Outputs the number of digis read.
 *
 * @return void
 */
void PndEmcExpClusterSplitter::FinishTask()
{
 cout << "=================================================" << endl;
 cout << "PndEmcExpClusterSplitter::FinishTask" << endl;
 cout << "=================================================" << endl;
 cout << "read digis #" << HowManyDidis << endl;
}

void PndEmcExpClusterSplitter::SetParContainers()
{

 // Get run and runtime database
 FairRun *run = FairRun::Instance();
 if (!run)
  LOG(fatal) << "SetParContainers: No analysis run";

 FairRuntimeDb *db = run->GetRuntimeDb();
 if (!db)
  LOG(fatal) << "SetParContainers: No runtime database";
 // Get Emc digitisation parameter container
 fGeoPar = (PndEmcGeoPar *)db->getContainer("PndEmcGeoPar");
 // Get Emc digitisation parameter container
 fDigiPar = (PndEmcDigiPar *)db->getContainer("PndEmcDigiPar");
 // Get Emc reconstruction parameter container
 fRecoPar = (PndEmcRecoPar *)db->getContainer("PndEmcRecoPar");
}

ClassImp(PndEmcExpClusterSplitter)


