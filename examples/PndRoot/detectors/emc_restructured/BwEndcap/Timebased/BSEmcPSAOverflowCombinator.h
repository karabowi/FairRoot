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

//-----------------------------------------------------------
// Description:
//     	Class of combinator that combine multi-PSA outputs
//      Guang Zhao
//-----------------------------------------------------------

#ifndef BSEMCPSAOVERFLOWCOMBINATOR_HH
#define BSEMCPSAOVERFLOWCOMBINATOR_HH

#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "BSEmcPSACombinator.h"

class BSEmcAbsPSA;
class TBuffer;
class TClass;
class TMemberInspector;

class BSEmcPSAOverflowCombinator : public BSEmcPSACombinator {
 public:
  BSEmcPSAOverflowCombinator(Double_t t_threshold, Double_t t_timediff, Bool_t t_single_mode = kFALSE);
  virtual Int_t Combine(const std::vector<BSEmcAbsPSA *> &t_psa_list, const std::vector<Int_t> &t_nhit_list, std::vector<Double_t> &t_energy_list,
                        std::vector<Double_t> &t_time_list) /*override*/;

 private:
  UInt_t match(BSEmcAbsPSA *t_psa1, Int_t t_nhit1, BSEmcAbsPSA *t_psa2, Int_t t_nhit2);
  void fill(BSEmcAbsPSA *t_psa, Int_t t_nhit);
  Bool_t overflow();

  Double_t fThreshold{0};
  Double_t fTimeDiff{0};

  std::vector<Double_t> fEnergyList{};
  std::vector<Double_t> fTimeList{};

  Bool_t fSingleAPDMode; // kTRUE for single APD, kFALSE for duo APD

  ClassDef(BSEmcPSAOverflowCombinator, 1)
};

#endif /*BSEMCPSAOVERFLOWCOMBINATOR_HH*/
