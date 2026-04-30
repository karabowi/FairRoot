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

#include "PndSTEMvdHitFinder.h"
#include "PndSTETools.h"
#include "TString.h"
#include "PndSttHit.h"
#include "PndSdsHit.h"
#include "FairRootManager.h"
#include "PndGeoHandling.h"
#include "FairLogger.h"
#include "PndHelixPropagator.h"
#include <vector>
#include <numeric>
#include <functional>
#include <utility> // for unique_ptr

using std::cout;
using std::endl;
using std::unique_ptr;

ClassImp(PndSTEMvdHitFinder);

std::vector<PndSdsHit *> PndSTEMvdHitFinder::FindBestHitsFromRiemannTrack(PndRiemannTrack mvdRiemannTrack, TClonesArray *mvdPixelHitsArray, TClonesArray *mvdStripHitsArray)
{

  LOG(debug) << "------ PndSTEMvdHitFinder::FindBestHitsFromRiemannTrack() ------" << endl;

  std::vector<PndSdsHit *> mvdHits;

  PndSdsHit *mvdHit = nullptr;

  for (int iMvdHit = 0; iMvdHit < (int)mvdPixelHitsArray->GetEntriesFast(); ++iMvdHit) {
    mvdHit = static_cast<PndSdsHit *>(mvdPixelHitsArray->At(iMvdHit));

    mvdHits.push_back(mvdHit);
  }

  for (Int_t iMvdHit = 0; iMvdHit < (int)mvdStripHitsArray->GetEntriesFast(); ++iMvdHit) {
    mvdHit = static_cast<PndSdsHit *>(mvdStripHitsArray->At(iMvdHit));

    mvdHits.push_back(mvdHit);
  }

  PndSdsHit *tempHit(nullptr);
  PndSdsHit *currentBestHit(nullptr);
  double tempDist = 99999;
  double smallestDist = 9999;

  std::vector<PndSdsHit *> collectedMvdHits[4];

  std::vector<PndSdsHit *> bestHit;

  std::vector<double> bestDistance;

  PndGeoHandling *geoH = PndGeoHandling::Instance();

  for (Int_t i_Hit = 0; i_Hit < (int)mvdHits.size(); i_Hit++) {

    mvdHit = mvdHits[i_Hit];

    // Choosing MVD barrel layer from the PndGeoHandling path
    if (geoH->GetPath(mvdHit->GetSensorID()).Contains("Blo1o")) { // First layer of MVD, pixels
      collectedMvdHits[0].push_back(mvdHit);
    }
    if (geoH->GetPath(mvdHit->GetSensorID()).Contains("Blo2o")) { // Second layer of MVD, pixels
      collectedMvdHits[1].push_back(mvdHit);
    }
    if (geoH->GetPath(mvdHit->GetSensorID()).Contains("Bl3o")) { // Third layer of MVD, strips
      collectedMvdHits[2].push_back(mvdHit);
    }
    if (geoH->GetPath(mvdHit->GetSensorID()).Contains("Bl4o")) { // Fourth layer of MVD, strips
      collectedMvdHits[3].push_back(mvdHit);
    }
  }

  mvdRiemannTrack.refit(true); // Need to refit with error calculation in order to have dR

  for (Int_t i_Layer = 3; i_Layer > -1; i_Layer--) {

    smallestDist = 9999; // Smallest distance between the track and a hit in the current layer
    tempDist = 99999;

    for (Int_t i_Hit = 0; i_Hit < (int)collectedMvdHits[i_Layer].size(); i_Hit++) {

      if (collectedMvdHits[i_Layer].size() > 0) {

        tempHit = collectedMvdHits[i_Layer][i_Hit];

        PndRiemannHit riemannHit(tempHit);
        tempDist = mvdRiemannTrack.dist((PndRiemannHit *)&riemannHit);

        if (tempDist < smallestDist) {

          smallestDist = tempDist;

          if (tempDist < fMvdHitDist) {
            currentBestHit = tempHit;
          }
        }
      }
    }

    if (smallestDist < fMvdHitDist) {
      if (collectedMvdHits[i_Layer].size() > 0) { // Only add if a hit was actually found in layer

        PndRiemannHit riemannHit(currentBestHit);

        bestHit.push_back(currentBestHit);

        mvdRiemannTrack.addHit(riemannHit);
        mvdRiemannTrack.sortHits();
        mvdRiemannTrack.refit(true);
      }
    }
  }

  return bestHit;
}

std::vector<PndSdsHit *> PndSTEMvdHitFinder::FindBestHits3D(PndRiemannTrack mvdRiemannTrack, TClonesArray *mvdPixelHitsArray, TClonesArray *mvdStripHitsArray)
{

  LOG(debug) << "------ PndSTEMvdHitFinder::FindBestHits3D() ------" << endl;

  std::vector<PndSdsHit *> mvdHits;
  std::vector<PndSdsHit *> bestMvdHits;

  mvdRiemannTrack.refit(true);

  // Putting pixel and strip hits together in one vector
  for (int iMvdHit = 0; iMvdHit < (int)mvdPixelHitsArray->GetEntriesFast(); ++iMvdHit) {
    PndSdsHit *mvdHit = static_cast<PndSdsHit *>(mvdPixelHitsArray->At(iMvdHit));
    mvdHits.push_back(mvdHit);
  }

  for (int iMvdHit = 0; iMvdHit < (int)mvdStripHitsArray->GetEntriesFast(); ++iMvdHit) {
    PndSdsHit *mvdHit = static_cast<PndSdsHit *>(mvdStripHitsArray->At(iMvdHit));
    mvdHits.push_back(mvdHit);
  }

  for (int i_Hit = 0; i_Hit < (int)mvdHits.size(); i_Hit++) {

    PndSdsHit *mvdHit = mvdHits[i_Hit];

    double xPosHit = mvdHit->GetX();
    double yPosHit = mvdHit->GetY();

    PndRiemannHit riemannHit(mvdHit);

    if (fNoMVDAreaExclusion == true) {

      double tempError = 1 / fWeightMVD;
      mvdHit->SetDxyz(tempError, tempError, tempError);
      bestMvdHits.push_back(mvdHit);
    } else {

      if (fUseHemisphere == false) {

        if (xPosHit * fXposTrack > 0 || yPosHit * fYposTrack > 0) {

          if (fAdjustWeightsMvd == true) {

            double tempError = 1 / fWeightMVD;
            mvdHit->SetDxyz(tempError, tempError, tempError);
          }

          bestMvdHits.push_back(mvdHit);
        }
      }

      if (fUseHemisphere == true) {

        bool onRightHemisphere = false;
        if (fLineSlope != 0) {
          PndSTETools *tool = new PndSTETools();
          onRightHemisphere = tool->IsHitInCorrectHemisphere(mvdHit, fxc, fLineSlope);
        } else {
          onRightHemisphere = true;
        }

        if (onRightHemisphere == true) {
          if (fAdjustWeightsMvd == true) {
            double tempError = 1 / fWeightMVD;
            mvdHit->SetDxyz(tempError, tempError, tempError);
          }
          bestMvdHits.push_back(mvdHit);
        }
      }
    }
  }

  return bestMvdHits;
}

std::vector<PndSdsHit *> PndSTEMvdHitFinder::FindBestHitsWithHelix(PndTrack *myMvdTrack, TClonesArray *mvdPixelHitsArray, TClonesArray *mvdStripHitsArray)
{

  LOG(debug) << "------ PndSTEMvdHitFinder::FindBestHitsWithHelix() ------" << endl;

  std::vector<PndSdsHit *> bestHits;

  std::vector<TClonesArray *> hitArrayVector;

  if (mvdPixelHitsArray != nullptr) {
    hitArrayVector.push_back(mvdPixelHitsArray);
  }
  if (mvdStripHitsArray != nullptr) {
    hitArrayVector.push_back(mvdStripHitsArray);
  }

  // First iteration handles pixels and second strips if both are present
  for (int i = 0; i < (int)hitArrayVector.size(); i++) {

    for (Int_t i_Hit = 0; i_Hit < (int)hitArrayVector[i]->GetEntries(); i_Hit++) {

      PndSdsHit *mvdHit = static_cast<PndSdsHit *>(hitArrayVector[i]->At(i_Hit));

      if (mvdHit == nullptr)
        continue;

      double xpos = mvdHit->GetX();
      double ypos = mvdHit->GetY();
      double zpos = mvdHit->GetZ();

      LOG(debug1) << xpos << " : " << ypos << endl;

      if (myMvdTrack == nullptr)
        continue;

      int charge = myMvdTrack->GetParamFirst().GetQ();
      TVector3 trackPos = myMvdTrack->GetParamFirst().GetPosition();
      TVector3 trackMom = myMvdTrack->GetParamFirst().GetMomentum();

      unique_ptr<PndHelixPropagator> propagator(new PndHelixPropagator(fBz, trackPos, trackMom, charge));
      FairTrackPar trackParAtXY = propagator->PropagateToZ(zpos);

      double deltax = trackParAtXY.GetX() - xpos;
      double deltay = trackParAtXY.GetY() - ypos;

      if (sqrt(deltax * deltax + deltay * deltay) < fMvdHitDist) {

        bestHits.push_back(mvdHit);
      }
    }
  }

  return bestHits;
}
