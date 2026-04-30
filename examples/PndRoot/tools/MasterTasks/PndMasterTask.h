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
 * @class PndMasterTask
 * @brief Abstract class for all the master task list classes
 * @details # Master task Class
 * This class is the basic for all the master task classes, digi, reco, pid and so on.
 * It provides basic functionalities, such as the possibility to set the Verbosity for each task separately or for all the tasks together, the possibility to getretrieve a Task in
 * order to use some setter, and a print of all the included tasks.
 * @author Stefano Spataro <spataro@to.infn.it>, Torino University
 * @version 1.0
 * @date Feb 1, 2016
 **
 **/

#ifndef PNDMASTERTASK_H
#define PNDMASTERTASK_H

#include "PndPersistencyTask.h"

class TClonesArray;

class PndMasterTask : public PndPersistencyTask {
 public:
  /**
   * @brief Default constructor
   */
  PndMasterTask();

  /**
   * @brief Proper Constructor
   */
  PndMasterTask(const char *name);

  /**
   * @brief Destructor
   */
  virtual ~PndMasterTask();

  /**
   * @brief It prints the list of tasks
   */
  void PrintTaskList();

  /**
   * @brief Set the persistency of all the tasks
   * @detail This function has to be implemented for each master task, since
   * different tasks use different functions to change the persistency of
   * the output TClonesArrays
   * @param fPersistency Persistency level: 0 no TCA, 1 all TCA
   */
  virtual void SetPersistency(Bool_t pers = kTRUE);

  /**
   * @brief Set the Verbosity to all the tasks
   * @param iVerbose Verbosity level: 0 no messages, the higher more messages
   */
  void SetVerbose(Int_t iVerbose = 1);

  /**
   * @brief Set the Verbosity to a single task
   * @details This function has to be used when you want to change the verbosity to a single task
   *
   * @param nTask Index of the task you want to modify. Instead of the integer you can use the enum k + task name (i.e. kMvdDigiTask, hSttHitProducerRealFast
   * @param iVerbose Verbosity. 0 -> no messages; the higher -> more messages
   */
  void SetVerbose(Int_t nTask, Int_t iVerbose);

  /**
   * @brief Return the pointer to a single task
   * @details This function allows to retrieve the pointer of a task inside the task list, when you want to use some particular setter or change someting in the task.
   *
   * @param nTask Index of the task you want to modify. Instead of the integer you can use the enum "k" + task name (i.e. kMvdDigiTask, kSttHitProducerRealFast, etc...)
   * @result A pointer to the corresponding task
   */
  FairTask *GetTask(Int_t nTask);

 protected:
  std::vector<PndPersistencyTask *> fBranchTasks;
  std::vector<FairTask *> fStandardTasks;
  std::map<PndPersistencyTask *, bool> fFixedPersistency;

 private:
  /** @cond CLASSIMP */
  ClassDef(PndMasterTask, 2); ///< 1st Implementation -> 1
                              /** @endcond */
};

#endif /* PNDMASTERTASK_H */
