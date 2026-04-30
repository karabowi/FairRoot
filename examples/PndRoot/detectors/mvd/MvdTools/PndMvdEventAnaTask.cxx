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

// -------------------------------------------------------------------------
// -----                PndMvdEventAnaTask source file             -----
// -----                  Created 18/07/08  by T.Stockmanns        -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"
#include "TVector3.h"

// framework includes
#include "FairRootManager.h"
#include "PndMvdEventAnaTask.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "FairLogger.h"
#include "PndMCTrack.h"
// PndMvd includes
#include "PndSdsMCPoint.h"
#include "PndSdsDigiPixel.h"
#include "PndSdsDigiStrip.h"
#include "PndSdsHit.h"
#include "PndSdsCluster.h"
#include "GFTrackCand.h"
#include "PndRiemannTrack.h"

#include <vector>
#include <map>

// -----   Default constructor   -------------------------------------------
PndMvdEventAnaTask::PndMvdEventAnaTask()
  : FairTask("Event Display task for PANDA PndMvd"), fPrintTrack(true), fPrintMCHit(true), fPrintCluster(true), fPrintPixDigis(true), fPrintPixHit(true), fPrintStripCluster(true),
    fPrintStripDigis(true), fPrintStripHit(true), fPrintTrackMatch(true), fPrintGhosts(true), fNTracks(0), fNPossibleTracks(0), fNCompleteTracks(0), fNPartTracks(0),
    fNNotFoundPossibleTracks(0), fNNotFoundTracks(0), fNGhostTracks(0), fEventNr(0)
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMvdEventAnaTask::~PndMvdEventAnaTask() {}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdEventAnaTask::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndMvdEventAnaTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input arrays
  fMCTracks = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fMCTracks) {
    std::cout << "-W- PndMvdEventAnaTask::Init: "
              << "No MCTrack"
              << " array!" << std::endl;
    return kERROR;
  }

  fMCHits = (TClonesArray *)ioman->GetObject("MVDPoint");
  if (!fMCHits) {
    std::cout << "-W- PndMvdEventAnaTask::Init: "
              << "No MVDPoint"
              << " array!" << std::endl;
    return kERROR;
  }

  fStripDigis = (TClonesArray *)ioman->GetObject("MVDStripDigis");
  if (!fStripDigis) {
    std::cout << "-W- PndMvdEventAnaTask::Init: "
              << "No MVDStripDigis"
              << " array!" << std::endl;
  }

  fPixDigis = (TClonesArray *)ioman->GetObject("MVDPixelDigis");
  if (!fPixDigis) {
    std::cout << "-W- PndMvdEventAnaTask::Init: "
              << "No MVDPixDigis"
              << " array!" << std::endl;
  }

  fPixReco = (TClonesArray *)ioman->GetObject("MVDHitsPixel");
  if (!fPixReco) {
    std::cout << "-W- PndMvdEventAnaTask::Init: "
              << "No MVDHitsPixel"
              << " array!" << std::endl;
  }

  fStripReco = (TClonesArray *)ioman->GetObject("MVDHitsStrip");
  if (!fStripReco) {
    std::cout << "-W- PndMvdEventAnaTask::Init: "
              << "No MVDHitsStrip"
              << " array!" << std::endl;
  }

  fPixCluster = (TClonesArray *)ioman->GetObject("MVDClusterCand");
  if (!fPixCluster) {
    std::cout << "-W- PndMvdEventAnaTask::Init: "
              << "No MVDClusterCand"
              << " array!" << std::endl;
  }

  fStripCluster = (TClonesArray *)ioman->GetObject("MVDStripClusterCand");
  if (!fStripCluster) {
    std::cout << "-W- PndMvdEventAnaTask::Init: "
              << "No MVDStripClusterCand"
              << " array!" << std::endl;
  }

  fTrackCand = (TClonesArray *)ioman->GetObject("MVDRiemannTrackCand");
  if (!fTrackCand) {
    std::cout << "-W- PndMvdEventAnaTask::Init: "
              << "No MVDRiemannTrackCand"
              << " array!" << std::endl;
    fTrackCand = (TClonesArray *)ioman->GetObject("MVDIdealTrackCand");
    if (!fTrackCand)
      std::cout << "-W- PndMvdEventAnaTask::Init: "
                << "No MVDIdealTrackCand"
                << " array!" << std::endl;
  }

  fHTracksPerEvent = new TH1I("HTracksPerEvent", "Tracks per Event", 21, -0.5, 20.5);
  fHHitsPerTrack = new TH1I("HHitsPerTrack", "Hits per Track", 21, 0.5, 20.5);
  fHEnergyPerHit = new TH1D("HEnergyPerHit", "Energy per Hit", 200, 0, 10000000);

  fHPointRes = new TH1D("HPointRes", "Point Resolution", 1000, 0, 0.1);
  fHPointRes->SetLineColor(1);
  fHPointResS = new TH1D("HPointResS", "Point Resolution", 1000, 0, 0.1);
  fHPointResS->SetLineColor(3);
  fHPointResD = new TH1D("HPointResD", "Point Resolution", 1000, 0, 0.1);
  fHPointResD->SetLineColor(4);
  fHPointResM = new TH1D("HPointResM", "Point Resolution", 1000, 0, 0.1);
  fHPointResM->SetLineColor(5);

  fHDigisPerCluster = new TH1I("HDigisPerCluster", "Digis per Cluster", 20, 0, 20);
  fHEnergyRes = new TH1D("HEnergyRes", "Energy Resolution", 1000, -100000, 100000);
  fHPRes = new TH1D("HPRes", "Momentum Resolution", 1000, -5, 5);
  fHPtRes = new TH1D("HPtRes", "Transversal Momentum Resolution", 1000, -5, 5);

  fHPointResStrip = new TH1D("HPointResStrip", "Point Resolution Strips", 1000, 0, 0.1);
  fHPointResStrip->SetLineColor(6);
  fHPointResSStrip = new TH1D("HPointResSStrip", "Point Resolution Strip", 1000, 0, 0.1);
  fHPointResSStrip->SetLineColor(7);
  fHPointResDStrip = new TH1D("HPointResDStrip", "Point Resolution Strip", 1000, 0, 0.1);
  fHPointResDStrip->SetLineColor(8);
  fHPointResMStrip = new TH1D("HPointResMStrip", "Point Resolution Strip", 1000, 0, 0.1);
  fHPointResMStrip->SetLineColor(9);

  fHDigisPerClusterStrip = new TH1I("HDigisPerClusterStrip", "Digis per Cluster Strips", 20, 0, 20);
  fHDigisPerClusterStrip->SetLineColor(2);
  fHEnergyResStrip = new TH1D("HEnergyResStrip", "Energy Resolution Strips", 1000, -100000, 100000);
  fHEnergyResStrip->SetLineColor(2);

  fHRiemannRes = new TH1I("HRiemannRes", "Quality of the Riemann Trackfinding", 16, -5.5, 10.5);
  fHRiemannFakes = new TH1I("HRiemannFakes", "Wrong assign hits in Track", 16, -5.5, 10.5);
  fHRiemannFakes->SetLineColor(2);
  fHRiemannTracksPerTrack = new TH1I("HRiemannTracksPerTrack", "Found Tracks per MC Track", 11, -0.5, 10.5);
  fHRiemannTracksPerTrackAdd = new TH1I("HRiemannTracksPerTrackAdd", "Found Tracks per MC Track", 11, -0.5, 10.5);
  fHRiemannTracksPerTrackAdd->SetLineColor(2);

  fHRiemannVertexResolutionX = new TH1D("HRiemannVertexResolutionX", "Difference between reco vertex and MC vertex", 1000, -1, 1);
  fHRiemannVertexResolutionY = new TH1D("HRiemannVertexResolutionY", "Difference between reco vertex and MC vertex", 1000, -1, 1);
  fHRiemannVertexResolutionZ = new TH1D("HRiemannVertexResolutionZ", "Difference between reco vertex and MC vertex", 1000, -1, 1);

  return kSUCCESS;
}
// -------------------------------------------------------------------------
void PndMvdEventAnaTask::SetParContainers()
{
  // Get Base Container
  //  FairRun* ana = FairRun::Instance();
  //  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
}

// -----   Public method Exec   --------------------------------------------
void PndMvdEventAnaTask::Exec(Option_t *)
{
  std::map<int, std::vector<int>> mcHitMap;            // Track ->  MCHits
  std::map<int, std::vector<int>> trackToTrackCandMap; // Track -> TrackCand

  std::vector<PndRiemannTrack> riemannTracks;
  std::vector<int> MCTrackOrderRiemann; // information which riemannTrack belongs to which MCTrack;

  TVector3 MCPos, RecoPos;
  double MCEnergy;
  double TrackP, TrackPt;

  mcHitMap = AssignHitsToTracks();
  fGhostCand.clear();
  fGhostCand.resize(fTrackCand->GetEntriesFast(), 0);
  fTrackPixHitIdMap.clear();
  fTrackStripHitIdMap.clear();

  std::cout << "------------Event " << fEventNr << "-------------" << std::endl;
  fEventNr++;
  fHTracksPerEvent->Fill(mcHitMap.size());
  fNTracks += mcHitMap.size();

  for (std::map<int, std::vector<int>>::const_iterator kIt = mcHitMap.begin(); kIt != mcHitMap.end(); kIt++) { // go through all tracks
    PndMCTrack *myTrack = (PndMCTrack *)(fMCTracks->At(kIt->first));
    std::vector<int> MChits = kIt->second;
    TrackPt = myTrack->GetMomentum().Pt();
    TrackP = myTrack->GetMomentum().Mag();
    fHHitsPerTrack->Fill(MChits.size());

    if (fPrintTrack) {
      std::cout << "<<<<<<<<<<< MCTrack >>>>>>>>>> " << std::endl;
      myTrack->Print(kIt->first);
      TVector3 startVertex = myTrack->GetStartVertex();
      std::cout << "Pt: " << TrackPt << " GeV/c; P: " << TrackP << " GeV/c" << std::endl;
      std::cout << "StartVertex: " << startVertex.X() << " " << startVertex.Y() << " " << startVertex.Z() << std::endl;
    }
    for (unsigned int p = 0; p < MChits.size(); p++) { // go through all hits in track
      PndSdsMCPoint *myPoint = (PndSdsMCPoint *)(fMCHits->At(MChits[p]));
      if (fPrintMCHit) {
        std::cout << "-------------------------------" << std::endl;
        myPoint->Print();
      }
      MCPos = (myPoint->GetPosition() + myPoint->GetPositionOut()) * 0.5;
      MCEnergy = myPoint->GetEnergyLoss() * 10E9;
      fHEnergyPerHit->Fill(MCEnergy);

      std::vector<int> pixCluster = GetClusters(MChits[p], true); // Indices of all clusters belonging to one hit

      for (unsigned int clInd = 0; clInd < pixCluster.size(); clInd++) {

        PndSdsCluster *myCluster = (PndSdsCluster *)(fPixCluster->At(pixCluster[clInd]));
        std::vector<Int_t> digiInd = myCluster->GetClusterList(); // gets the list of pixel Digis belonging to the cluster
        int recoHit = GetRecoHit(pixCluster[clInd], true);
        fHDigisPerCluster->Fill(digiInd.size());
        if (recoHit > -1)
          fTrackPixHitIdMap[kIt->first].push_back(recoHit);

        PrintClusterDigiInfo(pixCluster[clInd], digiInd, true);
        if (recoHit < 0)
          LOG(warn) << " No Reco Hit found!";
        else
          PrintRecoHitInfo(recoHit, digiInd.size(), MCPos, MCEnergy, true);
      }

      std::vector<int> stripCluster = GetClusters(MChits[p], false);

      int hitCount = 0;
      int oldRecoHit = -1;
      for (unsigned int clInd = 0; clInd < stripCluster.size(); clInd++) {
        int recoHit = -1;

        PndSdsCluster *myCluster = (PndSdsCluster *)(fStripCluster->At(stripCluster[clInd]));
        std::vector<Int_t> digiInd = myCluster->GetClusterList(); // gets the list of pixel Digis belonging to the cluster
        recoHit = GetRecoHit(stripCluster[clInd], false);

        fHDigisPerClusterStrip->Fill(digiInd.size());
        std::cout << "RecoHit: " << recoHit << std::endl;
        if (recoHit > -1) {
          hitCount++;
          if (oldRecoHit != recoHit) {
            fTrackStripHitIdMap[kIt->first].push_back(recoHit);
            oldRecoHit = recoHit;
          }
        }
        PrintClusterDigiInfo(stripCluster[clInd], digiInd, false);
        // std::cout << " hitCount: " << hitCount << " oldReco " << oldRecoHit <<  " recoHit: " << recoHit << std::endl;
        if (recoHit > -1) {
          if (hitCount > 1) {
            PrintRecoHitInfo(recoHit, digiInd.size(), MCPos, MCEnergy, false);
            if ((oldRecoHit > -1) && (oldRecoHit != recoHit))
              PrintRecoHitInfo(oldRecoHit, digiInd.size(), MCPos, MCEnergy, false);
          }
        }
      }
    }
    std::vector<int> pixHits = fTrackPixHitIdMap[kIt->first];
    std::vector<int> stripHits = fTrackStripHitIdMap[kIt->first];

    std::cout << std::endl;
    std::cout << "TrackID " << kIt->first << ": ";
    for (unsigned int testInd = 0; testInd < pixHits.size(); testInd++)
      std::cout << " 5/" << pixHits[testInd];

    for (unsigned int testInd = 0; testInd < stripHits.size(); testInd++)
      std::cout << " 4/" << stripHits[testInd];
    std::cout << std::endl;

    std::cout << "MCHitMap: ";
    for (unsigned int testInd = 0; testInd < mcHitMap[kIt->first].size(); testInd++)
      std::cout << mcHitMap[kIt->first].at(testInd) << " ";
    std::cout << std::endl;

    std::vector<int> matches;
    std::vector<int> candidates;
    GetTrackCandsForMCTrack(pixHits, stripHits, matches, candidates);
    std::cout << "TrackCands for MCTrack: ";
    for (unsigned int i = 0; i < candidates.size(); i++)
      std::cout << candidates[i];
    std::cout << std::endl;
    trackToTrackCandMap[kIt->first] = candidates;

    int TrackMatch = 0;
    int oldmatches = 0;
    int highestMatch = -1;
    for (unsigned int i = 0; i < matches.size(); i++) {
      if (oldmatches < matches[i]) {
        oldmatches = matches[i];
        highestMatch = i;
      }
      GFTrackCand *myCand = (GFTrackCand *)fTrackCand->At(candidates[i]);
      PndRiemannTrack myRiemannTrack;
      MCTrackOrderRiemann.push_back(kIt->first);
      myRiemannTrack.SetVerbose(1);
      for (UInt_t j = 0; j < myCand->getNHits(); j++) {
        unsigned int detId, hitId;
        myCand->getHit(j, detId, hitId);
        PndRiemannHit hit(GetFairHit(detId, hitId));
        myRiemannTrack.addHit(hit);
      }

      myRiemannTrack.refit();
      myRiemannTrack.szFit();
      TVectorD n = myRiemannTrack.n();
      // std::cout << "RiemannNormal: "  << n[0] << " " << n[1] << " " << n[2] << std::endl;
      riemannTracks.push_back(myRiemannTrack);

      double curv = myCand->getCurv();
      double dip = myCand->getDip();
      double pt = 1 / curv;
      pt *= 2;
      pt *= 0.00299792;
      unsigned int detId, hitId;
      if (matches[i] > 2) {
        TrackMatch++;
        myCand->getHit(0, detId, hitId);
        if (detId > 0)
          fHRiemannFakes->Fill(myCand->getNHits() - matches[i]);
        else
          fHRiemannFakes->Fill(myCand->getNHits() - matches[i] - 1);
      }
      if (fPrintTrackMatch) {
        myCand->getHit(0, detId, hitId);
        std::cout << std::endl;
        std::cout << "HitMatch: " << matches[i] << std::endl;
        std::cout << "Hits in Track: " << myCand->getNHits() << " Difference: ";
        if (detId > 0)
          std::cout << myCand->getNHits() - matches[i] << std::endl;
        else
          std::cout << myCand->getNHits() - matches[i] - 1 << std::endl;

        PrintTrackCand(myCand);
        std::cout << "Pt: " << pt << " GeV/c "
                  << " P: " << pt / dip << " GeV/c" << std::endl;
        std::cout << "Pt error: " << TrackPt - pt << " P error: " << TrackP - pt / dip << std::endl;
        std::cout << "TrackID: " << kIt->first << " mcHitMap.size: " << mcHitMap[kIt->first].size() << " matches: " << matches[i] << std::endl;
        std::cout << "Missing Hits: " << mcHitMap[kIt->first].size() - matches[i] << std::endl;
        if (mcHitMap[kIt->first].size() < (myCand->getNHits() - 1))
          fHRiemannRes->Fill(-2); // more hits than expected
      }
    }

    if (mcHitMap[kIt->first].size() > 2) {
      if (mcHitMap[kIt->first].size() > 3) {
        fNPossibleTracks++;
        if (oldmatches == 0)
          fNNotFoundPossibleTracks++;
      } else if (myTrack->GetMotherID() < 0) {
        fNPossibleTracks++;
        if (oldmatches == 0)
          fNNotFoundPossibleTracks++;
      }
    }
    if (oldmatches == 0)
      fNNotFoundTracks++;

    if (matches.size() > 0) { // there are riemann tracks to a MC Track
      GFTrackCand *myCand = (GFTrackCand *)fTrackCand->At(candidates[highestMatch]);
      double curv = myCand->getCurv();
      double dip = myCand->getDip();
      double pt = 1 / curv;
      pt *= 2;
      pt *= 0.00299792;
      fHPtRes->Fill(TrackPt - pt);
      fHPRes->Fill(TrackP - pt / dip);
      fHRiemannRes->Fill(mcHitMap[kIt->first].size() - matches[highestMatch]);
      if (mcHitMap[kIt->first].size() - matches[highestMatch] == 0)
        fNCompleteTracks++;
      else
        fNPartTracks++;
    }
    std::cout << "Found TracksPerTrack: " << TrackMatch;
    if (mcHitMap[kIt->first].size() > 2)
      std::cout << " 3Hits+" << std::endl;
    if (mcHitMap[kIt->first].size() > 2)
      fHRiemannTracksPerTrackAdd->Fill(TrackMatch);
    fHRiemannTracksPerTrack->Fill(TrackMatch);
  }
  if (riemannTracks.size() > 1) {
    std::cout << riemannTracks.size() << " Riemann Tracks used!" << std::endl;
    for (unsigned int i = 0; i < riemannTracks.size() - 1; i++) {
      for (unsigned int j = i + 1; j < riemannTracks.size(); j++) {
        TVector3 p1, p2;
        // riemannTracks[i].refit();
        // riemannTracks[i].szFit();
        // std::cout << "Track: " << i << std::endl;
        // riemannTracks[i].PrintHits();

        // riemannTracks[j].refit();
        // riemannTracks[j].szFit();
        // std::cout << "Track: " << j << std::endl;
        // riemannTracks[j].PrintHits();

        std::cout << "Vertex Test for: " << MCTrackOrderRiemann[i] << " and " << MCTrackOrderRiemann[j] << std::endl;
        int result = riemannTracks[i].calcIntersection(riemannTracks[j], p1, p2);

        if (result == 1) {
          std::cout << "Vertex1: " << p1.X() << " " << p1.Y() << " " << p1.Z() << std::endl;
          std::cout << "Vertex2: " << p2.X() << " " << p2.Y() << " " << p2.Z() << std::endl;
          PndMCTrack *myMCTrack = (PndMCTrack *)(fMCTracks->At(MCTrackOrderRiemann[i]));
          TVector3 MCVertex = myMCTrack->GetStartVertex();
          std::cout << "MCVertex " << MCTrackOrderRiemann[i] << ": " << MCVertex.X() << " " << MCVertex.Y() << " " << MCVertex.Z() << std::endl;
          MCVertex -= (p1 + p2) * 0.5;
          std::cout << "Difference: " << MCVertex.X() << " " << MCVertex.Y() << " " << MCVertex.Z() << std::endl;
          fHRiemannVertexResolutionX->Fill(MCVertex.X());
          fHRiemannVertexResolutionY->Fill(MCVertex.Y());
          fHRiemannVertexResolutionZ->Fill(MCVertex.Z());
        }
      }
    }
  }
  if (fPrintGhosts) {
    std::cout << std::endl;
    std::cout << "----Ghost TrackCandidates:-------" << std::endl;
    for (unsigned int tcIndex = 0; tcIndex < fGhostCand.size(); tcIndex++) {
      if (fGhostCand[tcIndex] < 3) {
        GFTrackCand *myTrackCand = (GFTrackCand *)fTrackCand->At(tcIndex);
        PrintTrackCand(myTrackCand);
        fHRiemannRes->Fill(-1); // Ghosts
        fNGhostTracks++;
      }
    }
    std::cout << std::endl;
  }
}

void PndMvdEventAnaTask::Finish()
{
  std::cout << std::endl;
  std::cout << "--------- Summary Track Fitting -----------" << std::endl;
  std::cout << "Number of Tracks: " << fNTracks << std::endl;
  std::cout << "Number of Tracks with more than 3 hits: " << fNPossibleTracks << std::endl;
  std::cout << (double)fNCompleteTracks / fNTracks * 100 << " % found completely" << std::endl;
  std::cout << (double)fNCompleteTracks / fNPossibleTracks * 100 << " % of possible tracks found completely" << std::endl;
  std::cout << (double)fNPartTracks / fNTracks * 100 << " % found partially" << std::endl;
  std::cout << (double)fNPartTracks / fNPossibleTracks * 100 << " % of possible tracks found partially" << std::endl;
  std::cout << (double)fNNotFoundPossibleTracks / fNPossibleTracks * 100 << " % of possible tracks not found" << std::endl;
  std::cout << (double)fNNotFoundTracks / fNTracks * 100 << " % of  Tracks not found in total" << std::endl;
  std::cout << (double)fNGhostTracks / fNTracks * 100 << " % of Ghost Tracks" << std::endl;
}

FairHit *PndMvdEventAnaTask::GetFairHit(Int_t detId, Int_t hitId)
{
  FairHit *p;

  if (detId == 5) {
    return (FairHit *)fPixReco->At(hitId);
  } else if (detId == 4) {
    return p = (FairHit *)fStripReco->At(hitId);
  }

  LOG(error) << " FairRiemannTrackCandDraw::GetFairHit : Unknown Detector with ID: " << detId;
  return nullptr;
}

// -------------------------------------------------------------------------
std::vector<int> PndMvdEventAnaTask::GetClusters(int MCHit, bool pixel)
{
  TClonesArray *cluster;

  std::vector<int> result;

  if (pixel == true) {
    cluster = fPixCluster;
  } else {
    cluster = fStripCluster;
  }

  if (cluster != nullptr) {
    for (int clIndex = 0; clIndex < cluster->GetEntriesFast(); clIndex++) { // go through all pixel clusters
      PndSdsCluster *myCluster = (PndSdsCluster *)(cluster->At(clIndex));
      if (MCHitBelongsToCluster(MCHit, myCluster, pixel)) {
        result.push_back(clIndex);
      }
    }
  }
  return result;
}

int PndMvdEventAnaTask::GetRecoHit(int clIndex, bool pixel) const
{
  TClonesArray *reco;

  if (pixel == true) {
    reco = fPixReco;
  } else {
    reco = fStripReco;
  }
  if (reco != nullptr) {
    for (int hitIndex = 0; hitIndex < reco->GetEntriesFast(); hitIndex++) {
      PndSdsHit *myHit = (PndSdsHit *)reco->At(hitIndex);
      if ((myHit->GetClusterIndex() == clIndex) || myHit->GetBotIndex() == clIndex) { // test if RecoHit belongs to cluster
        return hitIndex;
      }
    }
  }
  return -1;
}
// -------------------------------------------------------------------------
void PndMvdEventAnaTask::PrintClusterDigiInfo(int clIndex, std::vector<Int_t> digiInd, bool pixel)
{
  TClonesArray *digis;

  if (pixel) {
    digis = fPixDigis;
  } else {
    digis = fStripDigis;
  }

  if (digis == nullptr)
    fPrintPixDigis = false;

  if (fPrintCluster)
    std::cout << "Hit belongs to cluster " << clIndex << std::endl;
  if (fPrintPixDigis) {
    for (unsigned int s = 0; s < digiInd.size(); s++) {
      PndSdsDigi *myDigi = (PndSdsDigi *)(digis->At(digiInd[s]));
      if (pixel)
        ((PndSdsDigiPixel *)myDigi)->Print();
      else
        ((PndSdsDigiStrip *)myDigi)->Print();
    }
  }
}

void PndMvdEventAnaTask::PrintRecoHitInfo(int hitInd, int digiSize, TVector3 MCPos, double MCEnergy, bool pixel) const
{
  TClonesArray *reco;
  TH1 *hPointRes;
  TH1 *hPointResS;
  TH1 *hPointResD;
  TH1 *hPointResM;
  TH1 *hEnergyRes;

  if (pixel) {
    reco = fPixReco;

    hPointRes = fHPointRes;
    hPointResS = fHPointResS;
    hPointResD = fHPointResD;
    hPointResM = fHPointResM;
    hEnergyRes = fHEnergyRes;
  } else {
    reco = fStripReco;

    hPointRes = fHPointResStrip;
    hPointResS = fHPointResSStrip;
    hPointResD = fHPointResDStrip;
    hPointResM = fHPointResMStrip;
    hEnergyRes = fHEnergyResStrip;
  }

  PndSdsHit *myHit = (PndSdsHit *)reco->At(hitInd);
  if (fPrintPixHit) {
    if (pixel)
      std::cout << "PixHit: ";
    else
      std::cout << "StripHit: ";
    std::cout << hitInd << std::endl; // write out RecoHit
    myHit->Print();
  }
  TVector3 RecoPos = myHit->GetPosition();
  double RecoEnergy = myHit->GetEloss() * 10E9;
  TVector3 result = MCPos - RecoPos;
  ((TH1D *)hPointRes)->Fill(result.Mag());
  if (digiSize == 1)
    hPointResS->Fill(result.Mag());
  if (digiSize == 2)
    hPointResD->Fill(result.Mag());
  if (digiSize > 2)
    hPointResM->Fill(result.Mag());
  ((TH1D *)hEnergyRes)->Fill(MCEnergy - RecoEnergy);
}

bool PndMvdEventAnaTask::MCHitBelongsToCluster(int HitIndex, PndSdsCluster *cluster, bool pixCluster)
{
  bool result = false;
  std::vector<Int_t> clusterList = cluster->GetClusterList();
  for (unsigned int i = 0; i < clusterList.size() && result == false; i++) {
    PndSdsDigi *myDigi;
    if (pixCluster)
      myDigi = (PndSdsDigi *)(fPixDigis->At(clusterList[i]));
    else
      myDigi = (PndSdsDigi *)(fStripDigis->At(clusterList[i]));
    for (int j = 0; j < myDigi->GetNIndices(); j++)
      if (myDigi->GetIndex(j) == HitIndex)
        result = true;
  }
  return result;
}
// -------------------------------------------------------------------------

void PndMvdEventAnaTask::GetTrackCandsForMCTrack(std::vector<int> pixHitId, std::vector<int> stripHitId, std::vector<int> &matches, std::vector<int> &result)
{
  unsigned int detId, hitId;
  int hitMatch = 0;

  if (fTrackCand != nullptr) {
    for (int i = 0; i < fTrackCand->GetEntriesFast(); i++) {
      GFTrackCand *myTrackCand = (GFTrackCand *)fTrackCand->At(i);
      hitMatch = 0;
      for (unsigned int j = 0; j < myTrackCand->getNHits(); j++) {
        myTrackCand->getHit(j, detId, hitId);
        // std::cout << "DetId: " << detId << " HitId: " << hitId << std::endl;
        if (detId == 5) {
          for (unsigned int k = 0; k < pixHitId.size(); k++) {
            // std::cout << "PixHitId: " << pixHitId[k] << std::endl;
            if ((int)hitId == pixHitId[k])
              hitMatch++;
          }
        } else if (detId == 4) {
          for (unsigned int k = 0; k < stripHitId.size(); k++) {
            // std::cout << "StripHitId: " << stripHitId[k] << std::endl;
            if ((int)hitId == stripHitId[k])
              hitMatch++;
          }
        }
      }
      // std::cout << "internal HitMatch: " << hitMatch << std::endl;
      if (hitMatch > 2) {
        matches.push_back(hitMatch);
        result.push_back(i);
      }
      if (fGhostCand[i] < hitMatch)
        fGhostCand[i] = hitMatch;
    }
  }
}

std::map<int, std::vector<int>> PndMvdEventAnaTask::AssignHitsToTracks()
{
  std::map<int, std::vector<int>> result;
  for (int i = 0; i < fMCHits->GetEntriesFast(); i++) {         // get all MC Hits
    PndSdsMCPoint *myPoint = (PndSdsMCPoint *)(fMCHits->At(i)); // sort MCHits with Tracks
    // PndMCTrack* myTrack = (PndMCTrack*)(fMCTracks->At(myPoint->GetTrackID()));
    result[myPoint->GetTrackID()].push_back(i);
  }
  return result;
}

void PndMvdEventAnaTask::PrintTrackCand(GFTrackCand *cand) const
{
  unsigned int det, hit;
  // std::cout << "TrackCand: " << cand->getCurv() << " curv, " << cand->getDip() << " dip, " << (int)(cand->inverted()) << " inverted." << "\n";
  for (unsigned int i = 0; i < cand->getNHits(); i++) {
    cand->getHit(i, det, hit);
    std::cout << det << "/" << hit << " ";
  }
  std::cout << "\n";
}

ClassImp(PndMvdEventAnaTask);
