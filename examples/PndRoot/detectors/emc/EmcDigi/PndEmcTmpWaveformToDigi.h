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
//      Class PndEmcTmpWaveformToDigi. Module to take the hit list for the
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
#pragma once
#ifndef PndEmcTmpWaveformToDigi_H
#define PndEmcTmpWaveformToDigi_H

#include <PndPersistencyTask.h>
#include <string>
//#include <vector>

class PndEmcHit;
class PndEmcTwoCoordIndex;
class PndEmcWaveform;

class TClonesArray;
class PndEmcDigiPar;
class PndEmcRecoPar;

/**
 * @brief currently not used, not in CMakeLists.txt
 * @ingroup PndEmc
 */
class PndEmcTmpWaveformToDigi : public PndPersistencyTask {

 public:
  // Constructors

  PndEmcTmpWaveformToDigi(Int_t verbose = 0, Bool_t storedigis = kTRUE);

  // Destructor

  virtual ~PndEmcTmpWaveformToDigi();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  void SetStorageOfData(Bool_t val); // Method to specify whether digis are stored or not.

 private:
  /** Input array of PndEmcWaveforms **/
  TClonesArray *fWaveformArray;

  /** output array of EmcDigis **/
  TClonesArray *fDigiArray;

  Double_t fSampleRate;
  Double_t fEnergyDigiThreshold;

  std::string fDigiPosMethod; // "surface" or "depth"
  Double_t fEmcDigiRescaleFactor;
  Double_t fEmcDigiPositionDepth;

  PndEmcDigiPar *fDigiPar; /** Digitisation parameter container **/
  PndEmcRecoPar *fRecoPar; /** Reconstruction parameter container **/
  /** Get parameter containers **/
  virtual void SetParContainers();
 
  /** Verbosity level **/
  Int_t fVerbose;

  ClassDef(PndEmcTmpWaveformToDigi, 1);
};

#endif
