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

/**
 *@class PndFtsTrackFinderTask
 *@brief PandaRoot task providing PndFtsTrackFinder functionality.
 *@author Bartosz Sobol
 *@date: 18.11.2021
 */

#include <iostream>
#include <TSystem.h>

#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"

#include "PndTrack.h"

#include "PndFtsTrackFinderTask.h"
#include "PndFtsSimpleHit.h"

PndFtsTrackFinderTask::PndFtsTrackFinderTask(const TString &name)
  : PndPersistencyTask{name}, ioman{nullptr}, fInBranchId{}, fInBranchName{"FTSHit"}, fOutBranchName{"FtsTrack"}, fOutCandBranchName{"FtsTrackCand"},
    fOutAnalyticBranchName{"FtsTrackAnalytic"}, fMomEstMethod{PndFtsTrackFinder::PndFtsMomEstMethod::CURVATURE}, fInHits{nullptr}, fOutTrackCands{"PndTrackCand"},
    fOutTracks{"PndTrack"}, fOutAnalyticTracks("PndFtsTrackFinder::PndFtsAnalyticTrack"), fTmpHits{}, fTmpTracks{}, fReconContext{nullptr}, fReconRunner{nullptr},
    fPndTrackProducer{nullptr}
{
  SetPersistency(true);
}

PndFtsTrackFinderTask::~PndFtsTrackFinderTask() = default;

InitStatus PndFtsTrackFinderTask::Init()
{
  ioman = FairRootManager::Instance();
  if (ioman == nullptr) {
    LOG(fatal) << "PndFtsTrackFinderTask::Init: RootManager not instantiated." << std::endl;
    return kFATAL;
  }

  fInBranchId = ioman->GetBranchId(fInBranchName);

  fInHits = reinterpret_cast<TClonesArray *>(ioman->GetObject(fInBranchName));
  if (fInHits == nullptr) {
    LOG(fatal) << "PndFtsTrackFinderTask::Init: " << fInBranchName << " branch not existing." << std::endl;
    return kERROR;
  }

  ioman->Register(fOutBranchName, "FTS", &fOutTracks, GetPersistency());
  ioman->Register(fOutCandBranchName, "FTS", &fOutTrackCands, GetPersistency());
  ioman->Register(fOutAnalyticBranchName, "FTS", &fOutAnalyticTracks, GetPersistency());

  if (fVerbose > 0) {
    LOG(info) << "PndFtsTrackFinderTask::Init: Registering of output branches done.\n";
  }

  const std::string vmcWorkDir = gSystem->Getenv("VMCWORKDIR");
  PndFtsTrackFinder::PndFtsGeom geom = ReadRootGeom(vmcWorkDir + "/geometry/FtsReconGeom.root");
  fReconContext = std::make_unique<PndFtsTrackFinder::PndFtsContext>(geom, fVerbose, gDebug, fMomEstMethod);
  fReconRunner = std::make_unique<PndFtsTrackFinder::PndFtsReconRunner>(*fReconContext);
  fPndTrackProducer = std::make_unique<PndFtsTrackFinderPndTrackProducer>(*fReconContext, fInBranchId, fInHits);

  fTmpHits.reserve(100);

  if (fVerbose > 0) {
    LOG(info) << "PndFtsTrackFinderTask::Init: ReconRunner initialized.\n";
  }

  return kSUCCESS;
}

void PndFtsTrackFinderTask::Exec(Option_t *option)
{
  fTmpHits.clear();
  fOutTracks.Delete();
  fOutTrackCands.Delete();
  fOutAnalyticTracks.Delete();

  for (auto iHit = 0; iHit < fInHits->GetEntriesFast(); ++iHit) {
    fTmpHits.emplace_back((PndFtsHit *)fInHits->At(iHit), iHit);
  }

  const auto foundTracks = fReconRunner->Exec(fTmpHits);

  for (auto iTrack = 0; iTrack < foundTracks.size(); ++iTrack) {
    fPndTrackProducer->CreatePndTrack(foundTracks[iTrack], fOutTracks[iTrack], fOutTrackCands[iTrack], fOutAnalyticTracks[iTrack]);

    ((PndTrack *)fOutTracks[iTrack])->SetEntryNr(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), FairRootManager::Instance()->GetBranchId(fOutBranchName), iTrack));
    ((PndTrack *)fOutTracks[iTrack])->AddLink(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), FairRootManager::Instance()->GetBranchId(fOutCandBranchName), iTrack));
    ((PndTrack *)fOutTracks[iTrack])->AddLink(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), FairRootManager::Instance()->GetBranchId(fOutAnalyticBranchName), iTrack));
  }

  fOutAnalyticTracks.Sort();
  fOutTrackCands.Sort();
  fOutTracks.Sort();

  //  fTmpTracks.insert(fTmpTracks.cend(), foundTracks.cbegin(), foundTracks.cend());
}

void PndFtsTrackFinderTask::Finish()
{
  //  PrintTmpTracks();
  ioman->Write();
}

void PndFtsTrackFinderTask::SetOutputBranchName(const TString &name)
{
  fOutBranchName = name;
  fOutCandBranchName = name + "Cand";
  fOutAnalyticBranchName = name + "Analytic";
}

void PndFtsTrackFinderTask::SetMomentumEstimationMethod(PndFtsTrackFinder::PndFtsMomEstMethod momEstMethod)
{
  fMomEstMethod = momEstMethod;
}

PndFtsTrackFinder::PndFtsGeom PndFtsTrackFinderTask::ReadRootGeom(const std::string_view filename)
{
  PndFtsTrackFinder::GeomStrawsArrayT straws{};
  PndFtsTrackFinder::GeomLayersZArrayT layersZ{};
  PndFtsTrackFinder::PndFtsStraw tmpStraw{};

  TFile geomRootFile(filename.data(), "READ");
  TTree *geomTree = nullptr;
  geomRootFile.GetObject("PndFtsTrGeom", geomTree);

  auto strawBranch = geomTree->GetBranch("Straw");
  strawBranch->SetAddress(&tmpStraw);

  auto nStraws = geomTree->GetEntries();
  if (static_cast<std::size_t>(nStraws) != straws.size()) {
    throw std::runtime_error("Wrong geom straw count");
  }

  for (auto i = 0; i < nStraws; ++i) {
    geomTree->GetEntry(i);
    straws[i] = tmpStraw;
  }

  std::sort(straws.begin(), straws.end(), [](const PndFtsTrackFinder::PndFtsStraw &lhs, const PndFtsTrackFinder::PndFtsStraw &rhs) { return lhs.fId < rhs.fId; });

  for (std::size_t i = 0; i < straws.size(); i += 100) {
    layersZ[straws[i].fLayerId - 1] = straws[i].fZ;
  }

  geomRootFile.Close();

  return PndFtsTrackFinder::PndFtsGeom{straws, layersZ};
}

void PndFtsTrackFinderTask::PrintTmpTracks() const
{
  auto outFile = fopen("fts_track_cands", "a");
  fmt::print(outFile, "[TrackReconFull]: Found tracks: {}\n", fTmpTracks.size());
  for (const auto &track : fTmpTracks) {
    fmt::print(outFile, "FT12: {} {} {} {}\n", track.fFT12LineZOX.fSlope, track.fFT12LineZOX.fIntercept, track.fFT12LineZOY.fSlope, track.fFT12LineZOY.fIntercept);
    fmt::print(outFile, "FT56: {} {} {} {}\n", track.fFT56LineZOX.fSlope, track.fFT56LineZOX.fIntercept, track.fFT56LineZOY.fSlope, track.fFT56LineZOY.fIntercept);
    fmt::print(outFile, "FT34: {} {} {}; {} {}\n", track.fFT34CircleZOX.fRadius, track.fFT34CircleZOX.fOrigin.fX, track.fFT34CircleZOX.fOrigin.fZ, track.fFT34LineZOY.fSlope,
               track.fFT34LineZOY.fIntercept);
    fmt::print(outFile, "Straws: ");
    for (const auto &hit : track.fHits) {
      fmt::print(outFile, "{} ", hit.fStrawId);
    }
    fmt::print(outFile, "\n");
  }
}

ClassImp(PndFtsTrackFinderTask)