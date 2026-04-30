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

#include "PndSTEMvdIdeal.h"
#include "PndSTEMvdHitFinder.h"
#include "PndSTETools.h"
#include "PndSTEDraw.h"
#include "TString.h"
#include "PndSttHit.h"
#include "PndSdsHit.h"
#include "FairRootManager.h"
#include "FairLogger.h"
#include <vector>
#include <numeric>
#include <functional>

using std::cout;
using std::endl;

ClassImp(PndSTEMvdIdeal);

void PndSTEMvdIdeal::AddHits(TClonesArray *inputTrackArray, TClonesArray *inputTrackCandArray, TClonesArray *inputPixelArray, TClonesArray *inputStripArray)
{

  LOG(debug) << "------ PndSTEMvdIdeal::AddHits() ------" << endl;

  fMapMvdHitBranchId.clear();

  Int_t branchIdPixel = FairRootManager::Instance()->GetBranchId("MVDHitsPixel");
  Int_t branchIdStrip = FairRootManager::Instance()->GetBranchId("MVDHitsStrip");

  for (int j = 0; j < (int)inputPixelArray->GetEntries(); j++) {
    PndSdsHit *mvdHit = static_cast<PndSdsHit *>(inputPixelArray->At(j));
    fMapMvdHitBranchId[mvdHit] = branchIdPixel;
  }

  for (int j = 0; j < (int)inputStripArray->GetEntries(); j++) {
    PndSdsHit *mvdHit = static_cast<PndSdsHit *>(inputStripArray->At(j));
    fMapMvdHitBranchId[mvdHit] = branchIdStrip;
  }

  PndRiemannTrack mvdRiemannTrack;
  PndRiemannTrack origmvdRiemannTrack;
  PndTrack myMvdTrack;
  PndTrack *tempTrack;

  fTracks.clear();
  fTrackCands.clear();
  fRiemannTracks.clear();

  // Call a function to create a Riemann track from input PndTrack
  std::vector<PndRiemannTrack> riemannTrackVector = CreateRiemannTrack(inputTrackArray);

  if (riemannTrackVector.size() > 0) {
    for (int iRiemannTrack = 0; iRiemannTrack < (int)riemannTrackVector.size(); ++iRiemannTrack) {

      std::vector<PndSdsHit *> mvdHits;
      std::vector<PndSdsHit *> mvdHitsNew;
      std::vector<PndSdsHit *> mvdHitsTest;

      mvdRiemannTrack = static_cast<PndRiemannTrack>(riemannTrackVector[iRiemannTrack]);
      origmvdRiemannTrack = static_cast<PndRiemannTrack>(riemannTrackVector[iRiemannTrack]);

      if (fDrawTrack == true) {

        PndSTEDraw *DrawObj = new PndSTEDraw();
        DrawObj->DrawTrack(mvdRiemannTrack);
      }

      tempTrack = static_cast<PndTrack *>(inputTrackArray->At(iRiemannTrack));

      fLineSlope = 0;

      PndSTETools *tool = new PndSTETools();

      tool->CalcSlope(tempTrack);
      fLineSlope = tool->GetLineSlope();

      PndSTEMvdHitFinder *mvdHitFinder = new PndSTEMvdHitFinder();
      mvdHitFinder->SetMagneticField();
      mvdHitFinder->SetMvdHitDist(fMvdHitDist);
      mvdHitFinder->SetUseHemisphere(fUseHemisphere);
      mvdHitFinder->SetNoAreaExclusion(fNoMVDAreaExclusion);
      if (fWeightMVD > 0) {
        mvdHitFinder->SetWeightsMvdHit(fWeightMVD);
      }
      mvdHitFinder->SetRunIn3D(fRunIn3D);

      double xPosTrack = mvdRiemannTrack.getHit(0)->x().X();
      double yPosTrack = mvdRiemannTrack.getHit(0)->x().Y();

      mvdHitFinder->SetXYPosTrack(xPosTrack, yPosTrack);

      if (fRunIn3D == true) {

        mvdHits = mvdHitFinder->FindBestHits3D(mvdRiemannTrack, inputPixelArray, inputStripArray);
        mvdHitsTest = mvdHitFinder->FindBestHits3D(mvdRiemannTrack, inputPixelArray, inputStripArray);

      } else {

        mvdHits = mvdHitFinder->FindBestHitsFromRiemannTrack(mvdRiemannTrack, inputPixelArray, inputStripArray);
      }

      // fReducedChi2 = 0.0;

      // double sumChi2Old = 99999999999;

      // int counter = 0;

      // while (counter < (int) mvdHits.size()-20) {
      //   // while(fSumChi2<sumChi2Old && counter<6){
      //   counter++;

      //   if (counter > 1) {

      //     sumChi2Old = fReducedChi2;
      //   }

      //   mvdHitsNew.clear();
      //   PndSTETools *tool = new PndSTETools();
      //   mvdHitsNew = tool->RefitRiemannTrack(mvdRiemannTrack, mvdHitsTest);
      //   mvdHitsTest.clear();

      //   for (int i_mvdHit = 0; i_mvdHit < (int) mvdHitsNew.size(); i_mvdHit++) {

      //     mvdHitsTest.push_back(mvdHitsNew[i_mvdHit]);
      //   }

      //   LOG(debug1) << " ------------------------------ " << endl;
      //   LOG(debug1) << "Chi2 Riemann circle fit: " << mvdRiemannTrack.ChiSquareDistCircle() / (mvdRiemannTrack.getNumHits() - 3) << endl;
      // }

      PndTrackCand tempTrackCand = PndTrackCand(*(static_cast<PndTrackCand *>(inputTrackCandArray->At(iRiemannTrack))));

      if (mvdHits.size() > 0) {

        double hitAddingCounter = 0;

        // Add all Riemann hits to Riemann track and hits to track cand
        for (int i_temp_Hit = 0; i_temp_Hit < (int)mvdHits.size(); i_temp_Hit++) {

          PndSdsHit *currentHit = mvdHits[i_temp_Hit];

          FairLink myMvdId(-1, FairRootManager::Instance()->GetEntryNr(), fMapMvdHitBranchId[currentHit], i_temp_Hit);

          tempTrackCand.AddHit(myMvdId, hitAddingCounter);

          hitAddingCounter = hitAddingCounter + 1;

          tempTrackCand.Sort();

          PndRiemannHit riemannHit(mvdHits[i_temp_Hit]);

          mvdRiemannTrack.addHit(riemannHit);
          mvdRiemannTrack.sortHits();
          mvdRiemannTrack.refit(true);
        }
      }

      fRiemannTracks.push_back(mvdRiemannTrack);

      PndTrack tempPndTrack = mvdRiemannTrack.getPndTrack(fBz);

      tempPndTrack.SetTrackCand(tempTrackCand);

      fTracks.push_back(tempPndTrack);
      fTrackCands.push_back(tempTrackCand);
      fTracks.back().SetLinks(*(tempTrackCand.GetPointerToLinks()));
    }
  }
}

std::vector<PndRiemannTrack> PndSTEMvdIdeal::CreateRiemannTrack(TClonesArray *inputTrackArray)
{

  LOG(debug) << "------ PndSTEMvdIdeal::CreateRiemannTrack() ------" << endl;

  std::vector<PndRiemannTrack> riemannTrackVector;

  if (inputTrackArray->GetEntries() > 0) {

    for (int i_input_track = 0; i_input_track < (int)inputTrackArray->GetEntries(); i_input_track++) {

      PndRiemannTrack newRiemannTrack;

      PndTrack *inputTrack = static_cast<PndTrack *>(inputTrackArray->At(i_input_track));
      FairMultiLinkedData linksSTTHit = inputTrack->GetLinksWithType(FairRootManager::Instance()->GetBranchId("STTHit"));

      if (linksSTTHit.GetNLinks() > 0) {

        for (int j = 0; j < linksSTTHit.GetNLinks(); j++) {

          PndSttHit *sttHit = static_cast<PndSttHit *>(FairRootManager::Instance()->GetCloneOfLinkData(linksSTTHit.GetLink(j)));

          if (sttHit != nullptr) {

            PndRiemannHit riemannHit(sttHit);
            newRiemannTrack.addHit(riemannHit);
          }
        }
      }

      if (newRiemannTrack.getNumHits() > 3) {
        newRiemannTrack.refit(true);

        riemannTrackVector.push_back(newRiemannTrack);
      }
    }
  }

  return riemannTrackVector;
}