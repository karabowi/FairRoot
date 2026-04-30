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
// -----                PndTrackingQualityBarrelTaskNewLinks source file             -----
// -----                  Created 18/07/08  by T.Stockmanns        -----
// -------------------------------------------------------------------------

/**
 * The quality numbers, qualityNumbers, as used here are defined in PndTrackingQualityAnalysis.h
 */

// C++ includes
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
#include "PndMCEntry.h"
#include "PndSttHit.h"
#include "PndSttTube.h"
//#include "PndSttMapCreator.h"
#include "PndStt2GeoHandler.h"
#include "RhoTuple.h"

// Class includes
#include "PndTrackingQualityBarrelTaskNewLinks.h"
#include "PndTrackingQualityBarrelAnalysisNewLinks.h"
#include "PndSdsHit.h"
#include "TGeoManager.h"
#include "PndGeoHandling.h"

// -----   Default constructor   -------------------------------------------
PndTrackingQualityBarrelTaskNewLinks::PndTrackingQualityBarrelTaskNewLinks(TString trackBranchName, TString idealBranchName, Bool_t pndTrackData)
  : FairTask("Creates PndMC Barrel test"), fBranchNames(), fMCInfoBranchName("MCTrackInfo"), fRecoInfoBranchName("RecoTrackInfo"), fMapLinkData(), fMapEfficiencies(),
    fMCInfoIdIdealId(), fNGhosts(0), fTrack(nullptr), fMCTrack(nullptr), fTrackCand(nullptr), fSttHitArray(nullptr), fMCTrackInfo(nullptr), fRecoTrackInfo(nullptr),
    fIdealTrack(nullptr), fSttTubeArray(nullptr), ioman(nullptr), fMapTrackQualifikation(), fTrackBranchName(trackBranchName), fIdealTrackBranchName(idealBranchName),
    fPndTrackOrTrackCand(pndTrackData), fSttParameters(nullptr), fTuple(nullptr), fPHisto(nullptr), fPRelHisto(nullptr), fPtHisto(nullptr), fPtRelHisto(nullptr),
    fQualyHisto(nullptr), fQualyStack(nullptr), fQualyHisto_mc(nullptr), fQualyHisto_neg(nullptr), fQualyHisto_pos(nullptr), fQualyHisto_all(nullptr), fEventNr(0)
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndTrackingQualityBarrelTaskNewLinks::~PndTrackingQualityBarrelTaskNewLinks() {}

// -----   Public method Init   --------------------------------------------
InitStatus PndTrackingQualityBarrelTaskNewLinks::Init()
{
  InitializeHistograms();

  ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndTrackingQualityBarrelTaskNewLinks::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  fTrack = (TClonesArray *)ioman->GetObject(fTrackBranchName);
  fMCTrack = (TClonesArray *)ioman->GetObject("MCTrack");
  //	fIdealTrackCand = (TClonesArray*) ioman->GetObject(fIdealTrackBranchName);
  fIdealTrack = (TClonesArray *)ioman->GetObject(fIdealTrackBranchName);
  fSttHitArray = (TClonesArray *)ioman->GetObject("STTHit");

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
    //	std::cout << "PndTrackingQualityAnalysis::Init() CorrectedSkewedHits present: " << FairRootManager::Instance()->GetBranchId("CorrectedSkewedHits") << " ";
    //	if (FairRootManager::Instance()->GetBranchId("CorrectedSkewedHits")  > 0){
    //		std::cout << "kTRUE";
    //		AddHitsBranchName("CorrectedSkewedHits");
    //	}
    //	std::cout << std::endl;
  }

  for (unsigned int i = 0; i < fBranchNames.size(); i++) {
    fMapEfficiencies[fBranchNames[i]] = new TH2D(fBranchNames[i], fBranchNames[i], 100, 0., 100., 50, 0, 1.1);
    fMapEfficiencies[fBranchNames[i]]->SetDrawOption("COLz");
  }

  // ----------------------------------------   maps of STT tubes
  // handles both old and new geometry
  fSttTubeArray = PndStt2GeoHandler::Instance(fSttParameters)->GetTubeArray();
  //PndStt2GeoHandler geoH(fSttParameters);
  //fTubeArray = geoH.GetTubeArray();
  
  //PndSttMapCreator *mapperStt = new PndSttMapCreator(fSttParameters);
  //fSttTubeArray = mapperStt->FillTubeArray();
  // ----------------------------------------------------  end map

  LOG(info) << " PndTrackingQualityBarrelTaskNewLinks::Init: Initialization successfull";

  return kSUCCESS;
}

void PndTrackingQualityBarrelTaskNewLinks::InitializeHistograms()
{
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

  fQualyHisto = new TH1I("fQualyHisto", "Quality of Trackfinding;;Counts", 26, -15.5, 10.5);
  fQualyHisto->SetDrawOption("TEXT HIST");
  LabelQualyHistogram(fQualyHisto);

  fQualyHisto_all = new TH1I("fQualyHisto_all", "Quality of Trackfinding;;Counts", 26, -15.5, 10.5);
  fQualyHisto_pos = new TH1I("fQualyHisto_pos", "Quality of Trackfinding;;Counts", 26, -15.5, 10.5);
  fQualyHisto_neg = new TH1I("fQualyHisto_neg", "Quality of Trackfinding;;Counts", 26, -15.5, 10.5);
  fQualyHisto_mc = new TH1I("fQualyHisto_mc", "Quality of Trackfinding;;Counts", 26, -15.5, 10.5);
  fQualyStack = new THStack();
}

void PndTrackingQualityBarrelTaskNewLinks::LabelQualyHistogram(TH1 *hist)
{
  hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kFullyFound), "Fully found");
  hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kPartiallyFound), "Partially found");
  hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kSpuriousFound), "Spurious found");
  hist->GetXaxis()->SetBinLabel(hist->FindFixBin(qualityNumbers::kGhost), "Ghosts");
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
void PndTrackingQualityBarrelTaskNewLinks::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRun::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
}

// -----   Public method Exec   --------------------------------------------
void PndTrackingQualityBarrelTaskNewLinks::Exec(Option_t *)
{
  fMCTrackInfo->Delete();
  fRecoTrackInfo->Delete();

  std::cout << "----- Event " << fEventNr << " ------" << std::endl;

  PndTrackingQualityBarrelAnalysisNewLinks qaAna(fTrackBranchName, fIdealTrackBranchName, new CircleHoughTrackFunctor(), fPndTrackOrTrackCand);
  qaAna.SetVerbose(fVerbose);
  qaAna.SetHitsBranchNames(fBranchNames);
  qaAna.Init();
  qaAna.AnalyseEvent(fRecoTrackInfo);

  std::map<Int_t, Int_t> qualiMap = qaAna.GetTrackQualification();
  std::map<Int_t, Int_t> mcStatusMap = qaAna.GetTrackMCStatus();
  std::map<Int_t, Int_t> mcFoundMap = qaAna.GetMCTrackFound();
  std::map<Int_t, TVector3> recoPMap = qaAna.GetP();
  //	std::map<Int_t, Double_t> recoPtMap = qaAna.GetPt();

  FillQualyHisto(qualiMap, qaAna.GetNGhosts());
  FillMCStatus(mcStatusMap);
  FillEfficiencies(qaAna.GetEfficiencies());
  MapToHist(qaAna.GetPResolution(), fPHisto);
  MapToHist(qaAna.GetPResolutionRel(), fPRelHisto);
  MapToHist(qaAna.GetPtResolution(), fPtHisto);
  MapToHist(qaAna.GetPtResolutionRel(), fPtRelHisto);

  // fill MC Track Info ......................................
  // for (std::map<Int_t, Int_t>::iterator iter = qualiMap.begin(); iter != qualiMap.end(); iter++) {
  for (std::map<Int_t, Int_t>::iterator iter = mcStatusMap.begin(); iter != mcStatusMap.end(); iter++) {
    Int_t mcTrackId = iter->first;
    Int_t idealTrackId = qaAna.GetIdealTrackIdFromMCTrackId(mcTrackId);
    Int_t trackQuality = iter->second;

    PndTrack *idealtrack = (PndTrack *)fIdealTrack->At(idealTrackId);

    PndMCTrack *myMcTrack = (PndMCTrack *)fMCTrack->At(mcTrackId);
    Int_t pdgId = myMcTrack->GetPdgCode();

    int size = fMCTrackInfo->GetEntriesFast();
    PndTrackingQualityMCInfo mctrackinfo = GetMCInfoFromIdealTrack(idealtrack);
    mctrackinfo.SetMCTrackID(mcTrackId);
    mctrackinfo.SetQuality(trackQuality);
    mctrackinfo.SetPDGCode(pdgId);
    //  mctrackinfo.SetReconstructabilityStatus();

    if (mctrackinfo.GetNofMCPoints() > 0) {
      new ((*fMCTrackInfo)[size]) PndTrackingQualityMCInfo(mctrackinfo);
      fMCInfoIdIdealId[idealTrackId] = size;
    }
    //     cout << "MCTRack " << mctrackinfo.GetMCTrackID() << endl;
  }
  // ............................................................

  // loop over reco track info and associate the mc track info
  for (int itrk = 0; itrk < fRecoTrackInfo->GetEntriesFast(); itrk++) {
    PndTrackingQualityRecoInfo *recoinfo = (PndTrackingQualityRecoInfo *)fRecoTrackInfo->At(itrk);
    Int_t idealTrackId = qaAna.GetIdealTrackIdFromRecoTrackId(recoinfo->GetRecoTrackID());
    PndTrackingQualityMCInfo *mctrackinfo = (PndTrackingQualityMCInfo *)fMCTrackInfo->At(GetMCInfoIdFromIdealTrackId(idealTrackId));
    recoinfo->SetMCTrackInfo(mctrackinfo);
  }

  // associate reconstructed and mc tracks
  AssociateRecoTracksToMCTracks();

  // Save the Tree (/RhoTuple) for some possible additional analysis
  for (std::map<Int_t, Int_t>::iterator iter = qualiMap.begin(); iter != qualiMap.end(); iter++) {
    Int_t mcTrackId = iter->first;
    Int_t trackQuality = iter->second;

    TVector3 recoMomentum = recoPMap[mcTrackId];

    PndMCTrack *myMcTrack = (PndMCTrack *)fMCTrack->At(mcTrackId);
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

  //	if (fVerbose > 1)
  qaAna.PrintTrackQualityMap();
  fEventNr++;
}

Int_t PndTrackingQualityBarrelTaskNewLinks::GetSumOfAllValidMCHits(FairMultiLinkedData *trackData)
{
  Int_t result = 0;
  for (unsigned int branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++) {
    result += trackData->GetLinksWithType(ioman->GetBranchId(fBranchNames[branchIndex])).GetNLinks();
  }
  return result;
}

void PndTrackingQualityBarrelTaskNewLinks::FillQualyHisto(std::map<Int_t, Int_t> trackQualifikation, Int_t nGhosts)
{

  fQualyHisto->Fill(qualityNumbers::kGhost, nGhosts);
  for (std::map<Int_t, Int_t>::iterator iter = trackQualifikation.begin(); iter != trackQualifikation.end(); iter++) {
    fQualyHisto->Fill(iter->second);
    if (iter->second > 0) {
      fQualyHisto->Fill(qualityNumbers::kFound);
    } else {
      fQualyHisto->Fill(qualityNumbers::kNotFound);
    }
  }
}

void PndTrackingQualityBarrelTaskNewLinks::FillMCStatus(std::map<Int_t, Int_t> trackMCStatus)
{
  int mcOffset = qualityNumbers::kPossibleSec - qualityNumbers::kMcPossibleSec;
  for (std::map<Int_t, Int_t>::iterator iter = trackMCStatus.begin(); iter != trackMCStatus.end(); iter++) {
    fQualyHisto->Fill(iter->second - mcOffset); // mcOffset = 6
  }
}

void PndTrackingQualityBarrelTaskNewLinks::FillEfficiencies(std::map<Int_t, std::map<TString, std::pair<Double_t, Int_t>>> efficiencies)
{
  for (std::map<Int_t, std::map<TString, std::pair<Double_t, Int_t>>>::iterator iterTracks = efficiencies.begin(); iterTracks != efficiencies.end(); iterTracks++) {
    std::map<TString, std::pair<Double_t, Int_t>> branchEfficiency = iterTracks->second;
    for (std::map<TString, std::pair<Double_t, Int_t>>::iterator iterBranch = branchEfficiency.begin(); iterBranch != branchEfficiency.end(); iterBranch++) {
      fMapEfficiencies[iterBranch->first]->Fill(iterBranch->second.second, iterBranch->second.first);
    }
  }
}
void PndTrackingQualityBarrelTaskNewLinks::MapToHist(std::map<Int_t, Double_t> map, TH1 *histo)
{
  for (std::map<Int_t, Double_t>::iterator iter = map.begin(); iter != map.end(); iter++) {
    histo->Fill(iter->second);
  }
}

void PndTrackingQualityBarrelTaskNewLinks::Finish()
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

  for (unsigned int i = 0; i < fBranchNames.size(); i++) {
    fMapEfficiencies[fBranchNames[i]]->Write();
  }
  fPHisto->Write();
  fPRelHisto->Write();
  fPtHisto->Write();
  fPtRelHisto->Write();
  fQualyHisto->Write();
  fQualyStack->Write();
  fTuple->GetInternalTree()->Write();

  Int_t allTracksWithHits = 0;
  Int_t allPossibleTracksWithHits = 0;
  Int_t allTracksWithHitsNotFound = 0;

  allTracksWithHits += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcAtLeastThreePrim)); // Todo
  allTracksWithHits += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcAtLeastThreeSec));
  allTracksWithHits += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcPossiblePrim));
  allTracksWithHits += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcPossibleSec));

  allPossibleTracksWithHits += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcPossiblePrim));
  allPossibleTracksWithHits += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcPossibleSec));

  allTracksWithHitsNotFound += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kAtLeastThreePrim));
  allTracksWithHitsNotFound += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kAtLeastThreeSec));
  allTracksWithHitsNotFound += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPossiblePrim));
  allTracksWithHitsNotFound += fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPossibleSec));

  std::cout << "fQualyHisto: All Tracks: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(-11)) + allTracksWithHits << std::endl
            << " Primary Tracks < 3 hits: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(-11)) << std::endl
            << " All Tracks with hits: " << allTracksWithHits << ". Not Found: " << allTracksWithHitsNotFound << std::endl
            << " Primary Tracks with >= 3 hits, but not a possible track: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcAtLeastThreePrim))
            << ". Not Found: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kAtLeastThreePrim)) << std::endl
            << " Secondary Tracks with >= 3 hits, but not a possible track: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcAtLeastThreeSec))
            << ". Not Found: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kAtLeastThreeSec)) << std::endl
            << " Primary Tracks possible: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcPossiblePrim))
            << ". Not Found: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPossiblePrim)) << std::endl
            << " Secondary Tracks possible: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kMcPossibleSec))
            << ". Not Found: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPossibleSec)) << std::endl

            << " FullyFound: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kFullyFound)) << " "
            << (Double_t)fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kFullyFound)) / allTracksWithHits * 100 << "% (of all tracks), "
            << (Double_t)fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kFullyFound)) / allPossibleTracksWithHits * 100 << "% (of all tracks possible)"

            << " PartlyFound: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPartiallyFound)) << " "
            << (Double_t)fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPartiallyFound)) / allTracksWithHits * 100 << "% "
            << (Double_t)fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPartiallyFound)) / allPossibleTracksWithHits * 100 << "% "

            << " Spurious: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kSpuriousFound)) << " "
            << (Double_t)fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kSpuriousFound)) / allTracksWithHits * 100 << "% "
            << (Double_t)fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kSpuriousFound)) / allPossibleTracksWithHits * 100 << "% "

            << " Ghosts: " << fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kGhost)) << " "
            << (Double_t)fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kGhost)) / allTracksWithHits * 100 << "% "
            << (Double_t)fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kGhost)) / allPossibleTracksWithHits * 100 << "% " << std::endl;
}

void PndTrackingQualityBarrelTaskNewLinks::ColorHistogram()
{
  fQualyHisto_pos->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kGhost), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kGhost)));
  fQualyHisto_pos->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kSpuriousFound), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kSpuriousFound)));
  fQualyHisto_pos->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPartiallyFound), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kPartiallyFound)));
  fQualyHisto_pos->SetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kFullyFound), fQualyHisto->GetBinContent(fQualyHisto->FindFixBin(qualityNumbers::kFullyFound)));
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

// SG!!!

Bool_t IsBarrelMVD(const PndSdsHit *hit)
{
  if (!hit)
    return 0;
  Int_t sensorID = hit->GetSensorID();
  gGeoManager->cd(PndGeoHandling::Instance()->GetPath(sensorID));
  TGeoHMatrix *transMat = gGeoManager->GetCurrentMatrix();
  if (!transMat) {
    std::cout << "\n\nIsBarrelMVD():: NO transition Matrix for MVD sensor " << sensorID << "\n\n!!!" << std::endl;
    return 0;
  }
  Double_t *mmm = transMat->GetRotationMatrix();
  if (!mmm) {
    std::cout << "\n\nIsBarrelMVD():: Can not extract transition Matrix for MVD sensor " << sensorID << ", something is completely wrong\n\n!!!" << std::endl;
    exit(0); // SG!!
    return 0;
  }
  bool forward = (fabs(mmm[6]) < 0.999 && fabs(mmm[7]) < 0.999);
  return (!forward);
}

PndTrackingQualityMCInfo PndTrackingQualityBarrelTaskNewLinks::GetMCInfoFromIdealTrack(PndTrack *idealtrack)
{

  PndTrackCand *idealtrkcand = idealtrack->GetTrackCandPtr();

  // Int_t nofsttpoint = idealtrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId("STTHit"));
  Int_t nofmvdpixpoint = idealtrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId("MVDHitsPixel"));
  Int_t nofmvdstrpoint = idealtrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId("MVDHitsStrip"));
  // Int_t nofmvdpoint = nofmvdpixpoint + nofmvdstrpoint;
  Int_t nofgempoint = idealtrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId("GEMHit"));

  int nofsttskewpoint = 0, nofsttparalpoint = 0;
  // this loop counts skewed (--> parallel) STT/FTS hits
  for (UInt_t ihit = 0; ihit < idealtrkcand->GetNHits(); ihit++) {
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

  // SG!!!
  /*
  nofmvdpixpoint = 0;
  nofmvdstrpoint = 0;
  nofgempoint = 0;

  TClonesArray *mvdPixArray = (TClonesArray*) FairRootManager::Instance()->GetObject("MVDHitsPixel");
  TClonesArray *mvdStripArray = (TClonesArray*) FairRootManager::Instance()->GetObject("MVDHitsStrip");

  for(Int_t ihit = 0; ihit < idealtrkcand->GetNHits(); ihit++) {
    PndTrackCandHit idealcandhit = idealtrkcand->GetSortedHit(ihit);
    Int_t hitID = idealcandhit.GetHitId();
    Int_t detID = idealcandhit.GetDetId();

    if(detID == FairRootManager::Instance()->GetBranchId("MVDHitsPixel")){
      const PndSdsHit *hit = dynamic_cast<const PndSdsHit*>(mvdPixArray->At(hitID));
      if(!hit) {
  std::cout << "GetMCInfoFromIdealTrack() : MVD pix. hit is not found "<<std::endl;
  exit(0);
  continue;
      }
      nofmvdpixpoint+=IsBarrelMVD(hit);
    }
    else    if(detID == FairRootManager::Instance()->GetBranchId("MVDHitsStrip")){
      const PndSdsHit *hit = dynamic_cast<const PndSdsHit*>(mvdStripArray->At(hitID));
      if(!hit) {
  std::cout << "GetMCInfoFromIdealTrack() : MVD strip hit is not found "<<std::endl;
  exit(0);
  continue;
      }
      nofmvdstrpoint+=IsBarrelMVD(hit);
    }
  }

  nofmvdpoint = nofmvdpixpoint + nofmvdstrpoint;
  */
  PndTrackingQualityMCInfo info(nofmvdpixpoint, nofmvdstrpoint, nofsttparalpoint, nofsttskewpoint, nofgempoint);

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
PndTrackingQualityRecoInfo PndTrackingQualityBarrelTaskNewLinks::GetRecoInfoFromRecoTrack(int recotrackid, PndTrack *track) {

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

void PndTrackingQualityBarrelTaskNewLinks::AssociateRecoTracksToMCTracks()
{
  // loop over mc track infos
  for (int imctrk = 0; imctrk < fMCTrackInfo->GetEntriesFast(); imctrk++) {
    PndTrackingQualityMCInfo *mcinfo = (PndTrackingQualityMCInfo *)fMCTrackInfo->At(imctrk);
    int mctrackid0 = mcinfo->GetMCTrackID();
    if (mcinfo->GetAssoRecoTrackID() != -1)
      continue;
    double tmpeff = 0., tmppur = 0.;
    int tmptruerecotrackid = -1;

    // loop over reco track infos
    PndTrackingQualityRecoInfo *tmprecoinfo = nullptr;
    for (int itrk = 0; itrk < fRecoTrackInfo->GetEntriesFast(); itrk++) {
      PndTrackingQualityRecoInfo *recoinfo = (PndTrackingQualityRecoInfo *)fRecoTrackInfo->At(itrk);
      int mctrackid = recoinfo->GetMCTrackID();
      if (mctrackid != mctrackid0)
        continue;
      mcinfo->SetRecoTrackID(recoinfo->GetRecoTrackID());
      recoinfo->SetClone();
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

ClassImp(PndTrackingQualityBarrelTaskNewLinks);
