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
 * @class PndMasterLocalRecoTask
 * @brief The default local reconstruction tasks
 * @details
 * # Master Local Reconstruction Task Class
 * This class includes all the local reconstruction tasks which need to be used in the default local reconstruction macros.
 * @remark If you find some obsolete task which needs to be changed, contact the computing coordinator.
 * @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>, FZ Juelich
 * @version 1.0
 * @date Mar 1, 2018
 **
 **/

#ifndef PndMasterLocalRecoTask_H
#define PndMasterLocalRecoTask_H

#include <PndPersistencyTask.h>
#include "PndMasterTask.h"
#include "FairTask.h"

class TClonesArray;

class PndMasterLocalRecoTask : public PndMasterTask {
 public:
  /**
   * @brief Default constructor with options
   * @details
   * @param options = ""                          -> default settings full setup
   * @param options = "day1"                      -> Setup for day1 experiment: no GEM, FTS1234, NO DISC, NO RICH
   * @param options = "gem" (added to "day1")     -> Setup for day1 experiment with 3 GEM planes
   * @param options = "fts1256" (added to "day1") -> Setup for day1 experiment with FTS1256 insted of FTS1234
   * Example: "day1+gem+fts1256" means day1 setup + GEM planes + fst1256
   */
  PndMasterLocalRecoTask(TString options = "");

  /**
   * @brief Destructor
   */
  virtual ~PndMasterLocalRecoTask();

  /**
   * @brief Set the persistency of all the tasks
   * @param pers Persistency level: 0 no TCA, 1 all TCA
   */
  virtual void SetPersistency(Bool_t pers = kTRUE);

 private:
  std::vector<PndPersistencyTask *> fBranchTasks;
  std::vector<FairTask *> fStandardTasks;
  std::map<PndPersistencyTask *, bool> fFixedPersistency;

  TString fOptions; ///< Options parsed to the digitization

  /** @cond CLASSIMP */
  ClassDef(PndMasterLocalRecoTask, 1);
  /** @endcond */
};

#endif /* PndMasterLocalRecoTask_H */
