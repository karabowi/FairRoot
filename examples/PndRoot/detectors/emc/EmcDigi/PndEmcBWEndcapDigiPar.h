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
#ifndef PNDEMCBWDIGIPAR_H
#define PNDEMCBWDIGIPAR_H

#include "FairParGenericSet.h"
#include "FairParamList.h"

#include "TString.h"
#include "TArrayD.h"

/**
 * @brief parameter set of Emc digitisation
 * @ingroup PndEmc
 */
class PndEmcBWEndcapDigiPar : public FairParGenericSet
{
	public:
	    PndEmcBWEndcapDigiPar(const char *name = "PndEmcBWEndcapDigiPar",
			  		  const char *title = "Emc digi parameter",
			  		  const char *context = "TestDefaultContext");
	    ~PndEmcBWEndcapDigiPar(void){};

		/* signal generator */
		Int_t    GetUse_photon_statistic()  { return fUse_photon_statistic; }
		Double_t GetQuantumEfficiencyAPD()  { return fQuantumEfficiencyAPD; }
		Double_t GetExcessNoiseFactorAPD()  { return fExcessNoiseFactorAPD; }
		Double_t GetSensitiveAreaAPD()      { return fSensitiveAreaAPD; }
		Double_t GetDetectedPhotonsPerMeV() { return fDetectedPhotonsPerMeV; }

	    Double_t GetTimeBeforeHit()         { return fTimeBeforeHit; }
	    Double_t GetTimeAfterHit()          { return fTimeAfterHit; }
		Double_t GetActiveTimeIncrement()   { return fActiveTimeIncrement; }
	    Double_t GetWfCutOffEnergy()        { return fWfCutOffEnergy; } //GeV  //0.001
	    Double_t GetSampleRate()            { return fSampleRate; }		 //ns^-1 <-> GHz    //0.08
	    Int_t    GetNBits()                 { return fNBits; }	         //1    //14
		Double_t GetEnCalib()               { return fEnCalib; }         //GeV/channel
	    Double_t GetPulseshapeTau()         { return fPulseshapeTau; }   //ns   //68.7
	    Double_t GetPulseshapeN()           { return fPulseshapeN; }	 //N    //1.667
	    Double_t GetHGLGRatio()             { return fHGLGRatio; }	//10.5
	    Double_t GetADCNoiseHigh()          { return fADCNoiseHigh; } 
	    Double_t GetADCNoiseLow()           { return fADCNoiseLow; } 
	    Double_t GetFENoiseHigh()           { return fFENoiseHigh; } 
	    Double_t GetFENoiseLow()            { return fFENoiseLow; }
	    Double_t GetPsSigmaHigh()           { return fPsSigmaHigh; }
	    Double_t GetPsSigmaLow()            { return fPsSigmaLow; }
	    Double_t GetGesSigmaHigh()          { return fGesSigmaHigh; }
	    Double_t GetGesSigmaLow()           { return fGesSigmaLow; }
		const TArrayD& GetPSFreq()          { return fPSFreq; }
		const TArrayD& GetPSPowerHigh()     { return fPSPowerHigh; }
		const TArrayD& GetPSPowerLow()      { return fPSPowerLow; }

		/* feature extraction */
		Double_t GetSignalOverflowHigh()    { return fSignalOverflowHigh; }
		const TArrayD&  GetFIRCoeff()       { return fFIRCoeff; }
		Int_t    GetTmaxTaps()              { return fTmaxTaps; }
		Int_t    GetTmaxGap()               { return fTmaxGap; }
		Double_t GetTmaxHitThresholdHigh()  { return fTmaxHitThresholdHigh; }
		Double_t GetTmaxHitThresholdLow()   { return fTmaxHitThresholdLow; }
		Double_t GetTmaxTutPeak()           { return fTmaxTutPeak; }
		Double_t GetTmaxHitVal()            { return fTmaxHitVal; }
		Double_t GetEnergyDigiThreshold()   { return fEnergyDigiThreshold; }



	    void putParams(FairParamList *list);
	    Bool_t getParams(FairParamList *list);
	
	private :
	    // Empty method, WHY is this Called in constructor(FIXME)
	    //void clear(void){};

		Int_t    fUse_photon_statistic;
		Double_t fQuantumEfficiencyAPD;
		Double_t fExcessNoiseFactorAPD;
		Double_t fSensitiveAreaAPD;
		Double_t fDetectedPhotonsPerMeV;

		Double_t fTimeBeforeHit;
		Double_t fTimeAfterHit;
		Double_t fActiveTimeIncrement;
		Double_t fWfCutOffEnergy;
		Double_t fSampleRate;
		Int_t    fNBits;
		Double_t fEnCalib;
		Double_t fPulseshapeTau;
		Double_t fPulseshapeN;
		Double_t fHGLGRatio;
		Double_t fADCNoiseHigh;
		Double_t fADCNoiseLow;
		Double_t fFENoiseHigh;
		Double_t fFENoiseLow;
		Double_t fPsSigmaHigh;
		Double_t fPsSigmaLow;
		Double_t fGesSigmaHigh;
		Double_t fGesSigmaLow;
		TArrayD  fPSFreq;
		TArrayD  fPSPowerHigh;
		TArrayD  fPSPowerLow;

		Double_t fSignalOverflowHigh;
        TArrayD  fFIRCoeff;
		Int_t    fTmaxTaps;
		Int_t    fTmaxGap;
		Double_t fTmaxHitThresholdHigh;
		Double_t fTmaxHitThresholdLow;
		Double_t fTmaxTutPeak;
		Double_t fTmaxHitVal;
		Double_t fEnergyDigiThreshold;

		ClassDef(PndEmcBWEndcapDigiPar, 1)
};

#endif
