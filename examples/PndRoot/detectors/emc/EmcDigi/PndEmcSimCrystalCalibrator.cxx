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

//---------------------------------------------------------
// Description:
// 	Calibrator for Emc Crystals for use in Simulations
//
// 	Christian Hammann
// 	31.7.2012

//---------------------------------------------------------

#include "PndEmcSimCrystalCalibrator.h"
#include <iostream>

using std::map;
using std::pair;
using std::make_pair;

PndEmcSimCrystalCalibrator::PndEmcSimCrystalCalibrator(Int_t verbose) :PndEmcAbsCrystalCalibrator(), fRandomCal(kFALSE), fRandomSigma(0), fVerbose(verbose) {
};

void PndEmcSimCrystalCalibrator::SetCalibration(Int_t ModId, Double_t cal, Int_t SignalNr, Double_t overflow){
	if((int)fModuleCalibrationMaps.size() < SignalNr+1) {
		fModuleCalibrationMaps.resize(SignalNr+1);
		fCrystalCalibrationMaps.resize(SignalNr+1);
	}	
		fModuleCalibrationMaps[SignalNr].insert(pair<Int_t, pair<Double_t,Double_t> >(ModId, make_pair(1.0/cal, overflow)));
 }

void PndEmcSimCrystalCalibrator::CreateRandomCalibration(Double_t ){ // Sigma //[R.K.03/2017] unused variable(s)
	//not implemented yet
};

Bool_t PndEmcSimCrystalCalibrator::Init(){
	return kTRUE;
};

PndEmcAbsCrystalCalibrator::CalibrationStatus_t PndEmcSimCrystalCalibrator::Calibrate(Double_t &Energy, Long_t DetId, Int_t SignalNr){
	Int_t ModId = (Int_t)(DetId / 1e8);


	CalibrationStatus_t returnValue = kCALERROR;

	if((int)fModuleCalibrationMaps.size() <= SignalNr) {
		if(fVerbose) std::cout << "no calibration found for signal number " << SignalNr << std::endl;
		returnValue = kCALMISSING;
	} else {

		if(fRandomCal) {
			map<Long_t,Double_t>::iterator it2;
			it2 = fCrystalCalibrationMaps[SignalNr].find(DetId);
			if(it2!=fCrystalCalibrationMaps[SignalNr].end()){
				Energy*=it2->second;
			} else {
				return kCALMISSING;
			}
		}


		map<Int_t, pair<Double_t, Double_t> >::iterator it;
		it=fModuleCalibrationMaps[SignalNr].find(ModId);
		if(it!=fModuleCalibrationMaps[SignalNr].end()) {
			Double_t calib = it->second.first;
			Double_t overflow = it->second.second;
			
			if(fVerbose) std::cout << "Calibrating signal " << SignalNr << "\t calib constant: " << calib << "\t overflow: " << overflow << std::endl;
			if(overflow>0 && Energy>=overflow) {
				returnValue = kCALOVERFLOW;
			} else {
				returnValue = kCALOK;
			}
			Energy*=calib;
		}
	}
	return returnValue;
}


ClassImp(PndEmcSimCrystalCalibrator);
