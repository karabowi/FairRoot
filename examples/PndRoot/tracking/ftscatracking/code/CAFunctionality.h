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

//-*- Mode: C++ -*-
// *****************************************************************************
//                                                                             *
// @Autors: I.Kulakov; M.Pugach; M.Zyzak; I.Kisel                                        *
// @e-mail: I.Kulakov@gsi.de; M.Pugach@gsi.de; M.Zyzak@gsi.de; I.Kisel@compeng.uni-frankfurt.de *
//                                                                             *
// *****************************************************************************

#ifndef CAFunctionality_H
#define CAFunctionality_H

#include "PndFTSCATrackParamVector.h"
#include "PndFTSCAParam.h"

// Functions and parameters of CA algorithm to be called from any place
namespace CAFunctionality {
float_m FitIteration(const PndFTSCAParam &caParam, const FTSCAHits &hits, PndFTSCATrackParamVector &param, const vector<TESV> &iHits, const FTSCATarget &target, bool dir,
                     bool useParam, const float_m &active = float_m(true));

float_m Fit(const PndFTSCAParam &caParam, const FTSCAHits &hits, PndFTSCATrackParamVector &param, const vector<TESV> &iHits, const FTSCATarget &target, bool dir,
            const float_m &active = float_m(true));

float_m FitUseParam(const PndFTSCAParam &caParam, const FTSCAHits &hits, PndFTSCATrackParamVector &param, const vector<TESV> &iHits, const FTSCATarget &target, bool dir,
                    bool usePar = false, const float_m &mask = float_m(true));
} // namespace CAFunctionality

#endif
