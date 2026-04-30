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

#ifndef BSEMCDIGITIZERTASK_HH
#define BSEMCDIGITIZERTASK_HH

#include <string>

#include "FairLogger.h"
#include "FairRootManager.h"

#include "PndConstContainer.h"
#include "PndContainerI.h"
#include "PndMutableContainer.h"
#include "PndProcess.h"
#include "PndProcessTask.h"

#include "BSEmcDigi.h"

#include "BSEmcMCDeposit.h"
#include "BSEmcMultiWaveform.h"

/**
 * @class BSEmcDigitizerTask
 * @brief Task managing all processes required to create BSEmcDigis from
 * BSEmcMCDeposits
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-01
 * @ingroup EmcBase
 */
class BSEmcDigitizerTask : public PndProcessTask {
 public:
  BSEmcDigitizerTask(const std::string &t_detectorname = "Barrel", Bool_t t_storedigis = kTRUE);
  virtual ~BSEmcDigitizerTask();

  void SetMCDepositBranchName(const std::string &t_depositbranchname) { fMCDepositBranchname = t_depositbranchname; }
  void SetDigiBranchName(const std::string &t_digibranchname) { fDigiBranchname = t_digibranchname; }
  void SetWaveformBranchName(const std::string &t_waveformbranchname) { fWaveformBranchname = t_waveformbranchname; }

 protected:
  virtual void PreProcessing(Option_t * /*unused*/) /*override*/;
  virtual void PostProcessing(Option_t * /*unused*/) /*override*/;
  virtual void SetupDataArrays() /*override*/;

 private:
  std::string fMCDepositBranchname{""};
  std::string fDigiBranchname{""};
  std::string fWaveformBranchname{""};
  ClassDef(BSEmcDigitizerTask, 1);
};

#endif /*BSEMCDIGITIZERTASK_HH*/
