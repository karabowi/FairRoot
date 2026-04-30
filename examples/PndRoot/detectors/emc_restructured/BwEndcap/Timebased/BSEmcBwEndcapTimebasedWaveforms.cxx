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

//----------------------------------------------------------------------
// Author List:
//     Guang Zhao
//----------------------------------------------------------------------
//
//
#include "BSEmcBwEndcapTimebasedWaveforms.h"

#include <PndPersistencyTask.h>
#include <cassert>
#include <fstream>
#include <iostream>

#include "TClonesArray.h"
#include "TROOT.h"
#include "TRandom.h"
#include "TStopwatch.h"

#include "FairLink.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "BSEmcAbsWaveformModifier.h"
#include "BSEmcBwEndcapDigiPar.h"
#include "BSEmcBwEndcapDigitizer.h"
#include "BSEmcBwEndcapNoiseAdder.h"
#include "BSEmcBwEndcapPulseAmplifier.h"
#include "BSEmcBwPulseshape.h"
#include "BSEmcFullStackedWaveformSimulator.h"
#include "BSEmcMCDeposit.h"
#include "BSEmcMultiWaveformSimulator.h"
#include "BSEmcWaveform.h"
#include "BSEmcWaveformBuffer.h"
#include "BSEmcWaveformData.h"

BSEmcBwEndcapTimebasedWaveforms::BSEmcBwEndcapTimebasedWaveforms(const std::string &t_detectorname, Bool_t t_storewaves)
  : BSEmcMCDepositToTimebasedWaveforms<BSEmcBwEndcapDigiPar>(t_detectorname, t_storewaves)
{
  fSingleAPDMode = kFALSE;
  fNoiseMode = 1;
  fTestMode = kFALSE;
}

//--------------
// Destructor --
//--------------
BSEmcBwEndcapTimebasedWaveforms::~BSEmcBwEndcapTimebasedWaveforms() {}

/**
 * @brief Init Task
 *
 * Prepares the TClonesArray of BSEmcHit for reading and BSEmcMultiWaveform for writing.
 * Also reads the EMC parameters and prepares the waveform simulator (BSEmcMultiWaveformSimulator).
 *
 * @return InitStatus
 * @retval kSUCCESS success
 */
void BSEmcBwEndcapTimebasedWaveforms::SetupSimulator()
{

  const Double_t tBefore = fDigiPar->GetTimeBeforeHit();                   // ns  //250
  const Double_t tAfter = fDigiPar->GetTimeAfterHit();                     // ns  //250
  const Double_t cutoff = fDigiPar->GetWfCutOffEnergy();                   // GeV  //0.001
  const Double_t activeTimeIncrement = fDigiPar->GetActiveTimeIncrement(); // ns   //10
  const Double_t sampleRate = fDigiPar->GetSampleRate();                   // ns^-1 <-> GHz    //0.08
  const Int_t bits = fDigiPar->GetNBits();                                 // 1    //14
  const Double_t tau = fDigiPar->GetPulseshapeTau() / sampleRate;          // 1->ns   //22->22*12.5
  const Double_t N = fDigiPar->GetPulseshapeN();                           // N    //1.667
  const Double_t *ps_freq = fDigiPar->GetPSFreq().GetArray();
  const Double_t *ps_power_HIGH = fDigiPar->GetPSPowerHigh().GetArray();
  const Double_t *ps_power_LOW = fDigiPar->GetPSPowerLow().GetArray();
  Int_t ps_size = fDigiPar->GetPSFreq().GetSize();             // 1
  const Double_t hglg_ratio = fDigiPar->GetHGLGRatio();        // 1      //10.5
  const Double_t encalib = fDigiPar->GetEnCalib();             // 0.3 MeV/channel
  const Double_t adc_noise_HIGH = fDigiPar->GetADCNoiseHigh(); // ADC    //3.5
  const Double_t adc_noise_LOW = fDigiPar->GetADCNoiseLow();   // ADC    //3.5
  const Double_t fe_noise_HIGH = fDigiPar->GetFENoiseHigh();   // ADC    //1.89
  const Double_t fe_noise_LOW = fDigiPar->GetFENoiseLow();     // ADC    //1.89
  const Double_t ps_sigma_HIGH = fDigiPar->GetPsSigmaHigh();   // ADC    //0.21
  const Double_t ps_sigma_LOW = fDigiPar->GetPsSigmaLow();     // ADC    //0.038
  const Double_t ges_sigma_HIGH = fDigiPar->GetGesSigmaHigh(); // ADC    //36.69
  const Double_t ges_sigma_LOW = fDigiPar->GetGesSigmaLow();   // ADC    //7.36

  BSEmcAbsPulseshape *fPulseshape = new BSEmcBwPulseshape(tau, N);
  Int_t seed = time(nullptr);
  BSEmcBwEndcapNoiseAdder *na =
    new BSEmcBwEndcapNoiseAdder(fNoiseMode, sampleRate, ps_freq, ps_power_HIGH, ps_size, adc_noise_HIGH, fe_noise_HIGH, ps_sigma_HIGH, ges_sigma_HIGH, seed, fVerbose);
  BSEmcBwEndcapNoiseAdder *na_lo =
    new BSEmcBwEndcapNoiseAdder(fNoiseMode, sampleRate, ps_freq, ps_power_LOW, ps_size, adc_noise_LOW, fe_noise_LOW, ps_sigma_LOW, ges_sigma_LOW, seed, fVerbose);
  BSEmcMultiWaveformSimulator *simulator = nullptr;
  if (!fSingleAPDMode) {
    LOG(info) << "BSEmcBwEndcapTimebasedWaveforms using Multi APD Mode";
    simulator = new BSEmcMultiWaveformSimulator(sampleRate, fPulseshape, 4);
    simulator->Init(tBefore, tAfter, cutoff, activeTimeIncrement);

    // convert waveform amplitude from energy to adc channel
    simulator->AddModifier(new BSEmcBwEndcapPulseAmplifier(1e3 / encalib * hglg_ratio, simulator->GetTotalScale(0)), 0);
    simulator->AddModifier(new BSEmcBwEndcapPulseAmplifier(1e3 / encalib, simulator->GetTotalScale(1)), 1);
    simulator->AddModifier(new BSEmcBwEndcapPulseAmplifier(1e3 / encalib * hglg_ratio, simulator->GetTotalScale(2)), 2);
    simulator->AddModifier(new BSEmcBwEndcapPulseAmplifier(1e3 / encalib, simulator->GetTotalScale(3)), 3);

    // add noise
    simulator->AddModifier(na, 0);
    simulator->AddModifier(na_lo, 1);
    simulator->AddModifier(na, 2);
    simulator->AddModifier(na_lo, 3);

    // digitize
    simulator->AddModifier(new BSEmcBwEndcapDigitizer(simulator->GetTotalScale(0), bits), 0);
    simulator->AddModifier(new BSEmcBwEndcapDigitizer(simulator->GetTotalScale(1), bits), 1);
    simulator->AddModifier(new BSEmcBwEndcapDigitizer(simulator->GetTotalScale(2), bits), 2);
    simulator->AddModifier(new BSEmcBwEndcapDigitizer(simulator->GetTotalScale(3), bits), 3);
  } else {
    LOG(info) << "BSEmcBwEndcapTimebasedWaveforms using Single APD Mode";
    simulator = new BSEmcMultiWaveformSimulator(sampleRate, fPulseshape, 2);
    simulator->Init(tBefore, tAfter, cutoff, activeTimeIncrement);

    // convert waveform amplitude from energy to adc channel
    simulator->AddModifier(new BSEmcBwEndcapPulseAmplifier(1e3 / encalib * hglg_ratio, simulator->GetTotalScale(0)), 0);
    simulator->AddModifier(new BSEmcBwEndcapPulseAmplifier(1e3 / encalib, simulator->GetTotalScale(1)), 1);

    // add noise
    simulator->AddModifier(na, 0);
    simulator->AddModifier(na_lo, 1);

    // digitize
    simulator->AddModifier(new BSEmcBwEndcapDigitizer(simulator->GetTotalScale(0), bits), 0);
    simulator->AddModifier(new BSEmcBwEndcapDigitizer(simulator->GetTotalScale(1), bits), 1);
  }
  fSimulator = simulator;
}

ClassImp(BSEmcBwEndcapTimebasedWaveforms)
