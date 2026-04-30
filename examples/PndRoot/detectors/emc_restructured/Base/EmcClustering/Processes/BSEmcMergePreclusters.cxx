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

#include "BSEmcMergePreclusters.h"

#include <algorithm>
#include <math.h>
#include <vector>

#include "TMath.h"
#include "TMathBase.h"
#include "TStopwatch.h"
#include "TString.h"
#include "TVector3.h"

#include "FairParSet.h"
#include "fairlogger/Logger.h"

#include "PndContainerI.h"
#include "PndMutableContainerI.h"
#include "PndParameterRegister.h"

#include "BSEmcCluster.h"
#include "BSEmcClusterPropertiesPar.h"

#include "BSEmcCrystalPositionPar.h"
#include "BSEmcDetectorID.h"
#include "BSEmcDigi.h"
#include "BSEmcGeoNeighbouringRelationPar.h"
#include "BSEmcPrecluster.h"

BSEmcMergePreclusters::BSEmcMergePreclusters() : PndProcess{"BSEmcMergePreclusters"}, fClusteringParName(""), fPositionParName(""), fNeighbouringRelationParName("") {}

BSEmcMergePreclusters::~BSEmcMergePreclusters() {}

void BSEmcMergePreclusters::SetDetectorName(const std::string &t_detectorName)
{
  fDetectorName = t_detectorName;
  if (fClusterPropertiesParName == "") {
    fClusterPropertiesParName = BSEmcClusterPropertiesPar::fgParameterName + fDetectorName;
  }
  if (fPositionParName == "") {
    fPositionParName = BSEmcCrystalPositionPar::fgParameterName + fDetectorName;
  }
  if (fNeighbouringRelationParName == "") {
    fNeighbouringRelationParName = BSEmcGeoNeighbouringRelationPar::fgParameterName + fDetectorName;
  }

  fParameterList.push_back(fClusterPropertiesParName);
  fParameterList.push_back(fNeighbouringRelationParName);
  fParameterList.push_back(fPositionParName);
}

void BSEmcMergePreclusters::SetupParameters(const PndParameterRegister *t_paramRegister)
{

  fPositionPar = dynamic_cast<BSEmcCrystalPositionPar *>(t_paramRegister->GetParameter(fPositionParName.c_str()));
  {
    BSEmcClusterPropertiesPar *parameter = dynamic_cast<BSEmcClusterPropertiesPar *>(t_paramRegister->GetParameter(fClusterPropertiesParName));
    fPositionProcess->SetClusterPropertiesPar(parameter);
    fPositionProcess->SetPositionPar(fPositionPar);
  }
  LOG(debug) << "BSEmcMergePreclusters::Init(): Start Initialization";

  if (fPosMethod == 0) {
    LOG(debug) << "Using LOGARITHMIC ENERGY WEIGHING for the cluster position calculation";
  } else if (fPosMethod == 1) {
    LOG(debug) << "Using LINEAR ENERGY WEIGHING for the cluster position calculation";
  } else if (fPosMethod == 2) {
    LOG(debug) << "Using SIMPLE LINEAR WEIGHING for the cluster position calculation";
  } else if (fPosMethod == 3) {
    LOG(debug) << "Using the REAL x,y position of digi with highest energy for the cluster position.";
  } else {
    LOG(error) << "BSEmcMergePreclusters::SetupParameters - Invalid position method specified. Selecting recommended default instead. ";
    fPosMethod = 0;
  }
  if (fNbMethod == 0) {
    LOG(debug) << "Using DEFAULT precluster radius method. (REAL X,Y CIRCLE)";
  } else if (fNbMethod == 1) {
    LOG(debug) << "Using SIMPLIFIED precluster radius method (CIRCLE).";
  } else if (fNbMethod == 2) {
    LOG(debug) << "Using SIMPLIFIED precluster radius method (RECTANGULAR BOX).";
  } else if (fNbMethod == 3) {
    LOG(debug) << "Using SIMPLIFIED precluster radius method (SQUARE BOX).";
  } else {
    LOG(error) << "BSEmcMergePreclusters::SetupParameters - Invalid neighbour method specified. Selecting recommended default instead. ";
    fNbMethod = 0;
  }

  LOG(debug) << "IMPORTANT: Did you remember to change BSEmcClusterRingSorter.cxx back to use BSEmcCluster instead of BSEmcPrecluster objects?";
  LOG(debug) << "           Also, don't forget to compile again after making these changes ;)\n";

  LOG(debug) << "BSEmcMergePreclusters: Intialization successful";
}

void BSEmcMergePreclusters::RequestDataContainer(PndContainerRegister *t_register)
{
  t_register->Request({fDigiBranchname, "BSEmcDigi", kFALSE, kFALSE});
  t_register->Request({fPreclusterBranchname, "BSEmcPrecluster", kTRUE, kTRUE});
  t_register->Request({fClusterBranchname, "BSEmcCluster", kTRUE, kTRUE});
}

void BSEmcMergePreclusters::GetDataContainer(PndContainerRegister *t_register)
{
  fDigiArray = t_register->GetInput<BSEmcDigi>(fDigiBranchname);
  fPreclusterArray = t_register->GetOutput<BSEmcPrecluster>(fPreclusterBranchname);
  fClusterArray = t_register->GetOutput<BSEmcCluster>(fClusterBranchname);

  if (fDigiBranchname != "") {
    t_register->SetAsDefaultBranchFor(fDigiBranchname, "BSEmcDigi");
  }
  if (fPreclusterBranchname != "") {
    t_register->SetAsDefaultBranchFor(fPreclusterBranchname, "BSEmcPrecluster");
  }
  if (fClusterBranchname != "") {
    t_register->SetAsDefaultBranchFor(fClusterBranchname, "BSEmcCluster");
  }
}

void BSEmcMergePreclusters::Process()
{
  fClusterArray->Reset();

  LOG(debug) << "BSEmcMergePreclusters::Process() attempting to create cluster out of " << fPreclusterArray->GetSize() << " " << fPreclusterArray->GetBranchName()
             << " precluster with " << fDigiArray->GetSize() << " " << fDigiArray->GetBranchName() << " digis.";

  std::vector<BSEmcCluster *> clusterCont = MergePreclusters(fDigiArray->GetVectorOfPtrToConst(), fPreclusterArray->GetVector());

  LOG(debug) << "BSEmcMergePreclusters::Process() created " << clusterCont.size() << "cluster out of " << fPreclusterArray->GetSize() << " " << fPreclusterArray->GetBranchName()
             << " precluster with " << fDigiArray->GetSize() << " " << fDigiArray->GetBranchName() << " digis.";

  for (BSEmcCluster *cluster : clusterCont) {
    fClusterArray->CreateCopy(*cluster);
    delete cluster;
  }
}

std::vector<BSEmcCluster *> BSEmcMergePreclusters::MergePreclusters(const std::vector<const BSEmcDigi *> &t_digiCont, const std::vector<BSEmcPrecluster *> &t_preclusterCont)
{
  // -----   Timer for clustering  -------------------------------------
  TStopwatch CNtimer;
  CNtimer.Start();
  fClusterNr = 0;

  // ----- START MERGING PRECLUSTERS INTO 'REAL' CLUSTERS ------------------
  std::vector<Int_t> neighbours = FindNeighbouringPrecluster(t_preclusterCont); // array keeping track which preclusters are neighbours
  std::vector<Int_t> addPreclusterToClusterNr = ClusterNeigbours(neighbours, t_preclusterCont);

  std::vector<BSEmcCluster *> clusterCont = MergePreclustersIntoCluster(addPreclusterToClusterNr, t_preclusterCont);
  FinishClusters(t_digiCont, clusterCont);

  std::sort(clusterCont.begin(), clusterCont.end(), [](BSEmcCluster *a, BSEmcCluster *b) { return a->GetTimeStamp() < b->GetTimeStamp(); });
  // ----- FINISHED MERGING PRECLUSTERS ------------------------------------
  Double_t CNrtime = CNtimer.RealTime();
  Double_t CNctime = CNtimer.CpuTime();
  CNtotRtime += CNrtime; // keep track how much time was spent on clustering
  CNtotCtime += CNctime;
  return clusterCont;
}

std::vector<Int_t> BSEmcMergePreclusters::FindNeighbouringPrecluster(const std::vector<BSEmcPrecluster *> &t_preclusterCont)
{
  Double_t dt = 0;
  Double_t deltaT = 0;
  std::vector<Int_t> neighbours; // array with cluster numbers for each precluster
  Int_t nNeighbours = 0;         // #neighbouring preclusters
  Int_t nPres = t_preclusterCont.size();

  for (Int_t iClus = 0; iClus < nPres; ++iClus) {

    precCounter++;

    /* Get a new precluster from the precluster array */
    BSEmcPrecluster *preclus1 = t_preclusterCont[iClus];

    if (iClus == nPres - 1) {
      continue; // don't check the last one (not needed anyway, but it is included in the loop so that its digis are loaded in the new digi array)
    }

    nNeighbours = 0;         // reset nNeighbours
    neighbours.push_back(0); // placeholder for nr of neighbours

    for (Int_t j = iClus + 1; j < nPres; j++) {

      BSEmcPrecluster *preclus2 = t_preclusterCont[j];
      // TODO: Is this enough for sufficient timeclustering? Might have to do spatial-time-spatial-clustering
      /*if(FairRunAna::Instance()->IsTimeStamp()) {
        dt = TMath::Abs(preclus1->GetTimeStamp()-preclus2->GetTimeStamp());
        deltaT = fClusterActiveTime; // threshold for time between digis in a cluster
      }*/

      //																										     ^
      // tag which preclusters should be merged														     |
      //							spatial distance														     time difference    (use the same as for digis)
      //									|																				   |       |
      //									V																	  			   V 	   V
      if (fNbMethod == 0) { //																								(DEFAULT METHOD: REAL POSITION CIRCLE (most accurate)
        Double_t distanceToCentre = (preclus1->GetPosition() - preclus2->GetPosition()).Mag();
        if (distanceToCentre <= (preclus1->GetRadius() + preclus2->GetRadius()) && dt <= deltaT) {
          neighbours.push_back(j);
          nNeighbours++;
        }
      } else if (fNbMethod == 1) { //																						(HARDWARE METHOD: CIRCLE)
        Double_t dx = preclus2->GetPosition().x() - preclus1->GetPosition().x();
        Double_t dy = preclus2->GetPosition().y() - preclus1->GetPosition().y();
        Double_t rTot = preclus1->GetRadius() + preclus2->GetRadius();
        if ((dx * dx + dy * dy) <= (rTot * rTot) && dt <= deltaT) {
          neighbours.push_back(j);
          nNeighbours++;
        }
      } else if (fNbMethod == 2) { //																						(HARDWARE METHOD: RECTANGULAR BOX)
        if (TMath::Abs(preclus2->GetPosition().x() - preclus1->GetPosition().x()) <= (preclus1->GetXRadius() + preclus2->GetXRadius()) &&
            TMath::Abs(preclus2->GetPosition().y() - preclus1->GetPosition().y()) <= (preclus1->GetYRadius() + preclus2->GetYRadius()) && dt <= deltaT) {
          neighbours.push_back(j);
          nNeighbours++;
        }
      } else if (fNbMethod == 3) { //																						(HARDWARE METHOD: SQUARE BOX)
        if (TMath::Abs(preclus2->GetPosition().x() - preclus1->GetPosition().x()) <= (preclus1->GetRadius() + preclus2->GetRadius()) &&
            TMath::Abs(preclus2->GetPosition().y() - preclus1->GetPosition().y()) <= (preclus1->GetRadius() + preclus2->GetRadius()) && dt <= deltaT) {
          neighbours.push_back(j);
          nNeighbours++;
        }
      }
    }
    neighbours[neighbours.size() - (nNeighbours + 1)] = nNeighbours; // write nr of neighbours to the appropiate entry in neighbours[]
  }
  return neighbours;
}

std::vector<Int_t> BSEmcMergePreclusters::ClusterNeigbours(const std::vector<Int_t> &t_neighbours, const std::vector<BSEmcPrecluster *> &t_preclusterCont)
{
  std::vector<Int_t> mergeIntoCluster; // array with cluster numbers for each precluster
  std::vector<Int_t> similarities2;    // array keeping track which clusters should be merged

  Int_t nPres = t_preclusterCont.size();

  for (Int_t a = 0; a < nPres; a++) {
    mergeIntoCluster.push_back(-1); // initialise all entries of isAdded2 to -1 (needed by algorithm)
  }

  // Primary Clustering
  Int_t k = 0;
  Int_t l = 0;
  Int_t nClusters = 0; // nr of clusters
  Int_t simLength = 0;
  while (l < (Int_t)t_neighbours.size()) {
    if (mergeIntoCluster[k] < 0) {                      // if it hasn't been added yet
      mergeIntoCluster[k] = nClusters;                  // put cluster nr in isAdded2 array
      for (Int_t j = 1; j < t_neighbours[l] + 1; j++) { // for the first neighbouring hit upto #neighbours for this precluster
        Int_t m = t_neighbours[l + j];                  // m = hit index
        if (mergeIntoCluster[m] < 0) {
          mergeIntoCluster[m] = nClusters; // if hit hasn't been added, put cluster nr here
        } else if (mergeIntoCluster[m] != nClusters) {
          if (nClusters > mergeIntoCluster[m]) {
            similarities2.push_back(nClusters);           // if it has, put current cluster nr in this array
            similarities2.push_back(mergeIntoCluster[m]); // together with its cluster nr (could be different, which is why is being stored here for later comparison)
          } else {
            similarities2.push_back(mergeIntoCluster[m]); // different order, so we always have the largest cluster nr first
            similarities2.push_back(nClusters);
          }
          simLength += 2; // increment simLength by 2, as we just wrote 2 elements to similarities
        }
      }
      nClusters++;
    } else { // if mergeIntoCluster[k] isn't -1, it means the precluster has been added already and mergeIntoCluster[k] contains its cluster nr
      for (Int_t j = 1; j < t_neighbours[l] + 1; j++) {
        Int_t m = t_neighbours[l + j];
        if (mergeIntoCluster[m] < 0) {
          mergeIntoCluster[m] = mergeIntoCluster[k]; // same as before, only use cluster nr found in mergeIntoCluster[k]
        } else if (mergeIntoCluster[m] != mergeIntoCluster[k]) {
          if (mergeIntoCluster[k] > mergeIntoCluster[m]) {
            similarities2.push_back(mergeIntoCluster[k]); // similarities2[] keeps tracks of which clusters have to be merged
            similarities2.push_back(mergeIntoCluster[m]);
          } else {
            similarities2.push_back(mergeIntoCluster[m]); // different order, so we always have the largest cluster nr first
            similarities2.push_back(mergeIntoCluster[k]);
          }
          simLength += 2;
        }
      }
    }
    k++;
    l += t_neighbours[l] + 1;
  }

  if (nPres != 0) {
    if (mergeIntoCluster[nPres - 1] < 0) {
      mergeIntoCluster[nPres - 1] = nClusters++;
    }
  }

  // Secondary clustering
  for (Int_t i = 0; i < simLength; i += 2) { // use info from similarities2[] to merge preclusters
    if (similarities2[i] != similarities2[i + 1]) {
      for (Int_t m = 0; m < nPres; m++) {
        if (mergeIntoCluster[m] == similarities2[i]) {
          mergeIntoCluster[m] = similarities2[i + 1];
        }
      }
      for (Int_t j = i + 2; j < simLength; j++) {
        if (similarities2[j] == similarities2[i]) {
          similarities2[j] = similarities2[i + 1];
        }
      }
    }
  }
  for (Int_t i = 0; i < nClusters; i++) {
    Int_t n = 0;
    for (Int_t j = 0; j < nPres; j++) {
      if (mergeIntoCluster[j] == i) {
        n++;
        mergeIntoCluster[j] = fClusterNr; // info in mergeIntoCluster: precluster "index" belongs to cluster "mergeIntoCluster[index]"
      }
    }
    if (n > 0) {
      fClusterNr++;
    }
  }

  return mergeIntoCluster;
}

std::vector<BSEmcCluster *>
BSEmcMergePreclusters::MergePreclustersIntoCluster(const std::vector<Int_t> &t_clusterNrForPrecluster, const std::vector<BSEmcPrecluster *> &t_preclusterCont)
{
  std::vector<BSEmcCluster *> clusters;
  TVector3 posVector;
  Double_t energyWeighFactor = NAN;
  Double_t energyWeightedTime = NAN;
  Double_t energyWeightedXPos = NAN;
  Double_t energyWeightedYPos = NAN;
  Double_t energyWeightedZPos = NAN;
  Double_t maxEn = 0;
  Double_t clusEn = 0;
  Double_t En = 0;
  Double_t W0 = 0;
  Double_t logWeight = 0;
  Double_t flightTime = 0;
  Int_t maxEn_idx = 0;
  Short_t clusModule = 0;
  Short_t sumModule = 0;
  Int_t nPrec = 0;
  std::vector<Double_t> currentDigisT;
  std::vector<Double_t> currentDigisE;
  std::vector<Double_t> currentDigisX;
  std::vector<Double_t> currentDigisY;
  std::vector<Double_t> currentDigisZ;
  std::vector<Double_t> memberDigiTimes;
  std::vector<Double_t> memberDigiEnergies;
  std::vector<Double_t> memberDigiXpos;
  std::vector<Double_t> memberDigiYpos;
  std::vector<Double_t> memberDigiZpos;

  for (Int_t iClus = 0; iClus < fClusterNr; iClus++) {
    BSEmcCluster *newCluster = new BSEmcCluster(); // make a new empty cluster
    clusters.push_back(newCluster);
    posVector.Clear();
    memberDigiTimes.clear();
    memberDigiEnergies.clear();
    memberDigiXpos.clear();
    memberDigiYpos.clear();
    memberDigiZpos.clear();
    currentDigisT.clear();
    currentDigisE.clear();
    currentDigisX.clear();
    currentDigisY.clear();
    currentDigisZ.clear();
    clusEn = 0;
    sumModule = 0;
    nPrec = 0;
    Int_t nPres = t_preclusterCont.size();
    for (Int_t i = 0; i < nPres; i++) {           // get all preclusters which should be added to the current cluster
      if (t_clusterNrForPrecluster[i] == iClus) { // using info from isAdded2[]
        BSEmcPrecluster *thisPrecluster = t_preclusterCont[i];

        currentDigisT = thisPrecluster->GetMemberDigiTimes();
        currentDigisE = thisPrecluster->GetMemberDigiEnergies();
        currentDigisX = thisPrecluster->GetMemberDigiXpos();
        currentDigisY = thisPrecluster->GetMemberDigiYpos();
        currentDigisZ = thisPrecluster->GetMemberDigiZpos();

        memberDigiTimes.insert(memberDigiTimes.end(), currentDigisT.begin(), currentDigisT.end());
        memberDigiEnergies.insert(memberDigiEnergies.end(), currentDigisE.begin(), currentDigisE.end());
        memberDigiXpos.insert(memberDigiXpos.end(), currentDigisX.begin(), currentDigisX.end());
        memberDigiYpos.insert(memberDigiYpos.end(), currentDigisY.begin(), currentDigisY.end());
        memberDigiZpos.insert(memberDigiZpos.end(), currentDigisZ.begin(), currentDigisZ.end());
        const Int_t detId = thisPrecluster->GetDigis()[0].fDetectorId;
        clusModule = BSEmcDetectorID{detId}.GetModule();
        clusEn += thisPrecluster->GetEnergy();
        sumModule += clusModule;
        nPrec++;
        newCluster->AddCluster(*thisPrecluster);
      }
    }
    if (nPrec != 0) {
      if (sumModule / nPrec != clusModule && ((clusModule != 1 || clusModule != 2) && (sumModule / nPrec != 1 || sumModule / nPrec != 2))) {
        wrongConnection++;
      }
    }
    En = 0;
    maxEn = 0;
    maxEn_idx = 0;
    if (fPosMethod == 0) {
      W0 = 4.071 - 0.678 * TMath::Power(clusEn, -0.534) * TMath::Exp(-TMath::Power(clusEn, 1.171));
    }
    energyWeighFactor = 0;
    energyWeightedTime = 0;
    energyWeightedXPos = 0;
    energyWeightedYPos = 0;
    energyWeightedZPos = 0;
    nTotClusters++;
    // LOG(debug) << "\ncluster " << iClus << ", energy: " << clusEn;
    for (Int_t j = 0; j < (Int_t)memberDigiTimes.size(); j++) {
      En = memberDigiEnergies[j];
      nTotDigis++;
      if (fPosMethod == 0) { // use logarithmic energy weighing
        logWeight = W0 + TMath::Log(En / clusEn);
        if (logWeight < 0) {
          continue; // discard contributions from low-energy hits
        }
        energyWeighFactor += logWeight;
        energyWeightedTime += logWeight * memberDigiTimes[j];
        energyWeightedXPos += logWeight * memberDigiXpos[j];
        energyWeightedYPos += logWeight * memberDigiYpos[j];
        energyWeightedZPos += logWeight * memberDigiZpos[j];
        // LOG(debug) << "    digi " << j << ": E=" << En << ", x=" << memberDigiXpos[j] << ", y=" << memberDigiYpos[j] << ", logWeight=" << logWeight;
      } else if (fPosMethod == 1) { // use linear energy weighing
        energyWeighFactor += En;
        energyWeightedTime += En * memberDigiTimes[j];
        energyWeightedXPos += En * memberDigiXpos[j];
        energyWeightedYPos += En * memberDigiYpos[j];
        energyWeightedZPos += En * memberDigiZpos[j];
      } else if (fPosMethod == 2) { // use linear weighing
        energyWeighFactor++;
        energyWeightedTime += memberDigiTimes[j];
        energyWeightedXPos += memberDigiXpos[j];
        energyWeightedYPos += memberDigiYpos[j];
        energyWeightedZPos += memberDigiZpos[j];
      }
      if (En > maxEn) {
        maxEn_idx = j;
        maxEn = En;
      }
    }
    newCluster->SetEnergy(clusEn);
    if (fPosMethod == 3) {
      posVector.SetXYZ(memberDigiXpos[maxEn_idx], memberDigiYpos[maxEn_idx], memberDigiZpos[maxEn_idx]); // use position of digi with highest energy
    } else {
      if (energyWeighFactor != 0.0) {
        posVector.SetXYZ(energyWeightedXPos / energyWeighFactor, energyWeightedYPos / energyWeighFactor, energyWeightedZPos / energyWeighFactor); // use weighted position
      } else {
        posVector.SetXYZ(0, 0, 0);
      }
    }
    newCluster->SetPosition(posVector);
    flightTime = posVector.Mag() / 29.9792458;                         // flight time in ns = distance to IP (in cm) / c
                                                                       //		newCluster->SetTimeStamp(energyWeightedTime/energyWeighFactor-flightTime); // use energy-weighted time
    newCluster->SetTimeStamp(memberDigiTimes[maxEn_idx] - flightTime); // use time of most energetic hit
  }
  return clusters;
}

void BSEmcMergePreclusters::FinishClusters(const std::vector<const BSEmcDigi *> &t_digiCont, const std::vector<BSEmcCluster *> &t_clusterCont)
{
  for (BSEmcCluster *cluster : t_clusterCont) {
    FinishCluster(t_digiCont, cluster);
  }
}

void BSEmcMergePreclusters::FinishCluster(const std::vector<const BSEmcDigi *> &t_digiCont, BSEmcCluster *t_cluster)
{

  //----set energy and time etc. to the clusters

  std::vector<BSEmcDigiInfo_t> list = t_cluster->GetDigis();

  Double_t total_energy = 0;
  Double_t max_energy = 0;
  Int_t max_energy_idx = 0;

  TVector3 tmpbumppos;
  for (BSEmcDigiInfo_t digiInfo : list) {
    Int_t idx = digiInfo.fDigiIdx;
    const BSEmcDigi *thedigi = t_digiCont[idx];
    total_energy += thedigi->GetEnergy();
    if (thedigi->GetEnergy() > max_energy) {
      max_energy = thedigi->GetEnergy();
      max_energy_idx = idx;
    }
  }

  t_cluster->SetEnergy(total_energy);

  if (fPosMethod == 3) { // use position of digi with highest energy
    TVector3 digiPos = fPositionPar->GetPositionData(t_digiCont[max_energy_idx]->GetDetectorId()).GetCentre();
    Double_t clusx = digiPos.x();
    Double_t clusy = digiPos.y();
    Double_t clusz = digiPos.z();
    tmpbumppos.SetXYZ(clusx, clusy, clusz);
    t_cluster->SetPosition(tmpbumppos);
  } else if (fPosMethod == 0) { // use default position method from fRecoPar
    tmpbumppos = fPositionProcess->CalculatePosition(t_cluster, t_digiCont);
    t_cluster->SetPosition(tmpbumppos);
  }

  // Double_t flightTime = tmpbumppos.Mag() / 29.9792458; // flight time in ns = distance to IP / c

  // t_cluster->SetTimeStamp(t_digiCont.at(max_energy_idx)->GetTimeStamp() - flightTime); // correct t_cluster time for photon flight time
  t_cluster->SetTimeStamp(t_digiCont[max_energy_idx]->GetTimeStamp());

  //-------------------stop-------------------------------------------------------------
}

ClassImp(BSEmcMergePreclusters)
