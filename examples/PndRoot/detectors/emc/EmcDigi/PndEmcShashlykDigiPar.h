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
#ifndef PNDEMCSHASHLYKDIGIPAR_H
#define PNDEMCSHASHLYKDIGIPAR_H

#include "FairParGenericSet.h"
#include "FairParamList.h"

#include "TString.h"
#include "TArrayD.h"

/**
 * @brief parameter set of Emc digitisation
 * @ingroup PndEmc
 */
class PndEmcShashlykDigiPar : public FairParGenericSet
{
public:
    PndEmcShashlykDigiPar(const char *name = "PndEmcShashlykDigiPar",
                    const char *title = "Emc digi parameter",
                    const char *context = "TestDefaultContext");
    ~PndEmcShashlykDigiPar(void){};

    /* signal generator */
    Int_t    GetUse_photon_statistic()  { return fUse_photon_statistic; }
    Double_t GetQuantumEfficiency()  { return fQuantumEfficiency; }
    Double_t GetExcessNoiseFactor()  { return fExcessNoiseFactor; }
    Double_t GetDetectedPhotonsPerMeV() { return fDetectedPhotonsPerMeV; }
	Double_t GetSamplingFactor() { return fSamplingFactor; }

    Double_t GetTimeBeforeHit()         { return fTimeBeforeHit; }
    Double_t GetTimeAfterHit()          { return fTimeAfterHit; }
    Double_t GetActiveTimeIncrement()   { return fActiveTimeIncrement; }
    Double_t GetWfCutOffEnergy()        { return fWfCutOffEnergy; } //GeV  //0.001
    Double_t GetSampleRate()            { return fSampleRate; }         //ns^-1 <-> GHz    //0.08
    Int_t    GetNBits()                 { return fNBits; }             //1    //14
    Double_t GetEnCalib()               { return fEnCalib; }         //MeV/channel
    Double_t GetPulseshapeMu()         { return fPulseshapeMu; }   //ns   //68.7
    Double_t GetPulseshapeSigma()           { return fPulseshapeSigma; }     //N    //1.667
    Double_t GetNoiseWidth()            { return fNoiseWidth; }

    /* feature extraction */
	Double_t GetCFFraction() { return fCF_fraction; }
	Int_t GetCFTap() { return fCF_tap; }
	Int_t GetCFNSamples() { return fCF_nsamples; }
	const TArrayD& GetCFTCorr() { return fCF_tcorr; }
	Int_t GetOFB0() { return fOF_b0; }
	Int_t GetOFM() { return fOF_m; }
	const TArrayD& GetOFA() { return fOF_a; }
	const TArrayD& GetOFB() { return fOF_b; }
	Double_t GetADCHitThreshold() { return fADCHitThreshold; }
	Double_t GetEnergyDigiThreshold() { return fEnergyDigiThreshold; }

    void putParams(FairParamList *list);
    Bool_t getParams(FairParamList *list);

private :
    // Empty method, WHY is this Called in constructor(FIXME)
    //void clear(void){};

    Int_t    fUse_photon_statistic;
    Double_t fQuantumEfficiency;
    Double_t fExcessNoiseFactor;
    Double_t fDetectedPhotonsPerMeV;
	Double_t fSamplingFactor;

    Double_t fTimeBeforeHit;
    Double_t fTimeAfterHit;
    Double_t fActiveTimeIncrement;
    Double_t fWfCutOffEnergy;
    Double_t fSampleRate;
    Int_t    fNBits;
    Double_t fEnCalib;
    Double_t fPulseshapeMu;
    Double_t fPulseshapeSigma;
    Double_t fNoiseWidth;

    Double_t fCF_fraction;
    Int_t fCF_tap;
    Int_t fCF_nsamples;
    TArrayD fCF_tcorr;
    Int_t fOF_b0;
    Int_t fOF_m;
    TArrayD fOF_a;
    TArrayD fOF_b;
    Double_t fADCHitThreshold;
	Double_t fEnergyDigiThreshold;

    ClassDef(PndEmcShashlykDigiPar, 1)
};

#endif
