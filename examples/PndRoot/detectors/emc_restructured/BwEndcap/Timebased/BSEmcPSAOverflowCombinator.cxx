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

#include "BSEmcPSAOverflowCombinator.h"

#include <cmath>

#include "BSEmcAbsPSA.h"

BSEmcPSAOverflowCombinator::BSEmcPSAOverflowCombinator(Double_t t_threshold, Double_t t_tdiff, Bool_t t_single_mode)
  : BSEmcPSACombinator(), fThreshold(t_threshold), fTimeDiff(t_tdiff), fSingleAPDMode(t_single_mode)
{
  fEnergyList.clear();
  fTimeList.clear();
}

//
// Give a single digi for the multi-waveform.
// Input vector index: 0 - HG/APD_0, 1 - LG/APD_0, 2 - HG/APD_1, 3 - LG/APD_1
// Functionality: combine outputs from the input vectors. Output hg signal if is not saturated, and always match hg/lg if fSingleAPDMode is set to be kFALSE
// Return: 0 for high gain, 1 for low gain, -1 for null
//
Int_t BSEmcPSAOverflowCombinator::Combine(const std::vector<BSEmcAbsPSA *> &t_psa_list, const std::vector<Int_t> &t_nhit_list, std::vector<Double_t> &t_energy_list,
                                          std::vector<Double_t> &t_time_list)
{

  Int_t apd_gain_index = -1;

  if (!fSingleAPDMode) {
    match(t_psa_list[0], t_nhit_list[0], t_psa_list[2], t_nhit_list[2]);
    apd_gain_index = 0;

    if (overflow()) {
      match(t_psa_list[1], t_nhit_list[1], t_psa_list[3], t_nhit_list[3]);
      apd_gain_index = 1;
    }
  } else {
    fill(t_psa_list[0], t_nhit_list[0]);
    apd_gain_index = 0;

    if (overflow()) { // if overflow, use lg
      fill(t_psa_list[1], t_nhit_list[1]);
      apd_gain_index = 1;
    }
  }

  t_energy_list = fEnergyList;
  t_time_list = fTimeList;
  return apd_gain_index;
}

UInt_t BSEmcPSAOverflowCombinator::match(BSEmcAbsPSA *t_psa1, Int_t t_nhit1, BSEmcAbsPSA *t_psa2, Int_t t_nhit2)
{
  fEnergyList.clear();
  fTimeList.clear();

  Double_t e1 = NAN, e2 = NAN, t1 = NAN, t2 = NAN;
  Int_t psa2_min_idx = 0;
  for (Int_t i = 0; i < t_nhit1; i++) {
    t_psa1->GetHit(i, e1, t1);
    for (Int_t j = psa2_min_idx; j < t_nhit2; j++) { // assume times are sorted in psa1 & psa2
      t_psa2->GetHit(j, e2, t2);
      if (fabs(t1 - t2) < fTimeDiff) {
        // fEnergyList.push_back(e1);
        fEnergyList.push_back((e1 + e2) / 2.);
        fTimeList.push_back((t1 + t2) / 2.);
        psa2_min_idx = j + 1;
        break;
      }
    }
  }
  return fEnergyList.size();
}
void BSEmcPSAOverflowCombinator::fill(BSEmcAbsPSA *t_psa, Int_t t_nhit)
{
  fEnergyList.clear();
  fTimeList.clear();
  for (Int_t i = 0; i < t_nhit; i++) { // hg
    Double_t e = NAN, t = NAN;
    t_psa->GetHit(i, e, t);
    fEnergyList.push_back(e);
    fTimeList.push_back(t);
  }
}

Bool_t BSEmcPSAOverflowCombinator::overflow()
{
  std::vector<Double_t>::iterator it;
  for (it = fEnergyList.begin(); it != fEnergyList.end(); it++) {
    if ((*it) > fThreshold) {
      return kTRUE;
    }
  }
  return kFALSE;
}

ClassImp(BSEmcPSAOverflowCombinator)
