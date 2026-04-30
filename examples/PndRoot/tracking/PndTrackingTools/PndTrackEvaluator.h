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
 * PndTrackEvaluator.h
 *
 *  Created on: 18.08.2021
 *      Author: tstockmanns
 */

#ifndef TRACKING_PNDTRACKINGTOOLS_PNDTRACKEVALUATOR_H_
#define TRACKING_PNDTRACKINGTOOLS_PNDTRACKEVALUATOR_H_

#include <map>
#include <vector>
#include <functional>

#include "PndTrackEvaluatorDetI.h"

#include "PndTrack.h"
class PndTrackEvaluator {
 public:
  PndTrackEvaluator();
  virtual ~PndTrackEvaluator();

  // void AddTest(std::function<bool(PndTrack)> function) { fTests.push_back(function); }
  bool IsValidTrack(PndTrack &track);

  bool CheckContinuity(PndTrack &track);

  void AddDetectorEvaluator(int branchId, PndTrackEvaluatorsDetI *eval) { detEvaluators[branchId] = eval; }

 private:
  //  std::vector<std::function<bool(PndTrack)>> fTests;
  std::map<int, PndTrackEvaluatorDetI *> detEvaluators;
  //PndSttStrawMap *fStrawMap = nullptr;
};

#endif /* TRACKING_PNDTRACKINGTOOLS_PNDTRACKEVALUATOR_H_ */
