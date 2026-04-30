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
 * @file PndTCAMutableContainer.h
 * @author Ben Salisbury (salisbury@hiskp.uni-bonn.de)
 * @brief Input and Output Container implementation of PndMutableContainerI
 * using an underlying TClonesArray
 *
 * Contains definition and implementation of PndTCAMutableContainer<T>
 *
 * @version 0.1
 * @date 2020-12-04
 *
 * @copyright Copyright (c) 2020
 *
 */
#ifndef PNDTCAMUTABLECONTAINER_HH
#define PNDTCAMUTABLECONTAINER_HH

#include <stdexcept>
#include <vector>

#include "TClonesArray.h"
#include "TString.h"

#include "FairLogger.h"
#include "FairRootManager.h"

#include "PndMutableContainerI.h"

/**
 * @brief PndTCAMutableContainer implementation of PndMutableContainerI<T> for
 * FairRootManager and TClonesArray
 *
 * Output data registered with FairRootManager as (TClonesArray *)
 *
 * @tparam T
 */
template <class T>
class PndTCAMutableContainer : public PndMutableContainerI<T> {
 public:
  /**
   * @brief Construct a new PndTCAMutableContainer object
   *
   * @param const TString &t_foldername = ""
   * @param Bool_t t_persistency = kTRUE
   */
  PndTCAMutableContainer(const TString &t_foldername = "", Bool_t t_persistency = kTRUE) : PndMutableContainerI<T>(t_foldername, t_persistency) {}

  /**
   * @brief Destroy the PndTCAOutput Container object
   *
   */
  virtual ~PndTCAMutableContainer() {}

  /**
   * @brief Delete all elements
   *
   */
  virtual void Reset() final
  {
    if (this->fTCA != nullptr) {
      this->fTCA->Delete();
    }
  }
  /**
   * @brief Create a copy of t_element in the TClonesArray and return a pointer to it.
   *
   * @param const T &t_element
   * @return T*
   */
  virtual T *CreateCopy(const T &t_element)
  {
    if (fTCA != nullptr) {
      new ((*fTCA)[fTCA->GetEntriesFast()]) T{t_element};
    } else {
      LOG(error) << "PndTCAMutableContainer::CreateCopy() no underlying TClonesArray";
      return nullptr;
    }
    return GetElement(GetSize() - 1);
  }

  /**
   * @brief remove the t_index-th element
   *
   * @param Int_t t_index
   */
  virtual void RemoveAt(Int_t t_index)
  {
    if (this->fTCA != nullptr) {
      this->fTCA->RemoveAt(t_index);
    }
  }

  /**
   * @brief Compress
   *
   */
  virtual void Compress()
  {
    if (this->fTCA != nullptr) {
      this->fTCA->Compress();
    }
  }

  /**
   * @brief Get pointer to the t_idx-th element
   *
   * @param Int_t t_idx
   * @return T*
   */
  virtual T *GetElement(Int_t t_idx) const final
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

#endif /*PNDTCAMUTABLECONTAINER_HH*/
