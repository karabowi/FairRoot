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

#ifndef PNDEMCSHASHLYKDIGI_H_
#define PNDEMCSHASHLYKDIGI_H_

#include <PndPersistencyTask.h>
#include "PndEmcDigiWriteoutBuffer.h"
#include "PndEmcSimCrystalCalibrator.h"



class PndEmcHit;
class PndEmcTwoCoordIndex;
class PndEmcWaveform;

class TClonesArray;
class PndEmcShashlykDigiPar;
class PndEmcRecoPar;
class PndEmcGeoPar;
class PndEmcAbsPSA;
class PndEmcAbsPulseshape;
class PndEmcPSAOptimalFilterAnalyser;

/**
 * @brief Task to create digis from waveforms.
 * 
 * This is an alternative to PndEmcWaveformToDigi, which despite its name could be 
 * used for all the EMC, not just BwEndcap.
 * @author Guang Zhao (zhaog@ihep.ac.cn)
 * @ingroup PndEmc
 */
class PndEmcShashlykDigi : public PndPersistencyTask
{
public:
    // Constructors
    PndEmcShashlykDigi(Int_t verbose=0, Bool_t storedigis=kTRUE);
    // Destructor
    virtual ~PndEmcShashlykDigi();

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);

    void SetStorageOfData(Bool_t val); // Method to specify whether digis are stored or not.

    virtual void SetCrystalCalibrator(PndEmcAbsCrystalCalibrator *Cal){
	    fCalibrator = dynamic_cast<PndEmcSimCrystalCalibrator*>(Cal);
	    if(fCalibrator==NULL) {
	    	std::cerr << "-E- in PndEmcShashlykDigi::SetCrystalCalibrator: Calibrator must be a PndEmcSimCrystalCalibrator. External Calibrator setting has no effect" << std::endl;
	    }
    };
	void SingleAPDMode(Bool_t mode = kTRUE) { fSingleAPDMode = mode; } 

protected:
	/** Get parameter containers **/
	virtual void SetParContainers();
private:
	// don't allow copying (-Weffc++)
	PndEmcShashlykDigi(const PndEmcShashlykDigi&);	// no implementation
	PndEmcShashlykDigi& operator= (const PndEmcShashlykDigi&);	// no implementation

private:

	/** Input array of PndEmcWaveforms **/
	TClonesArray* fWaveformArray;  
	
	/** output array of EmcDigis **/
	TClonesArray* fDigiArray;  
	
	Double_t fEnergyDigiThreshold;
	TString fDigiPosMethod;	//"surface" or "depth"
	Double_t fEmcDigiRescaleFactor;
	Double_t fEmcDigiPositionDepthPWO;
	Double_t fEmcDigiPositionDepthShashlyk;

	PndEmcPSAOptimalFilterAnalyser* fOFAnalyser;

	PndEmcSimCrystalCalibrator *fCalibrator;

	PndEmcShashlykDigiPar*    fDigiPar;      //!< Digitisation parameter container
	PndEmcRecoPar*    fRecoPar;      //!< Reconstruction parameter container
	PndEmcGeoPar*    fGeoPar;      //!< Digitisation parameter container

	/** Verbosity level **/
	Int_t fVerbose;
	Bool_t fSingleAPDMode;

	ClassDef(PndEmcShashlykDigi, 1)
};

#endif
