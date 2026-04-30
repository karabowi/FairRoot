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

#ifndef PNDPROCESS_HH
#define PNDPROCESS_HH

#include <string>
#include <typeinfo>
#include <vector>

#include "TStopwatch.h"
#include "TSystem.h"

#include "FairLogger.h"

#include "PndContainerRegister.h"
#include "PndParameterRegister.h"

/**
 * @class PndProcess
 * @brief Base Process class
 * @details Base class for Processes. Processes are intended to help separate data and parameter handling from the implementation of algorithms
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup PndEmc
 */
class PndProcess {
 public:
  PndProcess() {}
  PndProcess(const std::string &t_processname) : fProcessName(t_processname) {}
  virtual ~PndProcess() {}

  /**
   * @brief Set the Detector name. Important, as most EmcParameter need to know for which detector they need to be fetched (each Emc Subdetector has its own set of parameters)
   *
   * @param t_detectorname
   */
  virtual void SetDetectorName(const std::string &t_detectorname) { fDetectorName = t_detectorname; }

  /**
   * @brief Get the List Of Required Parameters
   *
   * @return const std::vector<std::string>&
   */
  const std::vector<std::string> &GetListOfRequiredParameters() const { return fParameterList; }

  /**
   * @brief Pass the container register to the process, and get the processes container requirements.
   *
   * @param t_register
   */
  virtual void RequestDataContainer(PndContainerRegister * /*t_register*/){};

  /**
   * @brief Pass the container register to the process, and set the processes container requirements.
   *
   * @param t_register
   */
  virtual void GetDataContainer(PndContainerRegister * /*t_register*/){};

  /**
   * @brief Fetch all parameters from the PndParameterRegister
   *
   * @param t_parameter
   */
  virtual void SetupParameters(const PndParameterRegister *t_parameter) = 0;

  /**
   * @brief PreProcess() is called before the actual Process() call in each event.
   * @details Intended for preprocess setup calls, e.g. for reseting internal process data, or, if not needed, to monitor processing time and memory increase
   */
  virtual void PreProcess()
  {
    gSystem->GetProcInfo(&fProcInfo);
    fLastMemSize = fProcInfo.fMemResident;
    fTimer.Reset();
    fTimer.Start();
  };
  /**
   * @brief The actual data transformation (digitizing, clustering, etc.) should be defined here.
   *
   */
  virtual void Process() = 0;

  /**
   * @brief Immediately after calling Process() PostProcess() is called for cleanup of internal process data, or if not needed, to end the time and memory increase measurement
   * started by PreProcess()
   *
   */
  virtual void PostProcess()
  {
    fTimer.Stop();
    fTotalTime += fTimer.RealTime();
    gSystem->GetProcInfo(&fProcInfo);
    LOG_IF(debug1, fProcInfo.fMemResident - fLastMemSize != 0) << "PndProcess" << fDetectorName << " Process() Memory increase after Process " << fProcessName << ". Increase by "
                                                               << fProcInfo.fMemResident - fLastMemSize << " KB. Total memory used is: " << fProcInfo.fMemResident << " KB.";

    fLastMemSize = fProcInfo.fMemResident;
  };

  /**
   * @brief Last actions at the end of the run
   *
   */
  virtual void TearDown() { LOG(debug) << fProcessName << " - " << fDetectorName << " took " << fTotalTime << " s processing time."; };

  /**
   * @brief Set the Event time
   *
   * @param t_eventtime
   */
  virtual void SetEventTime(Double_t t_eventtime) { fEventTime = t_eventtime; }

 protected:
  std::vector<std::string> fParameterList{}; ///< Parameter names required by this PndProcess. Needs to be populated in derived class
  std::string fDetectorName{""};             ///< Set Detector name this PndProcess transforms data for. Required for example by EMC Processes to fetch right parameters
  std::string fProcessName{"PndProcess"};    ///< Name of current PndProcess (for debugging)
  TStopwatch fTimer{};                       ///< Timer to monitor Process() time
  Double_t fTotalTime{0};                    ///< Time taken by this Process' Process()
  ProcInfo_t fProcInfo;                      ///< Helper to access cpu process Memory Info
  Long_t fLastMemSize; ///< Value containing the memory footprint of the cpu process before the Process() call of this PndProcess, to be able to flag if during Process() used
                       ///< memory increased
  Double_t fEventTime{0};
};

#endif /*PNDPROCESS_HH*/
