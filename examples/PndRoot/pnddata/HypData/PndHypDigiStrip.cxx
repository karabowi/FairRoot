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

#include "PndHypDigiStrip.h"
#include "PndHypDigi.h"

PndHypDigiStrip::PndHypDigiStrip() : fChannel(-1)
{
  // fIndex = -1;
}

PndHypDigiStrip::PndHypDigiStrip(Int_t index, Int_t detID, TString detName, Int_t fe, Int_t chan, Double_t charge) : PndHypDigi(index, detID, detName, fe, charge), fChannel(chan)
{
}

// const void PndHypDigiStrip::print(){
// //      std::cout << *((PndHypStrip*)this) << std::endl;
// }

// Bool_t const HasNeighbour(const PndHypDigiStrip& d2)
// 	{
//  	  return true; // Dummy so far
//  	}

Bool_t PndHypDigiStrip::operator==(const PndHypDigiStrip &d2) const
{
  return ( // fIndex==d2.GetIndex()
    fDetID == d2.GetDetID() && fDetName == d2.GetDetName() && fFE == d2.GetFE() && fChannel == d2.GetChannel() && fCharge == d2.GetCharge()
    //  && fMCID==d2.GetMCID()
  );
}

ClassImp(PndHypDigiStrip)
