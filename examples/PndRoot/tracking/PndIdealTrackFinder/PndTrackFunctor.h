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

/*
 * PndTrackFunctor.h
 *
 *  Created on: 02.02.2017
 *      Author: Stockmanns
 */

#ifndef PNDTOOLS_TRACKINGQA_PNDTRACKFUNCTOR_H_
#define PNDTOOLS_TRACKINGQA_PNDTRACKFUNCTOR_H_

#include "FairMultiLinkedData.h"
#include "FairRootManager.h"

#include <functional>

class PndTrackFunctor {
 public:
  virtual Bool_t operator()(FairMultiLinkedData *a, Bool_t primary) { return Call(a, primary); };
  static PndTrackFunctor *make_PndTrackFunctor(std::string functorName);
  virtual Bool_t Call(FairMultiLinkedData *a, Bool_t primary) = 0;
  virtual void Print() = 0;

  virtual ~PndTrackFunctor(){};
};

class StandardTrackFunctor : public PndTrackFunctor {
  Bool_t Call(FairMultiLinkedData *a, Bool_t)
  { // primary //[R.K.03/2017] unused variable(s)
    FairRootManager *ioman = FairRootManager::Instance();
    Bool_t possibleTrack = kFALSE;
    possibleTrack =
      (possibleTrack | ((a->GetLinksWithType(ioman->GetBranchId("MVDHitsPixel")).GetNLinks() + a->GetLinksWithType(ioman->GetBranchId("MVDHitsStrip")).GetNLinks()) > 3));

    possibleTrack = (possibleTrack | ((a->GetLinksWithType(ioman->GetBranchId("MVDHitsPixel")).GetNLinks() + a->GetLinksWithType(ioman->GetBranchId("MVDHitsStrip")).GetNLinks() +
                                       a->GetLinksWithType(ioman->GetBranchId("STTHit")).GetNLinks() + a->GetLinksWithType(ioman->GetBranchId("GEMHit")).GetNLinks()) > 5));

    return possibleTrack;
  }

  void Print() { std::cout << "StandardTrackFunctor: > 3 Hits in MVD or > 5 Hits in (MVD+Stt+GEM)" << std::endl; }
};

class OnlySttFunctor : public PndTrackFunctor {
  Bool_t Call(FairMultiLinkedData *a, Bool_t)
  { // primary //[R.K.03/2017] unused variable(s)
    FairRootManager *ioman = FairRootManager::Instance();
    Bool_t possibleTrack = kFALSE;

    possibleTrack = (possibleTrack | (a->GetLinksWithType(ioman->GetBranchId("STTHit")).GetNLinks() > 5));

    return possibleTrack;
  }
  void Print() { std::cout << "OnlySttFunctor: > 5 Hits in Stt" << std::endl; }
};

class OnlySttTimeBasedFunctor : public PndTrackFunctor {
  Bool_t Call(FairMultiLinkedData *a, Bool_t)
  {
    FairRootManager *ioman = FairRootManager::Instance();
    Bool_t possibleTrack = kFALSE;

    possibleTrack = (possibleTrack | (a->GetLinksWithType(ioman->GetBranchId("STTSortedHits")).GetNLinks() > 5));

    return possibleTrack;
  }
  void Print() { std::cout << "OnlySttTimeBasedFunctor: > 5 Hits in Stt" << std::endl; }
};

class RiemannMvdSttGemFunctor : public PndTrackFunctor {
  Bool_t Call(FairMultiLinkedData *a, Bool_t)
  { // primary //[R.K.03/2017] unused variable(s)
    FairRootManager *ioman = FairRootManager::Instance();
    Bool_t possibleTrack = kFALSE;
    Bool_t mvdHits = ((a->GetLinksWithType(ioman->GetBranchId("MVDHitsPixel")).GetNLinks() + a->GetLinksWithType(ioman->GetBranchId("MVDHitsStrip")).GetNLinks()) > 2);

    if (mvdHits) {
      possibleTrack = (a->GetLinksWithType(ioman->GetBranchId("STTHit")).GetNLinks() > 1) | (a->GetLinksWithType(ioman->GetBranchId("GEMHit")).GetNLinks() > 1);
    }
    return possibleTrack;
  }
  void Print() { std::cout << "RiemannMvdSttGemFunctor: > 2 Hits in MVD and >0 Hits in (Stt+Gem)" << std::endl; }
};

class CircleHoughTrackFunctor : public PndTrackFunctor {
  Bool_t Call(FairMultiLinkedData *a, Bool_t primary)
  {
    if (primary == kFALSE)
      return kFALSE;
    FairRootManager *ioman = FairRootManager::Instance();
    Bool_t possibleTrack = kFALSE;

    Int_t nHitsMvdPixel = a->GetLinksWithType(ioman->GetBranchId("MVDHitsPixel")).GetNLinks();
    Int_t nHitsMvdStrip = a->GetLinksWithType(ioman->GetBranchId("MVDHitsStrip")).GetNLinks();
    if (nHitsMvdPixel + nHitsMvdStrip > 2) { // First requirement: more than two MVD hits
      // Int_t nHitsStt = a->GetLinksWithType(ioman->GetBranchId("STTHit")).GetNLinks();
      // Int_t nHitsGem = a->GetLinksWithType(ioman->GetBranchId("GEMHit")).GetNLinks();
      possibleTrack = kTRUE;
      // possibleTrack = (nHitsMvdPixel + nHitsMvdStrip + nHitsStt + nHitsGem > 6);  // Second requirement: More than six hits total
    }
    return possibleTrack;
  }
  void Print() { std::cout << "CircleHoughTrackFunctor: >= 3 Hits in MVD and primary track" << std::endl; }
};

class FtsTrackFunctor : public PndTrackFunctor {
  Bool_t Call(FairMultiLinkedData *a, Bool_t)
  { // primary //[R.K.03/2017] unused variable(s)
    FairRootManager *ioman = FairRootManager::Instance();
    Bool_t possibleTrack = kFALSE;

    possibleTrack = (possibleTrack | (a->GetLinksWithType(ioman->GetBranchId("FTSHit")).GetNLinks() > 5));

    return possibleTrack;
  }
  void Print() { std::cout << "FTSTrackFunctor: > 5 Hits in Fts" << std::endl; }
};

class NoFtsTrackFunctor : public PndTrackFunctor {
  Bool_t Call(FairMultiLinkedData *a, Bool_t primary)
  {
    FairRootManager *ioman = FairRootManager::Instance();
    Bool_t possibleTrack = kFALSE;

    possibleTrack = (possibleTrack | !(a->GetLinksWithType(ioman->GetBranchId("FTSHit")).GetNLinks() > 0));
    if (possibleTrack == kFALSE)
      return kFALSE;
    StandardTrackFunctor standard;
    possibleTrack = kFALSE;
    possibleTrack = (possibleTrack | standard(a, primary));

    return possibleTrack;
  }
  void Print() { std::cout << "noFTSTrackFunctor: no Hits in Fts + standardFunctor!" << std::endl; }
};

class AllTracksFunctor : public PndTrackFunctor {
  Bool_t Call(FairMultiLinkedData *, Bool_t)
  { // a  primary//[R.K.03/2017] unused variable(s)
    return kTRUE;
  }
  void Print() { std::cout << "AllTracksFunctor: take everything" << std::endl; }
};

#endif /* PNDTOOLS_TRACKINGQA_PNDTRACKFUNCTOR_H_ */
