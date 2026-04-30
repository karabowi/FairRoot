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

#ifndef PndEmcPSAOverflowCombinator_h
#define PndEmcPSAOverflowCombinator_h

#include "PndEmcPSACombinator.h"

class PndEmcPSAOverflowCombinator : public PndEmcPSACombinator {
public:
    PndEmcPSAOverflowCombinator(Double_t threshold, Double_t timediff, Bool_t single_mode = kFALSE);
    virtual Int_t Combine(const std::vector<PndEmcAbsPSA*>& psa_list, const std::vector<Int_t>& nhit_list, 
        std::vector<Double_t>& energy_list, std::vector<Double_t>& time_list);

private:
    Double_t fThreshold;
    Double_t fTimeDiff;
    UInt_t match(PndEmcAbsPSA* psa1, Int_t nhit1, PndEmcAbsPSA* psa2, Int_t nhit2);
    void fill(PndEmcAbsPSA* psa, Int_t nhit);
    Bool_t overflow();

    std::vector<Double_t> fEnergyList;
    std::vector<Double_t> fTimeList;

    Bool_t fSingleAPDMode; // kTRUE for single APD, kFALSE for duo APD

    ClassDef(PndEmcPSAOverflowCombinator, 1)

};

#endif

