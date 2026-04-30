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
//     	Abstract class of combinator that to combine multi-PSA outputs
//      Guang Zhao
//-----------------------------------------------------------

#ifndef BSEMCPSACOMBINATOR_HH
#define BSEMCPSACOMBINATOR_HH

#include <vector>

#include "TObject.h"

class BSEmcAbsPSA;

class BSEmcPSACombinator {
 public:
  BSEmcPSACombinator() {}
  virtual ~BSEmcPSACombinator() {}
  virtual Int_t
  Combine(const std::vector<BSEmcAbsPSA *> &t_psa_list, const std::vector<Int_t> &t_nhit_list, std::vector<Double_t> &t_energy_list, std::vector<Double_t> &t_time_list) = 0;

  ClassDef(BSEmcPSACombinator, 1)
};

#endif /*BSEMCPSACOMBINATOR_HH*/
