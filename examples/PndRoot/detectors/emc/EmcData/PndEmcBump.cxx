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
//	Class PndEmcBump
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Author List:
//	Stephen J. Gowdy	Originator
// Copyright Information:
//	Copyright (C) 1997	University of Edinburgh
//
// Dima Melnychuk, adaption for PANDA
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------

#include "PndEmcBump.h"

//---------------
// C++ Headers --
//---------------

#include <iostream>
#include <iomanip>
#include <vector>
using std::vector;

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

#include "PndEmcSharedDigi.h"
using std::endl;
using std::ios;
using std::ostream;
using std::setw;

//----------------
// Constructors --
//----------------

PndEmcBump::PndEmcBump() : fClusterIndex(0)
{
  SetNBumps(1);
}

// use compiler-generated copy constructor

//--------------
// Destructor --
//--------------

PndEmcBump::~PndEmcBump() {}

void PndEmcBump::Print(Int_t iBump) const
{
  std::cout << "Bump[" << iBump << "], (digis, E)=(" << DigiList().size() << ", " << GetEnergy() << ")" << endl;
}
//-------------
// Modifiers --
//-------------

void PndEmcBump::MadeFrom(Int_t clusterIndex)
{
  fClusterIndex = clusterIndex;
}

// Double_t
// PndEmcBump::RnumberOfDigis() const
// {
// 	Double_t sum = 0;
// 	PndEmcDigi* current;
//
// 	std::vector<PndEmcDigi*>::const_iterator digi_iter;
// 	for (digi_iter=fDigiList.begin();digi_iter!=fDigiList.end();++digi_iter)
// 	{
// 		sum+=(*digi_iter)->dynamic_cast_PndEmcSharedDigi()->weight();
// 	}
//
// 	return sum;
// }
//
// Int_t
// PndEmcBump::NumberOfDigis() const
// {
// 	Int_t numberOfDigis = Int_t( RnumberOfDigis() + 0.5 );
// 	if( numberOfDigis < 1 ) numberOfDigis = 1;
//
// 	return( numberOfDigis );
// }
