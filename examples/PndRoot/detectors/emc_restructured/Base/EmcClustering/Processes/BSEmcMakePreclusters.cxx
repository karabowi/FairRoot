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
// 	+ Implementation of online clustering algorithm
// 	+ Mapping of digis into virtual DC (more closely resembles real readout chain)
//
// Environment:
//	Software developed for the PANDA Detector at GSI.
//

#include "BSEmcMakePreclusters.h"

#include <algorithm>
#include <limits>
#include <math.h>
#include <vector>

#include "TError.h"
#include "TStopwatch.h"
#include "TString.h"
#include "TVector3.h"

#include "FairParSet.h"
#include "FairRunAna.h"
#include "fairlogger/Logger.h"

#include "PndContainerI.h"
#include "PndMutableContainerI.h"
#include "PndParameterRegister.h"

#include "BSEmcCluster.h"
#include "BSEmcClusterPropertiesPar.h"
#include "BSEmcClusterRadiusAlgo.h"

#include "BSEmcClusteringPar.h"
#include "BSEmcCrystalPositionPar.h"
#include "BSEmcDetectorID.h"
#include "BSEmcDigi.h"
#include "BSEmcGeoNeighbouringRelationPar.h"
#include "BSEmcPrecluster.h"

BSEmcMakePreclusters::BSEmcMakePreclusters()
  : PndProcess{"BSEmcMakePreclusters"}, fClusteringParName(""), fPositionParName(""), fNeighbouringRelationParName(""), fDigiEnergyThreshold(0)

{
}

BSEmcMakePreclusters::~BSEmcMakePreclusters() {}

void BSEmcMakePreclusters::SetDetectorName(const std::string &t_detectorName)
{
  fDetectorName = t_detectorName;
  if (fClusteringParName == "") {
    fClusteringParName = BSEmcClusteringPar::fgParameterName + fDetectorName;
  }
  if (fClusterPropertiesParName == "") {
    fClusterPropertiesParName = BSEmcClusterPropertiesPar::fgParameterName + fDetectorName;
  }
  if (fPositionParName == "") {
    fPositionParName = BSEmcCrystalPositionPar::fgParameterName + fDetectorName;
  }
  if (fNeighbouringRelationParName == "") {
    fNeighbouringRelationParName = BSEmcGeoNeighbouringRelationPar::fgParameterName + fDetectorName;
  }

  fParameterList.push_back(fClusteringParName);
  fParameterList.push_back(fClusterPropertiesParName);
  fParameterList.push_back(fNeighbouringRelationParName);
  fParameterList.push_back(fPositionParName);
}

void BSEmcMakePreclusters::SetupParameters(const PndParameterRegister *t_paramRegister)
{
  {
    BSEmcClusteringPar *parameter = dynamic_cast<BSEmcClusteringPar *>(t_paramRegister->GetParameter(fClusteringParName));
    fDigiEnergyThreshold = parameter->GetDigiEnergyThreshold();
    LOG(debug) << "BSEmcMakePreclusters::Init() - using: fDigiEnergyThreshold = " << fDigiEnergyThreshold;
  }
  fNeighbouringRelationPar = dynamic_cast<BSEmcGeoNeighbouringRelationPar *>(t_paramRegister->GetParameter(fNeighbouringRelationParName.c_str()));

  fPositionPar = dynamic_cast<BSEmcCrystalPositionPar *>(t_paramRegister->GetParameter(fPositionParName.c_str()));
  {
    BSEmcClusterPropertiesPar *parameter = dynamic_cast<BSEmcClusterPropertiesPar *>(t_paramRegister->GetParameter(fClusterPropertiesParName));
    fPositionProcess->SetClusterPropertiesPar(parameter);
    fPositionProcess->SetPositionPar(fPositionPar);
  }
  LOG(debug) << " BSEmcMakePreclusters::Init(): Start Initialization";

  if (fPosMethod == 0) {
    LOG(debug) << "Using DEFAULT precluster position and radius calculation method (from fRecoPar)";
  } else if (fPosMethod == 1) {
    LOG(debug) << "Using SIMPLIFIED position and radius calculation method (REAL x,y position).";
  } else if (fPosMethod == 2) {
    LOG(debug) << "Using SIMPLIFIED position and radius calculation method (MAPPED x,y position).";
  } else {
    LOG(error) << "BSEmcMakePreclusters::Init() - Invalid position method specified. Selecting recommended default instead. ";
    fPosMethod = 1;
  }

  LOG(debug) << "\nIMPORTANT: ";
  LOG(debug) << " -> Did you remember to change PndEmcClusterRingSorter.cxx to use BSEmcPrecluster instead of PndEmcCluster objects?";
  LOG(debug) << "    Also, don't forget to compile again after making these changes ;)";
  LOG(debug) << " -> Make sure that you set BSEmcPrecluster to use the digi position method you want (using BSEmcMakePreclusters->SetDigiPositionMethod()).";
  LOG(debug) << "    You're using the ";

  if (fDigiPosType == 0) {
    LOG(debug) << "MAPPED XPAD,YPAD position.\n";
  } else if (fDigiPosType == 1) {
    LOG(debug) << "REAL x,y position.\n";
  } else {
    LOG(error) << "BSEmcMakePreclusters::Init: "
               << "Unknown digi position method/digi position method not set!";
  }

  LOG(debug) << "-I- BSEmcMakePreclusters: Intialization successful";
}

void BSEmcMakePreclusters::RequestDataContainer(PndContainerRegister *t_register)
{
  t_register->Request({fDigiBranchname, "BSEmcDigi", kFALSE, kFALSE});
  t_register->Request({fPreclusterBranchname, "BSEmcPrecluster", kTRUE, kTRUE});
}

void BSEmcMakePreclusters::GetDataContainer(PndContainerRegister *t_register)
{
  fDigiArray = t_register->GetInput<BSEmcDigi>(fDigiBranchname);
  fPreclusterArray = t_register->GetOutput<BSEmcPrecluster>(fPreclusterBranchname);

  if (fDigiBranchname != "") {
    t_register->SetAsDefaultBranchFor(fDigiBranchname, "BSEmcDigi");
  }
  if (fPreclusterBranchname != "") {
    t_register->SetAsDefaultBranchFor(fPreclusterBranchname, "BSEmcPrecluster");
  }
}

void BSEmcMakePreclusters::Process()
{
  fPreclusterArray->Reset();
  std::vector<BSEmcPrecluster *> preclusterCont = Precluster(fDigiArray->GetVectorOfPtrToConst());
  LOG(debug) << "BSEmcMakePreclusters::Process() forming " << preclusterCont.size() << " Precluster from " << fDigiArray->GetSize() << " " << fDigiArray->GetBranchName()
             << " and storing them in " << fPreclusterArray->GetBranchName();
  for (BSEmcPrecluster *precluster : preclusterCont) {
    fPreclusterArray->CreateCopy(*precluster);
    delete precluster;
  }
}

std::vector<BSEmcPrecluster *> BSEmcMakePreclusters::Precluster(const std::vector<const BSEmcDigi *> &t_digiCont)
{
  std::vector<BSEmcPrecluster *> preclusterCont;
  Int_t nDigis = t_digiCont.size();

  // -----   Timer for preclustering  -----------------------------------------
  TStopwatch DCtimer;
  DCtimer.Start();

  // --------------------- START SEARCHING FOR PRECLUSTERS ---------------------------

  // Int_t dx = 0;
  // Int_t dy = 0;
  Int_t nPreclusters = 0;
  Int_t nDigisPassed = 0; // #digis that passed thresholds
  Int_t nDCdigis = 0;
  Int_t nDCs = 0; // #DC active

  Double_t dt = 0;
  Double_t deltaT = 0;

  /*if(FairRunAna::Instance()->IsTimeStamp()) {
    dt = 0;
    deltaT = fClusterActiveTime; // threshold for time between digis in a cluster
  }*/

  std::vector<Int_t> neighbours;   // array keeping track which digis are neighbours
  std::vector<Int_t> DigiPassed;   // array containing indices of digis that passed the energy cuts
  std::vector<Int_t> XPadPassed;   // array keeping track of the x-coordinate (in pad space) of digis that passed the energy cuts
  std::vector<Int_t> YPadPassed;   // array keeping track of the y-coordinate (in pad space) of digis that passed the energy cuts
  std::vector<Int_t> isAdded;      // array with cluster numbers for each digi
  std::vector<Int_t> similarities; // array keeping track which clusters should be merged
  std::vector<Double_t> TPassed;   // array keeping track of the timestamp of digis that passed the energy cuts
  std::vector<Bool_t> tagArray;    // array to tag which digis to consider

  for (Int_t iDC = 0; iDC < 159; iDC++) { // loop over all virtual Data Concentrators (DCs)

    //----- Tag which digis are in the current virtual DC -----//
    // if (iDC != 103) continue; // for comparison with hardware prototype, only use 1 DC (at random chosen from FwEndcap)
    tagArray.clear();
    nDCdigis = 0;

    for (Int_t i = 0; i < nDigis; i++) {
      const BSEmcDigi *myDigi = t_digiCont.at(i);
      if (myDigi->GetDCNumber() == iDC) {
        tagArray.push_back(true);
        nDCdigis++;
      } else {
        tagArray.push_back(false);
      }
    }

    nDigisPassed = 0;

    if (nDCdigis == 0) {
      continue; // skip empty DCs
    }

    nDCs++;

    // reset vectors
    neighbours.clear();
    DigiPassed.clear();
    XPadPassed.clear();
    YPadPassed.clear();
    TPassed.clear();
    isAdded.clear();
    similarities.clear();

    for (Int_t a = 0; a < nDCdigis; a++) {
      isAdded.push_back(-1); // initialise all entries of isAdded to -1 (needed by algorithm)
    }

    //----- Loop over all digis to add them to preclusters -----//

    for (Int_t iDigi = 0; iDigi < nDigis; ++iDigi) {

      if (tagArray[iDigi] == false) {
        continue; // only process digis that are in the current virtual DC
      }

      digiCounter++;

      /* Get a new digi from the digi array */
      const BSEmcDigi *theDigi = t_digiCont.at(iDigi);

      // thresholds: if energy is below the corresponding threshold, digi is not considered in clustering

      if (theDigi->GetEnergy() < fDigiEnergyThreshold) {
        continue;
      }
      // fill arrays with digi information for neighbour relationship building
      DigiPassed.push_back(iDigi); // keep track which digis in t_digiCont passed the thresholds, needed to correct for gaps in isAdded[] later on caused by hits being skipped when
                                   // they are below threshold
      BSEmcDetectorID id(theDigi->GetDetectorId());
      Int_t xpad = id.GetXPad();
      Int_t ypad = id.GetYPad();
      Int_t module = id.GetModule();

      if (FairRunAna::Instance()->IsTimeStamp()) {
        TPassed.push_back(theDigi->GetTimeStamp());
      }
      if (xpad < 0 && module == 3) {
        XPadPassed.push_back(xpad + 1); // correct for gaps in FwEndcap
      } else {
        XPadPassed.push_back(xpad);
      }
      if (ypad < 0 && module == 3) {
        YPadPassed.push_back(ypad + 1);
      } else {
        YPadPassed.push_back(ypad);
      }
      nDigisPassed++;
    }

    if (nDigisPassed > nDCdigis) {
      Fatal("Exec", "Attempt to process more digis than are present in this virtual data concentrator");
    }

    if (nDigisPassed == 0) {
      continue; // skip DCs that only have digis with energies below threshold
    }

    //----- First, construct matrix containing information which digis are neighbouring -----//

    for (Int_t d = 0; d < nDigisPassed - 1; d++) { // check all pairs of digis, so all digis up to the second-to-last one
      Int_t nNeighbours = 0;
      neighbours.push_back(0); // placeholder for nr of neighbours
      for (Int_t e = d + 1; e < nDigisPassed; e++) {
        // dx = XPadPassed[e] - XPadPassed[d]; // check euclidian distance between all digis which passed threshold
        // dy = YPadPassed[e] - YPadPassed[d]; // this construction ensures all pairs of digis are only checked once
        // if(FairRunAna::Instance()->IsTimeStamp()) dt = TMath::Abs(TPassed[e] - TPassed[d]);	// also take into account that non-consecutive digi pairs may differ in time more
        // than dtau ns

        /*				if (dy*dy <= deltaD*deltaD && dx*dx <= deltaD*deltaD && dt <= deltaT) { 	// ONLINE VERSION. If the distance is small enough, the digis are neighbours
                  neighbours.push_back(e); // construct array containing neighbouring digis
                  nNeighbours++; // keep track of nr of neighbours
                }*/
        if (fNeighbouringRelationPar->AreNeighbours(t_digiCont.at(DigiPassed[e])->GetDetectorId(), t_digiCont.at(DigiPassed[d])->GetDetectorId()) &&
            dt <= deltaT) {        // BUILT-IN PandaRoot VERSION. Check which digis are neighbours
          neighbours.push_back(e); // construct array containing neighbouring digis
          nNeighbours++;           // keep track of nr of neighbours
        }
      }
      neighbours[neighbours.size() - (nNeighbours + 1)] = nNeighbours; // write nr of neighbours to the appropiate entry in neighbours[]
    }

    // Primary Clustering
    Int_t k = 0;
    Int_t l = 0;
    Int_t nClusters = 0; // nr of preclusters
    Int_t simLength = 0;
    while (l < (Int_t)neighbours.size()) {
      if (isAdded[k] < 0) {                             // if it hasn't been added yet
        isAdded[k] = nClusters;                         // put internal cluster nr in isAdded array
        for (Int_t j = 1; j < neighbours[l] + 1; j++) { // for the first neighbouring hit upto #neighbours for this digi
          Int_t m = neighbours[l + j];                  // m = hit index
          if (isAdded[m] < 0) {
            isAdded[m] = nClusters; // if hit hasn't been added, put internal cluster nr here
          } else if (isAdded[m] != nClusters) {
            if (nClusters > isAdded[m]) {
              similarities.push_back(nClusters);  // if it has, put current cluster nr in this array
              similarities.push_back(isAdded[m]); // together with its cluster nr (could be different, which is why is being stored here for later comparison)
            } else {
              similarities.push_back(isAdded[m]); // different order, so we always have the largest cluster nr first
              similarities.push_back(nClusters);
            }
            simLength += 2; // increment simLength by 2, as we just wrote 2 elements to similarities
          }
        }
        nClusters++;
      } else { // if isAdded[k] isn't -1, it means the digi has been added already and isAdded[k] contains its cluster nr
        for (Int_t j = 1; j < neighbours[l] + 1; j++) {
          Int_t m = neighbours[l + j];
          if (isAdded[m] < 0) {
            isAdded[m] = isAdded[k]; // same as before, only use cluster nr found in isAdded[k]
          } else if (isAdded[m] != isAdded[k]) {
            if (isAdded[k] > isAdded[m]) {
              similarities.push_back(isAdded[k]); // similarities[] keeps tracks of which preclusters have to be merged
              similarities.push_back(isAdded[m]);
            } else {
              similarities.push_back(isAdded[m]); // different order, so we always have the largest cluster nr first
              similarities.push_back(isAdded[k]);
            }
            simLength += 2;
          }
        }
      }
      k++;
      l += neighbours[l] + 1;
    }

    if (isAdded[nDigisPassed - 1] < 0) {
      isAdded[nDigisPassed - 1] = nClusters++;
    }

    // Secondary clustering
    for (Int_t i = 0; i < simLength; i += 2) { // use info from similarities[] to merge preclusters
      if (similarities[i] != similarities[i + 1]) {
        for (Int_t m = 0; m < nDigisPassed; m++) {
          if (isAdded[m] == similarities[i]) {
            isAdded[m] = similarities[i + 1];
          }
        }
        for (Int_t j = i + 2; j < simLength; j++) {
          if (similarities[j] == similarities[i]) {
            similarities[j] = similarities[i + 1];
          }
        }
      }
    }
    for (Int_t i = 0; i < nClusters; i++) {
      Int_t n = 0;
      for (Int_t j = 0; j < nDigisPassed; j++) {
        if (isAdded[j] == i) {
          n++;
          isAdded[j] = nPreclusters; // info in isAdded: digi "index" belongs to precluster "isAdded[index]"
        }
      }
      if (n > 0) {
        nPreclusters++; // precluster nr, don't reset it while searching for preclusters in a timebunch
      }
    }

    //----- Finally, use isAdded to merge digis into clusters -----//

    for (Int_t i = 0; i < nDigisPassed; i++) {
      const BSEmcDigi *digi = t_digiCont.at(DigiPassed[i]);
      Double_t pos_x = NAN, pos_y = NAN, pos_z = NAN;
      TVector3 crystalCentre = fPositionPar->GetPositionData(digi->GetDetectorId()).GetCentre();

      if (fDigiPosType == 0) {
        BSEmcDetectorID detID{digi->GetDetectorId()};
        pos_x = detID.GetXPad();
        pos_y = detID.GetYPad();
      } else if (fDigiPosType == 1) {
        pos_x = crystalCentre.X();
        pos_y = crystalCentre.Y();
      }
      pos_z = crystalCentre.Z();
      if (isAdded[i] < 0) {
        continue;
      }
      if (isAdded[i] < (Int_t)preclusterCont.size()) { // if isAdded[i] is smaller than the current #clusters, the cluster to which this digi belongs already exists, and we
                                                       // should add it to this cluster
        BSEmcPrecluster *precluster = preclusterCont.at(isAdded[i]); // set pointer to the cluster we need, as indicated by isAdded[]
        precluster->AddDigiInfo(DigiPassed[i], digi->GetDetectorId(), digi->GetEnergy(), digi->GetTimeStamp(), pos_x, pos_y, pos_z);
        precluster->AddDigiLink(digi->GetEntryNr());
      } else { // if not, make a new cluster for this digi
        BSEmcPrecluster *newPrecluster = new BSEmcPrecluster();
        preclusterCont.push_back(newPrecluster);
        newPrecluster->AddDigiInfo(DigiPassed[i], digi->GetDetectorId(), digi->GetEnergy(), digi->GetTimeStamp(), pos_x, pos_y, pos_z);
        newPrecluster->AddDigiLink(digi->GetEntryNr());
      }
    }
  }

  // ----- FINISHED BUILDING PRECLUSTERS -----------------------------------

  FinishPreclusters(t_digiCont, preclusterCont);
  std::sort(preclusterCont.begin(), preclusterCont.end(), [](BSEmcPrecluster *a, BSEmcPrecluster *b) { return a->GetTimeStamp() < b->GetTimeStamp(); });

  DCtimer.Stop();
  Double_t DCrtime = DCtimer.RealTime();
  Double_t DCctime = DCtimer.CpuTime();
  DCtimer.Reset();

  DCtotRtime += (DCrtime / nDCs); // keep track how much time was spent on preclustering
  DCtotCtime += (DCctime / nDCs);
  return preclusterCont;
}

void BSEmcMakePreclusters::FinishPreclusters(const std::vector<const BSEmcDigi *> &t_digiCont, const std::vector<BSEmcPrecluster *> &t_preclusterCont)
{
  for (BSEmcPrecluster *precluster : t_preclusterCont) {
    FinishPrecluster(t_digiCont, precluster);
  }
}

void BSEmcMakePreclusters::FinishPrecluster(const std::vector<const BSEmcDigi *> &t_digiCont, BSEmcPrecluster *precluster)
{

  std::vector<BSEmcDigiInfo_t> digiList = precluster->GetDigis();

  Double_t Etot = 0;
  Double_t Emax = 0;
  Double_t maxDist = 0;
  Int_t Emax_idx = 0;

  for (BSEmcDigiInfo_t digiInfo : digiList) {
    Int_t idx = digiInfo.fDigiIdx;
    const BSEmcDigi *thedigi = t_digiCont.at(idx);
    Etot += thedigi->GetEnergy();
    if (thedigi->GetEnergy() > Emax) {
      Emax = thedigi->GetEnergy();
      Emax_idx = idx;
    }
  }
  precluster->SetEnergy(Etot);
  precluster->SetTimeStamp(t_digiCont.at(Emax_idx)->GetTimeStamp());

  TVector3 tmpprecpos;
  TVector3 tempPosition = fPositionProcess->CalculatePosition(precluster, t_digiCont);
  Double_t xmax = -std::numeric_limits<double>::max(); // smaller then smallest x, so it will always write a correct value
  Double_t xmin = std::numeric_limits<double>::max();
  Double_t ymax = -std::numeric_limits<double>::max();
  Double_t ymin = std::numeric_limits<double>::max();
  for (BSEmcDigiInfo_t digiInfo : digiList) {
    Int_t idx = digiInfo.fDigiIdx;
    const BSEmcDigi *thedigi = t_digiCont.at(idx);
    TVector3 digiPosition = fPositionPar->GetPositionData(thedigi->GetDetectorId()).GetCentre();
    BSEmcDetectorID id(thedigi->GetDetectorId());
    Int_t xpad = id.GetXPad();
    Int_t ypad = id.GetYPad();

    if (fPosMethod == 0) { // use built-in method to get estimate for size
      Double_t distanceToCentre = (tempPosition - digiPosition).Mag();
      if (distanceToCentre > maxDist) { // get cluster size (rough estimate)
        maxDist = distanceToCentre;
      }
    } else if (fPosMethod == 1) { // use simplified method to get estimate for size and position
      if (digiPosition.x() > xmax) {
        xmax = digiPosition.x();
      }
      if (digiPosition.x() < xmin) {
        xmin = digiPosition.x();
      }
      if (digiPosition.y() > ymax) {
        ymax = digiPosition.y();
      }
      if (digiPosition.y() < ymin) {
        ymin = digiPosition.y();
      }
    } else if (fPosMethod == 2) { // use simplified method with mapping to get estimate for size and position
      if (xpad > xmax) {
        xmax = xpad;
      }
      if (xpad < xmin) {
        xmin = xpad;
      }
      if (ypad > ymax) {
        ymax = ypad;
      }
      if (ypad < ymin) {
        ymin = ypad;
      }
    }
  }
  TVector3 maxDigiPos = fPositionPar->GetPositionData(t_digiCont.at(Emax_idx)->GetDetectorId()).GetCentre();
  BSEmcClusterRadiusAlgo radiusCalculator{};

  if (fPosMethod == 0) { // use built-in method to get estimate for position
    tmpprecpos = fPositionProcess->CalculatePosition(precluster, t_digiCont);
    precluster->SetPosition(tmpprecpos);
  }
  if (fPosMethod == 1) {
    Double_t yradius = 0.5 * (ymax - ymin);
    Double_t xradius = 0.5 * (xmax - xmin);
    precluster->SetXRadius(radiusCalculator.GetModifiedYRadius(yradius));
    precluster->SetYRadius(radiusCalculator.GetModifiedXRadius(xradius));
    yradius > xradius ? maxDist = yradius : maxDist = xradius;
    Double_t zpos = maxDigiPos.z();
    tmpprecpos.SetXYZ(xmax - xradius, ymax - yradius, zpos);
    precluster->SetPosition(tmpprecpos);
  } else if (fPosMethod == 2) { // (to have the radius an int, make everything twice as big)
    Double_t yradius = ymax - ymin;
    Double_t xradius = xmax - xmin;
    precluster->SetXRadius(radiusCalculator.GetModifiedYRadius(yradius, 2));
    precluster->SetYRadius(radiusCalculator.GetModifiedXRadius(xradius, 2));
    yradius > xradius ? maxDist = yradius : maxDist = xradius;
    Double_t zpos = maxDigiPos.z();
    tmpprecpos.SetXYZ(2 * xmax + 2 * xmin, 2 * ymax + 2 * ymin, zpos);
    precluster->SetPosition(tmpprecpos);
  }
  precluster->SetRadius(radiusCalculator.GetModifiedRadius(
    maxDist, fPosMethod)); // new function added to PndEmcCluster (correction for crystal size already in function): arguments: Double_t calculated radius, Int_t
                           // multiplier for addition of single-cluster radius (default value: 1, set to 2 to use diameter [automatically set by using fPosMethod
                           // as input, with multiplier set to 1 if fPosMethod=0], set to higher values to manually increase radius)

  // -------------- stop (end of precluster finding) -------------------------------------------------------
}

ClassImp(BSEmcMakePreclusters)
