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
 * @class PndBranchBurstBuilder_timeCut_STB
 *
 * @brief Task Class for building Bursts of events in the simulation.
 *
 * @details
 * This Version contains a workaround for "semi-timebased branches" of the PndBranchBurstBuilder_timeCut
 * It checks if the fastest hit of an event occurs within a defined time window and combines these events into Bursts
 * As the Event based output, the data in the Bursts are not sorted in time!
 * Attention: to Keep this workaround simple not every potential case is taken into account.
 * If the used time windows are to small so that maybe no event occurs, unwanted splitting of events within the next Burst will happen!
 *
 * @date July 12, 2017
 * @author Dominik Steinschaden
 *
 */


#pragma once

#include "PndBranchBurstBuilder.h"
#include "TClonesArray.h"

#include <vector>


class PndBranchBurstBuilder_timeCut_STB : public PndBranchBurstBuilder {
 public:
  /** Default constructor **/
  PndBranchBurstBuilder_timeCut_STB();
  /** Destructor **/
  virtual ~PndBranchBurstBuilder_timeCut_STB();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /// Method to collect the data of full Burst for an Single Branch. inBranchNum state the position in the InputBranchName vector
  virtual TClonesArray *GetBurstData(size_t branchNum);

  void SetTimePeriod(double val) { fTimePeriod = val; }


  virtual void FinishTask();
  //    virtual void Finish() { };

 protected:
  double fTimePeriod;
  double fBurstNum;
  bool fWriteOut;
  std::vector<TClonesArray *> fTempArrays;

  ClassDef(PndBranchBurstBuilder_timeCut_STB, 0);
};

