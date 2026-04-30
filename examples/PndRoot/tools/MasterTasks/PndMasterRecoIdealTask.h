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
 * @class PndMasterRecoIdealTask
 * @brief The default reconstruction tasks
 * @details
 * # Master Reconstruction Task Class
 * This class includes all the reconstruction tasks which need to be used in the default reconstruction macros.
 * @remark If you find some obsolete task which needs to be changed, contact the computing coordinator.
 * @author Stefano Spataro <spataro@to.infn.it>, Torino University
 * @version 1.0
 * @date Feb 1, 2016
 **
 **/

#ifndef PNDMASTERRECOIDEALTASK_H
#define PNDMASTERRECOIDEALTASK_H

#include "PndMasterTask.h"

class TClonesArray;

class PndMasterRecoIdealTask : public PndMasterTask {
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
  PndMasterRecoIdealTask(TString fOptions = "");

  /**
   * @brief Destructor
   */
  virtual ~PndMasterRecoIdealTask();

 private:

  TString fOptions; ///< Options parsed to the reconstruction

  /** @cond CLASSIMP */
  ClassDef(PndMasterRecoIdealTask, 2);
  /** @endcond */
};

#endif /* PNDMASTERRECOTASK_H */
