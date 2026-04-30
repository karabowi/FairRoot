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

#include "PndEmcPSAFPGAIntegratingAnalyser.h"

#include "PndEmcPSAFPGAFilterCF.h"
#include "PndEmcPSAFPGAFilterMA.h"
#include "PndEmcPSAFPGAFilterDelay.h"
#include "PndEmcPSAFPGALinFitter.h"

#include <string>
#include <iostream>

PndEmcPSAFPGAIntegratingAnalyser::PndEmcPSAFPGAIntegratingAnalyser() :
  PndEmcPSAFPGASampleAnalyser() {
}

PndEmcPSAFPGAIntegratingAnalyser::PndEmcPSAFPGAIntegratingAnalyser(const std::string &fname) :
  PndEmcPSAFPGASampleAnalyser(fname) {
}

PndEmcPSAFPGAIntegratingAnalyser::~PndEmcPSAFPGAIntegratingAnalyser() {
}

void PndEmcPSAFPGAIntegratingAnalyser::reset() {
  local_time = 0;
  Number_of_hits = 0;
  CF_filter->resetToZero();
  MA_triggering->resetToZero();
  pulse_detected = false;
  return;
}

void PndEmcPSAFPGAIntegratingAnalyser::put(float valueToStore) {
	//    std::cout << valueToStore << " ";
	float signal = valueToStore - baseline_value;
	float signal_delayed = Signal_delay->put(signal);
	float cf_value = CF_filter->put(signal);
	float cf_value_delayed = CF_delay->put(cf_value);
	float ma_value = MA_triggering->put(signal);

	if(local_time < 30){ //first 30 samples are baseline
		baseline_value *=local_time;
		baseline_value += valueToStore;
		baseline_value /=local_time+1;
		local_time++;
		//        std::cout << baseline_value << " ";
		return;
	}

	int dT = local_time - rough_pulse_timing;
	if (!pulse_detected) {
		if((cf_value_delayed < 0.0) && (cf_value > 0.0) && (ma_value > analyserParams.hit_threshold)){
			// hit detected
			// CF
			CF_Fitter->reset();
			CF_Fitter->putPoint(local_time, cf_value_delayed);
			cf_zero_crossing_points = 1;
			pulse_detected = true;
			rough_pulse_timing = local_time;
			integral[Number_of_hits] = 0.0;
			amplitude[Number_of_hits] = 0.0;
		}
	} else {
		// CF timing
		if(cf_zero_crossing_points < analyserParams.cf_fitter_length) {
			CF_Fitter->putPoint(local_time, cf_value_delayed);
			cf_zero_crossing_points++;
		}
		// Integral
		integral[Number_of_hits] += signal_delayed;
		// Amplitude
		if((dT < 2*analyserParams.cf_delay) && (signal_delayed > amplitude[Number_of_hits])) {
			amplitude[Number_of_hits] = signal_delayed;
		}
		// Finalize pulse detection
		if ((dT > analyserParams.cf_delay) && (signal_delayed < analyserParams.hit_threshold/analyserParams.ma_trig_M)) {
			pulse_detected = false;
			CF_Fitter->fit();
			double a = CF_Fitter->offset();
			double k = CF_Fitter->slope();
			time[Number_of_hits] = 0.0;
			if(k > 0){ 
				time[Number_of_hits] = -a/k;
			    Number_of_hits++;
std::cout << time[Number_of_hits-1] << std::endl;
            }
		}
	}

	local_time++;
}

ClassImp(PndEmcPSAFPGAIntegratingAnalyser);
