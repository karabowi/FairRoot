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

#include "PndSTEAnalysisTask.h"
#include "TString.h"

#include "PndSciTHit.h"
#include "FairRootManager.h"
#include "TTimeStamp.h"

#include "FairMultiLinkedData.h"
#include "FairLink.h"

using std::cout;
using std::endl;

ClassImp(PndSTEAnalysisTask);

InitStatus PndSTEAnalysisTask::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    cout << "-E- PndHitFinderTask::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  InitHitArray(fHitBranchName);
  InitTrackArray(fTrackBranchName);

  InitHistograms();

  eventCount = 0;

  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndSTEAnalysisTask::Exec(Option_t *)
{
  if (eventCount == 0) {

    cout << "------ PndSTEAnalysisTask::Exec() ------" << endl;
  }

  eventCount++;

  LOG(debug) << "------ Event: " << eventCount << " ------" << endl;

  if (fAnalyzeBtofHits) {
    /** @brief Loop over all BToF hits */
    for (int i_Hit = 0; (int)i_Hit < hitArray->GetEntries(); i_Hit++) {

      PndSciTHit *myHit = static_cast<PndSciTHit *>(hitArray->At(i_Hit));

      AssociateBToFHitWithMCTrack(myHit);
    }
  }

  /** @brief Loop over all BToF hits */
  for (int i_Track = 0; (int)i_Track < trackArray->GetEntries(); i_Track++) {
    numHits = 0;

    PndTrack *myTrack = static_cast<PndTrack *>(trackArray->At(i_Track));
    if (myTrack == nullptr)
      continue;

    // Produce momentum distribution histograms
    //////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<FairLink> myMcTracks = myTrack->GetSortedMCTracks();

    int numMCTracks = myMcTracks.size();

    hNumMcTracksPerIdealTrack->Fill(numMCTracks);

    if (myMcTracks.size() == 0)
      continue;

    PndMCTrack *myMcTrack = static_cast<PndMCTrack *>(FairRootManager::Instance()->GetCloneOfLinkData(myMcTracks[0]));

    TVector3 mom(myTrack->GetParamFirst().GetPx(), myTrack->GetParamFirst().GetPy(), myTrack->GetParamFirst().GetPz());

    if (numMCTracks == 1) {

      PndMCTrack *myFilteredMcTrack = static_cast<PndMCTrack *>(FairRootManager::Instance()->GetCloneOfLinkData(myMcTracks[0]));

      TVector3 mcMomFiltered(myFilteredMcTrack->GetMomentum());

      hMomFiltered->Fill(mcMomFiltered.Pt(), mom.Pt() - mcMomFiltered.Pt());
      hMomRelFiltered->Fill(mcMomFiltered.Pt(), (mom.Pt() - mcMomFiltered.Pt()) / mcMomFiltered.Pt());

      hTotMomFiltered->Fill(mcMomFiltered.Mag(), mom.Pt() - mcMomFiltered.Pt());
      hTotMomRelFiltered->Fill(mcMomFiltered.Mag(), (mom.Pt() - mcMomFiltered.Pt()) / mcMomFiltered.Pt());

      hMomResFiltered->Fill(mom.Pt() - mcMomFiltered.Pt());
      hMomRelResFiltered->Fill((mom.Pt() - mcMomFiltered.Pt()) / mcMomFiltered.Pt());
    }

    if (numMCTracks > 1) {

      double minimumFilteredResolution = 99999999.0;
      double minimumFilteredRelativeResolution = 9999999.0;
      double minimumPtFiltered = 999999.0;
      double minimumPFiltered = 999999.0;

      LOG(debug) << "Track Number " << i_Track << " with pt " << mom.Pt() << endl;
      LOG(debug) << "Start point track: " << myTrack->GetParamFirst().GetX() << " : " << myTrack->GetParamFirst().GetY() << " : " << myTrack->GetParamFirst().GetZ() << endl;

      for (int i_mcTrack = 0; i_mcTrack < (int)myMcTracks.size(); i_mcTrack++) {

        PndMCTrack *myFilteredMcTrack = static_cast<PndMCTrack *>(FairRootManager::Instance()->GetCloneOfLinkData(myMcTracks[i_mcTrack]));

        TVector3 mcMomFiltered(myFilteredMcTrack->GetMomentum());

        LOG(debug) << "MCtrack Number " << i_mcTrack << " with pt " << mcMomFiltered.Pt() << endl;
        LOG(debug) << "StartPoint of mc track: " << myFilteredMcTrack->GetStartVertex().X() << " : " << myFilteredMcTrack->GetStartVertex().Y() << " : "
                   << myFilteredMcTrack->GetStartVertex().Z() << endl;

        double filteredResolution = mom.Pt() - mcMomFiltered.Pt();
        double filteredRelativeResolution = (mom.Pt() - mcMomFiltered.Pt()) / mcMomFiltered.Pt();

        if (sqrt(minimumFilteredResolution * minimumFilteredResolution) > sqrt(filteredResolution * filteredResolution)) {

          // std::cout << i_mcTrack <<  " : " << minimumFilteredResolution << std::endl;
          minimumFilteredResolution = filteredResolution;
          minimumFilteredRelativeResolution = filteredRelativeResolution;
          minimumPtFiltered = mcMomFiltered.Pt();
          minimumPFiltered = mcMomFiltered.Mag();
        }
      }

      hMomFiltered->Fill(minimumPtFiltered, minimumFilteredResolution);
      hMomRelFiltered->Fill(minimumPtFiltered, minimumFilteredRelativeResolution);

      hTotMomFiltered->Fill(minimumPFiltered, minimumFilteredResolution);
      hTotMomRelFiltered->Fill(minimumPFiltered, minimumFilteredRelativeResolution);

      hMomResFiltered->Fill(minimumFilteredResolution);
      hMomRelResFiltered->Fill(minimumFilteredRelativeResolution);
    }

    if (myMcTrack) {
      TVector3 mcMom(myMcTrack->GetMomentum());
      double_t angle = mcMom.Theta();

      // FairMultiLinkedData links=NULL;
      FairMultiLinkedData links = myTrack->GetLinksWithType(FairRootManager::Instance()->GetBranchId("STTHit"));
      // FairMultiLinkedData linksCorrectedHits=NULL;
      FairMultiLinkedData linksCorrectedHits = myTrack->GetLinksWithType(FairRootManager::Instance()->GetBranchId("CorrectedIsochrones"));

      PndTrackCand myCand = myTrack->GetTrackCand();

      numHits = myCand.GetNHits();

      hMomRes->Fill((mom.Pt() - mcMom.Pt()));
      hRelMomRes->Fill((mom.Pt() - mcMom.Pt()) / mcMom.Pt());

      hMomNumHitsRealistic->Fill(numHits, (mom.Pt() - mcMom.Pt()));
      hRelMomNumHitsRealistic->Fill(numHits, (mom.Pt() - mcMom.Pt()) / mcMom.Pt());

      hMomAngleRealistic->Fill(angle * 180 / 3.14, (mom.Pt() - mcMom.Pt()));
      hRelMomAngleRealistic->Fill(angle * 180 / 3.14, (mom.Pt() - mcMom.Pt()) / mcMom.Pt());

      hMomRealistic->Fill(mcMom.Pt(), (mom.Pt() - mcMom.Pt()));
      hRelMomRealistic->Fill(mcMom.Pt(), (mom.Pt() - mcMom.Pt()) / mcMom.Pt());

      hTotMom->Fill(mcMom.Mag(), mom.Pt() - mcMom.Pt());
      hTotMomRel->Fill(mcMom.Mag(), (mom.Pt() - mcMom.Pt()) / mcMom.Pt());

      if (numHits > fCut || numHits == fCut) {

        hMomResWithCut->Fill((mom.Pt() - mcMom.Pt()));
        hMomRelResWithCut->Fill((mom.Pt() - mcMom.Pt()) / mcMom.Pt());
      }
      if (numHits < fCut) {

        hMomResCutAwayPart->Fill((mom.Pt() - mcMom.Pt()));
        hMomRelResCutAwayPart->Fill((mom.Pt() - mcMom.Pt()) / mcMom.Pt());
      }

      if (mcMom.Pt() > 0.1 || mcMom.Pt() == 0.1) {

        hMomResWithCutPTCut->Fill((mom.Pt() - mcMom.Pt()));
        hMomRelResWithCutPTCut->Fill((mom.Pt() - mcMom.Pt()) / mcMom.Pt());
      }
      if (mcMom.Pt() < 0.1) {

        hMomResCutAwayPartPTCut->Fill((mom.Pt() - mcMom.Pt()));
        hMomRelResCutAwayPartPTCut->Fill((mom.Pt() - mcMom.Pt()) / mcMom.Pt());
      }

      ///////////////// For consistency checking /////////////////

      hAllRes_NumMCTracks->Fill(numMCTracks, (mom.Pt() - mcMom.Pt()));

      ///// Strange tracks if momentum resolution < -0.2 //////

      if ((mom.Pt() - mcMom.Pt()) < -0.1) {
        // Number of hits per track
        hNegativeRes_NumHits->Fill(numHits, (mom.Pt() - mcMom.Pt()));
        // MC Momentum at start of track
        hNegativeRes_MCMom->Fill(mcMom.Mag(), (mom.Pt() - mcMom.Pt()));
        // Reco momentum at start of track
        hNegativeRes_RecoMom->Fill(mom.Mag(), (mom.Pt() - mcMom.Pt()));
        // MC angle
        hNegativeRes_MCTheta->Fill(angle * 180 / 3.14, (mom.Pt() - mcMom.Pt()));
        // Num MC Tracks
        hNegativeRes_NumMCTracks->Fill(numMCTracks, (mom.Pt() - mcMom.Pt()));
      }

      ///// Strange tracks if momentum resolution > 0.2 //////

      if ((mom.Pt() - mcMom.Pt()) > 0.1) {
        // Number of hits per track
        hPositiveRes_NumHits->Fill(numHits, (mom.Pt() - mcMom.Pt()));
        // MC Momentum at start of track
        hPositiveRes_MCMom->Fill(mcMom.Mag(), (mom.Pt() - mcMom.Pt()));
        // Reco momentum at start of track
        hPositiveRes_RecoMom->Fill(mom.Mag(), (mom.Pt() - mcMom.Pt()));
        // MC angle
        hPositiveRes_MCTheta->Fill(angle * 180 / 3.14, (mom.Pt() - mcMom.Pt()));
        // Num MC Tracks
        hPositiveRes_NumMCTracks->Fill(numMCTracks, (mom.Pt() - mcMom.Pt()));
      }

      ////// Not strange tracks ///////

      if ((mom.Pt() - mcMom.Pt()) < 0.1 && (mom.Pt() - mcMom.Pt()) > -0.1) {
        // Number of hits per track
        hGoodRes_NumHits->Fill(numHits, (mom.Pt() - mcMom.Pt()));
        // MC Momentum at start of track
        hGoodRes_MCMom->Fill(mcMom.Mag(), (mom.Pt() - mcMom.Pt()));
        // Reco momentum at start of track
        hGoodRes_RecoMom->Fill(mom.Mag(), (mom.Pt() - mcMom.Pt()));
        // MC angle
        hGoodRes_MCTheta->Fill(angle * 180 / 3.14, (mom.Pt() - mcMom.Pt()));
        // Num MC Tracks
        hGoodRes_NumMCTracks->Fill(numMCTracks, (mom.Pt() - mcMom.Pt()));
      }

      if ((mom.Pt() - mcMom.Pt()) / mcMom.Pt() < -0.1) {
        // Number of hits per track
        hNegativeRelRes_NumHits->Fill(numHits, (mom.Pt() - mcMom.Pt()) / mcMom.Pt());
        // MC Momentum at start of track
        hNegativeRelRes_MCMom->Fill(mcMom.Mag(), (mom.Pt() - mcMom.Pt()) / mcMom.Pt());
        // Reco momentum at start of track
        hNegativeRelRes_RecoMom->Fill(mom.Mag(), (mom.Pt() - mcMom.Pt()) / mcMom.Pt());
        // MC angle
        hNegativeRelRes_MCTheta->Fill(angle * 180 / 3.14, (mom.Pt() - mcMom.Pt()) / mcMom.Pt());
        // Num MC Tracks
        hNegativeRelRes_NumMCTracks->Fill(numMCTracks, (mom.Pt() - mcMom.Pt()) / mcMom.Pt());
      }

      ///// Strange tracks if momentum resolution > 0.2 //////

      if ((mom.Pt() - mcMom.Pt()) / mcMom.Pt() > 0.1) {
        // Number of hits per track
        hPositiveRelRes_NumHits->Fill(numHits, (mom.Pt() - mcMom.Pt()) / mcMom.Pt());
        // MC Momentum at start of track
        hPositiveRelRes_MCMom->Fill(mcMom.Mag(), (mom.Pt() - mcMom.Pt()) / mcMom.Pt());
        // Reco momentum at start of track
        hPositiveRelRes_RecoMom->Fill(mom.Mag(), (mom.Pt() - mcMom.Pt()) / mcMom.Pt());
        // MC angle
        hPositiveRelRes_MCTheta->Fill(angle * 180 / 3.14, (mom.Pt() - mcMom.Pt()) / mcMom.Pt());
        // Num MC Tracks
        hPositiveRelRes_NumMCTracks->Fill(numMCTracks, (mom.Pt() - mcMom.Pt()) / mcMom.Pt());
      }

      ////// Not strange tracks ///////

      if ((mom.Pt() - mcMom.Pt()) / mcMom.Pt() < 0.1 && (mom.Pt() - mcMom.Pt()) / mcMom.Pt() > -0.1) {
        // Number of hits per track
        hGoodRelRes_NumHits->Fill(numHits, (mom.Pt() - mcMom.Pt()) / mcMom.Pt());
        // MC Momentum at start of track
        hGoodRelRes_MCMom->Fill(mcMom.Mag(), (mom.Pt() - mcMom.Pt()) / mcMom.Pt());
        // Reco momentum at start of track
        hGoodRelRes_RecoMom->Fill(mom.Mag(), (mom.Pt() - mcMom.Pt()) / mcMom.Pt());
        // MC angle
        hGoodRelRes_MCTheta->Fill(angle * 180 / 3.14, (mom.Pt() - mcMom.Pt()) / mcMom.Pt());
        // Num MC Tracks
        hGoodRelRes_NumMCTracks->Fill(numMCTracks, (mom.Pt() - mcMom.Pt()) / mcMom.Pt());
      }

      hNumMCTracksAbsolute->Fill(numMCTracks, (mom.Pt() - mcMom.Pt()));
      hNumMCTracksRelative->Fill(numMCTracks, (mom.Pt() - mcMom.Pt()) / mcMom.Pt());
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////

    if (fAnalyzeBtofHits) {
      AssociateTrackWithMCTrack(myTrack);
      AnalyzeBToFHits(myTrack);
    }
  }
}

void PndSTEAnalysisTask::AssociateTrackWithMCTrack(PndTrack *recoTrack)
{

  LOG(debug) << "------ PndSTEAnalysisTask::AssociateTrackWithMCTrack() ------" << endl;

  std::vector<FairLink> mcTracks = recoTrack->GetSortedMCTracks();

  std::vector<int> indicesMcTrack;
  indicesMcTrack.clear();

  indicesMcTrack.push_back(mcTracks[0].GetIndex());
  indicesMcTrack.push_back(mcTracks[0].GetEntry());

  fMapRecoTrackIndices[recoTrack] = indicesMcTrack;
}

void PndSTEAnalysisTask::AssociateBToFHitWithMCTrack(PndSciTHit *btofHit)
{

  LOG(debug) << "------ PndSTEAnalysisTask::AssociateBToFHitWithMCTrack() ------" << endl;

  std::vector<int> indicesBToFHit;
  indicesBToFHit.clear();

  if (btofHit != nullptr) {
    FairMultiLinkedData links = btofHit->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack"));

    // Check the links both from the Reco tracks and from the Hits to see that they are set correctly

    if (links.GetNLinks() > 1) {
      std::cout << "WARNING! BTOF hit was made from several MCTracks!" << std::endl;
    }

    double hitTime = btofHit->GetTimeStamp();

    indicesBToFHit.push_back(links.GetLink(0).GetIndex());
    indicesBToFHit.push_back(links.GetLink(0).GetEntry());

    fMapBToFHitIndices[hitTime] = indicesBToFHit;
  }
}

void PndSTEAnalysisTask::AnalyzeBToFHits(PndTrack *recoTrack)
{

  LOG(debug) << "------ PndSTEAnalysisTask::AnalyzeBToFHits() ------" << endl;

  PndTrackCand *recoTrackCand = recoTrack->GetTrackCandPtr();

  // recoTrackCand->PrintLinkInfo();

  FairMultiLinkedData linksTrackToHit = recoTrackCand->GetLinksWithType(FairRootManager::Instance()->GetBranchId("SciTHit"));

  if (linksTrackToHit.GetNLinks() == 0) {
    fNotAssignedHits++;
  }
  if (linksTrackToHit.GetNLinks() > 0) {
    // Only one BToF hit should be linked to each track
    PndSciTHit *hit = static_cast<PndSciTHit *>(FairRootManager::Instance()->GetCloneOfLinkData(linksTrackToHit.GetLink(0)));

    if (hit != nullptr) {

      double hitTime = hit->GetTimeStamp();

      if (fMapBToFHitIndices[hitTime].size() == 2) {
        if (fMapBToFHitIndices[hitTime].at(0) == fMapRecoTrackIndices[recoTrack].at(0) && fMapBToFHitIndices[hitTime].at(1) == fMapRecoTrackIndices[recoTrack].at(1)) {

          /** @brief BToF hit correctly assigned, the total ammount is written out after the final event */
          fCorrectlyAssignedHits++;
        }
        if (fMapBToFHitIndices[hitTime].at(0) != fMapRecoTrackIndices[recoTrack].at(0) || fMapBToFHitIndices[hitTime].at(1) != fMapRecoTrackIndices[recoTrack].at(1)) {

          /** @brief BToF hit incorrectly assigned, the total ammount is written out after the final event */
          fWronglyAssignedHits++;
        }
      }
    }
  }
}

void PndSTEAnalysisTask::InitHitArray(TString hitBranchName)
{
  hitArray = (TClonesArray *)FairRootManager::Instance()->GetObject(hitBranchName);
  std::cout << "Successfully Initiated hit branch of name " << hitBranchName << std::endl;
}

void PndSTEAnalysisTask::InitTrackArray(TString trackBranchName)
{
  trackArray = (TClonesArray *)FairRootManager::Instance()->GetObject(trackBranchName);
  std::cout << "Successfully Initiated track branch of name " << trackBranchName << std::endl;
}

void PndSTEAnalysisTask::InitHistograms()
{

  hNumMcTracksPerIdealTrack = new TH1F("hNumMcTracksPerIdealTrack", " ", 100, 0, 100);
  // Fixing the X axis
  hNumMcTracksPerIdealTrack->GetXaxis()->SetLabelSize(0.05);
  hNumMcTracksPerIdealTrack->GetXaxis()->SetTitleSize(0.06);
  hNumMcTracksPerIdealTrack->GetXaxis()->SetTitleOffset(0.99);
  hNumMcTracksPerIdealTrack->GetXaxis()->CenterTitle();
  hNumMcTracksPerIdealTrack->GetXaxis()->SetTitle("Number of MC Tracks / Ideal Track");
  // Fixing the Y axis
  hNumMcTracksPerIdealTrack->GetYaxis()->SetLabelSize(0.05);
  hNumMcTracksPerIdealTrack->GetYaxis()->SetTitleSize(0.06);
  hNumMcTracksPerIdealTrack->GetYaxis()->SetTitleOffset(0.69);
  hNumMcTracksPerIdealTrack->GetYaxis()->CenterTitle();
  hNumMcTracksPerIdealTrack->GetYaxis()->SetTitle("Occurrence");

  hMomRealistic = new TH2F("hMomRealistic", " ", 500, 0, 5, 1000, -1, 1);
  // Fixing the X axis
  hMomRealistic->GetXaxis()->SetLabelSize(0.05);
  hMomRealistic->GetXaxis()->SetTitleSize(0.06);
  hMomRealistic->GetXaxis()->SetTitleOffset(0.99);
  hMomRealistic->GetXaxis()->CenterTitle();
  hMomRealistic->GetXaxis()->SetTitle("p_{t}^{MC} / GeV/c");
  // Fixing the Y axis
  hMomRealistic->GetYaxis()->SetLabelSize(0.05);
  hMomRealistic->GetYaxis()->SetTitleSize(0.06);
  hMomRealistic->GetYaxis()->SetTitleOffset(0.69);
  hMomRealistic->GetYaxis()->CenterTitle();
  hMomRealistic->GetYaxis()->SetTitle("p_{t}^{RECO} - p_{t}^{MC} / GeV/c");

  hMomRealisticExpanded = new TH2F("hMomRealisticExpanded", " ", 500, 0, 5, 5000, -2, 2);
  // Fixing the X axis
  hMomRealisticExpanded->GetXaxis()->SetLabelSize(0.05);
  hMomRealisticExpanded->GetXaxis()->SetTitleSize(0.06);
  hMomRealisticExpanded->GetXaxis()->SetTitleOffset(0.99);
  hMomRealisticExpanded->GetXaxis()->CenterTitle();
  hMomRealisticExpanded->GetXaxis()->SetTitle("p_{t}^{MC} / GeV/c");
  // Fixing the Y axis
  hMomRealisticExpanded->GetYaxis()->SetLabelSize(0.05);
  hMomRealisticExpanded->GetYaxis()->SetTitleSize(0.06);
  hMomRealisticExpanded->GetYaxis()->SetTitleOffset(0.69);
  hMomRealisticExpanded->GetYaxis()->CenterTitle();
  hMomRealisticExpanded->GetYaxis()->SetTitle("p_{t}^{RECO} - p_{t}^{MC} / GeV/c");

  hRelMomRealistic = new TH2F("hRelMomRealistic", " ", 500, 0, 5, 1000, -1, 1);
  // Fixing the X axis
  hRelMomRealistic->GetXaxis()->SetLabelSize(0.05);
  hRelMomRealistic->GetXaxis()->SetTitleSize(0.06);
  hRelMomRealistic->GetXaxis()->SetTitleOffset(0.99);
  hRelMomRealistic->GetXaxis()->CenterTitle();
  hRelMomRealistic->GetXaxis()->SetTitle("p_{t}^{MC} / GeV/c");
  // Fixing the Y axis
  hRelMomRealistic->GetYaxis()->SetLabelSize(0.05);
  hRelMomRealistic->GetYaxis()->SetTitleSize(0.06);
  hRelMomRealistic->GetYaxis()->SetTitleOffset(0.69);
  hRelMomRealistic->GetYaxis()->CenterTitle();
  hRelMomRealistic->GetYaxis()->SetTitle("(p_{t}^{RECO} - p_{t}^{MC}) / p_{t}^{MC}");

  hRelMomRealisticExpanded = new TH2F("hRelMomRealisticExpanded", " ", 500, 0, 5, 5000, -2, 2);
  // Fixing the X axis
  hRelMomRealisticExpanded->GetXaxis()->SetLabelSize(0.05);
  hRelMomRealisticExpanded->GetXaxis()->SetTitleSize(0.06);
  hRelMomRealisticExpanded->GetXaxis()->SetTitleOffset(0.99);
  hRelMomRealisticExpanded->GetXaxis()->CenterTitle();
  hRelMomRealisticExpanded->GetXaxis()->SetTitle("p_{t}^{MC} / GeV/c");
  // Fixing the Y axis
  hRelMomRealisticExpanded->GetYaxis()->SetLabelSize(0.05);
  hRelMomRealisticExpanded->GetYaxis()->SetTitleSize(0.06);
  hRelMomRealisticExpanded->GetYaxis()->SetTitleOffset(0.69);
  hRelMomRealisticExpanded->GetYaxis()->CenterTitle();
  hRelMomRealisticExpanded->GetYaxis()->SetTitle("(p_{t}^{RECO} - p_{t}^{MC}) / p_{t}^{MC}");

  hMomAngleRealistic = new TH2F("hMomAngleRealistic", " ", 500, 0, 360, 100, -1, 1);
  // Fixing the X axis
  hMomAngleRealistic->GetXaxis()->SetLabelSize(0.05);
  hMomAngleRealistic->GetXaxis()->SetTitleSize(0.06);
  hMomAngleRealistic->GetXaxis()->SetTitleOffset(0.99);
  hMomAngleRealistic->GetXaxis()->CenterTitle();
  hMomAngleRealistic->GetXaxis()->SetTitle("#theta / Degrees");
  // Fixing the Y axis
  hMomAngleRealistic->GetYaxis()->SetLabelSize(0.05);
  hMomAngleRealistic->GetYaxis()->SetTitleSize(0.06);
  hMomAngleRealistic->GetYaxis()->SetTitleOffset(0.69);
  hMomAngleRealistic->GetYaxis()->CenterTitle();
  hMomAngleRealistic->GetYaxis()->SetTitle("p_{t}^{RECO} - p_{t}^{MC} / GeV/c");

  hRelMomAngleRealistic = new TH2F("hRelMomAngleRealistic", " ", 500, 0, 360, 100, -1, 1);
  // Fixing the X axis
  hRelMomAngleRealistic->GetXaxis()->SetLabelSize(0.05);
  hRelMomAngleRealistic->GetXaxis()->SetTitleSize(0.06);
  hRelMomAngleRealistic->GetXaxis()->SetTitleOffset(0.99);
  hRelMomAngleRealistic->GetXaxis()->CenterTitle();
  hRelMomAngleRealistic->GetXaxis()->SetTitle("p_{t}^{MC} / GeV/c");
  // Fixing the Y axis
  hRelMomAngleRealistic->GetYaxis()->SetLabelSize(0.05);
  hRelMomAngleRealistic->GetYaxis()->SetTitleSize(0.06);
  hRelMomAngleRealistic->GetYaxis()->SetTitleOffset(0.69);
  hRelMomAngleRealistic->GetYaxis()->CenterTitle();
  hRelMomAngleRealistic->GetYaxis()->SetTitle("(p_{t}^{RECO} - p_{t}^{MC}) / p_{t}^{MC}");

  hMomNumHitsRealistic = new TH2F("hMomNumHitsRealistic", " ", 100, 0, 100, 100, -1, 1);
  // Fixing the X axis
  hMomNumHitsRealistic->GetXaxis()->SetLabelSize(0.05);
  hMomNumHitsRealistic->GetXaxis()->SetTitleSize(0.06);
  hMomNumHitsRealistic->GetXaxis()->SetTitleOffset(0.99);
  hMomNumHitsRealistic->GetXaxis()->CenterTitle();
  hMomNumHitsRealistic->GetXaxis()->SetTitle("Number of STT Hits");
  // Fixing the Y axis
  hMomNumHitsRealistic->GetYaxis()->SetLabelSize(0.05);
  hMomNumHitsRealistic->GetYaxis()->SetTitleSize(0.06);
  hMomNumHitsRealistic->GetYaxis()->SetTitleOffset(0.69);
  hMomNumHitsRealistic->GetYaxis()->CenterTitle();
  hMomNumHitsRealistic->GetYaxis()->SetTitle("p_{t}^{RECO} - p_{t}^{MC} / GeV/c");

  hRelMomNumHitsRealistic = new TH2F("hRelMomNumHitsRealistic", " ", 100, 0, 100, 100, -1, 1);
  // ixing the X axis
  hRelMomNumHitsRealistic->GetXaxis()->SetLabelSize(0.05);
  hRelMomNumHitsRealistic->GetXaxis()->SetTitleSize(0.06);
  hRelMomNumHitsRealistic->GetXaxis()->SetTitleOffset(0.99);
  hRelMomNumHitsRealistic->GetXaxis()->CenterTitle();
  hRelMomNumHitsRealistic->GetXaxis()->SetTitle("Number of STT Hits");
  // Fixing the Y axis
  hRelMomNumHitsRealistic->GetYaxis()->SetLabelSize(0.05);
  hRelMomNumHitsRealistic->GetYaxis()->SetTitleSize(0.06);
  hRelMomNumHitsRealistic->GetYaxis()->SetTitleOffset(0.69);
  hRelMomNumHitsRealistic->GetYaxis()->CenterTitle();
  hRelMomNumHitsRealistic->GetYaxis()->SetTitle("(p_{t}^{RECO} - p_{t}^{MC}) / p_{t}^{MC}");

  hMomRes = new TH1F("hMomRes", "", 100, -1, 1);
  // ixing the X axis
  hMomRes->GetXaxis()->SetLabelSize(0.05);
  hMomRes->GetXaxis()->SetTitleSize(0.06);
  hMomRes->GetXaxis()->SetTitleOffset(0.99);
  hMomRes->GetXaxis()->CenterTitle();
  hMomRes->GetXaxis()->SetTitle("p_{t}^{RECO} - p_{t}^{MC} / GeV/c");
  // Fixing the Y axis
  hMomRes->GetYaxis()->SetLabelSize(0.05);
  hMomRes->GetYaxis()->SetTitleSize(0.06);
  hMomRes->GetYaxis()->SetTitleOffset(0.69);
  hMomRes->GetYaxis()->CenterTitle();
  hMomRes->GetYaxis()->SetTitle("Occurrence");

  hRelMomRes = new TH1F("hRelMomRes", " ", 100, -1, 1);
  // ixing the X axis
  hRelMomRes->GetXaxis()->SetLabelSize(0.05);
  hRelMomRes->GetXaxis()->SetTitleSize(0.06);
  hRelMomRes->GetXaxis()->SetTitleOffset(0.99);
  hRelMomRes->GetXaxis()->CenterTitle();
  hRelMomRes->GetXaxis()->SetTitle("(p_{t}^{RECO} - p_{t}^{MC}) / p_{t}^{MC}");
  // Fixing the Y axis
  hRelMomRes->GetYaxis()->SetLabelSize(0.05);
  hRelMomRes->GetYaxis()->SetTitleSize(0.06);
  hRelMomRes->GetYaxis()->SetTitleOffset(0.69);
  hRelMomRes->GetYaxis()->CenterTitle();
  hRelMomRes->GetYaxis()->SetTitle("Occurrence");

  hMomResFiltered = new TH1F("hMomResFiltered", "", 100, -1, 1);
  // ixing the X axis
  hMomResFiltered->GetXaxis()->SetLabelSize(0.05);
  hMomResFiltered->GetXaxis()->SetTitleSize(0.06);
  hMomResFiltered->GetXaxis()->SetTitleOffset(0.99);
  hMomResFiltered->GetXaxis()->CenterTitle();
  hMomResFiltered->GetXaxis()->SetTitle("p_{t}^{RECO} - p_{t}^{MC} / GeV/c");
  // Fixing the Y axis
  hMomResFiltered->GetYaxis()->SetLabelSize(0.05);
  hMomResFiltered->GetYaxis()->SetTitleSize(0.06);
  hMomResFiltered->GetYaxis()->SetTitleOffset(0.69);
  hMomResFiltered->GetYaxis()->CenterTitle();
  hMomResFiltered->GetYaxis()->SetTitle("Occurrence");

  hMomRelResFiltered = new TH1F("hMomRelResFiltered", "", 100, -1, 1);
  // ixing the X axis
  hMomRelResFiltered->GetXaxis()->SetLabelSize(0.05);
  hMomRelResFiltered->GetXaxis()->SetTitleSize(0.06);
  hMomRelResFiltered->GetXaxis()->SetTitleOffset(0.99);
  hMomRelResFiltered->GetXaxis()->CenterTitle();
  hMomRelResFiltered->GetXaxis()->SetTitle("(p_{t}^{RECO} - p_{t}^{MC}) / p_{t}^{MC}");
  // Fixing the Y axis
  hMomRelResFiltered->GetYaxis()->SetLabelSize(0.05);
  hMomRelResFiltered->GetYaxis()->SetTitleSize(0.06);
  hMomRelResFiltered->GetYaxis()->SetTitleOffset(0.69);
  hMomRelResFiltered->GetYaxis()->CenterTitle();
  hMomRelResFiltered->GetYaxis()->SetTitle("Occurrence");

  hMomResWithCut = new TH1F("hMomResWithCut", " ", 100, -1, 1);
  // ixing the X axis
  hMomResWithCut->GetXaxis()->SetLabelSize(0.05);
  hMomResWithCut->GetXaxis()->SetTitleSize(0.06);
  hMomResWithCut->GetXaxis()->SetTitleOffset(0.99);
  hMomResWithCut->GetXaxis()->CenterTitle();
  hMomResWithCut->GetXaxis()->SetTitle("p_{t}^{RECO} - p_{t}^{MC} / GeV/c");
  // Fixing the Y axis
  hMomResWithCut->GetYaxis()->SetLabelSize(0.05);
  hMomResWithCut->GetYaxis()->SetTitleSize(0.06);
  hMomResWithCut->GetYaxis()->SetTitleOffset(0.69);
  hMomResWithCut->GetYaxis()->CenterTitle();
  hMomResWithCut->GetYaxis()->SetTitle("Occurrence");

  hMomRelResWithCut = new TH1F("hMomRelResWithCut", " ", 100, -1, 1);
  // ixing the X axis
  hMomRelResWithCut->GetXaxis()->SetLabelSize(0.05);
  hMomRelResWithCut->GetXaxis()->SetTitleSize(0.06);
  hMomRelResWithCut->GetXaxis()->SetTitleOffset(0.99);
  hMomRelResWithCut->GetXaxis()->CenterTitle();
  hMomRelResWithCut->GetXaxis()->SetTitle("(p_{t}^{RECO} - p_{t}^{MC}) / p_{t}^{MC}");
  // Fixing the Y axis
  hMomRelResWithCut->GetYaxis()->SetLabelSize(0.05);
  hMomRelResWithCut->GetYaxis()->SetTitleSize(0.06);
  hMomRelResWithCut->GetYaxis()->SetTitleOffset(0.69);
  hMomRelResWithCut->GetYaxis()->CenterTitle();
  hMomRelResWithCut->GetYaxis()->SetTitle("Occurrence");

  hMomResCutAwayPart = new TH1F("hMomResCutAwayPart", " ", 100, -1, 1);
  // ixing the X axis
  hMomResCutAwayPart->GetXaxis()->SetLabelSize(0.05);
  hMomResCutAwayPart->GetXaxis()->SetTitleSize(0.06);
  hMomResCutAwayPart->GetXaxis()->SetTitleOffset(0.99);
  hMomResCutAwayPart->GetXaxis()->CenterTitle();
  hMomResCutAwayPart->GetXaxis()->SetTitle("p_{t}^{RECO} - p_{t}^{MC} / GeV/c");
  // Fixing the Y axis
  hMomResCutAwayPart->GetYaxis()->SetLabelSize(0.05);
  hMomResCutAwayPart->GetYaxis()->SetTitleSize(0.06);
  hMomResCutAwayPart->GetYaxis()->SetTitleOffset(0.69);
  hMomResCutAwayPart->GetYaxis()->CenterTitle();
  hMomResCutAwayPart->GetYaxis()->SetTitle("Occurrence");

  hMomRelResCutAwayPart = new TH1F("hMomRelResCutAwayPart", " ", 100, -1, 1);
  // ixing the X axis
  hMomRelResCutAwayPart->GetXaxis()->SetLabelSize(0.05);
  hMomRelResCutAwayPart->GetXaxis()->SetTitleSize(0.06);
  hMomRelResCutAwayPart->GetXaxis()->SetTitleOffset(0.99);
  hMomRelResCutAwayPart->GetXaxis()->CenterTitle();
  hMomRelResCutAwayPart->GetXaxis()->SetTitle("(p_{t}^{RECO} - p_{t}^{MC}) / p_{t}^{MC}");
  // Fixing the Y axis
  hMomRelResCutAwayPart->GetYaxis()->SetLabelSize(0.05);
  hMomRelResCutAwayPart->GetYaxis()->SetTitleSize(0.06);
  hMomRelResCutAwayPart->GetYaxis()->SetTitleOffset(0.69);
  hMomRelResCutAwayPart->GetYaxis()->CenterTitle();
  hMomRelResCutAwayPart->GetYaxis()->SetTitle("Occurrence");

  hMomResWithCutPTCut = new TH1F("hMomResWithCutPTCut", " ", 100, -1, 1);
  // ixing the X axis
  hMomResWithCutPTCut->GetXaxis()->SetLabelSize(0.05);
  hMomResWithCutPTCut->GetXaxis()->SetTitleSize(0.06);
  hMomResWithCutPTCut->GetXaxis()->SetTitleOffset(0.99);
  hMomResWithCutPTCut->GetXaxis()->CenterTitle();
  hMomResWithCutPTCut->GetXaxis()->SetTitle("p_{t}^{RECO} - p_{t}^{MC} / GeV/c");
  // Fixing the Y axis
  hMomResWithCutPTCut->GetYaxis()->SetLabelSize(0.05);
  hMomResWithCutPTCut->GetYaxis()->SetTitleSize(0.06);
  hMomResWithCutPTCut->GetYaxis()->SetTitleOffset(0.69);
  hMomResWithCutPTCut->GetYaxis()->CenterTitle();
  hMomResWithCutPTCut->GetYaxis()->SetTitle("Occurrence");

  hMomRelResWithCutPTCut = new TH1F("hMomRelResWithCutPTCut", " ", 100, -1, 1);
  // ixing the X axis
  hMomRelResWithCutPTCut->GetXaxis()->SetLabelSize(0.05);
  hMomRelResWithCutPTCut->GetXaxis()->SetTitleSize(0.06);
  hMomRelResWithCutPTCut->GetXaxis()->SetTitleOffset(0.99);
  hMomRelResWithCutPTCut->GetXaxis()->CenterTitle();
  hMomRelResWithCutPTCut->GetXaxis()->SetTitle("(p_{t}^{RECO} - p_{t}^{MC}) / p_{t}^{MC}");
  // Fixing the Y axis
  hMomRelResWithCutPTCut->GetYaxis()->SetLabelSize(0.05);
  hMomRelResWithCutPTCut->GetYaxis()->SetTitleSize(0.06);
  hMomRelResWithCutPTCut->GetYaxis()->SetTitleOffset(0.69);
  hMomRelResWithCutPTCut->GetYaxis()->CenterTitle();
  hMomRelResWithCutPTCut->GetYaxis()->SetTitle("Occurrence");

  hMomResCutAwayPartPTCut = new TH1F("hMomResCutAwayPartPTCut", " ", 100, -1, 1);
  // ixing the X axis
  hMomResCutAwayPartPTCut->GetXaxis()->SetLabelSize(0.05);
  hMomResCutAwayPartPTCut->GetXaxis()->SetTitleSize(0.06);
  hMomResCutAwayPartPTCut->GetXaxis()->SetTitleOffset(0.99);
  hMomResCutAwayPartPTCut->GetXaxis()->CenterTitle();
  hMomResCutAwayPartPTCut->GetXaxis()->SetTitle("p_{t}^{RECO} - p_{t}^{MC} / GeV/c");
  // Fixing the Y axis
  hMomResCutAwayPartPTCut->GetYaxis()->SetLabelSize(0.05);
  hMomResCutAwayPartPTCut->GetYaxis()->SetTitleSize(0.06);
  hMomResCutAwayPartPTCut->GetYaxis()->SetTitleOffset(0.69);
  hMomResCutAwayPartPTCut->GetYaxis()->CenterTitle();
  hMomResCutAwayPartPTCut->GetYaxis()->SetTitle("Occurrence");

  hMomRelResCutAwayPartPTCut = new TH1F("hMomRelResCutAwayPartPTCut", " ", 100, -1, 1);
  // Fixing the X axis
  hMomRelResCutAwayPartPTCut->GetXaxis()->SetLabelSize(0.05);
  hMomRelResCutAwayPartPTCut->GetXaxis()->SetTitleSize(0.06);
  hMomRelResCutAwayPartPTCut->GetXaxis()->SetTitleOffset(0.99);
  hMomRelResCutAwayPartPTCut->GetXaxis()->CenterTitle();
  hMomRelResCutAwayPartPTCut->GetXaxis()->SetTitle("(p_{t}^{RECO} - p_{t}^{MC}) / p_{t}^{MC}");
  // Fixing the Y axis
  hMomRelResCutAwayPartPTCut->GetYaxis()->SetLabelSize(0.05);
  hMomRelResCutAwayPartPTCut->GetYaxis()->SetTitleSize(0.06);
  hMomRelResCutAwayPartPTCut->GetYaxis()->SetTitleOffset(0.69);
  hMomRelResCutAwayPartPTCut->GetYaxis()->CenterTitle();
  hMomRelResCutAwayPartPTCut->GetYaxis()->SetTitle("Occurrence");

  hPositiveRes_NumHits = new TH2F("hPositiveRes_NumHits", " ", 100, 0, 100, 100, -1, 1);
  hNegativeRes_NumHits = new TH2F("hNegativeRes_NumHits", " ", 100, 0, 100, 100, -1, 1);
  hGoodRes_NumHits = new TH2F("hGoodRes_NumHits", " ", 100, 0, 100, 100, -1, 1);
  hPositiveRes_RecoMom = new TH2F("hPositiveRes_RecoMom", " ", 100, 0, 10, 100, -1, 1);
  hNegativeRes_RecoMom = new TH2F("hNegativeRes_RecoMom", " ", 100, 0, 10, 100, -1, 1);
  hGoodRes_RecoMom = new TH2F("hGoodRes_RecoMom", " ", 100, 0, 10, 100, -1, 1);
  hPositiveRes_MCMom = new TH2F("hPositiveRes_MCMom", " ", 100, 0, 6, 100, -1, 1);
  hNegativeRes_MCMom = new TH2F("hNegativeRes_MCMom", " ", 100, 0, 6, 100, -1, 1);
  hGoodRes_MCMom = new TH2F("hGoodRes_MCMom", " ", 100, 0, 6, 100, -1, 1);
  hPositiveRes_MCTheta = new TH2F("hPositiveRes_MCTheta", " ", 100, 0, 360, 100, -1, 1);
  hNegativeRes_MCTheta = new TH2F("hNegativeRes_MCTheta", " ", 100, 0, 360, 100, -1, 1);
  hGoodRes_MCTheta = new TH2F("hGoodRes_MCTheta", " ", 100, 0, 360, 100, -1, 1);
  hPositiveRes_NumMCTracks = new TH2F("hPositiveRes_NumMCtracks", " ", 20, 0, 20, 100, -1, 1);
  hNegativeRes_NumMCTracks = new TH2F("hNegativeRes_NumMCTracks", " ", 20, 0, 20, 100, -1, 1);
  hGoodRes_NumMCTracks = new TH2F("hGoodRes_NumMCTracks", " ", 20, 0, 20, 100, -1, 1);

  hPositiveRelRes_NumHits = new TH2F("hPositiveRelRes_NumHits", " ", 100, 0, 100, 100, -1, 1);
  hNegativeRelRes_NumHits = new TH2F("hNegativeRelRes_NumHits", " ", 100, 0, 100, 100, -1, 1);
  hGoodRelRes_NumHits = new TH2F("hGoodRelRes_NumHits", " ", 100, 0, 100, 100, -1, 1);
  hPositiveRelRes_RecoMom = new TH2F("hPositiveRelRes_RecoMom", " ", 100, 0, 10, 100, -1, 1);
  hNegativeRelRes_RecoMom = new TH2F("hNegativeRelRes_RecoMom", " ", 100, 0, 10, 100, -1, 1);
  hGoodRelRes_RecoMom = new TH2F("hGoodRelRes_RecoMom", " ", 100, 0, 10, 100, -1, 1);
  hPositiveRelRes_MCMom = new TH2F("hPositiveRelRes_MCMom", " ", 100, 0, 6, 100, -1, 1);
  hNegativeRelRes_MCMom = new TH2F("hNegativeRelRes_MCMom", " ", 100, 0, 6, 100, -1, 1);
  hGoodRelRes_MCMom = new TH2F("hGoodRelRes_MCMom", " ", 100, 0, 6, 100, -1, 1);
  hPositiveRelRes_MCTheta = new TH2F("hPositiveRelRes_MCTheta", " ", 100, 0, 360, 100, -1, 1);
  hNegativeRelRes_MCTheta = new TH2F("hNegativeRelRes_MCTheta", " ", 100, 0, 360, 100, -1, 1);
  hGoodRelRes_MCTheta = new TH2F("hGoodRelRes_MCTheta", " ", 100, 0, 360, 100, -1, 1);
  hPositiveRelRes_NumMCTracks = new TH2F("hPositiveRelRes_NumMCtracks", " ", 20, 0, 20, 100, -1, 1);
  hNegativeRelRes_NumMCTracks = new TH2F("hNegativeRelRes_NumMCTracks", " ", 20, 0, 20, 100, -1, 1);
  hGoodRelRes_NumMCTracks = new TH2F("hGoodRelRes_NumMCTracks", " ", 20, 0, 20, 100, -1, 1);

  hAllRes_NumMCTracks = new TH2F("hAllRes_NumMCTracks", " ", 20, 0, 20, 100, -1, 1);
  // Fixing the X axis
  hAllRes_NumMCTracks->GetXaxis()->SetLabelSize(0.05);
  hAllRes_NumMCTracks->GetXaxis()->SetTitleSize(0.06);
  hAllRes_NumMCTracks->GetXaxis()->SetTitleOffset(0.99);
  hAllRes_NumMCTracks->GetXaxis()->CenterTitle();
  hAllRes_NumMCTracks->GetXaxis()->SetTitle("Number of MC tracks per RECO track");
  // Fixing the Y axis
  hAllRes_NumMCTracks->GetYaxis()->SetLabelSize(0.05);
  hAllRes_NumMCTracks->GetYaxis()->SetTitleSize(0.06);
  hAllRes_NumMCTracks->GetYaxis()->SetTitleOffset(0.69);
  hAllRes_NumMCTracks->GetYaxis()->CenterTitle();
  hAllRes_NumMCTracks->GetYaxis()->SetTitle("(p_{t}^{RECO} - p_{t}^{MC}) / GeV/c");

  hAllRelRes_NumMCTracks = new TH2F("hAllRelRes_NumMCTracks", " ", 20, 0, 20, 100, -1, 1);
  // Fixing the X axis
  hAllRelRes_NumMCTracks->GetXaxis()->SetLabelSize(0.05);
  hAllRelRes_NumMCTracks->GetXaxis()->SetTitleSize(0.06);
  hAllRelRes_NumMCTracks->GetXaxis()->SetTitleOffset(0.99);
  hAllRelRes_NumMCTracks->GetXaxis()->CenterTitle();
  hAllRelRes_NumMCTracks->GetXaxis()->SetTitle("Number of MC tracks per RECO track");
  // Fixing the Y axis
  hAllRelRes_NumMCTracks->GetYaxis()->SetLabelSize(0.05);
  hAllRelRes_NumMCTracks->GetYaxis()->SetTitleSize(0.06);
  hAllRelRes_NumMCTracks->GetYaxis()->SetTitleOffset(0.69);
  hAllRelRes_NumMCTracks->GetYaxis()->CenterTitle();
  hAllRelRes_NumMCTracks->GetYaxis()->SetTitle("(p_{t}^{RECO} - p_{t}^{MC}) / p_{t}^{MC}");

  hNumMCTracksAbsolute = new TH2F("hNumMCTracksAbsolute", " ", 4, 0, 4, 1000, -1, 1);
  // Fixing the X axis
  hNumMCTracksAbsolute->GetXaxis()->SetLabelSize(0.05);
  hNumMCTracksAbsolute->GetXaxis()->SetTitleSize(0.06);
  hNumMCTracksAbsolute->GetXaxis()->SetTitleOffset(0.99);
  hNumMCTracksAbsolute->GetXaxis()->CenterTitle();
  hNumMCTracksAbsolute->GetXaxis()->SetTitle("Number of MC tracks per RECO track");
  // Fixing the Y axis
  hNumMCTracksAbsolute->GetYaxis()->SetLabelSize(0.05);
  hNumMCTracksAbsolute->GetYaxis()->SetTitleSize(0.06);
  hNumMCTracksAbsolute->GetYaxis()->SetTitleOffset(0.69);
  hNumMCTracksAbsolute->GetYaxis()->CenterTitle();
  hNumMCTracksAbsolute->GetYaxis()->SetTitle("(p_{t}^{RECO} - p_{t}^{MC}) / GeV/c");

  hNumMCTracksRelative = new TH2F("hNumMCTracksRelative", " ", 4, 0, 4, 1000, -1, 1);
  // Fixing the X axis
  hNumMCTracksRelative->GetXaxis()->SetLabelSize(0.05);
  hNumMCTracksRelative->GetXaxis()->SetTitleSize(0.06);
  hNumMCTracksRelative->GetXaxis()->SetTitleOffset(0.99);
  hNumMCTracksRelative->GetXaxis()->CenterTitle();
  hNumMCTracksRelative->GetXaxis()->SetTitle("Number of MC tracks per RECO track");
  // Fixing the Y axis
  hNumMCTracksRelative->GetYaxis()->SetLabelSize(0.05);
  hNumMCTracksRelative->GetYaxis()->SetTitleSize(0.06);
  hNumMCTracksRelative->GetYaxis()->SetTitleOffset(0.69);
  hNumMCTracksRelative->GetYaxis()->CenterTitle();
  hNumMCTracksRelative->GetYaxis()->SetTitle("(p_{t}^{RECO} - p_{t}^{MC}) / p_{t}^{MC}");

  hMomFiltered = new TH2F("hMomFiltered", " ", 500, 0, 5, 100, -1, 1);
  // Fixing the X axis
  hMomFiltered->GetXaxis()->SetLabelSize(0.05);
  hMomFiltered->GetXaxis()->SetTitleSize(0.06);
  hMomFiltered->GetXaxis()->SetTitleOffset(0.99);
  hMomFiltered->GetXaxis()->CenterTitle();
  hMomFiltered->GetXaxis()->SetTitle("p_{t}^{MC} / GeV/c");
  // Fixing the Y axis
  hMomFiltered->GetYaxis()->SetLabelSize(0.05);
  hMomFiltered->GetYaxis()->SetTitleSize(0.06);
  hMomFiltered->GetYaxis()->SetTitleOffset(0.69);
  hMomFiltered->GetYaxis()->CenterTitle();
  hMomFiltered->GetYaxis()->SetTitle("(p_{t}^{RECO} - p_{t}^{MC}) / GeV/c");

  hMomRelFiltered = new TH2F("hMomRelFiltered", " ", 500, 0, 5, 100, -1, 1);
  // Fixing the X axis
  hMomRelFiltered->GetXaxis()->SetLabelSize(0.05);
  hMomRelFiltered->GetXaxis()->SetTitleSize(0.06);
  hMomRelFiltered->GetXaxis()->SetTitleOffset(0.99);
  hMomRelFiltered->GetXaxis()->CenterTitle();
  hMomRelFiltered->GetXaxis()->SetTitle("p_{t}^{MC} / GeV/c");
  // Fixing the Y axis
  hMomRelFiltered->GetYaxis()->SetLabelSize(0.05);
  hMomRelFiltered->GetYaxis()->SetTitleSize(0.06);
  hMomRelFiltered->GetYaxis()->SetTitleOffset(0.69);
  hMomRelFiltered->GetYaxis()->CenterTitle();
  hMomRelFiltered->GetYaxis()->SetTitle("(p_{t}^{RECO} - p_{t}^{MC}) / p_{t}^{MC}");

  hTotMom = new TH2F("hTotMom", " ", 500, 0, 5, 100, -1, 1);
  // Fixing the X axis
  hTotMom->GetXaxis()->SetLabelSize(0.05);
  hTotMom->GetXaxis()->SetTitleSize(0.06);
  hTotMom->GetXaxis()->SetTitleOffset(0.99);
  hTotMom->GetXaxis()->CenterTitle();
  hTotMom->GetXaxis()->SetTitle("p_{tot}^{MC} / GeV/c");
  // Fixing the Y axis
  hTotMom->GetYaxis()->SetLabelSize(0.05);
  hTotMom->GetYaxis()->SetTitleSize(0.06);
  hTotMom->GetYaxis()->SetTitleOffset(0.69);
  hTotMom->GetYaxis()->CenterTitle();
  hTotMom->GetYaxis()->SetTitle("(p_{t}^{RECO} - p_{t}^{MC}) / GeV/c");

  hTotMomRel = new TH2F("hTotMomRel", " ", 500, 0, 5, 100, -1, 1);
  // Fixing the X axis
  hTotMomRel->GetXaxis()->SetLabelSize(0.05);
  hTotMomRel->GetXaxis()->SetTitleSize(0.06);
  hTotMomRel->GetXaxis()->SetTitleOffset(0.99);
  hTotMomRel->GetXaxis()->CenterTitle();
  hTotMomRel->GetXaxis()->SetTitle("p_{tot}^{MC} / GeV/c");
  // Fixing the Y axis
  hTotMomRel->GetYaxis()->SetLabelSize(0.05);
  hTotMomRel->GetYaxis()->SetTitleSize(0.06);
  hTotMomRel->GetYaxis()->SetTitleOffset(0.69);
  hTotMomRel->GetYaxis()->CenterTitle();
  hTotMomRel->GetYaxis()->SetTitle("(p_{t}^{RECO} - p_{t}^{MC}) / p_{t}^{MC}");

  hTotMomFiltered = new TH2F("hTotMomFiltered", " ", 500, 0, 5, 100, -1, 1);
  // Fixing the X axis
  hTotMomFiltered->GetXaxis()->SetLabelSize(0.05);
  hTotMomFiltered->GetXaxis()->SetTitleSize(0.06);
  hTotMomFiltered->GetXaxis()->SetTitleOffset(0.99);
  hTotMomFiltered->GetXaxis()->CenterTitle();
  hTotMomFiltered->GetXaxis()->SetTitle("p_{tot}^{MC} / GeV/c");
  // Fixing the Y axis
  hTotMomFiltered->GetYaxis()->SetLabelSize(0.05);
  hTotMomFiltered->GetYaxis()->SetTitleSize(0.06);
  hTotMomFiltered->GetYaxis()->SetTitleOffset(0.69);
  hTotMomFiltered->GetYaxis()->CenterTitle();
  hTotMomFiltered->GetYaxis()->SetTitle("(p_{t}^{RECO} - p_{t}^{MC}) / GeV/c");

  hTotMomRelFiltered = new TH2F("hTotMomRelFiltered", " ", 500, 0, 5, 100, -1, 1);
  // Fixing the X axis
  hTotMomRelFiltered->GetXaxis()->SetLabelSize(0.05);
  hTotMomRelFiltered->GetXaxis()->SetTitleSize(0.06);
  hTotMomRelFiltered->GetXaxis()->SetTitleOffset(0.99);
  hTotMomRelFiltered->GetXaxis()->CenterTitle();
  hTotMomRelFiltered->GetXaxis()->SetTitle("p_{tot}^{MC} / GeV/c");
  // Fixing the Y axis
  hTotMomRelFiltered->GetYaxis()->SetLabelSize(0.05);
  hTotMomRelFiltered->GetYaxis()->SetTitleSize(0.06);
  hTotMomRelFiltered->GetYaxis()->SetTitleOffset(0.69);
  hTotMomRelFiltered->GetYaxis()->CenterTitle();
  hTotMomRelFiltered->GetYaxis()->SetTitle("(p_{t}^{RECO} - p_{t}^{MC}) / p_{t}^{MC}");
}

void PndSTEAnalysisTask::FinishEvent()
{

  LOG(debug) << "------ PndSTEAnalysisTask::FinishEvent() ------" << endl;

  fMapRecoTrackIndices.clear();
  fMapBToFHitIndices.clear();
}

void PndSTEAnalysisTask::Finish()
{

  hMomRes->Write();
  hRelMomRes->Write();
  hMomResFiltered->Write();
  hMomRelResFiltered->Write();
  hMomResWithCut->Write();
  hMomRelResWithCut->Write();
  hMomResCutAwayPart->Write();
  hMomRelResCutAwayPart->Write();

  hMomResWithCutPTCut->Write();
  hMomRelResWithCutPTCut->Write();
  hMomResCutAwayPartPTCut->Write();
  hMomRelResCutAwayPartPTCut->Write();

  hMomRealistic->Write();
  hRelMomRealistic->Write();

  TH1D *hMomRealisticLargePt = (TH1D *)hMomRealistic->ProjectionY("Proj Large Pt ", 52, 499);
  // TH1D* hMomRealisticLargeMidPt = (TH1D* ) hMomRealistic->ProjectionY("Proj Large Mid Pt ",52,100);
  TH1D *hMomRealisticSmallMidPt = (TH1D *)hMomRealistic->ProjectionY("Proj Small Mid Pt ", 12, 52);
  TH1D *hMomRealisticSmallPt = (TH1D *)hMomRealistic->ProjectionY("Proj Small Pt ", 1, 12);

  hMomRealisticLargePt->GetXaxis()->SetTitleOffset(0.99);

  // Fixing the Y axis
  hMomRealisticLargePt->GetYaxis()->SetLabelSize(0.05);
  hMomRealisticLargePt->GetYaxis()->SetTitleSize(0.06);
  hMomRealisticLargePt->GetYaxis()->SetTitleOffset(0.69);
  hMomRealisticLargePt->GetYaxis()->CenterTitle();
  hMomRealisticLargePt->GetYaxis()->SetTitle("Occurrence");

  hMomRealisticSmallPt->GetXaxis()->SetTitleOffset(0.99);

  // Fixing the Y axis
  hMomRealisticSmallPt->GetYaxis()->SetLabelSize(0.05);
  hMomRealisticSmallPt->GetYaxis()->SetTitleSize(0.06);
  hMomRealisticSmallPt->GetYaxis()->SetTitleOffset(0.69);
  hMomRealisticSmallPt->GetYaxis()->CenterTitle();
  hMomRealisticSmallPt->GetYaxis()->SetTitle("Occurrence");

  TH1D *hMomRelRealisticLargePt = (TH1D *)hRelMomRealistic->ProjectionY("Proj Large Pt, Rel Mom ", 52, 499);
  // TH1D* hMomRelRealisticLargeMidPt = (TH1D* ) hRelMomRealistic->ProjectionY("Proj Large Mid Pt, Rel Mom ",52,100);
  TH1D *hMomRelRealisticSmallMidPt = (TH1D *)hRelMomRealistic->ProjectionY("Proj Small Mid Pt,Rel Mom ", 12, 52);
  TH1D *hMomRelRealisticSmallPt = (TH1D *)hRelMomRealistic->ProjectionY("Proj Small Pt, Rel Mom ", 1, 12);

  hMomRelRealisticLargePt->GetXaxis()->SetTitleOffset(0.99);

  // Fixing the Y axis
  hMomRelRealisticLargePt->GetYaxis()->SetLabelSize(0.05);
  hMomRelRealisticLargePt->GetYaxis()->SetTitleSize(0.06);
  hMomRelRealisticLargePt->GetYaxis()->SetTitleOffset(0.69);
  hMomRelRealisticLargePt->GetYaxis()->CenterTitle();
  hMomRelRealisticLargePt->GetYaxis()->SetTitle("Occurrence");

  hMomRelRealisticSmallPt->GetXaxis()->SetTitleOffset(0.99);

  // Fixing the Y axis
  hMomRelRealisticSmallPt->GetYaxis()->SetLabelSize(0.05);
  hMomRelRealisticSmallPt->GetYaxis()->SetTitleSize(0.06);
  hMomRelRealisticSmallPt->GetYaxis()->SetTitleOffset(0.69);
  hMomRelRealisticSmallPt->GetYaxis()->CenterTitle();
  hMomRelRealisticSmallPt->GetYaxis()->SetTitle("Occurrence");

  hMomRealisticLargePt->Write();
  // hMomRealisticLargeMidPt->Write();
  hMomRealisticSmallMidPt->Write();
  hMomRealisticSmallPt->Write();
  hMomRelRealisticLargePt->Write();
  // hMomRelRealisticLargeMidPt->Write();
  hMomRelRealisticSmallMidPt->Write();
  hMomRelRealisticSmallPt->Write();

  hMomAngleRealistic->Write();
  hRelMomAngleRealistic->Write();
  hMomNumHitsRealistic->Write();
  hRelMomNumHitsRealistic->Write();

  hPositiveRes_NumHits->Write();
  hNegativeRes_NumHits->Write();
  hGoodRes_NumHits->Write();
  hPositiveRes_RecoMom->Write();
  hNegativeRes_RecoMom->Write();
  hGoodRes_RecoMom->Write();
  hPositiveRes_MCMom->Write();
  hNegativeRes_MCMom->Write();
  hGoodRes_MCMom->Write();
  hPositiveRes_MCTheta->Write();
  hNegativeRes_MCTheta->Write();
  hGoodRes_MCTheta->Write();
  hPositiveRes_NumMCTracks->Write();
  hNegativeRes_NumMCTracks->Write();
  hGoodRes_NumMCTracks->Write();

  hPositiveRelRes_NumHits->Write();
  hNegativeRelRes_NumHits->Write();
  hGoodRelRes_NumHits->Write();
  hPositiveRelRes_RecoMom->Write();
  hNegativeRelRes_RecoMom->Write();
  hGoodRelRes_RecoMom->Write();
  hPositiveRelRes_MCMom->Write();
  hNegativeRelRes_MCMom->Write();
  hGoodRelRes_MCMom->Write();
  hPositiveRelRes_MCTheta->Write();
  hNegativeRelRes_MCTheta->Write();
  hGoodRelRes_MCTheta->Write();
  hPositiveRelRes_NumMCTracks->Write();
  hNegativeRelRes_NumMCTracks->Write();
  hGoodRelRes_NumMCTracks->Write();

  hMomFiltered->Write();
  hMomRelFiltered->Write();
  // TH1D* hMomRelFilteredLargePt = new TH1D("hMomRelFilteredLargePt", " ", 100, -1, 1);

  TH1D *hMomFilteredLargePt = (TH1D *)hMomFiltered->ProjectionY("Proj Large Pt, Mom Filtered", 12, 100);
  TH1D *hMomFilteredSmallPt = (TH1D *)hMomFiltered->ProjectionY("Proj Small Pt, Mom Filtered ", 1, 12);

  TH1D *hMomRelFilteredLargePt = (TH1D *)hMomRelFiltered->ProjectionY("Proj Large Pt, Mom Rel Filtered ", 12, 100);
  TH1D *hMomRelFilteredSmallPt = (TH1D *)hMomRelFiltered->ProjectionY("Proj Small Pt, Mom Rel Filtered ", 1, 12);

  hMomFilteredLargePt->GetXaxis()->SetTitleOffset(0.99);

  // Fixing the Y axis
  hMomFilteredLargePt->GetYaxis()->SetLabelSize(0.05);
  hMomFilteredLargePt->GetYaxis()->SetTitleSize(0.06);
  hMomFilteredLargePt->GetYaxis()->SetTitleOffset(0.69);
  hMomFilteredLargePt->GetYaxis()->CenterTitle();
  hMomFilteredLargePt->GetYaxis()->SetTitle("Occurrence");

  hMomFilteredSmallPt->GetXaxis()->SetTitleOffset(0.99);

  // Fixing the Y axis
  hMomFilteredSmallPt->GetYaxis()->SetLabelSize(0.05);
  hMomFilteredSmallPt->GetYaxis()->SetTitleSize(0.06);
  hMomFilteredSmallPt->GetYaxis()->SetTitleOffset(0.69);
  hMomFilteredSmallPt->GetYaxis()->CenterTitle();
  hMomFilteredSmallPt->GetYaxis()->SetTitle("Occurrence");

  hMomRelFilteredLargePt->GetXaxis()->SetTitleOffset(0.99);

  // Fixing the Y axis
  hMomRelFilteredLargePt->GetYaxis()->SetLabelSize(0.05);
  hMomRelFilteredLargePt->GetYaxis()->SetTitleSize(0.06);
  hMomRelFilteredLargePt->GetYaxis()->SetTitleOffset(0.69);
  hMomRelFilteredLargePt->GetYaxis()->CenterTitle();
  hMomRelFilteredLargePt->GetYaxis()->SetTitle("Occurrence");

  hMomRelFilteredSmallPt->GetXaxis()->SetTitleOffset(0.99);

  // Fixing the Y axis
  hMomRelFilteredSmallPt->GetYaxis()->SetLabelSize(0.05);
  hMomRelFilteredSmallPt->GetYaxis()->SetTitleSize(0.06);
  hMomRelFilteredSmallPt->GetYaxis()->SetTitleOffset(0.69);
  hMomRelFilteredSmallPt->GetYaxis()->CenterTitle();
  hMomRelFilteredSmallPt->GetYaxis()->SetTitle("Occurrence");

  hMomFilteredLargePt->Write();
  hMomFilteredSmallPt->Write();

  hMomRelFilteredLargePt->Write();
  hMomRelFilteredSmallPt->Write();

  hTotMom->Write();
  hTotMomRel->Write();

  hTotMomFiltered->Write();
  hTotMomRelFiltered->Write();

  hNumMCTracksAbsolute->Write();
  hNumMCTracksRelative->Write();

  hNumMcTracksPerIdealTrack->Write();

  hMomRealisticExpanded->Write();
  hRelMomRealisticExpanded->Write();

  if (fAnalyzeBtofHits == true) {
    cout << "------------------- PndSTEAnalysisTask ---------------------" << endl;
    cout << "Correctly assigned hits: " << fCorrectlyAssignedHits << endl;
    cout << "Wrongly assigned hits: " << fWronglyAssignedHits << endl;
    cout << "Not assigned hits: " << fNotAssignedHits << endl;
  }
}