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
 * PndTrackEvaluator.cxx
 *
 *  Created on: 18.08.2021
 *      Author: tstockmanns
 */

#include <PndTrackEvaluator.h>
#include "PndTrackCand.h"
#include "PndSttHit.h"

PndTrackEvaluator::PndTrackEvaluator()
{
  // TODO Auto-generated constructor stub
}

PndTrackEvaluator::~PndTrackEvaluator()
{
  // TODO Auto-generated destructor stub
}

bool PndTrackEvaluator::IsValidTrack(PndTrack &track)
{
  return CheckContinuity(track);
}

bool PndTrackEvaluator::CheckContinuity(PndTrack &track)
{
  bool result = true;
  PndTrackCand *trackCand = track.GetTrackCandPtr();
  std::vector<PndTrackCandHit> hitArray = trackCand->GetSortedHits();
  int previousDet = -1;
  PndTrackCandHit previousHit;
  for (auto hit : hitArray) {
    if (hit.GetType() != previousDet) {
      if (detEvaluators.count(previousDet) > 0) {
        result = detEvaluators[previousDet]->CheckLastHit(previousHit);
        result = detEvaluators[previousDet]->CheckNextDetector(previousHit, hit);
      }
      if (detEvaluators.count(hit.GetType()) > 0) {
        result = detEvaluators[hit.GetType()]->CheckFirstHit(hit);
      }
      previousDet = hit.GetType();
      previousHit = hit;
    } else {
      result = detEvaluators[hit.GetType()]->CheckTwoHits(previousHit, hit);
      previousHit = hit;
    }
    if (result == false)
      return result;
  }
  return result;
}
