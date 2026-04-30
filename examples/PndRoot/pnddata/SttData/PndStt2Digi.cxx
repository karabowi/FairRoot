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
 
#include "PndStt2Digi.h"

PndStt2Digi::PndStt2Digi() : FairTimeStamp(-1.0), fMcIndex(-1), fDetID(-1), fTubeID(-1), fEloss(-1.0), fDriftTime(-1.0), fHitTime(-1.0), fDist2Wire(-1.0) {}

PndStt2Digi::PndStt2Digi(Int_t mcindex, Int_t detID, Int_t tubeID, Double_t eloss, Double_t driftTime, Double_t hittime, Double_t dist2wire, Double_t timestamp)
  : FairTimeStamp(timestamp), fMcIndex(mcindex), fDetID(detID), fTubeID(tubeID), fEloss(eloss), fDriftTime(driftTime), fHitTime(hittime), fDist2Wire(dist2wire)
{
}

ClassImp(PndStt2Digi);
