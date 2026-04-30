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

#include "BSEmcMultiPSA.h"

#include "fairlogger/Logger.h"

#include "BSEmcMultiWaveform.h"
#include "BSEmcPSACombinator.h"
#include "BSEmcWaveform.h"

BSEmcMultiPSA::BSEmcMultiPSA(BSEmcPSACombinator *t_combinator) : BSEmcAbsPSA(), fCombinator(t_combinator) {}

void BSEmcMultiPSA::AddPSA(BSEmcAbsPSA *t_psa)
{
  fPSA.push_back(t_psa);
  fNhit.push_back(0);
}

BSEmcMultiPSA::~BSEmcMultiPSA()
{
  fPSA.clear();
  fNhit.clear();
}

void BSEmcMultiPSA::Reset()
{
  std::vector<BSEmcAbsPSA *>::iterator it;
  for (it = fPSA.begin(); it != fPSA.end(); it++) {
    (*it)->Reset();
  }
  fEnergyList.clear();
  fTimeList.clear();
}

Int_t BSEmcMultiPSA::Process(const BSEmcWaveform *t_waveform)
{

  Reset();

  BSEmcMultiWaveform *multiWf = const_cast<BSEmcMultiWaveform *>(dynamic_cast<const BSEmcMultiWaveform *>(t_waveform));
  if (multiWf == nullptr) {
    LOG(error) << "passed waveform is not of type BSEmcMultiwaveform";
    return -1;
  }

  UInt_t npsa = fPSA.size();
  if (npsa > (UInt_t)multiWf->GetNumberOfWaveforms()) {
    LOG(error) << "number of psa is greater than number of waveforms";
    return -1;
  }

  for (UInt_t i = 0; i < npsa; i++) {
    multiWf->SetActiveWaveform(i);
    fNhit[i] = fPSA[i]->Process(multiWf);
  }

  fAPDGainIndex = fCombinator->Combine(fPSA, fNhit, fEnergyList, fTimeList);
  return (Int_t)fEnergyList.size();
}

void BSEmcMultiPSA::GetHit(Int_t t_idx, Double_t &t_energy, Double_t &t_time)
{
  t_energy = fEnergyList[t_idx];
  t_time = fTimeList[t_idx];
}
