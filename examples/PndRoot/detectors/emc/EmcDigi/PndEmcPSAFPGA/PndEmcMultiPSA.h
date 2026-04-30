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
//     	Wrapping class to manage multiple independent PSAs 
//      Guang Zhao
//-----------------------------------------------------------


#ifndef PNDEMCMULTIPSA_H_
#define PNDEMCMULTIPSA_H_

#include <vector>

#include "PndEmcAbsPSA.h"

class PndEmcPSACombinator;
class PndEmcWaveform;

/**
 * @brief Wrapping class to manage multiple independent PSAs
 * 
 * @author Guang Zhao (zhaog@ihep.ac.cn)
 * @ingroup PndEmc
 */
class PndEmcMultiPSA : public PndEmcAbsPSA 
{
	public:
		PndEmcMultiPSA(PndEmcPSACombinator*);
		void AddPSA(PndEmcAbsPSA*);

		virtual ~PndEmcMultiPSA();

		virtual Int_t Process(const PndEmcWaveform* waveform);
		virtual void Reset();
		virtual void GetHit(Int_t i, Double_t &energy, Double_t &time);
		Int_t GetAPDGainIndex() { return fAPDGainIndex; }

	private:
		std::vector<PndEmcAbsPSA*> fPSA;
		std::vector<Int_t> fNhit;
		PndEmcPSACombinator* fCombinator; // combinator to combine multi-PSA outputs to a signle output
		Int_t fAPDGainIndex; // 0 for hg, 1 for lg
		std::vector<Double_t> fEnergyList;
		std::vector<Double_t> fTimeList;
		
		ClassDef(PndEmcMultiPSA, 1)
};

#endif
