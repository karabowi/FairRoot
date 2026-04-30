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
// Author List:
//      Phil Strother                  	Original author
// 	Dima Melnichuk 			- adaption for PANDA
// 	Philippp Mahlberg		- integrtion in timebased simulation concept
//  Guang Zhao          - implementation for bwec
//----------------------------------------------------------------------

//#pragma once
#ifndef PndEmcBWEndcapTimebasedWaveforms_H
#define PndEmcBWEndcapTimebasedWaveforms_H

#include <PndPersistencyTask.h>

class PndEmcMapper;
class PndEmcBWEndcapDigiPar;
class PndEmcGeoPar;
class PndEmcWaveformBuffer;
class PndEmcAbsWaveformSimulator;
class PndEmcMultiWaveformSimulator;
class PndEmcAbsWaveformModifier;
class PndEmcAbsPulseshape;
class TClonesArray;

#define MULTI

/**
 * @brief Taks to create waveforms from hits.
 * 
 * This is an alternative to PndEmcHitsToWaveform, which despite its name could be 
 * used for all the EMC, not just BwEndcap.
 * @author Guang Zhao (zhaog@ihep.ac.cn)
 * @ingroup PndEmc
 */
class PndEmcBWEndcapTimebasedWaveforms : public PndPersistencyTask
{
public:
  // Constructors
  PndEmcBWEndcapTimebasedWaveforms(Int_t verbose=0, Bool_t storewaves=kFALSE);
  // Destructor
  virtual ~PndEmcBWEndcapTimebasedWaveforms();

  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);

  void RunTimebased(Bool_t timebased = kTRUE) { fActivateBuffering = timebased; }
  void SetExternalSimulator(PndEmcAbsWaveformSimulator* simulator) { fExternalSimulator = simulator; }
  void SetStorageOfData(Bool_t storeWaves = kTRUE) {SetPersistency(storeWaves);} //!< Method to specify whether waveforms are stored or not.
  void StoreDataClass(Bool_t storeData = kTRUE) { fStoreDataClass = storeData; }
  void SingleAPDMode(Bool_t mode = kTRUE) { fSingleAPDMode = mode; }
  void NoiseMode(Int_t mode = 1) { fNoiseMode = mode; } // 1 for reduced ifft, 0 for full ifft
  void RunTestMode(Double_t energy = 0.01) { fTestMode = kTRUE; fTestEnergy = energy; }

protected:  
  /** Get parameter containers **/
  virtual void SetParContainers();

private:
   // don't allow copying (-Weffc++)
   PndEmcBWEndcapTimebasedWaveforms(const PndEmcBWEndcapTimebasedWaveforms&);	// no implementation
   PndEmcBWEndcapTimebasedWaveforms& operator= (const PndEmcBWEndcapTimebasedWaveforms&);	// no implementation

private:

  /** Input array of PndEmcHits **/
  TClonesArray* fHitArray;

  /** Output array of PndEmcWaveforms **/
  PndEmcWaveformBuffer* fWaveformBuffer;

  Bool_t fStoreDataClass;
  Bool_t fActivateBuffering;

  PndEmcBWEndcapDigiPar*    fDigiPar; 	//!< Digitisation parameter container // to be defined
  PndEmcGeoPar*     fGeoPar;       	//!< Geometry parameter container

  Bool_t fUse_photon_statistic;
  Double_t fNPhotoElectronsPerMeV;
  Double_t fExcessNoiseFactor;

  PndEmcAbsWaveformSimulator* fExternalSimulator;
  PndEmcMultiWaveformSimulator* fAPD_LOWHIGH;

  Bool_t fSingleAPDMode;
  Int_t fNoiseMode;
  Bool_t fTestMode;
  Double_t fTestEnergy;

  ClassDef(PndEmcBWEndcapTimebasedWaveforms, 1)
};

#endif
