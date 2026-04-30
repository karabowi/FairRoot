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
// Description:
//	Class PndEmcAbsClusterProperty
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Stephen J. Gowdy	Originator
//	Phil Strother   	Originator
//
// Copyright Information:
//	Copyright (C) 1998	University of Edinburgh
//
//------------------------------------------------------------------------

#include "PndEmcAbsClusterProperty.h"
#include "PndEmcCluster.h"

//----------------
// Constructors --
//----------------
PndEmcAbsClusterProperty::PndEmcAbsClusterProperty(const PndEmcCluster &toUse, const TClonesArray *digiArray)
  : fMyCluster(toUse), fMembers(toUse.MemberDigiMap()), fDigiArray(digiArray)
{
}

//--------------
// Destructor --
//--------------
PndEmcAbsClusterProperty::~PndEmcAbsClusterProperty() {}

const PndEmcCluster &PndEmcAbsClusterProperty::MyCluster() const
{
  return fMyCluster;
}

const std::map<Int_t, Int_t> &PndEmcAbsClusterProperty::Members() const
{
  return fMembers;
}

const TClonesArray *PndEmcAbsClusterProperty::DigiArray() const
{
  return fDigiArray;
}
