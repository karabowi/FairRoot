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

#include "PndSTEGem.h"
#include "TString.h"
#include "PndGemHit.h"
#include "FairRootManager.h"
#include "PndHelixPropagator.h"
#include "PndTrack.h"
#include <utility>

using std::cout;
using std::endl;
using std::unique_ptr;

ClassImp(PndSTEGem);

std::vector<PndGemHit *> PndSTEGem::FindBestHitsWithHelix(PndTrack gemTrack, TClonesArray *gemHitsArray)
{

  LOG(debug) << "------ PndSTEGem::FindBestHitsWithHelix() ------" << endl;

  std::vector<PndGemHit *> bestHits;

  std::vector<PndGemHit *> collectedGemHits[3];

  for (int iGemHit = 0; iGemHit < (int)gemHitsArray->GetEntriesFast(); ++iGemHit) {

    PndGemHit *gemHit = static_cast<PndGemHit *>(gemHitsArray->At(iGemHit));

    LOG(debug1) << "Gem hit in station number: " << gemHit->GetStationNr() << endl;

    if (gemHit->GetStationNr() == 1) {
      collectedGemHits[0].push_back(gemHit);
    } else if (gemHit->GetStationNr() == 2) {
      collectedGemHits[1].push_back(gemHit);
    } else if (gemHit->GetStationNr() == 3) {
      collectedGemHits[2].push_back(gemHit);
    }
  }

  for (int i_Layer = 0; i_Layer < 3; ++i_Layer) {

    LOG(debug1) << "Layer loop" << endl;
    
    if (collectedGemHits[i_Layer].size() == 0) continue;

    for (int i_Hit = 0; i_Hit < (int)collectedGemHits[i_Layer].size(); i_Hit++) {

      LOG(debug1) << "Hit loop" << endl;

      PndGemHit *gemHit = static_cast<PndGemHit *>(collectedGemHits[i_Layer][i_Hit]);

      double xpos = gemHit->GetX();
      double ypos = gemHit->GetY();
      double zpos = gemHit->GetZ();

      int charge = gemTrack.GetParamLast().GetQ();
      TVector3 trackPos = gemTrack.GetParamLast().GetPosition();
      TVector3 trackMom = gemTrack.GetParamLast().GetMomentum();

      unique_ptr<PndHelixPropagator> propagator(new PndHelixPropagator(fBz, trackPos, trackMom, charge));
      FairTrackPar trackParAtXY = propagator->PropagateToZ(zpos);

      double deltax = trackParAtXY.GetX() - xpos;
      double deltay = trackParAtXY.GetY() - ypos;

      if (sqrt(deltax * deltax + deltay * deltay) < fGemHitDist) {

        bestHits.push_back(gemHit);

        LOG(debug1) << "Best hits size() " << bestHits.size() << endl;
      }
    }
  }

  return bestHits;
}

void PndSTEGem::AddHits(TClonesArray *inputTrackArray, TClonesArray *inputTrackCandArray, TClonesArray *inputHitArray)
{

  LOG(debug) << "------ PndSTEGem::AddHits() ------" << endl;

  fTracks.clear();
  fTrackCands.clear();

  std::vector<PndGemHit *> bestGemHits;
  PndTrackCand gemTrackCand;
  PndTrack gemTrack;
  int branchIdGemHit = FairRootManager::Instance()->GetBranchId("GEMHit");

  LOG(debug1) << "Number of input tracks: " << inputTrackArray->GetEntries() << endl;

  for (int iTrack = 0; iTrack < (int)inputTrackArray->GetEntries(); ++iTrack) {

    gemTrack = PndTrack(*static_cast<PndTrack *>(inputTrackArray->At(iTrack)));

    bestGemHits.clear();

    bestGemHits = FindBestHitsWithHelix(gemTrack, inputHitArray);

    LOG(debug1) << "Number of GemHits: " << bestGemHits.size() << endl;

    gemTrackCand = PndTrackCand(*static_cast<PndTrackCand *>(inputTrackCandArray->At(iTrack)));

    if (bestGemHits.size() > 0) {

      for (int i_BestHit = 0; i_BestHit < (int)bestGemHits.size(); i_BestHit++) {

        FairLink myGem(-1, FairRootManager::Instance()->GetEntryNr(), branchIdGemHit, i_BestHit);

        gemTrackCand.AddHit(myGem, i_BestHit);
      }
    }

    fTracks.push_back(gemTrack);
    fTrackCands.push_back(gemTrackCand);

    LOG(debug1) << "--------------------------------------------------------" << endl;
    LOG(debug1) << "Number of Gem Tracks: " << fTracks.size() << endl;
    LOG(debug1) << "Number of Gem Track Cands: " << fTrackCands.size() << endl;
    LOG(debug1) << "--------------------------------------------------------" << endl;
  }
}
