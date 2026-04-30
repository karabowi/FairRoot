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

/* **********************************************
 * MVA classifiers interface.                   *
 * Author: M.Babai@rug.nl                       *
 * LICENSE:                                     *
 * Version:                                     *
 * License:                                     *
 * **********************************************
 */
//#pragma once
#ifndef PND_MVA_CLASSIFIER_H
#define PND_MVA_CLASSIFIER_H

// C++ includes
#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <cmath>
#include <cassert>

// PND PID includes.
#include "PndMvaDataSet.h"
#include "PndMvaUtil.h"

/**
 * Main interface definition.
 */
class PndMvaClassifier {
 public:
  //! Constructor
  /**
   *@param InPut Input file name containing weights.
   *@ClassNames Names of available (signal) classes.
   *@VarNames Names of the used variables(features).
   */
  explicit PndMvaClassifier(std::string const &InPut, std::vector<std::string> const &ClassNames, std::vector<std::string> const &VarNames);
  //! Destructor
  virtual ~PndMvaClassifier();

  /**
   * @param EvtData: Event data to be classified.
   * @param result:  Classification results. Currently the shortest
   *  distance for each class is stored in result.
   */
  virtual void GetMvaValues(std::vector<float> EvtData, std::map<std::string, float> &result) = 0;
  /**
   *@param EvtData: Event, to be classified.
   *@return Name of the class with the best MVA value.
   */
  virtual std::string *Classify(std::vector<float> EvtData) = 0;

  // virtual std::string *Classify( std::vector<float> EvtData,
  //    		 std::map<std::string, float>& result ) = 0;

  virtual void Initialize();

  //! Get the list of available classes (labels).
  inline std::vector<PndMvaClass> const &GetClasses() const;

  //! Get the list of available variables
  inline std::vector<PndMvaVariable> const &GetVariables() const;

 protected:
  //! Normalize the given event vector.
  virtual void NormalizeEvent(std::vector<float> &EvtVector) const;

  inline void SetAppType(AppType t);

  //! Data set. Holds event Weights
  PndMvaDataSet m_dataSets;

 private:
  // To avoid mistakes.
  PndMvaClassifier(PndMvaClassifier const &other);
  PndMvaClassifier &operator=(PndMvaClassifier const &other);
};

inline void PndMvaClassifier::SetAppType(AppType t)
{
  m_dataSets.SetAppType(t);
};
// End of class interface
//___________________ Inline implement ___________
//! Get the list of available classes (labels).
inline std::vector<PndMvaClass> const &PndMvaClassifier::GetClasses() const
{
  return m_dataSets.GetClasses();
};

//! Get the list of available variables
inline std::vector<PndMvaVariable> const &PndMvaClassifier::GetVariables() const
{
  return m_dataSets.GetVars();
};
#endif
