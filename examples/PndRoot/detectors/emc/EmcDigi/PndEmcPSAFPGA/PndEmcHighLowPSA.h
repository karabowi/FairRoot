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
//     	Wrapping class to manage two independent PSAs 
//     	for high, resp. low gain 
//      Philipp Mahlberg
//-----------------------------------------------------------


#ifndef PNDEMCHIGHLOWPSA_H_
#define PNDEMCHIGHLOWPSA_H_


#include "PndEmcAbsPSA.h"

class PndEmcPSAFPGASampleAnalyser;


/**
 * @brief Wrapping class to manage two independent PSAs for high, resp. low gain
 * 
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup PndEmc
 */
class PndEmcHighLowPSA : public PndEmcAbsPSA 
{
	public:
		PndEmcHighLowPSA(Int_t verbose=0);
		virtual void Init(PndEmcPSAFPGASampleAnalyser* highgainPSA, PndEmcPSAFPGASampleAnalyser* lowgainPSA, Double_t overflowThreshold, Int_t highgainWfIndex=0, Int_t lowgainWfIndex=1);
		virtual ~PndEmcHighLowPSA(){}

		virtual void SetVerbose(Int_t verbose=0) { fVerbose = verbose; };
		virtual Int_t Process(const PndEmcWaveform* waveform);
		virtual void Reset();
		virtual void GetHit(Int_t i, Double_t &energy, Double_t &time);
		virtual void GetHit2(Int_t i, Double_t &energy, Double_t &time, Int_t &PileupType);
		virtual Int_t GetWaveformIdx(Int_t i);

	private:
		PndEmcPSAFPGASampleAnalyser* fHighgainPSA;
		PndEmcPSAFPGASampleAnalyser* fLowgainPSA;
		
		Int_t fIdx_high;
		Int_t fIdx_low;

		std::vector< std::pair<PndEmcPSAFPGASampleAnalyser*, int> >fHitsInFE;
		Double_t fOverflowThreshold;

		Int_t fVerbose;	

		ClassDef(PndEmcHighLowPSA, 1)
};

#endif
