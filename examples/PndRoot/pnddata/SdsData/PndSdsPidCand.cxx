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

//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndSdsPidCand
//      see PndSdsPidCand.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM        (original author)
//	Tobias Baldauf	     TUD	(adapted for SSD Pid)
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndSdsPidCand.h"
#include "TObject.h"
#include "TROOT.h"
#include <iostream>
#include "TString.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <vector>
#include "TMath.h"

// Class Member definitions -----------

ClassImp(PndSdsPidCand);

void PndSdsPidCand::SetLikelihood(int lundId, double likelihood)
{
  this->flikelihood[lundId] = likelihood;
}

double PndSdsPidCand::GetLikelihood(int lundId)
{
  return flikelihood[lundId];
}

double PndSdsPidCand::GetSsdHitdE(int ssdhit) const
{
  return fdE[ssdhit];
}

double PndSdsPidCand::GetSsdHitdx(int ssdhit) const
{
  return fdx[ssdhit];
}

double PndSdsPidCand::GetSsdHitMomentum(int ssdhit) const
{
  return fmomentum[ssdhit];
}

int PndSdsPidCand::GetSsdHits() const
{
  return fssdhits;
}

void PndSdsPidCand::AddSsdHit(double dE, double dx, double p)
{
  fmomentum.push_back(p);
  fdE.push_back(dE);
  fdx.push_back(dx);
  fssdhits++;
}
