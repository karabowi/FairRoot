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
 * @class PndBranchBurstBuilder_eventBased
 *
 * @brief Task Class for building Bursts of events in the simulation.
 *
 * @details
 * This Version combines a certain number of TTree entries ("Events") coming from an event based simulation/digitization to Burst of Events.
 * As the Event based output, the data in the Bursts are not sorted in time!
 * Between the output of the Bursts empty events are written to the output array.
 *
 * @date July 12, 2017
 * @author Dominik Steinschaden
 *
 */


#pragma once

#include "PndBranchBurstBuilder.h"

#include <vector>

class PndBranchBurstBuilder_eventBased : public PndBranchBurstBuilder {
 public:
  /** Default constructor **/
  PndBranchBurstBuilder_eventBased();
  /** Destructor **/
  virtual ~PndBranchBurstBuilder_eventBased();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /// Method to collect the data of full Burst for an Single Branch. inBranchNum state the position in the InputBranchName vector
  virtual TClonesArray *GetBurstData(size_t branchNum);

  virtual void FinishTask();

  void SetNEvents(int num) { fNEvents = num; }

 protected:
  int fNEvents;
  std::vector<TClonesArray *> fTempArrays;

  ClassDef(PndBranchBurstBuilder_eventBased, 0);
};

