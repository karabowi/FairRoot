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

/////////////////////////////////////////////////////////////
//
//  PndEmcHeader
//
//  Header of Emc informations
//
//  Created 21/04/07  by S.Spataro
//
///////////////////////////////////////////////////////////////

#include "PndEmcHeader.h"
#include <iostream>

using namespace std;

// -----   Default constructor   -------------------------------------------
PndEmcHeader::PndEmcHeader() : fPx(0), fPy(0), fPz(0), fHitEnergy(0), fCluEnergy(0), nHitMult(0), nDigiMult(0), nCluMult(0) {}
// -------------------------------------------------------------------------

// -----   Default constructor   -------------------------------------------
PndEmcHeader::PndEmcHeader(Double32_t ene_hit, Int_t mult_hit, Int_t mult_digi, Double32_t ene_clu, Int_t mult_clu, TVector3 p_clu)
  : fPx(p_clu.X()), fPy(p_clu.Y()), fPz(p_clu.Z()), fHitEnergy(ene_hit), fCluEnergy(ene_clu), nHitMult(mult_hit), nDigiMult(mult_digi), nCluMult(mult_clu)
{
}
// -------------------------------------------------------------------------

// Copy
PndEmcHeader::PndEmcHeader(const PndEmcHeader &copy)
  : TObject(copy), fPx(copy.fPx), fPy(copy.fPy), fPz(copy.fPz), fHitEnergy(copy.fHitEnergy), fCluEnergy(copy.fCluEnergy), nHitMult(copy.nHitMult), nDigiMult(copy.nDigiMult),
    nCluMult(copy.nCluMult)
{
}
// -----   Destructor   ----------------------------------------------------
PndEmcHeader::~PndEmcHeader() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndEmcHeader::Print(const Option_t *) const
{
  cout << "EMC header: fired crystals= " << GetHitMult() << ", digi= " << GetDigiMult() << ", Total energy= " << GetHitEnergy()
       << " [GeV], Reconstructed clusters= " << GetCluMult() << ", Total energy in clusters= " << GetCluEnergy() << " [GeV]" << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndEmcHeader)
