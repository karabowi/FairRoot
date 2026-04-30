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
// 	Philipp Mahlberg
//----------------------------------------------------------------------
//
//
#include "BSEmcFwEndcapTimebasedWaveforms.h"

#include "FairRun.h"

#include "BSEmcAbsWaveformModifier.h"
#include "BSEmcBaselineAdder.h"
#include "BSEmcFittedPulseshape.h"
#include "BSEmcFittedPulseshape_nw.h"
#include "BSEmcFullStackedWaveformSimulator.h"
#include "BSEmcFwEndcapWaveformGenPar.h"
#include "BSEmcMultiWaveformSimulator.h"
#include "BSEmcShapingNoiseAdder.h"
#include "BSEmcWaveformClipper.h"
#include "BSEmcWaveformDigitizer.h"

BSEmcFwEndcapTimebasedWaveforms::BSEmcFwEndcapTimebasedWaveforms(Bool_t t_storewaves) : BSEmcMCDepositToTimebasedWaveforms<BSEmcFwEndcapWaveformGenPar>("FwEndcap", t_storewaves)
{
  this->SetPhotonStatisticParName(std::string{BSEmcDigiPar::fgParameterName + fDetectorName.Data() + "APD"});
}
//--------------
// Destructor --
//--------------
BSEmcFwEndcapTimebasedWaveforms::~BSEmcFwEndcapTimebasedWaveforms() {}

void BSEmcFwEndcapTimebasedWaveforms::SetupSimulator()
{
  const Double_t tBefore = fDigiPar->GetTimeBeforeHit();
  const Double_t tAfter = fDigiPar->GetTimeAfterHit();
  const Double_t cutoff = fDigiPar->GetWfCutOffEnergy(); // GeV 		//0.001
  const Double_t activeTimeIncrement = 10;               // ns		//50
  const Double_t sampleRate = fDigiPar->GetSampleRate(); // ns^-1 <-> GHz	//0.08
  const Int_t bits = fDigiPar->GetNBits();               // 1		//14

  const Double_t tau = fDigiPar->GetPulseshapeTau(); // ns		//68.7
  const Double_t N = fDigiPar->GetPulseshapeN();     // N		//1.667
  const Double_t v1 = fDigiPar->GetPulseshapev1();   // = 0;
  const Double_t tcr = fDigiPar->GetPulseshapetcr(); // = 26.3995;//2.11196*12.5;
  const Double_t tcf = fDigiPar->GetPulseshapetcf(); // = 302.76625;//24.2213*12.5;
  const Double_t t_r = fDigiPar->GetPulseshapet_r(); // = 52.69625;//4.2157*12.5;
  const Double_t res = fDigiPar->GetPulseshapeRes(); // = 0;

  const Double_t energyRange_HIGH = fDigiPar->GetEnergyRangeHigh(); // GeV		//1
  const Double_t energyRange_LOW = fDigiPar->GetEnergyRangeLow();   // GeV		//15
  const Double_t noiseWidth_HIGH = fDigiPar->GetNoiseWidthHigh();   // GeV		//0.0023
  const Double_t noiseWidth_LOW = fDigiPar->GetNoiseWidthLow();     // GeV		//0.0035

  const Double_t totalChannels = (Int_t)(TMath::Power(2, bits) + 0.5);
  const Double_t baseline_HIGH = fDigiPar->GetBaselineMeanHigh();
  const Double_t baseline_LOW = fDigiPar->GetBaselineMeanLow();
  const Double_t baselineDistributionWidth_HIGH = fDigiPar->GetBaselineSigmaHigh();
  const Double_t baselineDistributionWidth_LOW = fDigiPar->GetBaselineSigmaLow();
  const Double_t bitresolution_HIGH = energyRange_HIGH / (totalChannels - baseline_HIGH);
  const Double_t bitresolution_LOW = energyRange_LOW / (totalChannels - baseline_LOW);

  BSEmcAbsPulseshape *pulseshape = new BSEmcFittedPulseshape_nw(v1, tcr, tcf, t_r, res, tau, N);
  BSEmcMultiWaveformSimulator *simulator = new BSEmcMultiWaveformSimulator(sampleRate, pulseshape, 2);

  simulator->Init(tBefore, tAfter, cutoff, activeTimeIncrement);

  simulator->AddModifier(new BSEmcShapingNoiseAdder(simulator->GetPulseRiseTime(), sampleRate, noiseWidth_HIGH, simulator->GetTotalScale(0)), 0);
  simulator->AddModifier(new BSEmcShapingNoiseAdder(simulator->GetPulseRiseTime(), sampleRate, noiseWidth_LOW, simulator->GetTotalScale(1)), 1);

  simulator->AddModifier(new BSEmcWaveformDigitizer(bitresolution_HIGH, simulator->GetTotalScale(0), totalChannels - baseline_HIGH), 0);
  simulator->AddModifier(new BSEmcWaveformDigitizer(bitresolution_LOW, simulator->GetTotalScale(1), totalChannels - baseline_LOW), 1);

  simulator->AddModifier(new BSEmcBaselineAdder(8185, baselineDistributionWidth_HIGH), 0);
  simulator->AddModifier(new BSEmcBaselineAdder(8190, baselineDistributionWidth_LOW), 1);

  simulator->AddModifier(new BSEmcWaveformClipper(totalChannels), 0);
  simulator->AddModifier(new BSEmcWaveformClipper(totalChannels), 1);
  fSimulator = simulator;
}

ClassImp(BSEmcFwEndcapTimebasedWaveforms)
