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
#ifndef PNDEMCBWENDCAPPULSEAMPLIFIER_H_
#define PNDEMCBWENDCAPPULSEAMPLIFIER_H_

#include "PndEmcAbsWaveformModifier.h"

class PndEmcWaveform;
class PndEmcAbsWaveformSimulator;

/**
 * @brief waveform modifier to amplify the amplitude
 * @author Guang Zhao (zhaog@ihep.ac.cn)
 * @ingroup PndEmc
 */
class PndEmcBWEndcapPulseAmplifier : public PndEmcAbsWaveformModifier 
{
    public:
        PndEmcBWEndcapPulseAmplifier();
        PndEmcBWEndcapPulseAmplifier(Double_t gain, Double_t inputScale);

		virtual Double_t GetScale() { return fScale/fInputScale; }
        virtual void  Modify(PndEmcWaveform* wf);

    protected:

    private:
		Double_t fScale;
		Double_t fInputScale;

        ClassDef(PndEmcBWEndcapPulseAmplifier, 1)
};

#endif
