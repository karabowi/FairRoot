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
// -----                      PndMQStraightLineTrackFinder                    -----
// -----                  Created 22/10/09  by M. Michel               -----
// -------------------------------------------------------------------------

#ifndef PndMQStraightLineTrackFinder_H
#define PndMQStraightLineTrackFinder_H

#include "FairTask.h"
#include "PndMCTrack.h"
#include "PndSdsGeoPar.h"
#include "PndSdsHit.h"
#include "PndSdsMCPoint.h"
#include "PndSdsClusterStrip.h"
#include "PndTrackCand.h"
#include "PndTrack.h"

#include <string>
#include <vector>
#include <utility>

struct StraightLineParams {
  TVector3 origin;
  TVector3 direction;
};

class PndMQStraightLineTrackFinder {
 public:
  /** Default constructor **/
  PndMQStraightLineTrackFinder();
  /** Destructor **/
  virtual ~PndMQStraightLineTrackFinder();

  void SetVerbose(Int_t verbose) { fVerbose = verbose; };
  void SetSearchRadius(Double_t accu) { dXY = accu; };
  std::vector<PndTrackCand> FindTracks(std::vector<PndSdsHit> hits);

 protected:
  Double_t GetTrackDip(PndMCTrack *myTrack);
  Double_t GetTrackCurvature(PndMCTrack *myTrack);
  void SortHitsToLayers(std::vector<PndSdsHit> hits);
  std::vector<std::vector<int>> GetStartCombination(int firstLayer, int secondLayer);
  PndTrackCand FindTrack(std::vector<int> startCombi) StraightLineParams GetLineParameters(std::vector<int> startCombi);
  TVector3 PropagateToXYPlane(StraightLineParams line, Double z);
  double DistanceOfPoints(TVector3 first, TVector3 second);

 private:
  Double_t fdXY;
  Int_t fNLayers;
  std::vector<std::vector<std::pair<PndSdsHit, bool>>> fHitsPerLayer;
};

#endif /* PndMQStraightLineTrackFinder_H */
