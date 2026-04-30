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

#ifndef PNDEMCFWENDCAPDIGI_H_
#define PNDEMCFWENDCAPDIGI_H_

#include <PndPersistencyTask.h>
#include "PndEmcHighLowPSA.h"
#include "PndEmcDigiWriteoutBuffer.h"
#include "PndEmcSimCrystalCalibrator.h"
#include "PndEmcPSAFPGASampleAnalyser.h"

#include "FairTask.h"

#include "TRandom.h"

#include <string>
#include <fstream>
//#include <vector>

class PndEmcHit;
class PndEmcTwoCoordIndex;
class PndEmcWaveform;

class TClonesArray;
class PndEmcFWEndcapDigiPar;
class PndEmcRecoPar;
class PndEmcGeoPar;
class PndEmcAbsPSA;
class PndEmcAbsPulseshape;
class PndEmcFWEndcapTimebasedWaveforms;

/**
 * @brief Task to create digis from waveforms.
 * 
 * This is an alternative to PndEmcWaveformToDigi, which despite its name could be 
 * used for all the EMC, not just FwEndcap.
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup PndEmc
 */
class PndEmcFWEndcapDigi : public PndPersistencyTask
{
public:
  // Constructors
  PndEmcFWEndcapDigi(Int_t verbose=0, Bool_t storedigis=kTRUE);
  // Destructor
  virtual ~PndEmcFWEndcapDigi();

  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);

  void SetStorageOfData(Bool_t val); // Method to specify whether digis are stored or not.
  void RunTimeBased(){fTimeOrderedDigi = kTRUE;};

  void SetPSAAlgorithms(PndEmcAbsPSA* highgain, PndEmcAbsPSA* lowgain) {
	  fHighgainPSA = dynamic_cast<PndEmcPSAFPGASampleAnalyser*>(highgain);
	  fLowgainPSA = dynamic_cast<PndEmcPSAFPGASampleAnalyser*>(lowgain);

    if (fHighgainPSA == nullptr || fLowgainPSA == nullptr) {
      std::cerr << "-E- in PndEmcFWEndcapDigi::SetPSAAlgorithms: PSA must be a PndEmcPSAFPGASampleAnalyser. External PSA setting has no effect" << std::endl;
    }
  } 

  virtual void SetCrystalCalibrator(PndEmcAbsCrystalCalibrator *Cal){
	  fCalibrator = dynamic_cast<PndEmcSimCrystalCalibrator*>(Cal);
    if (fCalibrator == nullptr) {
      std::cerr << "-E- in PndEmcFWEndcapDigi::SetCrystalCalibrator: Calibrator must be a PndEmcSimCrystalCalibrator. External Calibrator setting has no effect" << std::endl;
    }
  };
protected:
	/** Get parameter containers **/
	virtual void SetParContainers();
private:
	// don't allow copying (-Weffc++)
	PndEmcFWEndcapDigi(const PndEmcFWEndcapDigi&);	// no implementation
	PndEmcFWEndcapDigi& operator= (const PndEmcFWEndcapDigi&);	// no implementation

private:

	/** Input array of PndEmcWaveforms **/
	TClonesArray* fWaveformArray;  
	
	/** output array of EmcDigis **/
	TClonesArray* fDigiArray;  
        TClonesArray* fDigiArray2;  
	 TClonesArray* fHitArray;

	Double_t fEnergyDigiThreshold;
	TString fDigiPosMethod;	//"surface" or "depth"
	Double_t fEmcDigiRescaleFactor;
	Double_t fEmcDigiPositionDepthPWO;
	Double_t fEmcDigiPositionDepthShashlyk;

	PndEmcPSAFPGASampleAnalyser* fHighgainPSA;
	PndEmcPSAFPGASampleAnalyser* fLowgainPSA;
	PndEmcHighLowPSA fHighLowPSA;

	PndEmcSimCrystalCalibrator *fCalibrator;

	PndEmcFWEndcapDigiPar*    fDigiPar;      //!< Digitisation parameter container
	PndEmcRecoPar*    fRecoPar;      //!< Reconstruction parameter container
	PndEmcGeoPar*    fGeoPar;      //!< Digitisation parameter container

	/** Verbosity level **/
	Int_t fVerbose;

	Int_t fNrOfEvents;
	Int_t evtCounter;
	Int_t nFwDigiProg;

	Bool_t fStoreDigis;
	Bool_t fTimeOrderedDigi; ///<set to kTRUE to use the time ordering of the output data.

	std::ofstream Energy;

	Double_t GetEnergyDepTimeResolution(Double_t); ///<Returns energy-dependent time resolution, used to assign timestamp to EmcDigis

	ClassDef(PndEmcFWEndcapDigi,1);
};

#endif
