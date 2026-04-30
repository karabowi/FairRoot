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
#include "BSEmcShashlykTimebasedWaveforms.h"

#include "FairRun.h"

#include "BSEmcWaveform.h"
#include "BSEmcWaveformData.h"
#include "BSEmcWaveformBuffer.h"
#include "BSEmcAbsWaveformModifier.h"
#include "BSEmcFullStackedWaveformSimulator.h"
#include "BSEmcMultiWaveformSimulator.h"
#include "BSEmcBwEndcapPulseAmplifier.h" // same to the bwec
#include "BSEmcShashlykNoiseAdder.h"
#include "BSEmcBwEndcapDigitizer.h" // same to the bwec
#include "BSEmcShashlykPulseshape.h"
#include "BSEmcShashlykDigiPar.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLink.h"

#include "TStopwatch.h"
#include "TROOT.h"
#include "TRandom.h"
#include "TClonesArray.h"

#include <iostream>
#include <fstream>
#include <cassert>

BSEmcShashlykTimebasedWaveforms::BSEmcShashlykTimebasedWaveforms(const std::string &t_detectorname, Bool_t t_storewaves)
  : BSEmcMCDepositToTimebasedWaveforms<BSEmcShashlykDigiPar>(t_detectorname, t_storewaves)
{
  SetPersistency(t_storewaves);
}

//--------------
// Destructor --
//--------------
BSEmcShashlykTimebasedWaveforms::~BSEmcShashlykTimebasedWaveforms() {}

/**
 * @brief Setup Simulator for Shashlyk waveform generation
 *
 */
void BSEmcShashlykTimebasedWaveforms::SetupSimulator()
{
    const Double_t tBefore = fDigiPar->GetTimeBeforeHit();     //ns  //250
    const Double_t tAfter = fDigiPar->GetTimeAfterHit();       //ns  //250
    const Double_t cutoff = fDigiPar->GetWfCutOffEnergy();     //GeV  //0.001
    const Double_t activeTimeIncrement = fDigiPar->GetActiveTimeIncrement(); //ns   //10
    const Double_t sampleRate = fDigiPar->GetSampleRate();     //ns^-1 <-> GHz    //0.08
    const Int_t bits = fDigiPar->GetNBits();                   //1    //14
    const Double_t mu = fDigiPar->GetPulseshapeMu()-TMath::Log(sampleRate);  //?    //1.4752 convert from samples to ns
    const Double_t sigma = fDigiPar->GetPulseshapeSigma();     //?    //0.6109
    const Double_t noise_width = fDigiPar->GetNoiseWidth();    // ADC      // 1.0
    const Double_t encalib = fDigiPar->GetEnCalib();           // MeV/ADC  // 0.77
    //fSamplingFactor = fDigiPar->GetSamplingFactor();           //1    //2.524

    BSEmcAbsPulseshape *fPulseshape = new BSEmcShashlykPulseshape(mu, sigma);

    BSEmcFullStackedWaveformSimulator *simulator = new BSEmcFullStackedWaveformSimulator(sampleRate, fPulseshape);
    simulator->Init(tBefore, tAfter, cutoff, activeTimeIncrement);
    // convert waveform amplitude from energy to adc channel
    simulator->AddModifier(new BSEmcBwEndcapPulseAmplifier(1. / encalib * 1e3, 1.));

    // add noise
    simulator->AddModifier(new BSEmcShashlykNoiseAdder(noise_width, time(NULL)));

    // digitize
    simulator->AddModifier(new BSEmcBwEndcapDigitizer(1., bits));

    fSimulator = simulator;
}

ClassImp(BSEmcShashlykTimebasedWaveforms)
