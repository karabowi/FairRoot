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

#include "PndEmcFullStackedWaveformSimulator.h"

#include "PndEmcWaveform.h"
#include "PndEmcWaveformData.h"
#include "PndEmcAbsPulseshape.h"

#include "TClonesArray.h"
#include "TMath.h"

#include "FairLink.h"
#include "FairRootManager.h"

#include <vector>
#include <map>
#include <iostream>

using namespace std;

PndEmcFullStackedWaveformSimulator::PndEmcFullStackedWaveformSimulator()
  : PndEmcAbsWaveformSimulator(0.), fSamplingBeforeFirstPulse(0), fSamplingAfterLastPulse(0), fCutoff(0), fPulseshape(nullptr), fScale(0.), f1GeVWaveform(nullptr),
    fPulseRiseTime(0), fActiveTimeIncrement(0)
{
}

PndEmcFullStackedWaveformSimulator::PndEmcFullStackedWaveformSimulator(Double_t sampleRate, PndEmcAbsPulseshape *pulseshape)
  : PndEmcAbsWaveformSimulator(sampleRate), fSamplingBeforeFirstPulse(0), fSamplingAfterLastPulse(0), fCutoff(0), fPulseshape(pulseshape), fScale(0.), f1GeVWaveform(nullptr),
    fPulseRiseTime(0), fActiveTimeIncrement(0)
{
}


PndEmcFullStackedWaveformSimulator::~PndEmcFullStackedWaveformSimulator() 
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
void PndEmcFullStackedWaveformSimulator::Init(Double_t samplingBeforeFirstPulse, Double_t samplingAfterLastPulse, Double_t cutoff, Double_t activeTimeIncrement) 
{
	fSamplingBeforeFirstPulse = samplingBeforeFirstPulse;
	fSamplingAfterLastPulse = samplingAfterLastPulse;
	fCutoff = cutoff;
	fActiveTimeIncrement = activeTimeIncrement;

	//generate signal with:
	Double_t tmpSamlingRate = 1.0; // in ns^-1
	Int_t tmpLength = 100; //in samples
	Int_t emergencyStop = 10000000;

	std::vector<Double_t> tmpSignal;

	Bool_t maxFound = kFALSE;
	Bool_t cutoffFound = kFALSE;

	Int_t maxSample;
	Int_t cutoffSample;

	do {
		tmpLength*=2;
		tmpSignal.resize(tmpLength);
		if(tmpLength>=emergencyStop) {
			std::cout << "-E PndEmcFullStackedWaveformSimulator: no maximum in pulse during " << emergencyStop/tmpSamlingRate << " ns found. WaveformGeneration might fail." << std::endl; 
			break;
		}

		for(Int_t iSample=0; iSample<tmpLength; iSample++) {
			tmpSignal[iSample] = CalcSingleWaveForTime(iSample/tmpSamlingRate, 1.0, 0.);
		}
		

		if(!maxFound) {	//search for maximum
			std::vector<Double_t>::const_iterator begin_constIt = tmpSignal.begin();
			std::vector<Double_t>::const_iterator end_constIt = tmpSignal.end();
			std::vector<Double_t>::const_iterator max_constIt = std::max_element(begin_constIt, end_constIt);
			maxSample = std::distance(begin_constIt, max_constIt);

			//generated pusle might be to short for proper maximum detection
			maxFound = (maxSample < tmpLength-1) ? kTRUE : kFALSE; 

			fScale = (*max_constIt);
			fPulseRiseTime = maxSample * tmpSamlingRate;
		}

		if(maxFound) { //search for cutOff
			for(Int_t iSample=maxSample; iSample<tmpLength; iSample++) {
				if(tmpSignal[iSample] < fCutoff*fScale) {
					cutoffFound = kTRUE;				
					cutoffSample = iSample;
					break;
				}
			}
		}

	} while(!(maxFound && cutoffFound)); 		//maximum inside signal, i.e. signal covers tail (at least partially) 

	//generate 1GeV signal again..now with correct sampling rate
	Double_t totalTime = (cutoffSample/tmpSamlingRate + fSamplingBeforeFirstPulse + fSamplingAfterLastPulse);

	try{
	std::vector<Double_t> signal((Int_t) TMath::Floor(totalTime*GetSampleRate() + 0.5), 0.);
	} catch(...) {
		std::cerr << "catched error" << std::endl;
		std::cerr << "sampleRate:" << GetSampleRate() << std::endl;
		std::cerr << "total Time:" << totalTime << std::endl;
		abort();
	}

	f1GeVWaveform = MakeSingleWaveform(1.0, 0.);

	std::cerr << "finishing init" << std::endl;
}



/**
 * @brief Get time interval for which the signal is above the cutoff
 * 
 * @param wfData Waveform
 * @param[out] startTime Start time of waveform (adjusted by fSamplingBeforeFirstPulse)
 * @param[out] activeTime Active time of waveform(s)
 * @return void
 */
void PndEmcFullStackedWaveformSimulator::GetAbsoluteTimeInterval(PndEmcWaveformData* wfData, Double_t& startTime, Double_t& activeTime) 
{
	const std::map<Double_t, Double_t>& hitMap = wfData->GetHitMap();
	startTime = hitMap.begin()->first - fSamplingBeforeFirstPulse;
	
	activeTime = hitMap.rbegin()->first + fPulseRiseTime;
	while (TMath::Abs(CalcWaveForTime(activeTime, wfData)) > fCutoff*fScale) {
		activeTime += fActiveTimeIncrement;
	}

	activeTime += fSamplingAfterLastPulse;

	SyncWithADCClock(startTime);
	SyncWithADCClock(activeTime);
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
Double_t PndEmcFullStackedWaveformSimulator::CalcSingleWaveForTime(Double_t absoluteTime, Double_t energy, Double_t pulseTime) 
{
	return fPulseshape->value(absoluteTime, energy, pulseTime);
}

/**
 * @brief Calculate pulse value at given time.
 * 
 * @param absoluteTime Time for which to calculate value.
 * @param wfData Waveform including the hit map.
 * @return Double_t Pulse value.
 */
Double_t PndEmcFullStackedWaveformSimulator::CalcWaveForTime(Double_t absoluteTime, PndEmcWaveformData* wfData) 
{
	Double_t returnValue = 0;

	for(std::map<Double_t, Double_t>::const_iterator it = wfData->GetHitMap().begin(); it!=wfData->GetHitMap().end(); ++it) {
		Double_t energy = it->second;
		Double_t pulseTime = it->first;

		returnValue += CalcSingleWaveForTime(absoluteTime, energy, pulseTime);
	}
	return returnValue;
}

/**
 * @brief Create PndEmcWaveform
 * 
 * @param wfData PndEmcWaveformData containing the hits.
 * @param arrayToStore If not null, the new PndEmcWaveform is created in this TClonesArray.
 * @return PndEmcWaveform*
 */
PndEmcWaveform* PndEmcFullStackedWaveformSimulator::MakeWaveform(PndEmcWaveformData* wfData, TClonesArray* arrayToStore)
{
	Double_t timeStamp = wfData->GetTimeStamp();	
	Int_t nSamples = static_cast<Int_t>((wfData->GetTimeOfLastSample()-timeStamp)*GetSampleRate() + 0.5);
	//cout << "TimeOfLastSample: " << wfData->GetTimeOfLastSample() << endl;
	//cout << "Timestamp: " << timeStamp << endl;
	//cout << "sampleRate: " << GetSampleRate() << endl;
	//cout << "calculted " << nSamples << " samples" << endl;
	std::vector<Double_t> signal(nSamples, 0.0);

	for(Int_t iSample=0; iSample<nSamples; ++iSample) {
		signal[iSample] = CalcWaveForTime(timeStamp + iSample/GetSampleRate(), wfData); 
// Uncomment to print out raw waveform datapoints
//cout << signal[iSample] << " ";
	}
//cout << endl;
//cout << "Waveform length: " << nSamples*5/GetSampleRate() << endl;
	Int_t hitIndex = -1;
	if(wfData->GetNLinks()) {
		hitIndex = wfData->GetLink(0).GetIndex();
	}

	PndEmcWaveform* wave;	
	if(arrayToStore) {
		wave = new ((*arrayToStore)[arrayToStore->GetEntriesFast()]) PndEmcWaveform(-1, wfData->GetDetectorId(), signal, hitIndex);
		wave->ResetLinks();
	} else {
		wave = new PndEmcWaveform(-1, wfData->GetDetectorId(), signal, hitIndex);
	}
	wave->SetLinks(wfData->GetLinksWithType(FairRootManager::Instance()->GetBranchId("EmcHit")));
	wave->AddInterfaceData(wfData);

	wave->SetTimeStamp(timeStamp);
	wave->SetSampleRate(fSampleRate*1.0e9);		//converting form GHz into Hz;

	return wave;

}

/**
 * @brief Create a PndEmcWaveform from the given parameters of a single hit.
 * 
 * @param hitEnergy ...
 * @param hitTime ...
 * @param arrayToStore ...
 * @param detId ...
 * @param trackId ...
 * @param hitIndex ...
 * @return PndEmcWaveform*
 */
PndEmcWaveform* PndEmcFullStackedWaveformSimulator::MakeSingleWaveform(Double_t hitEnergy, Double_t hitTime, TClonesArray* arrayToStore, Int_t detId, Int_t trackId, Int_t hitIndex) 
{
	std::vector<Double_t> signal;

	Double_t startTime = hitTime - fSamplingBeforeFirstPulse;
	SyncWithADCClock(startTime);

	Int_t nRising = (Int_t) ((fSamplingBeforeFirstPulse + fPulseRiseTime) * GetSampleRate());
	Int_t iSample =0;

	for(;iSample<nRising; ++iSample) { 	//time before pulse + rising edge
		signal.push_back(CalcSingleWaveForTime(startTime + iSample/GetSampleRate(), hitEnergy, hitTime));
	}

	do { 					//falling edge..check whether pulse undergoes cutoff value
		signal.push_back(CalcSingleWaveForTime(startTime + iSample/GetSampleRate(), hitEnergy, hitTime));
	} while (signal[iSample++] > fCutoff*fScale);

	Int_t addSamples = (Int_t) (fSamplingAfterLastPulse*GetSampleRate());

	for(addSamples += iSample; iSample<addSamples; iSample++) {
		signal.push_back(CalcSingleWaveForTime(startTime + iSample/GetSampleRate(), hitEnergy, hitTime));
	}
	std::cout << " signal.size" <<signal.size()<< std::endl;
	PndEmcWaveform* wave;	
	if(arrayToStore) {		
		wave = new ((*arrayToStore)[arrayToStore->GetEntriesFast()]) PndEmcWaveform(trackId, detId, signal, hitIndex);
	} else {
		wave = new PndEmcWaveform(trackId, detId, signal, hitIndex);
	}
//cout << startTime << endl;
	wave->SetTimeStamp(startTime);
	wave->SetSampleRate(5*fSampleRate*1.0e9);		//converting from GHz into Hz;

	return wave;
}
