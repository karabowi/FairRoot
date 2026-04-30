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
 * PndTrackEvaluatorDetStt.h
 *
 *  Created on: 19.08.2021
 *      Author: tstockmanns
 */

#ifndef TRACKING_PNDTRACKINGTOOLS_PNDTRACKEVALUATORDETSTT_H_
#define TRACKING_PNDTRACKINGTOOLS_PNDTRACKEVALUATORDETSTT_H_

#include "PndTrackEvaluatorDetI.h"
//#include "PndSttStrawMap.h"
#include "PndSttHit.h"
//#include "PndSttGeometryMap.h"

#include "FairLink.h"
#include "FairRootManager.h"

class PndStt2GeoHandler;

class PndTrackEvaluatorDetStt : public PndTrackEvaluatorDetI {
 public:
  PndTrackEvaluatorDetStt(PndStt2GeoHandler *geoH);
  //PndTrackEvaluatorDetStt(PndSttGeometryMap *sttMap);
  virtual ~PndTrackEvaluatorDetStt();

  virtual bool CheckFirstHit(FairLink *hit);
  virtual bool CheckFirstHit(PndSttHit *hit);

  virtual bool CheckLastHit(FairLink *hit);
  virtual bool CheckLastHit(PndSttHit *hit);

  virtual bool CheckTwoHits(FairLink *first, FairLink *second);
  virtual bool CheckTwoHits(PndSttHit *firstSttHit, PndSttHit *secondSttHit);

  virtual bool CheckNextDetector(FairLink *first, FairLink *second) { return true; }

 protected:
  PndSttHit *GetSttHit(FairLink *hit) { return static_cast<PndSttHit *>(FairRootManager::Instance()->GetCloneOfLinkData(*hit)); }

 private:
  PndStt2GeoHandler *fSttGeoH;
  //PndSttGeometryMap *fSttMap;
};

#endif /* TRACKING_PNDTRACKINGTOOLS_PNDTRACKEVALUATORDETSTT_H_ */
