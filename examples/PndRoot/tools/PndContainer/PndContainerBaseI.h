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
 * @file PndContainerBaseI.h
 * @author Ben Salisbury (salisbury@hiskp.uni-bonn.de)
 * @brief Interface to a datacontainer to be used in PandaROOT
 *
 * Helps to abstract the passed around data container (TClonesArray * or std::vector<T> *)
 *
 * @version 0.1
 * @date 2020-12-04
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef PNDCONTAINERBASEI_H
#define PNDCONTAINERBASEI_H

#include <vector>

#include "TString.h"

class PndContainerBaseI {

 public:
  /**
   * @brief Construct a new PndContainerBaseI object
   *
   * @param const TString &t_foldername : Name of folder
   * @param Bool_t t_persistency : Should date be stored to file
   */
  PndContainerBaseI() {}
  /**
   * @brief Destroy the PndContainerBaseI object
   *
   */
  virtual ~PndContainerBaseI(){};

  /**
   * @brief Get the number of members in internal data
   *
   * @return ssize_t
   */
  virtual ssize_t GetSize() const = 0;

  /**
   * @brief Get the BranchName
   *
   * @return const TString&
   */
  const TString &GetBranchName() const { return fBranchName; }

  /**
   * @brief Set the Branch Name
   *
   * @param t_branchname
   */
  void SetBranchName(const TString &t_branchname) { fBranchName = t_branchname; }

 protected:
  TString fBranchName{""};
};

#endif /*PNDCONTAINERBASEI_H*/
