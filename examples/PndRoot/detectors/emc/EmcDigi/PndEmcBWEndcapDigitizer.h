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
#ifndef PNDEMCBWENDCAPDIGITIZER_H_
#define PNDEMCBWENDCAPDIGITIZER_H_

#include "PndEmcAbsWaveformModifier.h"

/**
 * @brief waveform digitzer
 * @author Guang Zhao (zhaog@ihep.ac.cn)
 * @ingroup PndEmc
 */
class PndEmcBWEndcapDigitizer : public PndEmcAbsWaveformModifier 
{
	public:
		PndEmcBWEndcapDigitizer();	
		PndEmcBWEndcapDigitizer(Double_t inputScale, Double_t nbits);

		virtual void  Modify(PndEmcWaveform* wf);
		virtual Double_t GetScale() { return fInputScale; }

	private:

		Double_t fInputScale;
		Double_t fNbits;

		ClassDef(PndEmcBWEndcapDigitizer, 1)
};

#endif
