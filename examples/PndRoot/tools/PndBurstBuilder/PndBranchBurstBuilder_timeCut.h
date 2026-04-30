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
 * @class PndBranchBurstBuilder_timeCut
 * @brief Task Class for building Bursts of events in the time based simulation.
 *
 * @details
 * This simple Burst builder combines the data of the continuous data stream within a certain time window into on Burst.
 * Therefore it can only work on data sorted in time and is ignoring any kind of event structure from the simulation stage.
 * and the Input branches containing the Hits from the digitization are not useable after the burst builder Task anymore!
 * The Output Bursts are also sorted in time.
 *
 * @date July 12, 2017
 * @author Dominik Steinschaden
 *
 */


#pragma once

#include "PndBranchBurstBuilder.h"
#include "FairTSBufferFunctional.h"

class PndBranchBurstBuilder_timeCut : public PndBranchBurstBuilder {
 public:
  /** Default constructor **/
  PndBranchBurstBuilder_timeCut();
  /** Destructor **/
  virtual ~PndBranchBurstBuilder_timeCut();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /// Method to collect the data of full Burst for an Single Branch. inBranchNum state the position in the InputBranchName vector
  virtual TClonesArray *GetBurstData(size_t branchNum);

  void SetTimePeriod(double val) { fTimePeriod = val; }

 protected:
  double fTimePeriod;
  BinaryFunctor *fStopFunctor;

  ClassDef(PndBranchBurstBuilder_timeCut, 0);
};

