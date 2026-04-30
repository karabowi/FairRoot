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
 * PndMvdRadDamHit.cxx
 *
 *  Created on: Dec 16, 2008
 *      Author: stockman
 */

#include "PndMvdRadDamHit.h"

PndMvdRadDamHit::PndMvdRadDamHit() : fTrackID(0), fHitID(0), fDetId(0), fPdgCode(0), fEnergy(0.), fMom(0., 0., 0.), fRadDam(0.) {}

PndMvdRadDamHit::PndMvdRadDamHit(Int_t trackID, Int_t hitID, Int_t detId, Int_t pdgCode, Double_t energy, TVector3 pos, TVector3 mom, Double_t radDam)
  : fTrackID(trackID), fHitID(hitID), fDetId(detId), fPdgCode(pdgCode), fEnergy(energy), fMom(mom), fRadDam(radDam)
{
  SetPosition(pos);
}

ClassImp(PndMvdRadDamHit);
