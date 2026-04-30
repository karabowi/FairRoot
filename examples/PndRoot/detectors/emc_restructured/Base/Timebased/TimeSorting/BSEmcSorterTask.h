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

/*
 * BSEmcSorterTask.h
 */

#ifndef BSEMCSORTERTASK_HH
#define BSEMCSORTERTASK_HH

#include <FairRingSorter.h>
#include <FairRingSorterTask.h>
#include <FairRootManager.h>

#include "TClonesArray.h"

#include "BSEmcRingSorter.h"

/**
 * @class BSEmcSorterTask
 * @brief templated version of a sorter task
 * @details sort Ts by time for later timebunching
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
template <class T>
class BSEmcSorterTask : public FairRingSorterTask {
 public:
  BSEmcSorterTask(){};
  BSEmcSorterTask(Int_t t_numberOfCells, Double_t t_widthOfCells, const std::string &t_inputbranch, const std::string &t_outputbranch)
    : FairRingSorterTask(t_numberOfCells, t_widthOfCells, TString{t_inputbranch}, TString{t_outputbranch}, "Emc")
  {
    fVerbose = 3;
  };

  virtual ~BSEmcSorterTask(){};

  virtual void AddNewDataToTClonesArray(FairTimeStamp *t_data) /*override*/
  {
    FairRootManager *ioman = FairRootManager::Instance();
    TClonesArray *myArray = ioman->GetTClonesArray(fOutputBranch);
    if (fVerbose > 1) {
      std::cout << "-I- BSEmcSorterTask::AddNewDataToTClonesArray Data: ";
      std::cout << *(T *)(t_data) << std::endl;
    }
    new ((*myArray)[myArray->GetEntries()]) T(*(dynamic_cast<T *>(t_data)));
  };
  virtual FairRingSorter *InitSorter(Int_t t_numberOfCells, Double_t t_widthOfCells) const /*override*/ { return new BSEmcRingSorter<T>(t_numberOfCells, t_widthOfCells); };
  void SetSourceBranch(const TString &t_branchname) { this->fInputBranch = t_branchname; }
  void SetTargetBranch(const TString &t_branchname) { this->fOutputBranch = t_branchname; }

  ClassDef(BSEmcSorterTask, 1);
};

#endif /*BSEMCSORTERTASK_HH*/
