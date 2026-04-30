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
// Description:
//           energy()            returns energy sum of digis in cluster.
//           where()             returns a TVector3 at the centre of the
//                                 cluster.
//           x()                 returns the x element of this.
//           y()                 returns the y element of this.
//           z()                 returns the z element of this.
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Author List:
//	Xiaorong Shi            Lawrence Livermore National Lab
//	Steve Playfer           University of Edinburgh
//	Stephen Gowdy           University of Edinburgh
//	Helmut Marsiske         SLAC

//---------------------------------------------------------------------

#include "PndEmcCluster.h"

#include "FairRunAna.h"
#include "FairLogger.h"

#include "PndEmcDigi.h"
#include "PndEmcXtal.h"
#include "PndEmcStructure.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TClonesArray.h"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <cfloat>
#include <vector>
#include <utility>
#include "assert.h"

using std::endl;
using std::vector;

//----------------
// Constructors --
//----------------

PndEmcCluster::PndEmcCluster()
  : fDigiList(), fMcList(), fMemberDigiMap(), fLocalMaxMap(), fEnergyValid(false), fEnergy(0), fWhereValid(false), fWhere(TVector3(0, 0, 0)), fNbumps(0), fZ20(0), fZ53(0),
    fLatMom(0), fTrackEntering(), fTrackExiting(), fRadius(0), fRadiusValid(false), fModule(0)
{
  fDigiList.clear();
  fMcList.clear();
  fMemberDigiMap.clear();
  fLocalMaxMap.clear();
}

//--------------
// Destructor --
//--------------

PndEmcCluster::~PndEmcCluster() {}

// Double_t PndEmcCluster::energy() const { // old name of energy getter, simply calls new energy getter
//	GetEnergy();
//}

Double_t PndEmcCluster::GetEnergy() const
{
  if (fEnergyValid)
    return fEnergy;
  else {
    LOG(error) << " in PndEmcCluster::GetEnergy(): Energy of cluster is not defined";
    abort();
  }
}

Double_t PndEmcCluster::theta() const
{
  return where().Theta();
}

Double_t PndEmcCluster::phi() const
{
  return where().Phi();
}

TVector3 PndEmcCluster::position() const
{
  return this->where();
}

TVector3 PndEmcCluster::where() const
{
  if (fWhereValid)
    return fWhere;
  else {
    LOG(error) << " in PndEmcCluster::where(): Position of cluster is not defined";
    abort();
  }
}

Double_t PndEmcCluster::x() const
{
  return where().x();
}

Double_t PndEmcCluster::y() const
{
  return where().y();
}

Double_t PndEmcCluster::z() const
{
  return where().z();
}

Double_t PndEmcCluster::FindPhiDiff(Double_t phi1, Double_t phi2)
{
  Double_t diff;
  diff = phi1 - phi2;

  while (diff > TMath::Pi())
    diff -= 2 * TMath::Pi();
  while (diff < -TMath::Pi())
    diff += 2 * TMath::Pi();

  return diff;
}

Double_t PndEmcCluster::GetRadius() const
{
  if (fRadiusValid)
    return fRadius;
  else {
    LOG(error) << " in PndEmcCluster::GetRadius(): Radius of cluster is not defined";
    abort();
  }
}

Short_t PndEmcCluster::GetModule() const
{
  return fModule;
}

//-------------
// Modifiers --
//-------------

void PndEmcCluster::addDigi(const TClonesArray *digiArray, Int_t iDigi)
{
  fDigiList.push_back(iDigi);
  PndEmcDigi *digi = (PndEmcDigi *)digiArray->At(iDigi);
  Int_t detectorId = digi->GetDetectorId();
  fMemberDigiMap.insert(std::pair<Int_t, Int_t>(detectorId, iDigi));

  fTimeStamp = (digi->GetTimeStamp() > this->GetTimeStamp()) ? digi->GetTimeStamp() : this->GetTimeStamp();
  // std::cout<<"digi belongs to track #"<<digi->GetTrackId()<<std::endl;
  ++fMcMap[digi->GetTrackId()];
  invalidateCache(kFALSE);
  SetInsertHistory(kFALSE);
  if (FairRunAna::Instance()->IsTimeStamp()) {
    AddLink((static_cast<PndEmcDigi *>(digiArray->At(iDigi))->GetEntryNr()));
  } else {
    AddLink(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), "EmcDigi", iDigi));
  }
  SetInsertHistory(kTRUE);
}
void PndEmcCluster::removeDigi(const TClonesArray *digiArray, Int_t iDigi)
{
  for (size_t i = 0; i < fDigiList.size(); ++i) {
    if (fDigiList[i] == iDigi) {
      fDigiList.erase(fDigiList.begin() + i);
      break;
    }
  }
  PndEmcDigi *digi = (PndEmcDigi *)digiArray->At(iDigi);
  Int_t detectorId = digi->GetDetectorId();
  fMemberDigiMap.erase(detectorId);
  --fMcMap[digi->GetTrackId()];
}
std::vector<Int_t>::iterator PndEmcCluster::removeDigi(const TClonesArray *digiArray, std::vector<Int_t>::iterator iDigi)
{
  PndEmcDigi *digi = (PndEmcDigi *)digiArray->At(*iDigi);
  // std::cout << "removing digi (ptr|detId): (" << digi << " | " << digi->GetDetectorId() << ")" << std::endl;

  std::map<Int_t, Int_t>::iterator it = fMemberDigiMap.find(digi->GetDetectorId());
  if (it != fMemberDigiMap.end() && it->second == *iDigi) {
    fMemberDigiMap.erase(it);
  }

  --fMcMap[digi->GetTrackId()];
  return fDigiList.erase(iDigi);
}

void PndEmcCluster::addCluster(PndEmcCluster *cluster, const TClonesArray *digiArray)
{
  const vector<Int_t> tmpList = cluster->DigiList();
  vector<Int_t>::const_iterator digi_iter;
  for (digi_iter = tmpList.begin(); digi_iter != tmpList.end(); ++digi_iter) {
    addDigi(digiArray, *digi_iter);
  }
  AddTracksEnteringExiting(cluster->GetTrackEntering(), cluster->GetTrackExiting());
  AddLinks(cluster->GetLinks());
}

void PndEmcCluster::addLocalMax(const TClonesArray *digiArray, Int_t iDigi)
{
  PndEmcDigi *digi = (PndEmcDigi *)digiArray->At(iDigi);
  Int_t detectorId = digi->GetDetectorId();
  fLocalMaxMap.insert(std::pair<Int_t, Int_t>(detectorId, iDigi));
};

void PndEmcCluster::addLocalMax(const PndEmcDigi *digi) ///// TIMEBASED EDIT
{
  if (FairRunAna::Instance()->IsTimeStamp()) {
    std::cout << "-E- PndEmcCluster::addLocalMax: May not use this method in timebased sim. Call PndEmcCluster::addLocalMax(const TClonesArray *digiArray, Int_t iDigi) instead. "
              << endl;
    abort();
  } else {
    Int_t detectorId = digi->GetDetectorId();
    Int_t iDigi = fMemberDigiMap.find(detectorId)->second;
    fLocalMaxMap.insert(std::pair<Int_t, Int_t>(detectorId, iDigi));
  }
};

// check if a digi belong to this cluster
bool PndEmcCluster::isInCluster(PndEmcDigi *theDigi, const TClonesArray *digiArray)
{
  vector<Int_t>::iterator digi_iter;
  for (digi_iter = fDigiList.begin(); digi_iter != fDigiList.end(); ++digi_iter) {
    if (theDigi->isNeighbour((PndEmcDigi *)digiArray->At(*digi_iter)))
      return true;
  }

  return false;
}

// Return digi with the highest energy in cluster
const PndEmcDigi *PndEmcCluster::Maxima(const TClonesArray *digiArray) const
{
  Double_t max = 0;
  PndEmcDigi *biggest = nullptr;

  std::vector<Int_t>::const_iterator digipos;
  for (digipos = fDigiList.begin(); digipos != fDigiList.end(); ++digipos) {
    PndEmcDigi *digi = (PndEmcDigi *)digiArray->At(*digipos);
    if (max < digi->GetEnergy()) {
      max = digi->GetEnergy();
      biggest = digi;
    }
  }

  return (biggest);
}

PndEmcDigi *PndEmcCluster::Maxima(const TClonesArray *digiArray)
{
  Double_t max = 0;
  PndEmcDigi *biggest = nullptr;

  std::vector<Int_t>::iterator digipos;
  for (digipos = fDigiList.begin(); digipos != fDigiList.end(); ++digipos) {
    PndEmcDigi *digi = (PndEmcDigi *)digiArray->At(*digipos);
    if (max < digi->GetEnergy()) {
      max = digi->GetEnergy();
      biggest = digi;
    }
  }

  return (biggest);
}

int PndEmcCluster::MaximaAsIndex(const TClonesArray *digiArray)
{
  Double_t max = 0;
  int biggestAsIndex = -1;

  std::vector<Int_t>::iterator digipos;
  for (digipos = fDigiList.begin(); digipos != fDigiList.end(); ++digipos) {
    PndEmcDigi *digi = static_cast<PndEmcDigi *>(digiArray->At(*digipos));
    if (max < digi->GetEnergy()) {
      max = digi->GetEnergy();
      biggestAsIndex = *digipos;
    }
  }

  return (biggestAsIndex);
}
/* // DISABLED BECAUSE OF TIMEBASED SIM. SET MODULE NR (fModule) WHEN CLUSTER IS CREATED BY CALLING PndEmcCluster::SetModule()
Short_t PndEmcCluster::GetModule() const
{
  // Get module in which cluster is located
  // If fLocalMaxMap is defined, i.e. after bump splitting procedure
  // the number of module if taken from the first local maxima.
  // Otherwise it is taken from the first digi in cluster.
  Short_t module;
  Int_t detectorId;
  if (fLocalMaxMap.size()>0)
  {
    std::map<Int_t,Int_t>::const_iterator iter=fLocalMaxMap.begin();
    detectorId=iter->first;
  }
  else
  {
    std::map<Int_t,Int_t>::const_iterator iter=fMemberDigiMap.begin();
    detectorId=iter->first;
  }

  module=detectorId/100000000;

  return module;
};
*/
typedef std::pair<Int_t, Int_t> map_ele;
struct Ascend {
  bool operator()(const map_ele &a, const map_ele &b)
  {
    // std::cout<<"a = "<<a.second<<", b = "<<b.second<<std::endl;
    return a.second > b.second;
  }
};

const std::vector<Int_t> &PndEmcCluster::GetMcList() const
{
  std::vector<map_ele> sortedVec;
  std::vector<FairLink> mcLinks;
  FairMultiLinkedData mcFairLinks = GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack"));

  for (int i = 0; i < mcFairLinks.GetNLinks(); i++) {
    mcLinks.push_back(mcFairLinks.GetLink(i));
  }

  std::sort(mcLinks.begin(), mcLinks.end(), [](const FairLink &a, const FairLink &b) -> bool { return a < b; });

  fMcList.clear();
  for (auto link : mcLinks)
    fMcList.push_back(link.GetIndex());
  return fMcList;
}

void PndEmcCluster::invalidateCache(bool kState)
{
  fEnergyValid = kState;
  fWhereValid = kState;
}

Int_t PndEmcCluster::NumberOfDigis() const
{
  return fDigiList.size();
}

Int_t PndEmcCluster::NBumps() const
{
  return fNbumps;
}

void PndEmcCluster::SetNBumps(unsigned nbumps)
{
  fNbumps = nbumps;
}

void PndEmcCluster::Print(const Option_t *) const
{
  std::cout << "*********************************" << endl;
  std::cout << "total energy of cluster: " << GetEnergy() << endl;
  std::cout << "TrackEntering: " << fTrackEntering << std::endl;
  std::cout << "TrackExiting: " << fTrackExiting << std::endl;
}

Double_t PndEmcCluster::DistanceToCentre(const TVector3 &aPoint) const
{
  return (where() - aPoint).Mag();
}

Double_t PndEmcCluster::DistanceToCentre(const PndEmcDigi *aDigi) const
{
  return (where() - aDigi->where()).Mag();
}

Double_t PndEmcCluster::GetEnergyCorrected() const
{
  Double_t e = this->GetEnergy();
  TVector3 clusterPosition = this->where();
  Double_t theta_cluster = clusterPosition.Theta();

  Double_t e1 = e;
  Double_t theta1 = theta_cluster;

  if ((clusterPosition.Z() < 180.0) && (theta_cluster < 140. * TMath::Pi() / 180.)) {
    if (e < 0.03)
      e1 = 0.03;
    if (e > 8.0)
      e1 = 8.0;
  }

  if ((clusterPosition.Z() < 180.0) && (theta_cluster > 140. * TMath::Pi() / 180.)) {
    if (e < 0.03)
      e1 = 0.03;
    if (e > 2.0)
      e1 = 2.0;
  }

  if (clusterPosition.Z() > 180.0) {
    if (e < 0.01)
      e1 = 0.01;
    if (e > 16.0)
      e1 = 16.0;
  }

  double b0 = 1.45312;
  double b1 = 2.79086e-02;
  double b2 = 3.91932e-04;
  double b3 = -1.23117e-03;
  double b4 = 2.72270e-01;
  double b5 = -1.31540;
  double b6 = 1.44447;
  double b7 = -4.05724e-01;
  double b8 = -2.07396;
  double b9 = 4.80507e-02;

  double p0 = 0;
  double p1 = 0;
  double p2 = 0;
  double p3 = 0;
  double p4 = 0;
  double p5 = 0;
  double p6 = 0;
  double p7 = 0;
  double p8 = 0;
  double p9 = 0;

  if (e1 < 1.0) {
    p0 = 4.13189e-02;
    p1 = -2.03834e-02;
    p2 = -2.58086e-03;
    p3 = -1.77821e-03;
    p4 = -1.73738e-02;
    p5 = 7.40362e-02;
    p6 = -6.41892e-02;
    p7 = -9.85564e-02;
    p8 = 1.50123e-01;
    p9 = -7.87742e-04;
  } else {
    p0 = 5.05003e-02;
    p1 = -3.47672e-02;
    p2 = 3.72767e-02;
    p3 = -1.26492e-02;
    p4 = -2.16876e-02;
    p5 = 1.02682e-01;
    p6 = -9.85242e-02;
    p7 = -1.39872e-01;
    p8 = 2.02309e-01;
    p9 = 1.11696e-03;
  }

  double t0 = 1.81631;
  double t1 = -1.71202e-02;
  double t2 = 3.59161e-03;
  double t3 = -3.46712e-04;
  double t4 = -3.73691e-01;
  double t5 = -1.56688;
  double t6 = -1.62618;
  double t7 = -4.10972e-01;
  double t8 = 2.2222;
  double t9 = 4.60908e-03;

  double factor1 = p0 + p1 * log(e1) + p2 * log(e1) * log(e1) + p3 * log(e1) * log(e1) * log(e1) + p4 * cos(theta1) + p5 * cos(theta1) * cos(theta1) +
                   p6 * cos(theta1) * cos(theta1) * cos(theta1) + p7 * cos(theta1) * cos(theta1) * cos(theta1) * cos(theta1) +
                   p8 * cos(theta1) * cos(theta1) * cos(theta1) * cos(theta1) * cos(theta1) + p9 * log(e1) * cos(theta1);

  double factor2 = t0 + t1 * log(e1) + t2 * log(e1) * log(e1) + t3 * log(e1) * log(e1) * log(e1) + t4 * cos(theta1) + t5 * cos(theta1) * cos(theta1) +
                   t6 * cos(theta1) * cos(theta1) * cos(theta1) + t7 * cos(theta1) * cos(theta1) * cos(theta1) * cos(theta1) +
                   t8 * cos(theta1) * cos(theta1) * cos(theta1) * cos(theta1) * cos(theta1) + t9 * log(e1) * cos(theta1);

  double factor3 = b0 + b1 * log(e1) + b2 * log(e1) * log(e1) + b3 * log(e1) * log(e1) * log(e1) + b4 * cos(theta1) + b5 * cos(theta1) * cos(theta1) +
                   b6 * cos(theta1) * cos(theta1) * cos(theta1) + b7 * cos(theta1) * cos(theta1) * cos(theta1) * cos(theta1) +
                   b8 * cos(theta1) * cos(theta1) * cos(theta1) * cos(theta1) * cos(theta1) + b9 * log(e1) * cos(theta1);

  double eout1 = e * exp(factor1);
  double eout2 = e * exp(factor2);
  double eout3 = e * exp(factor3);

  double eout4 = (3.31694 - 0.0183379 / sqrt(e1) + 0.0327113 / e1 + 0.00040156 / (e1 * e1) - 0.00641305 / (e1 * sqrt(e1))) * e / 3.0144;

  if (clusterPosition.Z() > 500.0)
    return eout4;
  else if ((clusterPosition.Z() < 180.0) && (theta_cluster > 140. * TMath::Pi() / 180.))
    return eout3;
  else if ((clusterPosition.Z() < 180.0) && (theta_cluster < 140. * TMath::Pi() / 180.))
    return eout1;
  else
    return eout2;
}

void PndEmcCluster::AddTracksEnteringExiting(const FairMultiLinkedData &tracksEntering, const FairMultiLinkedData &tracksExiting)
{
  std::map<FairLink, LinkScoreBoard> scoreBoard;
  std::set<FairLink> entering, exiting;

  //	std::cout << "tracksEntering " << tracksEntering << std::endl;
  //	std::cout << "tracksExiting " << tracksExiting << std::endl;
  //	std::cout << "exitingEntering " << fTrackEntering << std::endl;
  //	std::cout << "existingExiting " << fTrackExiting << std::endl;

  FillScoreBoard(tracksEntering, scoreBoard, 3);
  FillScoreBoard(tracksExiting, scoreBoard, 2);
  FillScoreBoard(fTrackEntering, scoreBoard, 1);
  FillScoreBoard(fTrackExiting, scoreBoard, 0);

  for (std::map<FairLink, LinkScoreBoard>::iterator iter = scoreBoard.begin(); iter != scoreBoard.end(); iter++) {
    // std::cout << iter->first << " " << iter->second.score << std::endl;
    switch (iter->second.score) {
    case 15:
      entering.insert(iter->first);
      exiting.insert(iter->first);
      break;
    case 14: entering.insert(iter->first); break;
    case 13: exiting.insert(iter->first); break;
    case 12:
      entering.insert(iter->first);
      exiting.insert(iter->first);
      break;
    case 11: entering.insert(iter->first); break;
    case 10: std::cout << "-E- PndEmcCluster::AddTracksEnteringExiting Same particle entering twice!" << std::endl; break;
    case 9: break;
    case 8: entering.insert(iter->first); break;
    case 7: exiting.insert(iter->first); break;
    case 6: break;
    case 5: std::cout << "-E- PndEmcCluster::AddTracksEnteringExiting Same particle exiting twice!" << std::endl; break;
    case 4: exiting.insert(iter->first); break;
    case 3:
      entering.insert(iter->first);
      exiting.insert(iter->first);
      break;
    case 2: entering.insert(iter->first); break;
    case 1: exiting.insert(iter->first); break;
    case 0: break;
    default: std::cout << "-E- PndEmcCluster::AddTracksEnteringExiting wrong score " << iter->second.score << std::endl; break;
    }
  }
  fTrackEntering.SetLinks(entering);
  fTrackExiting.SetLinks(exiting);

  //	std::cout << "Entering after merge " << fTrackEntering << std::endl;
  //	std::cout << "Exiting after merge " << fTrackExiting << std::endl;
}

void PndEmcCluster::FillScoreBoard(FairMultiLinkedData tracks, std::map<FairLink, LinkScoreBoard> &scoreBoard, Int_t shift)
{
  std::set<FairLink> links = tracks.GetLinks();
  for (std::set<FairLink>::iterator linkIter = links.begin(); linkIter != links.end(); linkIter++) {
    scoreBoard[*linkIter].SetValShift(kTRUE, shift);
  }
}

// Return Lorentz vector of this cluster, assuming the starting point to be in the IP
TLorentzVector PndEmcCluster::GetLorentzVector()
{
  double clusEnergy = GetEnergy();
  double clusDir = TMath::Sqrt(x() * x() + y() * y() + z() * z());
  fLorentzVector.SetPxPyPzE(x() * clusEnergy / clusDir, y() * clusEnergy / clusDir, z() * clusEnergy / clusDir, clusEnergy);
  return fLorentzVector;
}

// Update indices of digis in fDigiArray (EXAMPLE USE: call after putting member digis in a new array e.g. using FairLinks) //
void PndEmcCluster::OverwriteDigiList(Int_t start, Int_t end)
{
  fDigiList.clear();
  for (Int_t i = start; i < end; i++) {
    fDigiList.push_back(i);
  }
}

ClassImp(PndEmcCluster)
