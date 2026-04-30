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
 * @file PndMutableContainer.h
 * @author Ben Salisbury (salisbury@hiskp.uni-bonn.de)
 * @brief Container to wrap PndTCA/STDMutableContainer (not needed anymore)
 *
 *
 * @version 0.1
 * @date 2020-12-04
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef PNDMUTABLECONTAINER_HH
#define PNDMUTABLECONTAINER_HH

#include <vector>

#include "PndContainerI.h"
#include "PndSTDMutableContainer.h"
#include "PndTCAMutableContainer.h"

template <class T>
class PndMutableContainer : public PndMutableContainerI<T> {

 public:
  /**
   * @brief Construct a new PndMutableContainer object
   *
   * @param const TString &t_foldername : Name of folder
   * @param Bool_t t_persistency : Should date be stored to file
   */
  PndMutableContainer(const TString &t_foldername = "", Bool_t t_persistency = kTRUE) : PndMutableContainerI<T>(t_foldername, t_persistency) {}

  /**
   * @brief Destroy the PndMutableContainer object
   *
   */
  virtual ~PndMutableContainer(){};

  /**
   * @brief Set the Output Container object
   *
   * @param t_container
   */
  void SetContainer(PndMutableContainerI<T> *t_container) { this->fImp.reset(t_container); }

  /**
   * @brief Reset data
   *
   * Should delete current data
   *
   */
  virtual void Reset() final
  {
    if (this->fImp != nullptr) {
      this->fImp->Reset();
    }
  }

  /**
   * @brief Create a copy of t_data in internal data and return ptr to it
   *
   * @param t_data
   * @return T*
   */
  virtual T *CreateCopy(const T &t_data) final
  {
    if (this->fImp != nullptr) {
      return this->fImp->CreateCopy(t_data);
    }
    return nullptr;
  }

  /**
   * @brief Delete object at t_index in internal data
   *
   * @param t_index
   */
  virtual void RemoveAt(Int_t t_index) final
  {
    if (this->fImp != nullptr) {
      this->fImp->RemoveAt(t_index);
    }
  }

  /**
   * @brief Get the number of members in internal data
   *
   * @return ssize_t
   */
  virtual ssize_t GetSize() const final
  {
    if (this->fImp != nullptr) {
      return this->fImp->GetSize();
    }
    return 0;
  }

  /**
   * @brief Get the Element object at t_index
   *
   * @param t_index
   * @return T*
   */
  virtual T *GetElement(Int_t t_index) const final
  {
    if (this->fImp != nullptr) {
      return this->fImp->GetElement(t_index);
    }
    return nullptr;
  }

  /**
   * @brief Compress
   *
   */
  virtual void Compress()
  {
    if (this->fImp != nullptr) {
      this->fImp->Compress();
    }
  }

 protected:
  std::unique_ptr<PndMutableContainerI<T>> fImp{nullptr};
};

#endif /*PNDMUTABLECONTAINER_HH*/
