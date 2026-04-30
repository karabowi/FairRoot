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

#ifndef PNDCONSTCONTAINERI_HH
#define PNDCONSTCONTAINERI_HH

#include "PndContainerI.h"

template <class T>
class PndConstContainerI : public PndContainerI<T> {

 public:
  /**
   * @brief Construct a new PndConstContainerI object
   */
  PndConstContainerI() : PndContainerI<T>{} {}
  /**
   * @brief Destroy the PndConstContainerI object
   *
   */
  virtual ~PndConstContainerI(){};

  /**
   * @brief Get the Element object at t_index
   *
   * @param t_index
   * @return T*
   */
  virtual T const *GetElement(Int_t t_index) const = 0;

  /**
   * @brief Get the Element object at t_index
   *
   * @param t_index
   * @return T const *
   */
  virtual T const *GetConstElementPtr(Int_t t_index) const { return this->GetElement(t_index); }

  /**
   * @brief return vector of ptrs pointing at internal data, to be used to access data independent of underlying data container
   *
   * @return std::vector<T *>
   */
  std::vector<T const *> GetVector() const
  {
    std::vector<T const *> result{};
    result.reserve(this->GetSize());
    for (Int_t i = 0; i < this->GetSize(); ++i) {
      result.push_back(this->GetElement(i));
    }
    return result;
  }
};

#endif /*PNDCONSTCONTAINERI_HH*/
