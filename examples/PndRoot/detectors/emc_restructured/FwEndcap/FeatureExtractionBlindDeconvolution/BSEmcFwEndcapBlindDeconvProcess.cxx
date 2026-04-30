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

#include "BSEmcFwEndcapBlindDeconvProcess.h"

//#include <bits/exception.h>
#include <math.h>
#include <set>

#include "TStopwatch.h"
#include "TString.h"

#include "FairLink.h"
#include "FairMultiLinkedData.h"
#include "FairRootManager.h"
#include "fairlogger/Logger.h"

#include "PndMutableContainerI.h"

#include "BSEmcFwEndcapBlindDeconvPar.h"
#include "BSEmcDataBranchNames.h"
#include "BSEmcDigi.h"
#include "BSEmcMultiWaveform.h"
#include "BSEmcWaveform.h"

BSEmcFwEndcapBlindDeconvProcess::BSEmcFwEndcapBlindDeconvProcess() : PndProcess("BSEmcFwEndcapBlindDeconvProcess") {}

//--------------
// Destructor --
//--------------
BSEmcFwEndcapBlindDeconvProcess::~BSEmcFwEndcapBlindDeconvProcess() {}

/**
 * @brief Sets the detectorname and fills the parameterlist with names of all required parameter
 *
 * @param t_detectorName
 */
void BSEmcFwEndcapBlindDeconvProcess::SetDetectorName(const std::string &t_detectorName)
{
  this->fDetectorName = t_detectorName;
  fMCDepositBranchName = BSEmcDataBranchNames::fgMCDepositBranchName + this->fDetectorName;
  fParameterList.push_back(BSEmcFwEndcapBlindDeconvPar::fgParameterName + "APD");
  fParameterList.push_back(BSEmcFwEndcapBlindDeconvPar::fgParameterName + "VPTT");
}

// -------------------------------------------------------------------------
void BSEmcFwEndcapBlindDeconvProcess::RequestDataContainer(PndContainerRegister *t_register)
{
  t_register->Request({fDigiBranchName, "BSEmcDigi", kTRUE, kTRUE});
  t_register->Request({fWaveformBranchName, "BSEmcMultiWaveform", kFALSE, kFALSE});
}

// -------------------------------------------------------------------------
void BSEmcFwEndcapBlindDeconvProcess::GetDataContainer(PndContainerRegister *t_register)
{
  fWaveformArray = t_register->GetInput<BSEmcMultiWaveform>(fWaveformBranchName);

  fDigiArray = t_register->GetOutput<BSEmcDigi>(fDigiBranchName);
}

/**
 * @brief Fetches the parameteres requested in SetDetectorName
 *
 * @param t_parameterRegister
 */
void BSEmcFwEndcapBlindDeconvProcess::SetupParameters(const PndParameterRegister *t_parameterRegister) /*override*/
{
  LOG(debug) << "BSEmcFwEndcapBlindDeconvProcess::SetupParameters start";
  {
    BSEmcFwEndcapBlindDeconvPar *par =
      dynamic_cast<BSEmcFwEndcapBlindDeconvPar *>(t_parameterRegister->GetParameter(TString{BSEmcFwEndcapBlindDeconvPar::fgParameterName + "APD"}.Data()));
    fBlindDeconvoluterAPD.set(par->GetDeconvolutionFilename());
    fPeakFinderAPD.set(par->GetPeakFinderThreshold(), par->GetPeakFinderLowerLimit(), par->GetPeakFinderUpperLimit());
    fPileUpDetectorAPD.set(par->GetPileUpDetectionYScale(), par->GetPileUpDetectionXOffset(), par->GetPileUpDetectionYOffset());
    fPeakTimeFinderAPD.set();

    fBaselineFinderAPD.set(par->GetBaselineSampleNumber());
  }
  {
    BSEmcFwEndcapBlindDeconvPar *par =
      dynamic_cast<BSEmcFwEndcapBlindDeconvPar *>(t_parameterRegister->GetParameter(TString{BSEmcFwEndcapBlindDeconvPar::fgParameterName + "VPTT"}.Data()));
    fBlindDeconvoluterVPTT.set(par->GetDeconvolutionFilename());
    fPeakFinderVPTT.set(par->GetPeakFinderThreshold(), par->GetPeakFinderLowerLimit(), par->GetPeakFinderUpperLimit());
    fPileUpDetectorVPTT.set(par->GetPileUpDetectionYScale(), par->GetPileUpDetectionXOffset(), par->GetPileUpDetectionYOffset());
    fPeakTimeFinderVPTT.set();

    fBaselineFinderVPTT.set(par->GetBaselineSampleNumber());
  }
  LOG(debug) << "BSEmcFwEndcapBlindDeconvProcess::SetupParameters end";
}

/**
 * @brief Runs the process.
 *
 * Loops over the waveforms and uses the pulse shape analyser (BSEmcAbsPSA) to
 * extract signal height and timing. As energy is not known at this point, every extracted feature is
 * transformed into an BSEmcDigi.
 *
 */
void BSEmcFwEndcapBlindDeconvProcess::Process()
{
  fDigiArray->Reset();
  std::vector<BSEmcDigi *> digis = ExtractDigis(fWaveformArray->GetVectorOfPtrToConst());
  for (BSEmcDigi *digi : digis) {
    fDigiArray->CreateCopy(*digi);
    delete digi;
  }
}

Bool_t BSEmcFwEndcapBlindDeconvProcess::IsVptt(const Int_t t_detId) const
{
  return std::binary_search(fVpttIds.begin(), fVpttIds.end(), t_detId);
}

std::vector<BSEmcPSAFPGAPeakFeatures_t> BSEmcFwEndcapBlindDeconvProcess::ExtractFeatures(const std::vector<Double_t> &t_samples, const Int_t t_detId) const
{
  LOG(debug) << "BSEmcFwEndcapBlindDeconvProcess::ExtractFeatures start";

  TStopwatch timer;
  timer.Start();

  std::vector<BSEmcPSAFPGAPeakFeatures_t> peaks_wd;
  Double_t baseline{0};
  if (IsVptt(t_detId)) {
    baseline = fBaselineFinderVPTT.put(t_samples);
    std::vector<Double_t> nobaseline;
    std::transform(t_samples.begin(), t_samples.end(), std::back_inserter(nobaseline), [baseline](const Double_t sample) { return sample - baseline; });
    const std::vector<Double_t> &wdsignal = fBlindDeconvoluterVPTT.put(nobaseline);
    peaks_wd = fPeakFinderVPTT.put(wdsignal);
    fPileUpDetectorVPTT.put(peaks_wd);
    fPeakTimeFinderVPTT.put(wdsignal, peaks_wd);
  } else {
    baseline = fBaselineFinderAPD.put(t_samples);
    std::vector<Double_t> nobaseline;
    std::transform(t_samples.begin(), t_samples.end(), std::back_inserter(nobaseline), [baseline](const Double_t sample) { return sample - baseline; });
    const std::vector<Double_t> &wdsignal = fBlindDeconvoluterAPD.put(nobaseline);
    peaks_wd = fPeakFinderAPD.put(wdsignal);
    fPileUpDetectorAPD.put(peaks_wd);
    fPeakTimeFinderAPD.put(wdsignal, peaks_wd);
  }
  for (BSEmcPSAFPGAPeakFeatures_t &peak : peaks_wd) {
    peak.Baseline = baseline;
  }

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  LOG(debug) << "BSEmcFwEndcapBlindDeconvProcess, Real time " << rtime << " s, CPU time " << ctime << " s for " << t_samples.size() << " samples";
  LOG(debug) << "BSEmcFwEndcapBlindDeconvProcess::ExtractFeatures end";

  return peaks_wd;
}

/**
 * @brief Runs the process.
 *
 * Loops over the waveforms and uses the pulse shape analyser (BSEmcAbsPSA) to
 * extract signal height and timing. As energy is not known at this point, every extracted feature is
 * transformed into an BSEmcDigi.
 *
 */
std::vector<BSEmcDigi *> BSEmcFwEndcapBlindDeconvProcess::ExtractDigis(const std::vector<const BSEmcMultiWaveform *> &t_waveforms) const
{
  LOG(debug) << "BSEmcFwEndcapBlindDeconvProcess::ExtractDigis start";

  std::vector<BSEmcDigi *> result;
  TStopwatch timer;
  timer.Start();
  for (const BSEmcMultiWaveform *theWaveform : t_waveforms) {
    const Int_t detId = theWaveform->GetDetectorId();

    std::vector<Double_t> samples = theWaveform->GetSignal(0);
    const Bool_t clips = (*std::max_element(samples.begin(), samples.end()) < 8192 * 2 - 1) ? kFALSE : kTRUE;
    if (clips) {
      LOG(debug) << "BSEmcFwEndcapBlindDeconvProcess for " << this->fDetectorName << " waveform for crystal " << detId << " clips";
      samples = theWaveform->GetSignal(1);
    }
    std::vector<BSEmcPSAFPGAPeakFeatures_t> peaks = ExtractFeatures(samples, detId);

    bool isDigitized = kFALSE;
    LOG(debug) << "BSEmcFwEndcapBlindDeconvProcess for " << this->fDetectorName << " waveform for crystal " << detId << " contains " << peaks.size() << " peaks.";
    for (const BSEmcPSAFPGAPeakFeatures_t &peak : peaks) {
      const Double_t energy = peak.MaximumValue;
      const Double_t timestamp = peak.PeakTime;
      BSEmcDigi *myDigi = new BSEmcDigi(detId, energy, timestamp);
      result.push_back(myDigi);
      BSEmcDigi::eGAIN gain = (clips) ? BSEmcDigi::eGAIN::kLOW : BSEmcDigi::eGAIN::kHIGH;
      myDigi->SetGainType(gain);
      myDigi->ResetLinks();
      myDigi->AddLinks(theWaveform->GetLinksWithType(FairRootManager::Instance()->GetBranchId(fMCDepositBranchName)));
      FairMultiLinkedData mcdeplinks = myDigi->GetLinksWithType(FairRootManager::Instance()->GetBranchId(fMCDepositBranchName));
      for (const FairLink &link : mcdeplinks.GetLinks()) {
        LOG(debug) << link;
      }
      LOG(debug) << "BSEmcFwEndcapBlindDeconvProcess for " << this->fDetectorName << " created Digi(detId: " << detId << ", energy: " << energy << ", timestamp: " << timestamp
                 << ") and has " << myDigi->GetLinksWithType(FairRootManager::Instance()->GetBranchId(fMCDepositBranchName)).GetNLinks() << " links to EmcMCDeposits";
      LOG_IF(debug, isDigitized) << "Two digis created for Crystal " << detId;
      isDigitized = kTRUE;
    }
  }

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  LOG(debug) << "BSEmcFwEndcapBlindDeconvProcess, Real time " << rtime << " s, CPU time " << ctime << " s for " << t_waveforms.size() << " waveforms";
  LOG(debug) << "BSEmcFwEndcapBlindDeconvProcess::ExtractDigis end";

  return result;
}
