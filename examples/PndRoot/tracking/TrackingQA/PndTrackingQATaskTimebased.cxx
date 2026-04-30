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
// -----                PndTrackingQATaskTimebased source file             -----
// -----                  Created 18/07/08  by T.Stockmanns        -----
// -------------------------------------------------------------------------

/**
 * The quality numbers, qualityNumbers, as used here are defined in PndTrackingQualityAnalysis.h
 */

// C++ includes
#include <PndTrackingQATaskTimebased.h>
#include <PndTrackingCloneInfo.h>
#include <PndTrackingQATimebased.h>
#include <iostream>
#include <functional>

// ROOT includes
#include "TROOT.h"
#include "TClonesArray.h"
#include "THStack.h"

// FairRoot includes
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "FairMultiLinkedData.h"
#include "FairLogger.h"

// PandaRoot includes
#include "PndMCTrack.h"
#include "PndTrack.h"
#include "PndSttHit.h"
#include "PndSttTube.h"
#include "PndSttMapCreator.h"
#include "RhoTuple.h"

#include <chrono>

// Class includes

// -----   Default constructor   -------------------------------------------
PndTrackingQATaskTimebased::PndTrackingQATaskTimebased(TString trackBranchName, TString idealBranchName, Bool_t pndTrackData)
  : FairTask("Creates PndMC test"), fMCInfoBranchName("MCTrackInfo"), fRecoInfoBranchName("RecoTrackInfo"), fTrackBranchName(trackBranchName),
    fIdealTrackBranchName(idealBranchName), fPndTrackOrTrackCand(pndTrackData), fRunTimeBased(kFALSE), fEventNr(0), fSecondaryDefinitionPCAXY(kFALSE), fdPCA(0.1)
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndTrackingQATaskTimebased::~PndTrackingQATaskTimebased() {}

// -----   Public method Init   --------------------------------------------
InitStatus PndTrackingQATaskTimebased::Init()
{

  fSumTime = 0.0;

  InitializeHistograms();

  ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndTrackingQATaskTimebased::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  fTrack = (TClonesArray *)ioman->GetObject(fTrackBranchName);
  fMCTrack = (TClonesArray *)ioman->GetObject("MCTrack");
  fIdealTrack = (TClonesArray *)ioman->GetObject(fIdealTrackBranchName);
  fSttHitArray = (TClonesArray *)ioman->GetObject("STTHit");

  if (fTrack == nullptr) {
    std::cout << "-E- PndTrackingQATaskTimebased::Init "
              << "no track branch " << fTrackBranchName << std::endl;
    return kFATAL;
  }
  if (fMCTrack == nullptr) {
    std::cout << "-E- PndTrackingQATaskTimebased::Init "
              << "no MC track branch " << std::endl;
    return kFATAL;
  }

  if (fIdealTrack == nullptr) {
    std::cout << "-E- PndTrackingQATaskTimebased::Init "
              << "no ideal track branch " << fIdealTrackBranchName << std::endl;
    return kFATAL;
  }

  // MC info for quality
  fMCTrackInfo = new TClonesArray("PndTrackingQualityMCInfo");
  ioman->Register(fMCInfoBranchName, "QualityAssurance", fMCTrackInfo, kTRUE); // CHECK
  fRecoTrackInfo = new TClonesArray("PndTrackingQualityRecoInfo");
  ioman->Register(fRecoInfoBranchName, "QualityAssurance", fRecoTrackInfo, kTRUE); // CHECK

  fTuple = new RhoTuple("qaTuple", "QA Rho");

  if (fBranchNames.size() == 0) {
    AddHitsBranchName("MVDHitsPixel");
    AddHitsBranchName("MVDHitsStrip");
    AddHitsBranchName("STTHit");
    AddHitsBranchName("GEMHit");
    AddHitsBranchName("FTSHit");
    //	std::cout << "PndTrackingQualityAnalysis::Init() CorrectedSkewedHits present: " << FairRootManager::Instance()->GetBranchId("CorrectedSkewedHits") << " ";
    //	if (FairRootManager::Instance()->GetBranchId("CorrectedSkewedHits")  > 0){
    //		std::cout << "kTRUE";
    //		AddHitsBranchName("CorrectedSkewedHits");
    //	}
    //	std::cout << std::endl;
  }

  if (fPossibleTrackFunctorName.Length() == 0)
    fPossibleTrackFunctorName = "StandardTrackFunctor";

  SetFunctor();
  for (size_t i = 0; i < fBranchNames.size(); i++) {
    fMapEfficiencies[fBranchNames[i]] = new TH2D(fBranchNames[i], fBranchNames[i], 100, 0., 100., 50, 0, 1.1);
    fMapEfficiencies[fBranchNames[i]]->SetDrawOption("COLz");
  }

  // ----------------------------------------   maps of STT tubes
  PndSttMapCreator *mapperStt = new PndSttMapCreator(fSttParameters);
  fSttTubeArray = mapperStt->FillTubeArray();
  // ----------------------------------------------------  end map

  clonesTimeBasedTotal = 0.0;
  fTotClonesTimeBased = 0;
  //	std::cout << "-I- PndTrackingQATaskTimebased::Init: Initialization successfull" << std::endl;

  return kSUCCESS;
}

void PndTrackingQATaskTimebased::InitializeHistograms()
{
  fIdealTracksPerEvent = new TH1I("fIdealTracksPerEvent", "Ideal Tracks per Event", 1000, -0.5, 999.5);
  fIdealTracksPerEvent->GetXaxis()->SetTitle("Tracks/Event");
  fIdealPHisto = new TH1I("fIdealPHisto", "Ideal total Momentum", 1500, -0.5, 14.5);
  fIdealPHisto->GetXaxis()->SetTitle("p [GeV/c]");
  fIdealPtHisto = new TH1I("fIdealPtHisto", "Ideal Tansversal Momentum", 1500, -0.5, 14.5);
  fIdealPtHisto->GetXaxis()->SetTitle("p_{t} [GeV/c]");
  fIdealPlHisto = new TH1I("fIdealPlHisto", "Ideal Longitudinal Momentum", 1500, -0.5, 14.5);
  fIdealPlHisto->GetXaxis()->SetTitle("p_{l} [GeV/c]");
  fPHisto = new TH1D("fPHisto", "Momentum Resolution", 1000, -1, 1);
  fPHisto->GetXaxis()->SetTitle("p^{RECO} - p^{MC} / GeV");
  fPHisto->GetYaxis()->SetTitle("counts");
  fPRelHisto = new TH1D("fPRelHisto", "Relative Momentum Resolution", 1000, -1, 1);
  fPRelHisto->GetXaxis()->SetTitle("(p^{RECO} - p^{MC}) / p^{MC}");
  fPRelHisto->GetYaxis()->SetTitle("counts");
  fPtHisto = new TH1D("fPtHisto", "Transverse Momentum Resolution", 1000, -1, 1);
  fPtHisto->GetXaxis()->SetTitle("p_{t}^{RECO} - p_{t}^{MC} / GeV");
  fPtHisto->GetYaxis()->SetTitle("counts");
  fPtRelHisto = new TH1D("fPtRelHisto", "Relative Transverse Momentum Resolution", 1000, -1, 1);
  fPtRelHisto->GetXaxis()->SetTitle("(p_{t}^{RECO} - p_{t}^{MC}) / p_{t}^{MC}");
  fPtRelHisto->GetYaxis()->SetTitle("counts");
  fPlHisto = new TH1D("fPlHisto", "Longitudinal Momentum Resolution", 1000, -1, 1);
  fPlHisto->GetXaxis()->SetTitle("p_{l}^{RECO} - p_{l}^{MC} / GeV");
  fPlHisto->GetYaxis()->SetTitle("counts");
  fPlRelHisto = new TH1D("fPlRelHisto", "Relative Longitudinal Momentum Resolution", 1000, -1, 1);
  fPlRelHisto->GetXaxis()->SetTitle("(p_{l}^{RECO} - p_{l}^{MC}) / p_{l}^{MC}");
  fPlRelHisto->GetYaxis()->SetTitle("counts");

  fQualyHisto = new TH1I("fQualyHisto", "Quality of Trackfinding;;Counts", 26, -15.5, 10.5);
  fQualyHisto->SetDrawOption("TEXT HIST");
  LabelQualyHistogram(fQualyHisto);

  fQualyHisto_all = new TH1I("fQualyHisto_all", "Quality of Trackfinding;;Counts", 26, -15.5, 10.5);
  fQualyHisto_pos = new TH1I("fQualyHisto_pos", "Quality of Trackfinding;;Counts", 26, -15.5, 10.5);
  fQualyHisto_neg = new TH1I("fQualyHisto_neg", "Quality of Trackfinding;;Counts", 26, -15.5, 10.5);
  fQualyHisto_mc = new TH1I("fQualyHisto_mc", "Quality of Trackfinding;;Counts", 26, -15.5, 10.5);
  fQualyStack = new THStack();

  fQualyHisto_rel_all = new TH1D("fQualyHisto_rel_all", "Quality of Trackfinding;;Relative", 26, -15.5, 10.5);
  fQualyHisto_rel_all->SetBarWidth(0.45);
  fQualyHisto_rel_all->SetBarOffset(0.1);
  fQualyHisto_rel_all->SetFillColor(kBlue);

  fQualyHisto_rel_possible = new TH1D("fQualyHisto_rel_possible", "Quality of Trackfinding;;Relative", 26, -15.5, 10.5);
  fQualyHisto_rel_possible->SetBarWidth(0.4);
  fQualyHisto_rel_possible->SetBarOffset(0.55);
  fQualyHisto_rel_possible->SetFillColor(kRed);

  LabelQualyHistogram(fQualyHisto_rel_all);
  LabelQualyHistogram(fQualyHisto_rel_possible);

  if (fRunTimeBased) {
    fEventPurityHisto = new TH1D("fEventPurityHisto", "Event Purity of Track", 140, -1.2, 1.2);
  }
}

void PndTrackingQATaskTimebased::LabelQualyHistogram(TH1 *hist)
{
  hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kFullyPure), "Fully Purely found");
  hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kFullyImpure), "Fully Impurely found");
  hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kPartiallyPure), "Partially Purely found");
  hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kPartiallyImpure), "Partially Impurely found");
  hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kGhost), "Ghosts");
  hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kClone), "Clones");
  hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kNotFound), "Total not found");
  hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kFound), "Total found");
  hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kPossibleSec), "Possible, Sec.");
  hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kPossiblePrim), "Possible, Prim.");
  hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kAtLeastThreeSec), ">= 3 Hits, Sec.");
  hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kAtLeastThreePrim), ">= 3 Hits, Prim.");
  hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kLessThanThreePrim), "< 3 Hits, Prim.");
  hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kMcPossibleSec), "MC: Possible, Sec.");
  hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kMcPossiblePrim), "MC: Possible, Prim.");
  hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kMcAtLeastThreeSec), "MC: >= 3 Hits, Sec.");
  hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kMcAtLeastThreePrim), "MC: >= 3 Hits, Prim.");
  hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kMcLessThanThreePrim), "MC: < 3 Hits, Prim.");
}

// -------------------------------------------------------------------------
void PndTrackingQATaskTimebased::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRun::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
}

void PndTrackingQATaskTimebased::SetFunctor()
{
  fPossibleTrackFunctor = PndTrackFunctor::make_PndTrackFunctor(fPossibleTrackFunctorName.Data());
}

// -----   Public method Exec   --------------------------------------------
void PndTrackingQATaskTimebased::Exec(Option_t *)
{
  fMCTrackInfo->Delete();
  fRecoTrackInfo->Delete();

  if (fVerbose > 0 && !fRunTimeBased) {
    std::cout << "----- Running Event Based " << fEventNr << " ------" << std::endl;
    std::cout << "----- Event " << fEventNr << " ------" << std::endl;
  }
  if (fVerbose > 0 && fRunTimeBased) {
    std::cout << "----- Running Time based -----" << std::endl;
    std::cout << "----- Processing data chunk nr: " << fEventNr << " ------" << std::endl;
  }

  // auto start_time = std::chrono::system_clock::now();

  PndTrackingQA qaAna(fTrackBranchName, fIdealTrackBranchName, fPossibleTrackFunctor, fPndTrackOrTrackCand);
  qaAna.SetVerbose(fVerbose);
  qaAna.SetHitsBranchNames(fBranchNames);
  qaAna.SetRunTimeBased(fRunTimeBased);
  if (fSecondaryDefinitionPCAXY)
    qaAna.SetSecondaryDefinitionPCAXY(fdPCA);

  qaAna.Init();

  qaAna.AnalyseEvent(fRecoTrackInfo);

  if (fRunTimeBased) {

    std::vector<double> eventPurity = qaAna.GetEventPurityVector();

    for (int i_eventInfo = 0; i_eventInfo < eventPurity.size(); i_eventInfo++) {
      fEventPurityHisto->Fill(eventPurity[i_eventInfo]);
    }
  }

  if (!fRunTimeBased) {

    std::map<Int_t, Int_t> mcFoundMap = qaAna.GetMCTrackFound();
    std::map<Int_t, TVector3> recoPMap = qaAna.GetP();
    std::map<Int_t, Int_t> qualiMap = qaAna.GetTrackQualification();
    std::map<Int_t, Int_t> mcStatusMap = qaAna.GetTrackMCStatus();
    FillQualyHisto(qualiMap, qaAna.GetNGhosts(), qaAna.GetNClones());
    FillMCStatus(mcStatusMap);

    for (std::map<Int_t, Int_t>::iterator iter = qualiMap.begin(); iter != qualiMap.end(); iter++) {
      Int_t mcTrackId = iter->first;

      Int_t idealTrackId = qaAna.GetIdealTrackIdFromMCTrackId(mcTrackId);

      if (idealTrackId < 0) {
        LOG(warn) << " PndTrackingQATaskTimebased::Exec no idealTrack for mcTrack " << mcTrackId;
        continue;
      }
      Int_t trackQuality = iter->second;

      PndTrack *idealtrack = (PndTrack *)fIdealTrack->At(idealTrackId);

      if (idealtrack == 0) {
        LOG(error) << " No ideal track found for idealTrackId " << idealTrackId;
        continue;
      }
      if (mcTrackId == -1) {
        LOG(warn) << " PndTrackingQATaskTimebased::Exec mcTrackId == -1";
        continue;
      }

      PndMCTrack *myMcTrack = (PndMCTrack *)fMCTrack->At(mcTrackId);

      if (myMcTrack == 0) {
        LOG(error) << " PndTrackingQATaskTimebased::Exec mcMyTrack == 0";
        continue;
      }
      Int_t pdgId = myMcTrack->GetPdgCode();

      int size = fMCTrackInfo->GetEntriesFast();
      PndTrackingQualityMCInfo mctrackinfo = GetMCInfoFromIdealTrack(idealtrack);
      mctrackinfo.SetMCTrackID(mcTrackId);
      mctrackinfo.SetQuality(trackQuality);
      mctrackinfo.SetPDGCode(pdgId);
      mctrackinfo.SetMomentum(myMcTrack->GetMomentum());
      mctrackinfo.SetMCQuality(mcStatusMap[mcTrackId]);
      //  mctrackinfo.SetReconstructabilityStatus();

      if (mctrackinfo.GetNofMCPoints() > 0) {
        new ((*fMCTrackInfo)[size]) PndTrackingQualityMCInfo(mctrackinfo);
        fMCInfoIdIdealId[idealTrackId] = size;
      }
    }

    for (int itrk = 0; itrk < fRecoTrackInfo->GetEntriesFast(); itrk++) {
      PndTrackingQualityRecoInfo *recoinfo = (PndTrackingQualityRecoInfo *)fRecoTrackInfo->At(itrk);
      Int_t idealTrackId = qaAna.GetIdealTrackIdFromRecoTrackId(recoinfo->GetRecoTrackID()); // This does not work time based
      recoinfo->SetIdealTrackId(idealTrackId);
      if (idealTrackId < 0) {
        LOG(warn) << " PndTrackingQATaskTimebased::Exec no idealTrack for recoTrack " << itrk;
        continue;
      }

      PndTrackingQualityMCInfo *mctrackinfo = (PndTrackingQualityMCInfo *)fMCTrackInfo->At(
        GetMCInfoIdFromIdealTrackId(idealTrackId)); // This function probably need to be replaced all together for the time based reconstruction in trackingQA
      if (!mctrackinfo)
        continue;
      recoinfo->SetMCTrackInfo(mctrackinfo);
    }
    // associate reconstructed and mc tracks
    AssociateRecoTracksToMCTracks();

    // Save the Tree (/RhoTuple) for some possible additional analysis
    for (std::map<Int_t, Int_t>::iterator iter = qualiMap.begin(); iter != qualiMap.end(); iter++) {
      Int_t mcTrackId = iter->first;
      Int_t trackQuality = iter->second;

      if (mcTrackId == -1) {
        LOG(warn) << " PndTrackingQATaskTimebased::Exec mcTrackId == -1";
        continue;
      }

      TVector3 recoMomentum = recoPMap[mcTrackId];

      PndMCTrack *myMcTrack = (PndMCTrack *)fMCTrack->At(mcTrackId);

      if (myMcTrack == 0) {
        LOG(error) << " PndTrackingQATaskTimebased::Exec mcMyTrack == 0";
        continue;
      }
      TVector3 mcMomentum = myMcTrack->GetMomentum();
      Int_t pdgId = myMcTrack->GetPdgCode();

      fTuple->Column("EvtNr", (Int_t)fEventNr); // number of the currently processed event

      fTuple->Column("McTrackId", (Int_t)mcTrackId);                        // id of the currently processed track
      fTuple->Column("McTrackFoundNTimes", (Int_t)mcFoundMap[mcTrackId]);   // A given MC track was found N times
      fTuple->Column("TrackQuality", (Int_t)trackQuality);                  // the quality of the current track
      fTuple->Column("Mc_TrackQuality", (Int_t)mcStatusMap[mcTrackId] - 6); // the mc quality of the current track, -6 to match the global qualityNumbers

      fTuple->Column("PdgId", (Int_t)pdgId);
      fTuple->Column("Mc_px", (Double_t)mcMomentum.Px()); // To compare to the additionally saved pt histogram, call Draw("Reco_pt:Mc_pt","TrackQuality > 0") on the Tuple
      fTuple->Column("Mc_py", (Double_t)mcMomentum.Py());
      fTuple->Column("Mc_pz", (Double_t)mcMomentum.Pz());
      fTuple->Column("Mc_pt", (Double_t)mcMomentum.Pt());
      fTuple->Column("Reco_px", (Double_t)recoMomentum.Px());
      fTuple->Column("Reco_py", (Double_t)recoMomentum.Py());
      fTuple->Column("Reco_pz", (Double_t)recoMomentum.Pz());
      fTuple->Column("Reco_pt", (Double_t)recoMomentum.Pt());
      //		fTuple->Column("Reco_pt2", (Double_t) recoPtMap[mcTrackId]);  // cross check

      fTuple->DumpData();
    }

    if (fVerbose > 0)
      qaAna.PrintTrackQualityMap();

    FillEfficiencies(qaAna.GetEfficiencies());

    MapToHist(qaAna.GetPResolution(), fPHisto);
    MapToHist(qaAna.GetPResolutionRel(), fPRelHisto);
    MapToHist(qaAna.GetPtResolution(), fPtHisto);
    MapToHist(qaAna.GetPtResolutionRel(), fPtRelHisto);
    MapToHist(qaAna.GetPlResolution(), fPlHisto);
    MapToHist(qaAna.GetPlResolutionRel(), fPlRelHisto);
  }

  if (fRunTimeBased) {
    std::map<FairLink, Int_t> mcFoundMap = qaAna.GetMCTrackFoundTimeBased();
    std::map<FairLink, TVector3> recoPMap = qaAna.GetTimeBasedP();
    std::map<FairLink, Int_t> qualiMap = qaAna.GetTrackQualificationTimeBased();
    std::map<FairLink, Int_t> mcStatusMap = qaAna.GetTrackMCStatusTimeBased();

    PndTrackingCloneInfo *cloneInfo = new PndTrackingCloneInfo(mcFoundMap, fMCTrack);
    cloneInfo->CalcNumClones(mcFoundMap, fMCTrack);
    fTotClonesTimeBased += cloneInfo->GetNumClones();

    FillQualyHistoTimeBased(qualiMap, qaAna.GetNGhosts());
    FillMCStatusTimeBased(mcStatusMap);

    for (std::map<FairLink, Int_t>::iterator iter = qualiMap.begin(); iter != qualiMap.end(); iter++) {
      FairLink mcTrackId = iter->first;

      FairLink idealTrackId = qaAna.GetIdealTrackFairLinkFromMCTrackFairLink(mcTrackId);

      Int_t trackQuality = iter->second;

      PndMCTrack *myMcTrack = (PndMCTrack *)FairRootManager::Instance()->GetCloneOfLinkData(mcTrackId);
      PndTrack *idealtrack = (PndTrack *)FairRootManager::Instance()->GetCloneOfLinkData(idealTrackId);

      if (myMcTrack == 0) {
        LOG(error) << " PndTrackingQATaskTimebased::Exec mcMyTrack == 0";
        continue;
      }
      if (idealtrack == 0) {
        LOG(error) << " PndTrackingQATaskTimebased::Exec idealtrack == 0";
        continue;
      }

      Int_t pdgId = myMcTrack->GetPdgCode();

      int size = fMCTrackInfo->GetEntriesFast();
      PndTrackingQualityMCInfo mctrackinfo = GetMCInfoFromIdealTrack(idealtrack);
      // TODO The line below might need to be taken back for time based case
      // mctrackinfo.SetMCTrackID(mcTrackId);
      mctrackinfo.SetQuality(trackQuality);
      mctrackinfo.SetPDGCode(pdgId);
      mctrackinfo.SetMomentum(myMcTrack->GetMomentum());
      mctrackinfo.SetMCQuality((Int_t)mcStatusMap[mcTrackId]);
      // mctrackinfo.SetReconstructabilityStatus();

      if (mctrackinfo.GetNofMCPoints() > 0) {
        new ((*fMCTrackInfo)[size]) PndTrackingQualityMCInfo(mctrackinfo);
        fTimeBasedMCInfoIdIdealId[idealTrackId] = size;
      }
      //     cout << "MCTRack " << mctrackinfo.GetMCTrackID() << endl;
    }
    // loop over reco track info and associate the mc track info
    for (int itrk = 0; itrk < fRecoTrackInfo->GetEntriesFast(); itrk++) {
      PndTrackingQualityRecoInfo *recoinfo = (PndTrackingQualityRecoInfo *)fRecoTrackInfo->At(itrk);
      Int_t idealTrackId = qaAna.GetIdealTrackIdFromRecoTrackId(recoinfo->GetRecoTrackID()); // This does not work time based
      recoinfo->SetIdealTrackId(idealTrackId);                                               // This approach probably needs to be replaced for the time based implementation
      if (idealTrackId < 0) {
        LOG(warn) << " PndTrackingQATaskTimebased::Exec no idealTrack for recoTrack " << itrk;
        continue;
      }

      PndTrackingQualityMCInfo *mctrackinfo = (PndTrackingQualityMCInfo *)fMCTrackInfo->At(
        GetMCInfoIdFromIdealTrackId(idealTrackId)); // This function probably need to be replaced all together for the time based reconstruction in trackingQA

      recoinfo->SetMCTrackInfo(mctrackinfo);
    }
    // associate reconstructed and mc tracks
    AssociateRecoTracksToMCTracks();

    // Save the Tree (/RhoTuple) for some possible additional analysis
    for (std::map<FairLink, Int_t>::iterator iter = qualiMap.begin(); iter != qualiMap.end(); iter++) {

      FairLink mcTrackId = iter->first;
      Int_t trackQuality = iter->second;

      TVector3 recoMomentum = recoPMap[mcTrackId];

      PndMCTrack *myMcTrack = (PndMCTrack *)FairRootManager::Instance()->GetCloneOfLinkData(mcTrackId);

      if (myMcTrack == 0) {
        LOG(error) << " PndTrackingQATaskTimebased::Exec mcMyTrack == 0";
        continue;
      }
      TVector3 mcMomentum = myMcTrack->GetMomentum();
      Int_t pdgId = myMcTrack->GetPdgCode();

      fTuple->Column("EvtNr", (Int_t)fEventNr); // number of the currently processed event

      // fTuple->Column("McTrackId", (Int_t) mcTrackId);  // id of the currently processed track
      fTuple->Column("McTrackFoundNTimes", (Int_t)mcFoundMap[mcTrackId]);   // A given MC track was found N times
      fTuple->Column("TrackQuality", (Int_t)trackQuality);                  // the quality of the current track
      fTuple->Column("Mc_TrackQuality", (Int_t)mcStatusMap[mcTrackId] - 6); // the mc quality of the current track, -6 to match the global qualityNumbers
      fTuple->Column("PdgId", (Int_t)pdgId);
      fTuple->Column("Mc_px", (Double_t)mcMomentum.Px()); // To compare to the additionally saved pt histogram, call Draw("Reco_pt:Mc_pt","TrackQuality > 0") on the Tuple
      fTuple->Column("Mc_py", (Double_t)mcMomentum.Py());
      fTuple->Column("Mc_pz", (Double_t)mcMomentum.Pz());
      fTuple->Column("Mc_pt", (Double_t)mcMomentum.Pt());
      fTuple->Column("Reco_px", (Double_t)recoMomentum.Px());
      fTuple->Column("Reco_py", (Double_t)recoMomentum.Py());
      fTuple->Column("Reco_pz", (Double_t)recoMomentum.Pz());
      fTuple->Column("Reco_pt", (Double_t)recoMomentum.Pt());
      // fTuple->Column("Reco_pt2", (Double_t) recoPtMap[mcTrackId]);  // cross check

      fTuple->DumpData();
    }

    if (fVerbose > 0)
      qaAna.PrintTrackQualityMap();

    FillEfficienciesTimeBased(qaAna.GetEfficienciesTimeBased());

    MapToHistTimeBased(qaAna.GetTimeBasedPResolution(), fPHisto);
    MapToHistTimeBased(qaAna.GetTimeBasedPResolutionRel(), fPRelHisto);
    MapToHistTimeBased(qaAna.GetTimeBasedPtResolution(), fPtHisto);
    MapToHistTimeBased(qaAna.GetTimeBasedPtResolutionRel(), fPtRelHisto);
    MapToHistTimeBased(qaAna.GetTimeBasedPlResolution(), fPlHisto);
    MapToHistTimeBased(qaAna.GetTimeBasedPlResolutionRel(), fPlRelHisto);
  }

  // The below works both time based and event based
  for (int i = 0; i < fIdealTrack->GetEntries(); i++) {
    PndTrack *myTrack = (PndTrack *)fIdealTrack->At(i);
    fIdealPHisto->Fill(myTrack->GetParamFirst().GetMomentum().Mag());
    fIdealPtHisto->Fill(myTrack->GetParamFirst().GetMomentum().Pt());
    fIdealPlHisto->Fill(myTrack->GetParamFirst().GetMomentum().Pz());
  }

  fIdealTracksPerEvent->Fill(fIdealTrack->GetEntries());

  fEventNr++;
}

Int_t PndTrackingQATaskTimebased::GetSumOfAllValidMCHits(FairMultiLinkedData *trackData)
{
  Int_t result = 0;
  for (size_t branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++) {
    result += trackData->GetLinksWithType(ioman->GetBranchId(fBranchNames[branchIndex])).GetNLinks();
  }
  return result;
}

void PndTrackingQATaskTimebased::FillQualyHisto(std::map<Int_t, Int_t> trackQualifikation, Int_t nGhosts, Int_t nClones)
{

  fQualyHisto->Fill(qualityNumbers::kGhost, nGhosts);
  fQualyHisto->Fill(qualityNumbers::kClone, nClones);

  for (std::map<Int_t, Int_t>::iterator iter = trackQualifikation.begin(); iter != trackQualifikation.end(); iter++) {
    fQualyHisto->Fill(iter->second);
    if (iter->second > 0) {
      fQualyHisto->Fill(qualityNumbers::kFound);
    } else {
      fQualyHisto->Fill(qualityNumbers::kNotFound);
    }
  }
}

void PndTrackingQATaskTimebased::FillQualyHistoTimeBased(std::map<FairLink, Int_t> trackQualifikation, Int_t nGhosts)
{

  fQualyHisto->Fill(qualityNumbers::kGhost, nGhosts);
  for (std::map<FairLink, Int_t>::iterator iter = trackQualifikation.begin(); iter != trackQualifikation.end(); iter++) {
    fQualyHisto->Fill(iter->second);
    if (iter->second > 0) {
      fQualyHisto->Fill(qualityNumbers::kFound);
    } else {
      fQualyHisto->Fill(qualityNumbers::kNotFound);
    }
  }
}

void PndTrackingQATaskTimebased::FillMCStatus(std::map<Int_t, Int_t> trackMCStatus)
{
  int mcOffset = qualityNumbers::kPossibleSec - qualityNumbers::kMcPossibleSec;
  for (std::map<Int_t, Int_t>::iterator iter = trackMCStatus.begin(); iter != trackMCStatus.end(); iter++) {
    fQualyHisto->Fill(iter->second - mcOffset); // mcOffset = 6
  }
}
void PndTrackingQATaskTimebased::FillMCStatusTimeBased(std::map<FairLink, Int_t> trackMCStatus)
{
  int mcOffset = qualityNumbers::kPossibleSec - qualityNumbers::kMcPossibleSec;
  for (std::map<FairLink, Int_t>::iterator iter = trackMCStatus.begin(); iter != trackMCStatus.end(); iter++) {
    fQualyHisto->Fill(iter->second - mcOffset); // mcOffset = 6
  }
}

void PndTrackingQATaskTimebased::FillEfficiencies(std::map<Int_t, std::map<TString, std::pair<Double_t, Int_t>>> efficiencies)
{
  for (std::map<Int_t, std::map<TString, std::pair<Double_t, Int_t>>>::iterator iterTracks = efficiencies.begin(); iterTracks != efficiencies.end(); iterTracks++) {
    std::map<TString, std::pair<Double_t, Int_t>> branchEfficiency = iterTracks->second;
    for (std::map<TString, std::pair<Double_t, Int_t>>::iterator iterBranch = branchEfficiency.begin(); iterBranch != branchEfficiency.end(); iterBranch++) {
      fMapEfficiencies[iterBranch->first]->Fill(iterBranch->second.second, iterBranch->second.first);
    }
  }
}
void PndTrackingQATaskTimebased::FillEfficienciesTimeBased(std::map<FairLink, std::map<TString, std::pair<Double_t, Int_t>>> efficiencies)
{
  for (std::map<FairLink, std::map<TString, std::pair<Double_t, Int_t>>>::iterator iterTracks = efficiencies.begin(); iterTracks != efficiencies.end(); iterTracks++) {
    std::map<TString, std::pair<Double_t, Int_t>> branchEfficiency = iterTracks->second;
    for (std::map<TString, std::pair<Double_t, Int_t>>::iterator iterBranch = branchEfficiency.begin(); iterBranch != branchEfficiency.end(); iterBranch++) {
      fMapEfficiencies[iterBranch->first]->Fill(iterBranch->second.second, iterBranch->second.first);
    }
  }
}

void PndTrackingQATaskTimebased::MapToHist(std::map<Int_t, Double_t> map, TH1 *histo)
{
  for (std::map<Int_t, Double_t>::iterator iter = map.begin(); iter != map.end(); iter++) {
    histo->Fill(iter->second);
  }
}
void PndTrackingQATaskTimebased::MapToHistTimeBased(std::map<FairLink, Double_t> map, TH1 *histo)
{
  for (std::map<FairLink, Double_t>::iterator iter = map.begin(); iter != map.end(); iter++) {
    histo->Fill(iter->second);
  }
}

// The function below works fine for time based data
void PndTrackingQATaskTimebased::SetQualyHisto(TH1 *histo, Bool_t relative, Int_t base)
{
  Int_t allTracks = 0;
  Int_t allTracksWithHits = 0;
  Int_t allPossibleTracksWithHits = 0;
  Int_t allTracksWithHitsNotFound = 0;

  Int_t mcLessThanThreePrim = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcLessThanThreePrim));
  Int_t mcAtLeastThreePrim = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcAtLeastThreePrim));
  Int_t mcAtLeastThreeSec = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcAtLeastThreeSec));
  Int_t mcPossiblePrim = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcPossiblePrim));
  Int_t mcPossibleSec = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcPossibleSec));

  Int_t lessThanThreePrim = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kLessThanThreePrim));
  Int_t atLeastThreePrim = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kAtLeastThreePrim));
  Int_t atLeastThreeSec = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kAtLeastThreeSec));
  Int_t possiblePrim = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPossiblePrim));
  Int_t possibleSec = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPossibleSec));

  Double_t FullyPure = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kFullyPure));
  Double_t FullyImpure = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kFullyImpure));
  Double_t PartiallyPure = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPartiallyPure));
  Double_t PartiallyImpure = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPartiallyImpure));
  Double_t allFound = FullyPure + FullyImpure + PartiallyPure + PartiallyImpure;

  Double_t ghosts = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kGhost));

  allTracksWithHits += mcAtLeastThreePrim; // Todo
  allTracksWithHits += mcAtLeastThreeSec;
  allTracksWithHits += mcPossiblePrim;
  allTracksWithHits += mcPossibleSec;

  allTracks = allTracksWithHits + lessThanThreePrim;

  allPossibleTracksWithHits += mcPossiblePrim;
  allPossibleTracksWithHits += mcPossibleSec;

  allTracksWithHitsNotFound += atLeastThreePrim;
  allTracksWithHitsNotFound += atLeastThreeSec;
  allTracksWithHitsNotFound += possiblePrim;
  allTracksWithHitsNotFound += possibleSec;

  Double_t divisor = 1.0;
  if (relative == kTRUE) {
    divisor = allTracks / 100.0;
  }

  histo->Fill(qualityNumbers::kMcAllTracksWithHits, (Double_t)allTracksWithHits / divisor);
  histo->Fill(qualityNumbers::kMcLessThanThreePrim, (Double_t)mcLessThanThreePrim / divisor);
  histo->Fill(qualityNumbers::kMcAtLeastThreePrim, (Double_t)mcAtLeastThreePrim / divisor);
  histo->Fill(qualityNumbers::kMcAtLeastThreeSec, (Double_t)mcAtLeastThreeSec / divisor);
  histo->Fill(qualityNumbers::kMcPossiblePrim, (Double_t)mcPossiblePrim / divisor);
  histo->Fill(qualityNumbers::kMcPossibleSec, (Double_t)mcPossibleSec / divisor);

  divisor = 1.0;

  if (mcLessThanThreePrim > 0) {
    if (relative == kTRUE) {
      divisor = mcLessThanThreePrim / 100.0;
    }
    histo->Fill(qualityNumbers::kLessThanThreePrim, (Double_t)(mcLessThanThreePrim - lessThanThreePrim) / divisor);
  }

  divisor = 1.0;

  if (mcAtLeastThreePrim > 0) {
    if (relative == kTRUE) {
      divisor = mcAtLeastThreePrim / 100.0;
    }
    histo->Fill(qualityNumbers::kAtLeastThreePrim, (Double_t)(mcAtLeastThreePrim - atLeastThreePrim) / divisor);
  }

  divisor = 1.0;

  if (mcAtLeastThreeSec > 0) {
    if (relative == kTRUE) {
      divisor = mcAtLeastThreeSec / 100.0;
    }
    histo->Fill(qualityNumbers::kAtLeastThreeSec, (Double_t)(Double_t)(mcAtLeastThreeSec - atLeastThreeSec) / divisor);
  }

  divisor = 1.0;

  if (mcPossiblePrim > 0) {
    if (relative == kTRUE) {
      divisor = mcPossiblePrim / 100.0;
    }
    histo->Fill(qualityNumbers::kPossiblePrim, (Double_t)(mcPossiblePrim - possiblePrim) / divisor);
  }

  divisor = 1.0;

  if (mcPossibleSec > 0) {
    if (relative == kTRUE) {
      divisor = mcPossibleSec / 100.0;
    }
    histo->Fill(qualityNumbers::kPossibleSec, (Double_t)(mcPossibleSec - possibleSec) / divisor);
  }

  Double_t baseDouble = base;
  if (relative == kTRUE) {
    baseDouble /= 100.0;
  }

  if (base == 0)
    return;

  histo->Fill(qualityNumbers::kFullyPure, FullyPure / baseDouble);
  histo->Fill(qualityNumbers::kFullyImpure, FullyImpure / baseDouble);
  histo->Fill(qualityNumbers::kPartiallyPure, PartiallyPure / baseDouble);
  histo->Fill(qualityNumbers::kPartiallyImpure, PartiallyImpure / baseDouble);
  histo->Fill(qualityNumbers::kGhost, ghosts / baseDouble);
  histo->Fill(qualityNumbers::kFound, allFound / baseDouble);
  histo->Fill(qualityNumbers::kNotFound, (base - allFound) / baseDouble);
}

// This function works time based
void PndTrackingQATaskTimebased::Finish()
{
  ColorHistogram();
  fQualyStack->Add(fQualyHisto_mc);
  fQualyStack->Add(fQualyHisto_neg);
  fQualyStack->Add(fQualyHisto_pos);
  fQualyStack->Add(fQualyHisto_all);
  fQualyStack->SetName("fQualyHistoColor");
  fQualyStack->SetTitle(fQualyHisto->GetTitle());

  // gROOT->SetBatch(kTRUE);
  // fQualyHisto->Draw();
  // LabelQualyHistogram((TH1*) fQualyStack);

  for (auto effmap : fMapEfficiencies) {
    effmap.second->Write();
  }

  fIdealTracksPerEvent->Write();
  fIdealPHisto->Write();
  fIdealPtHisto->Write();
  fIdealPlHisto->Write();

  fPHisto->Write();
  fPRelHisto->Write();
  fPtHisto->Write();
  fPtRelHisto->Write();
  fPlHisto->Write();
  fPlRelHisto->Write();
  fQualyHisto->Write();
  fQualyStack->Write();

  Int_t allTracks = 0;
  Int_t allTracksWithHits = 0;
  Int_t allPossibleTracksWithHits = 0;
  Int_t allTracksWithHitsNotFound = 0;

  Int_t mcLessThanThreePrim = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcLessThanThreePrim));
  Int_t mcAtLeastThreePrim = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcAtLeastThreePrim));
  Int_t mcAtLeastThreeSec = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcAtLeastThreeSec));
  Int_t mcPossiblePrim = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcPossiblePrim));
  Int_t mcPossibleSec = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcPossibleSec));

  Int_t lessThanThreePrim = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kLessThanThreePrim));
  Int_t atLeastThreePrim = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kAtLeastThreePrim));
  Int_t atLeastThreeSec = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kAtLeastThreeSec));
  Int_t possiblePrim = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPossiblePrim));
  Int_t possibleSec = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPossibleSec));

  allTracksWithHits += mcAtLeastThreePrim; // Todo
  allTracksWithHits += mcAtLeastThreeSec;
  allTracksWithHits += mcPossiblePrim;
  allTracksWithHits += mcPossibleSec;

  allTracks = allTracksWithHits + lessThanThreePrim;

  allPossibleTracksWithHits += mcPossiblePrim;
  allPossibleTracksWithHits += mcPossibleSec;

  allTracksWithHitsNotFound += atLeastThreePrim;
  allTracksWithHitsNotFound += atLeastThreeSec;
  allTracksWithHitsNotFound += possiblePrim;
  allTracksWithHitsNotFound += possibleSec;

  Double_t FullyPure = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kFullyPure));
  Double_t FullyImpure = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kFullyImpure));
  Double_t PartiallyPure = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPartiallyPure));
  Double_t PartiallyImpure = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPartiallyImpure));

  // The three lines below are only used time-based
  PndTrackingCloneInfo *cloneInfo = new PndTrackingCloneInfo(fTimeBasedMapTrackMCStatusForCloneCalc, fMCTrack);
  cloneInfo->CalcNumClones(fTimeBasedMapTrackMCStatusForCloneCalc, fMCTrack);
  Double_t clonesTimeBased = cloneInfo->GetNumClones();
  clonesTimeBasedTotal = clonesTimeBasedTotal + clonesTimeBased;

  // Double_t allFound = FullyPure + FullyImpure + PartiallyPure + PartiallyImpure; //[R.K.04/2017] unused variable
  // Double_t notFound = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kNotFound)); //[R.K.03/2017] unused variable

  Double_t ghosts = fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kGhost));

  if (!fRunTimeBased) {
    std::cout << "------ Results Event Based trackingQA ------" << std::endl;
  }
  if (fRunTimeBased) {
    std::cout << "------ Results Time Based trackingQA ------" << std::endl;
  }

  std::cout << "fQualyHisto: All Tracks: " << allTracks << std::endl
            << " Primary Tracks < 3 hits: " << mcLessThanThreePrim << ": Not Found: " << lessThanThreePrim << std::endl
            << " All Tracks with hits: " << allTracksWithHits << ". Not Found: " << allTracksWithHitsNotFound << std::endl
            << " Primary Tracks with >= 3 hits, but not a possible track: " << mcAtLeastThreePrim << ". Not Found: " << atLeastThreePrim << std::endl
            << " Secondary Tracks with >= 3 hits, but not a possible track: " << mcAtLeastThreeSec << ". Not Found: " << atLeastThreeSec << std::endl
            << " Primary Tracks possible: " << mcPossiblePrim << ". Not Found: " << possiblePrim << " rel: " << (mcPossiblePrim - possiblePrim) * 100. / mcPossiblePrim << std::endl
            << " Secondary Tracks possible: " << mcPossibleSec << ". Not Found: " << possibleSec << " rel: " << (mcPossibleSec - possibleSec) * 100. / mcPossibleSec << std::endl
            << " All Possible Tracks with hits: " << allPossibleTracksWithHits << std::endl

            << " FullyPure: " << FullyPure << " " << FullyPure / allTracksWithHits * 100.0 << "% (of all tracks), " << FullyPure / allPossibleTracksWithHits * 100.0
            << "% (of all tracks possible)"

            << " FullyImpure: " << FullyImpure << " " << FullyImpure / allTracksWithHits * 100.0 << "% (of all tracks), " << FullyImpure / allPossibleTracksWithHits * 100.0
            << "% (of all tracks possible)"

            << " PartiallyPureFound: " << PartiallyPure << " " << PartiallyPure / allTracksWithHits * 100.0 << "% " << PartiallyPure / allPossibleTracksWithHits * 100.0 << "% "

            << " PartiallyImpureFound: " << PartiallyImpure << " " << PartiallyImpure / allTracksWithHits * 100.0 << "% " << PartiallyImpure / allPossibleTracksWithHits * 100.0
            << "% "

            << " Ghosts: " << ghosts << " " << ghosts / allTracksWithHits * 100.0 << "% " << ghosts / allPossibleTracksWithHits * 100.0 << "% " << std::endl;

  if (!fRunTimeBased) {
    std::cout << "Clones: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kClone)) << " "
              << (Double_t)fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kClone)) / allTracksWithHits * 100.0 << "% (of all tracks) " << std::endl;
  }
  if (fRunTimeBased) {
    // Writes out the time based clones as obtained from PndTrackingCloneInfo.cxx
    std::cout << "Clones: " << fTotClonesTimeBased << " " << (Double_t)fTotClonesTimeBased / allTracksWithHits * 100.0 << "% (of all tracks) " << std::endl;
  }

  SetQualyHisto(fQualyHisto_rel_all, kTRUE, allTracksWithHits);
  SetQualyHisto(fQualyHisto_rel_possible, kTRUE, allPossibleTracksWithHits);

  fQualyHisto->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcAllTracksWithHits), allTracksWithHits);
  fQualyHisto->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kLessThanThreePrim), mcLessThanThreePrim - lessThanThreePrim);
  fQualyHisto->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kAtLeastThreePrim), mcAtLeastThreePrim - atLeastThreePrim);
  fQualyHisto->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kAtLeastThreeSec), mcAtLeastThreeSec - atLeastThreeSec);
  fQualyHisto->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPossiblePrim), mcPossiblePrim - possiblePrim);
  fQualyHisto->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPossibleSec), mcPossibleSec - possibleSec);

  fQualyHisto_rel_all->Write();
  fQualyHisto_rel_possible->Write();
  fQualyHisto->Write();

  if (fRunTimeBased) {
    fEventPurityHisto->Write();
  }

  fTuple->GetInternalTree()->Write();

  std::cout << "Finish finished!" << std::endl;
}

// Fine for time based data
void PndTrackingQATaskTimebased::ColorHistogram()
{
  fQualyHisto_pos->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kGhost), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kGhost)));
  fQualyHisto_pos->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPartiallyImpure), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPartiallyImpure)));
  fQualyHisto_pos->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPartiallyPure), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPartiallyPure)));
  fQualyHisto_pos->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kFullyPure), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kFullyPure)));
  fQualyHisto_pos->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kFullyImpure), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kFullyImpure)));
  fQualyHisto_pos->SetLineColor(kGreen + 2);

  fQualyHisto_all->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kFound), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kFound)));
  fQualyHisto_all->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kNotFound), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kNotFound)));
  fQualyHisto_all->SetLineColor(kBlack);

  fQualyHisto_neg->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPossibleSec), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPossibleSec)));
  fQualyHisto_neg->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPossiblePrim), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPossiblePrim)));
  fQualyHisto_neg->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kAtLeastThreeSec), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kAtLeastThreeSec)));
  fQualyHisto_neg->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kAtLeastThreePrim),
                                 fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kAtLeastThreePrim)));
  fQualyHisto_neg->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kLessThanThreePrim),
                                 fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kLessThanThreePrim)));
  fQualyHisto_neg->SetLineColor(kRed + 2);

  fQualyHisto_mc->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcPossibleSec), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcPossibleSec)));
  fQualyHisto_mc->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcPossiblePrim), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcPossiblePrim)));
  fQualyHisto_mc->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcAtLeastThreeSec),
                                fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcAtLeastThreeSec)));
  fQualyHisto_mc->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcAtLeastThreePrim),
                                fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcAtLeastThreePrim)));
  fQualyHisto_mc->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcLessThanThreePrim),
                                fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcLessThanThreePrim)));
  fQualyHisto_mc->SetLineColor(kBlue);
}

// This function works time based
PndTrackingQualityMCInfo PndTrackingQATaskTimebased::GetMCInfoFromIdealTrack(PndTrack *idealtrack)
{

  PndTrackCand *idealtrkcand = idealtrack->GetTrackCandPtr(); // Ok time based

  // Int_t nofsttpoint = idealtrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId("STTHit")); //[R.K. 01/2017] unused variable
  Int_t nofmvdpixpoint = idealtrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId("MVDHitsPixel"));
  Int_t nofmvdstrpoint = idealtrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId("MVDHitsStrip"));
  // Int_t nofmvdpoint = nofmvdpixpoint + nofmvdstrpoint; //[R.K. 01/2017] unused variable
  Int_t nofgempoint = idealtrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId("GEMHit"));
  Int_t nofftspoint = idealtrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId("FTSHit"));

  int nofsttskewpoint = 0, nofsttparalpoint = 0;

  if (!fRunTimeBased) {
    // This loop works event based
    // this loop counts skewed (--> parallel) STT/FTS hits
    for (size_t ihit = 0; ihit < idealtrkcand->GetNHits(); ihit++) {
      PndTrackCandHit idealcandhit = idealtrkcand->GetSortedHit(ihit);
      Int_t hitID = idealcandhit.GetHitId();
      Int_t detID = idealcandhit.GetDetId();

      if (detID != FairRootManager::Instance()->GetBranchId("STTHit"))
        continue;

      PndSttHit *stthit = (PndSttHit *)fSttHitArray->At(hitID);

      Int_t tubeID = stthit->GetTubeID();
      PndSttTube *tube = (PndSttTube *)fSttTubeArray->At(tubeID);
      if (tube->IsSkew())
        nofsttskewpoint++;
      else
        nofsttparalpoint++;
    }
  }

  if (fRunTimeBased) {
    FairMultiLinkedData idealTrackLinksToSttHits = idealtrack->GetLinksWithType(ioman->GetBranchId("STTHit"));
    for (int ihit = 0; ihit < idealTrackLinksToSttHits.GetNLinks(); ihit++) {
      PndSttHit *stthit = (PndSttHit *)ioman->GetCloneOfLinkData(idealTrackLinksToSttHits.GetLink(ihit));
      if ((stthit) == nullptr)
        continue;
      Int_t tubeID = stthit->GetTubeID();
      PndSttTube *tube = (PndSttTube *)fSttTubeArray->At(tubeID);
      if (tube->IsSkew())
        nofsttskewpoint++;
      else
        nofsttparalpoint++;

      stthit->Delete();
    }
  }

  // std::cout << "Number of parallel tubes: " << nofsttparalpoint << " and skewed: " << nofsttskewpoint << std::endl;

  PndTrackingQualityMCInfo info(nofmvdpixpoint, nofmvdstrpoint, nofsttparalpoint, nofsttskewpoint, nofgempoint, nofftspoint);
  std::vector<FairLink> mcTracks = idealtrack->GetSortedMCTracks();
  if (mcTracks.size() > 0) {
    PndMCTrack *myMCTrack = (PndMCTrack *)FairRootManager::Instance()->GetCloneOfLinkData(mcTracks[0]);
    if (myMCTrack != nullptr) {
      info.SetVertex(myMCTrack->GetStartVertex());
      if (myMCTrack->GetMotherID() < 0) {
        info.SetIsPrimary(kTRUE);
      } else {
        info.SetIsPrimary(kFALSE);
      }
    }
  }
  // CHECK
  // Bool_t isreco = Reconstructability(nofmvdpixpoint, nofmvdstrpoint, nofsttparalpoint, nofsttskewpoint, nofgempoint, nofscitilpoint);
  //  info.SetReconstructability(isreco);

  info.SetPositionFirst(idealtrack->GetParamFirst().GetPosition());
  info.SetMomentumFirst(idealtrack->GetParamFirst().GetMomentum());
  info.SetPositionLast(idealtrack->GetParamLast().GetPosition());
  info.SetMomentumLast(idealtrack->GetParamLast().GetMomentum());
  info.SetCharge(idealtrack->GetParamFirst().GetQ());

  return info;
}
/**
PndTrackingQualityRecoInfo PndTrackingQATaskTimebased::GetRecoInfoFromRecoTrack(int recotrackid, PndTrack *track) {

  PndTrackCand *trkcand = track->GetTrackCandPtr();

  Int_t nofsttpoint = trkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId("STTHit"));
  Int_t nofmvdpixpoint = trkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId("MVDHitsPixel"));
  Int_t nofmvdstrpoint = trkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId("MVDHitsStrip"));
  int nofmvdpoint = nofmvdpixpoint + nofmvdstrpoint;
  Int_t nofgempoint = trkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId("GEMHit"));

  int nofsttskewpoint = 0, nofsttparalpoint = 0;
  // this loop counts skewed (--> parallel) STT/FTS hits
  for(Int_t ihit = 0; ihit < trkcand->GetNHits(); ihit++) {
    PndTrackCandHit candhit = trkcand->GetSortedHit(ihit);
    Int_t hitID = candhit.GetHitId();
    Int_t detID = candhit.GetDetId();

    if(detID != FairRootManager::Instance()->GetBranchId("STTHit") continue;
    PndSttHit *stthit = (PndSttHit*) fSttHitArray->At(hitID);
    Int_t tubeID = stthit->GetTubeID();
    PndSttTube *tube = (PndSttTube*) fSttTubeArray->At(tubeID);
    if(tube->IsSkew()) nofsttskewpoint++;
    else nofsttparalpoint++;
  }

  // -------------------------------------------------------
  int mctrackid = trackid->GetCorrTrackID();
  int noftruehits = trackid->GetMultTrackID();

  int nofassomctracks = trackid->GetNCorrTrackId();
  int noffakehits = 0;
  for(int itrk = 1; itrk <  nofassomctracks; itrk++) {
    int noffake = trackid->GetMultTrackID(itrk);
    noffakehits += noffake;
  }

  //:...................:
  int noftruemvdpixhits = 0, noftruemvdstrhits = 0, noftruesttparalhits = 0, noftruesttskewhits = 0, noftruegemhits = 0, noftruescitilhits = 0;
  int noffakemvdpixhits = 0, noffakemvdstrhits = 0, noffakesttparalhits = 0, noffakesttskewhits = 0, noffakegemhits = 0, noffakescitilhits = 0;

  FairHit *hit = nullptr;
  FairMCPoint *point = nullptr;
  for(Int_t ihit = 0; ihit < trkcand->GetNHits(); ihit++) {
    PndTrackCandHit candhit = trkcand->GetSortedHit(ihit);
    Int_t hitID1 = candhit.GetHitId();
    Int_t detID1 = candhit.GetDetId();

    if(detID1 == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) {
      hit = (FairHit*) fMvdPixelHitArray->At(hitID1);
      int refindex = hit->GetRefIndex();
      if(refindex == -1) noffakemvdpixhits++;
      else {
  point = (FairMCPoint*) fMvdPointArray->At(refindex);
  int reftrackid = point->GetTrackID();
  if(reftrackid != mctrackid) noffakemvdpixhits++;
  else noftruemvdpixhits++;
      }
    }
    else if(detID1 == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) {
      hit = (FairHit*) fMvdStripHitArray->At(hitID1);
      int refindex = hit->GetRefIndex();
      if(refindex == -1) noffakemvdstrhits++;
      else {
  point = (FairMCPoint*) fMvdPointArray->At(refindex);
  int reftrackid = point->GetTrackID();
  if(reftrackid != mctrackid) noffakemvdstrhits++;
  else noftruemvdstrhits++;
      }
    }
    if(detID1 == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
      hit = (FairHit*) fSttHitArray->At(hitID1);

      Int_t tubeID = ((PndSttHit*) hit)->GetTubeID();
      PndSttTube *tube = (PndSttTube*) fSttTubeArray->At(tubeID);
      int refindex = hit->GetRefIndex();
      if(refindex == -1) {
  if(tube->IsSkew()) noffakesttskewhits++;
  else noffakesttparalhits++;
      }
      else {
  point = (FairMCPoint*) fSttPointArray->At(refindex);
  int reftrackid = point->GetTrackID();
  if(reftrackid != mctrackid) {
    if(tube->IsSkew()) noffakesttskewhits++;
    else noffakesttparalhits++;
  }
  else {
    if(tube->IsSkew()) noftruesttskewhits++;
    else noftruesttparalhits++;
  }
      }
    }
    else if(detID1 == FairRootManager::Instance()->GetBranchId(fGemBranch)) {
      hit = (FairHit*) fGemHitArray->At(hitID1);
      int refindex = hit->GetRefIndex();
      if(refindex == -1) noffakegemhits++;
      else {
  point = (FairMCPoint*) fGemPointArray->At(refindex);
  {
    int reftrackid = point->GetTrackID();
    if(reftrackid != mctrackid) noffakegemhits++;
    else noftruegemhits++;
  }
      }
    }
    else if(detID1 == FairRootManager::Instance()->GetBranchId(fSciTBranch)) {
      hit = (FairHit*) fSciTilHitArray->At(hitID1);
      int refindex = hit->GetRefIndex();
      if(refindex == -1) noffakescitilhits++;
      else {
  point = (FairMCPoint*) fSciTilPointArray->At(refindex);
  int reftrackid = point->GetTrackID();
  if(reftrackid != mctrackid) noffakescitilhits++;
  else noftruescitilhits++;
      }
    }
    // ----------------------------------------------------
  }

  PndTrkRecoTrackInfo info(recotrackid);
  info.SetNofMvdPixTrueHits(noftruemvdpixhits);
  info.SetNofMvdStrTrueHits(noftruemvdstrhits);
  info.SetNofSttParalTrueHits(noftruesttparalhits);
  info.SetNofSttSkewTrueHits(noftruesttskewhits);
  info.SetNofGemTrueHits(noftruegemhits);
  info.SetNofSciTilTrueHits(noftruescitilhits);
  info.SetNofMvdPixFakeHits(noffakemvdpixhits);
  info.SetNofMvdStrFakeHits(noffakemvdstrhits);
  info.SetNofSttParalFakeHits(noffakesttparalhits);
  info.SetNofSttSkewFakeHits(noffakesttskewhits);
  info.SetNofGemFakeHits(noffakegemhits);
  info.SetNofSciTilFakeHits(noffakescitilhits);

  info.SetMCTrackID(mctrackid);

  info.SetPositionFirst(track->GetParamFirst().GetPosition());
  info.SetMomentumFirst(track->GetParamFirst().GetMomentum());

  info.SetPositionLast(track->GetParamLast().GetPosition());
  info.SetMomentumLast(track->GetParamLast().GetMomentum());

  info.SetCharge(track->GetParamFirst().GetQ());


  for(int jtrk = 0; jtrk < fMCTrackInfo->GetEntriesFast(); jtrk++) {
    PndTrkMCTrackInfo *mcinfo = (PndTrkMCTrackInfo*) fMCTrackInfo->At(jtrk);
    int mcinfotrackid = mcinfo->GetMCTrackID();
    //        cout << "mcinfotrackid " << mcinfo->GetMCTrackID() << " " << info.GetMCTrackID() << endl;
    if(mcinfotrackid == mctrackid) {

      info.SetNofMvdPixMissingHits(mcinfo->GetNofMvdPixPoints() - noftruemvdpixhits);
      info.SetNofMvdStrMissingHits(mcinfo->GetNofMvdStrPoints() - noftruemvdstrhits);
      info.SetNofSttParalMissingHits(mcinfo->GetNofSttParalPoints() - noftruesttparalhits);
      info.SetNofSttSkewMissingHits(mcinfo->GetNofSttSkewPoints() - noftruesttskewhits);
      info.SetNofGemMissingHits(mcinfo->GetNofGemPoints() - noftruegemhits);
      info.SetNofSciTilMissingHits(mcinfo->GetNofSciTilPoints() - noftruescitilhits);


//       cout << "good mcinfotrackid " << mcinfo->GetMCTrackID() << " " << info.GetMCTrackID() << endl;
      info.SetMCTrackInfo(mcinfo);
      break;
    }
  }

  return info;
}


 **/

void PndTrackingQATaskTimebased::AssociateRecoTracksToMCTracks()
{
  // loop over mc track infos
  for (int imctrk = 0; imctrk < fMCTrackInfo->GetEntriesFast(); imctrk++) {
    PndTrackingQualityMCInfo *mcinfo = (PndTrackingQualityMCInfo *)fMCTrackInfo->At(imctrk); // Get the MC info of current MC track, ok event based
    int mctrackid0 = mcinfo->GetMCTrackID();                                                 // Get MC track ID from mcinfo, should work event based
    if (mcinfo->GetAssoRecoTrackID() != -1)
      continue;                      // Continues if there is no associated recoTrackInfo
    double tmpeff = 0., tmppur = 0.; // Temporary efficiency and purity
    int tmptruerecotrackid = -1;

    // loop over reco track infos
    PndTrackingQualityRecoInfo *tmprecoinfo = nullptr; // "Reset" temporary tracking reco info
    for (int itrk = 0; itrk < fRecoTrackInfo->GetEntriesFast(); itrk++) {
      PndTrackingQualityRecoInfo *recoinfo = (PndTrackingQualityRecoInfo *)fRecoTrackInfo->At(itrk); // Get the reco treack info of current reco track
      int mctrackid = recoinfo->GetMCTrackID();                                                      // Get ID of associated MC track, this works only event based

      // TODO: For time based one need to obtain the mctrack Fair Link instead, this might cause a problem
      if (mctrackid != mctrackid0)
        continue; // Continues if Mc track id is not the same as the one in the outer loop

      mcinfo->SetRecoTrackID(recoinfo->GetRecoTrackID()); // Sets the RecoTrackID of the mc info to the RecoTrackId of the recoInfo
      recoinfo->SetClone();
      // The two lines of code below is to account for when there are several reco infos connected to the mc info
      // One wants to choose the one with higher efficiency or purity
      // it must have either the higher efficiency ...
      if (recoinfo->GetEfficiency() < tmpeff)
        continue;
      // ... or, if they are even, the highest purity
      if (recoinfo->GetEfficiency() == tmpeff && recoinfo->GetPurity() < tmppur)
        continue;

      tmpeff = recoinfo->GetEfficiency();
      tmppur = recoinfo->GetPurity();
      tmptruerecotrackid = recoinfo->GetRecoTrackID();
      tmprecoinfo = recoinfo;
    }
    if (tmprecoinfo == nullptr)
      continue;

    tmprecoinfo->SetTrue();
    mcinfo->SetAssoRecoTrackID(tmptruerecotrackid);
  }
}

ClassImp(PndTrackingQATaskTimebased);
