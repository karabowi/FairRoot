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

//#pragma once
#ifndef PNDEMCBWENDCAPNOISEADDER_H_
#define PNDEMCBWENDCAPNOISEADDER_H_

#include "PndEmcAbsWaveformModifier.h"
#include "TString.h"
#include "TRandom.h"

class PndEmcWaveform;
class TVirtualFFT;

/**
 * @brief waveform modifier to add noise to waveform
 * @author Guang Zhao (zhaog@ihep.ac.cn)
 * @ingroup PndEmc
 */
class PndEmcBWEndcapNoiseAdder : public PndEmcAbsWaveformModifier 
{
    public:
        PndEmcBWEndcapNoiseAdder();
		~PndEmcBWEndcapNoiseAdder();
        PndEmcBWEndcapNoiseAdder(Int_t mode, Double_t sampling_rate, const Double_t* ps_freq,
			const Double_t* ps_power, Int_t ps_size, Double_t adc_noise, Double_t fe_noise, 
			Double_t sigma_ps, Double_t sigma_ges, Int_t seed, Int_t verbose); // mode: 0 - full, 1 - reduced, 2 - last

        virtual void  Modify(PndEmcWaveform* wf);

    private:
		Int_t fTraceLength;
		Int_t fBufferPosition;
		Int_t fWaveformLength;
		Int_t fPowerSpecSample;
		Double_t fSamplingRate;
		Double_t fAdcNoise;
		Double_t fFENoise;
        Double_t fSigmaPs;
        Double_t fSigmaGes;
		Double_t fInputScale;
		const Double_t* fPSFreq;
		const Double_t* fPSPower;
		TVirtualFFT* fft;
		Int_t fVerbose;

		Int_t fBufferSize;
		Int_t fPSNoiseMode; // 0 for precision; 1 for speed; 2 for last
		Double_t fBufferUseRate;
		Bool_t fBufferIsEmpty;
		Double_t* fPSNoiseBuffer;
		Double_t* fBandNoiseBuffer;
		Double_t* fFFTFreqBuffer;
		Double_t* fFFTReCoeffBuffer;
		Double_t* fFFTImCoeffBuffer;
		void expand_buffer(Int_t wf_size);

		void get_ps_noise(PndEmcWaveform* wf);
		void generate_ps_noise();
		void generate_band_noise(Double_t min_freq, Double_t max_freq);
		void get_rnd_noise(PndEmcWaveform* wf, Double_t noise);

        ClassDef(PndEmcBWEndcapNoiseAdder, 1)
};

#endif
