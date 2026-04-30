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

#ifndef PNDPROCESSORHANDLER_HH
#define PNDPROCESSORHANDLER_HH

#include <memory>
#include <string>

#include "PndParameterRegister.h"
#include "PndContainerRegister.h"
#include "PndProcess.h"

/**
 * @brief Base class for classes handling a set of PndProcesses.
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 *
 * @tparam DataStruct
 */
class PndProcessHandler {
 public:
  PndProcessHandler(const std::string &t_detectorname = "") : fDetectorName(t_detectorname){};
  virtual ~PndProcessHandler();

  /**
   * @brief Add a PndProcess of appropriate type to the list of PndProcesses to be managed
   *
   */
  void AddProcess(PndProcess * /*t_process*/);

  /**
   * @brief Call for each PndProcess the SetupParameter
   *
   */
  void SetupProcesses();

  const std::vector<PndProcess *> &GetProcesses() { return fProcesses; }
  PndParameterRegister *GetParameterRegister() { return fParameterRegister.get(); }
  const PndParameterRegister *GetParameterRegister() const { return fParameterRegister.get(); }
  const std::string &GetDetectorName() const { return fDetectorName; }

  virtual void GetRequiredDataContainerInfo();
  virtual void PassDataContainersToProcesses();

 protected:
  virtual void SetupProcess(PndProcess *t_process);

  std::vector<PndProcess *> fProcesses{};
  std::unique_ptr<PndParameterRegister> fParameterRegister{new PndParameterRegister};
  std::unique_ptr<PndContainerRegister> fContainerRegister{new PndContainerRegister};
  std::string fDetectorName{""};
};


#endif /*PNDPROCESSORHANDLER_HH*/
