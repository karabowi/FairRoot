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
 * @file PndSTDConstContainer.h
 * @author Ben Salisbury (salisbury@hiskp.uni-bonn.de)
 * @brief Input and Output Container implementation of PndConstContainerI using
 * an underlying std::vector<T> (const)*
 *
 * Contains definition and implementation of PndSTDConstContainer<T> and
 * PndSTDMutableContainer<T>
 *
 * PndSTDConstContainer<T>  causes some issues. We cannot implement
 * std::vector<T *> GetVector() in PndConstContainerI without T *
 * GetElement(Int_t t_idx)
 * on the std::vector<T> const * fSTD which is why we had to implement an ugly
 * workaround by copying  fSTD into a persistent helper vector<T> and pass that
 * address
 * in GetElement. ConstContainer should not be changed anyhow, but this is
 * definitely odd.
 *
 * @version 0.1
 * @date 2020-12-04
 *
 * @copyright Copyright (c) 2020
 *
 */
#ifndef PNDSTDCONSTCONTAINER_HH
#define PNDSTDCONSTCONTAINER_HH

#include <stdexcept>
#include <vector>

#include "TString.h"

#include "FairLogger.h"
#include "FairRootManager.h"

#include "PndConstContainerI.h"

/**
 * @brief PndSTDConstContainer implementation of PndConstContainerI<T> for
 * FairRootManager and std::vector<T>
 *
 * Input data retrieved from FairRootManager is immutable (std::vector<T> const
 * *)
 *
 * @tparam T
 */
template <class T>
class PndSTDConstContainer : public PndConstContainerI<T> {
 public:
  /**
   * @brief Construct a new PndSTDConstContainer object
   *
   */
  PndSTDConstContainer() : PndConstContainerI<T>() {}
  /**
   * @brief Destroy the PndSTDConstContainer object
   *
   */
  virtual ~PndSTDConstContainer(){};

  /**
   * @brief Get the t_idx Element
   *
   * @param t_idx
   * @return T const *
   */
  virtual T const *GetElement(Int_t t_idx) const final
  {
    if (fSTD != nullptr) {
      return &(fSTD->at(t_idx));
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
    if (fSTD != nullptr) {
      return fSTD->size();
    }
    return 0;
  }
  /**
   * @brief Set the Data
   *
   * @param t_container
   */
  void SetData(std::vector<T> const *t_container) { fSTD = t_container; }

  /**
   * @brief Get the Data
   *
   * @return std::vector<T> const*
   */
  std::vector<T> const *GetData() { return fSTD; }

 private:
  std::vector<T> const *fSTD{nullptr};
};

#endif /*PNDSTDCONSTCONTAINER_HH*/
