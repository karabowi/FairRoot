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

#include "PndMvdRiemannTrackFinderTaskEff.h"

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <vector>

// Root includes
#include "TROOT.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"
#include "TFile.h"
#include "TVector.h"

// framework includes
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "PndMCTrack.h"
#include "FairLogger.h"

// PndMvd includes
#include "PndTrackCand.h"
#include "PndSdsHit.h"
#include "PndMCTrack.h"
#include "PndDetectorList.h"

#include "PndRiemannHit.h"
#include "PndRiemannTrack.h"

PndMvdRiemannTrackFinderTaskEff::PndMvdRiemannTrackFinderTaskEff()
  : FairTask("MVD Riemann Track Finder"), eff0H(nullptr), effH(nullptr), GhH(nullptr), fHitBranch("MVDHitsPixel"), fHitBranch2("MVDHitsStrip"), fMCTrackBranch("MCTrack"),
    fIdealTrackBranch("MVDIdealTrackCand"), fFTrackBranch("MVDRiemannTrackCand"), fEventNr(0), fMaxSZChi2(1), fMaxSZDist(10), fMinPointDist(1), fMaxDist(1), fHitArray(nullptr),
    fHitArray2(nullptr), fTrackCandArray(nullptr), fIdealTrackCandArray(nullptr), fMCTracksArray(nullptr)
{
}

PndMvdRiemannTrackFinderTaskEff::~PndMvdRiemannTrackFinderTaskEff() {}

void PndMvdRiemannTrackFinderTaskEff::SetParContainers() {}

InitStatus PndMvdRiemannTrackFinderTaskEff::ReInit()
{
  InitStatus stat = kSUCCESS;
  return stat;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdRiemannTrackFinderTaskEff::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    std::cout << "-E- PndMvdRiemannTrackFinderTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input array
  fHitArray = (TClonesArray *)ioman->GetObject(fHitBranch);
  if (!fHitArray) {
    std::cout << "-W- PndMvdRiemannTrackFinderTask::Init: "
              << "No hitArray!" << std::endl;
    return kERROR;
  }

  fHitArray2 = (TClonesArray *)ioman->GetObject(fHitBranch2);
  if (!fHitArray2) {
    std::cout << "-W- PndMvdRiemannTrackFinderTask::Init: "
              << "No hitArray2!" << std::endl;
    return kERROR;
  }

  fMCTracksArray = (TClonesArray *)ioman->GetObject(fMCTrackBranch);
  if (!fMCTracksArray) {
    std::cout << "-W- PndMvdRiemannTrackFinderTask::Init: "
              << "No MCtracks!" << std::endl;
    return kERROR;
  }

  fIdealTrackCandArray = (TClonesArray *)ioman->GetObject(fIdealTrackBranch);
  if (!fIdealTrackCandArray) {
    std::cout << "-W- PndMvdRiemannTrackFinderTask::Init: "
              << "No IdealTrackCands!" << std::endl;
    return kERROR;
  }
  fTrackCandArray = (TClonesArray *)ioman->GetObject(fFTrackBranch);
  if (!fTrackCandArray) {
    std::cout << "-W- PndMvdRiemannTrackFinderTask::Init: "
              << "No FTrackCands!" << std::endl;
    return kERROR;
  }

  int Nbin = 10;
  eff0H = new TH2F("eff0H", "eff0H", Nbin, 0.1, 1, Nbin, 15, 150);
  effH = new TH2F("effH", "effH", Nbin, 0.1, 1, Nbin, 15, 150);

  GhH = new TH2F("GhH", "GhH", Nbin, 0.1, 1, Nbin, 15, 150);

  LOG(info) << " PndMvdRiemannTrackFinderTask: Initialisation successfull";
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndMvdRiemannTrackFinderTaskEff::Exec(Option_t *)
{

  std::vector<PndTrackCand *> RecoT;

  for (int i = 0; i < fIdealTrackCandArray->GetEntriesFast(); i++) {
    PndTrackCand *cand = (PndTrackCand *)fIdealTrackCandArray->At(i);
    PndMCTrack *myTrack = (PndMCTrack *)fMCTracksArray->At(cand->getMcTrackId());
    if (CheckRecoTrack(cand, myTrack)) {
      if (myTrack->GetMotherID() == -1)
        cand->AddHit(0, 0, 0);
      RecoT.push_back(cand);
      TVector3 Pvec = myTrack->GetMomentum();
      double Theta = 180.0 * Pvec.Theta() / TMath::Pi();
      double P = Pvec.Mag();
      eff0H->Fill(P, Theta);
    }
  }
  ComparingFandR(RecoT);
  std::cout << "                             Event n: " << fEventNr++ << std::endl;
}

void PndMvdRiemannTrackFinderTaskEff::FinishEvent() {}
bool PndMvdRiemannTrackFinderTaskEff::CheckRecoTrack(PndTrackCand *cand, PndMCTrack *) // myTrack //[R.K.03/2017] unused variable(s)
{
  if (/*myTrack->GetPdgCode()==211 &&*/ (cand->GetNHits() > 2) /* && (myTrack->GetMotherID()==-1)*/) {
    int count = 0;
    unsigned int detIDi, hitIDi;
    unsigned int detIDj, hitIDj;
    for (unsigned int i = 0; i < cand->GetNHits(); i++) {
      detIDi = cand->GetSortedHit(i).GetDetId();
      hitIDi = cand->GetSortedHit(i).GetHitId();
      PndSdsHit *pointI;
      if ((int)detIDi == FairRootManager::Instance()->GetBranchId(fHitBranch))
        pointI = (PndSdsHit *)fHitArray->At(hitIDi);
      else if ((int)detIDi == FairRootManager::Instance()->GetBranchId(fHitBranch2))
        pointI = (PndSdsHit *)fHitArray2->At(hitIDi);
      else
        pointI = 0;

      for (unsigned int j = 0; j < cand->GetNHits(); j++) {
        detIDj = cand->GetSortedHit(j).GetDetId();
        hitIDj = cand->GetSortedHit(j).GetHitId();
        PndSdsHit *pointJ;
        if ((int)detIDj == FairRootManager::Instance()->GetBranchId(fHitBranch))
          pointJ = (PndSdsHit *)fHitArray->At(hitIDj);
        else if ((int)detIDj == FairRootManager::Instance()->GetBranchId(fHitBranch2))
          pointJ = (PndSdsHit *)fHitArray2->At(hitIDj);
        else
          pointJ = 0;

        if ((pointI != 0) && (pointJ != 0) && (i != j)) {
          TVector3 a = pointI->GetPosition() - pointJ->GetPosition();
          if (a.Mag() < fMinPointDist) {
            count++;
          }
        }
      }
    }
    if ((cand->GetNHits() - count / 2) < 3) {
      std::cout << "Less then 3 base points in RecoTrack"
                << "   " << cand->GetNHits() << "   " << count << std::endl;
      return false;
    } else
      return true;
  } else
    return false;
}
void PndMvdRiemannTrackFinderTaskEff::ComparingFandR(std::vector<PndTrackCand *> RecoT)
{
  unsigned int detidRC, hitidRC;
  unsigned int detidF, hitidF;
  for (int trackF = 0; trackF < fTrackCandArray->GetEntriesFast(); trackF++) {
    for (unsigned int trackRC = 0; trackRC < RecoT.size(); trackRC++) {
      int cSame = 0;
      for (unsigned int iF = 0; iF < ((PndTrackCand *)fTrackCandArray->At(trackF))->GetNHits(); iF++) {
        detidF = ((PndTrackCand *)fTrackCandArray->At(trackF))->GetSortedHit(iF).GetDetId();
        hitidF = ((PndTrackCand *)fTrackCandArray->At(trackF))->GetSortedHit(iF).GetHitId();
        for (unsigned int iRC = 0; iRC < RecoT[trackRC]->GetNHits(); iRC++) {
          detidRC = RecoT[trackRC]->GetSortedHit(iRC).GetDetId();
          hitidRC = RecoT[trackRC]->GetSortedHit(iRC).GetHitId();
          if (detidRC == detidF && hitidRC == hitidF) {
            cSame++;
          }
        }
      }

      PndMCTrack *myTrack = (PndMCTrack *)fMCTracksArray->At(RecoT[trackRC]->getMcTrackId());
      if ((cSame >= 4) && ((int)(((PndTrackCand *)fTrackCandArray->At(trackF))->GetNHits()) == cSame)) {
        std::cout << "TRUE FOUND" << std::endl;
        TVector3 Pvec = myTrack->GetMomentum();
        double Theta = 180.0 * Pvec.Theta() / TMath::Pi();
        double P = Pvec.Mag();
        effH->Fill(P, Theta);
        RecoT.erase(RecoT.begin() + trackRC);
        break;
      } else {
        if ((trackRC) == RecoT.size() - 1) {
          std::cout << "        FALSE FOUND" << std::endl;
          AddGhostTrack(trackF);
        }
      }
    }
  }
}

void PndMvdRiemannTrackFinderTaskEff::AddGhostTrack(int trackF)
{
  PndRiemannTrack track;
  unsigned int detID, hitID;
  bool sign = true;
  for (UInt_t i = 0; i < ((PndTrackCand *)fTrackCandArray->At(trackF))->GetNHits(); i++) {
    detID = ((PndTrackCand *)fTrackCandArray->At(trackF))->GetSortedHit(i).GetDetId();
    hitID = ((PndTrackCand *)fTrackCandArray->At(trackF))->GetSortedHit(i).GetHitId();
    PndSdsHit *point;
    if ((int)detID == FairRootManager::Instance()->GetBranchId(fHitBranch))
      point = (PndSdsHit *)fHitArray->At(hitID);
    else if ((int)detID == FairRootManager::Instance()->GetBranchId(fHitBranch2))
      point = (PndSdsHit *)fHitArray2->At(hitID);
    else
      point = 0;
    if (point != 0) {
      PndRiemannHit hit;
      hit.setXYZ(point->GetX(), point->GetY(), point->GetZ());
      hit.setDXYZ(point->GetDx(), point->GetDy(), point->GetDz());
      track.addHit(hit);
    }
    if (i == 1) {
      if (point->GetZ() > 0) {
        sign = true;
      } else
        sign = false;
    }
  }
  track.refit();
  track.szFit();
  Double_t R = track.r();
  Double_t Pt = R * (2 * 3 * 1E8) / (1E9 * 100);
  Double_t dip = track.dip();
  Double_t Theta;
  if (sign)
    Theta = (TMath::ATan(TMath::Power(TMath::Tan(TMath::ACos(dip)), -1))) * 180 / TMath::Pi();
  else
    Theta = (TMath::Pi() - TMath::ATan(TMath::Power(TMath::Tan(TMath::ACos(dip)), -1))) * 180 / TMath::Pi();
  Double_t P = Pt / TMath::Cos(Theta);
  GhH->Fill(P, Theta);
}

ClassImp(PndMvdRiemannTrackFinderTaskEff);
