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

#include "PndSTETrackCreator.h"
#include "TString.h"

#include "FairLogger.h"

#include "PndSTEMvd.h"
#include "PndSTEMvdIdeal.h"
#include "PndSTEMvdHelix.h"
#include "PndSTEGem.h"
#include "PndSTEBTof.h"

using std::cout;
using std::endl;

ClassImp(PndSTETrackCreator);

void PndSTETrackCreator::AddHitsToMvdTrack(TClonesArray *inputTrackArray, TClonesArray *inputRiemannTrackArray, TClonesArray *inputTrackCandArray,
                                                            TClonesArray *inputPixelArray, TClonesArray *inputStripArray)
{

  LOG(debug) << "------ PndSTETrackCreator::AddHitsToMvdTrack() ------" << endl;

  fInputPndTracks = inputTrackArray;
  fInputPndRiemannTracks = inputRiemannTrackArray;
  fInputPndTrackCands = inputTrackCandArray;

  fInputMvdHitsPixels = inputPixelArray;
  fInputMvdHitsStrips = inputStripArray;

  CreateTracks();
}

void PndSTETrackCreator::AddHitsToGemOrBtofTrack(TClonesArray *inputTrackArray, TClonesArray *inputTrackCandArray, TClonesArray *inputGemOrBtofArray)
{

  LOG(debug) << "------ PndSTETrackCreator::AddHitsToGemOrBtofTrack() ------" << endl;

  fInputPndTracks = inputTrackArray;
  fInputPndTrackCands = inputTrackCandArray;

  fInputGemOrBtofHits = inputGemOrBtofArray;

  CreateTracks();
}

void PndSTETrackCreator::CreateTracks()
{

  LOG(debug) << "------ PndSTETrackCreator::CreateTracks() ------" << endl;

  if (fIncludeMvd == true) {

    if (fIdealTrack == false && fUseHelix == false) {

      PndSTEMvd *mvdTracker = new PndSTEMvd();

      mvdTracker->SetMagneticField();
      mvdTracker->SetMvdHitDist(fMvdHitDist);
      mvdTracker->SetUseHemisphere(fUseHemisphere);
      mvdTracker->SetNoAreaExclusion(fNoMVDAreaExclusion);
      if (fWeightMVD > 0) {
        mvdTracker->SetWeightsMvdHit(fWeightMVD);
      }
      mvdTracker->SetRunIn3D(fRunIn3D);
      mvdTracker->SetDrawTracks(fDrawTrack);

      mvdTracker->AddHits(fInputPndTracks, fInputPndRiemannTracks, fInputPndTrackCands, fInputMvdHitsPixels, fInputMvdHitsStrips);

      fTracks.clear();
      fTrackCands.clear();
      fRiemannTracks.clear();

      for (int i = 0; i < mvdTracker->NumTrackCands(); i++) {
        fTracks.push_back(mvdTracker->GetTrack(i));
        fTrackCands.push_back(mvdTracker->GetTrackCand(i));
      }
      for (int i = 0; i < mvdTracker->NumRiemannTracks(); i++) {
        fRiemannTracks.push_back(mvdTracker->GetRiemannTrack(i));
      }
    }

    if (fIdealTrack == true) {
      PndSTEMvdIdeal *mvdTracker = new PndSTEMvdIdeal();

      mvdTracker->SetMagneticField();
      mvdTracker->SetMvdHitDist(fMvdHitDist);
      mvdTracker->SetUseHemisphere(fUseHemisphere);
      mvdTracker->SetNoAreaExclusion(fNoMVDAreaExclusion);
      if (fWeightMVD > 0) {
        mvdTracker->SetWeightsMvdHit(fWeightMVD);
      }
      mvdTracker->SetRunIn3D(fRunIn3D);
      mvdTracker->SetDrawTracks(fDrawTrack);

      mvdTracker->AddHits(fInputPndTracks, fInputPndTrackCands, fInputMvdHitsPixels, fInputMvdHitsStrips);

      fTracks.clear();
      fTrackCands.clear();
      fRiemannTracks.clear();

      for (int i = 0; i < mvdTracker->NumTrackCands(); i++) {
        fTracks.push_back(mvdTracker->GetTrack(i));
        fTrackCands.push_back(mvdTracker->GetTrackCand(i));
      }
      for (int i = 0; i < mvdTracker->NumRiemannTracks(); i++) {
        fRiemannTracks.push_back(mvdTracker->GetRiemannTrack(i));
      }
    }

    if (fUseHelix == true) {
      PndSTEMvdHelix *mvdTracker = new PndSTEMvdHelix();

      mvdTracker->SetMagneticField();
      mvdTracker->SetMvdHitDist(fMvdHitDist);
      mvdTracker->SetRunIn3D(fRunIn3D);

      mvdTracker->AddHits(fInputPndTracks, fInputPndTrackCands, fInputMvdHitsPixels, fInputMvdHitsStrips);

      fTracks.clear();
      fTrackCands.clear();
      fRiemannTracks.clear();

      for (int i = 0; i < mvdTracker->NumTrackCands(); i++) {
        fTracks.push_back(mvdTracker->GetTrack(i));
        fTrackCands.push_back(mvdTracker->GetTrackCand(i));
      }
      for (int i = 0; i < mvdTracker->NumRiemannTracks(); i++) {
        fRiemannTracks.push_back(mvdTracker->GetRiemannTrack(i));
      }
    }
  }

  if (fIncludeGem == true) {

    PndSTEGem *gemTracker = new PndSTEGem();
    gemTracker->SetMagneticField();
    gemTracker->SetGemHitDist(fGemHitDist);
    gemTracker->AddHits(fInputPndTracks, fInputPndTrackCands, fInputGemOrBtofHits);

    fTracks.clear();
    fTrackCands.clear();
    fRiemannTracks.clear();

    for (int i = 0; i < gemTracker->NumTrackCands(); i++) {
      fTracks.push_back(gemTracker->GetTrack(i));
      fTrackCands.push_back(gemTracker->GetTrackCand(i));
    }
  }

  if (fIncludeBtof == true) {

    PndSTEBTof *btofHitFinder = new PndSTEBTof();
    btofHitFinder->SetMagneticField();
    btofHitFinder->SetBtofHitDist(fBtofHitDist);
    btofHitFinder->AddHit(fInputPndTracks, fInputPndTrackCands, fInputGemOrBtofHits);

    fTracks.clear();
    fTrackCands.clear();
    fRiemannTracks.clear();

    for (int i = 0; i < btofHitFinder->NumTrackCands(); i++) {
      fTracks.push_back(btofHitFinder->GetTrack(i));
      fTrackCands.push_back(btofHitFinder->GetTrackCand(i));
    }
  }
}