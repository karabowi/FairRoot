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

#include "BSEmcMultiWaveformSimulator.h"

#include <vector>

#include "TClonesArray.h"
#include "TCollection.h"
#include "TList.h"
#include "TObject.h"

#include "fairlogger/Logger.h"

#include "BSEmcAbsWaveformModifier.h"
#include "BSEmcMultiWaveform.h"
#include "BSEmcWaveform.h"

class BSEmcWaveformData;

BSEmcMultiWaveformSimulator::BSEmcMultiWaveformSimulator() : BSEmcFullStackedWaveformSimulator(), fWfMultiplicity(0) {}

BSEmcMultiWaveformSimulator::BSEmcMultiWaveformSimulator(Double_t t_sampleRate, BSEmcAbsPulseshape *t_pulseshape, Int_t t_wfMultiplicity)
  : BSEmcFullStackedWaveformSimulator(t_sampleRate, t_pulseshape), fWfMultiplicity(t_wfMultiplicity)
{
}

BSEmcWaveform *BSEmcMultiWaveformSimulator::MakeWaveform(BSEmcWaveformData *t_wfData, TClonesArray *t_arrayToStore)
{
  BSEmcWaveform *singleWaveform = BSEmcFullStackedWaveformSimulator::MakeWaveform(t_wfData, nullptr);
  BSEmcMultiWaveform *multiWaveform = MultiplyWaveform(singleWaveform, t_arrayToStore);

  delete singleWaveform;
  return multiWaveform;
}

BSEmcWaveform *BSEmcMultiWaveformSimulator::MakeSingleWaveform(Double_t t_depositEnergy, Double_t t_depositTime, TClonesArray *t_arrayToStore, Int_t t_detId, Int_t t_trackId,
                                                               Int_t t_depositIndex)
{
  BSEmcWaveform *singleWaveform = BSEmcFullStackedWaveformSimulator::MakeSingleWaveform(t_depositEnergy, t_depositTime, nullptr, t_detId, t_trackId, t_depositIndex);
  BSEmcMultiWaveform *multiWaveform = MultiplyWaveform(singleWaveform, t_arrayToStore);

  delete singleWaveform;
  return multiWaveform;
}

/**
 * @brief Add a modifier (BSEmcAbsWaveformModifier)
 *
 * pass wfIndex Modifier shall work on as second parameter
 *
 * @param wfModifier
 * @param wfIndex
 * @return void
 */
void BSEmcMultiWaveformSimulator::AddModifier(BSEmcAbsWaveformModifier *t_wfModifier, Int_t t_wfIndex)
{
  fIndexList.push_back(t_wfIndex);
  BSEmcFullStackedWaveformSimulator::AddModifier(t_wfModifier);
}

/**
 * @brief Call modifiers on waveform @p wf
 *
 * @param wf waveform
 * @return BSEmcWaveform* @p wf
 */
BSEmcWaveform *BSEmcMultiWaveformSimulator::CallModifiers(BSEmcWaveform *t_wf)
{
  BSEmcMultiWaveform *multiWaveform = dynamic_cast<BSEmcMultiWaveform *>(t_wf);
  Int_t count = 0;
  TIter iterModifiers(&fListOfWaveformModifiers);

  while (BSEmcAbsWaveformModifier *wfmod = dynamic_cast<BSEmcAbsWaveformModifier *>(iterModifiers())) {
    if (fIndexList[count] >= 0) {
      multiWaveform->SetActiveWaveform(fIndexList[count]);
    }
    wfmod->Modify(t_wf);
    LOG(debug3) << "Signal after modifier: " << count << ":";
    for (Double_t sig : t_wf->GetSignal()) {
      LOG(debug3) << sig;
    }
    ++count;
  }
  return t_wf;
}

/**
 * @brief Return scale after all modifiers
 *
 * @param wfIndex Waveform index.
 * @return Double_t The Scale.
 */
Double_t BSEmcMultiWaveformSimulator::GetTotalScale(Int_t t_wfIndex)
{
  Double_t totalScale = GetScale();

  TIter iterModifiers(&fListOfWaveformModifiers);
  Int_t count = 0;

  while (BSEmcAbsWaveformModifier *wfmod = dynamic_cast<BSEmcAbsWaveformModifier *>(iterModifiers())) {
    if (fIndexList[count] == t_wfIndex || fIndexList[count] < 0) {
      totalScale *= wfmod->GetScale();
    }
    ++count;
  }
  return totalScale;
}

/**
 * @brief Creates a BSEmcMultiWaveform from a BSEmcWaveform
 *
 * @param singleWaveform The single waveform from which to create the multi-waveform.
 * @param arrayToStore If not nullptr, the new BSEmcMultiWaveform is created in this TClonesArray.
 * @return BSEmcMultiWaveform*
 */
BSEmcMultiWaveform *BSEmcMultiWaveformSimulator::MultiplyWaveform(const BSEmcWaveform *t_singleWaveform, TClonesArray *t_arrayToStore)
{
  const std::vector<Double_t> &signal = t_singleWaveform->GetSignal();

  BSEmcMultiWaveform *multiWaveform = nullptr;

  if (nullptr != t_arrayToStore) {
    multiWaveform = new ((*t_arrayToStore)[t_arrayToStore->GetEntriesFast()])
      BSEmcMultiWaveform(t_singleWaveform->GetTrackId(), t_singleWaveform->GetDetectorId(), signal.size(), t_singleWaveform->GetHitIndex());
  } else {
    multiWaveform = new BSEmcMultiWaveform(t_singleWaveform->GetTrackId(), t_singleWaveform->GetDetectorId(), signal.size(), t_singleWaveform->GetHitIndex());
  }

  for (Int_t iWf = 0; iWf < fWfMultiplicity; ++iWf) {
    multiWaveform->SetWaveform(signal, signal.size(), iWf);
  }

  multiWaveform->SetLinks(t_singleWaveform->GetLinks());
  multiWaveform->SetTimeStamp(t_singleWaveform->GetTimeStamp());
  multiWaveform->SetSampleRate(t_singleWaveform->GetSampleRate()); // converting form ns into s;

  return multiWaveform;
}
