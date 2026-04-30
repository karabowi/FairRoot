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

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndPidBremCorrected4Mom	                                        //
//                                                                      //
// Container for Bremstrahlung radiaton corrected momentum              //
//                                                                      //
// Author: Klaus Goetzen, GSI, 12.06.08		                        //
// Copyright (C) 2008, GSI Darmstadt.		         		//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "PndPidBremCorrected4Mom.h"

PndPidBremCorrected4Mom::PndPidBremCorrected4Mom() : fXmomentum(0.), fYmomentum(0.), fZmomentum(0.), fEnergy(0.), fPidCandIdx(-1), fPhiBumpList(), fSepBumpList()
{
  fPhiBumpList.clear();
  fSepBumpList.clear();
}

PndPidBremCorrected4Mom::PndPidBremCorrected4Mom(TLorentzVector &p4)
  : fXmomentum(p4.X()), fYmomentum(p4.Y()), fZmomentum(p4.Z()), fEnergy(p4.E()), fPidCandIdx(-1), fPhiBumpList(), fSepBumpList()
{
  fPhiBumpList.clear();
  fSepBumpList.clear();
}

PndPidBremCorrected4Mom::~PndPidBremCorrected4Mom() {}

ClassImp(PndPidBremCorrected4Mom)
