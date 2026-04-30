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

#include "PndEmcPSATmaxAnalyser.h"
#include "PndEmcWaveform.h"
#include "PndEmcMultiWaveform.h"

#include <iostream>
#include <fstream>
#include <cmath>

using std::cout;
using std::endl;

PndEmcPSATmaxAnalyser::PndEmcPSATmaxAnalyser(
	const Double_t* coeff, Int_t taps, Int_t gap, Int_t samplingrate,
	Double_t hit_thr, Double_t tut_peak, Double_t hit_val, Int_t verbose)
{
	fFIRCoeff = coeff;
	fTaps = taps;
	fGap = gap;
	fHitThr = hit_thr;
	fTutPeak = tut_peak;
	fHitVal = hit_val;
	fTimeStep = 1./samplingrate * 1e3; // time step in ns
	fVerbose = verbose;
}

PndEmcPSATmaxAnalyser::~PndEmcPSATmaxAnalyser() {
}

void PndEmcPSATmaxAnalyser::Reset() {
	fEnergyList.clear();
	fTimeList.clear();
}

Int_t PndEmcPSATmaxAnalyser::Process(const PndEmcWaveform *waveform) {
	// convert vector to array
	Int_t size = waveform->GetWaveformLength();
	Double_t* input = new Double_t[size];
	Double_t time_stamp = waveform->GetTimeStamp(); // ns
	Double_t sample_rate = waveform->GetSampleRate()/1E9; // GHz

	static Int_t iWf = 0;
	if (fVerbose > 2) {
		cout << "[PndEmcPSATmaxAnalyser]:[TMAX] " << "waveform_" << iWf << " " << size << endl;
	}

	std::vector<Double_t> wf = waveform->GetSignal();
	for (Int_t i = 0; i < size; i++) {
		input[i] = wf[i];
		if (fVerbose > 2) cout << "[PndEmcPSATmaxAnalyser]:[TMAX] " << time_stamp+(double)i/sample_rate << " " << input[i] << endl;
	}

	//FIR smoothing
	Double_t* signal = fir(input, size);
	if (fVerbose > 2) {
		cout << "[PndEmcPSATmaxAnalyser]:[TMAX] " << "waveform_fir_" << iWf << " " << size << endl;
		for (Int_t i = 0; i < size; i++)
		{
			cout << "[PndEmcPSATmaxAnalyser]:[TMAX] " << time_stamp + (double)i / sample_rate << " " << signal[i] << endl;
		}
	}
	
	//Derivative
	std::vector<Double_t> deri_n(size, 0.);
	Double_t deri_p;
	std::vector<Double_t> deri(size, 0.);
	std::vector<Double_t> deri_time(size, 0.);
	std::vector<Double_t> deri2_time(size, 0.);
	
	for (int n = fTaps + fGap; n < size; n++) {
		deri_p = -signal[n-fGap] + signal[n];
		deri_time[n] = -signal[n] + signal[n-fGap];
		if(deri_time[n] < 0){
			deri_n[n] = 0;
		}
		else{
			deri_n[n] = deri_time[n];
		}
		deri[n] = deri_p + deri_n[n];
	}

	//Second derivative
	for (int n = fTaps + 2*fGap; n < size; n++) {
		deri2_time[n] = -deri_time[n] + deri_time[n-fGap];
	}

	delete[] signal;

	if (fVerbose > 2) {
		cout << "[PndEmcPSATmaxAnalyser]:[TMAX] " << "waveform_D_" << iWf << " " << size << endl;
		for (Int_t i = 0; i < size; i++)
		{
			cout << "[PndEmcPSATmaxAnalyser]:[TMAX] " << time_stamp + (double)i / sample_rate << " " << -deri_time[i] << endl;
		}
		cout << "[PndEmcPSATmaxAnalyser]:[TMAX] " << "waveform_Dinv_" << iWf << " " << size << endl;
		for (Int_t i = 0; i < size; i++)
		{
			cout << "[PndEmcPSATmaxAnalyser]:[TMAX] " << time_stamp + (double)i / sample_rate << " " << deri[i] << endl;
		}
		cout << "[PndEmcPSATmaxAnalyser]:[TMAX] " << "waveform_D2_" << iWf << " " << size << endl;
		for (Int_t i = 0; i < size; i++)
		{
			cout << "[PndEmcPSATmaxAnalyser]:[TMAX] " << time_stamp + (double)i / sample_rate << " " << -deri2_time[i] << endl;
		}
	}
	
	//Integral
	Double_t sum = 0;
	Double_t temp = 0;
	Double_t d_last = 0;
	Double_t zp = 0;
	Double_t time = 0;
	Int_t tut_int = 0;
	Double_t hit_conv = 0;
	Bool_t hit = kFALSE;
	std::vector<Double_t> vamp;
	std::vector<Double_t> vtime;
	std::vector<Double_t> ds(size, 0.);
	std::vector<double_t> hc(size, 0.);
	
	//Events
	for (Int_t n = 0; n < size ; n++) {
		//Time extraction
		//if ((d_last < 0) && (deri_time[n] > 0)) {
		//	zp = -d_last / (deri_time[n] - d_last);
		//	time = n - 1 + zp;
		//}
		if (d_last <= 0 && deri2_time[n] > 0) {
			zp = -d_last/(deri2_time[n] - d_last);
			time = n - 1 + zp; // use 2nd derivative to extract time
		}
		d_last = deri2_time[n];
		//Amplitude
		if ((sum + deri[n]) > temp){
			tut_int += 1;
			sum += deri[n] / fGap;
			hit_conv = sum * hit_det(tut_int);
			if (hit_conv >= fHitThr) hit = kTRUE;
		}
		//Build event
		else{
			if (hit){
				time = time_stamp + time/sample_rate;
				fEnergyList.push_back(temp);
				fTimeList.push_back(time);

				if (fVerbose > 2) {
					vamp.push_back(temp);
					vtime.push_back(time);
				}
			}
			sum = 0;
			tut_int = 0;
			hit_conv = 0;
			hit = kFALSE;
		}
		temp = sum;

		if (fVerbose > 2) {
			ds[n] = temp;
			hc[n] = hit_conv;
		}
	}


	delete[] input;

	if (fVerbose > 2) {
		cout << "[PndEmcPSATmaxAnalyser]:[TMAX] " << "waveform_Ds_" << iWf << " " << size << endl;
		for (int i = 0; i < size; i++)
		{
			cout << "[PndEmcPSATmaxAnalyser]:[TMAX] " << time_stamp + (double)i / sample_rate << " " << ds[i] << endl;
		}
		cout << "[PndEmcPSATmaxAnalyser]:[TMAX] " << "waveform_hitConv_" << iWf << " " << size << endl;
		for (int i = 0; i < size; i++)
		{
			cout << "[PndEmcPSATmaxAnalyser]:[TMAX] " << time_stamp + (double)i / sample_rate << " " << hc[i] << endl;
		}
		cout << "[PndEmcPSATmaxAnalyser]:[TMAX] " << "result " << vamp.size() << endl;
		for (UInt_t i = 0; i < vamp.size(); i++)
		{
			cout << "[PndEmcPSATmaxAnalyser]:[TMAX] " << vamp[i] << " " << vtime[i] << endl;
		}
		iWf++;
	}

	return fEnergyList.size();
}

void PndEmcPSATmaxAnalyser::GetHit(Int_t i, Double_t &Energy, Double_t& Time) {
	Energy = fEnergyList[i];
	Time = fTimeList[i];
}

//
// 20-coeff FIR filting to the raw waveform
//
Double_t* PndEmcPSATmaxAnalyser::fir(Double_t* input, Int_t size){
	
	Double_t* output = new Double_t[size];
	
	Double_t insamp[fTaps - 1 + size];
	memset( insamp, 0, sizeof( insamp ) );
	
	memcpy( &insamp[fTaps - 1], input, size * sizeof(Double_t) );

	Double_t acc;
	const Double_t* coeffp;
	Double_t* inputp;
	
	for (Int_t n = 0; n < size; n++ ) {
		coeffp = fFIRCoeff;
		inputp = &insamp[fTaps - 1 + n];
		acc = 0;
		for (Int_t k = 0; k < fTaps; k++ ) {
			acc += (*coeffp++) * (*inputp--);
		}
		output[n] = acc;
	}
	
	memmove( &insamp[0], &insamp[size], (fTaps - 1) * sizeof(Double_t) );
	

	return output;	
}

Double_t PndEmcPSATmaxAnalyser::hit_det(Int_t tut_int){
	Double_t a = log(fHitVal) / fTutPeak;
	if (tut_int <= fTutPeak) return exp(a * tut_int);
	else return fHitVal * exp(-a * (tut_int - fTutPeak));
}

ClassImp(PndEmcPSATmaxAnalyser)
