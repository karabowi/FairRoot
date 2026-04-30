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

/**
 * @class PndGetEventTimeTask
 * @brief Task to extract the event time from a digi file into a vector
 *
 * @details This task runs in the `Init()` stage over all entries in the digi file
 * and generates a vector with all event times.
 * This vector is passed as a pointer in each event.
 *
 * @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 */

#pragma once

#include "TClonesArray.h"
#include "FairTask.h"
#include "FairEventHeader.h"
//#include "PndSdsHit.h"


#include <vector>

class PndGetEventTimeTask : public FairTask {
 public:
  PndGetEventTimeTask();
  virtual ~PndGetEventTimeTask();

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);
  virtual void FinishEvent();

 private:
  int fEventNr = -1;

  FairEventHeader* fHeader = nullptr;
  TBranch* fBranch = nullptr;


  std::vector<double>* fEventTime = nullptr;
  std::vector<double> fEventTimeCopy;
  bool fRunOnce = true;

  void Register();
  void Reset();
  void ProduceHits();

  ClassDef(PndGetEventTimeTask, 1);
};

