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

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	Class Template for EmcSharedDigi. Inherits from EmcDigi
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Adapted for the PANDA experiment at GSI
//
// Author List:
//	Stephen J. Gowdy           University of Edinburgh
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "PndEmcSharedDigi.h"

#include <iostream>

//----------------
// Constructors --
//----------------
PndEmcSharedDigi::PndEmcSharedDigi(const PndEmcDigi &theDigi, const double w) : PndEmcDigi(theDigi), fWeight(w) {}

PndEmcSharedDigi::PndEmcSharedDigi(const PndEmcSharedDigi &theDigi) : PndEmcDigi(theDigi), fWeight(theDigi.fWeight) {}

PndEmcSharedDigi::PndEmcSharedDigi() : fWeight(0) {}
//--------------
// Destructor --
//--------------

PndEmcSharedDigi::~PndEmcSharedDigi() {}

PndEmcDigi *PndEmcSharedDigi::clone() const
{
  return new PndEmcSharedDigi(*this);
}

//-------------
// Operators --
//-------------
void PndEmcSharedDigi::weight(double newWeight)
{
  fWeight = newWeight;
}

//-------------
// Selectors --
//-------------

PndEmcSharedDigi *PndEmcSharedDigi::dynamic_cast_PndEmcSharedDigi()
{
  return this;
}

const PndEmcSharedDigi *PndEmcSharedDigi::dynamic_cast_PndEmcSharedDigi() const
{
  return this;
}

ClassImp(PndEmcSharedDigi)
