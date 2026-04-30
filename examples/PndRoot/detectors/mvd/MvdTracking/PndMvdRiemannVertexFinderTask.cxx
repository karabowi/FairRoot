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

#include "PndMvdRiemannVertexFinderTask.h"

#include <iostream>
#include <math.h>

// Root includes
#include "TROOT.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"
#include "TVector3.h"
#include "TH1F.h"

// framework includes
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "PndMCTrack.h"
#include "FairLogger.h"

// PndMvd includes
// #include "PndMvdTrackCand.h"
#include "PndSdsHit.h"
#include "PndSdsMCPoint.h"
#include "PndSdsCluster.h"
#include "PndSdsDigi.h"
//#include "PndRiemannTrackFinder.h"
#include "PndRiemannHit.h"
#include "PndRiemannTrack.h"
#include "PndDetectorList.h"
#include "PndSdsMCPoint.h"

PndMvdRiemannVertexFinderTask::PndMvdRiemannVertexFinderTask()
  : FairTask("MVD Riemann VERTEX Finder"), delta(nullptr), wrongV(nullptr), eff(), ghosts(), fHitBranch("MVDHitsPixel"), fHitBranch2("MVDHitsStrip"),
    fTrackBranch("MVDRiemannTrackCand"), fIdealTrackCandBranch("MVDIdealTrackCand"), fMCTrackBranch("MCTrack"), fEventNr(0), fVerbose(0), fVertexCut(0.), fHitArray(nullptr),
    fHitArray2(nullptr), fTrackCandArray(nullptr), fTrackArray(nullptr), fIdealTrackCandArray(nullptr), fMCTrackArray(nullptr), fVertex(nullptr), fMCVertex(nullptr)
{
}

PndMvdRiemannVertexFinderTask::~PndMvdRiemannVertexFinderTask() {}

void PndMvdRiemannVertexFinderTask::SetParContainers()
{
  // Get Base Container
  /*
    FairRun* ana = FairRun::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();
    fGeoPar = (PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar"));
  */
}

InitStatus PndMvdRiemannVertexFinderTask::ReInit()
{

  InitStatus stat = kSUCCESS;
  return stat;

  /*
  FairRun* ana = FairRun::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  fGeoPar=(PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar"));

  return kSUCCESS;
  */
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdRiemannVertexFinderTask::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    std::cout << "-E- PndMvdRiemannVertexFinderTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input array
  fHitArray = (TClonesArray *)ioman->GetObject(fHitBranch);
  if (!fHitArray) {
    std::cout << "-W- PndMvdRiemannVertexFinderTask::Init: "
              << "No hitArray!" << std::endl;
    return kERROR;
  }

  fHitArray2 = (TClonesArray *)ioman->GetObject(fHitBranch2);
  if (!fHitArray2) {
    std::cout << "-W- PndMvdRiemannVertexFinderTask::Init: "
              << "No hitArray2!" << std::endl;
    return kERROR;
  }

  fTrackCandArray = (TClonesArray *)ioman->GetObject(fTrackBranch);
  if (!fTrackCandArray) {
    std::cout << "-W- PndMvdRiemannVertexFinderTask::Init: "
              << "No tracks!" << std::endl;
    return kERROR;
  }

  fIdealTrackCandArray = (TClonesArray *)ioman->GetObject(fIdealTrackCandBranch);
  if (!fIdealTrackCandArray) {
    std::cout << "-W- PndMvdRiemannVertexFinderTask::Init: "
              << "No ideal tracks!" << std::endl;
    return kERROR;
  }

  fMCTrackArray = (TClonesArray *)ioman->GetObject(fMCTrackBranch);
  if (!fMCTrackArray) {
    std::cout << "-W- PndMvdRiemannVertexFinderTask::Init: "
              << "No MC tracks!" << std::endl;
    return kERROR;
  }

  fTrackArray = new TClonesArray("PndRiemannTrack");

  delta = new TH1F("delta", "delta", 1000, 0, 10);
  wrongV = new TH1F("wrong", "wrong", 1000, 0, 10);

  fVertex = new TClonesArray("PndSdsMCPoint");
  ioman->Register("Vertex", "MVD", fVertex, kTRUE);

  fMCVertex = new TClonesArray("PndSdsMCPoint");
  ioman->Register("MCVertex", "MVD", fMCVertex, kTRUE);

  //  fRiemannTrackArray = new TClonesArray("PndRiemannTrack");
  //  ioman->Register("MVDRiemannTrack","MVD",fRiemannTrackArray, kTRUE);

  LOG(info) << " PndMvdRiemannVertexFinderTask: Initialisation successfull";
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndMvdRiemannVertexFinderTask::Exec(Option_t *)
{
  // Reset output array
  if (!fTrackCandArray) {
    std::cout << "PndMvdRiemannVertexFinderTask no TrackCandArray!" << std::endl;
    return;
  }
  std::vector<int> CheckedCand;
  refit(CheckedCand);

  std::vector<std::pair<int, int>> PairCand;
  std::vector<std::pair<int, int>> TrueMCCand;
  std::vector<std::pair<int, int>> FalseMCCand;
  std::vector<std::pair<int, int>> MCCand;
  int MaxIndex = 0;
  FindVertex(CheckedCand, PairCand, TrueMCCand, FalseMCCand, MCCand, MaxIndex);
  CalcEfficiency(TrueMCCand, FalseMCCand, MCCand);

  std::cout << "Done event : " << fEventNr++ << std::endl;
}
void PndMvdRiemannVertexFinderTask::CalcEfficiency(std::vector<std::pair<int, int>> TrueMCCand, std::vector<std::pair<int, int>> FalseMCCand,
                                                   std::vector<std::pair<int, int>> MCCand)
{
  int counter = 0;
  for (unsigned int i = 0; i < MCCand.size(); i++) {
    int MCfirst = MCCand[i].first;
    int MCsecond = MCCand[i].second;
    for (unsigned int j = 0; j < TrueMCCand.size(); j++) {
      int first = TrueMCCand[j].first;
      int second = TrueMCCand[j].second;
      if ((MCfirst == first && MCsecond == second) or (MCfirst == second && MCsecond == first)) {
        counter++;
        break;
      }
    }
  }
  eff.first += counter;
  eff.second += MCCand.size();
  ghosts.first += FalseMCCand.size();
  ghosts.second += MCCand.size();
  //		std::cout<<"eff= "<<((double)counter)/((double)MCCand.size())<<std::endl;
  //		std::cout<<"ghosts= "<<((double)FalseMCCand.size())/((double)MCCand.size())<<std::endl;
}

void PndMvdRiemannVertexFinderTask::FindVertex(std::vector<int> CheckedCand, std::vector<std::pair<int, int>> &PairCand, std::vector<std::pair<int, int>> &TrueMCCand,
                                               std::vector<std::pair<int, int>> &FalseMCCand, std::vector<std::pair<int, int>> &MCCand, int &MaxIndex)
{
  for (int i = 0; i < fTrackArray->GetEntriesFast(); i++) {
    PndRiemannTrack track1 = *(PndRiemannTrack *)fTrackArray->At(i);
    track1.SetVerbose(-1);
    for (int j = i + 1; j < fTrackArray->GetEntriesFast(); j++) {
      PndRiemannTrack track2 = *(PndRiemannTrack *)fTrackArray->At(j);
      track2.SetVerbose(-1);
      TVector3 p1, p2, av;
      if ((track2.calcIntersection(track1, p1, p2) == 1) && CheckTwoCands(CheckedCand[i], CheckedCand[j])) {
        //					  std::cout<<"Vertex: "<<i<<" : "<<j<<std::endl;
        pair<int, int> Pair(i, j);
        PairCand.push_back(Pair);
        if (j > MaxIndex)
          MaxIndex = j;
        av = p1 + p2;
        av *= 0.5;
        PndSdsMCPoint *Vertex1 = new PndSdsMCPoint(0, 0, -1, p1, p1, p1, p1, 0, 0, 0);
        PndSdsMCPoint *Vertex2 = new PndSdsMCPoint(0, 0, -1, p2, p2, p2, p2, 0, 0, 0);
        new ((*fVertex)[fVertex->GetEntriesFast()]) PndSdsMCPoint(*Vertex1);
        new ((*fVertex)[fVertex->GetEntriesFast()]) PndSdsMCPoint(*Vertex2);

        int c1 = FoundCandInMCCands(CheckedCand[i]);
        int c2 = FoundCandInMCCands(CheckedCand[j]);
        if (c1 > -1 && c2 > -1) {
          //							  std::cout<<c1<<"  "<<c2<<std::endl;
          PndTrackCand *Cand1 = (PndTrackCand *)fIdealTrackCandArray->At(c1);
          PndMCTrack *myTrack1 = (PndMCTrack *)fMCTrackArray->At(Cand1->getMcTrackId());
          PndTrackCand *Cand2 = (PndTrackCand *)fIdealTrackCandArray->At(c2);
          PndMCTrack *myTrack2 = (PndMCTrack *)fMCTrackArray->At(Cand2->getMcTrackId());
          if (((myTrack1->GetPdgCode() == myTrack2->GetPdgCode()) or
               ((myTrack1->GetPdgCode() * myTrack2->GetPdgCode() < 0) && (fabs(myTrack1->GetPdgCode()) != fabs(myTrack2->GetPdgCode()))))) { /// for D+D-
            delta->Fill((myTrack1->GetStartVertex() - p1).Mag());
            delta->Fill((myTrack1->GetStartVertex() - p2).Mag());
            pair<int, int> Pair2(c1, c2);
            TrueMCCand.push_back(Pair2);
          } else {
            wrongV->Fill((myTrack1->GetStartVertex() - p1).Mag());
            wrongV->Fill((myTrack1->GetStartVertex() - p2).Mag());
            pair<int, int> Pair3(c1, c2);
            FalseMCCand.push_back(Pair3);
          }
        }
      }
    }
  }

  for (int i = 0; i < fIdealTrackCandArray->GetEntriesFast(); i++) {
    PndTrackCand *Cand1 = (PndTrackCand *)fIdealTrackCandArray->At(i);
    PndMCTrack *myTrack1 = (PndMCTrack *)fMCTrackArray->At(Cand1->getMcTrackId());
    for (int j = i + 1; j < fIdealTrackCandArray->GetEntriesFast(); j++) {
      PndTrackCand *Cand2 = (PndTrackCand *)fIdealTrackCandArray->At(j);
      PndMCTrack *myTrack2 = (PndMCTrack *)fMCTrackArray->At(Cand2->getMcTrackId());
      if (((myTrack1->GetPdgCode() == myTrack2->GetPdgCode()) or
           ((myTrack1->GetPdgCode() * myTrack2->GetPdgCode() < 0) && (fabs(myTrack1->GetPdgCode()) != fabs(myTrack2->GetPdgCode()))))) {
        if (CheckRecoTrack(Cand1, myTrack1) && CheckRecoTrack(Cand2, myTrack2)) {
          pair<int, int> Pair(i, j);
          MCCand.push_back(Pair);
          //					  std::cout<<"IdealVertex: "<<i<<" : "<<j<<std::endl;
        }
      }
    }
  }
}

void PndMvdRiemannVertexFinderTask::refit(std::vector<int> &CheckedCand)
{
  for (int i = 0; i < fTrackCandArray->GetEntriesFast(); i++) {
    PndTrackCand *Cand = (PndTrackCand *)fTrackCandArray->At(i);
    unsigned int detId, hitId;
    PndRiemannTrack track;
    for (unsigned int j = 0; j < Cand->GetNHits(); j++) {
      detId = Cand->GetSortedHit(j).GetDetId();
      hitId = Cand->GetSortedHit(j).GetHitId();
      PndSdsHit *point = new PndSdsHit();
      //		  std::cout<<"detId=  "<<detId<<"  "<<"hitId= "<<hitId<<std::endl;
      if ((int)detId == FairRootManager::Instance()->GetBranchId(fHitBranch))
        point = (PndSdsHit *)fHitArray->At(hitId);
      else if ((int)detId == FairRootManager::Instance()->GetBranchId(fHitBranch2))
        point = (PndSdsHit *)fHitArray2->At(hitId);
      else
        point = 0;
      if (point != 0) {
        PndRiemannHit hit;
        hit.setXYZ(point->GetX(), point->GetY(), point->GetZ());
        hit.setDXYZ(point->GetDx(), point->GetDy(), point->GetDz());
        track.addHit(hit);
      }
    }
    track.refit(true);
    track.szFit(true);
    track.SetVertexCut(fVertexCut);
    int size = fTrackArray->GetEntriesFast();
    new ((*fTrackArray)[size]) PndRiemannTrack(track);
    CheckedCand.push_back(i);
  }
  std::cout << "--------------------------------------------------" << fTrackCandArray->GetEntriesFast() << std::endl;
}

int PndMvdRiemannVertexFinderTask::FoundCandInMCCands(int candN)
{
  PndTrackCand *FoundCand = (PndTrackCand *)fTrackCandArray->At(candN);
  for (int k = 0; k < fIdealTrackCandArray->GetEntriesFast(); k++) {
    PndTrackCand *TestCand = (PndTrackCand *)fIdealTrackCandArray->At(k);
    unsigned int counter = 0;
    bool ZeroPos = false;
    for (unsigned int i = 0; i < FoundCand->GetNHits(); i++) {
      unsigned int d1, h1;
      d1 = FoundCand->GetSortedHit(i).GetDetId();
      h1 = FoundCand->GetSortedHit(i).GetHitId();
      if (d1 == 0) {
        ZeroPos = true;
        continue;
      }
      for (unsigned int j = 0; j < TestCand->GetNHits(); j++) {
        unsigned int d2, h2;
        d2 = TestCand->GetSortedHit(j).GetDetId();
        h2 = TestCand->GetSortedHit(j).GetHitId();
        if (d1 == d2 && h1 == h2) {
          counter++;
        }
      }
    }
    if ((counter == FoundCand->GetNHits() && !ZeroPos) or (counter == FoundCand->GetNHits() - 1 && ZeroPos)) {
      return k;
    }
  }
  return -1;
}

bool PndMvdRiemannVertexFinderTask::CheckVertex(std::vector<int> Combination, std::vector<std::pair<int, int>> PairCand)
{
  int count1 = 0;
  for (unsigned int i = 0; i < PairCand.size(); i++) {
    int ind1 = PairCand[i].first, ind2 = PairCand[i].second;
    int count2 = 0;
    for (unsigned int j = 0; j < Combination.size(); j++) {
      if ((ind1 == Combination[j]) or (ind2 == Combination[j]))
        count2++;
    }
    if (count2 == 2)
      count1++;
  }

  if (count1 == 3)
    return true;
  else
    return false;
}
bool PndMvdRiemannVertexFinderTask::CheckTwoCands(int first, int second)
{
  PndTrackCand *cand1 = (PndTrackCand *)fTrackCandArray->At(first);
  PndTrackCand *cand2 = (PndTrackCand *)fTrackCandArray->At(second);
  int counter = 0;
  for (unsigned int i = 0; i < cand1->GetNHits(); i++) {
    unsigned int d1, h1;
    d1 = cand1->GetSortedHit(i).GetDetId();
    h1 = cand1->GetSortedHit(i).GetHitId();
    for (unsigned int j = 0; j < cand2->GetNHits(); j++) {
      unsigned int d2, h2;
      d2 = cand2->GetSortedHit(j).GetDetId();
      h2 = cand2->GetSortedHit(j).GetHitId();
      if (d1 == d2 && h1 == h2) {
        counter++;
      }
    }
  }
  if (counter < 3)
    return true;
  else
    return false;
}

bool PndMvdRiemannVertexFinderTask::CheckRecoTrack(PndTrackCand *cand, PndMCTrack *myTrack)
{
  if (/*myTrack->GetPdgCode()==211 &&*/ (cand->GetNHits() > 2) && (myTrack->GetMotherID() == -1)) {
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
          if (a.Mag() < 1) {
            count++;
          }
        }
      }
    }
    if ((cand->GetNHits() - count / 2) < 3) {
      //			std::cout <<"Less then 3 base points in RecoTrack"<<"   "<<cand->getNHits()<<"   "<<count <<std::endl;
      return false;
    } else
      return true;

  } else
    return false;
}

void PndMvdRiemannVertexFinderTask::FinishEvent()
{
  fTrackCandArray->Clear();
  fIdealTrackCandArray->Clear();
  fTrackArray->Clear();

  fVertex->Clear();
  fMCVertex->Clear();
}

ClassImp(PndMvdRiemannVertexFinderTask);
