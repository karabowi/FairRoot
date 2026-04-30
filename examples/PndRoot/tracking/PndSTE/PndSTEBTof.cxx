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

#include "PndSTEBTof.h"
#include "TString.h"
#include "PndSciTHit.h"
#include "FairRootManager.h"
#include "FairLogger.h"
#include "PndHelixPropagator.h"
#include <utility>

using std::cout;
using std::endl;
using std::unique_ptr;

ClassImp(PndSTEBTof);

PndSciTHit *PndSTEBTof::FindBestHitWithHelix(PndTrack btofTrack, TClonesArray *btofHitsArray)
{

  LOG(debug) << "------ PndSTEBTof::FindBestHitsWithHelix() ------" << endl;

  PndSciTHit *bestHit(nullptr);
  double bestDist = 9999;

  for (Int_t iBtofHit = 0; iBtofHit < (int)btofHitsArray->GetEntriesFast(); iBtofHit++) {

    PndSciTHit *btofHit = static_cast<PndSciTHit *>(btofHitsArray->At(iBtofHit));

    double xpos = btofHit->GetX();
    double ypos = btofHit->GetY();
    double zpos = btofHit->GetZ();

    int charge = btofTrack.GetParamLast().GetQ();
    TVector3 trackPos = btofTrack.GetParamLast().GetPosition();
    TVector3 trackMom = btofTrack.GetParamLast().GetMomentum();

    unique_ptr<PndHelixPropagator> propagator(new PndHelixPropagator(fBz, trackPos, trackMom, charge));
    FairTrackPar trackParAtXY = propagator->PropagateToZ(zpos);

    double deltax = trackParAtXY.GetX() - xpos;
    double deltay = trackParAtXY.GetY() - ypos;

    if (sqrt(deltax * deltax + deltay * deltay) < fBtofHitDist) {
      if (sqrt(deltax * deltax + deltay * deltay) < bestDist) {

        bestDist = sqrt(deltax * deltax + deltay * deltay);
        bestHit = btofHit;
      }
    }
  }

  return bestHit;
}

void PndSTEBTof::AddHit(TClonesArray *inputTrackArray, TClonesArray *inputTrackCandArray, TClonesArray *inputHitArray)
{

  LOG(debug) << "------ PndSTEBTof::AddHit() ------" << endl;

  PndSciTHit *bestBtofHit(nullptr);
  PndTrackCand btofTrackCand;
  PndTrack btofTrack;

  int branchIdBToFHit = FairRootManager::Instance()->GetBranchId("SciTHit");

  for (int iTrack = 0; iTrack < (int)inputTrackArray->GetEntries(); ++iTrack) {

    btofTrack = PndTrack(*static_cast<PndTrack *>(inputTrackArray->At(iTrack)));
    btofTrackCand = PndTrackCand(*static_cast<PndTrackCand *>(inputTrackCandArray->At(iTrack)));

    bestBtofHit = FindBestHitWithHelix(btofTrack, inputHitArray);

    if (bestBtofHit != nullptr) {

      FairLink myBToF(-1, FairRootManager::Instance()->GetEntryNr(), branchIdBToFHit, 0);

      btofTrackCand.AddHit(myBToF, btofTrackCand.GetNHits());

      btofTrack.AddLink(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), FairRootManager::Instance()->GetBranchId("TrackCand"), fTracks.size()));
    }

    fTracks.push_back(btofTrack);
    fTrackCands.push_back(btofTrackCand);

    LOG(debug1) << "--------------------------------------------------------" << endl;
    LOG(debug1) << "Number of Btof Tracks: " << fTracks.size() << endl;
    LOG(debug1) << "Number of Btof Track Cands: " << fTrackCands.size() << endl;
    LOG(debug1) << "--------------------------------------------------------" << endl;
  }
}
