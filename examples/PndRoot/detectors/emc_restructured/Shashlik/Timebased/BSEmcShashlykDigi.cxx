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

#include "BSEmcShashlykDigi.h"

#include "BSEmcPSAOptimalFilterAnalyser.h"
#include "BSEmcShashlykDigiPar.h"
#include "BSEmcShashlykPulseshape.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TStopwatch.h"
#include "TF1.h"
#include "TObject.h"
#include "TList.h"

#include <iostream>
#include <vector>
#include <utility>

using std::cout;
using std::endl;

BSEmcShashlykDigi::BSEmcShashlykDigi(const std::string &t_detectorname, Bool_t t_storedigis) : BSEmcExtractDigisFromWaveforms<BSEmcShashlykDigiPar>(t_detectorname, t_storedigis)
{
  SetPersistency(t_storedigis);
}

//--------------
// Destructor --
//--------------
BSEmcShashlykDigi::~BSEmcShashlykDigi() {}

/**
 * @brief Defines/Instantiates the Pulse Shape Analysis used for the
 * Shashlyk used in the Feature Extraction
 */
void BSEmcShashlykDigi::DefinePSA()
{

	/* psa */
	//Double_t sample_rate = fDigiPar->GetSampleRate();
	Double_t cf_frac = fDigiPar->GetCFFraction();
	Int_t cf_tap = fDigiPar->GetCFTap();
	Int_t cf_n = fDigiPar->GetCFNSamples();
	const Double_t* cf_tcorr = fDigiPar->GetCFTCorr().GetArray();
	Int_t of_m = fDigiPar->GetOFM();
	Int_t of_b0 = fDigiPar->GetOFB0();
	const Double_t* of_a = fDigiPar->GetOFA().GetArray();
	const Double_t* of_b = fDigiPar->GetOFB().GetArray();
	Double_t hit_threshold = fDigiPar->GetADCHitThreshold();
    Double_t mu = fDigiPar->GetPulseshapeMu()-TMath::Log(fDigiPar->GetSampleRate());
    Double_t sigma = fDigiPar->GetPulseshapeSigma();
    BSEmcShashlykPulseshape *shape = new BSEmcShashlykPulseshape(mu, sigma);
    fPSA = new BSEmcPSAOptimalFilterAnalyser(cf_frac, cf_tap, cf_tcorr, cf_n, of_a, of_b, of_m, of_b0, shape, hit_threshold, 0., fVerbose);
}

/**
 * @brief return highgain flag for Shashlyk waveforms.
 *
 */
BSEmcDigi::eGAIN BSEmcShashlykDigi::GetGainType(BSEmcWaveform * /*t_waveform*/, Int_t /*t_hit*/) const
{

  return BSEmcDigi::eGAIN::kHIGH;
}

ClassImp(BSEmcShashlykDigi)
