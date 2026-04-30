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

/*
 * PndBranchEnum.h
 *
 *  Created on: 06.07.2023
 *      Author: tstockmanns
 */

#pragma once

#include <TString.h>

namespace TrackingQA {
enum class branchEnum { MVDHitsPixel, MVDHitsStrip, GEMHit, FTSHit, STTHit, STTHitParal, STTHitSkew, Undefined };

branchEnum StringToBranchEnum(TString branchString);
TString BranchEnumToString(branchEnum branch);

} // namespace TrackingQA
