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

#include "BSEmcFullStackedWaveformSimulator.h"

#include <FairRootManager.h>
#include <algorithm>
#include <iterator>
#include <map>
#include <stdlib.h>
#include <utility>
#include <vector>

#include "TClonesArray.h"
#include "TMath.h"
#include "TMathBase.h"

#include "FairLink.h"
#include "fairlogger/Logger.h"

#include "BSEmcAbsPulseshape.h"
#include "BSEmcWaveform.h"
#include "BSEmcWaveformData.h"

using namespace std;

BSEmcFullStackedWaveformSimulator::BSEmcFullStackedWaveformSimulator()
  : BSEmcAbsWaveformSimulator(0.), fSamplingBeforeFirstPulse(0), fSamplingAfterLastPulse(0), fCutoff(0), fPulseshape(nullptr), fScale(0.), f1GeVWaveform(nullptr),
    fPulseRiseTime(0), fActiveTimeIncrement(0)
{
}

BSEmcFullStackedWaveformSimulator::BSEmcFullStackedWaveformSimulator(Double_t t_sampleRate, BSEmcAbsPulseshape *t_pulseshape)
  : BSEmcAbsWaveformSimulator(t_sampleRate), fSamplingBeforeFirstPulse(0), fSamplingAfterLastPulse(0), fCutoff(0), fPulseshape(t_pulseshape), fScale(0.), f1GeVWaveform(nullptr),
    fPulseRiseTime(0), fActiveTimeIncrement(0)
{
}

BSEmcFullStackedWaveformSimulator::~BSEmcFullStackedWaveformSimulator()
{
  delete f1GeVWaveform;
}

/**
 * @brief Init the simulator
 *
 * @param samplingBeforeFirstPulse Time to start sampling before first pulse, in ns
 * @param samplingAfterLastPulse Time to stop sampling after last pulse, in ns
 * @param cutoff Threshold under which signal is considered to have stopped, in GeV
 * @param activeTimeIncrement Increment when checking if pulse is over cutoff, in ns
 * @return void
 */
void BSEmcFullStackedWaveformSimulator::Init(Double_t t_samplingBeforeFirstPulse, Double_t t_samplingAfterLastPulse, Double_t t_cutoff, Double_t t_activeTimeIncrement)
{
  fSamplingBeforeFirstPulse = t_samplingBeforeFirstPulse;
  fSamplingAfterLastPulse = t_samplingAfterLastPulse;
  fCutoff = t_cutoff;
  fActiveTimeIncrement = t_activeTimeIncrement;

  // generate signal with:
  Double_t tmpSamlingRate = 1.0; // in ns^-1
  Int_t tmpLength = 100;         // in samples
  Int_t emergencyStop = 10000000;

  std::vector<Double_t> tmpSignal;

  Bool_t maxFound = kFALSE;
  Bool_t cutoffFound = kFALSE;

  Int_t maxSample = 0;
  Int_t cutoffSample = 0;

  do {
    tmpLength *= 2;
    tmpSignal.resize(tmpLength);
    if (tmpLength >= emergencyStop) {
      LOG(error) << "BSEmcFullStackedWaveformSimulator: no maximum in pulse during " << emergencyStop / tmpSamlingRate << " ns found. WaveformGeneration might fail.";
      break;
    }

    for (Int_t iSample = 0; iSample < tmpLength; iSample++) {
      tmpSignal[iSample] = CalcSingleWaveForTime(iSample / tmpSamlingRate, 1.0, 0.);
    }

    if (!maxFound) { // search for maximum
      std::vector<Double_t>::const_iterator begin_constIt = tmpSignal.begin();
      std::vector<Double_t>::const_iterator end_constIt = tmpSignal.end();
      std::vector<Double_t>::const_iterator max_constIt = std::max_element(begin_constIt, end_constIt);
      maxSample = std::distance(begin_constIt, max_constIt);

      // generated pusle might be to short for proper maximum detection
      maxFound = (maxSample < tmpLength - 1) ? kTRUE : kFALSE;

      fScale = (*max_constIt);
      fPulseRiseTime = maxSample * tmpSamlingRate;
    }

    if (maxFound) { // search for cutOff
      for (Int_t iSample = maxSample; iSample < tmpLength; iSample++) {
        if (tmpSignal[iSample] < fCutoff * fScale) {
          cutoffFound = kTRUE;
          cutoffSample = iSample;
          break;
        }
      }
    }

  } while (!(maxFound && cutoffFound)); // maximum inside signal, i.e. signal covers tail (at least partially)

  // generate 1GeV signal again..now with correct sampling rate
  Double_t totalTime = (cutoffSample / tmpSamlingRate + fSamplingBeforeFirstPulse + fSamplingAfterLastPulse);

  try {
    std::vector<Double_t> signal((Int_t)TMath::Floor(totalTime * GetSampleRate() + 0.5), 0.);
  } catch (...) {
    LOG(error) << "catched error - sampleRate:" << GetSampleRate() << "total Time:" << totalTime;
    abort();
  }

  f1GeVWaveform = MakeSingleWaveform(1.0, 0.);

  LOG(debug) << "BSEmcFullStackedWaveformSimulator finishing init";
}

/**
 * @brief Get time interval for which the signal is above the cutoff
 *
 * @param wfData Waveform
 * @param[out] startTime Start time of waveform (adjusted by fSamplingBeforeFirstPulse)
 * @param[out] activeTime Active time of waveform(s)
 * @return void
 */
void BSEmcFullStackedWaveformSimulator::GetAbsoluteTimeInterval(BSEmcWaveformData *t_wfData, Double_t &t_startTime, Double_t &t_activeTime)
{
  const std::map<Double_t, Double_t> &depositMap = t_wfData->GetDepositMap();
  t_startTime = depositMap.begin()->first - fSamplingBeforeFirstPulse;

  t_activeTime = depositMap.rbegin()->first + fPulseRiseTime;
  while (TMath::Abs(CalcWaveForTime(t_activeTime, t_wfData)) > fCutoff * fScale) {
    t_activeTime += fActiveTimeIncrement;
  }

  t_activeTime += fSamplingAfterLastPulse;

  SyncWithADCClock(t_startTime);
  SyncWithADCClock(t_activeTime);
}

/**
 * @brief Return pulse at given time and for given energy.
 *
 * Time in the pulse is @p absoluteTime-@p pulseTime
 *
 * @param absoluteTime Time for which to calculate value.
 * @param energy Energy of the pulse.
 * @param pulseTime Time when the pulse starts.
 * @return Double_t Pulse value.
 */
Double_t BSEmcFullStackedWaveformSimulator::CalcSingleWaveForTime(Double_t t_absoluteTime, Double_t t_energy, Double_t t_pulseTime)
{
  return fPulseshape->value(t_absoluteTime, t_energy, t_pulseTime);
}

/**
 * @brief Calculate pulse value at given time.
 *
 * @param absoluteTime Time for which to calculate value.
 * @param wfData Waveform including the deposit map.
 * @return Double_t Pulse value.
 */
Double_t BSEmcFullStackedWaveformSimulator::CalcWaveForTime(Double_t t_absoluteTime, BSEmcWaveformData *t_wfData)
{
  Double_t returnValue = 0;
  LOG(debug2) << "Looking at waveform at timepoint " << t_absoluteTime << "for detectorId " << t_wfData->GetDetectorId() << " with " << t_wfData->GetDepositMap().size()
              << " deposits contributing";
  for (const auto &it : t_wfData->GetDepositMap()) {
    const Double_t energy = it.second;
    const Double_t pulseTime = it.first;
    const Double_t result = CalcSingleWaveForTime(t_absoluteTime, energy, pulseTime);
    LOG(debug2) << "Energy: " << energy << " and PulseTime: " << pulseTime << " Contributing to signal at " << t_absoluteTime << " with " << result;
    returnValue += result;
  }
  return returnValue;
}

/**
 * @brief Create BSEmcWaveform
 *
 * @param wfData BSEmcWaveformData containing the deposits.
 * @param arrayToStore If not nullptr, the new BSEmcWaveform is created in this TClonesArray.
 * @return BSEmcWaveform*
 */
BSEmcWaveform *BSEmcFullStackedWaveformSimulator::MakeWaveform(BSEmcWaveformData *t_wfData, TClonesArray *t_arrayToStore)
{
  Int_t nSamples = static_cast<Int_t>((t_wfData->GetTimeOfLastSample() - t_wfData->GetTimeStamp()) * GetSampleRate() + 0.5);
  LOG(debug3) << "TimeOfLastSample: " << t_wfData->GetTimeOfLastSample() << "Timestamp: " << t_wfData->GetTimeStamp() << "sampleRate: " << GetSampleRate() << "calculted "
              << nSamples << " samples";

  std::vector<Double_t> signal(nSamples, 0.0);

  for (Int_t iSample = 0; iSample < nSamples; ++iSample) {
    signal[iSample] = CalcWaveForTime(t_wfData->GetTimeStamp() + iSample * 1 / GetSampleRate(), t_wfData);
  }

  Int_t depositIndex = -1;
  if (t_wfData->GetNLinks() != 0) {
    depositIndex = t_wfData->GetLink(0).GetIndex();
  }

  BSEmcWaveform *wave = nullptr;
  if (t_arrayToStore != nullptr) {
    wave = new ((*t_arrayToStore)[t_arrayToStore->GetEntriesFast()]) BSEmcWaveform(-1, t_wfData->GetDetectorId(), signal, depositIndex);
    wave->ResetLinks();
  } else {
    wave = new BSEmcWaveform(-1, t_wfData->GetDetectorId(), signal, depositIndex);
  }
  wave->SetOverlapTime(t_wfData->GetOverlapTime());
  wave->SetLinks(t_wfData->GetLinksWithType(FairRootManager::Instance()->GetBranchId(fMCDepositBranchName)));
  wave->AddInterfaceData(t_wfData);

  Double_t timeStamp = t_wfData->GetTimeStamp();
  wave->SetTimeStamp(timeStamp);
  wave->SetSampleRate(fSampleRate * 1.0e9); // converting form ns into s;

  return wave;
}

/**
 * @brief Create a BSEmcWaveform from the given parameters of a single deposit.
 *
 * @param depositEnergy ...
 * @param depositTime ...
 * @param arrayToStore ...
 * @param detId ...
 * @param trackId ...
 * @param depositIndex ...
 * @return BSEmcWaveform*
 */
BSEmcWaveform *BSEmcFullStackedWaveformSimulator::MakeSingleWaveform(Double_t t_depositEnergy, Double_t t_depositTime, TClonesArray *t_arrayToStore, Int_t t_detId, Int_t t_trackId,
                                                                     Int_t t_depositIndex)
{
  std::vector<Double_t> signal;

  Double_t startTime = t_depositTime - fSamplingBeforeFirstPulse;
  SyncWithADCClock(startTime);

  Int_t nRising = (Int_t)((fSamplingBeforeFirstPulse + fPulseRiseTime) * GetSampleRate());
  Int_t iSample = 0;

  for (; iSample < nRising; ++iSample) { // time before pulse + rising edge
    signal.push_back(CalcSingleWaveForTime(startTime + iSample / GetSampleRate(), t_depositEnergy, t_depositTime));
  }

  do { // falling edge..check weather pulse undergoes cutoff value
    signal.push_back(CalcSingleWaveForTime(startTime + iSample / GetSampleRate(), t_depositEnergy, t_depositTime));
  } while (signal[iSample++] > fCutoff * fScale);

  Int_t addSamples = (Int_t)(fSamplingAfterLastPulse * GetSampleRate());

  for (addSamples += iSample; iSample < addSamples; iSample++) {
    signal.push_back(CalcSingleWaveForTime(startTime + iSample / GetSampleRate(), t_depositEnergy, t_depositTime));
  }

  BSEmcWaveform *wave = nullptr;
  if (t_arrayToStore != nullptr) {
    wave = new ((*t_arrayToStore)[t_arrayToStore->GetEntriesFast()]) BSEmcWaveform(t_trackId, t_detId, signal, t_depositIndex);
  } else {
    wave = new BSEmcWaveform(t_trackId, t_detId, signal, t_depositIndex);
  }

  Double_t timestamp = startTime;
  wave->SetTimeStamp(timestamp);
  wave->SetSampleRate(fSampleRate * 1.0e9); // converting form ns into s;

  return wave;
}
