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
 * PndSimpleTrack.cpp
 *
 *  Created on: 05.03.2009
 *      Author: stockman
 */

#include "PndSimpleTrack.h"

ClassImp(PndSimpleTrack);

PndSimpleTrack::PndSimpleTrack() : fTrackParam(), fTrackCand(), fChi2(0) {}

PndSimpleTrack::PndSimpleTrack(const FairTrackPar &param, const PndTrackCand &cand, const double &chi2) : fTrackParam(param), fTrackCand(cand), fChi2(chi2)
{

  SetTimeStamp(cand.GetTimeStamp());
  SetTimeStampError(cand.GetTimeStampError());
}

void PndSimpleTrack::Print()
{
  std::cout << "TrackParam" << std::endl;
  fTrackParam.Print();
}
