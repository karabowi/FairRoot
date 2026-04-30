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

#include "BSEmcFwEndcapWaveformGeneration.h"

#include <vector>

#include "RtypesCore.h"
#include "TMath.h"

#include "FairParSet.h"

#include "PndParameterRegister.h"

#include "BSEmcBaselineAdder.h"
#include "BSEmcDigiPar.h"
#include "BSEmcFittedPulseshape_nw.h"
#include "BSEmcFwEndcapWaveformGenPar.h"
#include "BSEmcMultiWaveformSimulator.h"
#include "BSEmcShapingNoiseAdder.h"
#include "BSEmcWaveformClipper.h"
#include "BSEmcWaveformDigitizer.h"

class BSEmcAbsPulseshape;

BSEmcFwEndcapWaveformGeneration::BSEmcFwEndcapWaveformGeneration() : BSEmcWaveformGenerationProcess() {}

BSEmcFwEndcapWaveformGeneration::~BSEmcFwEndcapWaveformGeneration() {}

void BSEmcFwEndcapWaveformGeneration::SetDetectorName(const std::string &t_detectorName)
{
  BSEmcWaveformGenerationProcess::SetDetectorName(t_detectorName);
  fParameterList.clear();
  if (fPhotonStatisticParName == "") {
    fPhotonStatisticParName = BSEmcDigiPar::fgParameterName + "FwEndcapAPD";
  }
  fParameterList.push_back(fPhotonStatisticParName);
  if (fWaveformGenerationParName == "") {
    fWaveformGenerationParName = BSEmcFwEndcapWaveformGenPar::fgParameterName;
  }
  fParameterList.push_back(fPhotonStatisticParName);
  fParameterList.push_back(fWaveformGenerationParName);
}

void BSEmcFwEndcapWaveformGeneration::SetupParameters(const PndParameterRegister *t_parameterRegister) /*override*/
{
  BSEmcWaveformGenerationProcess::SetupParameters(t_parameterRegister);
  fWaveformGenerationPar = dynamic_cast<BSEmcFwEndcapWaveformGenPar *>(t_parameterRegister->GetParameter(fWaveformGenerationParName.c_str()));
  SetupSimulator();
}

void BSEmcFwEndcapWaveformGeneration::SetupSimulator()
{
  const Double_t tBefore = fWaveformGenerationPar->GetTimeBeforeHit();
  const Double_t tAfter = fWaveformGenerationPar->GetTimeAfterHit();
  const Double_t cutoff = fWaveformGenerationPar->GetWfCutOffEnergy(); // GeV 		//0.001
  const Double_t activeTimeIncrement = 10;                             // ns		//50
  const Double_t sampleRate = fWaveformGenerationPar->GetSampleRate(); // ns^-1 <-> GHz	//0.08
  const Int_t bits = fWaveformGenerationPar->GetNBits();               // 1		//14

  const Double_t tau = fWaveformGenerationPar->GetPulseshapeTau(); // ns		//68.7
  const Double_t N = fWaveformGenerationPar->GetPulseshapeN();     // N		//1.667
  const Double_t v1 = fWaveformGenerationPar->GetPulseshapev1();   // = 0;
  const Double_t tcr = fWaveformGenerationPar->GetPulseshapetcr(); // = 26.3995;//2.11196*12.5;
  const Double_t tcf = fWaveformGenerationPar->GetPulseshapetcf(); // = 302.76625;//24.2213*12.5;
  const Double_t t_r = fWaveformGenerationPar->GetPulseshapet_r(); // = 52.69625;//4.2157*12.5;
  const Double_t res = fWaveformGenerationPar->GetPulseshapeRes(); // = 0;

  const Double_t energyRange_HIGH = fWaveformGenerationPar->GetEnergyRangeHigh(); // GeV		//1
  const Double_t energyRange_LOW = fWaveformGenerationPar->GetEnergyRangeLow();   // GeV		//15
  const Double_t noiseWidth_HIGH = fWaveformGenerationPar->GetNoiseWidthHigh();   // GeV		//0.0023
  const Double_t noiseWidth_LOW = fWaveformGenerationPar->GetNoiseWidthLow();     // GeV		//0.0035
  const Double_t totalChannels = (Int_t)(TMath::Power(2, bits) + 0.5);
  const Double_t baseline_HIGH = fWaveformGenerationPar->GetBaselineMeanHigh();
  const Double_t baseline_LOW = fWaveformGenerationPar->GetBaselineMeanLow();
  const Double_t baselineDistributionWidth_HIGH = fWaveformGenerationPar->GetBaselineSigmaHigh();
  const Double_t baselineDistributionWidth_LOW = fWaveformGenerationPar->GetBaselineSigmaLow();
  const Double_t bitresolution_HIGH = energyRange_HIGH / (totalChannels - baseline_HIGH);
  const Double_t bitresolution_LOW = energyRange_LOW / (totalChannels - baseline_LOW);

  BSEmcAbsPulseshape *pulseshape = new BSEmcFittedPulseshape_nw(v1, tcr, tcf, t_r, res, tau, N);
  BSEmcMultiWaveformSimulator *simulator = new BSEmcMultiWaveformSimulator(sampleRate, pulseshape, 2);

  simulator->Init(tBefore, tAfter, cutoff, activeTimeIncrement);

  simulator->AddModifier(new BSEmcShapingNoiseAdder(simulator->GetPulseRiseTime(), sampleRate, noiseWidth_HIGH, simulator->GetTotalScale(0)), 0);
  simulator->AddModifier(new BSEmcShapingNoiseAdder(simulator->GetPulseRiseTime(), sampleRate, noiseWidth_LOW, simulator->GetTotalScale(1)), 1);

  simulator->AddModifier(new BSEmcWaveformDigitizer(bitresolution_HIGH, simulator->GetTotalScale(0), totalChannels - baseline_HIGH), 0);
  simulator->AddModifier(new BSEmcWaveformDigitizer(bitresolution_LOW, simulator->GetTotalScale(1), totalChannels - baseline_LOW), 1);

  simulator->AddModifier(new BSEmcBaselineAdder(baseline_HIGH, baselineDistributionWidth_HIGH), 0);
  simulator->AddModifier(new BSEmcBaselineAdder(baseline_LOW, baselineDistributionWidth_LOW), 1);

  simulator->AddModifier(new BSEmcWaveformClipper(totalChannels), 0);
  simulator->AddModifier(new BSEmcWaveformClipper(totalChannels), 1);

  SetSimulator(simulator);
}
