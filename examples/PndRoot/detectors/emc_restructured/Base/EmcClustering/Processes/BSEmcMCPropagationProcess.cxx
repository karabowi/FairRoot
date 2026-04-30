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

#include "BSEmcMCPropagationProcess.h"

#include <memory>
#include <set>
#include <utility>

#include "TObject.h"

#include "FairLink.h"
#include "FairMultiLinkedData.h"
#include "FairRootManager.h"
#include "fairlogger/Logger.h"

#include "PndContainerI.h"
#include "PndMutableContainerI.h"
#include "PndProcess.h"

#include "BSEmcCluster.h"

#include "BSEmcDataBranchNames.h"
#include "BSEmcDigi.h"
#include "BSEmcMCDeposit.h"
#include "BSEmcSubCluster.h"

#include "PndParameterRegister.h"

BSEmcMCPropagationProcess::BSEmcMCPropagationProcess() : PndProcess{"BSEmcMCPropagationProcess"} {}

BSEmcMCPropagationProcess::~BSEmcMCPropagationProcess() {}

void BSEmcMCPropagationProcess::SetDetectorName(const std::string &t_detectorName)
{
  fDetectorName = t_detectorName;
}

void BSEmcMCPropagationProcess::SetupParameters(const PndParameterRegister * /*t_paramRegister*/)
{
  fMCDepositBranchname = BSEmcDataBranchNames::fgMCDepositBranchName + fDetectorName;
  fDigiBranchname = BSEmcDataBranchNames::fgDigiBranchName + fDetectorName;
  fClusterBranchname = BSEmcDataBranchNames::fgClusterBranchName + fDetectorName;
  fSubClusterBranchname = BSEmcDataBranchNames::fgSubClusterBranchName + fDetectorName;
}

void BSEmcMCPropagationProcess::RequestDataContainer(PndContainerRegister *t_register)
{
  t_register->Request({fMCDepositBranchname, "BSEmcMCDeposit", kFALSE, kFALSE});
  t_register->Request({fDigiBranchname, "BSEmcDigi", kFALSE, kFALSE});
  t_register->Request({fClusterBranchname, "BSEmcCluster", kTRUE, kTRUE});
  t_register->Request({fSubClusterBranchname, "BSEmcSubCluster", kTRUE, kTRUE});
}

void BSEmcMCPropagationProcess::GetDataContainer(PndContainerRegister *t_register)
{
  fMCDepositArray = t_register->GetInput<BSEmcMCDeposit>(fMCDepositBranchname);
  fDigiArray = t_register->GetInput<BSEmcDigi>(fDigiBranchname);
  fClusterArray = t_register->GetOutput<BSEmcCluster>(fClusterBranchname);
  fSubClusterArray = t_register->GetOutput<BSEmcSubCluster>(fSubClusterBranchname);

  fMCDepositBranchname = fMCDepositArray->GetBranchName();
  fDigiBranchname = fDigiArray->GetBranchName();
  fClusterBranchname = fClusterArray->GetBranchName();
  fSubClusterBranchname = fSubClusterArray->GetBranchName();
}

void BSEmcMCPropagationProcess::Process()
{
  std::vector<BSEmcCluster *> cluster = fClusterArray->GetVector();
  std::vector<BSEmcSubCluster *> subcluster = fSubClusterArray->GetVector();
  std::vector<const BSEmcDigi *> digis = fDigiArray->GetVectorOfPtrToConst();
  LOG(debug) << "BSEmcMCPropagationProcess::AssignLinks to CLUSTER";
  AssignLinks(digis, cluster);
  LOG(debug) << "BSEmcMCPropagationProcess::AssignLinks to SUBCLUSTER";
  AssignLinks(digis, subcluster);
}

template <class T>
void BSEmcMCPropagationProcess::AssignLinks(const std::vector<const BSEmcDigi *> &t_digiCont, const std::vector<T *> &t_array)
{
  Int_t nClusters = t_array.size();
  LOG(debug) << "BSEmcMCPropagationProcess::Exec " << nClusters << " to be processed";

  for (Int_t clusterIdx = 0; clusterIdx < nClusters; ++clusterIdx) {
    BSEmcCluster *cluster = t_array.at(clusterIdx);
    LOG(debug) << "BSEmcMCPropagationProcess::Exec "
               << "Cluster " << clusterIdx << " contains " << cluster->GetDigis().size() << " digis.";
    ResetLinksBesideDepositLinks(cluster);

    for (const BSEmcDigiInfo_t &digiInfo : cluster->GetDigis()) {
      const BSEmcDigi *digi = t_digiCont.at(digiInfo.fDigiIdx);
      if (digi != nullptr) {
        FairMultiLinkedData depositLinks = digi->GetLinksWithType(FairRootManager::Instance()->GetBranchId(fMCDepositBranchname));
        LOG(DEBUG1) << "BSEmcMCPropagationProcess::Exec Digi " << digiInfo.fDigiIdx << " has " << depositLinks.GetNLinks() << " depositLinks for branch " << fMCDepositBranchname
                    << " and has " << digi->GetLinks().size() << " links in total.";

        for (Int_t j = 0; j < depositLinks.GetNLinks(); j++) {
          // std::unique_ptr<BSEmcMCDeposit> deposit =
          // std::unique_ptr<BSEmcMCDeposit>(dynamic_cast<BSEmcMCDeposit
          // *>(FairRootManager::Instance()->GetCloneOfLinkData(depositLinks.GetLink(j))));
          const BSEmcMCDeposit *deposit = fMCDepositArray->GetConstElementPtr(depositLinks.GetLink(j).GetIndex());
          if (deposit != nullptr) {
            if (cluster->GetLinks().count(depositLinks.GetLink(j)) == 0) {
              AddTracksEnteringExiting(cluster, deposit->GetTrackEntering(), deposit->GetTrackExiting());
              cluster->AddLink(depositLinks.GetLink(j));
              LOG(trace) << "BSEmcMCPropagationProcess::Exec Added " << depositLinks.GetLink(j) << "to cluster " << clusterIdx;
            } else {
              LOG(trace) << "BSEmcMCPropagationProcess::Exec cluster->GetLinks().count(depositLinks.GetLink(j)) != 0";
            }
          } else {
            LOG(error) << "BSEmcMCPropagationProcess::Exec - " << this->fMCDepositBranchname << "- FairLink " << depositLinks.GetLink(j) << "to BSEmcMCDeposit delivers null";
          }
        }
      } else {
        LOG(error) << "BSEmcMCPropagationProcess::Exec "
                   << "digi is nullptr!";
      }
    }

    FairMultiLinkedData tdepositLinks = cluster->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack"));
    LOG(trace) << "subcluster->GetLinksWithType(MCTrack) = " << tdepositLinks.GetNLinks();
  }
}

void BSEmcMCPropagationProcess::ResetLinksBesideDepositLinks(BSEmcCluster *t_cluster)
{
  FairMultiLinkedData clusterdigiLinks = t_cluster->GetLinksWithType(FairRootManager::Instance()->GetBranchId(fDigiBranchname));
  FairMultiLinkedData clusterclusterLinks = t_cluster->GetLinksWithType(FairRootManager::Instance()->GetBranchId(fClusterBranchname));
  t_cluster->ResetLinks();
  t_cluster->SetInsertHistory(kFALSE);
  for (const FairLink &link : clusterdigiLinks.GetLinks()) {
    t_cluster->AddLink(link);
  }
  for (const FairLink &link : clusterclusterLinks.GetLinks()) {
    t_cluster->AddLink(link);
  }
  FairMultiLinkedData tdepositLinks = t_cluster->GetLinksWithType(FairRootManager::Instance()->GetBranchId(fMCDepositBranchname));
  LOG(trace) << "subcluster->GetLinksWithType(fMCDepositBranchname) = " << tdepositLinks.GetNLinks();
  FairMultiLinkedData digiLinks = t_cluster->GetLinksWithType(FairRootManager::Instance()->GetBranchId(fDigiBranchname));
  LOG(trace) << "subcluster->GetLinksWithType(fDigiBranchname) = " << digiLinks.GetNLinks();
  FairMultiLinkedData clusterLinks = t_cluster->GetLinksWithType(FairRootManager::Instance()->GetBranchId(fClusterBranchname));
  LOG(trace) << "subcluster->GetLinksWithType(fClusterBranchname) = " << clusterLinks.GetNLinks();
  FairMultiLinkedData subclusterLinks = t_cluster->GetLinksWithType(FairRootManager::Instance()->GetBranchId(fSubClusterBranchname));
  LOG(trace) << "subcluster->GetLinksWithType(fSubClusterBranchname) = " << subclusterLinks.GetNLinks();
}

/**
 * @brief Updates the links to entering and exiting tracks
 *
 * For each track a bitmask is created which contains the information if the track enters or exits
 * the cluster (bit 1 and 0 for tracks already in the cluster, bit 3 and 2 for the ones added here).
 * Afterwards, the bitmask is checked for each track and the lists of entering and exiting tracks is
 * updated. Tracks which entered before and exit now or vice versa (bitmask=6 or 9) are removed
 * from the list as they only cross between crystals in the same cluster.
 *
 * @param tracksEntering Additional tracks entering the cluster (e.g. from adding digi or merging clusters)
 * @param tracksExiting Additional tracks exiting the cluster
 * @return void
 */
void BSEmcMCPropagationProcess::AddTracksEnteringExiting(BSEmcCluster *t_cluster, const FairMultiLinkedData &t_tracksEntering, const FairMultiLinkedData &t_tracksExiting)
{
  std::map<FairLink, LinkScoreBoard> scoreBoard;
  std::set<FairLink> entering, exiting;
  FairMultiLinkedData trackEntering = t_cluster->GetTrackEntering();
  FairMultiLinkedData trackExiting = t_cluster->GetTrackExiting();

  FillScoreBoard(t_tracksEntering, scoreBoard, 3);
  FillScoreBoard(t_tracksExiting, scoreBoard, 2);
  FillScoreBoard(trackEntering, scoreBoard, 1);
  FillScoreBoard(trackExiting, scoreBoard, 0);

  for (auto &iter : scoreBoard) {
    // std::cout << iter->first << " " << iter->second.score;
    t_cluster->AddLink(FairLink{"MCTrack", iter.first.GetIndex()});
    switch (iter.second.score) {
    case 15:
      entering.insert(iter.first);
      exiting.insert(iter.first);
      break;
    case 14: entering.insert(iter.first); break;
    case 13: exiting.insert(iter.first); break;
    case 12:
      entering.insert(iter.first);
      exiting.insert(iter.first);
      break;
    case 11: entering.insert(iter.first); break;
    case 10: LOG(error) << " BSEmcMCPropagationProcess::AddTracksEnteringExiting Same particle entering twice!"; break;
    case 9: break;
    case 8: entering.insert(iter.first); break;
    case 7: exiting.insert(iter.first); break;
    case 6: break;
    case 5: LOG(error) << " BSEmcMCPropagationProcess::AddTracksEnteringExiting Same particle exiting twice!"; break;
    case 4: exiting.insert(iter.first); break;
    case 3:
      entering.insert(iter.first);
      exiting.insert(iter.first);
      break;
    case 2: entering.insert(iter.first); break;
    case 1: exiting.insert(iter.first); break;
    case 0: break;
    default: LOG(error) << " BSEmcMCPropagationProcess::AddTracksEnteringExiting wrong score " << iter.second.score; break;
    }
  }
  trackEntering.SetLinks(entering);
  trackExiting.SetLinks(exiting);
  t_cluster->SetTrackEntering(trackEntering);
  t_cluster->SetTrackExiting(trackExiting);
}

void BSEmcMCPropagationProcess::FillScoreBoard(FairMultiLinkedData t_tracks, std::map<FairLink, LinkScoreBoard> &t_scoreBoard, Int_t t_shift)
{
  std::set<FairLink> links = t_tracks.GetLinks();
  for (const auto &link : links) {
    t_scoreBoard[link].SetValShift(kTRUE, t_shift);
  }
}
