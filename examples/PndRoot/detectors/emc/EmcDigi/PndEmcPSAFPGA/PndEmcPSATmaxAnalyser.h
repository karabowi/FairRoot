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

//-----------------------------------------------------------
// Description:
//     	Class of TMAX filter for feature extraction
//      Guang Zhao
//-----------------------------------------------------------

#ifndef PNDEMCPSATMAXANALYSER_HH
#define PNDEMCPSATMAXANALYSER_HH

#include "PndEmcAbsPSA.h"

#include "TString.h"

class PndEmcWaveform;

class PndEmcPSATmaxAnalyser : public PndEmcAbsPSA
{

public:
    PndEmcPSATmaxAnalyser(const Double_t* coeff, Int_t taps, Int_t gap, Int_t samplingrate, 
        Double_t hit_threshold, Double_t tut_peak, Double_t hit_val, Int_t verbose);
    virtual ~PndEmcPSATmaxAnalyser();

    virtual Int_t Process(const PndEmcWaveform *waveform);

    virtual void GetHit(Int_t i, Double_t &Energy, Double_t &Time);

    virtual void Reset();


private:
    Double_t* fir(Double_t*, Int_t);
    Double_t hit_det(Int_t tut_int);

    Int_t fTaps;
    Int_t fGap;
    Double_t fHitThr;
    Double_t fTutPeak;
    Double_t fHitVal;
    const Double_t *fFIRCoeff;
    Double_t fTimeStep;

    std::vector<Double_t> fEnergyList;
    std::vector<Double_t> fTimeList;

    Int_t fVerbose;

    ClassDef(PndEmcPSATmaxAnalyser, 1)
};

#endif

