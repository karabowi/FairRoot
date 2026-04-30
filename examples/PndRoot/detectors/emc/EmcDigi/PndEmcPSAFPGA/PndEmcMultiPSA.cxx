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

#include "PndEmcMultiPSA.h"
#include "PndEmcWaveform.h"
#include "PndEmcMultiWaveform.h"
#include "PndEmcPSACombinator.h"


PndEmcMultiPSA::PndEmcMultiPSA(PndEmcPSACombinator* combinator) {
	fCombinator = combinator;
}

void PndEmcMultiPSA::AddPSA(PndEmcAbsPSA* psa) {
	fPSA.push_back(psa);
	fNhit.push_back(0);
}

PndEmcMultiPSA::~PndEmcMultiPSA() {
	fPSA.clear();
	fNhit.clear();
}

void PndEmcMultiPSA::Reset() {
	std::vector<PndEmcAbsPSA*>::iterator it;
	for (it = fPSA.begin(); it != fPSA.end(); it++) {
		(*it)->Reset();
	}
	fEnergyList.clear();
	fTimeList.clear();
}


Int_t PndEmcMultiPSA::Process(const PndEmcWaveform* waveform) {

	Reset();

	PndEmcMultiWaveform* multiWf = const_cast<PndEmcMultiWaveform*>(dynamic_cast<const PndEmcMultiWaveform*>(waveform));
	if(!multiWf) {
		std::cerr << "passed waveform is not of type PndEmcMultiwaveform" << std::endl;
		return -1;
	}

	UInt_t npsa = fPSA.size();
	if (npsa > (UInt_t)multiWf->GetNumberOfWaveforms()) {
		std::cerr << "number of psa is greater than number of waveforms" << std::endl;
		return -1;
	}

	for (UInt_t i = 0; i < npsa; i++) {
		multiWf->SetActiveWaveform(i);
		fNhit[i] = fPSA[i]->Process(multiWf);
	}

	fAPDGainIndex = fCombinator->Combine(fPSA, fNhit, fEnergyList, fTimeList);
	return (Int_t)fEnergyList.size();
}


void PndEmcMultiPSA::GetHit(Int_t i, Double_t& energy, Double_t& time) {
	energy = fEnergyList[i];
	time = fTimeList[i];
}


