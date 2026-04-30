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
 * PndTrackEvaluatorDetI.h
 *
 *  Created on: 19.08.2021
 *      Author: tstockmanns
 */

#ifndef TRACKING_PNDTRACKINGTOOLS_PNDTRACKEVALUATORDETI_H_
#define TRACKING_PNDTRACKINGTOOLS_PNDTRACKEVALUATORDETI_H_

class FairLink;

class PndTrackEvaluatorDetI {
 public:
  PndTrackEvaluatorDetI();
  virtual ~PndTrackEvaluatorDetI();

  virtual bool CheckFirstHit(FairLink *hit) { return true; }
  virtual bool CheckLastHit(FairLink *hit) { return true; }
  virtual bool CheckTwoHits(FairLink *first, FairLink *second) { return true; }
  virtual bool CheckNextDetector(FairLink *first, FairLink *second) { return true; }
};

#endif /* TRACKING_PNDTRACKINGTOOLS_PNDTRACKEVALUATORDETI_H_ */
