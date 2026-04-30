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

#include "PndSTEMvd.h"
#include "PndSTEMvdHitFinder.h"
#include "PndSTETools.h"
#include "PndSTEDraw.h"
#include "PndSdsHit.h"
#include "FairLogger.h"
#include <vector>
#include <numeric>
#include <functional>

using std::cout;
using std::endl;

ClassImp(PndSTEMvd);

std::vector<PndRiemannTrack> PndSTEMvd::AddHits(TClonesArray *inputTrackArray, TClonesArray *inputRiemannTrackArray, TClonesArray *inputTrackCandArray,
                                                    TClonesArray *inputPixelArray, TClonesArray *inputStripArray)
{

  LOG(debug) << "------ PndSTEMvd::AddHits() ------" << endl;

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

  PndTrack myMvdTrack;
  PndTrack *tempTrack;

  fTracks.clear();
  fTrackCands.clear();
  fRiemannTracks.clear();

  std::vector<PndSdsHit *> mvdHits;
  std::vector<PndSdsHit *> mvdHitsNew;
  std::vector<PndSdsHit *> mvdHitsTest;

  PndRiemannTrack mvdRiemannTrack;
  PndRiemannTrack origmvdRiemannTrack;

  for (int iRiemannTrack = 0; iRiemannTrack < (int)inputTrackArray->GetEntries(); ++iRiemannTrack) {

    mvdHits.clear();
    mvdHitsNew.clear();
    mvdHitsTest.clear();

    if (inputTrackArray->At(iRiemannTrack) == nullptr) {
      continue;
    }

    mvdRiemannTrack = PndRiemannTrack(*static_cast<PndRiemannTrack *>(inputRiemannTrackArray->At(iRiemannTrack)));
    origmvdRiemannTrack = PndRiemannTrack(*static_cast<PndRiemannTrack *>(inputRiemannTrackArray->At(iRiemannTrack)));

    // The statement below is for filtering out poorly fitted tracks.
    // These are usually tracks with less than 15 STT hits

    // if (origmvdRiemannTrack.getNumHits() < 15) {

    // continue;
    //}

    if (fDrawTrack == true) {
      PndSTEDraw *DrawObj = new PndSTEDraw();
      DrawObj->DrawTrack(mvdRiemannTrack);
    }

    fLineSlope = 0;
    if (inputTrackArray->GetEntries() != 0) {
      tempTrack = static_cast<PndTrack *>(inputTrackArray->At(iRiemannTrack));
      if (tempTrack != nullptr) {
        PndSTETools *tool = new PndSTETools();
        tool->CalcSlope(tempTrack);
        fLineSlope = tool->GetLineSlope();
      }
    }

    PndSTEMvdHitFinder *hitFinder = new PndSTEMvdHitFinder();
    hitFinder->SetMagneticField();
    hitFinder->SetMvdHitDist(fMvdHitDist);
    hitFinder->SetUseHemisphere(fUseHemisphere);
    hitFinder->SetNoAreaExclusion(fNoMVDAreaExclusion);
    if (fWeightMVD > 0) {
      hitFinder->SetWeightsMvdHit(fWeightMVD);
    }
    hitFinder->SetRunIn3D(fRunIn3D);

    double xPosTrack = mvdRiemannTrack.getHit(0)->x().X();
    double yPosTrack = mvdRiemannTrack.getHit(0)->x().Y();

    hitFinder->SetXYPosTrack(xPosTrack, yPosTrack);

    if (fRunIn3D == true) {

      mvdHits = hitFinder->FindBestHits3D(mvdRiemannTrack, inputPixelArray, inputStripArray);
      mvdHitsTest = hitFinder->FindBestHits3D(mvdRiemannTrack, inputPixelArray, inputStripArray);
    } else {

      mvdHits = hitFinder->FindBestHitsFromRiemannTrack(mvdRiemannTrack, inputPixelArray, inputStripArray);
    }

    if (fRunIn3D == true) {

      int counter = 0;

      int iterations = (int)mvdHits.size() - 6; // get the number of iterations

      if (iterations > 0) {

        while (counter < (int)mvdHits.size() && counter < iterations) {

          counter++;

          // MVD hits after reduced chi2 and refit
          PndSTETools *tools = new PndSTETools();
          mvdHitsNew = tools->RefitRiemannTrack(mvdRiemannTrack, mvdHitsTest);

          if (fDrawTrack == true) {
            PndSTEDraw *DrawObj = new PndSTEDraw();
            DrawObj->DrawTrack(mvdRiemannTrack);
          }
          mvdHitsTest.clear();

          for (int i_mvdHit = 0; i_mvdHit < (int)mvdHitsNew.size(); i_mvdHit++) {

            mvdHitsTest.push_back(mvdHitsNew[i_mvdHit]);
          }

          LOG(debug2) << " ------------------------------ " << std::endl;
          LOG(debug2) << "Iteration: " << counter << std::endl;
        }
      } else {

        for (int i_mvdHit = 0; i_mvdHit < (int)mvdHitsTest.size(); i_mvdHit++) {

          mvdHitsNew.push_back(mvdHitsTest[i_mvdHit]);
        }
      }
    }

    if (fRunIn3D == false) {

      for (int i_mvdHit = 0; i_mvdHit < (int)mvdHits.size(); i_mvdHit++) {

        mvdHitsNew.push_back(mvdHits[i_mvdHit]);
      }
    }

    PndTrackCand tempTrackCand = PndTrackCand(*static_cast<PndTrackCand *>(inputTrackCandArray->At(iRiemannTrack)));

    if (mvdHitsNew.size() > 0) {

      double hitAddingCounter = 0;

      // Add all Riemann hits to Riemann track and hits to track cand
      for (int i_temp_Hit = 0; i_temp_Hit < (int)mvdHitsNew.size(); i_temp_Hit++) {

        PndSdsHit *currentHit = mvdHitsNew[i_temp_Hit];

        FairLink myMvdId(-1, FairRootManager::Instance()->GetEntryNr(), fMapMvdHitBranchId[currentHit], i_temp_Hit);

        tempTrackCand.AddHit(myMvdId, hitAddingCounter);

        hitAddingCounter = hitAddingCounter + 1;

        tempTrackCand.Sort();

        PndRiemannHit riemannHit(mvdHitsNew[i_temp_Hit]);

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

  return fRiemannTracks;
}
