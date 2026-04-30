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

#include "PndMvdRiemannTrackFinderTaskCutPar.h"

#include <iostream>
// Root includes
#include "TROOT.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"
#include "TVector.h"
#include "TH2F.h"
#include <TFile.h>

// framework includes
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "PndMCTrack.h"
#include "FairLogger.h"

// PndMvd includes
//#include "TrackCand.h"
#include "PndTrackCand.h"
#include "PndRiemannTrack.h"
#include "PndRiemannHit.h"
#include "PndDetectorList.h"

PndMvdRiemannTrackFinderTaskCutPar::PndMvdRiemannTrackFinderTaskCutPar()
  : FairTask("MVD Riemann Track Finder Cuts"), fHitBranch("MVDHitsPixel"), fHitBranch2("MVDHitsStrip"), fMCTrackBranch("MCTrack"), fTrackBranch("MVDIdealTrackCand"), fEventNr(0),
    fMaxSZChi2(1), fMaxSZDist(10), fMinPointDist(1), fMaxDist(1), fHitArray(nullptr), fHitArray2(nullptr), fTrackCandArray(nullptr), fMCTrackArray(nullptr),
    fRiemannTracks(nullptr), fNCut(10), fNbin(100000), frangeDist(2), frangeChi2(1), fPtS(0.1), fPtF(1.0), fThetaS(15), fThetaF(150), fhistsDist(), fhistsChi2(),
    fCutDistH(nullptr), fCutChi2H(nullptr)
{
  ///// creating of Dist and Chi2 histograms
  for (int i = 0; i < fNPt; i++) {
    for (int j = 0; j < fNTh; j++) {
      TH1F *Hist1 = new TH1F("histDist", "h", fNbin, -frangeDist, frangeDist);
      fhistsDist[i][j] = Hist1;
      TH1F *Hist2 = new TH1F("histChi2", "h", fNbin, 0, frangeChi2);
      fhistsChi2[i][j] = Hist2;
    }
  }

  fCutDistH = new TH2F("CutDist", "CutDist", fNPt, fPtS, fPtF, fNTh, fThetaS, fThetaF);
  fCutChi2H = new TH2F("CutChi2", "CutChi2", fNPt, fPtS, fPtF, fNTh, fThetaS, fThetaF);

  fCutDistH->GetXaxis()->SetTitle("Pt, GeV/c");
  fCutDistH->GetYaxis()->SetTitle("Theta");
  fCutChi2H->GetXaxis()->SetTitle("Pt, GeV/c");
  fCutChi2H->GetYaxis()->SetTitle("Theta");
}

PndMvdRiemannTrackFinderTaskCutPar::~PndMvdRiemannTrackFinderTaskCutPar() {}

void PndMvdRiemannTrackFinderTaskCutPar::SetParContainers() {}

InitStatus PndMvdRiemannTrackFinderTaskCutPar::ReInit()
{

  InitStatus stat = kSUCCESS;
  return stat;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdRiemannTrackFinderTaskCutPar::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    std::cout << "-E- PndMvdRiemannTrackFinderTaskCutPar::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input array
  fHitArray = (TClonesArray *)ioman->GetObject(fHitBranch);
  if (!fHitArray) {
    std::cout << "-W- PndMvdRiemannTrackFinderTaskCutPar::Init: "
              << "No hitArray!" << std::endl;
    return kERROR;
  }

  fHitArray2 = (TClonesArray *)ioman->GetObject(fHitBranch2);
  if (!fHitArray2) {
    std::cout << "-W- PndMvdRiemannTrackFinderTaskCutPar::Init: "
              << "No hitArray2!" << std::endl;
    return kERROR;
  }

  fRiemannTracks = new TClonesArray("PndRiemannTrack");

  fTrackCandArray = (TClonesArray *)ioman->GetObject(fTrackBranch);
  if (!fTrackCandArray) {
    std::cout << "-W- PndMvdRiemannTrackFinderTaskCutPar::Init: "
              << "No IdealTrackCands!" << std::endl;
    return kERROR;
  }

  fMCTrackArray = (TClonesArray *)ioman->GetObject(fMCTrackBranch);
  if (!fMCTrackArray) {
    std::cout << "-W- PndMvdRiemannTrackFinderTaskCutPar::Init: "
              << "No MCTrack!" << std::endl;
    return kERROR;
  }

  LOG(info) << " PndMvdRiemannTrackFinderTaskCutPar: Initialisation successfull";
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndMvdRiemannTrackFinderTaskCutPar::Exec(Option_t *)
{

  // Reset output array
  if (!fTrackCandArray) {
    std::cout << "PndMvdRiemannTrackFinderTaskCutPar::Exec no TrackCandArray" << std::endl;
    return;
  }

  fRiemannTracks->Clear();
  CalcRiemannTracks();
  CalcParHists();
  std::cout << "Done event no. " << fEventNr++ << std::endl;
}

void PndMvdRiemannTrackFinderTaskCutPar::CalcRiemannTracks()
{
  for (int j = 0; j < fTrackCandArray->GetEntriesFast(); j++) {
    PndRiemannHit hit[20];
    PndRiemannTrack track;
    PndRiemannTrack track0;
    if (((PndTrackCand *)fTrackCandArray->At(j))->GetNHits() > fNCut) {
      new ((*fRiemannTracks)[j]) PndRiemannTrack(track0);
      continue;
    }
    PndMCTrack *myTrack = (PndMCTrack *)fMCTrackArray->At(((PndTrackCand *)fTrackCandArray->At(j))->getMcTrackId());
    int shift = 0;
    if (myTrack->GetMotherID() == -1) { /// if track vertex has ZeroPos
      hit[0].setXYZ(0, 0, 0);
      hit[0].setDXYZ(0.1, 0.1, 0.1);
      shift = 1;
    }
    int count = shift;
    int i = 0;
    while ((count < 3) && (i < (int)((PndTrackCand *)fTrackCandArray->At(j))->GetNHits())) {
      unsigned int detId, hitId;
      detId = ((PndTrackCand *)fTrackCandArray->At(j))->GetSortedHit(i).GetDetId();
      hitId = ((PndTrackCand *)fTrackCandArray->At(j))->GetSortedHit(i).GetHitId();
      if ((int)detId == FairRootManager::Instance()->GetBranchId(fHitBranch)) {
        hit[count].setXYZ(((PndSdsHit *)fHitArray->At(hitId))->GetPosition().X(), ((PndSdsHit *)fHitArray->At(hitId))->GetPosition().Y(),
                          ((PndSdsHit *)fHitArray->At(hitId))->GetPosition().Z());
        hit[count].setDXYZ(((PndSdsHit *)fHitArray->At(hitId))->GetDx(), ((PndSdsHit *)fHitArray->At(hitId))->GetDy(), ((PndSdsHit *)fHitArray->At(hitId))->GetDz());
        i++;
        count++;
      } else if ((int)detId == FairRootManager::Instance()->GetBranchId(fHitBranch2)) {
        hit[count].setXYZ(((PndSdsHit *)fHitArray2->At(hitId))->GetPosition().X(), ((PndSdsHit *)fHitArray2->At(hitId))->GetPosition().Y(),
                          ((PndSdsHit *)fHitArray2->At(hitId))->GetPosition().Z());
        hit[count].setDXYZ(((PndSdsHit *)fHitArray2->At(hitId))->GetDx(), ((PndSdsHit *)fHitArray2->At(hitId))->GetDy(), ((PndSdsHit *)fHitArray2->At(hitId))->GetDz());
        i++;
        count++;
      } else {
        i++;
        std::cout << "ERROR" << std::endl;
        continue;
      };

      if (i > 1) {
        if (!CheckTooCloseHits(hit[i - 2 + shift], hit[i - 1 + shift]))
          count--;
      }
    }
    if (count == 3) {
      for (int k = 0; k < count; k++) {
        track.addHit(hit[k]);
      }
      track.refit();
      track.szFit();
      new ((*fRiemannTracks)[j]) PndRiemannTrack(track);
    } else {
      new ((*fRiemannTracks)[j]) PndRiemannTrack(track0);
    }
  }
}
void PndMvdRiemannTrackFinderTaskCutPar::CalcParHists()
{
  for (int i = 0; i < fTrackCandArray->GetEntriesFast(); i++) {
    if ((((PndRiemannTrack *)fRiemannTracks->At(i))->r() > 0) && (((PndTrackCand *)fTrackCandArray->At(i))->GetNHits() <= fNCut)) {
      unsigned int detId, hitId;
      TVector3 pos;
      PndMCTrack *myTrack = (PndMCTrack *)fMCTrackArray->At(((PndTrackCand *)fTrackCandArray->At(i))->getMcTrackId());
      double Pt = myTrack->GetPt();
      double Theta = (myTrack->GetMomentum().Theta()) * 180 / TMath::Pi();
      PndRiemannHit hit0;
      for (unsigned int j = 0; j < ((PndTrackCand *)fTrackCandArray->At(i))->GetNHits(); j++) {
        detId = ((PndTrackCand *)fTrackCandArray->At(i))->GetSortedHit(j).GetDetId();
        hitId = ((PndTrackCand *)fTrackCandArray->At(i))->GetSortedHit(j).GetHitId();
        if ((int)detId == FairRootManager::Instance()->GetBranchId(fHitBranch)) {
          hit0.setXYZ(((PndSdsHit *)fHitArray->At(hitId))->GetPosition().X(), ((PndSdsHit *)fHitArray->At(hitId))->GetPosition().Y(),
                      ((PndSdsHit *)fHitArray->At(hitId))->GetPosition().Z());
          hit0.setDXYZ(((PndSdsHit *)fHitArray->At(hitId))->GetDx(), ((PndSdsHit *)fHitArray->At(hitId))->GetDy(), ((PndSdsHit *)fHitArray->At(hitId))->GetDz());
        } else if ((int)detId == FairRootManager::Instance()->GetBranchId(fHitBranch2)) {
          hit0.setXYZ(((PndSdsHit *)fHitArray2->At(hitId))->GetPosition().X(), ((PndSdsHit *)fHitArray2->At(hitId))->GetPosition().Y(),
                      ((PndSdsHit *)fHitArray2->At(hitId))->GetPosition().Z());
          hit0.setDXYZ(((PndSdsHit *)fHitArray2->At(hitId))->GetDx(), ((PndSdsHit *)fHitArray2->At(hitId))->GetDy(), ((PndSdsHit *)fHitArray2->At(hitId))->GetDz());
        } else {
          std::cout << "ERROR" << std::endl;
          continue;
        };
      }
      if ((Pt >= fPtS) && (Pt <= fPtF) && (Theta >= fThetaS) && (Theta <= fThetaF)) {
        fhistsDist[TMath::FloorNint(((Pt - fPtS) * fNPt / (fPtF - fPtS)))][TMath::FloorNint(((Theta - fThetaS) * fNTh / (fThetaF - fThetaS)))]->Fill(
          ((PndRiemannTrack *)fRiemannTracks->At(i))->dist(&hit0));
        fhistsChi2[TMath::FloorNint(((Pt - fPtS) * fNPt / (fPtF - fPtS)))][TMath::FloorNint(((Theta - fThetaS) * fNTh / (fThetaF - fThetaS)))]->Fill(
          ((PndRiemannTrack *)fRiemannTracks->At(i))->calcSZChi2(&hit0));
      }
    }
  }
}
bool PndMvdRiemannTrackFinderTaskCutPar::CheckTooCloseHits(PndRiemannHit hit1, PndRiemannHit hit2)
{
  // int count=0;
  TVector3 delta;
  delta = hit1.x() - hit2.x();
  if (delta.Mag() < fMinPointDist) {
    return false;
  } else
    return true;
}

void PndMvdRiemannTrackFinderTaskCutPar::CalcCuts()
{
  for (int i = 0; i < fNPt; i++) {
    for (int j = 0; j < fNTh; j++) {
      double Th1 = 0.95;
      double CutDist = 0;
      double CutChi2 = 0;
      double S01 = fhistsDist[i][j]->Integral();
      double S1 = 0;
      if (S01 > 0) {
        S1 += fhistsDist[i][j]->GetBinContent(fNbin / 2);
        for (int k = 1; k < (fNbin / 2); k++) {
          S1 += fhistsDist[i][j]->GetBinContent(fNbin / 2 - k) + fhistsDist[i][j]->GetBinContent(fNbin / 2 + k);
          if ((S1 / S01) >= Th1) {
            CutDist = ((double)k * frangeDist) / ((double)fNbin / 2.0);
            break;
          }
        }
      }

      double Th2 = Th1;
      double S02 = fhistsChi2[i][j]->Integral();
      double S2 = 0;
      if (S02 > 0) {
        for (int k = 0; k < fNbin; k++) {
          S2 += fhistsChi2[i][j]->GetBinContent(k);
          if ((S2 / S02) >= Th2) {
            CutChi2 = ((double)k * frangeChi2) / ((double)fNbin);
            break;
          }
        }
      }
      fCutDistH->SetBinContent(i + 1, j + 1, CutDist);
      fCutChi2H->SetBinContent(i + 1, j + 1, CutChi2);
    }
  }
  TFile fout("MVDCutHists.root", "recreate");
  fCutDistH->Write();
  fCutChi2H->Write();
  fout.Close();
}
void PndMvdRiemannTrackFinderTaskCutPar::FinishEvent()
{
  fRiemannTracks->Clear();
}

ClassImp(PndMvdRiemannTrackFinderTaskCutPar);
