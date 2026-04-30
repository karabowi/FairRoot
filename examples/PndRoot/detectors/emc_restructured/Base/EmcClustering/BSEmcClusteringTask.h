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

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
// File and Version Information:
// $Id: $
// Software developed for the PANDA Detector at GSI.
// Author List:
//	Jan Zhong
//	Marcel Tiemens
//	Áron Kripkó
//---------------------------------------------------------------------
//#pragma once
#ifndef BSEMCCLUSTERINGTASK_HH
#define BSEMCCLUSTERINGTASK_HH

#include <string>
#include <vector>

#include "FairLogger.h"

#include "PndConstContainer.h"
#include "PndContainerI.h"
#include "PndMutableContainer.h"
#include "PndProcess.h"
#include "PndProcessTask.h"

#include "BSEmcCluster.h"

#include "BSEmcClusteringPar.h"
#include "BSEmcDigi.h"
#include "BSEmcMCDeposit.h"
#include "BSEmcSubCluster.h"

/**
 * @class BSEmcClusteringTask
 * @brief Task to orchestrate the full reconstruction line from BSEmcDigis into BSEmcRecoHits
 * @details Attache Clustering/Reco-Processes to this task.
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcClusteringTask : public PndProcessTask {
 public:
  // Constructors
  BSEmcClusteringTask(const std::string &t_detectorName = "Barrel", Bool_t t_storeclusters = kTRUE);
  // Destructor
  virtual ~BSEmcClusteringTask();

  void SetMCDepositBranchName(const std::string &t_branchname) { fMCDepositBranchname = t_branchname; }
  void SetDigiBranchName(const std::string &t_branchname) { fDigiBranchname = t_branchname; }
  void SetDigiBranchAsMutable(Bool_t t_mutable) { fMutableDigiBranch = t_mutable; }
  void SetPreclusterBranchName(const std::string &t_branchname) { fPreclusterBranchname = t_branchname; }
  void SetClusterBranchName(const std::string &t_branchname) { fClusterBranchname = t_branchname; }
  void SetSubClusterBranchName(const std::string &t_branchname) { fSubClusterBranchname = t_branchname; }
  void SetRecoHitBranchName(const std::string &t_branchname) { fRecoHitBranchname = t_branchname; }

 protected:
  virtual void SetupDataArrays() /*override*/;
  virtual void PreProcessing(Option_t * /*unused*/) /*override*/;
  virtual void PostProcessing(Option_t * /*unused*/) /*override*/;

 private:
  std::string fMCDepositBranchname{""};
  std::string fDigiBranchname{""};
  std::string fPreclusterBranchname{""};
  std::string fClusterBranchname{""};
  std::string fSubClusterBranchname{""};
  std::string fRecoHitBranchname{""};

  Bool_t fMutableDigiBranch{kFALSE};
  ClassDef(BSEmcClusteringTask, 1)
};

#endif /*BSEMCCLUSTERINGTASK_HH*/
