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
 * @file PndTCAConstContainer.h
 * @author Ben Salisbury (salisbury@hiskp.uni-bonn.de)
 * @brief Input and Output Container implementation of PndConstContainerI using
 * an underlying TClonesArray
 *
 * Contains definition and implementation of PndTCAConstContainer<T>
 *
 * @version 0.1
 * @date 2020-12-04
 *
 * @copyright Copyright (c) 2020
 *
 */
#ifndef PNDTCACONSTCONTAINER_HH
#define PNDTCACONSTCONTAINER_HH

#include <stdexcept>
#include <vector>

#include "TClonesArray.h"
#include "TString.h"

#include "FairLogger.h"
#include "FairRootManager.h"

#include "PndConstContainerI.h"

/**
 * @brief PndTCAConstContainer implementation of PndConstContainerI<T> for
 * FairRootManager and TClonesArray
 *
 * Input data retrieved from FairRootManager is immutable (TClonesArray *)
 *
 * @tparam T
 */
template <class T>
class PndTCAConstContainer : public PndConstContainerI<T> {
 public:
  /**
   * @brief Construct a new PndTCAConstContainer object
   *
   */
  PndTCAConstContainer() : PndConstContainerI<T>() {}

  /**
   * @brief Destroy the PndTCAConstContainer object
   *
   */
  virtual ~PndTCAConstContainer(){};

  /**
   * @brief Get pointer to the t_idx-th element
   *
   * @param Int_t t_idx
   * @return T const *
   */
  virtual T const *GetElement(Int_t t_idx) const final
  {
    if (fTCA != nullptr) {
      return static_cast<T *>(fTCA->At(t_idx));
    }
    return nullptr;
  }

  /**
   * @brief Get the number of elements
   *
   * @return ssize_t
   */
  virtual ssize_t GetSize() const final
  {
    if (fTCA != nullptr) {
      return fTCA->GetEntriesFast();
    }
    return 0;
  }

  /**
   * @brief Set the TClonesArray address
   *
   * @param TClonesArray *t_tca
   */
  void SetTCA(TClonesArray *t_tca) { fTCA = t_tca; }

  /**
   * @brief Get the TClonesArray address
   *
   * @return TClonesArray *
   */
  TClonesArray *GetTCA() { return fTCA; }

 protected:
  TClonesArray *fTCA{nullptr};
};

#endif /*PNDTCAConstContainer_HH*/
