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
 * @file PndContainerI.h
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

#ifndef PNDCONTAINERI_H
#define PNDCONTAINERI_H

#include <vector>

#include "TString.h"
#include "PndContainerBaseI.h"

template <class T>
class PndContainerI : public PndContainerBaseI {

 public:
  /**
   * @brief Construct a new PndContainerI object
   *
   * @param const TString &t_foldername : Name of folder
   * @param Bool_t t_persistency : Should date be stored to file
   */
  PndContainerI() {}
  /**
   * @brief Destroy the PndContainerI object
   *
   */
  virtual ~PndContainerI(){};

  /**
   * @brief Get the Element object at t_index
   *
   * @param t_index
   * @return T const *
   */
  virtual T const *GetConstElementPtr(Int_t t_index) const = 0;

  /**
   * @brief return vector of const ptrs pointing at internal data, to be used to access data independent of underlying data container
   *
   * @return std::vector<T *>
   */
  std::vector<T const *> GetVectorOfPtrToConst() const
  {
    std::vector<T const *> result{};
    result.reserve(this->GetSize());
    for (Int_t i = 0; i < this->GetSize(); ++i) {
      result.push_back(this->GetConstElementPtr(i));
    }
    return result;
  }

  TString GetClassName() const { return T{}.ClassName(); }
};

#endif /*PNDCONTAINERI_H*/
