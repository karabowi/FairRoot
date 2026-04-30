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

// -------------------------------------------------------------------------
// -----                    PndHypHit source file                -----
// -----                Modified for hyp purpose by A. Sanchez   -----
// -------------------------------------------------------------------------

#include "stdlib.h"
#include <iostream>
#include <algorithm>
#include "PndHypHit.h"
#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TGeoVolume.h"

// -----   Default constructor   -------------------------------------------
PndHypHit::PndHypHit() : fDetName(""), fCharge(0.), fNDigiHits(0), fBotIndex(-1) {}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndHypHit::PndHypHit(Int_t detID, TString detName, TVector3 &pos, TVector3 &dpos, Int_t index, Double_t charge, Int_t NDigiHits)
  : FairHit(detID, pos, dpos, index), fDetName(detName), fCharge(charge), fNDigiHits(NDigiHits), fBotIndex(-1)
{
}

// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndHypHit::~PndHypHit() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndHypHit::Print(const Option_t *opt) const
{
  std::cout << *this << " opt=" << opt << std::endl;
}

// -------------------------------------------------------------------------
ClassImp(PndHypHit)
