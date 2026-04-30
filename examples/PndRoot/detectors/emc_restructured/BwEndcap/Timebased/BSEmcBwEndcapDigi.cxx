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

#include "BSEmcBwEndcapDigi.h"

#include <stdlib.h>

#include "TArrayD.h"

#include "fairlogger/Logger.h"

#include "BSEmcAbsPSA.h"
#include "BSEmcBwEndcapDigiPar.h"
#include "BSEmcMultiPSA.h"
#include "BSEmcPSAOverflowCombinator.h"
#include "BSEmcPSATmaxAnalyser.h"

class BSEmcWaveform;

BSEmcBwEndcapDigi::BSEmcBwEndcapDigi(const std::string &t_detectorname, Bool_t t_storedigis)
  : BSEmcExtractDigisFromWaveforms<BSEmcBwEndcapDigiPar>(t_detectorname, t_storedigis), fCombinator(nullptr), fHighgainPSA(nullptr), fLowgainPSA(nullptr)
{
  fSingleAPDMode = kFALSE;
}

//--------------
// Destructor --
//--------------
BSEmcBwEndcapDigi::~BSEmcBwEndcapDigi()
{
  if (fCombinator != nullptr) {
    delete fCombinator;
  }
  if (fHighgainPSA != nullptr) {
    delete fHighgainPSA;
  }
  if (fLowgainPSA != nullptr) {
    delete fLowgainPSA;
  }
}

/**
 * @brief Define PSA for detector
 *
 * @return
 */
void BSEmcBwEndcapDigi::DefinePSA()
{
  /* psa */
  // backward
  Double_t sample_rate = fDigiPar->GetSampleRate();
  const Double_t *fir_coeff = fDigiPar->GetFIRCoeff().GetArray();
  Int_t taps = fDigiPar->GetTmaxTaps();
  Int_t gap = fDigiPar->GetTmaxGap();
  Double_t hit_thr_hi = fDigiPar->GetTmaxHitThresholdHigh();
  Double_t hit_thr_lo = fDigiPar->GetTmaxHitThresholdLow();
  Double_t tut_peak = fDigiPar->GetTmaxTutPeak();
  Double_t hit_val = fDigiPar->GetTmaxHitVal();
  Double_t sig_overflow = fDigiPar->GetSignalOverflowHigh();
  Double_t sig_timediff = fDigiPar->GetPulseshapeTau() / sample_rate / 3.;

  if (fCombinator == nullptr) {
    if (!fSingleAPDMode) {
      fCombinator = new BSEmcPSAOverflowCombinator(sig_overflow, sig_timediff);
    } else {
      fCombinator = new BSEmcPSAOverflowCombinator(sig_overflow, sig_timediff, kTRUE); // single APD
    }
  }
  if (fPSA == nullptr) {
    fPSA = new BSEmcMultiPSA(fCombinator);
  }
  if (fHighgainPSA == nullptr) {
    fHighgainPSA = new BSEmcPSATmaxAnalyser(fir_coeff, taps, gap, sample_rate, hit_thr_hi, tut_peak, hit_val, fVerbose);
  }
  if (fLowgainPSA == nullptr) {
    fLowgainPSA = new BSEmcPSATmaxAnalyser(fir_coeff, taps, gap, sample_rate, hit_thr_lo, tut_peak, hit_val, fVerbose);
  }
  if (fHighgainPSA != nullptr && fLowgainPSA != nullptr) {
    if (!fSingleAPDMode) {
      dynamic_cast<BSEmcMultiPSA *>(fPSA)->AddPSA(fHighgainPSA);
      dynamic_cast<BSEmcMultiPSA *>(fPSA)->AddPSA(fLowgainPSA);
      dynamic_cast<BSEmcMultiPSA *>(fPSA)->AddPSA(fHighgainPSA);
      dynamic_cast<BSEmcMultiPSA *>(fPSA)->AddPSA(fLowgainPSA);
    } else { // single APD
      dynamic_cast<BSEmcMultiPSA *>(fPSA)->AddPSA(fHighgainPSA);
      dynamic_cast<BSEmcMultiPSA *>(fPSA)->AddPSA(fLowgainPSA);
    }
  } else {
    LOG(error) << "BSEmcBwEndcapDigi::DefinePSA() No highgain and/or lowgain psa";
    exit(-1);
  }

  LOG(info) << "BSEmcBwEndcapDigi: Intialization successfull";
}

BSEmcDigi::eGAIN BSEmcBwEndcapDigi::GetGainType(BSEmcWaveform * /*t_waveform*/, Int_t t_hit) const
{
  Int_t gainType = dynamic_cast<BSEmcMultiPSA *>(fPSA)->GetAPDGainIndex();
  BSEmcDigi::eGAIN type = BSEmcDigi::eGAIN::kNONE;
  switch (gainType) {
  case 0:
    type = BSEmcDigi::eGAIN::kHIGH;
    LOG(trace) << "BSEmcBwEndcapDigi GetGainType(hit: " << t_hit << ") is kHIGH";
    break;
  case 1:
    type = BSEmcDigi::eGAIN::kLOW;
    LOG(trace) << "BSEmcBwEndcapDigi GetGainType(hit: " << t_hit << ") is kLOW";
    break;
  default: LOG(trace) << "BSEmcBwEndcapDigi GetGainType(hit: " << t_hit << ") is kNONE"; break;
  }
  return type;
}
ClassImp(BSEmcBwEndcapDigi)
