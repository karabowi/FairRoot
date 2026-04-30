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

#ifndef PNDCONTAINERREGISTER_HH
#define PNDCONTAINERREGISTER_HH

#include <RtypesCore.h>
#include <map>

#include "PndMutableContainer.h"
#include "TString.h"

#include "FairLogger.h"

#include "PndConstContainerI.h"
#include "PndContainerBaseI.h"
#include "PndMutableContainerI.h"

class PndContainerRegister : public TObject {
 public:
  struct PndContainerType_t {
    TString fBranchname{""};
    TString fClassType{""};
    Bool_t fIsOutput{kFALSE};
    Bool_t fIsPersistent{kTRUE};
    Bool_t RequestsSpecificBranch() const { return fBranchname != ""; }
    TString GetKey() const { return fBranchname + fClassType; }
  };

  void Request(const PndContainerType_t &t_container)
  {
    auto pos = fRequests.find(t_container.GetKey());
    if (pos != fRequests.end()) {
      if ((*pos).second.fClassType != t_container.fClassType) {
        LOG(fatal) << "Attempting to request branch " << t_container.fBranchname << " for " << t_container.fClassType << " but branchname already used for "
                   << (*pos).second.fClassType << ". Ignoring this request!";
      } else {
        (*pos).second.fIsOutput |= t_container.fIsOutput;
        (*pos).second.fIsPersistent |= t_container.fIsPersistent;
        LOG(debug) << "Setting request for an output branch " << t_container.fBranchname << " for " << t_container.fClassType;
      }
    } else {
      fRequests[t_container.GetKey()] = t_container;
      LOG(debug) << "Setting request for branch " << t_container.fBranchname << " for " << t_container.fClassType;
    }
  }

  Bool_t IsBranchSet(const TString &t_branchname) const
  {
    auto pos = fInputs.find(t_branchname);
    if (pos != fInputs.end()) {
      return (pos->second) != nullptr;
    }
    auto opos = fOutputs.find(t_branchname);
    if (opos != fOutputs.end()) {
      return (opos->second) != nullptr;
    }
    return false;
  }

  template <class T>
  PndMutableContainerI<T> *GetOutput(const TString &t_branchname)
  {
    LOG(debug) << "PndContainerRegister::GetOutput(" << t_branchname << ") for " << T{}.ClassName();

    if (t_branchname == "" || fDefaultBranches[T{}.ClassName()] == t_branchname) {
      LOG(debug) << "PndContainerRegister::GetOutput(" << t_branchname << ") from default register";
      auto *defaultBranch = GetDefaultBranch<T>();
      if (dynamic_cast<PndMutableContainerI<T> *>(defaultBranch)) {
        return dynamic_cast<PndMutableContainerI<T> *>(defaultBranch);
      } else {
        LOG(error) << "Attempted to cast a InputContainer " << t_branchname << ") for " << T{}.ClassName() << " to PndMutableContainerI! Returning nullptr!";
        return nullptr;
      }
    }
    return dynamic_cast<PndMutableContainerI<T> *>(fOutputs[t_branchname]);
  }

  template <class T>
  void SetOutput(const TString &t_branchname, PndContainerI<T> *t_ptr)
  {
    const TString classname = t_ptr->GetClassName();
    fOutputs[t_branchname] = t_ptr;
    fInputs[t_branchname] = t_ptr;

    if (fDefaultBranches.find(classname) == fDefaultBranches.end()) {
      SetAsDefaultBranchFor(t_branchname, classname);
    }
  }

  template <class T>
  PndContainerI<T> *GetInput(const TString &t_branchname)
  {
    LOG(debug) << "PndContainerRegister::GetInput(" << t_branchname << ") for " << T{}.ClassName();
    if (t_branchname == "" || fDefaultBranches[T{}.ClassName()] == t_branchname) {
      LOG(debug) << "PndContainerRegister::GetInput(" << t_branchname << ") from default register";

      return (GetDefaultBranch<T>());
    }
    return dynamic_cast<PndContainerI<T> *>(fInputs[t_branchname]);
  }

  template <class T>
  void SetInput(const TString &t_branchname, PndContainerI<T> *t_ptr)
  {
    const TString classname = t_ptr->GetClassName();
    fInputs[t_branchname] = t_ptr;
    if (fDefaultBranches.find(classname) == fDefaultBranches.end()) {
      SetAsDefaultBranchFor(t_branchname, classname);
    }
  }

  void SetAsDefaultBranchFor(const TString &t_branchname, const TString &t_class)
  {
    LOG(info) << "PndContainerRegister::SetAsDefaultBranchFor() Setting " << t_branchname << " as active branch for " << t_class;
    fDefaultBranches[t_class] = t_branchname;
  }

  template <class T>
  PndContainerI<T> *GetDefaultBranch()
  {
    const TString classname = T{}.ClassName();
    const TString &defaultBranch = fDefaultBranches[classname];
    LOG(debug) << "PndContainerRegister::GetDefaultBranch() Returning default branch " << defaultBranch << " for " << classname;

    auto pos = fInputs.find(defaultBranch);
    if (pos != fInputs.end()) {
      return dynamic_cast<PndContainerI<T> *>(pos->second);
    }
    return dynamic_cast<PndContainerI<T> *>(fOutputs[defaultBranch]);
  }

  const TString &GetCurrentDefaultBranchName(const TString &t_classname) const { return fDefaultBranches.at(t_classname); }

  template <class T>
  const TString &GetCurrentDefaultBranchName() const
  {
    const TString classname = T{}.ClassName();
    return GetCurrentDefaultBranchName(classname);
  }

  const std::map<TString, PndContainerType_t> &GetRequests() { return fRequests; }
  const std::map<TString, TString> &GetBranches() { return fContainerOfType; }
  std::map<TString, PndContainerBaseI *> &Inputs() { return fInputs; }
  std::map<TString, PndContainerBaseI *> &Outputs() { return fOutputs; }

  virtual ~PndContainerRegister() = default;

 private:
  std::map<TString, PndContainerType_t> fRequests{};
  std::map<TString, TString> fDefaultBranches{};
  std::map<TString, TString> fContainerOfType{};
  std::map<TString, PndContainerBaseI *> fInputs{};
  std::map<TString, PndContainerBaseI *> fOutputs{};

  ClassDef(PndContainerRegister, 1);
};

#endif /*PNDCONTAINERREGISTER_HH*/
