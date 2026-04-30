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

#ifndef BSEMCPLOTTASK_HH
#define BSEMCPLOTTASK_HH

#include <stdexcept>
#include <string>
#include <vector>

#include "FairLogger.h"

#include "PndConstContainer.h"
#include "PndContainerI.h"
#include "PndProcess.h"
#include "PndProcessTask.h"

#include "BSEmcCluster.h"
#include "BSEmcDigi.h"
#include "BSEmcMCDeposit.h"
#include "BSEmcMCPoint.h"
#include "BSEmcMultiWaveform.h"
#include "BSEmcPrecluster.h"
#include "BSEmcRecoHit.h"
#include "BSEmcSubCluster.h"
#include "PndMCTrack.h"

#include "BSEmcPlotProcess.h"

/**
 * @class BSEmcPlotTask
 * @brief Task to handle PlotProcesses
 * @details Fetches a set of EmcDetector Data Containers and passes their addresses on to the handled BSEmcPlotProcesses
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcPlotTask : public PndProcessTask {
 public:
  // Constructors
  BSEmcPlotTask(const std::string &t_detectorname, BSEmcHistogrammer *t_histogrammer);
  // Destructor
  virtual ~BSEmcPlotTask();

  void SetMCPointBranchName(const std::string &t_branchname) { fMCPointBranchname = t_branchname; }
  void SetMCDepositBranchName(const std::string &t_branchname) { fMCDepositBranchname = t_branchname; }
  void SetWaveformBranchName(const std::string &t_branchname) { fWaveformBranchname = t_branchname; }
  void SetDigiBranchName(const std::string &t_branchname) { fDigiBranchname = t_branchname; }
  void SetPreclusterBranchName(const std::string &t_branchname) { fPreclusterBranchname = t_branchname; }
  void SetClusterBranchName(const std::string &t_branchname) { fClusterBranchname = t_branchname; }
  void SetSubClusterBranchName(const std::string &t_branchname) { fSubClusterBranchname = t_branchname; }
  void SetRecoHitBranchName(const std::string &t_branchname) { fRecoHitBranchname = t_branchname; }

  void SetFolderPrefix(const std::string &t_folderPrefix) { fFolderPrefix = t_folderPrefix; }

 protected:
  virtual void SetupDataArrays() /*override*/;
  virtual void PreProcessing(Option_t * /*unused*/) /*override*/;
  virtual void PostProcessing(Option_t * /*unused*/) /*override*/;
  virtual void ExecuteTasks(Option_t *t_option) /*override*/;

  virtual void SetupProcess(PndProcess *t_process) /*override*/;

 private:
  BSEmcHistogrammer *fHistogrammer{nullptr};

  std::string fMCPointBranchname{""};
  std::string fMCDepositBranchname{""};
  std::string fWaveformBranchname{""};
  std::string fDigiBranchname{""};
  std::string fPreclusterBranchname{""};
  std::string fClusterBranchname{""};
  std::string fSubClusterBranchname{""};
  std::string fRecoHitBranchname{""};

  std::string fFolderPrefix{""};

  ClassDef(BSEmcPlotTask, 1)
};

#endif /*BSEMCPLOTTASK_HH*/
