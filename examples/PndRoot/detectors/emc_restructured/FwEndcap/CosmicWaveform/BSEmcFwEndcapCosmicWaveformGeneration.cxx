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

#include "BSEmcFwEndcapCosmicWaveformGeneration.h"

#include <vector>

#include "RtypesCore.h"
#include "TMath.h"

#include "FairParSet.h"
#include "fairlogger/Logger.h"

#include "PndParameterRegister.h"

#include "BSEmcBaselineAdder.h"
#include "BSEmcLUTBasedPulseshape.h"
#include "BSEmcDigiPar.h"
#include "BSEmcFwEndcapCosmicWaveformGenPar.h"
#include "BSEmcMultiWaveformSimulator.h"
#include "BSEmcSamplingNoiseAdder.h"
#include "BSEmcWaveformClipper.h"
#include "BSEmcWaveformDigitizer.h"

class BSEmcAbsPulseshape;

BSEmcFwEndcapCosmicWaveformGeneration::BSEmcFwEndcapCosmicWaveformGeneration() : BSEmcWaveformGenerationProcess() {}

BSEmcFwEndcapCosmicWaveformGeneration::~BSEmcFwEndcapCosmicWaveformGeneration() {}

void BSEmcFwEndcapCosmicWaveformGeneration::SetDetectorName(const std::string &t_detectorName)
{
  BSEmcWaveformGenerationProcess::SetDetectorName(t_detectorName);
  fParameterList.clear();
  if (fPhotonStatisticParName == "") {
    fPhotonStatisticParName = BSEmcDigiPar::fgParameterName + "FwEndcapAPD";
  }
  if (fWaveformGenerationParName == "") {
    fWaveformGenerationParName = BSEmcFwEndcapCosmicWaveformGenPar::fgParameterName + "APD";
  }
  fParameterList.push_back(fPhotonStatisticParName);
  fParameterList.push_back(fWaveformGenerationParName);
  LOG(info) << "BSEmcFwEndcapCosmicWaveformGeneration::SetDetectorName Adding " << fWaveformGenerationParName << " to request list";
}

void BSEmcFwEndcapCosmicWaveformGeneration::SetupParameters(const PndParameterRegister *t_parameterRegister) /*override*/
{
  BSEmcWaveformGenerationProcess::SetupParameters(t_parameterRegister);
  fWaveformGenerationPar = dynamic_cast<BSEmcFwEndcapCosmicWaveformGenPar *>(t_parameterRegister->GetParameter(fWaveformGenerationParName.c_str()));
  SetupSimulator();
}

void BSEmcFwEndcapCosmicWaveformGeneration::SetupSimulator()
{
  const Double_t sampleRate = fWaveformGenerationPar->GetSampleRate(); // ns^-1 <-> GHz	//0.08
  const Int_t bits = fWaveformGenerationPar->GetNBits();               // 1		//14

  const Double_t energyRange_HIGH = fWaveformGenerationPar->GetEnergyRangeHigh(); // GeV		//1
  const Double_t energyRange_LOW = fWaveformGenerationPar->GetEnergyRangeLow();   // GeV		//15
  const Double_t noiseWidth_HIGH = fWaveformGenerationPar->GetNoiseWidthHigh();   // GeV		//0.0023
  const Double_t noiseWidth_LOW = fWaveformGenerationPar->GetNoiseWidthLow();     // GeV		//0.0035
  const Double_t totalChannels = (Int_t)(TMath::Power(2, bits) + 0.5);
  const Double_t usableChannels = totalChannels - (Int_t)(TMath::Power(2, bits - 1) + 0.5);

  const Double_t baseline_HIGH = fWaveformGenerationPar->GetBaselineMeanHigh();
  const Double_t baseline_LOW = fWaveformGenerationPar->GetBaselineMeanLow();
  const Double_t baselineDistributionWidth_HIGH = fWaveformGenerationPar->GetBaselineSigmaHigh();
  const Double_t baselineDistributionWidth_LOW = fWaveformGenerationPar->GetBaselineSigmaLow();
  const Double_t bitresolution_HIGH = energyRange_HIGH / (usableChannels);
  const Double_t bitresolution_LOW = energyRange_LOW / (usableChannels);

  const Double_t tBefore = fWaveformGenerationPar->GetTimeBeforeHit();
  const Double_t tAfter = fWaveformGenerationPar->GetTimeAfterHit();
  const Double_t cutoff = fWaveformGenerationPar->GetWfCutOffEnergy(); // GeV //0.001
  const Double_t activeTimeIncrement = 10; // ns //50

  BSEmcLUTBasedPulseshape *pulseshape = new BSEmcLUTBasedPulseshape();
  TString filename = gSystem->Getenv("VMCWORKDIR");
  filename += "/input/" + fPulseshapeLUT;
  pulseshape->SetLUTFile(filename.Data());
  BSEmcMultiWaveformSimulator *simulator = new BSEmcMultiWaveformSimulator(sampleRate, pulseshape, 2);

  simulator->Init(tBefore, tAfter, cutoff, activeTimeIncrement);

  simulator->AddModifier(new BSEmcSamplingNoiseAdder(noiseWidth_HIGH*0.0001), 0);
  simulator->AddModifier(new BSEmcSamplingNoiseAdder(noiseWidth_LOW*0.0001), 1);

  simulator->AddModifier(new BSEmcWaveformDigitizer(bitresolution_HIGH, 1, usableChannels), 0); // digitization process
  simulator->AddModifier(new BSEmcWaveformDigitizer(bitresolution_LOW, 1, usableChannels), 1);

  simulator->AddModifier(new BSEmcBaselineAdder(baseline_HIGH, baselineDistributionWidth_HIGH), 0);
  simulator->AddModifier(new BSEmcBaselineAdder(baseline_LOW, baselineDistributionWidth_LOW), 1);

  simulator->AddModifier(new BSEmcWaveformClipper(totalChannels), 0);
  simulator->AddModifier(new BSEmcWaveformClipper(totalChannels), 1);

  SetSimulator(simulator);
}
