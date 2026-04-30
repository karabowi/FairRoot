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

#ifndef PNDMUTABLECONTAINERI_HH
#define PNDMUTABLECONTAINERI_HH

#include "PndContainerI.h"

template <class T>
class PndMutableContainerI : public PndContainerI<T> {

 public:
  /**
   * @brief Construct a new PndMutableContainerI object
   *
   * @param const TString &t_foldername : Name of folder
   * @param Bool_t t_persistency : Should date be stored to file
   */
  PndMutableContainerI(const TString &t_foldername = "", Bool_t t_persistency = kTRUE) : PndContainerI<T>{}, fFolderName(t_foldername), fPersistency(t_persistency) {}
  /**
   * @brief Destroy the PndMutableContainerI object
   *
   */
  virtual ~PndMutableContainerI(){};

  /**
   * @brief Reset data
   *
   * Should delete current data
   *
   */
  virtual void Reset() = 0;

  /**
   * @brief Create a copy of t_data in internal data and return ptr to it
   *
   * @param t_data
   * @return T*
   */
  virtual T *CreateCopy(const T &t_data) = 0;

  /**
   * @brief Delete object at t_index in internal data
   *
   * @param t_index
   */
  virtual void RemoveAt(Int_t t_index) = 0;

  /**
   * @brief Compress
   *
   */
  virtual void Compress() {}

  /**
   * @brief Get the Element object at t_index
   *
   * @param t_index
   * @return T*
   */
  virtual T *GetElement(Int_t t_index) const = 0;

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
  std::vector<T *> GetVector() const
  {
    std::vector<T *> result{};
    result.reserve(this->GetSize());
    for (Int_t i = 0; i < this->GetSize(); ++i) {
      result.push_back(this->GetElement(i));
    }
    return result;
  }

  /**
   * @brief Set the FolderName
   *
   * @param t_foldername
   */
  void SetFolderName(const TString &t_foldername) { fFolderName = t_foldername; }

  /**
   * @brief Set the Persistency of the data
   *
   * @param t_persistency
   */
  void SetPersistency(Bool_t t_persistency) { fPersistency = t_persistency; }

  /**
   * @brief Get the Folder Name
   *
   * @return const TString&
   */
  const TString &GetFolderName() const { return fFolderName; }

  /**
   * @brief Get the Persistency of the data
   *
   * @return Bool_t
   */
  Bool_t GetPersistency() const { return fPersistency; }

 protected:
  TString fFolderName{""};
  Bool_t fPersistency{kFALSE};
};

#endif /*PNDMUTABLECONTAINERI_HH*/
