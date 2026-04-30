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
//      Implementation of class PndMvdPidCand
//      see PndMvdPidCand.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM        (original author)
//	Tobias Baldauf	     TUD	(adapted for MVD Pid)
//	Alicia Sanchez	     UniMainz   (adapted for HYP Pid)
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndHypPidCand.h"
#include "TObject.h"
#include "TROOT.h"
#include <iostream>
#include "TString.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <vector>
#include "TMath.h"

// Class Member definitions -----------

ClassImp(PndHypPidCand);

void PndHypPidCand::SetLikelihood(int lundId, double likelihood)
{
  this->flikelihood[lundId] = likelihood;
}

double PndHypPidCand::GetLikelihood(int lundId)
{
  return flikelihood[lundId];
}

double PndHypPidCand::GetHypHitdE(int hyphit) const
{
  return fdE[hyphit];
}

double PndHypPidCand::GetHypHitdx(int hyphit) const
{
  return fdx[hyphit];
}

double PndHypPidCand::GetHypHitMomentum(int hyphit) const
{
  return fmomentum[hyphit];
}

int PndHypPidCand::GetHypHits() const
{
  return fhyphits;
}

void PndHypPidCand::AddHypHit(double dE, double dx, double p)
{
  fmomentum.push_back(p);
  fdE.push_back(dE);
  fdx.push_back(dx);
  fhyphits++;
}
