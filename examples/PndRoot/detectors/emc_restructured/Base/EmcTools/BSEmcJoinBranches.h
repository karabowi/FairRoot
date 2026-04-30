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

#ifndef BSEMCJOINBRANCHES_HH
#define BSEMCJOINBRANCHES_HH

#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TString.h"

#include "FairTask.h"

#include "PndPersistencyTask.h"

class TClonesArray;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcJoinBranches
 * @brief Task to join several branches (supplied as t_srcbranchnames in ctor) into one t_targetbranchname
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcJoinBranches : public PndPersistencyTask {
 public:
  BSEmcJoinBranches();
  BSEmcJoinBranches(const std::vector<TString> &t_srcbranchnames, const TString &t_targetbranchname, Bool_t t_storeTarget = kTRUE);
  virtual ~BSEmcJoinBranches();
  virtual InitStatus Init() /*override*/;
  virtual void Exec(Option_t * /*unused*/) /*override*/;

 protected:
 private:
  std::vector<TString> fSourceBranchNames{};
  TString fTargetBranchName{""};
  std::vector<TClonesArray *> fSourceArrays{}; //!
  TClonesArray *fTargetArray{nullptr};         //!

  ClassDef(BSEmcJoinBranches, 1);
};

#endif /*BSEMCJOINBRANCHES_HH*/
