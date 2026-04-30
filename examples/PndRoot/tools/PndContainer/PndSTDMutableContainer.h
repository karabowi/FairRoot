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
 * @file PndSTDMutableContainer.h
 * @author Ben Salisbury (salisbury@hiskp.uni-bonn.de)
 * @brief Input and Output Container implementation of PndMutableContainerI
 using an underlying std::vector<T> (const)*
 *
 * Contains definition and implementation of PndSTDMutableContainer<T>

 * @version 0.1
 * @date 2020-12-04
 *
 * @copyright Copyright (c) 2020
 *
 */
#ifndef PNDSTDMUTABLECONTAINER_HH
#define PNDSTDMUTABLECONTAINER_HH

#include <stdexcept>
#include <vector>

#include "TString.h"

#include "FairLogger.h"
#include "FairRootManager.h"

#include "PndMutableContainerI.h"

/**
 * @brief PndSTDMutableContainer implementation of PndMutableContainerI<T> for
 * FairRootManager and std::vector<T>
 *
 * Output data registered with FairRootManager as (std::vector<T> *)
 *
 * @tparam T
 */
template <class T>
class PndSTDMutableContainer : public PndMutableContainerI<T> {
 public:
  /**
   * @brief Construct a new PndSTDMutableContainer object
   *
   * @param const TString &t_foldername = ""
   * @param Bool_t t_persistency = kTRUE
   */
  PndSTDMutableContainer(const TString &t_foldername = "", Bool_t t_persistency = kTRUE) : PndMutableContainerI<T>(t_foldername, t_persistency) {}

  /**
   * @brief Destroy the PndSTDMutableContainer object
   *
   */
  virtual ~PndSTDMutableContainer() {}

  /**
   * @brief Get the t_idx Element
   *
   * @param Int_t t_idx
   * @return T*
   */
  virtual T *GetElement(Int_t t_idx) const final
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
   * @brief Create a Copy object of t_element and return pointer to copy
   *
   * @param const T &t_element
   * @return T*
   */
  virtual T *CreateCopy(const T &t_element)
  {
    if (fSTD != nullptr) {
      fSTD->push_back(T{t_element});
    } else {
      LOG(error) << "PndSTDMutableContainer::CreateCopy() no underlying std::vector";
      return nullptr;
    }
    return GetElement(GetSize() - 1);
  }

  /**
   * @brief remove t_index element
   *
   * @param Int_t t_index
   */
  virtual void RemoveAt(Int_t t_index) final
  {
    if (this->fSTD != nullptr) {
      this->fSTD->erase(this->fSTD->begin() + t_index);
    }
  }

  /**
   * @brief Compress
   *
   */
  virtual void Compress() {}

  /**
   * @brief "Delete" all elements
   *
   */
  virtual void Reset() final
  {
    if (this->fSTD != nullptr) {
      this->fSTD->clear();
    }
  }

  /**
   * @brief Set the Data
   *
   * @param std::vector<T> *t_container
   */
  void SetData(std::vector<T> *t_container) { fSTD = t_container; }

  /**
   * @brief Get the Data
   *
   * @return std::vector<T>*
   */
  std::vector<T> *GetData() { return fSTD; }

 private:
  std::vector<T> *fSTD{nullptr};
};

#endif /*PNDSTDMUTABLECONTAINER_HH*/
