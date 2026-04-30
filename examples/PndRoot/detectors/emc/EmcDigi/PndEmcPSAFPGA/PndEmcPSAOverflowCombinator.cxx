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

#include "PndEmcPSAOverflowCombinator.h"
#include "PndEmcAbsPSA.h"
#include <cmath>

PndEmcPSAOverflowCombinator::PndEmcPSAOverflowCombinator(Double_t threshold, Double_t tdiff, Bool_t single_mode) {
    fThreshold = threshold;
    fTimeDiff = tdiff;
    fSingleAPDMode = single_mode;
    fEnergyList.clear();
    fTimeList.clear();
}

//
// Give a single digi for the multi-waveform.
// Input vector index: 0 - HG/APD_0, 1 - LG/APD_0, 2 - HG/APD_1, 3 - LG/APD_1
// Functionality: combine outputs from the input vectors. Output hg signal if is not saturated, and always match hg/lg if fSingleAPDMode is set to be kFALSE
// Return: 0 for high gain, 1 for low gain, -1 for null
//
Int_t PndEmcPSAOverflowCombinator::Combine(const std::vector<PndEmcAbsPSA*>& psa_list, const std::vector<Int_t>& nhit_list, 
    std::vector<Double_t>& energy_list, std::vector<Double_t>& time_list) {

    Int_t apd_gain_index = -1;

    if (!fSingleAPDMode) {
        match(psa_list[0], nhit_list[0], psa_list[2], nhit_list[2]);
        apd_gain_index = 0;

        if (overflow()) {
            match(psa_list[1], nhit_list[1], psa_list[3], nhit_list[3]);
            apd_gain_index = 1;
        }
    }
    else {
        fill(psa_list[0], nhit_list[0]);
        apd_gain_index = 0;

        if (overflow()) { // if overflow, use lg
            fill(psa_list[1], nhit_list[1]);
            apd_gain_index = 1;
        }
    }

    energy_list = fEnergyList;
    time_list = fTimeList;
    return apd_gain_index;
}

UInt_t PndEmcPSAOverflowCombinator::match(PndEmcAbsPSA* psa1, Int_t nhit1, PndEmcAbsPSA* psa2, Int_t nhit2) {
    fEnergyList.clear();
    fTimeList.clear();


    Double_t e1, e2, t1, t2;
    Int_t psa2_min_idx = 0;
    for (Int_t i = 0; i < nhit1; i++) {
        psa1->GetHit(i, e1, t1);
        for (Int_t j = psa2_min_idx; j < nhit2; j++) { // assume times are sorted in psa1 & psa2
            psa2->GetHit(j, e2, t2);
            if (fabs(t1 - t2) < fTimeDiff) {
                //fEnergyList.push_back(e1);
                fEnergyList.push_back((e1 + e2) / 2.);
                fTimeList.push_back((t1 + t2) / 2.);
                psa2_min_idx = j + 1;
                break;
            }
        }
    }
    return fEnergyList.size();
}
void PndEmcPSAOverflowCombinator::fill(PndEmcAbsPSA* psa, Int_t nhit) {
    fEnergyList.clear();
    fTimeList.clear();
    for (Int_t i = 0; i < nhit; i++) { // hg
        Double_t e, t;
        psa->GetHit(i, e, t);
        fEnergyList.push_back(e);
        fTimeList.push_back(t);
    }
}

Bool_t PndEmcPSAOverflowCombinator::overflow() {
    std::vector<Double_t>::iterator it;
    for (it = fEnergyList.begin(); it != fEnergyList.end(); it++) {
        if ((*it) > fThreshold) return kTRUE;
    }
    return kFALSE;
}

ClassImp(PndEmcPSAOverflowCombinator)

