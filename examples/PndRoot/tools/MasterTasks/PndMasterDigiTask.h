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
 * @class PndMasterDigiTask
 * @brief The default digitization tasks
 * @details
 * # Master Digitization Task Class
 * This class includes all the digitization tasks which need to be used in the default digitization macros.
 * @remark If you find some obsolete task which needs to be changed, contact the computing coordinator.
 * @author Stefano Spataro <spataro@to.infn.it>, Torino University
 * @version 1.0
 * @date Feb 1, 2016
 **
 **/

#ifndef PNDMASTERDIGITASK_H
#define PNDMASTERDIGITASK_H

#include "PndMasterTask.h"

class TClonesArray;

class PndMasterDigiTask : public PndMasterTask {
 public:
  /**
   * @brief Default constructor with options
   * @details
   * @param options = ""         -> default settings full setup
   * @param options = "day1"                      -> Setup for day1 experiment: no GEM, FTS1234, NO DISC, NO RICH
   * @param options = "gem" (added to "day1")     -> Setup for day1 experiment with 3 GEM planes
   * @param options = "fts1256" (added to "day1") -> Setup for day1 experiment with FTS1256 insted of FTS1234
   * Example: "day1+gem+fts1256" means day1 setup + GEM planes + fst1256
   */
  PndMasterDigiTask(TString options = "");

  /**
   * @brief Destructor
   */
  virtual ~PndMasterDigiTask();

  /**
   * @brief Set the persistency of all the tasks
   * @param pers Persistency level: 0 no TCA, 1 all TCA
   */
  virtual void SetPersistency(Bool_t pers = kTRUE);

 private:
  /**
   * @brief struct of the task list
   * @details This struct avoids to retrieve tasks using the integer value (misleading), but uses an easier scheme. The enum is "k" + the class name, i.e. "kPndMvdDigiTask" for the
   * class PndMvdDigiTask. The function PrintTaskList() can be used to check the list of the tasks and the corresponding number
   */
  struct digiTaskList {
    Short_t kPndSttHitProducerRealFast;
    Short_t kPndStt2DigiTask;
    Short_t kPndStt2HitProducerIdeal;
    Short_t kPndStt2HitProducerReal;
    Short_t kPndMvdDigiTask;
    Short_t kPndMvdClusterTask;
    Short_t kPndEmcHitsToWaveform;
    Short_t kPndEmcWaveformToDigi;
    Short_t kPndEmcMakeCluster;
    Short_t kPndEmcMakeBump;
    Short_t kPndSciTHitProducerIdeal;
    Short_t kPndSciTDigiTask;
    Short_t kPndMdtHitProducerIdeal;
    Short_t kPndMdtTrkProducer;
    Short_t kPndDrcHitProducerReal;
    Short_t kPndGemDigitize;
    Short_t kPndGemFindHits;
    Short_t kPndFtsHitProducerRealFast;
    Short_t kPndFtofHitProducerIdeal;
    Short_t kPndRichHitProducer;
  } digi;

  TString fOptions; ///< Options parsed to the digitization

  /** @cond CLASSIMP */
  ClassDef(PndMasterDigiTask, 2);
  /** @endcond */
};

#endif /* PNDMASTERDIGITASK_H */
