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
//      Class PndEmcHitsToWaveform. Module to take the hit list for the
//      calorimeter and make ADC waveforms from them.
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Author List:
//      Phil Strother                  Original author
// 		 Dima Melnichuk - adaption for PANDA
// Copyright Information:
//      Copyright (C) 1996             Imperial College
//----------------------------------------------------------------------
//#pragma once
#ifndef PndEmcHitsToWaveform_H
#define PndEmcHitsToWaveform_H

#include <PndPersistencyTask.h>
#include "PndEmcWaveform.h"
#include "PndEmcTwoCoordIndex.h"

// class PndEmcTwoCoordIndex;
// class PndEmcWaveform;
class PndEmcMapper;

class TClonesArray;
class PndEmcDigiPar;
class PndEmcGeoPar;
class PndEmcWaveformWriteoutBuffer;
class PndEmcAbsPulseshape;

/**
 * @brief Takes list of PndEmcHits and creates PndEmcWaveform
 * @ingroup PndEmc
 */
class PndEmcHitsToWaveform : public PndPersistencyTask {
 public:
  // Constructors
  PndEmcHitsToWaveform(Int_t verbose = 0, Bool_t storewaves = kTRUE);
  // Destructor
  virtual ~PndEmcHitsToWaveform();

  virtual InitStatus Init();
  virtual void Exec(Option_t *opt);

  // PndEmcWaveform * AddWaveform(Int_t detId,Int_t hitIndex,Int_t numOfSamples);
  PndEmcWaveform *AddWaveform(Int_t detId, Int_t hitIndex, Int_t numOfSamples, Double_t timeStamp, Double_t sampleRate, Int_t MCTrackID);

  void SetStorageOfData(Bool_t val); // Method to specify whether waveforms are stored or not.

  void RunTimeBased() { fTimeOrderedWaveform = kTRUE; }

  void FinishTask();

 protected:
  /** Get parameter containers **/
  virtual void SetParContainers();

 private:
  // don't allow copying (-Weffc++)
  PndEmcHitsToWaveform(const PndEmcHitsToWaveform &);            // no implementation
  PndEmcHitsToWaveform &operator=(const PndEmcHitsToWaveform &); // no implementation

 private:
  /** Input array of PndEmcHits **/
  TClonesArray *fHitArray;

  /** Output array of PndEmcWaveforms **/
  TClonesArray *fWaveformArray;
  PndEmcWaveformWriteoutBuffer *fDataBuffer;
  Bool_t fTimeOrderedWaveform;

  Double_t fOneBitResolution;
  Double_t fOneBitResolutionBW;
  Double_t fOneBitResolutionPMT;
  Double_t fOneBitResolutionFWD;

  Int_t fNBits;
  Double_t fDetectedPhotonsPerMeV;
  Double_t fDetectedPhotonsPerMeV_PMT;
  Double_t fNPhotoElectronsPerMeVAPDBarrel;
  Double_t fNPhotoElectronsPerMeVAPDBWD;
  Double_t fNPhotoElectronsPerMeVVPT;
  Double_t fNPhotoElectronsPerMeVPMT;
  Double_t fSensitiveAreaAPD; // mm^2
  Double_t fSensitiveAreaVPT; // mm^2
  Double_t fQuantumEfficiencyAPD;
  Double_t fQuantumEfficiencyVPT;
  Double_t fQuantumEfficiencyPMT;
  Double_t fExcessNoiseFactorAPD;
  Double_t fExcessNoiseFactorVPT;
  Double_t fExcessNoiseFactorPMT;
  Double_t fIncoherent_elec_noise_width_GeV_APD; // GeV
  Double_t fIncoherent_elec_noise_width_GeV_VPT; // GeV
  Double_t fEnergyRange;                         // GeV
  Double_t fEnergyRangeBW;                       // GeV
  Double_t fFirstSamplePhase;
  Int_t fNumber_of_samples_in_waveform;
  Int_t fNumber_of_samples_in_waveform_pmt;
  Int_t fNumber_of_samples_in_waveform_fwd;
  Double_t fASIC_Shaping_int_time;  // s
  Double_t fPMT_Shaping_int_time;   // s
  Double_t fPMT_Shaping_diff_time;  // s
  Double_t fFWD_Shaping_int_time;   // s
  Double_t fFWD_time_constant;      // s
  Double_t fCrystal_time_constant;  // s
  Double_t fShashlyk_time_constant; // s
  Double_t fShashlykSamplingFactor;
  Double_t fSampleRate;
  Double_t fSampleRate_PMT;
  Double_t fSampleRate_FWD;
  Int_t fUse_shaped_noise;
  Int_t fUse_photon_statistic;
  Int_t fNoiseAllChannels;
  Int_t fMapVersion;

  Double_t fFirstADCBinTime;

  Double_t fGevPeakAnalogue;
  Double_t fGevPeakAnalogue_PMT;
  Double_t fGevPeakAnalogue_FWD;

  PndEmcDigiPar *fDigiPar; /** Digitisation parameter container **/
  PndEmcGeoPar *fGeoPar;   /** Geometry parameter container **/

  /** Verbosity level **/
  Int_t fVerbose;

  // counters for task
  Int_t HowManyHit;
  Int_t nWaveformProduced;
  Int_t HowManyEventPileup;

  // pulse shapes
  PndEmcAbsPulseshape *pulseshape1;
  PndEmcAbsPulseshape *pulseshape2;
  PndEmcAbsPulseshape *pulseshape3;

  ClassDef(PndEmcHitsToWaveform, 2);
};

#endif
