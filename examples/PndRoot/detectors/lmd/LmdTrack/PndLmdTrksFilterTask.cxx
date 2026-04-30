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

#include "PndLmdTrksFilterTask.h"

#include "PndTrack.h"
#include "PndSdsHit.h"
#include "PndSdsMCPoint.h"
#include "PndSdsMergedHit.h"
#include "PndLmdGeometryHelper.h"

#include <vector>

#include "FairLogger.h"
#include "FairHit.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TROOT.h"

PndLmdTrksFilterTask::PndLmdTrksFilterTask(double LabMomentum, bool UseKinematicsFilter, double IPXMean, double IPYMean)
  : FairTask("Tracks filtering Task for PANDA Lmd"), verboseLevel(0), fTrkCandArray(nullptr), fTrkArray(nullptr), fTrkOutArray(nullptr), fEventNr(0),
    fUseKinematicsFilter(UseKinematicsFilter), fIPXMean(IPXMean), fIPYMean(IPYMean)
{
  fTrkCandName = "LMDTrackCand";
  fTrkName = "LMDPndTrack";
  fTrkOutName = "LMDPndTrackFilt";

  RadialPhiDiffBefore = new TH1D("RadialPhiDiffBefore", "", 1000, -6, 6);
  RadialPhiDiffAfter = new TH1D("RadialPhiDiffAfter", "", 1000, -6, 6);
  RadialMagnitudeRatioBefore = new TH1D("RadialMagnitudeRatioBefore", "", 1000, 0, 6);
  RadialMagnitudeRatioAfter = new TH1D("RadialMagnitudeRatioAfter", "", 1000, 0, 6);

  RadialPhiDiffMagnitudeRatioBefore = new TH2D("RadialPhiDiffMagnitudeRatioBefore", "", 1000, -6, 6, 1000, 0, 6);
  RadialPhiDiffMagnitudeRatioAfter = new TH2D("RadialPhiDiffMagnitudeRatioAfter", "", 1000, -6, 6, 1000, 0, 6);

  // mean and sigma are determined from parameterization (simulations)
  PhiDiffMean = 0.0;                                   // mean is fixed for all energies
  PhiDiffSigma = 0.152 / LabMomentum + 0.0056;         // a/p_lab+c parameterization
  MagnitudeRatioMean = 0.91;                           // mean is fixed for all energies
  MagnitudeRatioSigma = 0.1336 / LabMomentum + 0.0058; // a/p_lab+c parameterization
}

InitStatus PndLmdTrksFilterTask::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (ioman == 0) {
    Error("PndLmdTrksFilterTask::Init", "RootManager not instantiated!");
    return kERROR;
  }

  fTrkArray = (TClonesArray *)ioman->GetObject(fTrkName);
  if (fTrkArray == nullptr) {
    Error("PndLmdTrksFilterTask::Init", "track-array not found!");
    return kERROR;
  }

  fTrkCandArray = (TClonesArray *)ioman->GetObject(fTrkCandName);
  if (fTrkCandArray == nullptr) {
    Error("PndLmdTrksFilterTask::Init", "trk-cand--array not found!");
    return kERROR;
  }

  fTrkOutArray = new TClonesArray("PndTrack");
  ioman->Register("LMDPndTrackFilt", "PndLmd", fTrkOutArray, kTRUE);

  return kSUCCESS;
}

void PndLmdTrksFilterTask::Exec(Option_t *)
{
  fTrkOutArray->Delete();

  // go through all tracks
  const unsigned int TrackCount = fTrkArray->GetEntriesFast();
  std::vector<unsigned int> AcceptedTrackIDs;

  PndLmdGeometryHelper &LmdGeoHelper = PndLmdGeometryHelper::getInstance();

  // first filter out bad tracks
  if (fUseKinematicsFilter) {
    for (unsigned int i = 0; i < TrackCount; i++) {
      PndTrack *trkpnd = (PndTrack *)(fTrkArray->At(i));
      // check trk kinematics -----
      FairTrackParP fFittedTrkP = trkpnd->GetParamFirst();

      TVector3 PosRecLMD(LmdGeoHelper.transformPndGlobalToLmdLocal(fFittedTrkP.GetPosition()));
      TVector3 DirectionLMD(fFittedTrkP.GetPx(), fFittedTrkP.GetPy(), fFittedTrkP.GetPz());
      double plab(DirectionLMD.Mag());
      DirectionLMD = DirectionLMD.Unit();
      TVector3 EndMomRecLMD = LmdGeoHelper.transformPndGlobalToLmdLocal(fFittedTrkP.GetPosition() + DirectionLMD);
      DirectionLMD = EndMomRecLMD - PosRecLMD;

      TVector3 RadialPositionDirection(PosRecLMD.X() - fIPXMean, PosRecLMD.Y() - fIPYMean, 0.0);
      TVector3 RadialMomentum(1000.0 * DirectionLMD.Px(), 1000.0 * DirectionLMD.Py(), 0.0);

      double phidiff(RadialPositionDirection.Phi() - RadialMomentum.Phi());
      // remove multiples of 2pi
      while (phidiff > TMath::Pi()) {
        phidiff -= 2.0 * TMath::Pi();
      }
      while (phidiff < -TMath::Pi()) {
        phidiff += 2.0 * TMath::Pi();
      }
      double magratio(RadialMomentum.Mag() / RadialPositionDirection.Mag());
      RadialPhiDiffBefore->Fill(phidiff);
      RadialMagnitudeRatioBefore->Fill(magratio);
      RadialPhiDiffMagnitudeRatioBefore->Fill(phidiff, magratio);

      // we use a 3 sigma cut around the mean
      if (std::abs(phidiff - PhiDiffMean) < 3.0 * PhiDiffSigma && std::abs(magratio - MagnitudeRatioMean) < 3.0 * MagnitudeRatioSigma) {
        RadialPhiDiffAfter->Fill(phidiff);
        RadialMagnitudeRatioAfter->Fill(magratio);
        RadialPhiDiffMagnitudeRatioAfter->Fill(phidiff, magratio);
        AcceptedTrackIDs.push_back(i);
      }
    }
  } else {
    for (unsigned int i = 0; i < TrackCount; i++) {
      AcceptedTrackIDs.push_back(i);
    }
  }

  // compare remaining tracks on hit level
  std::vector<unsigned int> RemoveTrackIDs;
  for (unsigned int i = 0; i < AcceptedTrackIDs.size(); ++i) {
    if (RemoveTrackIDs.end() != std::find(RemoveTrackIDs.begin(), RemoveTrackIDs.end(), AcceptedTrackIDs[i]))
      continue;
    for (unsigned int j = i + 1; j < AcceptedTrackIDs.size(); ++j) {
      if (RemoveTrackIDs.end() != std::find(RemoveTrackIDs.begin(), RemoveTrackIDs.end(), AcceptedTrackIDs[j]))
        continue;
      auto hitseq1 = getHitIDSequence(AcceptedTrackIDs[i]);
      auto hitseq2 = getHitIDSequence(AcceptedTrackIDs[j]);

      unsigned int NumberOfIdenticalHits = getNumberOfIdenticalHits(hitseq1, hitseq2);

      if (NumberOfIdenticalHits == 4) {
        // this should never happen, but if so just remove the current track
        RemoveTrackIDs.push_back(AcceptedTrackIDs[i]);
        break;
      } else if (NumberOfIdenticalHits >= 2 || (NumberOfIdenticalHits == 1 && hitseq1.size() == 3 && hitseq2.size() == 3)) {
        // pick track with higher hit count
        if (hitseq1.size() > hitseq2.size()) {
          RemoveTrackIDs.push_back(AcceptedTrackIDs[j]);
        } else if (hitseq2.size() > hitseq1.size()) {
          RemoveTrackIDs.push_back(AcceptedTrackIDs[i]);
          break;
        } else {
          // if they are equal, pick the track with the better (smaller) chi2
          PndTrack *trkpnd1 = (PndTrack *)(fTrkArray->At(AcceptedTrackIDs[i]));
          double chi21 = trkpnd1->GetChi2();
          PndTrack *trkpnd2 = (PndTrack *)(fTrkArray->At(AcceptedTrackIDs[j]));
          double chi22 = trkpnd2->GetChi2();
          if (chi21 > chi22) {
            RemoveTrackIDs.push_back(AcceptedTrackIDs[j]);
          } else {
            RemoveTrackIDs.push_back(AcceptedTrackIDs[i]);
            break;
          }
        }
      }
    }
  }
  // if (RemoveTrackIDs.size() > 0) LOG(info) << "Removing " << RemoveTrackIDs.size() << " of "
  //    << AcceptedTrackIDs.size() << " tracks because of hit similarity!";

  // save good trks
  int rec_trk = 0;
  for (unsigned int i : AcceptedTrackIDs) {
    if (RemoveTrackIDs.end() != std::find(RemoveTrackIDs.begin(), RemoveTrackIDs.end(), i)) {
      continue;
    }
    PndTrack *trkpnd = (PndTrack *)(fTrkArray->At(i));

    new ((*fTrkOutArray)[rec_trk]) PndTrack(*(trkpnd)); // save Track
    rec_trk++;
  }
  if (fVerbose > 2)
    LOG(info) << "Ev#" << fEventNr << ": " << rec_trk << " trks saved out of " << TrackCount;
  fEventNr++;
}

std::vector<int> PndLmdTrksFilterTask::getHitIDSequence(unsigned int TrackID) const
{
  std::vector<int> HitIDs;

  PndTrack *trkpnd = (PndTrack *)(fTrkArray->At(TrackID));

  int candID = trkpnd->GetRefIndex();
  PndTrackCand *trkcand = (PndTrackCand *)fTrkCandArray->At(candID);

  for (Int_t iHit = 0; iHit < trkcand->GetNHits(); iHit++) {
    PndTrackCandHit candhit = (PndTrackCandHit)(trkcand->GetSortedHit(iHit));
    Int_t hitID = candhit.GetHitId();
    HitIDs.push_back(hitID);
  }
  return HitIDs;
}

unsigned int PndLmdTrksFilterTask::getNumberOfIdenticalHits(const std::vector<int> &seq1, const std::vector<int> &seq2) const
{
  size_t Counter(0);
  for (auto x : seq1) {
    if (std::find(seq2.begin(), seq2.end(), x) != seq2.end())
      ++Counter;
  }
  return Counter;
}

void PndLmdTrksFilterTask::Finish()
{
  LOG(info) << "saving histograms to file...";
  RadialPhiDiffBefore->Write();
  RadialMagnitudeRatioBefore->Write();
  RadialPhiDiffAfter->Write();
  RadialMagnitudeRatioAfter->Write();
  RadialPhiDiffMagnitudeRatioBefore->Write();
  RadialPhiDiffMagnitudeRatioAfter->Write();
}

ClassImp(PndLmdTrksFilterTask);
