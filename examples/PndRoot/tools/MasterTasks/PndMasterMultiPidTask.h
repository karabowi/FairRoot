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
 * @class PndMasterMultiPidTask
 * @brief The default pid tasks
 * @details
 * # Master Pid Task Class
 * This class includes all the pid tasks which need to be used in the default pid macros.
 * @remark If you find some obsolete task which needs to be changed, contact the computing coordinator.
 * @author Stefano Spataro <spataro@to.infn.it>, Torino University
 * @version 1.0
 * @date Feb 1, 2016
 **
 **/

#ifndef PndMasterMultiPidTask_H
#define PndMasterMultiPidTask_H

#include "PndMasterTask.h"
#include <array>

class TClonesArray;

class PndMasterMultiPidTask : public PndMasterTask {
 public:
  /**
   * @brief Default constructor with options
   * @details
   * @param options =""                          -> default settings full setup
   * @param options ="day1"                      -> Setup for day1 experiment: no GEM, FTS1234, NO DISC, NO RICH
   * @param options ="gem" (added to "day1")     -> Setup for day1 experiment with 3 GEM planes
   * @param options ="fts1256" (added to "day1") -> Setup for day1 experiment with FTS1256 insted of FTS1234
   * @param options ="filtered"					 -> the option filtered was used in the reco stage to remove tracks with wrong Pz
   * Example: "day1+gem+fts1256" means day1 setup + GEM planes + fst1256
   */
  PndMasterMultiPidTask(TString options = "");

  /**
   * @brief Destructor
   */
  virtual ~PndMasterMultiPidTask();

  /**
   * @brief Set the persistency of all the tasks
   * @param pers Persistency level: 0 no TCA, 1 all TCA
   */
  virtual void SetPersistency(Bool_t pers = kTRUE);

 private:
  TString fOptions;                ///< Options parsed to the pid
  std::array<Bool_t, 6> fHypoFlag; //! Flag to check which hypotheses to do pid with
  void PrintTaskSetup();

  /** @cond CLASSIMP */
  ClassDef(PndMasterMultiPidTask, 2);
  /** @endcond */
};

#endif /* PndMasterMultiPidTask_H */
