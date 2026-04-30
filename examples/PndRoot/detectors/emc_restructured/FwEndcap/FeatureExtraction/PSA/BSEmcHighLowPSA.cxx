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

#include "BSEmcHighLowPSA.h"

#include <memory>
#include <set>

#include "fairlogger/Logger.h"

#include "BSEmcMultiWaveform.h"
#include "BSEmcPSAFPGASampleAnalyser.h"
#include "BSEmcWaveform.h"

BSEmcHighLowPSA::BSEmcHighLowPSA(Int_t t_verbose) : fHighgainPSA(nullptr), fLowgainPSA(nullptr), fVerbose(t_verbose){};

void BSEmcHighLowPSA::Init(BSEmcPSAFPGASampleAnalyser *t_highgainPSA, BSEmcPSAFPGASampleAnalyser *t_lowgainPSA, Double_t t_overflowThreshold, Int_t t_highgainWfIndex,
                           Int_t t_lowgainWfIndex)
{
  fHighgainPSA = t_highgainPSA;
  fLowgainPSA = t_lowgainPSA;
  fIdx_high = t_highgainWfIndex;
  fIdx_low = t_lowgainWfIndex;
  fOverflowThreshold = t_overflowThreshold;
}

void BSEmcHighLowPSA::Reset()
{
  // fActivePSA=0;
  fHitsInFE.clear();
  fHighgainPSA->Reset();
  fLowgainPSA->Reset();
}

Int_t BSEmcHighLowPSA::Process(const BSEmcWaveform *t_waveform)
{

  Reset();

  BSEmcMultiWaveform *multiWf = const_cast<BSEmcMultiWaveform *>(dynamic_cast<const BSEmcMultiWaveform *>(t_waveform));

  if (multiWf == nullptr) {
    LOG(error) << "passed waveform is not of type BSEmcMultiwaveform";
    return -1;
  }

  // active method

  Int_t activeWf = multiWf->GetActiveWaveform();

  multiWf->SetActiveWaveform(fIdx_low);
  std::vector<Double_t> signal_low = t_waveform->GetSignal();
  std::vector<Double_t>::iterator it_low = signal_low.begin();

  multiWf->SetActiveWaveform(fIdx_high);
  std::vector<Double_t> signal_high = t_waveform->GetSignal();
  std::vector<Double_t>::iterator it_high = signal_high.begin();

  multiWf->SetActiveWaveform(activeWf);

  Bool_t active_high = kFALSE;
  Bool_t active_low = kFALSE;
  Int_t NofHithigh = 0;
  Int_t NofHitlow = 0;
  Int_t counter_low = 0;
  Int_t counter_high = 0;

  BSEmcPSAFPGASampleAnalyser::status_t status_low = BSEmcPSAFPGASampleAnalyser::status_t::kUndefined;
  BSEmcPSAFPGASampleAnalyser::status_t status_high = BSEmcPSAFPGASampleAnalyser::status_t::kUndefined;

  std::set<Int_t> hits_lowgain;
  Bool_t overflow = kFALSE;

  while (it_low != signal_low.end() && it_high != signal_high.end()) {

    fHighgainPSA->put(*it_high);
    fLowgainPSA->put(*it_low);
    status_high = fHighgainPSA->GetStatus();
    status_low = fLowgainPSA->GetStatus();
    NofHithigh = fHighgainPSA->nHits();
    NofHitlow = fLowgainPSA->nHits();

    if (status_high == BSEmcPSAFPGASampleAnalyser::kPulseFinished) {
      active_high = kFALSE;
      if (!overflow) {
        for (Int_t i = counter_high; i < NofHithigh; i++) {
          fHitsInFE.push_back(std::make_pair(fHighgainPSA, i));
          LOG_IF(info, fVerbose >= 2) << "PndEmcHighLowPSA: adding highgain hit: #:" << i;
        }
      }
      counter_high = NofHithigh;
    } else if (status_high == BSEmcPSAFPGASampleAnalyser::kPileupFinished) {
      active_high = kFALSE;
      if (!overflow) {
        fHitsInFE.push_back(std::make_pair(fHighgainPSA, counter_high));
        fHitsInFE.push_back(std::make_pair(fHighgainPSA, counter_high + 1));
        if (fVerbose >= 2) {
          LOG(info) << "BSEmcHighLowPSA: adding highgain hits #:" << counter_high << ","
                    << counter_high + 1; // FIXME: Are you sure, you want to change the counter_high for debug code? I do not think so.
          // counter_high++; // [R.K. 9/2018] made implicit operation explicit
          // LOG(info) << counter_high;
        }
      }
      counter_high += 2;
    } else if (status_high == BSEmcPSAFPGASampleAnalyser::kPulseDetected) {
      active_high = kTRUE;
    }

    if (active_high && ((*it_high) > fOverflowThreshold) && !overflow) {
      if (fVerbose >= 2) {
        LOG(info) << "BSEmcHighLowPSA: overflow detected";
      }
      overflow = kTRUE;
    }

    if (status_low == BSEmcPSAFPGASampleAnalyser::kPulseFinished) {

      active_low = kFALSE;
      if ((!active_low)) {
        if (overflow) {
          for (int i = counter_low; i < NofHitlow; i++) {
            fHitsInFE.push_back(std::make_pair(fLowgainPSA, i));
            if (fVerbose >= 2) {
              LOG(info) << "PndEmcHighLowPSA: adding lowgain hit: #:" << i;
            }
          }
        }
      }
      counter_low = NofHitlow;
    } else if (status_low == BSEmcPSAFPGASampleAnalyser::kPileupFinished) {
      if (fVerbose >= 3) {
        LOG(info) << "BSEmcHighLowPSA: pileup in lowgain candidate: #" << counter_low << ", #" << counter_low + 1;
      }
      hits_lowgain.insert(counter_low);
      hits_lowgain.insert(counter_low + 1);
      counter_low += 2;
      active_low = kFALSE;
    } else if (status_low == BSEmcPSAFPGASampleAnalyser::kPulseDetected) {
      active_low = kTRUE;
    }
    if ((!active_low) && (!active_high)) {
      overflow = kFALSE;
    }
    /*if ((!active_high) && (!active_low)) {
      if (overflow) {
        if (fVerbose >= 2) {
          LOG(info) << "I- BSEmcHighLowPSA: adding lowgain hit(s) #:";
          for (std::set<Int_t>::iterator it = hits_lowgain.begin(); it != hits_lowgain.end(); ++it) {
            LOG(info) << *it << ",";
          }
          LOG(info) << "\b\n" << std::flush;
        }
        for (std::set<Int_t>::iterator it = hits_lowgain.begin(); it != hits_lowgain.end(); ++it) {
          fHitsInFE.push_back(std::make_pair(fLowgainPSA, *it));
        }
      }
      hits_lowgain.clear();
      overflow = kFALSE;
    }*/
    it_low++;
    it_high++;
  }
  if (fVerbose >= 3) {
    LOG(info) << " fHitsInFE.size() " << fHitsInFE.size();
  }
  return fHitsInFE.size();
}

void BSEmcHighLowPSA::GetHit(Int_t t_idx, Double_t &t_energy, Double_t &t_time)
{

  BSEmcPSAFPGASampleAnalyser *psa = fHitsInFE[t_idx].first;
  if (psa != nullptr) {
    psa->GetHit(fHitsInFE[t_idx].second, t_energy, t_time);
  } else {
    LOG(error) << "BSEmcHighLowPSA::GetHit: No suitable PSA available";
    t_energy = 0;
    t_time = 0;
  }
}

void BSEmcHighLowPSA::GetHit(Int_t t_i, Double_t &t_energy, Double_t &t_time, Int_t &t_PileupType)
{

  BSEmcPSAFPGASampleAnalyser *psa = fHitsInFE[t_i].first;
  if (psa != nullptr) {
    psa->GetHit(fHitsInFE[t_i].second, t_energy, t_time, t_PileupType);
    if (fVerbose >= 3) {
      LOG(info) << "GetHit fHitsInFE[t_i].second " << fHitsInFE[t_i].second;
    }
  } else {
    t_energy = 0;
    t_time = 0;
  }
}

Int_t BSEmcHighLowPSA::GetWaveformIdx(Int_t t_idx)
{

  if (t_idx < (Int_t)fHitsInFE.size() && t_idx >= 0) {
    BSEmcPSAFPGASampleAnalyser *psa = fHitsInFE[t_idx].first;
    if (psa == fHighgainPSA) {
      return fIdx_high;
    } else if (psa == fLowgainPSA) {
      return fIdx_low;
    }
  }

  return -1; // error
}
