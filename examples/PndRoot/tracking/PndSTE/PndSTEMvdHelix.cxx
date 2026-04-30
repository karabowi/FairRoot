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

#include "PndSTEMvdHelix.h"
#include "PndSTEMvdHitFinder.h"
#include "PndSTEDraw.h"
#include "TString.h"
#include "PndSttHit.h"
#include "PndSdsHit.h"
#include "FairRootManager.h"
#include "FairLogger.h"
#include "PndHelixPropagator.h"
#include "FairLink.h"
#include <vector>
#include <numeric>
#include <functional>
#include <utility> // for unique_ptr

using std::cout;
using std::endl;
using std::unique_ptr;

ClassImp(PndSTEMvdHelix);

std::vector<PndTrack> PndSTEMvdHelix::AddHits(TClonesArray *inputTrackArray, TClonesArray *inputTrackCandArray, TClonesArray *inputPixelArray, TClonesArray *inputStripArray)
{

  LOG(debug) << "------ PndSTEMvdHelix::AddHits() ------" << endl;

  fMapMvdHitBranchId.clear();
  std::vector<PndTrack> trackVector;

  PndTrack myMvdTrack;
  PndTrack myMvdTrackNew;
  PndTrackCand myMvdTrackCand;

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

  for (int iTrack = 0; iTrack < (int)inputTrackArray->GetEntries(); ++iTrack) {

    std::vector<PndSdsHit *> mvdHits;

    myMvdTrack = PndTrack(*static_cast<PndTrack *>(inputTrackArray->At(iTrack)));
    myMvdTrackCand = PndTrackCand(*static_cast<PndTrackCand *>(inputTrackCandArray->At(iTrack)));

    PndSTEMvdHitFinder *mvdHitFinder = new PndSTEMvdHitFinder();
    mvdHitFinder->SetMagneticField();
    mvdHitFinder->SetMvdHitDist(fMvdHitDist);
    mvdHitFinder->SetUseHemisphere(fUseHemisphere);
    mvdHitFinder->SetNoAreaExclusion(fNoMVDAreaExclusion);
    mvdHitFinder->SetWeightsMvdHit(fWeightMVD);
    mvdHitFinder->SetRunIn3D(fRunIn3D);

    mvdHits = mvdHitFinder->FindBestHitsWithHelix(&myMvdTrack, inputPixelArray, inputStripArray);

    for (int iHits = 0; iHits < (int)mvdHits.size(); ++iHits) {

      FairLink myMvdId(-1, FairRootManager::Instance()->GetEntryNr(), fMapMvdHitBranchId[mvdHits[iHits]], iHits);

      myMvdTrackCand.AddHit(myMvdId, iHits);
    }

    myMvdTrack.SetLinks(*(myMvdTrackCand.GetPointerToLinks()));

    trackVector.push_back(myMvdTrack);

    fTracks.push_back(myMvdTrack);
    fTrackCands.push_back(myMvdTrackCand);
  }

  return trackVector;
}