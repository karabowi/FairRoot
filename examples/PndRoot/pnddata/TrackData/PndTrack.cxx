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
 * PndTrack.cpp
 *
 *  Created on: 05.03.2009
 *      Author: stockman
 */

#include "PndTrack.h"

#include <FairRootManager.h>

ClassImp(PndTrack);

PndTrack::PndTrack() : fTrackParamFirst(), fTrackParamLast(), fTrackCand(), fRefToPndTrackCand(), fPidHypo(0), fFlag(0), fChi2(-1.), fNDF(0), fRefIndex(-1) {}

PndTrack::PndTrack(const FairTrackParP &first, const FairTrackParP &last, const PndTrackCand &cand, Int_t flag, Double_t chi2, Int_t ndf, Int_t pid, Int_t id, Int_t type)
  : fTrackParamFirst(first), fTrackParamLast(last), fTrackCand(cand), fRefToPndTrackCand(), fPidHypo(pid), fFlag(flag), fChi2(chi2), fNDF(ndf), fRefIndex(id)
{
  if (!(type < 0))
    SetLink(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), type, id));
  if (cand.GetPointerToLinks() != 0)
    AddLinks(*cand.GetPointerToLinks());
  SetTimeStamp(cand.GetTimeStamp());
  SetTimeStampError(cand.GetTimeStampError());
}

void PndTrack::Print()
{
  std::cout << "FirstTrackPar" << std::endl;
  fTrackParamFirst.Print();
  std::cout << "LastTrackPar" << std::endl;
  fTrackParamLast.Print();
  std::cout << "chi2 " << fChi2 << std::endl;
  std::cout << "ndf " << fNDF << std::endl;
  std::cout << "fRefIndex " << fRefIndex << std::endl;
}
