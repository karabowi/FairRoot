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

#ifndef PNDPARAMETERREGISTER_HH
#define PNDPARAMETERREGISTER_HH

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include "FairLogger.h"
#include "FairParGenericSet.h"
#include "FairParSet.h"

/**
 * @brief Helper class to indirect the Parameter fetching via the FairRuntimeDb
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 */
class PndParameterRegister : public TObject {
 public:
  /**
   * @brief Add a parameter name to the list of requested parameters
   *
   * @param t_request
   */
  void AddRequest(const std::string &t_request)
  {
    if (std::find(std::begin(fRequests), std::end(fRequests), t_request) == std::end(fRequests)) {
      fRequests.push_back(t_request);
    }
  }

  /**
   * @brief Add a list of parameter names to the list of requested parameters
   *
   * @param t_requests
   */
  void AddRequestList(const std::vector<std::string> &t_requests)
  {
    for (const std::string &request : t_requests) {
      AddRequest(request);
    }
  }

  /**
   * @brief Access the parameter via the name
   *
   * @param t_name
   * @return FairParSet*
   */
  FairParSet *GetParameter(const std::string &t_name) const
  {
    if (fRegister.find(t_name) != fRegister.end()) {
      return fRegister.at(t_name);
    }
    return nullptr;
  }

  /**
   * @brief Set the parameter pointer t_parameter for the parameter t_name
   *
   * @param t_name
   * @param t_parameter
   */
  void SetParameter(const std::string &t_name, FairParSet *t_parameter)
  {
    if (fRegister.find(t_name) != fRegister.end()) {
      return;
    } else {
      fRegister[t_name] = t_parameter;
    }
  }

  /**
   * @brief Get the list of requested parameter names
   *
   * @return const std::vector<std::string>&
   */
  const std::vector<std::string> &GetRequestList() const { return fRequests; }
  virtual ~PndParameterRegister() = default;

 private:
  std::vector<std::string> fRequests{};            ///< List of parameter names requested by all the PndProcesses.
  std::map<std::string, FairParSet *> fRegister{}; ///< Mapping of parameter name to FairParSet pointer so that PndProcesses can access FairParSet via PndParameterRegister

  ClassDef(PndParameterRegister, 1);
};

#endif /*PNDPARAMETERREGISTER_HH*/
