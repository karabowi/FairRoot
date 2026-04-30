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

#ifndef PNDPROCESSTASK_H
#define PNDPROCESSTASK_H

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

#include "TClonesArray.h"
#include "TString.h"

#include "FairLogger.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "PndConstContainer.h"
#include "PndMutableContainer.h"
#include "PndPersistencyTask.h"
#include "PndProcess.h"
#include "PndProcessHandler.h"

/**
 * @brief Task based version of a PndProcessHandler.
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @tparam DataStruct
 */

class PndProcessTask : public PndPersistencyTask, public PndProcessHandler {
 public:
  PndProcessTask(const std::string &t_detectorName);
  virtual ~PndProcessTask();

  /**
   * @brief Initialise this task. This must be called AFTER SetParContainers(), otherwise we will try to pass the parameters to the PndProcesses, while the parameter were not
   * fetched yet.
   *
   * @return InitStatus
   */
  virtual InitStatus Init() /*override*/ final;

  /**
   * @brief Prepare Task for processing of PndProcess (e.g. reset data arrays), process the PndProcesses and do cleanup during the PostProcessing
   *
   */
  virtual void Exec(Option_t *t_opt = nullptr) /*override*/;

  /**
   * @brief End the task, call TearDown for all PndProcesses
   *
   */
  virtual void Finish() /*override*/;

 protected:
  /**
   * @brief Fetch the FairRuntimeDb and populate the PndParameterRegister with all requested parameter. Has to be called before Init()
   *
   */
  void SetParContainers() /*override*/ final;

  /**
   * @brief Do some task preparation before PndProcesses are run once per event.
   *
   */
  virtual void PreProcessing(Option_t * /*unused*/);

  /**
   * @brief Do some task post clean up after PndProcesses are run once per event.
   *
   */
  virtual void PostProcessing(Option_t * /*unused*/);

  /**
   * @brief Define which data containers need to be fetched from the FairRootManager, pass the data container addresses into a simple struct to distribute the addresses to the
   * PndProcesses so they can read and write the data to them. Needs to be implemented in the derived PndProcessTask, such as the BSEmcClusteringTask.
   */
  virtual void SetupDataArrays() = 0;

  template <class T>
  Bool_t DispatchRequest(const PndContainerRegister::PndContainerType_t &t_container, PndContainerRegister *t_register, const TString &t_defaultName)
  {
    if (t_container.fClassType == T().ClassName()) {
      if (!HandledRequest<T>(t_container, t_register, t_defaultName)) {
        LOG(error) << "PndProcessTask::DispatchRequest(): " << this->fDetectorName << " - No " << t_container.fClassType << " branch found. Aborting.";
        throw std::exception(); // Exception is catched in PndProcessTask::Init(), which will return kERROR InitStatus
      }
      return true;
    }
    return false;
  }

  TClonesArray *GetInputArray(const TString &t_branchname, const TString &t_classtype)
  {
    TClonesArray *tca{nullptr};
    FairRootManager *ioman = FairRootManager::Instance();
    if (ioman == nullptr) {
      throw std::runtime_error("FairRootManager::Instance() returned nullptr");
    } else {
      tca = dynamic_cast<TClonesArray *>(ioman->GetObject(t_branchname));
      if (tca == nullptr) {
        LOG(error) << "PndTCAConstContainer<" << t_classtype << ">::Init(" << t_branchname << "): No " << t_classtype << " array with name " << t_branchname << " found!";
        return tca;
      }
    }
    return tca;
  }

  TClonesArray *GetOutputArray(const TString &t_branchname, const TString &t_classtype, const TString &t_foldername, Bool_t t_persistenxy)
  {
    TClonesArray *tca{nullptr};
    FairRootManager *ioman = FairRootManager::Instance();
    if (ioman == nullptr) {
      throw std::runtime_error("FairRootManager::Instance() returned nullptr");
    } else {
      if (ioman->CheckBranch(t_branchname) == 0) {
        LOG(debug) << "Attempting to register " << t_branchname << " as a container for " << t_classtype << " in folder " << t_foldername;
        tca = ioman->Register(t_branchname, t_classtype, t_foldername, t_persistenxy);
      } else {
        tca = dynamic_cast<TClonesArray *>(ioman->GetObject(t_branchname));
        if (tca == nullptr) {
          LOG(error) << "PndTCAMutableContainer<" << t_classtype << ">::Init(" << t_branchname << "): No " << t_classtype << " array with name " << t_branchname
                     << " already exists but could not be fetched!";

          return tca;
        }
        LOG(debug) << "Found a branch already called " << t_branchname << ". Using this for container for " << t_classtype << " in folder " << t_foldername;
      }
    }
    return tca;
  }

  template <class T>
  Bool_t HandledRequest(const PndContainerRegister::PndContainerType_t &t_container, PndContainerRegister *t_register, const TString &t_defaultName)
  {

    TString branchname = (t_container.RequestsSpecificBranch()) ? t_container.fBranchname : t_defaultName;
    LOG(debug) << "PndProcessTask::HandledRequest " << t_container.fBranchname << "(" << branchname << ") for " << t_container.fClassType;

    if (t_register->IsBranchSet(branchname)) {
      return true;
    }
    Bool_t status = kFALSE;
    if (t_container.fIsOutput) {

      PndTCAMutableContainer<T> *output = new PndTCAMutableContainer<T>();
      output->SetPersistency(t_container.fIsPersistent);
      output->SetTCA(GetOutputArray(branchname, t_container.fClassType, "Emc" + fDetectorName, t_container.fIsPersistent));
      output->SetBranchName(branchname);

      t_register->SetOutput(branchname, output);
      LOG(debug) << "PndProcessTask::HandledRequest " << t_container.fBranchname << "(" << branchname << ") for " << t_container.fClassType << " as output container.";
      status = kTRUE;
    } else {
      PndTCAConstContainer<T> *input = new PndTCAConstContainer<T>();
      input->SetTCA(GetInputArray(branchname, t_container.fClassType));
      input->SetBranchName(branchname);

      t_register->SetInput(branchname, input);
      LOG(debug) << "PndProcessTask::HandledRequest " << t_container.fBranchname << "(" << branchname << ") for " << t_container.fClassType << " as input container.";
      status = kTRUE;
    }
    return status;
  }

 private:
  ClassDef(PndProcessTask, 1);
};

#endif /*PNDPROCESSTASK_H*/
