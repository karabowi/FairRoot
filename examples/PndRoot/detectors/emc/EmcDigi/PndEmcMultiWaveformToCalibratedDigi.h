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

//----------------------------------------------------------------------
// File and Version Information:
//      $Id: Exp $
//
// Description:
//      Class PndEmcMultiWaveformToCalibratedDigi. Module to take the hit list for the 
//      calorimeter and make ADC waveforms from them.
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI		
//		
// Author List:
//      Phil Strother                  Original Author
// Dima Melnichuk - adaption for PANDA				
//
// Copyright Information:
//      Copyright (C) 1996             Imperial College
//
//----------------------------------------------------------------------
//#pragma once
#ifndef PndEmcMultiWaveformToCalibratedDigi_H
#define PndEmcMultiWaveformToCalibratedDigi_H

#include <PndPersistencyTask.h>
#include <string>		
#include <map>
#include "TString.h"
//#include <vector>

class PndEmcHit;
class PndEmcTwoCoordIndex;
class PndEmcMultiWaveform;
class PndEmcWaveform;

class TClonesArray;
class PndEmcDigiPar;
class PndEmcRecoPar;
class PndEmcAbsPSA;
class PndEmcAbsPulseshape;

/**
 * @brief Module to take the hit list for the 
 *      calorimeter and make ADC waveforms from them.
 * @ingroup PndEmc
 * @author Ch. Hammann <chammann@hiskp.uni-bonn.de>
 */
class PndEmcMultiWaveformToCalibratedDigi : public PndPersistencyTask
{

public:

  // Constructors

  PndEmcMultiWaveformToCalibratedDigi(Int_t verbose=0, Bool_t storedigis=kTRUE);

  // Destructor

  virtual ~PndEmcMultiWaveformToCalibratedDigi();

  /** Virtual method Init **/
  virtual InitStatus Init();

  void SetCalibrationFile(const char* calibrationfilename){fCalibrationFileName  = calibrationfilename;};

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  void SetStorageOfData(Bool_t val); // Method to specify whether digis are stored or not.

  virtual void SetPSAAlgorithm(PndEmcAbsPSA *psa){fpsaAlgorithm=psa;}
  virtual void SetPSAAlgorithmPMT(PndEmcAbsPSA *psa){fpsaAlgorithm_pmt=psa;}

private:
	void ReadCalibrationFile();
	/** Input array of PndEmcMultiWaveforms **/
	TClonesArray* fWaveformArray;  
	
	/** output array of EmcDigis **/
	TClonesArray* fDigiArray;  
	
	Double_t fSampleRate;
	Double_t fSampleRate_PMT;
	Double_t fEnergyDigiThreshold;
	Double_t fASIC_Shaping_int_time;      //s
	Double_t fPMT_Shaping_int_time;    //s
	Double_t fPMT_Shaping_diff_time;    //s
	Double_t fCrystal_time_constant;  //s
	Double_t fShashlyk_time_constant;  //s
	Int_t fNumber_of_samples_in_waveform;
	Int_t fNumber_of_samples_in_waveform_pmt;
	
  	//std::string fDigiPosMethod;// "surface" or "depth"
	TString fDigiPosMethod;
	Double_t fEmcDigiRescaleFactor;
	Double_t fEmcDigiPositionDepthPWO;
	Double_t fEmcDigiPositionDepthShashlyk;
	
	PndEmcAbsPulseshape* fPulseshape;
	PndEmcAbsPulseshape* fPulseshape_pmt;
	PndEmcAbsPSA *fpsaAlgorithm;
	PndEmcAbsPSA *fpsaAlgorithm_pmt;

	PndEmcDigiPar*    fDigiPar;      /** Digitisation parameter container **/
	PndEmcRecoPar*    fRecoPar;      /** Reconstruction parameter container **/
	/** Get parameter containers **/
	virtual void SetParContainers();
	
	/** Verbosity level **/
	Int_t fVerbose;

	Bool_t fStoreDigis;
	
	Double_t fWfNormalisation; // Waveform normalisation constant
	Double_t fWfNormalisation_pmt;
	Double_t fWfNormalisation_proto192;
	
	std::map<Int_t,Double_t> fCalibrationMap;
	std::map<Int_t,Double_t> fGainMap;
	TString fCalibrationFileName;

	ClassDef(PndEmcMultiWaveformToCalibratedDigi,2);
	
	
};

#endif





