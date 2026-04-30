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

#ifndef PndEmcPSACombinator_h
#define PndEmcPSACombinator_h

#include "TObject.h"
#include <vector>

class PndEmcAbsPSA;

class PndEmcPSACombinator {
public:
    PndEmcPSACombinator() {}
    virtual ~PndEmcPSACombinator() {}
    virtual Int_t Combine(const std::vector<PndEmcAbsPSA*>& psa_list, const std::vector<Int_t>& nhit_list, 
        std::vector<Double_t>& energy_list, std::vector<Double_t>& time_list) = 0;

    ClassDef(PndEmcPSACombinator, 1)

};

#endif
