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
//      Class PndEmcMakeDigi. This class takes array of EmcHit's and produce
// an array of EmcDigis.
// It is convenient to study reconstruction algoritms without
// disturbance from digitization
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI		
//----------------------------------------------------------------------
//#pragma once
#ifndef PndEmcMakeDigi_H
#define PndEmcMakeDigi_H

#include <PndPersistencyTask.h>
#include <string>

class PndEmcTwoCoordIndex;
class PndEmcHit;
class PndEmcDigi;

class TClonesArray;
class PndEmcGeoPar;
class PndEmcDigiPar;
class PndEmcRecoPar;

/**
 * @brief Task to create PndEmcDigi from PndEmcHit.
 * 
 * This task creates digis directly from hits, bypassing the creation and 
 * analysis of waveforms.
 * @ingroup PndEmc
 */
class PndEmcMakeDigi : public PndPersistencyTask
{
public:
  // Constructors
  PndEmcMakeDigi(Bool_t storedigis=kTRUE);
  // Destructor
  virtual ~PndEmcMakeDigi();

  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);

protected:  
  PndEmcDigi* AddDigi(Int_t trackID,Int_t detID, Float_t energy, Float_t time, Int_t hitIndex);
  void SetStorageOfData(Bool_t val); //!< Method to specify whether digis are stored or not.
	/** Get parameter containers **/
	virtual void SetParContainers();
private:
	// don't allow copying (-Weffc++)
	PndEmcMakeDigi(const PndEmcMakeDigi&);	// no implementation
	PndEmcMakeDigi& operator= (const PndEmcMakeDigi&);	// no implementation

public:  // Set and Get methods
  void SetDigiPosMethod(const std::string& digiPosMethod);
  const std::string& GetDigiPosMethod() const;
  
private:
	/** Input array of EmcWaveforms **/
	TClonesArray* fHitArray;  
	/** output array of EmcDigis **/
	TClonesArray* fDigiArray;  
	
	Double_t fThreshold;
	
  	std::string fDigiPosMethod;// "surface" or "depth"
	Double_t fEmcDigiRescaleFactor;
	Double_t fEmcDigiPositionDepthPWO;
	Double_t fEmcDigiPositionDepthShashlyk;
	
	Int_t fUseDigiEffectiveSmearing;
	Double_t fDetectedPhotonsPerMeV;
	Double_t fNPhotoElectronsPerMeVAPDBarrel;
	Double_t fNPhotoElectronsPerMeVAPDBWD;
	Double_t fNPhotoElectronsPerMeVVPT;
	Double_t fSensitiveAreaAPD; //mm^2
	Double_t fSensitiveAreaVPT; //mm^2
	Double_t fQuantumEfficiencyAPD;
	Double_t fQuantumEfficiencyVPT;
	Double_t fExcessNoiseFactorAPD;
	Double_t fExcessNoiseFactorVPT;
	Double_t fIncoherent_elec_noise_width_GeV_APD; //GeV
	Double_t fIncoherent_elec_noise_width_GeV_VPT; //GeV

	Int_t fMapVersion;

	PndEmcGeoPar*     fGeoPar;       //!< Geometry parameter container
	PndEmcDigiPar*    fDigiPar;      //!< Digitisation parameter container
	PndEmcRecoPar*    fRecoPar;      //!< Reconstruction parameter container
	
	Bool_t fStoreDigis;

	Int_t fNrOfEvents;
	Int_t evtCounter;
	Int_t nDigiProg;

	ClassDef(PndEmcMakeDigi,1);
};

#endif
