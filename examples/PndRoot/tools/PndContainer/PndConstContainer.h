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
 * @file PndConstContainer.h
 * @author Ben Salisbury (salisbury@hiskp.uni-bonn.de)
 * @brief Container to wrap PndTCA/STDConstContainer (not needed anymore)
 *
 *
 * @version 0.1
 * @date 2020-12-04
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef PNDCONSTCONTAINER_HH
#define PNDCONSTCONTAINER_HH

#include <vector>

#include "PndContainerI.h"
#include "PndSTDConstContainer.h"
#include "PndTCAConstContainer.h"

template <class T>
class PndConstContainer : public PndConstContainerI<T> {

 public:
  /**
   * @brief Construct a new PndConstContainer object
   *
   * @param const TString &t_foldername : Name of folder
   * @param Bool_t t_persistency : Should date be stored to file
   */
  PndConstContainer() : PndConstContainerI<T>() {}

  /**
   * @brief Destroy the PndConstContainer object
   *
   */
  virtual ~PndConstContainer(){};

  /**
   * @brief Set the Input Container object
   *
   * @param t_container
   */
  void SetConstContainer(PndConstContainerI<T> *t_container) { this->fImp.reset(t_container); }

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
  virtual T const *GetElement(Int_t t_index) const final
  {
    if (this->fImp != nullptr) {
      return this->fImp->GetElement(t_index);
    }
    return nullptr;
  }

 protected:
  std::unique_ptr<PndConstContainerI<T>> fImp{nullptr};
};

#endif /*PNDCONSTCONTAINER_HH*/
