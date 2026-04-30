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

#ifndef BSEMCMCDEPOSITPRODUCERTASK_HH
#define BSEMCMCDEPOSITPRODUCERTASK_HH

#include "FairLogger.h"

#include "PndContainerI.h"
#include "PndConstContainer.h"
#include "PndMutableContainer.h"
#include "PndProcessTask.h"

#include "BSEmcMCDeposit.h"
#include "BSEmcMCPoint.h"
#include "PndMCTrack.h"

/**
 * @class BSEmcMCDepositProducerTask
 * @brief Task to manage all Processes required to transfrom BSEmcMCPoints into
 * BSEmcMCDeposits
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-01
 * @ingroup EmcBase
 */
class BSEmcMCDepositProducerTask : public PndProcessTask {
 public:
  BSEmcMCDepositProducerTask(const std::string &t_detectorName = "Barrel", Bool_t t_storeDeposits = kTRUE);
  virtual ~BSEmcMCDepositProducerTask();

  void SetPointBranchName(const std::string &t_depositbranchname) { fPointBranchname = t_depositbranchname; }
  void SetMCDepositBranchName(const std::string &t_depositbranchname) { fMCDepositBranchname = t_depositbranchname; }

 protected:
  virtual void PreProcessing(Option_t * /*unused*/) /*override*/;
  virtual void PostProcessing(Option_t * /*unused*/) /*override*/;

  virtual void SetupDataArrays() /*override*/;

 private:
  std::string fPointBranchname{""};
  std::string fMCDepositBranchname{""};
  ClassDef(BSEmcMCDepositProducerTask, 1);
};

#endif /*BSEMCMCDEPOSITPRODUCERTASK_HH*/
