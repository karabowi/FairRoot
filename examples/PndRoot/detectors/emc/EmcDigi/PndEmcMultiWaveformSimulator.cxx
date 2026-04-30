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

#include "PndEmcMultiWaveformSimulator.h"
#include "PndEmcWaveform.h"
#include "PndEmcMultiWaveform.h"
#include "PndEmcWaveformData.h"
#include "PndEmcAbsPulseshape.h"
#include "PndEmcAbsWaveformModifier.h"

#include "TClonesArray.h"
#include "TList.h"

#include <vector>


PndEmcMultiWaveformSimulator::PndEmcMultiWaveformSimulator() : PndEmcFullStackedWaveformSimulator(), fWfMultiplicity(0) 
{
}

PndEmcMultiWaveformSimulator::PndEmcMultiWaveformSimulator(Double_t sampleRate, PndEmcAbsPulseshape* pulseshape, Int_t wfMultiplicity) : PndEmcFullStackedWaveformSimulator(sampleRate, pulseshape), fWfMultiplicity(wfMultiplicity) 
{
}


PndEmcWaveform* PndEmcMultiWaveformSimulator::MakeWaveform(PndEmcWaveformData* wfData, TClonesArray* arrayToStore) 
{
  PndEmcWaveform *singleWaveform = PndEmcFullStackedWaveformSimulator::MakeWaveform(wfData, nullptr);
  PndEmcMultiWaveform* multiWaveform = MultiplyWaveform(singleWaveform, arrayToStore);

	delete singleWaveform;
	return multiWaveform;
}


PndEmcWaveform* PndEmcMultiWaveformSimulator::MakeSingleWaveform(Double_t hitEnergy, Double_t hitTime, TClonesArray* arrayToStore, Int_t detId, Int_t trackId, Int_t hitIndex) 
{
  PndEmcWaveform *singleWaveform = PndEmcFullStackedWaveformSimulator::MakeSingleWaveform(hitEnergy, hitTime, nullptr, detId, trackId, hitIndex);
  PndEmcMultiWaveform* multiWaveform = MultiplyWaveform(singleWaveform, arrayToStore);
	
	delete singleWaveform;
	return multiWaveform;
}


/**
 * @brief Add a modifier (PndEmcAbsWaveformModifier)
 * 
 * pass wfIndex Modifier shall work on as second parameter
 * 
 * @param wfModifier 
 * @param wfIndex 
 * @return void
 */
void PndEmcMultiWaveformSimulator::AddModifier(PndEmcAbsWaveformModifier* wfModifier, Int_t wfIndex) 
{
	fIndexList.push_back(wfIndex);
	PndEmcFullStackedWaveformSimulator::AddModifier(wfModifier);
}

/**
 * @brief Call modifiers on waveform @p wf
 * 
 * @param wf waveform
 * @return PndEmcWaveform* @p wf
 */
PndEmcWaveform* PndEmcMultiWaveformSimulator::CallModifiers(PndEmcWaveform* wf) 
{
	PndEmcMultiWaveform* multiWaveform = dynamic_cast<PndEmcMultiWaveform*>(wf);
	Int_t count = 0;
	TIter iterModifiers(&fListOfWaveformModifiers);

	while(PndEmcAbsWaveformModifier* wfmod = (PndEmcAbsWaveformModifier*) iterModifiers()) {
		if(fIndexList[count]>=0) {
			multiWaveform->SetActiveWaveform(fIndexList[count]);
		}
		wfmod->Modify(wf);
		++count;
	}	       
	return wf;
}

/**
 * @brief Return scale after all modifiers
 * 
 * @param wfIndex Waveform index.
 * @return Double_t The Scale.
 */
Double_t PndEmcMultiWaveformSimulator::GetTotalScale(Int_t wfIndex) 
{
	Double_t totalScale = GetScale();

	TIter iterModifiers(&fListOfWaveformModifiers);
	Int_t count = 0;

	while(PndEmcAbsWaveformModifier* wfmod = (PndEmcAbsWaveformModifier*) iterModifiers()) {
		if(fIndexList[count] == wfIndex || fIndexList[count]<0) {	
			totalScale *= wfmod->GetScale();
		}
		++count;
	}
	return totalScale;
}


/**
 * @brief Creates a PndEmcMultiWaveform from a PndEmcWaveform
 * 
 * @param singleWaveform The single waveform from which to create the multi-waveform.
 * @param arrayToStore If not null, the new PndEmcMultiWaveform is created in this TClonesArray.
 * @return PndEmcMultiWaveform*
 */
PndEmcMultiWaveform* PndEmcMultiWaveformSimulator::MultiplyWaveform(const PndEmcWaveform* singleWaveform, TClonesArray* arrayToStore) 
{
	const std::vector<Double_t>& signal = singleWaveform->GetSignal();

  PndEmcMultiWaveform *multiWaveform = nullptr;

  if (nullptr != arrayToStore) {
    multiWaveform = new ((*arrayToStore)[arrayToStore->GetEntriesFast()]) PndEmcMultiWaveform(singleWaveform->GetTrackId(), singleWaveform->GetDetectorId(), signal.size(), singleWaveform->GetHitIndex());
  } else {
    multiWaveform = new PndEmcMultiWaveform(singleWaveform->GetTrackId(), singleWaveform->GetDetectorId(), signal.size(), singleWaveform->GetHitIndex());
  }

  for(Int_t iWf = 0; iWf<fWfMultiplicity; ++iWf) {
		multiWaveform->SetWaveform(signal, signal.size(), iWf);
	}

	multiWaveform->SetLinks(singleWaveform->GetLinks());
	multiWaveform->SetTimeStamp(singleWaveform->GetTimeStamp());
	multiWaveform->SetSampleRate(singleWaveform->GetSampleRate());	

	return multiWaveform;
}
