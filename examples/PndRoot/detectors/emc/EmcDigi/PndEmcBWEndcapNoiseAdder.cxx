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

#include "PndEmcBWEndcapNoiseAdder.h"
#include "PndEmcAbsWaveformSimulator.h"

#include "PndEmcWaveform.h"
#include "TRandom.h"
#include "TMath.h"
#include "TVirtualFFT.h"
#include "FairLogger.h"

#include <vector>
#include <fstream>
#include <iostream>

using std::cout;
using std::endl;

PndEmcBWEndcapNoiseAdder::PndEmcBWEndcapNoiseAdder() {
    fTraceLength = 0.;
    fPowerSpecSample = 0.;
    fSamplingRate = 0.;
    fAdcNoise = 0.;
    fFENoise = 0.;
    fSigmaPs = 0.;
    fSigmaGes = 0.;
    fTraceLength = 128;
    fPSNoiseBuffer = 0;
    fBandNoiseBuffer = 0;
    fFFTFreqBuffer = 0;
    fFFTReCoeffBuffer = 0;
    fFFTImCoeffBuffer = 0;
    fft = nullptr;
    fBufferIsEmpty = kTRUE;
    fBufferSize = 0;
    fBufferUseRate = 0.;
    fPSNoiseMode = 1;
    fVerbose = 0;
}

PndEmcBWEndcapNoiseAdder::PndEmcBWEndcapNoiseAdder(
    Int_t mode, Double_t sampling_rate, const Double_t* ps_freq, const Double_t* ps_power, Int_t ps_size,
    Double_t adc_noise, Double_t fe_noise,
    Double_t sigma_ps, Double_t sigma_ges, Int_t seed, Int_t verbose)
{
    fPSNoiseMode = mode;
    fSamplingRate = sampling_rate * 1E3; // GHz -> MHz
    fAdcNoise = adc_noise;
    fFENoise = fe_noise;
    fSigmaPs = sigma_ps;
    fSigmaGes = sigma_ges;
    fPSFreq = ps_freq;
    fPSPower = ps_power;
    fPowerSpecSample = 2 * ps_size + 1;
    fTraceLength = 1024;

    fBufferIsEmpty = kTRUE;
    fBufferSize = 16 * fTraceLength;
    fBufferUseRate = 0.;
    fBufferPosition = 0;
    fPSNoiseBuffer = new Double_t[fBufferSize];
    fBandNoiseBuffer = new Double_t[fTraceLength];
    fFFTFreqBuffer = new Double_t[fTraceLength];
    fFFTReCoeffBuffer = new Double_t[fTraceLength];
    fFFTImCoeffBuffer = new Double_t[fTraceLength];

    fft = TVirtualFFT::FFT(1, &fTraceLength, "C2CBACKWARD ES K");

    fVerbose = verbose;
}

PndEmcBWEndcapNoiseAdder::~PndEmcBWEndcapNoiseAdder() {
    if (!fPSNoiseBuffer) delete[] fPSNoiseBuffer;
    if (!fBandNoiseBuffer) delete[] fBandNoiseBuffer;
    if (!fFFTFreqBuffer) delete[] fFFTFreqBuffer;
    if (!fFFTReCoeffBuffer) delete[] fFFTReCoeffBuffer;
    if (!fFFTImCoeffBuffer) delete[] fFFTImCoeffBuffer;
    if (!fft) delete fft;
}

void PndEmcBWEndcapNoiseAdder::Modify(PndEmcWaveform* wf) {
    fWaveformLength = wf->GetWaveformLength();

    get_ps_noise(wf);
    get_rnd_noise(wf, fAdcNoise);
    get_rnd_noise(wf, fFENoise);
}

void PndEmcBWEndcapNoiseAdder::get_ps_noise(PndEmcWaveform* wf) {
    if (fWaveformLength > fBufferSize) expand_buffer(fWaveformLength);

    switch (fPSNoiseMode)
    {
        case 0: // precision mode
            fBufferPosition += fWaveformLength;
            if (fBufferIsEmpty || fBufferPosition + fWaveformLength > fBufferSize) {
                generate_ps_noise();
                fBufferPosition = 0;
                fBufferIsEmpty = kFALSE;
            }
            break;

        case 1: // speed mode
            fBufferUseRate += (Double_t)fWaveformLength / fBufferSize;
            //cout << "use rate = " << fBufferUseRate << endl;
            if (fBufferIsEmpty || fBufferUseRate > 1000.) {
                generate_ps_noise();
                fBufferUseRate = 0.;
                fBufferIsEmpty = kFALSE;
            }
            do {
              fBufferPosition = (Int_t)(gRandom->Rndm() * fBufferSize);
            } while (fBufferPosition + fWaveformLength > fBufferSize);
            break;

        default:
            return;
    }

    std::vector<Double_t>::iterator it;
    Int_t idx = 0;
	for (it = GetWaveformReference(wf).begin(); it != GetWaveformReference(wf).end(); ++it) { 
        (*it) += fPSNoiseBuffer[fBufferPosition + idx];
        idx++;
    }


    if (fVerbose > 2) {
        cout << "[PndEmcBWEndcapNoiseAdder]:[ps noise] " << fWaveformLength << endl;
        for (int i = 0; i < fWaveformLength; i++)
        {
            cout << "[PndEmcBWEndcapNoiseAdder]:[ps noise] " << fPSNoiseBuffer[fBufferPosition + i] << endl;
        }
    }

}

void PndEmcBWEndcapNoiseAdder::generate_ps_noise() {
    if (fVerbose > 2) cout << "[PndEmcBWEndcapNoiseAdder]: generating ps noise" << endl;
	Double_t power_gain = sqrt((Double_t)fTraceLength/fPowerSpecSample);
    Double_t fac = sqrt(fSigmaGes*fSigmaGes - fAdcNoise*fAdcNoise - fFENoise*fFENoise)/fSigmaPs;

    Int_t nTraces = fBufferSize/fTraceLength;

    for (Int_t i = 0; i < nTraces; i++) { // generate nTraces ps noises
        for (int j = 0; j < fPowerSpecSample / 2 - 1; j++) {
            Double_t low, high, pwr;
            low = ((j == 0) ? 0 : fPSFreq[j - 1]);
            high = fPSFreq[j];
            pwr = fPSPower[j];
            Double_t power_factor = pow(10, (pwr / 20));

            generate_band_noise(low, high);
            for (Int_t k = 0; k < fTraceLength; k++) {
                if (j == 0)
                    fPSNoiseBuffer[k + i*fTraceLength] = 0.;
                fPSNoiseBuffer[k + i*fTraceLength] += fBandNoiseBuffer[k] * power_factor * power_gain * fac;
            }
        }
    }
}


void PndEmcBWEndcapNoiseAdder::generate_band_noise(Double_t min_freq, Double_t max_freq) {
    Int_t samples = fTraceLength;
	Double_t sample_rate = fSamplingRate;

    // calculate frequences for ifft
    if (fBufferIsEmpty) {
        for (Int_t i = 0; i < samples; i++) {
            if (i < (samples + 1) / 2)
                fFFTFreqBuffer[i] = 1. / samples * sample_rate * i;
            else
                fFFTFreqBuffer[i] = 1. / samples * sample_rate * (samples - i);
        }
    }

    // randomly generate phases
    Int_t np = (samples - 1) / 2;
    const Double_t pi = 3.1415927;
    Int_t non_zero_term = 0;
    for (Int_t i = 0; i < samples; i++) {
        if (fFFTFreqBuffer[i] >= min_freq && fFFTFreqBuffer[i] <= max_freq) {
            if (i >= 1 && i < np + 1) {
              Double_t phase = 2 * pi * gRandom->Rndm();
              fFFTReCoeffBuffer[i] = cos(phase);
              fFFTImCoeffBuffer[i] = sin(phase);
            }
            else if (i >= samples - np && i < samples) {
                fFFTReCoeffBuffer[i] = fFFTReCoeffBuffer[samples - i];
                fFFTImCoeffBuffer[i] = -fFFTImCoeffBuffer[samples - i];
            }
            else {
                fFFTReCoeffBuffer[i] = 1.;
                fFFTImCoeffBuffer[i] = 0.;
            }
            non_zero_term++;
        } 
        else {
            fFFTReCoeffBuffer[i] = 0.;
            fFFTImCoeffBuffer[i] = 0.;
        }
    }

    if (non_zero_term == 0) { // skip fft if all coefficiences are 0
        for (Int_t i = 0; i < samples; i++) {
            fBandNoiseBuffer[i] = 0.;
        }
        return;
    }

    // inverse-fft
    fft->SetPointsComplex(fFFTReCoeffBuffer, fFFTImCoeffBuffer);
    fft->Transform();
    fft->GetPointsComplex(fFFTReCoeffBuffer, fFFTImCoeffBuffer);

    for (Int_t i = 0; i < samples; i++) {
        fBandNoiseBuffer[i] = fFFTReCoeffBuffer[i]/samples;
    }
}

void PndEmcBWEndcapNoiseAdder::get_rnd_noise(PndEmcWaveform* wf, Double_t noise_width) {
    if (fVerbose > 2) {
        cout << "[PndEmcBWEndcapNoiseAdder]:[rnd noise] " << fWaveformLength << " " << noise_width << endl;
    }

    std::vector<Double_t>::iterator it;
    Int_t i = 0;
	for (it = GetWaveformReference(wf).begin(); it != GetWaveformReference(wf).end(); ++it) {
    Double_t noise = gRandom->Gaus(0, noise_width);
    (*it) += noise;
    i++;

    if (fVerbose > 2)
      cout << "[PndEmcBWEndcapNoiseAdder]:[rnd noise] " << noise << endl;
    }
}

void PndEmcBWEndcapNoiseAdder::expand_buffer(Int_t wf_size) {
    if (!fPSNoiseBuffer) delete[] fPSNoiseBuffer;
    while (fBufferSize < wf_size) {
        fBufferSize *= 2;
    }
    fPSNoiseBuffer = new Double_t[fBufferSize];
    LOG(info) << " PndEmcBWEndcapNoiseAdder::buffer expanded: " << fBufferSize;
}
